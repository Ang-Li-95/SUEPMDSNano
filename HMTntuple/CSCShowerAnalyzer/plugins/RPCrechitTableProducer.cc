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
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"

#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


class RPCrechitTableProducer : public edm::global::EDProducer<> {
  public:
    RPCrechitTableProducer(const edm::ParameterSet &iConfig)
      :
      geometryToken_(esConsumes<RPCGeometry, MuonGeometryRecord>()),
      inputToken_(consumes<RPCRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitLabel"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~RPCrechitTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("recHitLabel")->setComment("input rpcRechit collection");
      desc.add<std::string>("name","rpcRecHits")->setComment("name of the output collection");
      descriptions.add("rpcRechitsTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::ESGetToken<RPCGeometry, MuonGeometryRecord> geometryToken_;
    edm::EDGetTokenT<RPCRecHitCollection> inputToken_;
    const std::string name_;
};


void RPCrechitTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  auto const& geo = iSetup.getData(geometryToken_);
  auto const& rechits = iEvent.get(inputToken_);


  std::vector<float> rpcRechitsX,rpcRechitsY,rpcRechitsZ,rpcRechitsPhi,rpcRechitsEta,rpcRechitsTime,rpcRechitsTimeError;
  std::vector<int> rpcRechitsBx,rpcRechitsRegion,rpcRechitsRing,rpcRechitsLayer,rpcRechitsStation,rpcRechitsSector; 


  for (auto const& rechit : rechits) {

    LocalPoint recHitLocalPosition = rechit.localPosition();
    auto geoid = rechit.geographicalId();
    RPCDetId rpcdetid = RPCDetId(geoid);
    auto thischamber = geo.chamber(rpcdetid);

    if (thischamber) {

      GlobalPoint globalPosition = thischamber->toGlobal(recHitLocalPosition);
      rpcRechitsX.push_back( globalPosition.x());
      rpcRechitsY.push_back( globalPosition.y());
      rpcRechitsZ.push_back( globalPosition.z());
      rpcRechitsPhi.push_back( globalPosition.phi());
      rpcRechitsEta.push_back( globalPosition.eta());
      rpcRechitsTime.push_back( rechit.time() );     
      rpcRechitsTimeError.push_back( rechit.timeError());
      rpcRechitsBx.push_back( rechit.BunchX() );
      rpcRechitsRegion.push_back( rpcdetid.region());
      rpcRechitsRing.push_back( rpcdetid.ring());
      rpcRechitsLayer.push_back( rpcdetid.layer());
      rpcRechitsStation.push_back( rpcdetid.station());
      rpcRechitsSector.push_back( rpcdetid.sector());

    }
  }

  auto rpcRechitTab = std::make_unique<nanoaod::FlatTable>(rechits.size(), name_, false, false);

  rpcRechitTab->addColumn<float>("X", rpcRechitsX, "rpc rechit X");
  rpcRechitTab->addColumn<float>("Y", rpcRechitsY, "rpc rechit Y");
  rpcRechitTab->addColumn<float>("Z", rpcRechitsZ, "rpc rechit Z");
  rpcRechitTab->addColumn<float>("Phi", rpcRechitsPhi, "rpc rechit Phi");
  rpcRechitTab->addColumn<float>("Eta", rpcRechitsEta, "rpc rechit Eta");
  rpcRechitTab->addColumn<float>("Time", rpcRechitsTime, "rpc rechit Time");
  rpcRechitTab->addColumn<float>("TimeError", rpcRechitsTimeError, "rpc rechit Time Error");
  rpcRechitTab->addColumn<int>("Bx", rpcRechitsBx, "rpc rechit layer");
  rpcRechitTab->addColumn<int>("Region", rpcRechitsRegion, "rpc rechit region");
  rpcRechitTab->addColumn<int>("Ring", rpcRechitsRing, "rpc rechit ring");
  rpcRechitTab->addColumn<int>("Layer", rpcRechitsLayer, "rpc rechit layer");
  rpcRechitTab->addColumn<int>("Station", rpcRechitsStation, "rpc rechit station");
  rpcRechitTab->addColumn<int>("Sector", rpcRechitsSector, "rpc rechit sector");

  iEvent.put(std::move(rpcRechitTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RPCrechitTableProducer);

