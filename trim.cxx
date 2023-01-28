#include "helper.h"
#include "tnp.h"
#include "input/input.h"

int main(int argc, char **argv) {
  
  if(argc != 4) {
    std::cout << "Use executable with following arguments: ./trim input output core" << std::endl;
    return -1;
  }
    
  // start time
  TStopwatch time;
  time.Start();
  
  int ncore = atoi(argv[3]);
  const auto processor_count = (ncore == -1) ? std::thread::hardware_concurrency() : ncore;
  std::cout << "ncpu detected : " << processor_count << ", using it all!" << std::endl;
  ROOT::EnableImplicitMT(processor_count);
  
  // characterize configuration
  Helper::config_t mycfg;
  mycfg.input  = argv[1];
  mycfg.output = argv[2];
  //mycfg.outputVar = default_branches;
  std::vector<std::string> out = Helper::makeList("input/out.dat");
  std::vector<std::string> hlt = Helper::makeList("input/HLT.dat");
  mycfg.outputVar = Helper::joinVector( out , hlt );
  
  mycfg.isMC   = ( mycfg.input.find("Run") != std::string::npos || mycfg.input.find("JPsi") != std::string::npos ) ? false : true;

  // filelist
  std::ifstream file(mycfg.input);
  std::string str;
  while (std::getline(file, str)) { mycfg.infiles.push_back(str); }

  std::cout << ">>> This is MC     : " << mycfg.isMC << std::endl;
  std::cout << ">>> Process input  : " << mycfg.input << std::endl;
  std::cout << ">>> Process output : " << mycfg.output << std::endl;
  for(auto ifile : mycfg.infiles) std::cout << ifile <<std::endl;

  ROOT::RDataFrame df_( "aod2nanoaod/Events" , mycfg.infiles);
  auto df = ROOT::RDF::RNode(df_);

  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Skimming
  // event-level filter
  df = df.Filter( "nJet>0"       , "POST-PROCESS : keeping events with at least 1 jet"      );
  //df = df.Filter( "nElectron>1"   , "POST-PROCESS : keeping events with at least 2 electrons" );
  df = df.Filter( "nMuon>1"       , "POST-PRO: keeping events with at least 2 muons"     );
  
  // particle-level filter
  df = Helper::ironing( df , "Jet_pt" , 1 ).Filter( "Jet_pt1>35" , "POST-PROCESS : events with jet1 pt > 35 GeV" );
  
  df = Helper::ironing( df , "Electron_pt" , 1 ).Filter( "Electron_pt1>20" , "POST-PROCESS : events with 1st electron pt > 20 GeV" );
  df = Helper::ironing( df , "Electron_pt" , 2 ).Filter( "Electron_pt2>20" , "POST-PROCESS : events with 2nd electron pt > 20 GeV" );
  
  df = Helper::ironing( df , "Muon_pt" , 1 ).Filter( "Muon_pt1>20" , "POST-PRO: events with 1st muon pt > 20 GeV" );
  df = Helper::ironing( df , "Muon_pt" , 2 ).Filter( "Muon_pt2>20" , "POST-PRO: events with 2nd muon pt > 20 GeV" );

  // refer to input/input.h
  df = runningInput( df , mycfg );
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  
  df.Snapshot( "events", mycfg.output , mycfg.outputVar );

  auto report = df.Report();
  report->Print();

  ROOT::RDF::SaveGraph( df , "graph.dot" );

  time.Stop();
  time.Print();
}
