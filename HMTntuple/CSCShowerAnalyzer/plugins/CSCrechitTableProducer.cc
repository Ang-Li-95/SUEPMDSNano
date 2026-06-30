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
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


class CSCrechitTableProducer : public edm::global::EDProducer<> {
  public:
    CSCrechitTableProducer(const edm::ParameterSet &iConfig)
      :
      geometryToken_(esConsumes<CSCGeometry, MuonGeometryRecord>()),
      inputToken_(consumes<CSCRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("recHitLabel"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~CSCrechitTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("recHitLabel")->setComment("input cscRechit collection");
      desc.add<std::string>("name","cscRechits")->setComment("name of the output collection");
      descriptions.add("cscRechitsTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> geometryToken_;
    edm::EDGetTokenT<CSCRecHit2DCollection> inputToken_;
    const std::string name_;
};


void CSCrechitTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  auto const& geo = iSetup.getData(geometryToken_);
  auto const& rechits = iEvent.get(inputToken_);

  std::vector<CSCDetId> unique_ids;

  std::vector<float> cscRechitsX,cscRechitsY,cscRechitsZ,cscRechitsPhi,cscRechitsEta,cscRechitsE,cscRechitsTpeak,cscRechitsTwire;
  std::vector<int> cscRechitsNStrips,cscRechitsHitWire,cscRechitsWGroupsBX,cscRechitsNWireGroups,cscRechitsQuality,cscRechitsChamber,cscRechitsIChamber,cscRechitsStation;;


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

      cscRechitsX.push_back( globalPosition.x());
      cscRechitsY.push_back( globalPosition.y());
      cscRechitsZ.push_back( globalPosition.z());
      cscRechitsPhi.push_back( globalPosition.phi());
      cscRechitsEta.push_back( globalPosition.eta());
      cscRechitsE.push_back( rechit.energyDepositedInLayer());//not saved
      cscRechitsTpeak.push_back( rechit.tpeak());
      cscRechitsTwire.push_back( rechit.wireTime());
      cscRechitsQuality.push_back( rechit.quality());
      if (CSCDetId::ring(detid) == 4)
        cscRechitsChamber.push_back( endcap * (CSCDetId::station(detid)*10 + 1));
      else
        cscRechitsChamber.push_back( endcap * (CSCDetId::station(detid)*10 + CSCDetId::ring(detid)));
      cscRechitsIChamber.push_back( CSCDetId::chamber(detid));
      cscRechitsStation.push_back( endcap *CSCDetId::station(detid));
	  cscRechitsNStrips.push_back(  rechit.nStrips());
	  cscRechitsHitWire.push_back(  rechit.hitWire());
	  cscRechitsWGroupsBX.push_back(  rechit.wgroupsBX());
	  cscRechitsNWireGroups.push_back(  rechit.nWireGroups());

    }
  }
  //ncscRechitsChambers = unique_ids.size();

  auto cscRechitTab = std::make_unique<nanoaod::FlatTable>(rechits.size(), name_, false, false);

  cscRechitTab->addColumn<float>("X", cscRechitsX, "csc rechit X");
  cscRechitTab->addColumn<float>("Y", cscRechitsY, "csc rechit Y");
  cscRechitTab->addColumn<float>("Z", cscRechitsZ, "csc rechit Z");
  cscRechitTab->addColumn<float>("Phi", cscRechitsPhi, "csc rechit Phi");
  cscRechitTab->addColumn<float>("Eta", cscRechitsEta, "csc rechit Eta");
  cscRechitTab->addColumn<float>("E", cscRechitsE, "csc rechit Energy deposited in layer");
  cscRechitTab->addColumn<float>("Tpeak", cscRechitsTpeak, "csc rechit time from cathode");
  cscRechitTab->addColumn<float>("Twire", cscRechitsTwire, "csc rechit time from anode");
  cscRechitTab->addColumn<int>("Quality", cscRechitsQuality, "csc rechit quality");
  cscRechitTab->addColumn<int>("Chamber", cscRechitsChamber, "csc rechit station-Ring");
  cscRechitTab->addColumn<int>("IChamber", cscRechitsIChamber, "csc rechit chamber in ring" );
  cscRechitTab->addColumn<int>("Station", cscRechitsStation, "csc rechit station");
  cscRechitTab->addColumn<int>("NStrips", cscRechitsNStrips, "csc rechit nstrips");
  cscRechitTab->addColumn<int>("WGroupsBX", cscRechitsWGroupsBX, "csc rechit wire group BX");
  cscRechitTab->addColumn<int>("HitWire",    cscRechitsHitWire, "csc rechit hit wire");
  cscRechitTab->addColumn<int>("NWireGroups", cscRechitsNWireGroups, "csc rechit n wire groups");


  iEvent.put(std::move(cscRechitTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCrechitTableProducer);

