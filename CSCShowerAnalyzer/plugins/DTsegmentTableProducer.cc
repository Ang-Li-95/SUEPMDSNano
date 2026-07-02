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
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


class DTsegmentTableProducer : public edm::global::EDProducer<> {
  public:
    DTsegmentTableProducer(const edm::ParameterSet &iConfig)
      :
      geometryToken_(esConsumes<DTGeometry, MuonGeometryRecord>()),
      inputToken_(consumes<DTRecSegment4DCollection>(iConfig.getParameter<edm::InputTag>("segmentLabel"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~DTsegmentTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("segmentLabel")->setComment("input dt segment collection");
      desc.add<std::string>("name","dtSegments")->setComment("name of the output collection");
      descriptions.add("dtSegmentsTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::ESGetToken<DTGeometry, MuonGeometryRecord> geometryToken_;
    edm::EDGetTokenT<DTRecSegment4DCollection> inputToken_;
    const std::string name_;
};


void DTsegmentTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  auto const& geo = iSetup.getData(geometryToken_);
  auto const& segments = iEvent.get(inputToken_);

  std::vector<float> dtSegmentsX,dtSegmentsY,dtSegmentsZ,dtSegmentsPhi,dtSegmentsEta;
  std::vector<int> dtSegmentsStation,dtSegmentsWheel;


  for (auto const& segment : segments) {

    LocalPoint localPosition = segment.localPosition();
    auto geoid = segment.geographicalId();
    DTChamberId dtdetid = DTChamberId(geoid);
    auto thischamber = geo.chamber(dtdetid);
    if (thischamber) {
      GlobalPoint globalPosition = thischamber->toGlobal(localPosition);

      dtSegmentsX.push_back( globalPosition.x());
      dtSegmentsY.push_back( globalPosition.y());
      dtSegmentsZ.push_back( globalPosition.z());
      dtSegmentsPhi.push_back( globalPosition.phi());
      dtSegmentsEta.push_back( globalPosition.eta());
      dtSegmentsWheel.push_back( dtdetid.wheel());
      dtSegmentsStation.push_back(  dtdetid.station());

    }
  }

  auto dtSegmentTab = std::make_unique<nanoaod::FlatTable>(segments.size(), name_, false, false);

  dtSegmentTab->addColumn<float>("X", dtSegmentsX, "dt segment X");
  dtSegmentTab->addColumn<float>("Y", dtSegmentsY, "dt segment Y");
  dtSegmentTab->addColumn<float>("Z", dtSegmentsZ, "dt segment Z");
  dtSegmentTab->addColumn<float>("Phi", dtSegmentsPhi, "dt segment Phi");
  dtSegmentTab->addColumn<float>("Eta", dtSegmentsEta, "dt segment Eta");
  dtSegmentTab->addColumn<int>("Station", dtSegmentsStation, "dt segment station");
  dtSegmentTab->addColumn<int>("Wheel", dtSegmentsWheel, "dt segment wheel");

  iEvent.put(std::move(dtSegmentTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DTsegmentTableProducer);

