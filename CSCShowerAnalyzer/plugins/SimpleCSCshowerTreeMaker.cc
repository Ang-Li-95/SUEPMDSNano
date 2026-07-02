// -*- C++ -*-
//
// Package:    L1Trigger/CSCTriggerPrimitives
// Class:      SimpleCSCshowerTreeMaker
//
/**\class SimpleCSCshowerTreeMaker SimpleCSCshowerTreeMaker.cc L1Trigger/CSCTriggerPrimitives/plugins/SimpleCSCshowerTreeMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ka Hei Martin Kwok
//         Created:  Thu, 07 Jul 2022 16:53:22 GMT
//
//

// system include files
#include <memory>
#include "TTree.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/CSCDigi/interface/CSCShowerDigiCollection.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
//
// class declaration
//

class SimpleCSCshowerTreeMaker : public edm::stream::EDFilter<> {
public:
  explicit SimpleCSCshowerTreeMaker(const edm::ParameterSet&);
  ~SimpleCSCshowerTreeMaker();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual bool filter(edm::Event&, const edm::EventSetup&) override;
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  
  void reset();
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
  //
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> geometryToken_;
  edm::EDGetTokenT<CSCRecHit2DCollection> inputToken_;
  const edm::EDGetTokenT<CSCShowerDigiCollection> dataLCTShower_token_;
  const edm::EDGetTokenT<CSCShowerDigiCollection> emulLCTShower_token_;
  const edm::EDGetTokenT<reco::MuonRecHitClusterCollection> ca4CSCrechitClusters_token_;
  const edm::EDGetTokenT<reco::MuonCollection> muons_token_;
  const edm::EDGetTokenT<edm::TriggerResults> triggerBitsToken_; 
  bool AsL1filter;
  bool AsRecofilter;
  bool debug_;
  TTree* hmtTree;
  int passL1;
  int hasCluster;
  int runNum;
  int lumiNum;
  int eventNum;
  bool isHLT_PPZeroBias;

#define HMTARRAYSIZE 1000
  int nlctHMT;
  int lctHMT_chamber[HMTARRAYSIZE];
  int lctHMT_sr[HMTARRAYSIZE];
  int lctHMT_sector[HMTARRAYSIZE];
  int lctHMT_bits[HMTARRAYSIZE];
  int lctHMT_BX[HMTARRAYSIZE];
  int lctHMT_ComparatorNHits[HMTARRAYSIZE];
  int lctHMT_WireNHits[HMTARRAYSIZE];
  int nelctHMT;
  int elctHMT_chamber[HMTARRAYSIZE];
  int elctHMT_sr[HMTARRAYSIZE];
  int elctHMT_sector[HMTARRAYSIZE];
  int elctHMT_bits[HMTARRAYSIZE];
  int elctHMT_BX[HMTARRAYSIZE];
  int elctHMT_ComparatorNHits[HMTARRAYSIZE];
  int elctHMT_WireNHits[HMTARRAYSIZE];

  int    nca4CSCcluster;
  int    ca4CSCclusterSize[HMTARRAYSIZE];    
  float  ca4CSCclusterX[HMTARRAYSIZE];       
  float  ca4CSCclusterY[HMTARRAYSIZE];       
  float  ca4CSCclusterZ[HMTARRAYSIZE];       
  float  ca4CSCclusterEta[HMTARRAYSIZE];     
  float  ca4CSCclusterPhi[HMTARRAYSIZE];     
  float  ca4CSCclusterTpeak[HMTARRAYSIZE];   
  float  ca4CSCclusterWireTime[HMTARRAYSIZE];
  float  ca4CSCclusterTime[HMTARRAYSIZE];    
  float  ca4CSCclusterTimeSpread[HMTARRAYSIZE];
  int    ca4CSCclusterME11_12[HMTARRAYSIZE];
  int    ca4CSCclusterNstation10[HMTARRAYSIZE];
  float  ca4CSCclusterAvgStation10[HMTARRAYSIZE];

#define CSCRECHITARRAYSIZE 100000
  int ncscRechits;
  int ncscRechitsChambers;
  float cscRechitsPhi[CSCRECHITARRAYSIZE];
  float cscRechitsEta[CSCRECHITARRAYSIZE];
  float cscRechitsX[CSCRECHITARRAYSIZE];
  float cscRechitsY[CSCRECHITARRAYSIZE];
  float cscRechitsZ[CSCRECHITARRAYSIZE];
  float cscRechitsE[CSCRECHITARRAYSIZE];
  float cscRechitsTpeak[CSCRECHITARRAYSIZE];
  float cscRechitsTwire[CSCRECHITARRAYSIZE];
  int   cscRechitsQuality[CSCRECHITARRAYSIZE];
  int   cscRechitsChannels[CSCRECHITARRAYSIZE];
  int   cscRechitsNStrips[CSCRECHITARRAYSIZE];
  int   cscRechitsHitWire[CSCRECHITARRAYSIZE];
  int   cscRechitsWGroupsBX[CSCRECHITARRAYSIZE];
  int   cscRechitsNWireGroups[CSCRECHITARRAYSIZE];
  int   cscRechitsDetId[CSCRECHITARRAYSIZE];
  int   cscRechitsChamber[CSCRECHITARRAYSIZE];
  int   cscRechitsIChamber[CSCRECHITARRAYSIZE];
  int   cscRechitsStation[CSCRECHITARRAYSIZE];

  int nMuons;
  float muonE[CSCRECHITARRAYSIZE];
  float muonPt[CSCRECHITARRAYSIZE];
  float muonEta[CSCRECHITARRAYSIZE];
  float muonPhi[CSCRECHITARRAYSIZE];
  int muonCharge[CSCRECHITARRAYSIZE];//muon charge
  bool muonIsLoose[CSCRECHITARRAYSIZE];
  bool muonIsMedium[CSCRECHITARRAYSIZE];
  bool  muonIsGlobal[CSCRECHITARRAYSIZE];
 
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
SimpleCSCshowerTreeMaker::SimpleCSCshowerTreeMaker(const edm::ParameterSet& iConfig) :
  //now do what ever initialization is needed
  //muons_token_(consumes(iConfig.getParameter<edm::InputTag>("muons")))
  geometryToken_(esConsumes<CSCGeometry, MuonGeometryRecord>()),
  inputToken_(consumes<CSCRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("recHitLabel"))),
  dataLCTShower_token_(consumes(iConfig.getParameter<edm::InputTag>("dataLCTShower"))),
  emulLCTShower_token_(consumes(iConfig.getParameter<edm::InputTag>("emulLCTShower"))),
  ca4CSCrechitClusters_token_(consumes(iConfig.getParameter<edm::InputTag>("ca4CSCrechitClusters"))),
  triggerBitsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("hltresults"))) 
{
  edm::Service<TFileService> fs;
  hmtTree = fs->make<TTree>("hmt", "HMT tree");
  AsL1filter   = iConfig.getParameter<bool>("AsL1filter");
  AsRecofilter = iConfig.getParameter<bool>("AsRecofilter");
  debug_ = iConfig.getParameter<bool>("debug");
}

SimpleCSCshowerTreeMaker::~SimpleCSCshowerTreeMaker() {
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool SimpleCSCshowerTreeMaker::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  bool has_LCTshs=false;
  bool has_CSCclusters=false;
  edm::Handle<CSCShowerDigiCollection> dataLCTshs;
  edm::Handle<CSCShowerDigiCollection> emulLCTshs;
  edm::Handle<reco::MuonRecHitClusterCollection> ca4CSCrechitClusters;
  //edm::Handle<reco::MuonCollection> muons;
  edm::Handle<edm::TriggerResults> triggerBits;

  auto const& geo = iSetup.getData(geometryToken_);
  auto const& rechits = iEvent.get(inputToken_);
  iEvent.getByToken(dataLCTShower_token_, dataLCTshs);
  //iEvent.getByToken(muons_token_, muons);
  iEvent.getByToken(emulLCTShower_token_, emulLCTshs);
  auto const& clusters = iEvent.get(ca4CSCrechitClusters_token_ );

  reset();

  runNum = iEvent.id().run();
  lumiNum = iEvent.luminosityBlock();
  eventNum = iEvent.id().event();

  iEvent.getByToken(triggerBitsToken_, triggerBits);
  const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);

  for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i) {
      std::string hltPathNameReq = "HLT_ZeroBias_v";
      if ((names.triggerName(i)).find(hltPathNameReq) == std::string::npos) continue;
      if ((names.triggerName(i)).find_last_of("_") == std::string::npos) continue;
      isHLT_PPZeroBias = triggerBits->accept(i);
  }

  //for(const pat::Muon &mu : *muons) {
  //  muonE[nMuons] = mu.energy();
  //  muonPt[nMuons] = mu.pt();
  //  muonEta[nMuons] = mu.eta();
  //  muonPhi[nMuons] = mu.phi();
  //  muonCharge[nMuons] = mu.charge();
  //  muonIsLoose[nMuons] = mu.isLooseMuon();
  //  muonIsMedium[nMuons] = mu.isMediumMuon();
  //  muonIsGlobal[nMuons] = muon::isGoodMuon(mu,muon::AllGlobalMuons);
  //  nMuons++;
  //}
  //std::cout << "ncscRechits  "<<ncscRechits<<std::endl;
  std::vector<CSCDetId> unique_ids;
  for (auto const& rechit : rechits) {

    LocalPoint recHitLocalPosition = rechit.localPosition();
    auto detid = rechit.cscDetId();
    for (auto id : unique_ids){
        if (id!=detid)  unique_ids.push_back(detid);
    }
    auto thischamber = geo.chamber(detid);
    int endcap = CSCDetId::endcap(detid) == 1 ? 1 : -1;
    if (thischamber) {
      GlobalPoint globalPosition = thischamber->toGlobal(recHitLocalPosition);

      cscRechitsX[ncscRechits] = globalPosition.x();
      cscRechitsY[ncscRechits] = globalPosition.y();
      cscRechitsZ[ncscRechits] = globalPosition.z();
      cscRechitsPhi[ncscRechits] = globalPosition.phi();
      cscRechitsEta[ncscRechits] = globalPosition.eta();
      cscRechitsE[ncscRechits] = rechit.energyDepositedInLayer();//not saved
      cscRechitsTpeak[ncscRechits] = rechit.tpeak();
      cscRechitsTwire[ncscRechits] = rechit.wireTime();
      cscRechitsQuality[ncscRechits] = rechit.quality();
      cscRechitsChamber[ncscRechits] = endcap * (CSCDetId::station(detid)*10 + CSCDetId::ring(detid));
      if (CSCDetId::ring(detid) == 4) cscRechitsChamber[ncscRechits] = endcap * (CSCDetId::station(detid)*10 + 1);
      cscRechitsIChamber[ncscRechits] = CSCDetId::chamber(detid);
      cscRechitsStation[ncscRechits] = endcap *CSCDetId::station(detid);
      ncscRechits++;
    }
  }
  ncscRechitsChambers = unique_ids.size();
  //std::cout<< "before nca4CSCcluster" << nca4CSCcluster<<std::endl;
  for (auto const& cluster : clusters) {
       ca4CSCclusterSize[nca4CSCcluster] = cluster.size() ;
       ca4CSCclusterEta[nca4CSCcluster]  = cluster.eta();
       ca4CSCclusterPhi[nca4CSCcluster]  = cluster.phi();
       ca4CSCclusterX[nca4CSCcluster]    = cluster.x();
       ca4CSCclusterY[nca4CSCcluster]    = cluster.y();
       ca4CSCclusterZ[nca4CSCcluster]    = cluster.z();
       ca4CSCclusterTime[nca4CSCcluster] = cluster.time();
       ca4CSCclusterTimeSpread[nca4CSCcluster] = cluster.timeSpread();
       ca4CSCclusterME11_12[nca4CSCcluster] = cluster.nME11() + cluster.nME12();
       ca4CSCclusterNstation10[nca4CSCcluster] = cluster.nStation();
       ca4CSCclusterAvgStation10[nca4CSCcluster] = cluster.avgStation();
       nca4CSCcluster++;
        //std::cout<< "after nca4CSCcluster" << nca4CSCcluster<<std::endl;
  }
  const std::map<std::pair<int, int>, int> histIndexCSC = {{{1, 1}, 8},
                                                           {{1, 2}, 7},
                                                           {{1, 3}, 6},
                                                           {{2, 1}, 5},
                                                           {{2, 2}, 4},
                                                           {{3, 1}, 3},
                                                           {{3, 2}, 2},
                                                           {{4, 1}, 1},
                                                           {{4, 2}, 0}};

  const int min_endcap = CSCDetId::minEndcapId();
  const int max_endcap = CSCDetId::maxEndcapId();
  const int min_station = CSCDetId::minStationId();
  const int max_station = CSCDetId::maxStationId();
  const int min_sector = CSCTriggerNumbering::minTriggerSectorId();
  const int max_sector = CSCTriggerNumbering::maxTriggerSectorId();
  const int min_subsector = CSCTriggerNumbering::minTriggerSubSectorId();
  const int max_subsector = CSCTriggerNumbering::maxTriggerSubSectorId();
  const int min_chamber = CSCTriggerNumbering::minTriggerCscId();
  const int max_chamber = CSCTriggerNumbering::maxTriggerCscId();

  for (int endc = min_endcap; endc <= max_endcap; endc++) {
    // loop on all stations
    for (int stat = min_station; stat <= max_station; stat++) {
      int numsubs = ((stat == 1) ? max_subsector : 1);
      // loop on sectors and subsectors
      for (int sect = min_sector; sect <= max_sector; sect++) {
        for (int subs = min_subsector; subs <= numsubs; subs++) {
          // loop on all chambers
          for (int cham = min_chamber; cham <= max_chamber; cham++) {
            // extract the ring number
            int ring = CSCTriggerNumbering::ringFromTriggerLabels(stat, cham);

            // actual chamber number =/= trigger chamber number
            int chid = CSCTriggerNumbering::chamberFromTriggerLabels(sect, subs, stat, cham);

            // 0th layer means whole chamber.
            CSCDetId detid(endc, stat, ring, chid, 0);

            int chamber = detid.chamber();
            int sr = histIndexCSC.at({stat, ring});
            if (endc == 1)
              sr = 17 - sr;
            
            auto range_dataLCTshs = dataLCTshs->get(detid);
            auto range_emulLCTshs = emulLCTshs->get(detid);
            for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
               //if (dlct->isNominalInTime()){   has_LCTshs=true; }
               if (dlct->isLooseInTime()){   has_LCTshs=true; }
               lctHMT_chamber[nlctHMT] = chamber;
               lctHMT_sr[nlctHMT] = sr;
               lctHMT_sector[nlctHMT] = sect;
               lctHMT_bits[nlctHMT] = dlct->bitsInTime();
               lctHMT_BX[nlctHMT] = dlct->getBX();
               lctHMT_ComparatorNHits[nlctHMT] = dlct->getComparatorNHits();
               lctHMT_WireNHits[nlctHMT] = dlct->getWireNHits();
               nlctHMT++;
            }
            for (auto elct = range_emulLCTshs.first; elct != range_emulLCTshs.second; elct++) {
               elctHMT_chamber[nelctHMT] = chamber;
               elctHMT_sr[nelctHMT] = sr;
               elctHMT_sector[nelctHMT] = sect;
               elctHMT_bits[nelctHMT] = elct->bitsInTime();
               elctHMT_BX[nelctHMT] = elct->getBX();
               elctHMT_ComparatorNHits[nelctHMT] = elct->getComparatorNHits();
               elctHMT_WireNHits[nelctHMT] = elct->getWireNHits();
               nelctHMT++;
            }
          }
        }
      }
    }
  }
  if (clusters.size()>=1) has_CSCclusters=true;
  if (debug_){
    std::cout<< "passL1 = "<<has_LCTshs<<std::endl;
    for(auto const& cls : clusters){
        std::cout<< "cls : size= "<<cls.size()<< " eta= "<<cls.eta()<<" phi= "<<cls.phi()<<"  time= "<<cls.time()<<" nStation10= "<<cls.nStation()<<std::endl;
    }   
    for(auto const& rechit :rechits){
        std::cout<< "rechit: " << (rechit) ;
    }
  }

  hasCluster = int(has_CSCclusters);
  passL1 = int(has_LCTshs);

  hmtTree->Fill();
  if (AsRecofilter) {
      return has_CSCclusters;
  }
  else {
    return has_LCTshs;
  }
  //  if(AsL1filter){
  //      if (has_LCTshs){
  //          hmtTree->Fill();
  //      }
  //      return has_LCTshs;
  //  } else if(AsRecofilter){
  //      if (has_CSCclusters){
  //          hmtTree->Fill();
  //      }
  //      return has_CSCclusters;
  //  }else
  //    return has_LCTshs;
}

void SimpleCSCshowerTreeMaker::reset(){
  nlctHMT = 0;
  nelctHMT = 0;
  runNum = 0;
  lumiNum = 0;
  eventNum = 0;
  isHLT_PPZeroBias = false;
  for ( int i = 0; i < HMTARRAYSIZE; i++) {
   lctHMT_chamber[i]   = 0;
   lctHMT_sr[i]        =0;
   lctHMT_sector[i]        =0;
   lctHMT_bits[i] =0;
   lctHMT_BX[i]=0;
   lctHMT_ComparatorNHits[i]=0;
   lctHMT_WireNHits[i]=0;
   elctHMT_chamber[i]   = 0;
   elctHMT_sr[i]        =0;
   elctHMT_sector[i]        =0;
   elctHMT_bits[i] =0;
   elctHMT_BX[i]=0;
   elctHMT_ComparatorNHits[i]=0;
   elctHMT_WireNHits[i]=0;
  }
 
  ncscRechits = 0;
  ncscRechitsChambers = 0;
  for ( int i = 0; i < CSCRECHITARRAYSIZE; i++) {
    cscRechitsPhi[i] = 0.0;
    cscRechitsEta[i] = 0.0;
    cscRechitsX[i] = 0.0;
    cscRechitsY[i] = 0.0;
    cscRechitsZ[i] = 0.0;
    cscRechitsE[i] = 0.0;
    cscRechitsTwire[i] = 0.0;
    cscRechitsTpeak[i] = 0.0;
    cscRechitsQuality[i] = 0.0;
    cscRechitsChannels[i] = 0;
    cscRechitsNStrips[i] = 0;
    cscRechitsHitWire[i] = 0;
    cscRechitsWGroupsBX[i] = 0;
    cscRechitsNWireGroups[i] = 0;
    cscRechitsDetId[i] = 0;
    cscRechitsStation[i] = 0;
    cscRechitsChamber[i] = 0;
    cscRechitsIChamber[i] = 0;
  }
  nMuons = 0;
  for(int i = 0; i < CSCRECHITARRAYSIZE; i++)
  {
    muonE[i] = 0.0;
    muonPt[i] = 0.0;
    muonEta[i] = 0.0;
    muonPhi[i] = 0.0;
    muonCharge[i] = -99;
    muonIsLoose[i] = false;
    muonIsMedium[i] = false;
    muonIsGlobal[i] = false;
  }
  nca4CSCcluster=0;
  for ( int i = 0; i < HMTARRAYSIZE; i++) {
        ca4CSCclusterSize[i] = 0; 
        ca4CSCclusterX[i] = 0.0;       
        ca4CSCclusterY[i] = 0.0;       
        ca4CSCclusterZ[i] = 0.0;       
        ca4CSCclusterEta[i] = 0.0;     
        ca4CSCclusterPhi[i] = 0.0;     
        ca4CSCclusterTpeak[i] = 0.0;   
        ca4CSCclusterWireTime[i] = 0.0;
        ca4CSCclusterTime[i] = 0.0;    
        ca4CSCclusterTimeSpread[i] = 0.0;  
        ca4CSCclusterME11_12[i] = 0;
        ca4CSCclusterNstation10[i] = 0;
        ca4CSCclusterAvgStation10[i] = 0.0;
  }

}


void
SimpleCSCshowerTreeMaker::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
  hmtTree->Branch("passL1",&passL1,"passL1/I");
  hmtTree->Branch("hasCluster",&hasCluster,"hasCluster/I");
  hmtTree->Branch("nlctHMT",&nlctHMT,"nlctHMT/I");
  hmtTree->Branch("runNum", &runNum, "runNum/i");
  hmtTree->Branch("lumiNum", &lumiNum, "lumiNum/i");
  hmtTree->Branch("eventNum", &eventNum, "eventNum/l");
  hmtTree->Branch("isHLT_PPZeroBias", &isHLT_PPZeroBias,"isHLT_PPZeroBias/O");
  hmtTree->Branch("lctHMT_chamber",&lctHMT_chamber,"lctHMT_chamber[nlctHMT]/I");
  hmtTree->Branch("lctHMT_sr",&lctHMT_sr,"lctHMT_sr[nlctHMT]/I");
  hmtTree->Branch("lctHMT_sector",&lctHMT_sector,"lctHMT_sector[nlctHMT]/I");
  hmtTree->Branch("lctHMT_bits",&lctHMT_bits,"lctHMT_bits[nlctHMT]/I");
  hmtTree->Branch("lctHMT_BX",&lctHMT_BX,"lctHMT_BX[nlctHMT]/I");
  hmtTree->Branch("lctHMT_ComparatorNHits",&lctHMT_ComparatorNHits,"lctHMT_ComparatorNHits[nlctHMT]/I");
  hmtTree->Branch("lctHMT_WireNHits",&lctHMT_WireNHits,"lctHMT_WireNHits[nlctHMT]/I");
  hmtTree->Branch("nelctHMT",&nelctHMT,"nelctHMT/I");
  hmtTree->Branch("elctHMT_chamber",&elctHMT_chamber,"elctHMT_chamber[nelctHMT]/I");
  hmtTree->Branch("elctHMT_sr",&elctHMT_sr,"elctHMT_sr[nelctHMT]/I");
  hmtTree->Branch("elctHMT_sector",&elctHMT_sector,"elctHMT_sector[nelctHMT]/I");
  hmtTree->Branch("elctHMT_bits",&elctHMT_bits,"elctHMT_bits[nelctHMT]/I");
  hmtTree->Branch("elctHMT_BX",&elctHMT_BX,"elctHMT_BX[nelctHMT]/I");
  hmtTree->Branch("elctHMT_ComparatorNHits",&elctHMT_ComparatorNHits,"elctHMT_ComparatorNHits[nelctHMT]/I");
  hmtTree->Branch("elctHMT_WireNHits",&elctHMT_WireNHits,"elctHMT_WireNHits[nelctHMT]/I");

  hmtTree->Branch("ncscRechits",&ncscRechits,"ncscRechits/I");
  hmtTree->Branch("ncscRechitsChambers",&ncscRechitsChambers,"ncscRechitsChambers/I");
  hmtTree->Branch("cscRechitsPhi",&cscRechitsPhi,"cscRechitsPhi[ncscRechits]/F");
  hmtTree->Branch("cscRechitsEta",&cscRechitsEta,"cscRechitsEta[ncscRechits]/F");
  hmtTree->Branch("cscRechitsX",&cscRechitsX,"cscRechitsX[ncscRechits]/F");
  hmtTree->Branch("cscRechitsY",&cscRechitsY,"cscRechitsY[ncscRechits]/F");
  hmtTree->Branch("cscRechitsZ",&cscRechitsZ,"cscRechitsZ[ncscRechits]/F");
  hmtTree->Branch("cscRechitsE",&cscRechitsE,"cscRechitsE[ncscRechits]/F");
  hmtTree->Branch("cscRechitsTpeak",&cscRechitsTpeak,"cscRechitsTpeak[ncscRechits]/F");
  hmtTree->Branch("cscRechitsTwire",&cscRechitsTwire,"cscRechitsTwire[ncscRechits]/F");
  hmtTree->Branch("cscRechitsQuality",&cscRechitsQuality,"cscRechitsQuality[ncscRechits]/I");
  hmtTree->Branch("cscRechitsChamber",&cscRechitsChamber,"cscRechitsChamber[ncscRechits]/I");
  hmtTree->Branch("cscRechitsIChamber",&cscRechitsIChamber,"cscRechitsIChamber[ncscRechits]/I");
  hmtTree->Branch("cscRechitsStation",&cscRechitsStation,"cscRechitsStation[ncscRechits]/I");


  hmtTree->Branch("nMuons", &nMuons,"nMuons/I");
  hmtTree->Branch("muonE", muonE,"muonE[nMuons]/F");
  hmtTree->Branch("muonPt", muonPt,"muonPt[nMuons]/F");
  hmtTree->Branch("muonEta", muonEta,"muonEta[nMuons]/F");
  hmtTree->Branch("muonPhi", muonPhi,"muonPhi[nMuons]/F");
  hmtTree->Branch("muonCharge", muonCharge, "muonCharge[nMuons]/I");
  hmtTree->Branch("muonIsLoose", muonIsLoose,"muonIsLoose[nMuons]/O");
  hmtTree->Branch("muonIsMedium", muonIsMedium,"muonIsMedium[nMuons]/O");
  hmtTree->Branch("muonIsGlobal", muonIsGlobal,"muonIsGlobal[nMuons]/O");

  hmtTree->Branch("nca4CSCcluster",&nca4CSCcluster          ,"nca4CSCcluster/I");
  hmtTree->Branch("ca4CSCclusterSize",&ca4CSCclusterSize    ,"ca4CSCclusterSize[nca4CSCcluster]/I");
  hmtTree->Branch("ca4CSCclusterX",&ca4CSCclusterX          ,"ca4CSCclusterX[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterY",&ca4CSCclusterY          ,"ca4CSCclusterY[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterZ",&ca4CSCclusterZ          ,"ca4CSCclusterZ[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterEta",&ca4CSCclusterEta        ,"ca4CSCclusterEta[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterPhi",&ca4CSCclusterPhi        ,"ca4CSCclusterPhi[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterTpeak",&ca4CSCclusterTpeak      ,"ca4CSCclusterTpeak[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterWireTime",&ca4CSCclusterWireTime   ,"ca4CSCclusterWireTime[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterTime",&ca4CSCclusterTime   ,"ca4CSCclusterTime[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterTimeSpread",&ca4CSCclusterTimeSpread   ,"ca4CSCclusterTimeSpread[nca4CSCcluster]/F");
  hmtTree->Branch("ca4CSCclusterME11_12",&ca4CSCclusterME11_12   ,"ca4CSCclusterME11_12[nca4CSCcluster]/I");
  hmtTree->Branch("ca4CSCclusterNstation10",&ca4CSCclusterNstation10   ,"ca4CSCclusterNstation10[nca4CSCcluster]/I");
  hmtTree->Branch("ca4CSCclusterAvgStation10",&ca4CSCclusterAvgStation10   ,"ca4CSCclusterAvgStation10[nca4CSCcluster]/F");


}

// ------------ method called when ending the processing of a run  ------------
/*
void
SimpleCSCshowerTreeMaker::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void
SimpleCSCshowerTreeMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
SimpleCSCshowerTreeMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void SimpleCSCshowerTreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(SimpleCSCshowerTreeMaker);
