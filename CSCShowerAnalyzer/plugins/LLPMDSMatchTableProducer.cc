// LLPMDSMatchTableProducer
//
// Turns the edm::ValueMap<suep::LLPMDSMatch> produced in the AODSIM step
// (module 'llpMDSRecHitMatcher', keyed to csc2DRecHits / dt1DRecHits /
// rpcRecHits) into a NanoAOD FlatTable, as an *extension* of the matching MDS
// rechit table (same name, same row count and order) — adding the LLP truth
// columns next to the rechit position columns.
//
// Hardened form: it also consumes the rechit collection and looks the ValueMap up
// by (ProductID, index) via vm.get(recHits.id(), i). That validates that the
// ValueMap is keyed to *exactly* the rechit collection being tabulated (it throws
// on a ProductID mismatch or out-of-range index), rather than trusting positional
// alignment of the flat vm.get(i). Because the rechit type differs per detector,
// the producer is a template instantiated for CSC/DT/RPC.

#include <memory>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "SUEPMDSNano/MDSFormats/interface/LLPMDSMatch.h"

template <typename TColl>
class LLPMDSMatchTableProducerT : public edm::global::EDProducer<> {
public:
  explicit LLPMDSMatchTableProducerT(const edm::ParameterSet& iConfig)
      : recHitToken_(consumes<TColl>(iConfig.getParameter<edm::InputTag>("recHits"))),
        matchToken_(consumes<edm::ValueMap<suep::LLPMDSMatch>>(iConfig.getParameter<edm::InputTag>("src"))),
        name_(iConfig.getParameter<std::string>("name")),
        doc_(iConfig.getParameter<std::string>("doc")),
        extension_(iConfig.getParameter<bool>("extension")) {
    produces<nanoaod::FlatTable>();
  }
  ~LLPMDSMatchTableProducerT() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("recHits")->setComment("rechit collection this extends (e.g. csc2DRecHits)");
    desc.add<edm::InputTag>("src")->setComment("edm::ValueMap<suep::LLPMDSMatch> from llpMDSRecHitMatcher");
    desc.add<std::string>("name", "cscRechits")->setComment("must match the rechit table this extends");
    desc.add<std::string>("doc", "LLP truth match for muon rechits");
    desc.add<bool>("extension", true)->setComment("extend the same-named rechit table");
    descriptions.addWithDefaultLabel(desc);
  }

private:
  void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const override {
    edm::Handle<TColl> recHits;
    iEvent.getByToken(recHitToken_, recHits);
    const auto& vm = iEvent.get(matchToken_);

    // The ValueMap must be keyed to exactly this rechit collection. vm.get(id, i)
    // below would throw on a mismatch; check up front for a clearer message.
    if (!vm.contains(recHits.id()))
      throw cms::Exception("LogicError")
          << "LLPMDSMatchTableProducer('" << name_
          << "'): the ValueMap is not keyed to the provided 'recHits' collection (ProductID mismatch). "
          << "Point 'recHits' at the same collection llpMDSRecHitMatcher keyed its ValueMap to.";

    const size_t n = recHits->size();
    std::vector<int> llpIdx(n), pdgId(n), matchType(n), nSimHits(n);
    std::vector<float> simX(n), simY(n), simZ(n), simE(n), simTof(n), simDxy(n);
    for (size_t i = 0; i < n; ++i) {
      const suep::LLPMDSMatch& m = vm.get(recHits.id(), i);  // (ProductID, index): validated + range-checked
      llpIdx[i] = m.llpIdx;
      pdgId[i] = m.pdgId;
      matchType[i] = m.matchType;
      nSimHits[i] = m.nSimHits;
      simX[i] = m.simX;
      simY[i] = m.simY;
      simZ[i] = m.simZ;
      simE[i] = m.energy;
      simTof[i] = m.tof;
      simDxy[i] = m.dxy;
    }

    auto tab = std::make_unique<nanoaod::FlatTable>(n, name_, false, extension_);
    tab->addColumn<int>("llpIdx", llpIdx, "matched LLP index (-1 if unmatched)");
    tab->addColumn<int>("llpPdgId", pdgId, "pdgId of the matched LLP decay product");
    tab->addColumn<int>("llpMatchType", matchType, "match bitmask: 1=geometric, 2=digisimlink, 3=both");
    tab->addColumn<int>("llpNSimHits", nSimHits, "number of LLP signal SimHits in this DetId");
    tab->addColumn<float>("llpSimX", simX, "matched SimHit global x [cm]");
    tab->addColumn<float>("llpSimY", simY, "matched SimHit global y [cm]");
    tab->addColumn<float>("llpSimZ", simZ, "matched SimHit global z [cm]");
    tab->addColumn<float>("llpSimE", simE, "matched SimHit energy loss [GeV]");
    tab->addColumn<float>("llpSimTof", simTof, "matched SimHit time of flight [ns]");
    tab->addColumn<float>("llpSimDxy", simDxy, "RecHit-SimHit local distance [cm] (-1 if no geometric SimHit)");
    iEvent.put(std::move(tab));
  }

  const edm::EDGetTokenT<TColl> recHitToken_;
  const edm::EDGetTokenT<edm::ValueMap<suep::LLPMDSMatch>> matchToken_;
  const std::string name_;
  const std::string doc_;
  const bool extension_;
};

typedef LLPMDSMatchTableProducerT<CSCRecHit2DCollection> CSCLLPMDSMatchTableProducer;
typedef LLPMDSMatchTableProducerT<DTRecHitCollection> DTLLPMDSMatchTableProducer;
typedef LLPMDSMatchTableProducerT<RPCRecHitCollection> RPCLLPMDSMatchTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCLLPMDSMatchTableProducer);
DEFINE_FWK_MODULE(DTLLPMDSMatchTableProducer);
DEFINE_FWK_MODULE(RPCLLPMDSMatchTableProducer);
