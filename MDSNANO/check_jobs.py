#!/usr/bin/env python3
"""
Check whether every array job from a submit_slurm.py work dir produced its
output -- purely by looking at the destination.

  ./check_jobs.py <work-dir> [--resubmit] [-v]

Each job runs cmsRun under 'set -e' and stages its output out only as the very
last step, so an output file present (and non-empty) at the destination means
that job succeeded end-to-end. This script therefore just checks, for every
filelists/job_<i>.txt, that <jobname>_<i>.root exists at the destination.

No Slurm job id is needed, so it works whether you submitted with
submit_slurm.py or a bare 'sbatch job.sh'. Each missing output maps back to its
filelists/job_<i>.txt and can be resubmitted with --resubmit, which runs
'sbatch --array=<missing> job.sh' (a command-line --array overrides the one
baked into job.sh). Exit status is 0 only if every expected output is present.
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

HERE = os.path.dirname(os.path.abspath(__file__))


def parse_jobsh(job_sh):
    """Pull the injected KEY="value" config lines out of a generated job.sh."""
    vals = {}
    with open(job_sh) as fh:
        for line in fh:
            m = re.match(r'^(\w+)="(.*)"$', line.strip())
            if m:
                vals[m.group(1)] = m.group(2)
    return vals


def job_indices(work_dir):
    """Array task indices, taken from filelists/job_<i>.txt."""
    idx = []
    for f in Path(work_dir, "filelists").glob("job_*.txt"):
        m = re.match(r"job_(\d+)\.txt$", f.name)
        if m:
            idx.append(int(m.group(1)))
    return sorted(idx)


def output_present(is_xrootd, host, dest_dir, name):
    """True if <dest_dir>/<name> exists and is non-empty (local or via xrdfs)."""
    if is_xrootd:
        res = subprocess.run(["xrdfs", host, "stat", "%s/%s" % (dest_dir, name)],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
        if res.returncode != 0:
            return False
        m = re.search(r"Size:\s+(\d+)", res.stdout)
        return int(m.group(1)) > 0 if m else True
    path = os.path.join(dest_dir, name)
    try:
        return os.path.isfile(path) and os.path.getsize(path) > 0
    except OSError:
        return False


def compress(indices):
    """[0,1,2,4] -> '0-2,4' for a tidy --array= list."""
    indices = sorted(indices)
    out, i = [], 0
    while i < len(indices):
        j = i
        while j + 1 < len(indices) and indices[j + 1] == indices[j] + 1:
            j += 1
        out.append(str(indices[i]) if i == j else "%d-%d" % (indices[i], indices[j]))
        i = j + 1
    return ",".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("work_dir", help="The submit_slurm.py --work-dir to check.")
    ap.add_argument("--resubmit", action="store_true",
                    help="Resubmit the missing tasks (sbatch --array=<missing> job.sh).")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="List every task, not just the missing ones.")
    args = ap.parse_args()

    work_dir = os.path.abspath(args.work_dir)
    job_sh = os.path.join(work_dir, "job.sh")
    if not os.path.isfile(job_sh):
        sys.exit("ERROR: %s not found - is that a submit_slurm.py work dir?" % job_sh)

    cfg = parse_jobsh(job_sh)
    jobname = cfg.get("JOBNAME", "job")
    is_xrootd = cfg.get("DEST_IS_XROOTD") == "1"
    host = cfg.get("DEST_HOST", "")
    dest_dir = cfg.get("DEST_DIR", "")
    indices = job_indices(work_dir)
    if not indices:
        sys.exit("ERROR: no filelists/job_*.txt found in %s" % work_dir)

    present, missing = [], []
    for i in indices:
        ok = output_present(is_xrootd, host, dest_dir, "%s_%d.root" % (jobname, i))
        (present if ok else missing).append(i)
        if args.verbose:
            print("  [%s] task %-4d %s_%d.root" % ("ok  " if ok else "MISS", i, jobname, i))

    print("=" * 60)
    print("Work dir : %s" % work_dir)
    print("Output   : %s%s"
          % (dest_dir, "  (xrootd: %s)" % host if is_xrootd else "  (local)"))
    print("Present  : %d / %d" % (len(present), len(indices)))

    if not missing:
        print("All %d outputs present - every job succeeded." % len(indices))
        print("=" * 60)
        sys.exit(0)

    arr = compress(missing)
    print("Missing  : %d  (tasks %s)" % (len(missing), arr))
    for i in missing:
        print("   task %-4d  <-  %s/filelists/job_%d.txt" % (i, work_dir, i))
    resubmit = ["sbatch", "--array=%s" % arr, job_sh]

    if args.resubmit:
        sbatch = shutil.which("sbatch")
        if not sbatch:
            print("\n--resubmit: 'sbatch' not found here; run on the host:")
            print("  %s" % " ".join(resubmit))
        else:
            print("\nResubmitting: %s" % " ".join(resubmit))
            sys.stdout.flush()
            r = subprocess.run([sbatch, "--array=%s" % arr, job_sh])
            if r.returncode != 0:
                sys.exit("ERROR: resubmit failed (rc=%d)" % r.returncode)
    else:
        print("\nResubmit the missing tasks with:")
        print("  %s" % " ".join(resubmit))
        print("  (or: %s/check_jobs.py %s --resubmit)" % (HERE, work_dir))
    print("=" * 60)
    sys.exit(1)


if __name__ == "__main__":
    main()
