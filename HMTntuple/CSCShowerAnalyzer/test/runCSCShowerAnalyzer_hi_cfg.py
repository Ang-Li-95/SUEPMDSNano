import sys
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from Configuration.Eras.Era_Run3_pp_on_PbPb_cff import Run3_pp_on_PbPb

isCondor = True
outname = "plots.root"

if not isCondor:
    options = VarParsing('analysis')
    options.register("runNumber", -1, VarParsing.multiplicity.singleton, VarParsing.varType.int,"run number")
    options.parseArguments()

    process = cms.Process("ANALYSIS", Run3_pp_on_PbPb)
    process.load("FWCore.MessageLogger.MessageLogger_cfi")

    process.options.wantSummary=True

    process.maxEvents = cms.untracked.PSet(
          input = cms.untracked.int32(options.maxEvents)
    )

    if (options.inputFiles[0]).split(".")[-1]=="txt":
        inputfiles = []
        with open(options.inputFiles[0]) as f:
            inputfiles = [line.strip() for line in f]
        print(inputfiles)
    else:
        inputfiles = options.inputFiles


    process.source = cms.Source(
          "PoolSource",
          fileNames = cms.untracked.vstring(inputfiles)
    )

    if options.runNumber==-1:
        outname = "plots.root"
    else:
        outname = "plots_%s.root"%options.runNumber

else:
    process = cms.Process("ANALYSIS", Run3_pp_on_PbPb)
    process.load("FWCore.MessageService.MessageLogger_cfi")


    ## input to be set by driver script
    process.source = cms.Source(
          "PoolSource",
          fileNames = cms.untracked.vstring(""),
          inputCommands = cms.untracked.vstring( 'keep *')
    )


process.options = cms.untracked.PSet(
      SkipEvent = cms.untracked.vstring('ProductNotFound'),
)


process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string(outname)
                                   )


process.load("Configuration/StandardSequences/FrontierConditions_GlobalTag_cff")
process.load("Configuration/StandardSequences/GeometryRecoDB_cff")
process.load("Configuration/StandardSequences/MagneticField_cff")
#process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
#process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')

process.load("CalibMuon.CSCCalibration.CSCL1TPLookupTableEP_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run3_data', '')
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase1_2023_realistic_hi", "")

process.load("EventFilter.DTRawToDigi.dtunpacker_cfi")
process.load("EventFilter.CSCRawToDigi.cscUnpacker_cfi")
process.unpacksequence = cms.Sequence(process.muonCSCDigis*process.muonDTDigis)


process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = "muonCSCDigis:MuonCSCComparatorDigi"
process.cscTriggerPrimitiveDigis.CSCWireDigiProducer = "muonCSCDigis:MuonCSCWireDigi"
process.cscTriggerPrimitiveDigis.commonParam.runME11ILT = False
process.cscTriggerPrimitiveDigis.commonParam.runME21ILT = False

#from L1Trigger.CSCTriggerPrimitives.CSCShowerAnalyzer_cfi import cscTriggerPrimitivesAnalyzer
from HMTntuple.CSCShowerAnalyzer.CSCShowerAnalyzer_cfi import cscTriggerPrimitivesAnalyzer


process.cscTriggerPrimitivesAnalyzer = cscTriggerPrimitivesAnalyzer
process.cscTriggerPrimitivesAnalyzer.debug=cms.bool(False)

process.simpleCSCshowerFilter = cms.EDFilter("SimpleCSCshowerFilter",
    muons = cms.InputTag("muons"),
    dataLCTShower = cms.InputTag("muonCSCDigis","MuonCSCShowerDigi"),
    emulLCTShower = cms.InputTag("cscTriggerPrimitiveDigis"),
    ca4CSCrechitClusters = cms.InputTag("ca4CSCrechitClusters"),
    recHitLabel = cms.InputTag("csc2DRecHits"),
    hltresults = cms.InputTag( "TriggerResults", "", "HLT" ),
    AsL1filter = cms.bool(False),
    AsRecofilter = cms.bool(True),
    debug = cms.bool(True)
)
process.l1filter_step = cms.Path(process.simpleCSCshowerFilter)
process.l1sequence = cms.Sequence(process.cscTriggerPrimitiveDigis)
process.l1sequence += process.cscTriggerPrimitivesAnalyzer
process.cscShowerAnalyzer = cms.Path(cscTriggerPrimitivesAnalyzer)


#import EventFilter.DTRawToDigi.dtunpacker_cfi
#from RecoLocalMuon.DTRecHit.dt1DRecHits_LinearDriftFromDB_cfi import *
#from RecoLocalMuon.CSCRecHitD.cscRecHitD_cfi import *

process.dummy1 = cms.ESSource("EmptyESSource",
                                  recordName = cms.string("CSCIndexerRecord"),
                                  firstValid = cms.vuint32(1),
                                  iovIsRunNotTime = cms.bool(True)
                              )

process.dummy2 = cms.ESSource("EmptyESSource",
                                  recordName = cms.string("CSCChannelMapperRecord"),
                                  firstValid = cms.vuint32(1),
                                  iovIsRunNotTime = cms.bool(True)
                              )

process.CSCIndexerESProducer = cms.ESProducer("CSCIndexerESProducer", AlgoName = cms.string("CSCIndexerStartup") )
process.CSCChannelMapperESProducer = cms.ESProducer("CSCChannelMapperESProducer", AlgoName = cms.string("CSCChannelMapperStartup") )
process.load('RecoLocalMuon.CSCRecHitD.cscRecHitD_cfi')
#process.load('RecoLocalMuon.DTRecHit.dt1DRecHits_LinearDriftFromDB_cfi')
#process.load('RecoLocalMuon.Configuration.RecoLocalMuon_cff')
#process.load('RecoMuon.Configuration.RecoMuonPPonly_cff')
process.reco = cms.Path(process.csc2DRecHits )
#process.reco = cms.Path(process.muonlocalreco * process.standalonemuontracking)
process.recoSeq = cms.Sequence(
                process.csc2DRecHits,
                process.globalreco_trackingTask,
                process.muonGlobalRecoTask)


#process.raw2digi_step = cms.Path(process.RawToDigi)
#process.L1Reco_step = cms.Path(process.L1Reco)
#process.reconstruction_step = cms.Path(process.reconstruction)
#process.reconstruction_step = cms.Path(process.recoSeq)


import RecoMuon.MuonRechitClusterProducer.cscRechitClusterProducer_cfi as CSCcluster

process.ca4CSCrechitClusters= CSCcluster.cscRechitClusterProducer.clone(
    recHitLabel = "csc2DRecHits",
    nRechitMin  = 50,
    rParam      = 0.4,
    nStationThres = 10,
)

process.producer = cms.Path(process.ca4CSCrechitClusters)


outf = "output_reco.root"

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(outf), # choose your output file
    outputCommands = cms.untracked.vstring(
              'drop *',
              #'keep *',
              #'keep *_gen*_*_*',
              'keep *ShowerDigi*_*cscTriggerPrimitive*_*_*',
              'keep *_csc2DRecHits_*_*',
              #'keep *_muons*_*_*',
              'keep *_ca4*_*_*',
    ),
    SelectEvents = cms.untracked.PSet(
      SelectEvents = cms.vstring("l1filter_step")
    ),

)


process.end = cms.EndPath(process.out)

process.p = cms.Path(process.unpacksequence * process.l1sequence )

process.schedule = cms.Schedule(process.p,
                                process.reco,
                                #process.raw2digi_step,
                                #process.L1Reco_step,
                                #process.cscShowerAnalyzer,
                                #process.reconstruction_step, 
                                process.producer,
                                process.l1filter_step,
                                process.end)

from FWCore.ParameterSet.MassReplace import massReplaceInputTag as MassReplaceInputTag
MassReplaceInputTag(process,new="rawPrimeDataRepacker")
