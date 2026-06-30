#!/bin/bash
#
# setup.sh — install all SUEP muon-detector-shower (MDS) NanoAOD packages into a
# CMSSW_15_0_2 area. Self-contained: no PhysicsTools checkout / cherry-pick needed.
#
# Usage:
#   cmsrel CMSSW_15_0_2
#   cd CMSSW_15_0_2/src && cmsenv
#   git clone <this-repo> SUEPMDSNano       # (or copy this folder here)
#   bash SUEPMDSNano/setup.sh [n_build_threads]
#
# It just copies the package trees into $CMSSW_BASE/src and builds. The pieces
# that used to be PhysicsTools/NanoAOD cherry-picks are now standalone in
# HMTntuple/CSCShowerAnalyzer (a plugin file + a customise), so there is no
# dependency on a modified base package.
set -euo pipefail
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -z "${CMSSW_BASE:-}" ]; then
  echo "ERROR: no CMSSW environment. Do: cmsrel CMSSW_15_0_2 && cd CMSSW_15_0_2/src && cmsenv" >&2
  exit 1
fi

echo "--- copying packages into $CMSSW_BASE/src ---"
cp -r "$REPO"/SUEPProduction "$CMSSW_BASE/src/"  # shared suep::LLPMDSMatch dataformat
cp -r "$REPO"/HMTntuple      "$CMSSW_BASE/src/"  # rechit/segment/shower + LLP-match + MDS table plugins
cp -r "$REPO"/MDSNANO        "$CMSSW_BASE/src/"  # run config + slurm tools

echo "--- scram b ---"
cd "$CMSSW_BASE/src"
scram b -j"${1:-8}"

cat <<'EOF'

Done. Running MDSNANO/RunIII2024MC.py over an AODSIM produced with the
llpMDSRecHitMatcher (see SVJ/Production) now adds LLP-truth columns
  llpIdx, llpPdgId, llpMatchType, llpNSimHits, llpSimX/Y/Z, llpSimE, llpSimTof, llpSimDxy
to the cscRechits / dtRecHits / rpcRecHits NanoAOD collections, e.g.:
  cmsRun MDSNANO/RunIII2024MC.py inputFiles=file:AODSIM.root outputFile=nano.root maxEvents=-1
EOF
