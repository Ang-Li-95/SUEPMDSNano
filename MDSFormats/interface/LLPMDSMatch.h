#ifndef MDSFormats_LLPMDSMatch_h
#define MDSFormats_LLPMDSMatch_h

namespace suep {

  // Per-RecHit truth match to a long-lived-particle (gen pdgId 999999) decay.
  //
  // Stored as an edm::ValueMap<suep::LLPMDSMatch> keyed to the standard muon
  // RecHit collections (csc2DRecHits / dt1DRecHits / rpcRecHits), i.e. there is
  // one entry per RecHit, in the collection's order. RecHits that are not matched
  // to any LLP decay have llpIdx < 0 (all other fields then meaningless).
  //
  // POD struct on purpose: an edm::ValueMap of this type is schema-stable across
  // CMSSW releases (unlike nanoaod::FlatTable), so it can be written in the AODSIM
  // step (CMSSW_14_0_21) and read back in the NanoAOD step (CMSSW_15_0_2).
  struct LLPMDSMatch {
    int llpIdx = -1;    // index of the matched LLP in the event (-1 = unmatched)
    int pdgId = 0;      // pdgId of the LLP decay product this hit traces to
    int matchType = 0;  // bitmask: 1 = geometric, 2 = digisimlink
    int nSimHits = 0;   // number of LLP signal SimHits in this DetId
    float simX = 0.f;   // matched SimHit global position [cm]
    float simY = 0.f;
    float simZ = 0.f;
    float energy = 0.f;  // matched SimHit energy loss [GeV]
    float tof = 0.f;     // matched SimHit time of flight [ns]
    float dxy = -1.f;    // RecHit-SimHit local distance [cm] (-1 if no geometric SimHit)
  };

}  // namespace suep

#endif
