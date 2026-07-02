import FWCore.ParameterSet.Config as cms

# Reads the edm::ValueMap<suep::LLPMDSMatch> made in the AODSIM step (module
# 'llpMDSRecHitMatcher') and emits the LLP truth columns as an *extension* of the
# matching muon rechit table (cscRechits / dtRecHits / rpcRecHits).
#
# The producer is a template instantiated per rechit type (CSC/DT/RPC); each
# instance consumes both the rechit collection ('recHits') and the ValueMap
# ('src') and looks the map up by (ProductID, index), which validates that the
# ValueMap is keyed to exactly that rechit collection.

cscLLPMatchTable = cms.EDProducer(
    "CSCLLPMDSMatchTableProducer",
    recHits=cms.InputTag("csc2DRecHits"),
    src=cms.InputTag("llpMDSRecHitMatcher", "csc"),
    name=cms.string("cscRechits"),  # must match the rechit table this extends
    doc=cms.string("LLP truth match for CSC rechits"),
    extension=cms.bool(True),
)

dtLLPMatchTable = cms.EDProducer(
    "DTLLPMDSMatchTableProducer",
    recHits=cms.InputTag("dt1DRecHits"),
    src=cms.InputTag("llpMDSRecHitMatcher", "dt"),
    name=cms.string("dtRecHits"),
    doc=cms.string("LLP truth match for DT rechits"),
    extension=cms.bool(True),
)

rpcLLPMatchTable = cms.EDProducer(
    "RPCLLPMDSMatchTableProducer",
    recHits=cms.InputTag("rpcRecHits"),
    src=cms.InputTag("llpMDSRecHitMatcher", "rpc"),
    name=cms.string("rpcRecHits"),
    doc=cms.string("LLP truth match for RPC rechits"),
    extension=cms.bool(True),
)
