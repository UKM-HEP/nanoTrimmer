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

  if (cfg.isMC) cfg.HLTobject = "GenPart";
  
  df = df.Filter( cfg.HLT+"==1" , "Passing "+ cfg.HLT +" trigger selection" );  
  df = makeLorentzVector( df , cfg.Flavor );
  df = makeLorentzVector( df , cfg.HLTobject );

  // object matching for data and mc
  df = matching( df , cfg );
  
  df = tnpvector( df , cfg );

  df = tnpkin( df , cfg , "Tag" );
  df = tnpkin( df , cfg , "Probe" );
  
  return df;
}






















#endif
