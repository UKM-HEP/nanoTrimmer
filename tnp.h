#ifndef TNP_H
#define TNP_H

#include "helper.h"

// tnp variable output definition
std::vector<std::string> TnP_variables = {
  "nTnP_pair",
  "TnP_mass",
  "Tag_Idx",
  "Probe_Idx"
  //"mcTrue",
  //"weight",
  //"tag_Ele_trigMVA",
  //"event_met_pfmet",
  //"event_met_pfphi",
  //"pair_mass",
  //"Tag_pt",
  //"Tag_eta",
  //"Tag_phi",
  //"Tag_charge",
  //"Tag_pdgId",
  //"Probe_pt",
  //"Probe_eta",
  //"Probe_phi",
  //"Probe_charge",
  //"Probe_pdgId",
  //"Probe_sieie",
  //"Probe_eInvMinusPInv",
  //"Probe_dz",
  //"Probe_dxy",
  //"Probe_cutBased_Fall17_V1",
  //"Probe_mvaFall17V1Iso_WP90",
  //"Probe_pfRelIso03_all",
  //"Tag_cutBased_Fall17_V1",
  //"Probe_mvaTTH",
  //"Probe_lostHits",
  //"Probe_convVeto",
  //"Tag_sc_eta",
  //"Probe_sc_eta",
  //"Probe_3charge",
  //"nElectron",
  //"Electron_pt"
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

// assigning pdgid for easy identification
template<typename T>
auto makePdgId( T &df , Helper::config_t &cfg , const std::string &flavor ){

  using namespace ROOT::VecOps;

  int ID = ( flavor == "Electron" ) ? 11 : 12;
  std::string lep_pdgid = flavor+"_pdgId";
  
  std::cout<<"lep_pdgid = "<<lep_pdgid<<std::endl;
  cfg.outputVar.push_back(flavor+"_pdgId");
  
  auto pdgId = [&ID]( const RVec<int>& lepton_charge ) {
    RVec<int> out;
    for (auto& it : lepton_charge ) {
      ID = ( it < 0 ) ? ID*(-1) : ID;
      out.push_back(ID);
    }
    return out;
  };
  
  return df.Define( lep_pdgid , pdgId , { lep_pdgid } );
}

// TnP WORKFLOW
// output the pair kinematic, mass, and idx
template <typename T>
auto tnpvector(T &df , Helper::config_t &cfg , const std::string &flavor ) {

  // https://github.com/latinos/LatinoAnalysis/blob/master/NanoGardener/python/modules/addTnpTree.py

  using namespace ROOT::VecOps;

  // dataset specific variables
  cfg.outputVar = Helper::joinVector( cfg.outputVar , TnP_variables  );

  // lambda function
  auto maketnpvector  = [&cfg,flavor](
			 const RVec<float>& lepton_pt,
			 const RVec<float>& lepton_eta,
			 const RVec<float>& lepton_phi,
			 const RVec<float>& lepton_mass,
			 const RVec<int>& lepton_charge
			 )
  {
    std::vector<valuetagpair> masspair_vector;
    int npair;
    RVec<float> masspair;
    RVec<int> tag_idx, probe_idx;
    
    // tag loop
    for (unsigned int i = 0 ; i < lepton_pt.size(); i++ ){
      
      // tag selection
      if ( flavor == "Electron" ){
	// Electron
	if ( !( lepton_pt[i] > 25. && abs(lepton_eta[i]) < 2.5 ) ) continue;
      }
      else if ( flavor == "Muon" ){
	// Muon
	if ( !( lepton_pt[i] > 25. && abs(lepton_eta[i]) < 2.5 ) ) continue;
      }
      else{
	std::cout<<"ERROR, Dude, something wrong"<<std::endl;
      }
      
      ROOT::Math::PtEtaPhiMVector v1 = Helper::makeLV<float>( lepton_pt[i] , lepton_eta[i] , lepton_phi[i] , lepton_mass[i] );
      
      // probe loop
      for (unsigned int j = 0 ; j < lepton_pt.size(); j++ ){

	if (i == j) continue;
	
	// probe selection
	if ( flavor == "Electron" ){
	  // Electron
	  if ( !( lepton_pt[i] > 15. && abs(lepton_eta[i]) < 2.5 ) ) continue;
	}
	else if ( flavor == "Muon" ){
	  // Muon
	  if ( !( lepton_pt[i] > 15. && abs(lepton_eta[i]) < 2.5 ) ) continue;
	}
	else{
	  std::cout<<"ERROR, Dude, something wrong"<<std::endl;
	}
	
	ROOT::Math::PtEtaPhiMVector v2 = Helper::makeLV<float>( lepton_pt[j] , lepton_eta[j] , lepton_phi[j] , lepton_mass[j] );
	
	float mass = (v1 + v2).M();
	
	if ( ( mass > cfg.kMaxMass ) || ( mass < cfg.kMinMass ) ) continue;

	std::cout<<"The mass is "<<mass<<std::endl;
	
	if ( (lepton_charge[i]*lepton_charge[j]) >0 ) continue;

	masspair_vector.push_back( std::make_pair( mass , std::make_pair( i , j ) ) );
	
      } // end probe loop
      
    } // end tag loop
    
    // the first pair is closer to zmass
    masspair_vector = Helper::IndexbyZmass(masspair_vector);
    
    // unpacking masspair vector information
    npair = masspair_vector.size();
    for (int k = 0 ; k < npair ; k++ ){
      float themasspair = masspair_vector[k].first;
      int thetag_idx = masspair_vector[k].second.first;
      int theprobe_idx = masspair_vector[k].second.second;
      
      // Save the first, second and third pair of tag and probe
      if (k>2) continue;

      // mass pair
      masspair.push_back( themasspair );
      
      // tag Idx
      tag_idx.push_back( thetag_idx );

      // probe Idx
      probe_idx.push_back( theprobe_idx );      
    } // end pair loop
    
    // output
    auto out = std::make_tuple( npair , masspair , tag_idx , probe_idx );

    return out;
  };

  if ( flavor == "Electron" ){
    return df.Define(
		     "maketnpvector" ,
		     maketnpvector ,
		     {
		       "Electron_pt",
		       "Electron_eta",
		       "Electron_phi",
		       "Electron_mass",
		       "Electron_charge"
		     }
		     )
      .Define("nTnP_pair","std::get<0>(maketnpvector)")
      .Define("TnP_mass","std::get<1>(maketnpvector)")
      .Define("Tag_Idx","std::get<2>(maketnpvector)")
      .Define("Probe_Idx","std::get<3>(maketnpvector)")

      ;
  }
  else if ( flavor == "Muon"  ) {
    return df.Define(
                     "maketnpvector" ,
                     maketnpvector ,
                     {
                       "Muon_pt",
                       "Muon_eta",
                       "Muon_phi",
                       "Muon_mass",
                       "Muon_charge"
                     }
                     )
      .Define("nTnP_pair","std::get<0>(maketnpvector)")
      .Define("TnP_mass","std::get<1>(maketnpvector)")
      .Define("Tag_Idx","std::get<2>(maketnpvector)")
      .Define("Probe_Idx","std::get<3>(maketnpvector)")
      ;
    
  }
  else{
    std::cout<<"Error, no flavor detected"<<std::endl;
    return df;
  } 
}

/*

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
*/

#endif
