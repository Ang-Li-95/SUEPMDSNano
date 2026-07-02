#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/DTLayerId.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


class DTrechitTableProducer : public edm::global::EDProducer<> {
  public:
    DTrechitTableProducer(const edm::ParameterSet &iConfig)
      :
      geometryToken_(esConsumes<DTGeometry, MuonGeometryRecord>()),
      inputToken_(consumes<DTRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitLabel"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~DTrechitTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("recHitLabel")->setComment("input dtRechit collection");
      desc.add<std::string>("name","dtRecHits")->setComment("name of the output collection");
      descriptions.add("dtRechitsTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::ESGetToken<DTGeometry, MuonGeometryRecord> geometryToken_;
    edm::EDGetTokenT<DTRecHitCollection> inputToken_;
    const std::string name_;
};


void DTrechitTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  auto const& geo = iSetup.getData(geometryToken_);
  auto const& rechits = iEvent.get(inputToken_);


  std::vector<float> dtRechitsX,dtRechitsY,dtRechitsZ,dtRechitsPhi,dtRechitsEta;
  std::vector<int> dtRechitsLayer,dtRechitsSuperLayer,dtRechitsSector,dtRechitsStation,dtRechitsWheel;


  for (auto const& rechit : rechits) {

    LocalPoint recHitLocalPosition = rechit.localPosition();
    auto geoid = rechit.geographicalId();
    DTChamberId dtdetid = DTChamberId(geoid);
    DTLayerId  dtlayerid = DTLayerId(geoid);

    auto thischamber = geo.chamber(dtdetid);
    auto thislayer   = geo.layer(dtlayerid);

    if (thischamber) {

      if (thislayer){
          GlobalPoint globalPosition = thislayer->toGlobal(recHitLocalPosition);
          dtRechitsX.push_back( globalPosition.x());
          dtRechitsY.push_back( globalPosition.y());
          dtRechitsZ.push_back( globalPosition.z());
          dtRechitsPhi.push_back( globalPosition.phi());
          dtRechitsEta.push_back( globalPosition.eta());
          dtRechitsLayer.push_back( dtlayerid.layer());
          dtRechitsSuperLayer.push_back( dtlayerid.superlayer());
      }
      else{
          GlobalPoint globalPosition = thischamber->toGlobal(recHitLocalPosition);
          dtRechitsX.push_back( globalPosition.x());
          dtRechitsY.push_back( globalPosition.y());
          dtRechitsZ.push_back( globalPosition.z());
          dtRechitsPhi.push_back( globalPosition.phi());
          dtRechitsEta.push_back( globalPosition.eta());
          dtRechitsLayer.push_back( 0 );     //default value;
          dtRechitsSuperLayer.push_back( 0 );//default value
      }
      dtRechitsSector.push_back( dtdetid.sector());
      dtRechitsStation.push_back( dtdetid.station());
      dtRechitsWheel.push_back( dtdetid.wheel());

    }
  }

  auto dtRechitTab = std::make_unique<nanoaod::FlatTable>(rechits.size(), name_, false, false);

  dtRechitTab->addColumn<float>("X", dtRechitsX, "dt rechit X");
  dtRechitTab->addColumn<float>("Y", dtRechitsY, "dt rechit Y");
  dtRechitTab->addColumn<float>("Z", dtRechitsZ, "dt rechit Z");
  dtRechitTab->addColumn<float>("Phi", dtRechitsPhi, "dt rechit Phi");
  dtRechitTab->addColumn<float>("Eta", dtRechitsEta, "dt rechit Eta");
  dtRechitTab->addColumn<int>("Layer", dtRechitsLayer, "dt rechit layer");
  dtRechitTab->addColumn<int>("SuperLayer", dtRechitsSuperLayer, "dt rechit superlayer");
  dtRechitTab->addColumn<int>("Sector", dtRechitsSector, "dt rechit sector");
  dtRechitTab->addColumn<int>("Station", dtRechitsStation, "dt rechit station");
  dtRechitTab->addColumn<int>("Wheel", dtRechitsWheel, "dt rechit wheel");

  iEvent.put(std::move(dtRechitTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DTrechitTableProducer);

