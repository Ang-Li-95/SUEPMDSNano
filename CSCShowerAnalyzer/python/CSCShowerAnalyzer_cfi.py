import FWCore.ParameterSet.Config as cms


cscTriggerPrimitivesAnalyzer = cms.EDAnalyzer(
    "CSCShowerAnalyzer",
    CSCComparatorDigiProducer = cms.InputTag("muonCSCDigis","MuonCSCComparatorDigi"),
    CSCWireDigiProducer = cms.InputTag("muonCSCDigis","MuonCSCWireDigi"),
    dataALCT      = cms.InputTag("muonCSCDigis", "MuonCSCALCTDigi"),
    dataLCT       = cms.InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"),
    dataALCTShower = cms.InputTag("muonCSCDigis", "MuonCSCShowerDigiAnodeALCT"),
    dataALCTShower_TMB = cms.InputTag("muonCSCDigis", "MuonCSCShowerDigiAnode"),
    emulALCTShower = cms.InputTag("cscTriggerPrimitiveDigis", "Anode"),
    dataCLCTShower = cms.InputTag("muonCSCDigis", "MuonCSCShowerDigiCathode"),
    emulCLCTShower = cms.InputTag("cscTriggerPrimitiveDigis", "Cathode"),
    dataLCTShower = cms.InputTag("muonCSCDigis","MuonCSCShowerDigi"),
    emulLCTShower = cms.InputTag("cscTriggerPrimitiveDigis"),
    debug = cms.bool(True)
)


