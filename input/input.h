#ifndef INPUT_H
#define INPUT_H

template <typename T>
auto runningInput( T &df , Helper::config_t &cfg ){

  // *********************************************************************
  // *********************************************************************
  // What is the integrated luminosity you are working with?
  cfg.Luminosity = 3000.; // in pb^{-1}

  // What is the HLT trigger you used for your analysis?
  cfg.HLT = "HLT_IsoMu30_eta2p1";

  // What is the type of lepton you are working with?
  cfg.Flavor = "Electron";

  // What is the HLT object you are working with?
  cfg.HLTobject = "TrigObj_ele";

  // What is the MAXIMUM mass of your favorite resonance?
  cfg.kMaxMass = 2.; // GeV

  // What is the MINIMUM mass of your favorite resonance?
  cfg.kMinMass = 5.; // GeV
  // *********************************************************************
  // *********************************************************************

 
  df = df.Filter( cfg.HLT+"==1" , "Passing "+HLT+" trigger selection" );
  
  df = makeLorentzVector( df , cfg.Flavor );
  
  if (!cfg.isMC) df = makeLorentzVector( df , cfg.HLTobject );
  
  df = matching( df , cfg );



  return df;
}






















#endif
