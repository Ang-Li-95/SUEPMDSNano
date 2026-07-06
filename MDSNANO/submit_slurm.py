#!/usr/bin/env python3
"""
Submit Slurm jobs that run RunIII2024MC.py (cmsRun) over a list of ROOT files.

The input file list is built by scanning a directory for ROOT files (local, or
remote EOS via an xrootd redirector), the files are split into chunks of N per
job, and every job stages its NanoAOD output out to a destination that can be
either a local path or an xrootd/EOS URL.

Example
-------
  # local input, local output
  ./submit_slurm.py -i /path/to/AODSIM -n 5 -o /path/to/output/nano

  # local input, EOS output via xrootd
  ./submit_slurm.py -i /path/to/AODSIM -n 5 \\
      -o root://eosuser.cern.ch//eos/user/a/ang/nano --job-name myprod

  # remote EOS input via xrootd, plus EOS output
  ./submit_slurm.py -i root://eoscms.cern.ch//store/user/lian/.../AODSIM -n 5 \\
      -o root://eosuser.cern.ch//eos/user/a/ang/nano
  # ...or a bare path with an explicit redirector:
  ./submit_slurm.py -i /store/user/lian/.../AODSIM --redirector root://eoscms.cern.ch/ ...

Each job runs:
  cmsRun RunIII2024MC.py fileList=<chunk.txt> outputFile=<name>.root maxEvents=-1
inside the matching CMSSW container (cmssw-el8 by default for this release).

This script needs no CMSSW environment and must run where 'sbatch' exists (the
host) -- not inside the cmssw container. Only the jobs enter the container.
"""

import argparse
import fnmatch
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

HERE = os.path.dirname(os.path.abspath(__file__))


# --------------------------------------------------------------------------- #
# Environment auto-detection helpers
# --------------------------------------------------------------------------- #
def find_cmssw_base(config_path, override):
    """Locate CMSSW_BASE from an override, the environment, or the config path."""
    if override:
        return os.path.abspath(override)
    if os.environ.get("CMSSW_BASE"):
        return os.environ["CMSSW_BASE"]
    d = os.path.dirname(os.path.abspath(config_path))
    while d not in ("/", ""):
        if os.path.basename(d).startswith("CMSSW_") and os.path.isdir(os.path.join(d, "src")):
            return d
        d = os.path.dirname(d)
    return None


def find_scram_arch(cmssw_base, override):
    """Read the SCRAM architecture of the release from <CMSSW_BASE>/.SCRAM/.

    The release directory is authoritative: the SCRAM_ARCH environment variable
    may be the host default (e.g. slc7) and not match how the release was built
    (e.g. el8), which would pick the wrong container.
    """
    if override:
        return override
    scram_dir = os.path.join(cmssw_base, ".SCRAM")
    if os.path.isdir(scram_dir):
        for name in sorted(os.listdir(scram_dir)):
            if "_amd64_" in name or "_aarch64_" in name:
                return name
    if os.environ.get("SCRAM_ARCH") and "_amd64_" in os.environ["SCRAM_ARCH"]:
        return os.environ["SCRAM_ARCH"]
    return None


def default_container(scram_arch):
    """Pick the cms container wrapper that matches the release OS (el8/el9)."""
    osprefix = scram_arch.split("_")[0] if scram_arch else ""
    if osprefix in ("el8", "el9"):
        return "cmssw-" + osprefix
    return ""  # slc7/cc7: assume the worker matches; user can override


# --------------------------------------------------------------------------- #
# Output destination parsing (local path or root://host//path)
# --------------------------------------------------------------------------- #
def parse_output(dest):
    dest = dest.rstrip("/")
    if "://" in dest:
        m = re.match(r"^(\w+)://([^/]+)/+(.*)$", dest)
        if not m:
            sys.exit("ERROR: could not parse xrootd destination: %s" % dest)
        return {"is_xrootd": True, "host": m.group(2), "dir": "/" + m.group(3)}
    return {"is_xrootd": False, "host": "", "dir": os.path.abspath(dest)}


# --------------------------------------------------------------------------- #
# Input file discovery (local directory, or remote EOS via xrootd redirector)
# --------------------------------------------------------------------------- #
def list_local(in_dir, pattern, recursive):
    """Find local ROOT files and return them as 'file:' PFNs."""
    p = Path(in_dir)
    it = p.rglob(pattern) if recursive else p.glob(pattern)
    return sorted("file:" + os.path.abspath(str(f)) for f in it if f.is_file())


def list_xrootd(host, base_path, pattern, recursive):
    """List ROOT files under an xrootd directory via 'xrdfs ls', return URLs.

    Note: the host must support directory listing, so point a redirector at the
    storage endpoint (e.g. root://eoscms.cern.ch/, root://eosuser.cern.ch/)
    rather than a read-only global redirector.
    """
    cmd = ["xrdfs", host, "ls"] + (["-R"] if recursive else []) + [base_path]
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         universal_newlines=True)
    if res.returncode != 0:
        sys.exit("ERROR: 'xrdfs %s ls %s' failed:\n%s"
                 % (host, base_path, res.stderr.strip()))
    files = []
    for line in res.stdout.splitlines():
        line = line.strip()
        if not line or not fnmatch.fnmatch(os.path.basename(line), pattern):
            continue
        path = line if line.startswith("/") else "/" + line
        files.append("root://%s/%s" % (host, path))  # host + /path => host//path
    return sorted(files)


def redirector_host(redirector):
    """Reduce 'root://cms-xrd-global.cern.ch/' (or similar) to just the host."""
    r = redirector.strip()
    if "://" in r:
        r = r.split("://", 1)[1]
    return r.split("/")[0]


def in_container():
    """True when running inside the cms apptainer/singularity container."""
    return bool(os.environ.get("APPTAINER_CONTAINER")
                or os.environ.get("SINGULARITY_CONTAINER")
                or os.path.exists("/.singularity.d"))


# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    ap.add_argument("-i", "--input-dir", required=True,
                    help="Directory scanned for input ROOT files. Either a local "
                         "path, or an xrootd URL root://host//path, or a bare "
                         "remote path combined with --redirector.")
    ap.add_argument("--redirector", default=None,
                    help="xrootd redirector/endpoint for a remote --input-dir "
                         "given as a bare path, e.g. --redirector root://eoscms.cern.ch/ "
                         "-i /store/user/.../AODSIM. Must support directory listing.")
    ap.add_argument("-n", "--files-per-job", required=True, type=int,
                    help="Number of input files processed per Slurm job.")
    ap.add_argument("-o", "--output", required=True,
                    help="Output destination directory. Local path or "
                         "xrootd URL, e.g. root://eosuser.cern.ch//eos/user/a/ang/out")
    ap.add_argument("-c", "--config", default=os.path.join(HERE, "RunIII2024MC.py"),
                    help="cmsRun configuration (default: %(default)s).")
    ap.add_argument("--max-events", default=-1, type=int,
                    help="maxEvents per job (default: -1 = all).")
    ap.add_argument("--job-name", default="nano",
                    help="Base name for the Slurm jobs and output files (default: %(default)s).")
    ap.add_argument("--work-dir", default=None,
                    help="Where job scripts, file lists and logs are written "
                         "(default: ./slurm_<job-name>).")
    ap.add_argument("--pattern", default="*.root",
                    help="Glob pattern for input files (default: %(default)s).")
    ap.add_argument("--no-recursive", action="store_true",
                    help="Only scan the top level of --input-dir (default: recurse).")

    # Slurm resources
    ap.add_argument("--time", default="08:00:00", help="Slurm wall time (default: %(default)s).")
    ap.add_argument("--mem", default="4000", help="Memory per job in MB (default: %(default)s).")
    ap.add_argument("--cpus", default=1, type=int, help="CPUs per task (default: %(default)s).")
    ap.add_argument("--partition", default=None, help="Slurm partition (optional).")
    ap.add_argument("--account", default=None, help="Slurm account (optional).")
    ap.add_argument("--max-concurrent", default=None, type=int,
                    help="Cap simultaneously running array tasks (Slurm '%%N').")
    ap.add_argument("--extra-sbatch", action="append", default=[],
                    help="Extra '#SBATCH' directive, e.g. --extra-sbatch '--qos=long'. Repeatable.")

    # CMSSW environment
    ap.add_argument("--cmssw-base", default=None, help="CMSSW_BASE (default: auto-detect).")
    ap.add_argument("--scram-arch", default=None, help="SCRAM_ARCH (default: auto-detect).")
    ap.add_argument("--container", default=None,
                    help="cms container wrapper to run cmsRun in, e.g. cmssw-el8. "
                         "Use 'none' to run directly (default: auto from SCRAM_ARCH).")

    ap.add_argument("--dry-run", action="store_true",
                    help="Generate everything but do not call sbatch.")
    args = ap.parse_args()

    # ---- validate config & CMSSW environment ----
    config = os.path.abspath(args.config)
    if not os.path.isfile(config):
        sys.exit("ERROR: config not found: %s" % config)

    cmssw_base = find_cmssw_base(config, args.cmssw_base)
    if not cmssw_base or not os.path.isdir(os.path.join(cmssw_base, "src")):
        sys.exit("ERROR: could not determine CMSSW_BASE. Pass --cmssw-base or run cmsenv.")
    scram_arch = find_scram_arch(cmssw_base, args.scram_arch)
    if not scram_arch:
        sys.exit("ERROR: could not determine SCRAM_ARCH. Pass --scram-arch.")
    container = args.container if args.container is not None else default_container(scram_arch)
    container = "" if container in ("", "none", "None") else container

    # ---- discover input files (local dir, xrootd URL, or path + redirector) ----
    if args.files_per_job < 1:
        sys.exit("ERROR: --files-per-job must be >= 1")
    raw_in = args.input_dir.rstrip("/")
    recursive = not args.no_recursive
    if raw_in.startswith("root://"):
        m = re.match(r"^root://([^/]+)/+(.*)$", raw_in)
        if not m:
            sys.exit("ERROR: could not parse xrootd input dir: %s" % args.input_dir)
        in_host, in_path = m.group(1), "/" + m.group(2)
        in_desc = "%s  [xrootd]" % args.input_dir
        files = list_xrootd(in_host, in_path, args.pattern, recursive)
    elif args.redirector:
        in_host = redirector_host(args.redirector)
        in_path = (raw_in if raw_in.startswith("/") else "/" + raw_in)
        in_desc = "root://%s/%s  [xrootd via redirector]" % (in_host, in_path)
        files = list_xrootd(in_host, in_path, args.pattern, recursive)
    else:
        in_dir = os.path.abspath(raw_in)
        if not os.path.isdir(in_dir):
            sys.exit("ERROR: input directory not found: %s" % in_dir)
        in_desc = "%s  [local]" % in_dir
        files = list_local(in_dir, args.pattern, recursive)
    if not files:
        sys.exit("ERROR: no files matching '%s' under %s" % (args.pattern, args.input_dir))

    # ---- split into chunks ----
    n = args.files_per_job
    chunks = [files[k:k + n] for k in range(0, len(files), n)]
    njobs = len(chunks)

    # ---- prepare work area ----
    work_dir = os.path.abspath(args.work_dir or os.path.join(os.getcwd(), "slurm_" + args.job_name))
    filelist_dir = os.path.join(work_dir, "filelists")
    log_dir = os.path.join(work_dir, "logs")
    os.makedirs(filelist_dir, exist_ok=True)
    os.makedirs(log_dir, exist_ok=True)
    # remove stale file lists so array indices always match the current run
    for old in Path(filelist_dir).glob("job_*.txt"):
        old.unlink()
    for i, chunk in enumerate(chunks):
        with open(os.path.join(filelist_dir, "job_%d.txt" % i), "w") as fh:
            fh.write("\n".join(chunk) + "\n")

    # ---- parse output destination ----
    out = parse_output(args.output)
    if not args.dry_run:
        if out["is_xrootd"]:
            # best-effort: pre-create the remote directory (jobs also retry this)
            subprocess.run(["xrdfs", out["host"], "mkdir", "-p", out["dir"]],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        else:
            os.makedirs(out["dir"], exist_ok=True)

    # ---- build the Slurm array script ----
    job_sh = os.path.join(work_dir, "job.sh")
    array = "0-%d" % (njobs - 1)
    if args.max_concurrent:
        array += "%%%d" % args.max_concurrent

    directives = [
        "#SBATCH --job-name=%s" % args.job_name,
        "#SBATCH --output=%s/%%x_%%A_%%a.out" % log_dir,
        "#SBATCH --error=%s/%%x_%%A_%%a.err" % log_dir,
        "#SBATCH --time=%s" % args.time,
        "#SBATCH --mem=%s" % args.mem,
        "#SBATCH --cpus-per-task=%d" % args.cpus,
        "#SBATCH --array=%s" % array,
    ]
    if args.partition:
        directives.append("#SBATCH --partition=%s" % args.partition)
    if args.account:
        directives.append("#SBATCH --account=%s" % args.account)
    for extra in args.extra_sbatch:
        directives.append("#SBATCH %s" % extra)

    injected = "\n".join([
        'CMSSW_BASE_DIR="%s"' % cmssw_base,
        'SCRAM_ARCH_USE="%s"' % scram_arch,
        'CONTAINER="%s"' % container,
        'CONFIG="%s"' % config,
        'WORKDIR="%s"' % work_dir,
        'JOBNAME="%s"' % args.job_name,
        'MAXEVENTS="%d"' % args.max_events,
        'DEST_IS_XROOTD="%d"' % (1 if out["is_xrootd"] else 0),
        'DEST_HOST="%s"' % out["host"],
        'DEST_DIR="%s"' % out["dir"],
    ])

    with open(job_sh, "w") as fh:
        fh.write("#!/bin/bash\n")
        fh.write("\n".join(directives) + "\n\n")
        fh.write("# ---- configuration injected by submit_slurm.py ----\n")
        fh.write(injected + "\n")
        fh.write(JOB_BODY)
    os.chmod(job_sh, 0o755)

    # ---- a one-line host submit helper (handy when sbatch is not reachable
    #      from where you are, e.g. inside the cmssw container) ----
    submit_sh = os.path.join(work_dir, "submit.sh")
    with open(submit_sh, "w") as fh:
        fh.write("#!/bin/bash\n")
        fh.write("# Run from a host shell (NOT inside the cmssw container).\n")
        fh.write("exec sbatch %s\n" % job_sh)
    os.chmod(submit_sh, 0o755)

    # ---- summary ----
    print("=" * 64)
    print("Input         : %s" % in_desc)
    print("Input files   : %d  (pattern '%s', %s)"
          % (len(files), args.pattern, "recursive" if recursive else "top-level"))
    print("Files per job : %d" % n)
    print("Jobs (array)  : %d  (%s)" % (njobs, array))
    print("Output dest   : %s%s"
          % (args.output, "  [xrootd]" if out["is_xrootd"] else "  [local]"))
    print("CMSSW_BASE    : %s" % cmssw_base)
    print("SCRAM_ARCH    : %s" % scram_arch)
    print("Container     : %s" % (container or "(none / run directly)"))
    print("Work dir      : %s" % work_dir)
    print("Job script    : %s" % job_sh)
    print("Submit helper : %s" % submit_sh)
    print("=" * 64)

    if args.dry_run:
        print("[dry-run] nothing submitted. To submit:\n  sbatch %s" % job_sh)
        return

    sbatch = shutil.which("sbatch")
    if not sbatch:
        if in_container():
            print("NOTE: 'sbatch' is not available inside the cmssw container.")
        else:
            print("NOTE: 'sbatch' was not found on this host.")
        print("      Everything is prepared - submit from a host shell with:")
        print("        sbatch %s" % job_sh)
        print("        (or simply:  bash %s )" % submit_sh)
        if in_container():
            print("      Tip: this script needs no cmsenv/container, so next time you can")
            print("           run it directly on the host; only the jobs use the container.")
        return

    res = subprocess.run([sbatch, job_sh])
    if res.returncode != 0:
        sys.exit("ERROR: sbatch failed (rc=%d). Submit manually with: sbatch %s"
                 % (res.returncode, job_sh))
    print("Check results later with:  %s/check_jobs.py %s" % (HERE, work_dir))


# --------------------------------------------------------------------------- #
# Body of the generated Slurm script. Raw string: $VARS expand at job runtime,
# \$(...) is written verbatim into the inner (in-container) script.
# --------------------------------------------------------------------------- #
JOB_BODY = r'''
set -eo pipefail

if [[ -z "${SLURM_ARRAY_TASK_ID:-}" ]]; then
  echo "ERROR: SLURM_ARRAY_TASK_ID unset - submit with 'sbatch' as an array job." >&2
  exit 1
fi
TASK="${SLURM_ARRAY_TASK_ID}"
FILELIST="${WORKDIR}/filelists/job_${TASK}.txt"
OUTNAME="${JOBNAME}_${TASK}.root"

if [[ ! -f "${FILELIST}" ]]; then
  echo "ERROR: input file list not found: ${FILELIST}" >&2
  exit 1
fi

echo "==> task ${TASK} on $(hostname) at $(date)"
echo "    inputs : ${FILELIST} ($(wc -l < "${FILELIST}") files)"
echo "    output : ${OUTNAME} -> ${DEST_DIR}"

# Per-job scratch directory, cleaned up on exit.
SCRATCH="${TMPDIR:-/tmp}/${JOBNAME}_${SLURM_JOB_ID:-$$}_${TASK}"
mkdir -p "${SCRATCH}"
trap 'rm -rf "${SCRATCH}"' EXIT

# Inner script: set up CMSSW and run cmsRun. Executed inside the container.
INNER="${SCRATCH}/run_cmsRun.sh"
cat > "${INNER}" <<EOF
#!/bin/bash
set -e
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd "${CMSSW_BASE_DIR}/src"
eval \$(scramv1 runtime -sh)
cd "${SCRATCH}"
cmsRun "${CONFIG}" fileList="${FILELIST}" outputFile="${OUTNAME}" maxEvents="${MAXEVENTS}"
EOF
chmod +x "${INNER}"

export SCRAM_ARCH="${SCRAM_ARCH_USE}"
if [[ -n "${CONTAINER}" && "${CONTAINER}" != "none" ]]; then
  # No bind paths needed: this cluster's apptainer uses 'mount hostfs = yes',
  # so /users, /groups, /scratch-cbe, /eos, /tmp ... are all visible inside.
  echo "==> running cmsRun inside ${CONTAINER}"
  "/cvmfs/cms.cern.ch/common/${CONTAINER}" --command-to-run bash "${INNER}"
else
  echo "==> running cmsRun directly (no container)"
  bash "${INNER}"
fi

# ---- stage out ----
OUTFILE="${SCRATCH}/${OUTNAME}"
if [[ ! -f "${OUTFILE}" ]]; then
  echo "ERROR: cmsRun finished but output missing: ${OUTFILE}" >&2
  exit 2
fi

if [[ "${DEST_IS_XROOTD}" == "1" ]]; then
  echo "==> xrdcp -> root://${DEST_HOST}/${DEST_DIR}/${OUTNAME}"
  xrdfs "${DEST_HOST}" mkdir -p "${DEST_DIR}" || true
  # --posc: commit the file only on a fully successful transfer, so an
  # interrupted copy leaves nothing -- "present" always means "complete".
  xrdcp -f --posc "${OUTFILE}" "root://${DEST_HOST}/${DEST_DIR}/${OUTNAME}"
else
  echo "==> cp -> ${DEST_DIR}/${OUTNAME}"
  mkdir -p "${DEST_DIR}"
  # copy to a temp on the destination filesystem, then atomically rename in,
  # so the final name only ever appears once the copy is complete.
  STAGE_TMP="${DEST_DIR}/.${OUTNAME}.part.${SLURM_JOB_ID:-$$}.${TASK}"
  cp -f "${OUTFILE}" "${STAGE_TMP}"
  mv -f "${STAGE_TMP}" "${DEST_DIR}/${OUTNAME}"
fi

echo "==> task ${TASK} done at $(date)"
'''


if __name__ == "__main__":
    main()



