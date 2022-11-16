#ifndef TRIM_H
#define TRIM_H
#include "helper.h"

// tnp variable output definition
std::vector<std::string> TnP_variables = {
  "mcTrue",
  "weight",
  "tag_Ele_trigMVA",
  "event_met_pfmet",
  "event_met_pfphi",
  "pair_mass",
  "Tag_pt",
  "Tag_eta",
  "Tag_phi",
  "Tag_charge",
  "Probe_pt",
  "Probe_eta",
  "Probe_phi",
  "Probe_charge",
  "Probe_sieie",
  "Probe_eInvMinusPInv",
  "Probe_dz",
  "Probe_dxy",
  "Probe_cutBased_Fall17_V1",
  "Probe_mvaFall17V1Iso_WP90",
  "Probe_pfRelIso03_all",
  "Tag_cutBased_Fall17_V1",
  "Probe_mvaTTH",
  "Probe_lostHits",
  "Probe_convVeto",
  "Tag_sc_eta",
  "Probe_sc_eta",
  "Probe_3charge",
  "nElectron",
  "Electron_pt"
  //"Probe_miniPFRelIso_all",
  //"Probe_r9",
  //"Probe_sip3d",
  //"Probe_mvaFall17V2noIso",
  //"Probe_jetBTagDeepFlavB",
  //"Probe_jetPtRelv2",
  //"Probe_jetIdx",
  //"nJet",
  //"Jet_pt"
};

// TnP WORKFLOW
template <typename T>
auto TnP(T &df , Helper::config_t &cfg) {
  // https://github.com/latinos/LatinoAnalysis/blob/master/NanoGardener/python/modules/addTnpTree.py

  // dataset specific variables
  cfg.outputVar = TnP_variables;
  if ( cfg.year == "2016" ) {
    cfg.outputVar.push_back("Probe_mvaSpring16GP_WP90");
    cfg.outputVar.push_back("Tag_mvaSpring16GP");
    cfg.outputVar.push_back("Probe_cutBased_HLTPreSel");
  }

  // Tag pt cuts are already at 2 GeV above trigger threshold, but no eta cut applied for 2016
  std::string tagCut="1==1";
  if ( cfg.year == "2016" ) tagCut="abs(Tag_eta)<2.07";

  auto df1 = df
    .Filter( "abs(Tag_pdgId)!=13 && abs(Probe_pdgId)!=13"      , " --> Tag and Probe are electron"      )
    .Filter( "abs(Probe_eta)<2.5"                              , " --> Probe candidate skim"            ) 
    .Filter( "!(abs(Tag_eta)>= 1.4442 && abs(Tag_eta)<=1.566)" , " --> Tag candidate eta skim"          )
    .Filter( tagCut                                            , " --> Tag candidate trig skim : "+tagCut )
    ;

  // dataset specific
  auto df2 = df1
    .Define( "mcTrue" , (cfg.isMC) ? "Tag_isGenMatched*Probe_isGenMatched>0" : "1" )
    .Define( "weight" , (cfg.isMC) ? "baseW*genWeight*puWeight" : "1"                  )
    ;
  auto df3 = (cfg.isMC) ? df2 : df2.Filter( "TnP_trigger==1" , " --> data is matched to HLT filter" );

  // Define variables
  auto df_out = df3
    .Define( "tag_Ele_trigMVA" , "Tag_mvaFall17V1Iso"         )
    .Define( "event_met_pfmet" , "PuppiMET_pt"                )
    .Define( "event_met_pfphi" , "PuppiMET_phi"               )
    .Define( "pair_mass"       , "TnP_mass"                   )
    .Define( "Tag_sc_eta"      , "Tag_deltaEtaSC+Tag_eta"     )
    .Define( "Probe_sc_eta"    , "Probe_deltaEtaSC+Probe_eta" )
    .Define( "Probe_3charge"   , "Probe_tightCharge==2"       )
    ;
  
  return df_out;
}

#endif
