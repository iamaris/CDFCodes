// File name:    GendLdz.hh
// Description:  Class which facilitates generation of the primary 
//               vertex z coordinate according to the betaStar dL/dz model.
// Author:       Jason Galyardt

#ifndef GendLdz_hh
#define GendLdz_hh

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandFlat.h"

typedef std::map<int, std::vector<double>, std::greater<int> > parMap;

// Forward class declarations
class Beamline;
class CotBeam;
class HepRandomEngine;
class RandFlat;

class GendLdz
{
protected:
  bool _debug;
  static const double _dLdzRange[2];
  double _envHeight;
  std::vector<double> _v;    // This is a dummy vector; should be empty
  std::vector<double> _parms;
  // The map containing the dL/dz parameters and their associated run numbers
  parMap _dLdzMap;
  int _lastMapRun;

  // Member functions
  int fillMap();
  double dLdzFcn(const double &z) const;
  double findMax();
  // inline functions
  double sign(const double &a, const double &b);
  void shift(double &a, double &b, double &c, double &d);

  // nested class
  // Create a function object for use as a predicate 
  // with the STL algorithm lower_bound()
  class FindBin
  {
  public:
    // overload operator()
    bool operator()(const parMap::value_type val1, const parMap::value_type val2) const
    {
      return val1.first > val2.first;
    }
  };

  // nested class
  // Create a function object for use as a predicate
  // with the STL algorithm min_element() (the default predicate is operator<, which
  // looks at both the key and it's assocaited value, which we don't want.
  class FindMinBin
  {
  public:
    // overload operator()
    bool operator()(const parMap::value_type val1, const parMap::value_type val2) const
    {
      return val1.first < val2.first;
    }
  };
  
public:
  // Constructors
  GendLdz();
  GendLdz(bool verbose);
  // Destructor
  ~GendLdz();
  // Public member functions
  int init();
  //  int getParms(const int &runNo);
  int getParms(const int &runNo, CotBeam* cotBeam);
  double throwZ(HepRandomEngine* engine);
};

#endif
