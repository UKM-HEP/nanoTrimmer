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
    
    auto makevector = [&col](
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
  
  using namespace ROOT::VecOps;

  // no other flavor then electron and muon are usually used in HEP analysis
  int theId = ( flavor == "Electron" ) ? 11 : 13;
  
  auto ismatched = [&cfg,&theId](
				 const RVec<float>& lepton_eta,
				 const RVec<float>& lepton_phi,
				 //const RVec<int>& obj_id,
				 const RVec<float>& obj_eta,
				 const RVec<float>& obj_phi
				 )
  {
    // Get indices of all possible combinations
    auto comb = Combinations( lepton_eta , obj_eta );
    const auto numComb = comb[0].size();
    RVec<int> ismatch(lepton_eta.size(),0);
    
    for (size_t i = 0 ; i < numComb ; i++) {
      const auto ilep = comb[0][i];
      const auto iobj = comb[1][i];
      
      //if ( abs(obj_id[iobj]) != theId ) continue;
      const auto deltarS = pow(lepton_eta[ilep] - obj_eta[iobj] , 2) + pow(Helper::DeltaPhi(lepton_phi[ilep], obj_phi[iobj] ), 2);
      
      if ( deltarS < cfg.minDeltaR ){
	ismatch[ilep] = 1;
	continue;
      }
    }
    
    return ismatch;
  };

  std::string v_out = (object != "GenPart") ? flavor+"_is"+object+"_matched" : flavor+"_mcTruth" ;
  cfg.outputVar.emplace_back( v_out );
  
  return df
    .Define( v_out , ismatched , {
	flavor+"_eta" ,
	flavor+"_phi" ,
	//object+"_Id",
	object+"_eta",
	object+"_phi",
      }
      );   
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
  auto maketnpvector  = [&cfg,&flavor](
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
      if (!ismatch[i]) continue;

      // pass the tight wp?
      if ( lepton_wp[i] != cfg.kWPTag ) continue;
      
      // probe loop
      for (unsigned int j = 0 ; j < lepton_4v.size(); j++ ){

	ROOT::Math::PtEtaPhiMVector probe = lepton_4v[j];

	if (i == j) continue;
	
	// probe selection
	if ( !( probe.Pt() > cfg.kMinProbePt && abs(probe.Eta()) < cfg.kMaxProbeEta ) ) continue;
	
	float mass = (tag + probe).M();

	std::cout<<"The invariant mass before passing is "<<mass<<std::endl;
	
	if ( ( mass > cfg.kMaxMass ) || ( mass < cfg.kMinMass ) ) continue;

	std::cout<<"The invariant mass passing is "<<mass<<std::endl;
	
	if ( (lepton_charge[i]*lepton_charge[j]) >0 ) continue;

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

  // tnp specifics variables
  std::vector<std::string> tnp_out = {
    "nTnP_pair",
    "TnP_mass",
    "Tag_Idx",
    "Probe_Idx",
  };
  
  cfg.outputVar = Helper::joinVector( cfg.outputVar , tnp_out  );

  std::string matcher = (cfg.isMC) ? flavor+"_mcTruth" : flavor+"_is"+object+"_matched";
  
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

  using namespace ROOT::VecOps;
  
  auto maketnpkin = [&flavor](
			const std::vector<ROOT::Math::PtEtaPhiMVector> lepton_4v,
			const RVec<int>& tp_idx,
			const RVec<int>& lepton_charge,
			const RVec<int>& lepton_wp,
			const RVec<int>& lepton_ismatch
			){
    RVec<float> tp_pt, tp_eta, tp_phi, tp_mass;
    RVec<int> tp_pdgid, tp_wp, tp_match;
    
    int theId = ( flavor == "Electron" ) ? 11 : 13;
    
    for ( unsigned int i =0 ; i < tp_idx.size() ; i++ ){
      
      tp_pt.emplace_back( lepton_4v[i].Pt() );
      tp_eta.emplace_back( lepton_4v[i].Eta() );
      tp_phi.emplace_back( lepton_4v[i].Phi() );
      tp_mass.emplace_back( lepton_4v[i].M() );
      tp_pdgid.emplace_back( theId*(lepton_charge[i]) );
      tp_wp.emplace_back( lepton_wp[i] );
      tp_match.emplace_back( lepton_ismatch[i] );
    }
    
    return std::make_tuple( tp_pt, tp_eta, tp_phi, tp_mass, tp_pdgid, tp_wp, tp_match );
  };

  // the tag and probe variables
  std::string v_out = (object != "GenPart") ? tp+"_is"+object+"_matched" : tp+"_mcTruth" ;
  std::string matcher = (cfg.isMC) ? flavor+"_mcTruth" : flavor+"_is"+object+"_matched";

  std::vector<std::string> tnp_out = {
    tp+"_pt",
    tp+"_eta",
    tp+"_phi",
    tp+"_mass",
    tp+"_pdgId",
    tp+"_wp",
    v_out
  };
  
  cfg.outputVar = Helper::joinVector( cfg.outputVar , tnp_out  );
  
  return df.Define(
		   tp+"_kin" ,
		   maketnpkin ,
		   {
		     flavor+"_4v",
		     "Tag_Idx",
		     flavor+"_charge",
		     flavor+"_cutBasedId",
		     matcher
		   }
		   )
    .Define( tp+"_pt"    , "std::get<0>("+tp+"_kin)" )
    .Define( tp+"_eta"   , "std::get<1>("+tp+"_kin)" )
    .Define( tp+"_phi"   , "std::get<2>("+tp+"_kin)" )
    .Define( tp+"_mass"  , "std::get<3>("+tp+"_kin)" )
    .Define( tp+"_pdgId" , "std::get<4>("+tp+"_kin)" )
    .Define( tp+"_wp"    , "std::get<5>("+tp+"_kin)" )
    .Define( v_out       , "std::get<6>("+tp+"_kin)" )
    ;
}

#endif
