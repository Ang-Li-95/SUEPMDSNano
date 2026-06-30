# NanoAOD Slurm production tools

Run `RunIII2024MC.py` (a CMSSW `cmsRun` NanoAOD config) over many ROOT files on a
Slurm cluster: scan a directory for inputs, split them into per-job chunks,
submit a Slurm **array** job, and stage each job's output to a local path or to
EOS over xrootd. A companion tool checks which jobs succeeded and resubmits the
failures.

## Contents

| File | Purpose |
|------|---------|
| `RunIII2024MC.py` | The `cmsRun` config. Extended to take input/output from the command line (backward compatible). |
| `submit_slurm.py` | Build the file lists and submit the Slurm array job. |
| `check_jobs.py`   | Check whether every job produced its output, and resubmit the missing ones. |

Both scripts are plain `python3` (standard library only) — they need **no CMSSW
environment**. Only the jobs themselves enter the CMSSW container.

## Requirements & where to run

- **Run on the host / login node, not inside the `cmssw-el8` container.**
  Submission needs `sbatch`, which is not available inside the container.
  The scripts auto-detect `CMSSW_BASE`/`SCRAM_ARCH`, so you do *not* need to
  `cmsenv` first.
- This release is built for **el8** while the nodes are **slc7**, so each job
  runs `cmsRun` inside the **`cmssw-el8`** apptainer container automatically
  (auto-selected from the release's `SCRAM_ARCH`). `apptainer` + `/cvmfs` must be
  available on the worker nodes (they are on this cluster).
- For EOS input/output you need `xrdfs`/`xrdcp` on the host and valid storage
  auth at job runtime (see [Notes](#notes--gotchas)).

## Quick start

```bash
# from a host shell, in this directory
./submit_slurm.py -i /path/to/AODSIM -n 5 -o /path/to/output/nano --job-name myprod

# ...later, once the array has finished:
./check_jobs.py slurm_myprod              # report success / list missing
./check_jobs.py slurm_myprod --resubmit   # rerun only the missing tasks
```

Add `--dry-run` to `submit_slurm.py` to generate everything (file lists, `job.sh`)
without actually submitting.

---

## 1. The cmsRun config: `RunIII2024MC.py`

A `VarParsing` block was added at the end so the same config can be parametrized
on the command line. **With no arguments it behaves exactly as before.**

```bash
# pass a file containing one input file per line (what the jobs use)
cmsRun RunIII2024MC.py fileList=files.txt outputFile=out.root maxEvents=-1

# or pass inputs inline
cmsRun RunIII2024MC.py inputFiles=file:/a.root,file:/b.root outputFile=out.root
```

| Argument | Meaning |
|----------|---------|
| `fileList=<txt>`   | Text file with one input file (PFN/LFN/URL) per line. Takes priority over `inputFiles`. |
| `inputFiles=a,b`   | Comma-separated input files. |
| `outputFile=<name>`| Output file name (a bare name is written locally as `file:<name>`). |
| `maxEvents=<N>`    | Events per job (`-1` = all). |

You normally don't call these by hand — `submit_slurm.py` builds the `fileList`
and passes the arguments for you.

---

## 2. Submitting: `submit_slurm.py`

```text
./submit_slurm.py -i <input> -n <files-per-job> -o <output> [options]
```

### Required

| Flag | Meaning |
|------|---------|
| `-i, --input-dir` | Where to find input ROOT files (see [input forms](#input-forms)). |
| `-n, --files-per-job` | Number of input files per job. |
| `-o, --output` | Output destination directory (local path or `root://host//path`). |

### Input forms

`-i` accepts any of:

```bash
-i /local/path/to/AODSIM                                   # local directory (recursed)
-i root://eoscms.cern.ch//store/user/lian/.../AODSIM       # xrootd URL
-i /store/user/lian/.../AODSIM --redirector root://eoscms.cern.ch/   # bare path + endpoint
```

Remote directories are listed with `xrdfs <host> ls -R`; point `--redirector` at
the **storage endpoint** (e.g. `eoscms.cern.ch`, `eosuser.cern.ch`), not a
read-only global redirector, since it must support directory listing.

### Output forms

```bash
-o /local/path/nano                              # local filesystem
-o root://eosuser.cern.ch//eos/user/a/ang/nano   # EOS over xrootd
```

The destination directory is created if missing. Each job writes
`<job-name>_<i>.root` there (`i` = array task index).

### Common options

| Flag | Default | Meaning |
|------|---------|---------|
| `--job-name` | `nano` | Base name for the jobs and output files. |
| `--work-dir` | `./slurm_<job-name>` | Where file lists, logs and scripts go. |
| `--max-events` | `-1` | Events per job. |
| `--pattern` | `*.root` | Which files to pick up. |
| `--no-recursive` | (off) | Only scan the top level of the input dir. |
| `--config` | `RunIII2024MC.py` | The cmsRun config to run. |
| `--dry-run` | (off) | Prepare everything but don't submit. |

### Slurm resources

| Flag | Default | Meaning |
|------|---------|---------|
| `--time` | `08:00:00` | Wall time per job. |
| `--mem` | `4000` | Memory per job (MB). |
| `--cpus` | `1` | CPUs per task. |
| `--partition` | (cluster default) | Slurm partition (here: `c`, `m`, `g`). |
| `--account` | — | Slurm account. |
| `--max-concurrent` | — | Cap simultaneously running array tasks (Slurm `%N`). |
| `--extra-sbatch` | — | Extra `#SBATCH` line, repeatable, e.g. `--extra-sbatch '--qos=long'`. |

### Environment overrides (rarely needed)

`--cmssw-base`, `--scram-arch`, `--container` (use `--container none` to run
`cmsRun` directly without a container) are all auto-detected by default.

### What it creates

```
<work-dir>/
├── job.sh                # the Slurm array script (sbatch job.sh)
├── submit.sh             # one-line helper: 'exec sbatch job.sh'
├── filelists/
│   ├── job_0.txt         # inputs for array task 0
│   └── job_1.txt ...
└── logs/
    └── <job-name>_<jobid>_<task>.out / .err
```

If you run `submit_slurm.py` from **inside** the container (where `sbatch` is
missing), it still prepares everything, writes `submit.sh`, prints the exact
`sbatch` command, and exits cleanly — just run `bash <work-dir>/submit.sh` from a
host shell.

### Examples

```bash
# 200 local AODSIM files, 10 per job (=20 jobs), output to EOS, cap 50 running
./submit_slurm.py -i /scratch-cbe/users/me/AODSIM -n 10 \
    -o root://eosuser.cern.ch//eos/user/m/me/nano \
    --job-name suep --max-concurrent 50 --time 12:00:00 --mem 6000

# remote EOS inputs, local output, just preview
./submit_slurm.py -i /store/user/lian/.../AODSIM --redirector root://eoscms.cern.ch/ \
    -n 5 -o /scratch-cbe/users/me/nano --dry-run
```

---

## 3. Checking results: `check_jobs.py`

```text
./check_jobs.py <work-dir> [--resubmit] [-v]
```

A job is counted as **succeeded** iff its output file `<job-name>_<i>.root` is
present and non-empty at the destination. That is a reliable end-to-end signal
because a job only stages its output out as its very last step, after `cmsRun`
exits successfully (see [How it works](#how-it-works)). No Slurm job id is needed,
so this works whether you submitted with `submit_slurm.py` or a bare `sbatch`.

```text
$ ./check_jobs.py slurm_suep
============================================================
Work dir : /…/slurm_suep
Output   : /eos/user/m/me/nano  (xrootd: eosuser.cern.ch)
Present  : 18 / 20
Missing  : 2  (tasks 7,13)
   task 7     <-  /…/slurm_suep/filelists/job_7.txt
   task 13    <-  /…/slurm_suep/filelists/job_13.txt

Resubmit the missing tasks with:
  sbatch --array=7,13 /…/slurm_suep/job.sh
  (or: /…/check_jobs.py /…/slurm_suep --resubmit)
============================================================
```

- Exit status is `0` only if **every** output is present (handy in scripts).
- `-v` lists every task, not just the missing ones.
- `--resubmit` runs the `sbatch --array=<missing> job.sh` for you.

### The check → resubmit loop

```bash
./check_jobs.py slurm_suep              # "All N outputs present" → done
./check_jobs.py slurm_suep --resubmit   # otherwise: rerun just the failures
# repeat until it reports all present
```

Resubmitting a subset works because a command-line `--array=` **overrides** the
`#SBATCH --array=` baked into `job.sh`, and each task is deterministic: task `i`
always reads `filelists/job_i.txt` and writes `<job-name>_i.root`.

> **Do not re-run `submit_slurm.py` to resubmit failures.** It regenerates all
> the file lists and submits the *entire* array again. Use the
> `sbatch --array=<missing> job.sh` route (i.e. `--resubmit`) instead.

---

## How it works

- **Container.** The release is el8, the nodes are slc7, so each task runs
  `cmsRun` inside `cmssw-el8` via
  `/cvmfs/cms.cern.ch/common/cmssw-el8 --command-to-run bash <inner script>`.
  This cluster's apptainer uses `mount hostfs = yes`, so all host filesystems
  (`/users`, `/groups`, `/scratch-cbe`, `/eos`, `/tmp`, …) are visible inside —
  no bind paths are needed.
- **Submission is a host operation.** `sbatch` (and munge auth) are not present
  in the container, so the submitter runs on the host. It needs no `cmsenv`.
- **Success = output present.** The inner script runs `cmsRun` under `set -e`;
  if `cmsRun` fails (including breaking mid-event-loop, where a partial output
  file exists and the exit code is non-zero), that non-zero status propagates out
  of the container and the outer `set -eo pipefail` aborts **before** the
  stage-out step. So a failed job never copies anything to the destination, and
  "output present" reliably means "this job fully succeeded".
- **Atomic stage-out.** Output is copied with `xrdcp --posc` (EOS) or to a temp
  file followed by an atomic `mv` (local), so an interrupted copy never leaves a
  partial file masquerading as a good one.

## Notes & gotchas

- **Check after the array finishes.** While jobs are still running their outputs
  don't exist yet, so they'll show as "missing".
- **EOS authentication.** Reading remote inputs / writing EOS outputs at job
  runtime needs valid auth on the worker (kerberos for CERN user EOS, an X509
  proxy or token for `/store` via AAA). Set this up the way your site requires.
- **Persistently-missing task = real failure.** If `--resubmit` keeps failing on
  the same task, the failure is deterministic (e.g. a corrupt input or a real
  bug). Open that task's `logs/*.err` and read the exception instead of
  resubmitting again.
- **Partition.** If the cluster requires an explicit partition/account, pass
  `--partition` / `--account` (this cluster's partitions are `c` (default),
  `m`, `g`).
