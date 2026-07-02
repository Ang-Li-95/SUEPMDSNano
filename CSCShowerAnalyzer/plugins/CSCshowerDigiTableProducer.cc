#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/CSCDigi/interface/CSCShowerDigiCollection.h"



class CSCshowerDigiTableProducer : public edm::global::EDProducer<> {
  public:
    CSCshowerDigiTableProducer(const edm::ParameterSet &iConfig)
      :
      LCTShower_token_(consumes(iConfig.getParameter<edm::InputTag>("LCTShower"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~CSCshowerDigiTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("LCTShower")->setComment("input csc digi collection");
      desc.add<std::string>("name","cscShowerDigi")->setComment("name of the output collection");
      descriptions.add("cscShowerDigiTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::EDGetTokenT<CSCShowerDigiCollection> LCTShower_token_;
    const std::string name_;
};


void CSCshowerDigiTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  edm::Handle<CSCShowerDigiCollection> dataLCTshs;
  iEvent.getByToken(LCTShower_token_, dataLCTshs);

  std::vector<int> chambers,stationRings,sector,bits,bx,comparatorNHits,wireNHits,showerType;

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
            for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
               //if (dlct->isLooseInTime()){   has_LCTshs=true; }
               chambers.push_back( chamber);
               stationRings.push_back( sr);
               sector.push_back( sect);
               bits.push_back( dlct->bitsInTime());
               bx.push_back( dlct->getBX());
               comparatorNHits.push_back( dlct->getComparatorNHits());
               wireNHits.push_back( dlct->getWireNHits());
               showerType.push_back( dlct->getShowerType());
            }
          }
        }
      }
    }
  }
  
  auto cscShowerDigiTab = std::make_unique<nanoaod::FlatTable>(chambers.size(), name_, false, false);

  cscShowerDigiTab->addColumn<int>("chamber", chambers , "csc shower digi chamber");
  cscShowerDigiTab->addColumn<int>("sr", stationRings , "csc shower digi station-ring [1-17]");
  cscShowerDigiTab->addColumn<int>("sector", sector , "csc shower digi sector");
  cscShowerDigiTab->addColumn<uint16_t>("bits", bits , "csc shower digi shower bit");
  cscShowerDigiTab->addColumn<uint16_t>("bx", bx , "csc shower digi BX");
  cscShowerDigiTab->addColumn<uint16_t>("comparatorNHits", comparatorNHits , "csc shower digi comparator Nhits");
  cscShowerDigiTab->addColumn<uint16_t>("wireNHits", wireNHits , "csc shower digi wire Nhits");
  cscShowerDigiTab->addColumn<uint16_t>("showerType", showerType , "csc shower digi showerType");


  iEvent.put(std::move(cscShowerDigiTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCshowerDigiTableProducer);

