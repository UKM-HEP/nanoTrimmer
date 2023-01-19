#ifndef INPUT_H
#define INPUT_H

#include "../tnp.h"

template <typename T>
auto runningInput( T &df , Helper::config_t &cfg ){

  // *********************************************************************
  // *********************************************************************
  // What is the integrated luminosity you are working with ?
  cfg.Luminosity = 3000.; // in pb^{-1}

  // What is the HLT trigger you used for your analysis ?
  cfg.HLT = "HLT_Ele27_WP80";

  // What is the type of lepton you are working with ?
  cfg.Flavor = "Electron";

  // What is the HLT object you are working with ?
  cfg.HLTobject = "TrigObj_ele";

  // What is the MINIMUM transverse momentum for your Tag ?
  cfg.kMinTagPt = 30; // GeV

  // What is the MAXIMUM pseudorapidity for your Tag ?
  cfg.kMaxTagEta = 2.4;

  // What is the MINIMUM transverse momentum for your Probe ?
  cfg.kMinProbePt = 25; // GeV

  // What is the MAXIMUM pseudorapidity for your Probe ?
  cfg.kMaxProbeEta = 2.4;

  // What is the MAXIMUM mass of your favorite resonance?
  cfg.kMaxMass = 120.; // GeV

  // What is the MINIMUM mass of your favorite resonance?
  cfg.kMinMass = 60.; // GeV

  // What is the defined WP for Tag?
  // Electron : 0 fail ; 1 veto ; 2 loose ; 3 medium ; 4 tight
  // Muon     : 1 loose ; 2 soft ; 4 tight
  cfg.kWPTag = 4;

  // *********************************************************************
  // *********************************************************************

  // Global variable processing
  if (cfg.isMC) cfg.HLTobject = "GenPart";
  df = df.Define( "weight" , (cfg.isMC) ? "Xsec*evtWeight*1" : "1" );
  df = df.Define("Muon_cutBasedId", "(Muon_looseId*1)+(Muon_softId*2)+(Muon_tightId*4)" );
  
  // object matching for data and mc
  // Offline post-processing
  df = makeLorentzVector( df , cfg.Flavor );      // produce 4 vector for the flavor for ease of computation
  df = makeLorentzVector( df , cfg.HLTobject );   // produce 4 vector for the HLT object for ease of computation
  df = matching( df , cfg );                      // object matching for DATA and MC
  df = tnpvector( df , cfg );                     // selecting candidate for TAG and PROBE
  df = tnpkin( df , cfg , "Tag" );                // saving the TAG kinematics: pt, eta, phi, mass, pdgId, matching information
  df = tnpkin( df , cfg , "Probe" );              // saving the PROBE kinematics: pt, eta, phi, mass, pdgId, matching information
  df = df.Define( "mcTrue" , (cfg.isMC) ? "Tag_isGenMatched*Probe_isGenMatched>0" : "1" ); //

  df = df
    .Filter( cfg.HLT+"==1" , "Passing "+ cfg.HLT +" trigger selection" );
    //.Filter( "!(abs(Tag_eta)>= 1.4442 && abs(Tag_eta)<=1.566)" , " --> Selecting event containing Tag candidates well covered inside the detector" );
  
  /*
  // TAG and PROBE selection
  //std::string Id = (cfg.Flavor == "Electron" ) ? "11" : "13"; // electron and muon are the stable lepton seen by CMS detector.
  df = df
    .Filter( cfg.HLT+"==1" , "Passing "+ cfg.HLT +" trigger selection" )
    //.Filter( "abs(Tag_pdgId)!="+Id+" && abs(Probe_pdgId)!="+Id , " --> Selecting event containing Tag and Probe pair made up of 2 "+cfg.Flavor     )
    //.Filter( "Tag_pt>"+cfg.kMinTagPt                           , " --> Selecting event cintaining Tag candidates with Pt > "+cfg.kMinTagPt         )
    //.Filter( "abs(Tag_eta)<"+cfg.kMaxTagEta                    , " --> Selecting event containing Tag candidates with Eta < "+cfg.kMaxTagEta       )
    .Filter( "!(abs(Tag_eta)>= 1.4442 && abs(Tag_eta)<=1.566)" , " --> Selecting event containing Tag candidates well covered inside the detector" )
    //.Filter( "Probe_pt>"+cfg.kMinProbePt                       , " --> Selecting event containing Probe candidates with Pt > "+cfg.kMinProbePt     )
    //.Filter( "abs(Probe_eta)<"+cfg.kMaxProbeEta                , " --> Selecting event containing Probe candidates with Eta > "cfg.kMaxProbeEta    )
    ;
  */
  
  return df;
}






















#endif
