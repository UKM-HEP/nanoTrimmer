#ifndef INPUT_H
#define INPUT_H

#include "../tnp.h"

template <typename T>
auto runningInput( T &df , Helper::config_t &cfg ){

  // *********************************************************************
  // *********************************************************************
  // What is the integrated luminosity you are working with ?
  cfg.Luminosity = 2330; // in pb^{-1}

  // What is the HLT trigger you used for your analysis ?
  cfg.HLT = "HLT_DoubleMu3 || HLT_DoubleMu3_Jpsi || HLT_DoubleMu3_LowMass || HLT_DoubleMu3_Quarkonium || HLT_Dimuon10_Jpsi_Barrel || HLT_Dimuon13_Jpsi_Barrel || HLT_Dimuon6_LowMass";
  
  // 
  // What is the type of lepton you are working with ?
  cfg.Flavor = "Muon";
  std::string Id = (cfg.Flavor == "Electron") ? "11" : "13";

  // What is the HLT object you are working with ?
  cfg.HLTobject = "TrigObj_jpsiu_runa";
  if (cfg.isMC) cfg.HLTobject = "GenPart";
  cfg.isjpsi = (cfg.HLTobject.find("TrigObj_jpsiu") != std::string::npos) ? true : false;
  std::cout<<"cfg.isjpsi : "<<cfg.isjpsi<<std::endl;
  
  // What is the MINIMUM transverse momentum for your Tag ?
  cfg.kMinTagPt = 6; // GeV

  // What is the MAXIMUM pseudorapidity for your Tag ?
  cfg.kMaxTagEta = 2.4;

  // What is the MINIMUM transverse momentum for your Probe ?
  cfg.kMinProbePt = 3; // GeV

  // What is the MAXIMUM pseudorapidity for your Probe ?
  cfg.kMaxProbeEta = 2.4;

  // What is the resonance cross section?
  // 7TeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSections
  // 8TeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
  // 7TeV J/Psi decay into mumu : http://cds.cern.ch/record/1279616/files/BPH-10-002-pas.pdf
  cfg.xsec = 289.1*1000; // pb

  // Number of event used in the dataset.
  // https://opendata.cern.ch/record/27
  cfg.numEvt = 62229649; 

  // What is the MAXIMUM mass of your favorite resonance?
  cfg.kMaxMass = 5.; // GeV

  // What is the MINIMUM mass of your favorite resonance?
  cfg.kMinMass = 2.; // GeV

  // What is the defined WP for Tag?
  // Electron : 0 fail ; 1 veto ; 2 loose ; 3 medium ; 4 tight
  // Muon     : 1 loose ; 2 soft ; 4 tight
  cfg.kWPTag = 7;

  // flat scale factor
  cfg.sf = 1.;

  // *********************************************************************
  // *********************************************************************

  // OFFLINE POST-PROCESSING
  // --> DONT TOUCH <--- ////////////////////////////////////////////////////////////////////
  df = df.Define( "xsec" , std::to_string(cfg.xsec) ).Define( "lumi" , std::to_string(cfg.Luminosity) ).Define( "nevent" , std::to_string(cfg.numEvt) ).Define( "sf" , std::to_string(cfg.sf) );
  df = (cfg.isMC) ? df.Define( "weights" , "(xsec/nevent)*lumi*evtWeight*sf" ) : df.Define( "weights" , "1" ) ; // produce total weights
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
    //.Filter( "nTnP_pair>=1" , "PRE-SELECTION: Selecting event containing at least one pair of tag and probe candidate" )
    .Filter( "abs(Tag_pdgId) == "+Id+" && Tag_pdgId+Probe_pdgId == 0" , "PRE-SELECTION: Selecting event containing Tag and Probe pair made up of 2 "+cfg.Flavor+" (pdgId = "+Id+")"     )
    .Filter( "Tag_wp == 7 ", "PRE-SELECTION: Selecting event containing Tag with working point tight" )
    ;
  
  return df;
}






















#endif
