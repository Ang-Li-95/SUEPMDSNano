# SUEPMDSNano

A **single, self-contained** git repository holding all the additions needed to
produce the SUEP / muon-detector-shower (MDS) NanoAOD, including the LLP-matched
muon-detector RecHit truth. It consolidates what previously lived in several
separate repos/checkouts (`HMTntuple`, `MDSNANO`, `SUEPProduction`, and a
`PhysicsTools/NanoAOD` cherry-pick) — and removes the PhysicsTools dependency
entirely (the cherry-picked lines are now standalone in this package).

## What's inside

```
SUEPMDSNano/                      # = the CMSSW subsystem; clone directly into $CMSSW_BASE/src
├── MDSFormats/                   # the suep::LLPMDSMatch dataformat (+ dictionary)
├── CSCShowerAnalyzer/            # CSC/DT/RPC rechit, segment, shower & LLP-match tables
│   ├── plugins/
│   │   ├── LLPMDSMatchTableProducer.cc          #   LLP-truth columns (templated, hardened)
│   │   └── MDSSimpleFlatTableProducerPlugins.cc #   was the PhysicsTools cherry-pick
│   └── python/custom_mds_cff.py                 #   add_mdsTables(process, saveRechits=True)
└── MDSNANO/                      # the cmsRun config + Slurm submit/check tools
    ├── RunIII2024MC.py
    ├── submit_slurm.py
    └── check_jobs.py
```

The repository itself acts as the CMSSW subsystem: cloned as
`$CMSSW_BASE/src/SUEPMDSNano`, scram picks up the packages
`SUEPMDSNano/MDSFormats` and `SUEPMDSNano/CSCShowerAnalyzer` directly — no
copy step needed.

## Install

```bash
cmsrel CMSSW_15_0_2
cd CMSSW_15_0_2/src && cmsenv
git clone <this-repo> SUEPMDSNano        # or copy this folder here
scram b -j 8
```

No `git cms-addpkg` / cherry-pick step — everything is self-contained.

## Run

```bash
cmsRun SUEPMDSNano/MDSNANO/RunIII2024MC.py inputFiles=file:AODSIM.root outputFile=nano.root maxEvents=-1
```

The AODSIM input must have been produced with the `llpMDSRecHitMatcher` module
(see the companion `SVJ/Production` area). `add_mdsTables(process, saveRechits=True)`
then attaches the LLP-truth columns to the muon rechit tables — one entry per
rechit, `llpIdx < 0` if unmatched:

| collection | added columns |
|---|---|
| `cscRechits`, `dtRecHits`, `rpcRecHits` | `llpIdx`, `llpPdgId`, `llpMatchType` (1=geo, 2=digisimlink, 3=both), `llpNSimHits`, `llpSimX/Y/Z`, `llpSimE`, `llpSimTof`, `llpSimDxy` |

It also adds `vx/vy/vz` (gen production vertex) to the standard `GenPart` table.

## How the LLP truth gets here

The matching itself runs upstream in the AODSIM step (`SVJ/Production`,
`SUEPProduction/MDS/LLPMDSRecHitMatcher`): gen `pdgId 999999` → decay products →
SimTracks → muon-detector RecHits (geometric and/or DigiSimLink). It publishes an
`edm::ValueMap<suep::LLPMDSMatch>` per rechit collection, **keyed to
`csc2DRecHits`/`dt1DRecHits`/`rpcRecHits`**. A `ValueMap` of a POD struct is
schema-stable across releases, so it survives the AODSIM (CMSSW_14_0_21) →
NanoAOD (CMSSW_15_0_2) boundary — a `nanoaod::FlatTable` does **not** (its
`ColumnType` enum differs between the two releases and crashes the output module
on read). `LLPMDSMatchTableProducer` here reads that ValueMap and turns it into
the NanoAOD columns above, looking up by `(ProductID, index)` so the rows are
guaranteed to line up with the rechit table they extend.

## Notes

- **No PhysicsTools dependency.** The two former cherry-picks now live here:
  the `SimpleMuonRecHitClusterFlatTableProducer` (and the L1 shower table
  producers) are instantiated in `plugins/MDSSimpleFlatTableProducerPlugins.cc`
  (it just `#include`s the release template header), and the gen `vx/vy/vz`
  columns are added by `add_mdsTables` as a customise of `genParticleTable`.
  If a future CMSSW release ships these officially, delete the standalone copies
  to avoid a duplicate plugin name.
- **`SUEPMDSNano/MDSFormats` is shared with the production area.** The same
  dataformat must also be built in the CMSSW_14_0_21 production area (it is,
  under `SVJ/Production/cmssw/SUEPProduction/MDSFormats`). Keep the two copies of
  `interface/LLPMDSMatch.h` identical — if you change the struct, bump the
  `ClassVersion`/checksum in `src/classes_def.xml` in **both** places.
