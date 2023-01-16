#include "helper.h"
#include "tnp.h"

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
  //mycfg.outputVar = default_branches;
  std::vector<std::string> out = Helper::makeList("data/out.dat");
  std::vector<std::string> hlt = Helper::makeList("data/HLT.dat");
  mycfg.outputVar = Helper::joinVector( out , hlt );
  
  mycfg.isMC   = ( mycfg.input.find("Run") != std::string::npos || mycfg.input.find("JPsi") != std::string::npos ) ? false : true;

  // What is the max and min mass range of the resonance?
  mycfg.kMaxMass = 140.;
  mycfg.kMinMass = 60.;

  // filelist
  std::ifstream file(mycfg.input);
  std::string str;
  while (std::getline(file, str)) { mycfg.infiles.push_back(str); }

  std::cout << ">>> Process input: " << mycfg.input << std::endl;
  std::cout << ">>> Process output: " << mycfg.output << std::endl;
  for(auto ifile : mycfg.infiles) std::cout << ifile <<std::endl;

  ROOT::RDataFrame df_( "aod2nanoaod/Events" , mycfg.infiles);
  auto df = ROOT::RDF::RNode(df_);

  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Skimming
  // event-level filter
  df = df.Filter( "nJet>1"       , "keeping events with njet>5"      );
  df = df.Filter( "nElectron>1"  , "keeping events with nelectron at least 2 electrons" );
  df = df.Filter( "nMuon>1"      , "keeping events with nmuon at least 2 muons"     );

  // particle-level filter
  df = df.Define( "Jet_pt1" , "Jet_pt[0]" ).Define( "Jet_pt2" , "Jet_pt[1]" );
  df = df.Define( "Electron_pt1" , "Electron_pt[0]" ).Define( "Electron_pt2" , "Electron_pt[1]");
  df = df.Define( "Muon_pt1" , "Muon_pt[0]" ).Define( "Muon_pt2" , "Muon_pt[1]" );

  //df = df.Filter( "Jet_pt1>20" , "events with jet1 pt > 20 GeV" );
  //df = df.Filter( "Jet_pt2>20" , "events with jet2 pt > 20 GeV" );
  df = df.Filter( "Electron_pt1>10" , "events with electron1 pt > 10 GeV" );
  df = df.Filter( "Electron_pt2>10" , "events with electron2 pt > 10 GeV" );
  //df = df.Filter( "Muon_pt1>10" , "events with muon1 pt > 20 GeV" );
  //df = df.Filter( "Muon_pt2>10" , "events with muon2 pt > 20 GeV" );

  // adding tnp variables
  std::string flavor = "Electron";
  df = tnpvector( df , mycfg , flavor );
  df = tnpkin_ele( df , mycfg ); //( flavor == "Electron" ) ? tnpkin_ele( df , mycfg ) : tnpkin_u( df , mycfg );
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  
  df.Snapshot( "events", mycfg.output , mycfg.outputVar );

  auto report = df.Report();
  report->Print();

  ROOT::RDF::SaveGraph( df , "graph.dot" );

  time.Stop();
  time.Print();
}
