import FWCore.ParameterSet.Config as cms

simpleCSCshowerFilter = cms.EDFilter("SimpleCSCshowerFilter",
    dataLCTShower = cms.InputTag("muonCSCDigis","MuonCSCShowerDigi"),
    ca4CSCrechitClusters = cms.InputTag("ca4CSCrechitClusters"),
    AsL1filter = False,
    AsRecofilter = True 
)
