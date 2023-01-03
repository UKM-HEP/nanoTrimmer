#ifndef TNP_H
#define TNP_H

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

bool IsMatched( lep, trigObjCollection){
  double dRmin = 0.1;
  bool match = false;
  for trigObj in trigObjCollection:
  trigObj.mass = 0; 
  dR = lep.p4().DeltaR(trigObj.p4()) ;
  if dR < dRmin: match = True ;
  return;
}

// TnP WORKFLOW
template <typename T>
auto TnP(T &df , std::string &flavor , Helper::config_t &cfg ) {
  // https://github.com/latinos/LatinoAnalysis/blob/master/NanoGardener/python/modules/addTnpTree.py

  float kMaxMass = 140.;
  float kMinMass = 60.;

  // dataset specific variables
  cfg.outputVar = Helper::joinVector( cfg.outputVar , TnP_variables  );

  // lambda function
  auto maketnp  = [&cfg](
			 const RVec<float>& lepton_pt,
			 const RVec<float>& lepton_eta,
			 const RVec<float>& lepton_phi,
			 const RVec<float>& lepton_mass,
			 const RVec<int>& lepton_charge,
			 const RVec<int>& lepton_Id,
			 const RVec<int>& lepton_pdgId
			 )
		  {
		    // convert collection into TLorentz Vector collection
		    std::vector<std::pair<PtEtaPhiMVector,int>> tag_vector, probe_vector;
		    std::vector<std::pair<std::pair<PtEtaPhiMVector,int>,std::pair<PtEtaPhiMVector,int>>> pair_vector;
		    PtEtaPhiMVector v1, v2;
		    float mass;
		    
		    // make pair LorentzVector
		    for (int_i i = 0 ; i < lepton_pt.size(); i++ ){
		      v1.SetPtEtaPhiM( 0., 0., 0., 0. );
		      // tag selection
		      if ( lepton_pt[i] > 30. && lepton_Id[i] > 1 && lepton_eta[i] < 2.5 ) v1.SetPtEtaPhiM( lepton_pt[i] , lepton_eta[i] , lepton_phi[i] , lepton_mass[i] );
		      
		      // looping probe candidate
		      for (int_i j = 0 ; j < lepton_pt.size(); j++ ){
			v2.SetPtEtaPhiM( 0., 0., 0., 0. );
			mass = 0.;
			// probe selection
			if !( lepton_pt[j] > 30. && lepton_Id[j] > 1 && lepton_eta[j] < 2.5 ) continue;
			v2.SetPtEtaPhiM( lepton_pt[j] , lepton_eta[j] , lepton_phi[j] , lepton_mass[j] );
			//Probe_vector.push_back(std::make_pair( v , j ) );
			
			mass = (v1 + v2).M();
			
			if ( ( mass > kMaxMass ) || ( mass < kMinMass ) ) continue;
			
			if ( (lepton_charge[i]*lepton_charge[j]) ==0 ) continue;
			
			probe_vector.push_back( std::make_pair( v2 , j ) );
			
		      }
		      if ( probe_vector.size() == 1 ) pair_vector.push_back( std::make_pair( v1 , i ) , probe_vector[0] );
		      
		    } // make pair LorentzVector

		    
		    // making tuple
		    
		    
		  };
  
    // make probe LorentzVector
    for (int_i i = 0 ; i < lepton_pt.size(); i++ ){
      v.SetPtEtaPhiM( lepton_pt[i] , lepton_eta[i] , lepton_phi[i] , lepton_mass[i] );
      Probe_vector.push_back(std::make_pair( v , i ) );
    }
        
    // Get indices of all possible combinations
    auto comb = Combinations( Tag_vector , Probe_vector );
    const auto numComb = comb[0].size();

    for (size_t i = 0 ; i < numComb ; i++) {
      const auto vec1 = comb[0][i].first;
      const auto vec1_idx = comb[0][i].second;
      const auto vec2 = comb[1][i].first;
      const auto vec2_idx = comb[1][i].second;

      
      float mass = (probe.p4()+tag.p4()).M()
                if mass > self.kMaxMass or mass < self.kMinMass: continue    
  };


  if ( flavor == "Electron" ){
    return df.Define(
		     "theTnp" ,
		     maketnp ,
		     {
		       "Electron_pt",
		       "Electron_eta",
		       "Electron_phi",
		       "Electron_mass",
		       "Electron_charge",
		       "Electron_cutBasedId"
		     }
		     
		      );
  }
  else{
    return df.Define(;
  }
}

  
  

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
