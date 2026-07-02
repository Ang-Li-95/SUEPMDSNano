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
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"


class CSCsegmentTableProducer : public edm::global::EDProducer<> {
  public:
    CSCsegmentTableProducer(const edm::ParameterSet &iConfig)
      :
      geometryToken_(esConsumes<CSCGeometry, MuonGeometryRecord>()),
      inputToken_(consumes<CSCSegmentCollection>(iConfig.getParameter<edm::InputTag>("segmentLabel"))),
      name_(iConfig.getParameter<std::string>("name"))
      {
      produces<nanoaod::FlatTable>(name_);
    }

    ~CSCsegmentTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("segmentLabel")->setComment("input csc segment collection");
      desc.add<std::string>("name","cscSegments")->setComment("name of the output collection");
      descriptions.add("cscSegmentsTable", desc);
    }

  private:
    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

    const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> geometryToken_;
    edm::EDGetTokenT<CSCSegmentCollection> inputToken_;
    const std::string name_;
};


void CSCsegmentTableProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
 
  auto const& geo = iSetup.getData(geometryToken_);
  auto const& segments = iEvent.get(inputToken_);

  std::vector<float> cscSegmentsX,cscSegmentsY,cscSegmentsZ,cscSegmentsPhi,cscSegmentsEta,cscSegmentsTime,cscSegmentsChi2,cscSegmentsDirectionX,cscSegmentsDirectionY,cscSegmentsDirectionZ;
  std::vector<int> cscSegmentsStation,cscSegmentsChamber,cscSegmentsIChamber;


  for (auto const& segment : segments) {

    LocalPoint localPosition = segment.localPosition();
    LocalVector localDirection = segment.localDirection();
    auto detid = segment.cscDetId();
    auto thischamber = geo.chamber(detid);
    int endcap = CSCDetId::endcap(detid) == 1 ? 1 : -1;
    if (thischamber) {
      GlobalPoint globalPosition = thischamber->toGlobal(localPosition);
      GlobalVector globalDirection = thischamber->toGlobal(localDirection);

      cscSegmentsX.push_back( globalPosition.x());
      cscSegmentsY.push_back( globalPosition.y());
      cscSegmentsZ.push_back( globalPosition.z());
      cscSegmentsPhi.push_back( globalPosition.phi());
      cscSegmentsEta.push_back( globalPosition.eta());
      cscSegmentsDirectionX.push_back( globalDirection.x());
      cscSegmentsDirectionY.push_back( globalDirection.y());
      cscSegmentsDirectionZ.push_back( globalDirection.z());
      cscSegmentsTime.push_back( segment.time());
      cscSegmentsChi2.push_back( segment.chi2());
      cscSegmentsIChamber.push_back( CSCDetId::chamber(detid));
      if (CSCDetId::ring(detid) == 4)
        cscSegmentsChamber.push_back( endcap * (CSCDetId::station(detid)*10 + 1));
      else
        cscSegmentsChamber.push_back( endcap * (CSCDetId::station(detid)*10 + CSCDetId::ring(detid)));
      cscSegmentsStation.push_back( endcap *CSCDetId::station(detid));

    }
  }

  auto cscSegmentTab = std::make_unique<nanoaod::FlatTable>(segments.size(), name_, false, false);

  cscSegmentTab->addColumn<float>("X", cscSegmentsX, "csc segment X");
  cscSegmentTab->addColumn<float>("Y", cscSegmentsY, "csc segment Y");
  cscSegmentTab->addColumn<float>("Z", cscSegmentsZ, "csc segment Z");
  cscSegmentTab->addColumn<float>("Phi", cscSegmentsPhi, "csc segment Phi");
  cscSegmentTab->addColumn<float>("Eta", cscSegmentsEta, "csc segment Eta");
  cscSegmentTab->addColumn<float>("DirectionX", cscSegmentsDirectionX, "csc segment direction X");
  cscSegmentTab->addColumn<float>("DirectionY", cscSegmentsDirectionY, "csc segment direction Y");
  cscSegmentTab->addColumn<float>("DirectionZ", cscSegmentsDirectionZ, "csc segment direction Z");
  cscSegmentTab->addColumn<float>("Time", cscSegmentsTime, "csc segment time from cathode");
  cscSegmentTab->addColumn<float>("Chi2", cscSegmentsChi2, "csc segment time from cathode");
  cscSegmentTab->addColumn<int>("Station", cscSegmentsStation, "csc segment station");
  cscSegmentTab->addColumn<int>("Chamber", cscSegmentsChamber, "csc segment station-Ring");
  cscSegmentTab->addColumn<int>("IChamber", cscSegmentsIChamber, "csc segment chamber");

  iEvent.put(std::move(cscSegmentTab), name_); 
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCsegmentTableProducer);

