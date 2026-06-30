// MDSSimpleFlatTableProducerPlugins
//
// Standalone instantiations of the NanoAOD SimpleFlatTableProducer template for
// the muon-system types used by the MDS NanoAOD. These were previously added
// directly to PhysicsTools/NanoAOD/plugins/SimpleFlatTableProducerPlugins.cc (a
// cherry-pick), which required checking out and modifying the base CMSSW package.
//
// Instead they live here and simply include the template header from the release,
// so there is NO dependency on a modified PhysicsTools/NanoAOD — the plugins are
// registered by this package's library. (If a future CMSSW release ships these
// instantiations itself, drop this file to avoid a duplicate plugin name.)
//
// SimpleMuonRecHitClusterFlatTableProducer is the one custom_mds_cff uses for the
// cscMDS/dtMDS rechit-cluster tables; the L1 shower ones are kept for parity.

#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"
typedef SimpleFlatTableProducer<reco::MuonRecHitCluster> SimpleMuonRecHitClusterFlatTableProducer;

#include "DataFormats/L1Trigger/interface/MuonShower.h"
typedef BXVectorSimpleFlatTableProducer<l1t::MuonShower> SimpleTriggerL1MuonShowerFlatTableProducer;

#include "DataFormats/L1TMuon/interface/RegionalMuonShower.h"
typedef BXVectorSimpleFlatTableProducer<l1t::RegionalMuonShower> SimpleTriggerL1RegionalMuonShowerFlatTableProducer;

DEFINE_FWK_MODULE(SimpleMuonRecHitClusterFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1MuonShowerFlatTableProducer);
DEFINE_FWK_MODULE(SimpleTriggerL1RegionalMuonShowerFlatTableProducer);
