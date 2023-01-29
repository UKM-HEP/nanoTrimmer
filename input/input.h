#ifndef INPUT_H
#define INPUT_H

#include "../tnp.h"

template <typename T>
auto runningInput( T &df , Helper::config_t &cfg ){

  // *********************************************************************
  // *********************************************************************
  // What is the integrated luminosity you are working with ?
  cfg.Luminosity = 890; // in pb^{-1}

  // What is the HLT trigger you used for your analysis ?
  cfg.HLT = "HLT_Ele27_WP80";

  // What is the type of lepton you are working with ?
  cfg.Flavor = "Electron";
  std::string Id = (cfg.Flavor == "Electron") ? "11" : "13";

  // What is the HLT object you are working with ?
  cfg.HLTobject = "TrigObj_ele";
  if (cfg.isMC) cfg.HLTobject = "GenPart";
  
  // What is the MINIMUM transverse momentum for your Tag ?
  cfg.kMinTagPt = 32; // GeV

  // What is the MAXIMUM pseudorapidity for your Tag ?
  cfg.kMaxTagEta = 2.4;

  // What is the MINIMUM transverse momentum for your Probe ?
  cfg.kMinProbePt = 25; // GeV

  // What is the MAXIMUM pseudorapidity for your Probe ?
  cfg.kMaxProbeEta = 2.4;

  // What is the resonance cross section?
  // 7TeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSections
  // 8TeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
  cfg.xsec =  1177.3; // pb

  // Number of event used in the dataset.
  // https://opendata.cern.ch/record/31
  cfg.numEvt = 41709195; 

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

  // OFFLINE POST-PROCESSING
  // --> DONT TOUCH <--- ////////////////////////////////////////////////////////////////////
  df = df.Define( "xsec" , std::to_string(cfg.xsec) ).Define( "lumi" , std::to_string(cfg.Luminosity) ).Define( "nevent" , std::to_string(cfg.numEvt) );
  df = (cfg.isMC) ? df.Define( "weights" , "(xsec/nevent)*lumi*evtWeight*1" ) : df.Define( "weights" , "1" ) ; // produce total weights
  df = df.Define("Muon_cutBasedId", "(Muon_looseId*1)+(Muon_softId*2)+(Muon_tightId*4)" );     // produce convenient cutbasedID for muon object
  df = makeLorentzVector( df , cfg.Flavor );                                                   // produce 4-vector for the flavor for ease of computation
  df = makeLorentzVector( df , cfg.HLTobject );                                                // produce 4-vector for the HLT object for ease of computation
  df = matching( df , cfg );                                                                   // object matching for DATA and MC
  df = tnpvector( df , cfg );                                                                  // selecting candidate for TAG and PROBE with SELECTION (check the code tnp.h))
  df = tnpkin( df , cfg , "Tag" );                                                             // saving the TAG kinematics: pt, eta, phi, mass, pdgId, matching information
  df = tnpkin( df , cfg , "Probe" );                                                           // saving the PROBE kinematics: pt, eta, phi, mass, pdgId, matching information
  df = df.Define( "mcTrue" , (cfg.isMC) ? "Tag_isGenMatched*Probe_isGenMatched>0" : "1" );     // produce mcTrue variable
  df = df.Define( "pair_mass" , "TnP_mass[0]" );
  // --> DONT TOUCH <--- ////////////////////////////////////////////////////////////////////
  
  // OFFLINE PRE-SELECTION
  df = df
    .Filter( cfg.HLT+"==1" , "PRE-SELECTION: Passing "+ cfg.HLT +" trigger selection" )
    .Filter( "!(abs(Tag_eta)>= 1.4442 && abs(Tag_eta)<=1.566)" , "PRE-SELECTION: Selecting event containing Tag candidates well covered inside the detector" )
    .Filter( "abs(Tag_pdgId) == "+Id+" && Tag_pdgId+Probe_pdgId == 0" , "PRE-SELECTION: Selecting event containing Tag and Probe pair made up of 2 "+cfg.Flavor     )
    .Filter( "Tag_wp == 4 ", "PRE-SELECTION: Selecting event containing Tag with working point tight" )
    ;
  
  return df;
}






















#endif
