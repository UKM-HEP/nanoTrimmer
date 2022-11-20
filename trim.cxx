#include "helper.h"

#include "helper.h"

// trimming, variables to be saved
std::vector<std::string> TnP_variables = {
  "nJet",
  "Jet_pt",
  "Jet_eta",
  "Jet_phi",
  "nElectron",
  "Electron_pt",
  "Electron_eta",
  "Electron_phi",
  "nMuon",
  "Muon_pt",
  "Muon_eta",
  "Muon_phi"
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
  df = df.Filter( "nJet<4"       , "remove events with number of jet >3"      );
  df = df.Filter( "nElectron<6"  , "remove events with number of electron >5" );
  df = df.Filter( "nMuon<6"      , "remove events with number of muon >5"     );
  df = df.Filter( "Jet_pt[0]>20" , "events with jet pt > 20 GeV"              );
  df = df.Filter( "Jet_pt[1]>20" , "events with jet pt > 20 GeV"              );
  
  df.Snapshot( "fitter_tree", mycfg.output , mycfg.outputVar );

  auto report = df.Report();
  report->Print();

  ROOT::RDF::SaveGraph( df , "graph.dot" );

  time.Stop();
  time.Print();
}
