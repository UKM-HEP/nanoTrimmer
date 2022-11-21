#include "helper.h"

// trimming, variables to be saved
std::vector<std::string> save_branches = {
  "nJet",
  "Jet_pt",
  "Jet_eta",
  "Jet_phi",
  "Jet_puId",
  "nElectron",
  "Electron_pt",
  "Electron_eta",
  "Electron_phi",
  "Electron_charge",
  "Electron_mass",
  "Electron_cutBasedId",
  "Electron_pfRelIso03_all",
  "Electron_jetIdx",
  "Electron_genPartIdx",
  "Electron_dEtaIn",
  "Electron_dPhiIn",
  "Electron_sigmaIEtaIEta",
  "Electron_HoE",
  "Electron_fbrem",
  "Electron_EoP_In",
  "Electron_IoEIoP_In",
  "Electron_Nmisshits",
  "nMuon",
  "Muon_pt",
  "Muon_eta",
  "Muon_phi",
  "Muon_mass",
  "Muon_charge",
  "Muon_pfRelIso03_all",
  "Muon_pfRelIso04_all",
  "Muon_tightId",
  "Muon_softId",
  "Muon_jetIdx",
  "Muon_genPartIdx",
  "Xsec",
  "evtWeight",
  "HLT_DoubleMu7",
  "HLT_Mu13_Mu8",
  "HLT_Mu17_Mu8HLT_Ele17_CaloTrk_Ele8_CaloTrk",
  "HLT_TripleEle10_CaloIdL_TrkIdVL",
  "HLT_Ele17_CaloTrk_Ele8_CaloTrk",
  "HLT_Ele15_Ele8_Ele5_CaloIdL_TrkIdVL",
  "HLT_Dimuon10_Jpsi_Barrel"
};


int main(int argc, char **argv) {

  //may return 0 when not able to detect   
  const auto processor_count = std::thread::hardware_concurrency();

  std::cout << "ncpu detected : " << processor_count << ", using it all!" << std::endl;
  ROOT::EnableImplicitMT(processor_count);

  if(argc != 3) {
        std::cout << "Use executable with following arguments: ./trim input output " << std::endl;
        return -1;
  }

  // start time
  TStopwatch time;
  time.Start();

  // characterize configuration
  Helper::config_t mycfg;
  mycfg.input  = argv[1];
  mycfg.output = argv[2];
  mycfg.isMC   = ( mycfg.input.find("Run") != std::string::npos || mycfg.input.find("JPsi") != std::string::npos ) ? false : true;

  // filelist
  std::ifstream file(mycfg.input);
  std::string str;
  while (std::getline(file, str)) { mycfg.infiles.push_back(str); }

  std::cout << ">>> Process input: " << mycfg.input << std::endl;
  std::cout << ">>> Process output: " << mycfg.output << std::endl;
  for(auto ifile : mycfg.infiles) std::cout << ifile <<std::endl;

  ROOT::RDataFrame df_( "aod2nanoaod/Events" , mycfg.infiles);
  auto df = ROOT::RDF::RNode(df_);

  // Skimming
  // event-level filter
  df = df.Filter( "nJet>0 && nJet<5"       , "remove events with number of jet >5"      );
  df = df.Filter( "nElectron>0 && nElectron<7"  , "remove events with number of electron >7" );
  df = df.Filter( "nMuon>0 && nMuon<7"      , "remove events with number of muon >7"     );

  // particle-level filter
  df = df.Define( "Jet_pt1" , "Jet_pt[0]" ).Define( "Jet_pt2" , "Jet_pt[1]" );
  df = df.Define( "Electron_pt1" , "Electron_pt[0]" ).Define( "Electron_pt2" , "Electron_pt[1]");
  df = df.Define( "Muon_pt1" , "Muon_pt[0]" ).Define( "Muon_pt2" , "Muon_pt[1]" );

  df = df.Filter( "Jet_pt1>20" , "events with jet1 pt > 20 GeV" );
  df = df.Filter( "Jet_pt2>20" , "events with jet2 pt > 20 GeV" );
  df = df.Filter( "Electron_pt1>10" , "events with electron1 pt > 10 GeV" );
  df = df.Filter( "Electron_pt2>10" , "events with electron2 pt > 10 GeV" );
  df = df.Filter( "Muon_pt1>10" , "events with muon1 pt > 20 GeV" );
  df = df.Filter( "Muon_pt2>10" , "events with muon2 pt > 20 GeV" );
  
  df.Snapshot( "events", mycfg.output , save_branches );

  auto report = df.Report();
  report->Print();

  ROOT::RDF::SaveGraph( df , "graph.dot" );

  time.Stop();
  time.Print();
}
