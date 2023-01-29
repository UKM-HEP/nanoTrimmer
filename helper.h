#ifndef HELPER_H
#define HELPER_H

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"
#include "ROOT/RSnapshotOptions.hxx"

#include "Math/Vector4D.h"
#include "TStopwatch.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <utility>
#include <algorithm>
#include <iterator>

#include "TRandom3.h"
#include "TMath.h"

#include <thread>

typedef std::pair<float,std::pair<int,int>> valuetagpair;
//typedef std::pair<std::pair<ROOT::Math::PtEtaPhiMVector,int>,std::pair<ROOT::Math::PtEtaPhiMVector,int>> tnp;
//typedef std::vector<std::pair<std::pair<ROOT::Math::PtEtaPhiMVector,int>,std::pair<ROOT::Math::PtEtaPhiMVector,int>>> tnp_pair;
//typedef ROOT::VecOps::RVec<ROOT::Math::PtEtaPhiMVector> VLorentzV;

namespace Helper {

  // load text file
  std::vector<std::string> makeList(const std::string &textfile)
  {
    std::ifstream file(textfile);
    std::string str;
    std::vector<std::string> vfout;
    while (std::getline(file, str)) { if(str.find('#')==std::string::npos) vfout.push_back(str); }
    return vfout;
  }

  // Join two vectors
  std::vector<std::string> joinVector( std::vector<std::string> &v1 , std::vector<std::string> &v2){
    // Initialise a vector
    // to store the common values
    // and an iterator
    // to traverse this vector
    std::vector<std::string> vout(v1.size() + v2.size());
    std::vector<std::string>::iterator it;

    it = set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), vout.begin());
    return vout;
  }
  
  /////////////////////
  
  /*
   * sort Gen matched candidates in ascending order
   */
  struct dRSorter {
    bool operator() (std::pair<std::pair<int,int>,float> i , std::pair<std::pair<int,int>,float> j) { return ( (i.second) < (j.second) ); }
  };

  /*
   * sort according to the least invariant mass toward z-pole
   */
  struct ZmassSorter {
    bool operator() ( valuetagpair i , valuetagpair j ) { return ( abs(i.first - 91.2) < abs(j.first - 91.2) );  }
  };

  template <typename T>
  std::vector<valuetagpair> IndexbyZmass(T vin ){
    ZmassSorter comparator;
    std::sort (vin.begin() , vin.end() , comparator);
    return vin;
  }

  /*
   * Index by deltaR
   */
  template <typename T>
    std::vector<int> IndexBydeltaR(T v){
    dRSorter comparator;
    std::vector<int> indecies;
    std::sort (v.begin() , v.end() , comparator);
    for (auto it = v.begin() ; it != v.end() ; ++it){
      indecies.push_back((*it).first.second);
    }
    return indecies;
  }

  struct compare
  {
    float masspair;
    compare(float const &i): masspair(i) {}
    
    bool operator()(valuetagpair const &i) {
      return (i.first == masspair);
    }
  };
  
  /*
   * Compute the difference in the azimuth coordinate taking the boundary conditions at 2*pi into account.
   */
  template <typename T = float>
    float DeltaPhi(T v1, T v2, const T c = M_PI)
    {
      auto r = std::fmod(v2 - v1, 2.0 * c);
      if (r < -c) {
	r += 2.0 * c;
      }
      else if (r > c) {
	r -= 2.0 * c;
      }
      return r;
    }

  /*
   * Iron out an array with specific index
   */
  template <typename T>
  auto ironing( T &df, const std::string &collection, int order){
    std::string order_ = std::to_string(order);
    std::string which = std::to_string(order-1);
    return df.Define( collection+order_ , collection+"["+which+"]" );
  }
  
  /*
   * vector maker
   */
  template <typename T>
  ROOT::Math::PtEtaPhiMVector makeLV(T pt, T eta, T phi, T m){
    ROOT::Math::PtEtaPhiMVector out(pt,eta,phi,m);
    return out;
  }

  /*
   * struct configuration
   */
  struct config_t {
    std::string input;
    std::string output;

    bool isMC;
    float Luminosity;
    float xsec;
    int numEvt;
    std::string HLT;
    std::string Flavor;
    std::string HLTobject;
    float kMinTagPt;
    float kMaxTagEta;
    float kMinProbePt;
    float kMaxProbeEta;
    //float kProbeWP;
    float kMaxMass;
    float kMinMass;
    float minDeltaR = 0.1;
    int kWPTag;

    std::vector<std::string> infiles;
    std::vector<std::string> outputVar;
  };
  
} //helper

#endif
