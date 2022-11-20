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

#include "utility" // std::pair
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

#include "TRandom3.h"
#include "TLorentzVector.h"

#include "TMath.h"

#include <thread>

namespace Helper {

  /*
   * sort Gen matched candidates in ascending order
   */
  struct dRSorter {
    bool operator() (std::pair<std::pair<int,int>,float> i , std::pair<std::pair<int,int>,float> j) { return ( (i.second) < (j.second) ); }
  };

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
  
  /*
   * bit decoder
   */
  template <typename T>
    int bitdecoder( T decimal , T kbit){
    // shift to specific bit to the left, and query on/off state
    return (decimal >> kbit & 1);
  }
  
  /*
   * Compute the difference in the azimuth coordinate taking the boundary conditions at 2*pi into account.
   */
  template <typename T>
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
   * vector maker
   */
  template<typename T>
    TLorentzVector VectorMaker(T pt, T eta, T phi, T m){
    TLorentzVector out;
    out.SetPtEtaPhiM(pt,eta,phi,m);
    return out;
  }

  /*
   * struct configuration
   */
  struct config_t {
    std::string input;
    std::string output;

    bool isMC;
    std::vector<std::string> infiles;
    std::vector<std::string> outputVar;
  };
  
} //helper

#endif
