#ifndef TNP_H
#define TNP_H

#include "helper.h"

// tnp variable output definition
//std::vector<std::string> TnP_variables = {
//  "nTnP_pair",
//  "TnP_mass",
//  "Tag_Idx",
//  "Probe_Idx",
  //"Tag_pt",
  //"Tag_eta",
  //"Tag_phi",
  //"Tag_mass",
  //"Tag_pdgId",
  //"Tag_wp",
  //"Probe_pt",
  //"Probe_eta",
  //"Probe_phi",
  //"Probe_mass",
  //"Probe_pdgId",
  //"Probe_wp"
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
//};

template <typename T>
auto makeLorentzVector( T &df , const std::string &col ) {

    using namespace ROOT::VecOps;
    
    auto makevector = [](
			     const RVec<float>& obj_pt,
			     const RVec<float>& obj_eta,
			     const RVec<float>& obj_phi,
			     const RVec<float>& obj_mass
			     )
    {
      std::vector<ROOT::Math::PtEtaPhiMVector> out;
      
      for (unsigned int i = 0 ; i < obj_pt.size(); i++ )
	out.emplace_back( Helper::makeLV<float>( obj_pt[i] , obj_eta[i] , obj_phi[i] , obj_mass[i] ) );
      
      return out;
    };
    
    return df.Define( col+"_4v" , makevector , {
	col+"_pt",
	col+"_eta",
	col+"_phi",
	col+"_mass"
      } );
}


template <typename T>
auto matching( T &df , Helper::config_t &cfg ) {

  std::string flavor = cfg.Flavor;
  std::string object = cfg.HLTobject;
  int Id = ( flavor == "Electron" ) ? 11 : 13;
    
  using namespace ROOT::VecOps;
  
  auto ismatched = [&cfg,Id](
			     const RVec<float>& lepton_eta,
			     const RVec<float>& lepton_phi,
			     const RVec<int>& obj_id,
			     const RVec<float>& obj_eta,
			     const RVec<float>& obj_phi
			     )
  {
    // Get indices of all possible combinations
    auto comb = Combinations( lepton_eta , obj_eta );
    const auto numComb = comb[0].size();
    RVec<int> ishltmatch(lepton_eta.size(), (cfg.isMC) ? 1 : 0 );
    RVec<int> isgenmatch(lepton_eta.size(), (cfg.isMC) ? 0 : 1 );
    
    int theId = Id;
    
    for (size_t i = 0 ; i < numComb ; i++) {
      const auto ilep = comb[0][i];
      const auto iobj = comb[1][i];
      
      if (cfg.isMC) if ( abs(obj_id[iobj]) != theId ) continue;
      
      const auto deltarS = pow(lepton_eta[ilep] - obj_eta[iobj] , 2) + pow(Helper::DeltaPhi(lepton_phi[ilep], obj_phi[iobj] ), 2);
      
      std::cout<<"Before : "<<ishltmatch[ilep]<<std::endl;
      
      if (cfg.isMC){ isgenmatch[ilep]  = (deltarS < cfg.minDeltaR) ? 1 : 0 ; continue; }
      if (!cfg.isMC){ ishltmatch[ilep] = (deltarS < cfg.minDeltaR) ? 1 : 0 ; continue; }
      
    }
    
    return std::make_tuple( ishltmatch , isgenmatch );
  };
  
  return df
    .Define( "matcher" , ismatched , {
	flavor+"_eta" ,
	flavor+"_phi" ,
	(cfg.isMC) ? "GenPart_pdgId" : flavor+"_charge" , // act as dummy for data
	object+"_eta",
	object+"_phi",
      }
      )
    .Define( flavor+"_isTrgObjMatched" , "std::get<0>(matcher)" )
    .Define( flavor+"_isGenMatched" , "std::get<1>(matcher)" )
    ;
}

// TnP WORKFLOW
// output the pair kinematic, mass, and idx
template <typename T>
auto tnpvector(T &df , Helper::config_t &cfg ) {

  // https://github.com/latinos/LatinoAnalysis/blob/master/NanoGardener/python/modules/addTnpTree.py

  std::string flavor = cfg.Flavor;
  std::string object = cfg.HLTobject;

  using namespace ROOT::VecOps;

  // lambda function
  auto maketnpvector  = [&cfg](
				       const std::vector<ROOT::Math::PtEtaPhiMVector> lepton_4v,
				       const RVec<int>& lepton_charge,
				       const RVec<int>& lepton_wp,
				       const RVec<int>& ismatch
				       )
  {
    std::vector<valuetagpair> masspair_vector;
    int npair;
    RVec<float> masspair;
    RVec<int> tag_idx, probe_idx;
    
    // tag loop
    for (unsigned int i = 0 ; i < lepton_4v.size(); i++ ){

      ROOT::Math::PtEtaPhiMVector tag = lepton_4v[i];
	
      // tag selection
      if ( !( tag.Pt() > cfg.kMinTagPt && abs(tag.Eta()) < cfg.kMaxTagEta ) ) continue;
      
      // match?
      //std::cout<<"matching : "<< ismatch[i] << std::endl;
      if ( !cfg.isMC && !ismatch[i] ) continue;

      // pass the tight wp?
      if ( lepton_wp[i] != cfg.kWPTag ) continue;
      
      // probe loop
      for (unsigned int j = 0 ; j < lepton_4v.size(); j++ ){

	ROOT::Math::PtEtaPhiMVector probe = lepton_4v[j];

	if (i == j) continue;
	
	// probe selection
	if ( !( probe.Pt() > cfg.kMinProbePt && abs(probe.Eta()) < cfg.kMaxProbeEta ) ) continue;
	
	float mass = (tag + probe).M();

	//std::cout<<"The invariant mass before passing is "<<mass<<std::endl;
	
	if ( ( mass > cfg.kMaxMass ) || ( mass < cfg.kMinMass ) ) continue;

	//std::cout<<"The invariant mass passing is "<<mass<<std::endl;
	
	if ( (lepton_charge[i]*lepton_charge[j]) >0 ) continue;

	if (masspair_vector.size()>1)
	  if ( std::any_of( masspair_vector.begin(), masspair_vector.end(), Helper::compare(mass) ) ) continue;

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
      //if (k>2) continue;

      // mass pair
      masspair.emplace_back( themasspair );
      
      // tag Idx
      tag_idx.emplace_back( thetag_idx );

      // probe Idx
      probe_idx.emplace_back( theprobe_idx );      
    } // end pair loop
    
    // output
    auto out = std::make_tuple( npair , masspair , tag_idx , probe_idx );

    return out;
  };

  // note, the MC genmatch is redundant (NOT USING IN CASE SAMPLE IS MC))
  std::string matcher = (!cfg.isMC) ? flavor+"_isTrgObjMatched" : flavor+"_isGenMatched" ;
  
  return df.Define(
		   "maketnpvector" ,
		   maketnpvector ,
		   {
		     flavor+"_4v",
		     flavor+"_charge",
		     flavor+"_cutBasedId",
		     matcher
		   }
		   )
    .Define("nTnP_pair","std::get<0>(maketnpvector)")
    .Define("TnP_mass","std::get<1>(maketnpvector)")
    .Define("Tag_Idx","std::get<2>(maketnpvector)")
    .Define("Probe_Idx","std::get<3>(maketnpvector)")
    ;
}

// tnp kinematics
template<typename T>
auto tnpkin( T &df , Helper::config_t &cfg , const std::string &tp ){

  std::string flavor = cfg.Flavor;
  std::string object = cfg.HLTobject;
  int Id = ( flavor != "Electron" ) ? 13 : 11;
  
  using namespace ROOT::VecOps;
  
  auto maketnpkin = [&cfg,Id](
			const std::vector<ROOT::Math::PtEtaPhiMVector> lepton_4v,
			const RVec<int>& tp_idx,
			const RVec<int>& lepton_charge,
			const RVec<int>& lepton_wp,
			const RVec<int>& lepton_ismatch
			){
    // need to press into iron... but we need to order the vector in descending order...
    
    RVec<float> tp_pt, tp_eta, tp_phi, tp_mass;
    RVec<int> tp_pdgid, tp_wp;
    RVec<int> tp_hltmatch(tp_idx.size(), (cfg.isMC) ? 0 : 1 );
    RVec<int> tp_genmatch(tp_idx.size(), (cfg.isMC) ? 1 : 0 );
    
    for ( unsigned int i =0 ; i < tp_idx.size() ; i++ ){
      
      int theIdx = tp_idx[i];
      int theId = Id;
      if ( lepton_charge[theIdx] < 0 ) theId = theId*(-1);
      
      tp_pt.emplace_back( lepton_4v[theIdx].Pt() );
      tp_eta.emplace_back( lepton_4v[theIdx].Eta() );
      tp_phi.emplace_back( lepton_4v[theIdx].Phi() );
      tp_mass.emplace_back( lepton_4v[theIdx].M() );
      tp_pdgid.emplace_back( theId );
      tp_wp.emplace_back( lepton_wp[theIdx] );
      if (cfg.isMC){
	tp_genmatch.emplace_back( lepton_ismatch[theIdx] );
      }
      else{
	tp_hltmatch.emplace_back( lepton_ismatch[theIdx] );
      }
    }
    
    return std::make_tuple( tp_pt, tp_eta, tp_phi, tp_mass, tp_pdgid, tp_wp, tp_hltmatch , tp_genmatch );
  };

  // the tag and probe variables
  //std::string matcher = (!cfg.isMC) ? flavor+"_isTrgObjMatched" : flavor+"_isGenMatched" ;

  /*
  std::vector<std::string> tnp_out = {
    tp+"_pt",
    tp+"_eta",
    tp+"_phi",
    tp+"_mass",
    tp+"_pdgId",
    tp+"_wp",
    matcher_out
  };
  */
  
  //cfg.outputVar = Helper::joinVector( cfg.outputVar , tnp_out  );

  std::string thekin = tp+"_kin";
  return df.Define(
		   thekin ,
		   maketnpkin ,
		   {
		     flavor+"_4v",
		     tp+"_Idx",
		     flavor+"_charge",
		     flavor+"_cutBasedId",
		     (cfg.isMC) ? flavor+"_isGenMatched" : flavor+"_isTrgObjMatched"
		   }
		   )
    .Define( tp+"_pt_v"              , "std::get<0>("+thekin+")" ).Define( tp+"_pt"              , tp+"_pt_v[0]"              )
    .Define( tp+"_eta_v"             , "std::get<1>("+thekin+")" ).Define( tp+"_eta"             , tp+"_eta_v[0]"             )
    .Define( tp+"_phi_v"             , "std::get<2>("+thekin+")" ).Define( tp+"_phi"             , tp+"_phi_v[0]"             )
    .Define( tp+"_mass_v"            , "std::get<3>("+thekin+")" ).Define( tp+"_mass"            , tp+"_mass_v[0]"            )
    .Define( tp+"_pdgId_v"           , "std::get<4>("+thekin+")" ).Define( tp+"_pdgId"           , tp+"_pdgId_v[0]"           )
    .Define( tp+"_wp_v"              , "std::get<5>("+thekin+")" ).Define( tp+"_wp"              , tp+"_wp_v[0]"              )
    .Define( tp+"_isTrgObjMatched_v" , "std::get<6>("+thekin+")" ).Define( tp+"_isTrgObjMatched" , tp+"_isTrgObjMatched_v[0]" )
    .Define( tp+"_isGenMatched_v"    , "std::get<7>("+thekin+")" ).Define( tp+"_isGenMatched"    , tp+"_isGenMatched_v[0]"    )
    ;
}

#endif
