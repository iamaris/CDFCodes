// File name:    GendLdz.cc
// Description:  Class which facilitates generation of the primary 
//               vertex z coordinate.
// Author:       Jason Galyardt, Carnegie Mellon University

#include "generatorMods/GendLdz.hh"
//Cot and Svx beam positions
#include "Alignment/CotBeam.hh"
#include "VertexObjects/Beamline.hh"

using std::vector;

const double GendLdz::_dLdzRange[2] = {-180.0, 180.0};

// Constructors
//_____________________________________________________________________________
GendLdz::GendLdz():
  _debug(false),
  _lastMapRun(293800),
  _envHeight(0.0)
{
}

//_____________________________________________________________________________
GendLdz::GendLdz(bool verbose):
  _debug(verbose),
  _lastMapRun(293800),
  _envHeight(0.0)
{
}


// Destructor
//_____________________________________________________________________________
GendLdz::~GendLdz()
{
}

//_____________________________________________________________________________
int GendLdz::init()
{
  if ( !fillMap() ) {
    return 0;
  } else {
    return -1;
  }
}

//_____________________________________________________________________________
double GendLdz::dLdzFcn(const double &z) const
{
  if (_parms.empty() || _parms.size() != 5) {
    // empty parameter set or vector size is greater than one parameter set
    return 0.0;
  }
  double deltaZ0 = z - _parms[1];
  double deltaZm = z - _parms[3];
  return _parms[0] * exp(-0.5 * deltaZ0 * deltaZ0 / (_parms[2] * _parms[2])) /
    (1 + deltaZm * deltaZm / (_parms[4] * _parms[4]));
}

//_____________________________________________________________________________
inline double GendLdz::sign(const double &a, const double &b)
{
  return b >= 0.0 ? fabs(a) : -fabs(a);
}

//_____________________________________________________________________________
inline void GendLdz::shift(double &a, double &b, double &c, double &d)
{
  a = b;
  b = c; 
  c = d;
}

//_____________________________________________________________________________
double GendLdz::findMax()
{
  // Use inverse parabolic interpolation to find the dL/dz function 
  // maximum; the function is well-enough behaved that we shouldn't 
  // need an adaptive search using, say, a Golden Section search.  No 
  // derivatives need be calculated.
  // See "Numerical Recipes in C", sections 10.1 & 10.2 (esp. "Brent's
  // Method").

  int iter; 
  const int ITMAX = 100;
  double ax, bx, cx, tol;
  double a, b, d, e, etemp, fu, fv, fw, fx, p, q, r, tol1, tol2, u, v, w, x, xm;
  const double epsilon = 1.0E-10;
  const double CGOLD = 0.3819660;

  // Initializations
  ax = -20.0;  // need ax > bx > cx
  bx = 0.0;
  cx = 20.0;
  tol = 3.0E-8; // should be no larger than the sqrt of the machine's floating pt precision
  e = 0.0;
  a = (ax < cx ? ax : cx);
  b = (ax > cx ? ax : cx);
  x = w = v = bx;
  fw = fv = fx = dLdzFcn(x);
  
  for (iter=0; iter<ITMAX; ++iter) {
    xm = 0.5 * (a + b);
    tol1 = tol * fabs(x) + epsilon;
    tol2 = 2.0 * tol1;
    if ( fabs(x - xm) <= (tol2 - 0.5 * (b - a)) ) {  // Are we done, yet?
      return fx;
    }
    if (fabs(e) > tol1) {      // Construct a parabolic fit
      r = (x - w) * (fx - fv);
      q = (x - v) * (fx - fw);
      p = (x - v) * q - (x - w) * r;
      q = 2.0 * (q - r);
      if (q > 0.0) 
	p = -p;
      q = fabs(q);
      etemp = e;
      e = d;
      if ( fabs(p) >= fabs(0.5 * q * etemp) || p <= q * (a - x) || p >= q * (b - x) ) {
	// Take the Golden sect. step into the larger of the two sections
	e = x >= xm ? a - x : b - x;  
	d = CGOLD * e;
      }
      else {
	// Take the parabolic step
	d = p / q;
	u = x + d;
	if ( (u - a) < tol2 || (b - u) < tol2 )
	  d = sign(tol1, xm - x);
      }
    } else {
      e = x >= xm ? a - x : b - x;
      d = CGOLD * e;
    }
    u = fabs(d) >= tol1 ? x + d : x + sign(tol1, d);
    fu = dLdzFcn(u);

    //    if (fu <= fx) {
    if (fu >= fx) {
      if (u >= x) 
	a = x; 
      else 
	b = x;
      shift(v, w, x, u);
      shift(fv, fw, fx, fu);
    } else {
      if (u < x) 
	a = u; 
      else 
	b = u;
      if (fu >= fw || w == x) {
	v = w;
	w = u;
	fv = fw;
	fw = fu;
      } else if (fu >= fv || v == x || v == w) {
	v = u;
	fv = fu;
      }
    }
  }
  // Should never reach here.
  std::cout << "  ERROR: GendLdz::findMax(): Max iterations reached. " << std::endl;
  return 0.0;
}

//_____________________________________________________________________________
// int GendLdz::getParms(const int &runNo)
int GendLdz::getParms(const int &runNo, CotBeam* cotBeam)
{
  // Get the parameters for the beta* dL/dz function for a given run
  _v.clear();
  _parms.clear(); // for safety

  parMap::iterator runIter(_dLdzMap.end());
  // Find the parameter set bin with the smallest low-edge. 
  parMap::iterator minRunIter = min_element(_dLdzMap.begin(), _dLdzMap.end(), 
					    GendLdz::FindMinBin() );
  int firstRun = minRunIter->first;

  bool useMap = true;

  if ( runNo < firstRun ) {
    std::cout << "  WARNING: GendLdz::getParms(): run " << runNo 
	 << " comes before the defined run ranges.  Using the betaStar "
	 << "z vertex function parameter set for run " << firstRun
	 << "....  " << std::endl;
    runIter = minRunIter;
  } 
//  else if ( runNo > _lastMapRun ) {
//    // Get the parameters from the database
//    if (cotBeam->isValid()) {
//      Beamline *beamline = new Beamline(cotBeam->getBeamline());
//      _parms.push_back(1.0);
//      _parms.push_back(beamline->MinimZFit());
//      _parms.push_back(beamline->WidthZFit());
//      _parms.push_back(beamline->ZzeroZFit());
//      _parms.push_back(beamline->BstarZFit());
//      delete beamline;
//      useMap = false;      
//    } else {
//      // failover to the hard-coded parameter sets
//      std::cout << "   WARNING: GendLdz::getParms(): cotBeam not valid for run "
//		<< runNo << ".  \n\tUsing the hard-coded parameter set instead." << std::endl;
//    }
//  }
  else if ( runNo > _lastMapRun ) {
      std::cout << "   Error: GendLdz::getParms(), requested run is beyond range available." << std::endl;
      return -1;
  }

  if (useMap) {
    if (runIter == _dLdzMap.end()) {
      // We have a valid run number.  NOTE: The last parameter set bin has 
      // an infinite run number as its high edge.
      // The STL algorithm lower_bound uses a binary search with a predicate 
      // (a user-defined predicate called FindBin() is used here).
      runIter = lower_bound( _dLdzMap.begin(), _dLdzMap.end(),
			     parMap::value_type(runNo, _v),
			     GendLdz::FindBin() );
    }
    _parms = runIter->second;
  }
  
  //  _parms = runIter->second;
  if (_parms.empty() || _parms.size() != 5) {
    return -1;
  } else {
    double fMax = findMax();
    _envHeight = fMax > 0.0 ? 1.1 * fMax: 1.5 * _parms[0];
    if (_debug) {
      std::cout << "  Using betaStar dL/dz parm set for run " 
		<< runIter->first << ": " << std::endl;
      std::cout << "      ";
      for (int i=0; i<5; ++i) {
	std::cout << _parms[i] << "  ";
      }
      std::cout << std::endl << "  Envelope Height = " << _envHeight << std::endl;
    }
  }
  return 0;
}


//_____________________________________________________________________________
double GendLdz::throwZ(HepRandomEngine* engine)
{
  // Throw a random number using the von Neumann method (Acceptance - Rejection
  // method).
  double urn2 = RandFlat::shoot(engine);
  double pvZ = RandFlat::shoot(engine, _dLdzRange[0], _dLdzRange[1]);
  while ( _envHeight * urn2 > dLdzFcn(pvZ) ) {
    urn2 = RandFlat::shoot(engine);
    pvZ = RandFlat::shoot(engine, _dLdzRange[0], _dLdzRange[1]);
  }
  return pvZ;
}


//_____________________________________________________________________________
int GendLdz::fillMap()
{
  // Fill the std::map object with the beta* dL/dz function parameters for 
  // each run range

  if (!_dLdzMap.empty())
    _dLdzMap.clear();

  _v.clear();
  vector<double>::iterator vIter = _v.begin();
  _v.insert(vIter, 5, 0.0); // grow the vector by 5 elements, initialized to 0.0 

  // Parameters are ordered as follows,
  // _v[0] -> amplitude (place holder)
  // _v[1] -> z_min
  // _v[2] -> sigma_z
  // _v[3] -> z_0
  // _v[4] -> betaStar

  // using DQM v7.0 good run list

  // bin 0
  int loEdge = 141597;
  _v[0] = 1.0;
  _v[1] = 2.1869135;
  _v[2] = 39.882187;
  _v[3] = 3.7244949;
  _v[4] = 44.345955;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 1
  loEdge = 144694;
  _v[0] = 1.0;
  _v[1] = 1.2287345;
  _v[2] = 39.165321;
  _v[3] = 3.1249712;
  _v[4] = 44.558681;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 2
  loEdge = 148908;
  _v[0] = 1.0;
  _v[1] = 1.2270985;
  _v[2] = 38.938511;
  _v[3] = 2.7717609;
  _v[4] = 43.847439;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 3
  loEdge = 150883;
  _v[0] = 1.0;
  _v[1] = 0.82355183;
  _v[2] = 39.293465;
  _v[3] = 3.3458574;
  _v[4] = 43.766235;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 4
  loEdge = 151974;
  _v[0] = 1.0;
  _v[1] = -0.5364536;
  _v[2] = 38.292007;
  _v[3] = 3.7700789;
  _v[4] = 42.715397;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 5
  loEdge = 152674;
  _v[0] = 1.0;
  _v[1] = -0.32550526;
  _v[2] = 38.806648;
  _v[3] = 3.4477313;
  _v[4] = 44.412807;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 6
  loEdge = 153374;
  _v[0] = 1.0;
  _v[1] = -1.1493269;
  _v[2] = 39.973984;
  _v[3] = 3.380794;
  _v[4] = 43.550041;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 7
  loEdge = 154112;
  _v[0] = 1.0;
  _v[1] = -0.5281558;
  _v[2] = 40.475403;
  _v[3] = 2.6157191;
  _v[4] = 44.028008;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 8
  loEdge = 155114;
  _v[0] = 1.0;
  _v[1] = -0.52782565;
  _v[2] = 41.67025;
  _v[3] = 2.833529;
  _v[4] = 43.106453;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 9
  loEdge = 155744;
  _v[0] = 1.0;
  _v[1] = -0.43910065;
  _v[2] = 41.06636;
  _v[3] = 2.7342243;
  _v[4] = 43.712795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 10
  loEdge = 156118;
  _v[0] = 1.0;
  _v[1] = -0.40168509;
  _v[2] = 40.059284;
  _v[3] = 3.330405;
  _v[4] = 45.607471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 11
  loEdge = 160440;
  _v[0] = 1.0;
  _v[1] = -0.61157066;
  _v[2] = 40.275585;
  _v[3] = 3.2907839;
  _v[4] = 44.530869;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 12
  loEdge = 161171;
  _v[0] = 1.0;
  _v[1] = 2.6951034;
  _v[2] = 39.015072;
  _v[3] = 3.878525;
  _v[4] = 46.461586;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 13
  loEdge = 162130;
  _v[0] = 1.0;
  _v[1] = 3.4593339;
  _v[2] = 37.901196;
  _v[3] = 1.847181;
  _v[4] = 53.489185;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 14
  loEdge = 162837;
  _v[0] = 1.0;
  _v[1] = 3.0768785;
  _v[2] = 36.599339;
  _v[3] = 3.1825945;
  _v[4] = 53.674011;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 15
  loEdge = 164302;
  _v[0] = 1.0;
  _v[1] = 3.5281138;
  _v[2] = 36.779598;
  _v[3] = 3.3301101;
  _v[4] = 51.353592;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 16
  loEdge = 165087;
  _v[0] = 1.0;
  _v[1] = 3.7567627;
  _v[2] = 39.393059;
  _v[3] = 2.7634583;
  _v[4] = 52.807934;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 17
  loEdge = 165839;
  _v[0] = 1.0;
  _v[1] = 2.7507639;
  _v[2] = 39.938492;
  _v[3] = 2.133064;
  _v[4] = 52.642109;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 18
  loEdge = 166479;
  _v[0] = 1.0;
  _v[1] = 2.0568953;
  _v[2] = 39.287746;
  _v[3] = 3.0095313;
  _v[4] = 53.638401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 19
  loEdge = 167138;
  _v[0] = 1.0;
  _v[1] = 3.9620686;
  _v[2] = 38.196941;
  _v[3] = 2.8197601;
  _v[4] = 52.573044;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 20
  loEdge = 167954;
  _v[0] = 1.0;
  _v[1] = 2.1130342;
  _v[2] = 38.74976;
  _v[3] = 3.6168587;
  _v[4] = 50.924076;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 21
  loEdge = 175078;
  _v[0] = 1.0;
  _v[1] = -1.9854946;
  _v[2] = 37.94117;
  _v[3] = 1.2624098;
  _v[4] = 53.348446;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 22
  loEdge = 177371;
  _v[0] = 1.0;
  _v[1] = -2.7167516;
  _v[2] = 39.346489;
  _v[3] = 0.31463349;
  _v[4] = 51.7505;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 23
  loEdge = 178070;
  _v[0] = 1.0;
  _v[1] = -1.903249;
  _v[2] = 39.561462;
  _v[3] = -1.0273155;
  _v[4] = 49.548351;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 24
  loEdge = 178438;
  _v[0] = 1.0;
  _v[1] = -2.4543455;
  _v[2] = 39.030445;
  _v[3] = -0.18161407;
  _v[4] = 50.618805;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 25
  loEdge = 178882;
  _v[0] = 1.0;
  _v[1] = -2.5312192;
  _v[2] = 41.173222;
  _v[3] = 0.01743523;
  _v[4] = 44.851154;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 26
  loEdge = 183097;
  _v[0] = 1.0;
  _v[1] = -3.0749972;
  _v[2] = 41.123173;
  _v[3] = 2.622973;
  _v[4] = 35.049873;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 27
  loEdge = 183786;
  _v[0] = 1.0;
  _v[1] = -3.29988;
  _v[2] = 41.789574;
  _v[3] = 3.2215471;
  _v[4] = 34.956474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 28
  loEdge = 184084;
  _v[0] = 1.0;
  _v[1] = -2.4055142;
  _v[2] = 41.997253;
  _v[3] = 2.8556883;
  _v[4] = 34.034248;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 29
  loEdge = 184453;
  _v[0] = 1.0;
  _v[1] = -2.6366129;
  _v[2] = 40.769318;
  _v[3] = 3.1012757;
  _v[4] = 33.767929;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 30
  loEdge = 185017;
  _v[0] = 1.0;
  _v[1] = -2.5988162;
  _v[2] = 41.867901;
  _v[3] = 2.8989198;
  _v[4] = 33.462391;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 31
  loEdge = 185375;
  _v[0] = 1.0;
  _v[1] = -2.1208189;
  _v[2] = 40.753082;
  _v[3] = 2.3202007;
  _v[4] = 34.189831;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 31a --> Dec. 2004 Runs in DQM v10.0a (not in v8.0)
  loEdge = 190753;
  _v[0] = 1.0;
  _v[1] = -4.7095375;
  _v[2] = 41.665023;
  _v[3] = 5.2590144;
  _v[4] = 33.946291;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // using DQM v8.0 good run list

  // bin 32
  loEdge = 191205;
  _v[0] = 1.0;
  _v[1] = -4.2354927;
  _v[2] = 42.817982;
  _v[3] = 5.0177722;
  _v[4] = 33.920773;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 33
  loEdge = 191723;
  _v[0] = 1.0;
  _v[1] = -3.1431668;
  _v[2] = 41.382252;
  _v[3] = 4.1822524;
  _v[4] = 32.047943;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 34
  loEdge = 192232;
  _v[0] = 1.0;
  _v[1] = -1.8626791;
  _v[2] = 42.298367;
  _v[3] = 3.1420121;
  _v[4] = 31.291983;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 35
  loEdge = 192871;
  _v[0] = 1.0;
  _v[1] = -2.0066347;
  _v[2] = 43.792362;
  _v[3] = 3.0171988;
  _v[4] = 31.438894;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 36
  loEdge = 193167;
  _v[0] = 1.0;
  _v[1] = -1.8597494;
  _v[2] = 39.956493;
  _v[3] = 2.8139536;
  _v[4] = 31.505947;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 37
  loEdge = 193892;
  _v[0] = 1.0; 
  _v[1] = -1.6263423;
  _v[2] = 40.737366;
  _v[3] = 3.0403757;
  _v[4] = 31.778358;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 38
  loEdge = 194261;
  _v[0] = 1.0;
  _v[1] = -1.7326349;
  _v[2] = 42.012913;
  _v[3] = 3.0040946;
  _v[4] = 31.853685;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 39
  loEdge = 194861;
  _v[0] = 1.0;
  _v[1] = -2.1548038;
  _v[2] = 42.462044;
  _v[3] = 3.1643889;
  _v[4] = 31.410841;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // using DQM v10.0a good run list

  // bin 40
  loEdge = 195384;
  _v[0] = 1.0;
  _v[1] = -1.4718577;
  _v[2] = 42.121101;
  _v[3] = 2.9703572;
  _v[4] = 31.776291;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 41
  loEdge = 195739;
  _v[0] = 1.0;
  _v[1] = -2.1354284;
  _v[2] = 42.663879;
  _v[3] = 3.3890755;
  _v[4] = 31.725254;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 42
  loEdge = 196100;
  _v[0] = 1.0;
  _v[1] = -2.2422342;
  _v[2] = 42.834797;
  _v[3] = 3.9061067;
  _v[4] = 31.746033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 43
  loEdge = 196416;
  _v[0] = 1.0;
  _v[1] = -2.6565485;
  _v[2] = 42.829697;
  _v[3] = 3.9194953;
  _v[4] = 31.475855;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 44
  loEdge = 196946;
  _v[0] = 1.0;
  _v[1] = -1.7918605;
  _v[2] = 43.407635;
  _v[3] = 3.4181411;
  _v[4] = 31.488705;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 45
  loEdge = 197404;
  _v[0] = 1.0;
  _v[1] = -1.5391979;
  _v[2] = 42.250309;
  _v[3] = 3.2609761;
  _v[4] = 31.374817;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 46
  loEdge = 198206;
  _v[0] = 1.0;
  _v[1] = -2.131165;
  _v[2] = 40.745018;
  _v[3] = 3.6165738;
  _v[4] = 31.902218;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 47
  loEdge = 198882;
  _v[0] = 1.0;
  _v[1] = -1.3686324;
  _v[2] = 41.353497;
  _v[3] = 3.7351124;
  _v[4] = 32.705593;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 48
  loEdge = 199620;
  _v[0] = 1.0;
  _v[1] = -1.6842721;
  _v[2] = 40.745434;
  _v[3] = 3.4720857;
  _v[4] = 32.870567;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 49
  loEdge = 200534;
  _v[0] = 1.0;
  _v[1] = -1.3535993;
  _v[2] = 43.059486;
  _v[3] = 2.8164201;
  _v[4] = 32.793037;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 50
  loEdge = 201130;
  _v[0] = 1.0;
  _v[1] = -1.0127515;
  _v[2] = 41.667381;
  _v[3] = 2.652061;
  _v[4] = 33.07967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 51
  loEdge = 201800;
  _v[0] = 1.0;
  _v[1] = -1.6225106;
  _v[2] = 40.960304;
  _v[3] = 3.0321259;
  _v[4] = 33.311687;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 52
  loEdge = 202335;
  _v[0] = 1.0;
  _v[1] = -1.2697345;
  _v[2] = 42.620617;
  _v[3] = 2.3947582;
  _v[4] = 33.333336;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 53
  loEdge = 202817;
  _v[0] = 1.0;
  _v[1] = -1.3200448;
  _v[2] = 42.032085;
  _v[3] = 2.387995;
  _v[4] = 33.081165;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 54
  loEdge = 203348;
  _v[0] = 1.0;
  _v[1] = -1.4707445;
  _v[2] = 41.812305;
  _v[3] = 2.3069696;
  _v[4] = 33.831852;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // DQM Good Run list v11

  // bin 55
  loEdge = 203819;
  _v[0] = 1.0;
  _v[1] = -1.6401825;
  _v[2] = 40.650951;
  _v[3] = 3.5862646;
  _v[4] = 33.853874;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 56
  loEdge = 204400;
  _v[0] = 1.0;
  _v[1] = -1.769788;
  _v[2] = 41.960258;
  _v[3] = 4.5992212;
  _v[4] = 30.724817;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 57
  loEdge = 204714;
  _v[0] = 1.0;
  _v[1] = -1.9530061;
  _v[2] = 40.447411;
  _v[3] = 5.6907997;
  _v[4] = 29.309446;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 58
  loEdge = 205073;
  _v[0] = 1.0;
  _v[1] = -2.2001414;
  _v[2] = 42.406898;
  _v[3] = 5.3807478;
  _v[4] = 29.206697;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  // bin 59
  loEdge = 205549;
  _v[0] = 1.0;
  _v[1] = -2.4529209;
  _v[2] = 41.840794;
  _v[3] = 5.3800044;
  _v[4] = 28.992268;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 60
  loEdge = 205964;
  _v[0] = 1.0;
  _v[1] = -2.4203756;
  _v[2] = 42.732098;
  _v[3] = 3.8892705;
  _v[4] = 28.181574;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 61
  loEdge = 206326;
  _v[0] = 1.0;
  _v[1] = -1.8983902;
  _v[2] = 45.053696;
  _v[3] = 2.9288642;
  _v[4] = 28.498398;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // DQM Good Run list v13

  // bin 62
  loEdge = 206990;
  _v[0] = 1.0;
  _v[1] = -1.9154198;
  _v[2] = 42.498272;
  _v[3] = 3.075305;
  _v[4] = 28.08914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 63
  loEdge = 208991;
  _v[0] = 1.0;
  _v[1] = -2.1753743;
  _v[2] = 41.22876;
  _v[3] = 3.5043392;
  _v[4] = 29.425285;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 64
  loEdge = 209264;
  _v[0] = 1.0;
  _v[1] = -2.6112466;
  _v[2] = 41.331478;
  _v[3] = 2.4877586;
  _v[4] = 30.038464;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 65
  loEdge = 209513;
  _v[0] = 1.0;
  _v[1] = -2.2443774;
  _v[2] = 40.69817;
  _v[3] = 1.8958937;
  _v[4] = 29.183838;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 66
  loEdge = 209911;
  _v[0] = 1.0;
  _v[1] = -2.5332782;
  _v[2] = 41.332451;
  _v[3] = 1.9469006;
  _v[4] = 28.996086;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 67
  loEdge = 211316;
  _v[0] = 1.0;
  _v[1] = -2.1958745;
  _v[2] = 40.884693;
  _v[3] = 1.9706795;
  _v[4] = 28.547907;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 68
  loEdge = 218180;
  _v[0] = 1.0;
  _v[1] = -2.9392204;
  _v[2] = 41.414032;
  _v[3] = 1.9418892;
  _v[4] = 30.382113;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 69
  loEdge = 219025;
  _v[0] = 1.0;
  _v[1] = -3.6806288;
  _v[2] = 40.779625;
  _v[3] = 4.3127341;
  _v[4] = 31.597628;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 70
  loEdge = 219223;
  _v[0] = 1.0;
  _v[1] = -3.3009374;
  _v[2] = 41.12962;
  _v[3] = 3.0222189;
  _v[4] = 31.509468;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 71
  loEdge = 219463;
  _v[0] = 1.0;
  _v[1] = -3.2921891;
  _v[2] = 43.327633;
  _v[3] = 3.4551177;
  _v[4] = 31.860928;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 72
  loEdge = 219607;
  _v[0] = 1.0;
  _v[1] = -3.4495287;
  _v[2] = 40.01881;
  _v[3] = 3.9357827;
  _v[4] = 32.035572;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 73
  loEdge = 219972;
  _v[0] = 1.0;
  _v[1] = -3.3181701;
  _v[2] = 40.978619;
  _v[3] = 3.3184199;
  _v[4] = 30.727135;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 74
  loEdge = 220254;
  _v[0] = 1.0;
  _v[1] = -3.0234988;
  _v[2] = 42.73687;
  _v[3] = 2.4534616;
  _v[4] = 30.960705;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 75
  loEdge = 220727;
  _v[0] = 1.0;
  _v[1] = -3.8261256;
  _v[2] = 39.90691;
  _v[3] = 3.6057026;
  _v[4] = 31.592775;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 76
  loEdge = 221375;
  _v[0] = 1.0;
  _v[1] = -3.0937788;
  _v[2] = 39.566853;
  _v[3] = 3.0469103;
  _v[4] = 31.126059;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 77
  loEdge = 221903;
  _v[0] = 1.0;
  _v[1] = -3.1103075;
  _v[2] = 39.638981;
  _v[3] = 3.043236;
  _v[4] = 30.634537;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // Using DQM GoodRun List v15
  // bin 78
  loEdge = 222530;
  _v[0] = 1.0;
  _v[1] = -3.5775316;
  _v[2] = 40.074093;
  _v[3] = 3.2570884;
  _v[4] = 31.051977;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 79
  loEdge = 222834;
  _v[0] = 1.0;
  _v[1] = -3.2243826;
  _v[2] = 39.264271;
  _v[3] = 3.192405;
  _v[4] = 31.081949;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 80
  loEdge = 223236;
  _v[0] = 1.0;
  _v[1] = -2.9182503;
  _v[2] = 39.805447;
  _v[3] = 2.8235807;
  _v[4] = 30.92071;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 81
  loEdge = 224056;
  _v[0] = 1.0;
  _v[1] = -3.0626791;
  _v[2] = 39.297821;
  _v[3] = 2.9607863;
  _v[4] = 31.738516;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 82
  loEdge = 224521;
  _v[0] = 1.0;
  _v[1] = -3.1993668;
  _v[2] = 38.448792;
  _v[3] = 3.2455597;
  _v[4] = 30.64765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 83
  loEdge = 227451;
  _v[0] = 1.0;
  _v[1] = -3.0535965;
  _v[2] = 39.60603;
  _v[3] = 3.3042562;
  _v[4] = 29.830845;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 84
  loEdge = 228664;
  _v[0] = 1.0;
  _v[1] = -2.6954441;
  _v[2] = 39.515438;
  _v[3] = 3.0815592;
  _v[4] = 30.556551;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 85
  loEdge = 229332;
  _v[0] = 1.0;
  _v[1] = -3.3226666;
  _v[2] = 38.672871;
  _v[3] = 3.5395732;
  _v[4] = 30.796835;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 86
  loEdge = 229928;
  _v[0] = 1.0;
  _v[1] = -2.868089;
  _v[2] = 39.607658;
  _v[3] = 3.3597014;
  _v[4] = 29.842165;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 87
  loEdge = 230568;
  _v[0] = 1.0;
  _v[1] = -2.9113526;
  _v[2] = 40.268238;
  _v[3] = 3.3485987;
  _v[4] = 29.745314;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 88
  loEdge = 231082;
  _v[0] = 1.0;
  _v[1] = -2.9620323;
  _v[2] = 41.608334;
  _v[3] = 3.150959;
  _v[4] = 29.337427;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 89 
  loEdge = 231474;
  _v[0] = 1.0;
  _v[1] = -3.247782;
  _v[2] = 40.715553;
  _v[3] = 3.37304;
  _v[4] = 30.059385;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 90
  loEdge = 231973;
  _v[0] = 1.0;
  _v[1] = -2.6329529;
  _v[2] = 39.741489;
  _v[3] = 3.1599576;
  _v[4] = 29.608051;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 91
  loEdge = 232493;
  _v[0] = 1.0;
  _v[1] = -3.1032894;
  _v[2] = 40.165596;
  _v[3] = 3.7307973;
  _v[4] = 30.782806;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // Period 11
  // bin 92
  loEdge = 233133;
  _v[0] = 1.0;
  _v[1] = -2.9038541;
  _v[2] = 39.28186;
  _v[3] = 3.1763864;
  _v[4] = 29.608032;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 93
  loEdge = 234413;
  _v[0] = 1.0;
  _v[1] = -2.6442761;
  _v[2] = 40.012596;
  _v[3] = 2.9151571;
  _v[4] = 29.217432;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 94
  loEdge = 235190;
  _v[0] = 1.0;
  _v[1] = -3.0705445;
  _v[2] = 39.976185;
  _v[3] = 3.3608444;
  _v[4] = 29.642036;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 95
  loEdge = 236132;
  _v[0] = 1.0;
  _v[1] = -3.6191483;
  _v[2] = 39.548756;
  _v[3] = 3.7842834;
  _v[4] = 29.745531;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 96
  loEdge = 236780;
  _v[0] = 1.0;
  _v[1] = -3.0365319;
  _v[2] = 38.918552;
  _v[3] = 3.1320329;
  _v[4] = 29.586178;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // Period 12
  // bin 97
  loEdge = 237311;
  _v[0] = 1.0;
  _v[1] = -2.8740387;
  _v[2] = 39.113228;
  _v[3] = 2.8093302;
  _v[4] = 29.442883;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 98
  loEdge = 238457;
  _v[0] = 1.0;
  _v[1] = -3.1170475;
  _v[2] = 39.862476;
  _v[3] = 3.1577072;
  _v[4] = 29.72341;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 99
  loEdge = 239487;
  _v[0] = 1.0;
  _v[1] = -3.4865232;
  _v[2] = 39.184341;
  _v[3] = 3.1812453;
  _v[4] = 29.253824;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 100
  loEdge = 240115;
  _v[0] = 1.0;
  _v[1] = -2.633106;
  _v[2] = 39.08353;
  _v[3] = 2.8402638;
  _v[4] = 29.727943;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 101
  loEdge = 240788;
  _v[0] = 1.0;
  _v[1] = -3.4661293;
  _v[2] = 38.310413;
  _v[3] = 2.0976939;
  _v[4] = 27.893629;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // Period 13
  // bin 102
  loEdge = 241491;
  _v[0] = 1.0;
  _v[1] = -3.3469443;
  _v[2] = 37.578514;
  _v[3] = 1.9215946;
  _v[4] = 27.898357;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 103
  loEdge = 242168;
  _v[0] = 1.0;
  _v[1] = -2.8000185;
  _v[2] = 37.888344;
  _v[3] = 1.0074039;
  _v[4] = 29.122936;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 104
  loEdge = 242640;
  _v[0] = 1.0;
  _v[1] = -2.8933716;
  _v[2] = 39.662548;
  _v[3] = 1.3085463;
  _v[4] = 29.659945;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 105
  loEdge = 242766;
  _v[0] = 1.0;
  _v[1] = -2.9822569;
  _v[2] = 38.876736;
  _v[3] = 1.971367;
  _v[4] = 30.508087;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 106
  loEdge = 243238;
  _v[0] = 1.0;
  _v[1] = -2.9736342;
  _v[2] = 38.909012;
  _v[3] = 2.9260597;
  _v[4] = 32.44595;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 107
  loEdge = 243808;
  _v[0] = 1.0;
  _v[1] = -2.8672462;
  _v[2] = 38.878765;
  _v[3] = 3.0009129;
  _v[4] = 33.446495;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 108
  loEdge = 244335;
  _v[0] = 1.0;
  _v[1] = -3.1168795;
  _v[2] = 38.732834;
  _v[3] = 2.3714015;
  _v[4] = 32.787186;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 109
  loEdge = 244675;
  _v[0] = 1.0;
  _v[1] = -3.276859;
  _v[2] = 37.953373;
  _v[3] = 2.6824515;
  _v[4] = 32.869747;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 110
  loEdge = 245292;
  _v[0] = 1.0;
  _v[1] = -3.2369256;
  _v[2] = 37.958595;
  _v[3] = 2.3307981;
  _v[4] = 30.662014;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 111
  loEdge = 245785;
  _v[0] = 1.0;
  _v[1] = -2.1675427;
  _v[2] = 40.865437;
  _v[3] = 2.596333;
  _v[4] = 25.627089;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 112
  loEdge = 246058;
  _v[0] = 1.0;
  _v[1] = -2.3239152;
  _v[2] = 41.520771;
  _v[3] = 2.3612463;
  _v[4] = 25.710447;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 113
  loEdge = 246231;
  _v[0] = 1.0;
  _v[1] = -3.1340706;
  _v[2] = 37.893299;
  _v[3] = 3.5019197;
  _v[4] = 23.787853;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 114
  loEdge = 253830;
  _v[0] = 1.0;
  _v[1] = -2.8464894;
  _v[2] = 36.959087;
  _v[3] = -0.19563678;
  _v[4] = 27.169287;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // p15
  // bin 115
  loEdge = 254800;
  _v[0] = 1.0;
  _v[1] = -3.1956623;
  _v[2] = 39.466125;
  _v[3] = -0.013834304;
  _v[4] = 25.248024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 116
  loEdge = 255195;
  _v[0] = 1.0;
  _v[1] = -2.8136392;
  _v[2] = 39.27499;
  _v[3] = -0.3744331;
  _v[4] = 25.41699;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 117
  loEdge = 255786;
  _v[0] = 1.0;
  _v[1] = -2.8376997;
  _v[2] = 39.29837;
  _v[3] = -0.47027171;
  _v[4] = 25.123375;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 118
  loEdge = 256194;
  _v[0] = 1.0;
  _v[1] = -2.6472549;
  _v[2] = 40.26207;
  _v[3] = -0.82681394;
  _v[4] = 25.853312;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 119
  loEdge = 256602;
  _v[0] = 1.0;
  _v[1] = -3.3868191;
  _v[2] = 38.676594;
  _v[3] = -0.47561777;
  _v[4] = 25.706022;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // period 16
  // bin 120
  loEdge = 256840;
  _v[0] = 1.0;
  _v[1] = -3.0443153;
  _v[2] = 38.947441;
  _v[3] = 0.1174565;
  _v[4] = 25.923641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 121
  loEdge = 257698;
  _v[0] = 1.0;
  _v[1] = -2.8867807;
  _v[2] = 37.809109;
  _v[3] = 2.6460347;
  _v[4] = 25.659323;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 122
  loEdge = 258395;
  _v[0] = 1.0;
  _v[1] = -2.4728785;
  _v[2] = 39.500408;
  _v[3] = 2.6340029;
  _v[4] = 25.782799;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // period 17
  // bin 123
  loEdge = 258880;
  _v[0] = 1.0;
  _v[1] = -2.9545145;
  _v[2] = 38.632866;
  _v[3] = 2.8805158;
  _v[4] = 25.364784;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 124
  loEdge = 259484;
  _v[0] = 1.0;
  _v[1] = -2.6628661;
  _v[2] = 37.857574;
  _v[3] = 2.6980221;
  _v[4] = 25.263746;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 125
  loEdge = 259973;
  _v[0] = 1.0;
  _v[1] = -2.8305302;
  _v[2] = 39.028736;
  _v[3] = 2.8168149;
  _v[4] = 24.744696;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  // bin 126
  loEdge = 260387;
  _v[0] = 1.0;
  _v[1] = -2.1591189;
  _v[2] = 38.802063;
  _v[3] = 2.6199207;
  _v[4] = 25.692518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  //p18 data
  loEdge = 261119;
  _v[0] = 1.0; 
  _v[1] = 3.86554;
  _v[2] = 33.2707;
  _v[3] = -3.78786;
  _v[4] = 23.5265;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261120;
  _v[0] = 1.0; 
  _v[1] = 2.92462;
  _v[2] = 38.0979;
  _v[3] = -1.91166;
  _v[4] = 26.2757;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261142;
  _v[0] = 1.0; 
  _v[1] = 3.87173;
  _v[2] = 30.7594;
  _v[3] = -4.95645;
  _v[4] = 23.0973;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261145;
  _v[0] = 1.0; 
  _v[1] = 4.91481;
  _v[2] = 35.6983;
  _v[3] = -3.61483;
  _v[4] = 27.6757;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261157;
  _v[0] = 1.0; 
  _v[1] = 4.81674;
  _v[2] = 29.1312;
  _v[3] = -3.75939;
  _v[4] = 24.5917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261158;
  _v[0] = 1.0; 
  _v[1] = 2.64588;
  _v[2] = 34.1777;
  _v[3] = -1.61959;
  _v[4] = 27.0974;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261163;
  _v[0] = 1.0; 
  _v[1] = 4.79394;
  _v[2] = 39.1896;
  _v[3] = -4.70438;
  _v[4] = 25.9322;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261185;
  _v[0] = 1.0; 
  _v[1] = 3.638;
  _v[2] = 35.6969;
  _v[3] = -2.88869;
  _v[4] = 25.455;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261186;
  _v[0] = 1.0; 
  _v[1] = -7.6745;
  _v[2] = 29.324;
  _v[3] = 6.60278;
  _v[4] = 35.3963;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261205;
  _v[0] = 1.0; 
  _v[1] = 3.47891;
  _v[2] = 35.0376;
  _v[3] = -2.78102;
  _v[4] = 25.14;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261211;
  _v[0] = 1.0; 
  _v[1] = 4.13195;
  _v[2] = 37.0241;
  _v[3] = -1.87965;
  _v[4] = 29.0326;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261223;
  _v[0] = 1.0; 
  _v[1] = 3.03662;
  _v[2] = 34.5149;
  _v[3] = -2.99027;
  _v[4] = 24.8682;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261225;
  _v[0] = 1.0; 
  _v[1] = 2.76252;
  _v[2] = 38.4735;
  _v[3] = -1.79583;
  _v[4] = 28.2274;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261297;
  _v[0] = 1.0; 
  _v[1] = 4.94472;
  _v[2] = 29.7675;
  _v[3] = -4.70304;
  _v[4] = 27.0592;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261385;
  _v[0] = 1.0; 
  _v[1] = 2.86665;
  _v[2] = 36.656;
  _v[3] = -2.65344;
  _v[4] = 25.4429;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261416;
  _v[0] = 1.0; 
  _v[1] = 2.35194;
  _v[2] = 28.3192;
  _v[3] = -2.07849;
  _v[4] = 25.2797;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261417;
  _v[0] = 1.0; 
  _v[1] = 1.97754;
  _v[2] = 34.8503;
  _v[3] = -2.15641;
  _v[4] = 25.7857;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261420;
  _v[0] = 1.0; 
  _v[1] = 3.09323;
  _v[2] = 38.7159;
  _v[3] = -3.98177;
  _v[4] = 26.0047;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261441;
  _v[0] = 1.0; 
  _v[1] = 3.53891;
  _v[2] = 32.2492;
  _v[3] = -3.06224;
  _v[4] = 24.1877;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261490;
  _v[0] = 1.0; 
  _v[1] = 3.24051;
  _v[2] = 32.9563;
  _v[3] = -2.05075;
  _v[4] = 26.7227;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261498;
  _v[0] = 1.0; 
  _v[1] = 2.43959;
  _v[2] = 39.9017;
  _v[3] = -2.26373;
  _v[4] = 24.7104;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261507;
  _v[0] = 1.0; 
  _v[1] = 2.97218;
  _v[2] = 35.7506;
  _v[3] = -2.33658;
  _v[4] = 25.5128;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261557;
  _v[0] = 1.0; 
  _v[1] = 2.63612;
  _v[2] = 37.6779;
  _v[3] = -0.923361;
  _v[4] = 28.8164;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261805;
  _v[0] = 1.0; 
  _v[1] = 5.15838;
  _v[2] = 33.0712;
  _v[3] = -4.95509;
  _v[4] = 26.901;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261812;
  _v[0] = 1.0; 
  _v[1] = 5.15838;
  _v[2] = 33.0712;
  _v[3] = -4.95509;
  _v[4] = 26.901;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261813;
  _v[0] = 1.0; 
  _v[1] = 4.70696;
  _v[2] = 39.2752;
  _v[3] = -5.56053;
  _v[4] = 23.9043;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261855;
  _v[0] = 1.0; 
  _v[1] = 3.05842;
  _v[2] = 36.5363;
  _v[3] = -2.2791;
  _v[4] = 27.1678;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261876;
  _v[0] = 1.0; 
  _v[1] = 3.09864;
  _v[2] = 32.0649;
  _v[3] = -2.7075;
  _v[4] = 24.8741;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261895;
  _v[0] = 1.0; 
  _v[1] = 4.12846;
  _v[2] = 36.4361;
  _v[3] = -3.71501;
  _v[4] = 25.5936;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261906;
  _v[0] = 1.0; 
  _v[1] = 3.09993;
  _v[2] = 36.3387;
  _v[3] = -2.33114;
  _v[4] = 25.3144;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261926;
  _v[0] = 1.0; 
  _v[1] = 2.82674;
  _v[2] = 35.2003;
  _v[3] = -1.78963;
  _v[4] = 23.7263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261928;
  _v[0] = 1.0; 
  _v[1] = 3.54752;
  _v[2] = 40.8434;
  _v[3] = -2.94849;
  _v[4] = 26.4027;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261957;
  _v[0] = 1.0; 
  _v[1] = 3.50768;
  _v[2] = 31.4266;
  _v[3] = -2.74404;
  _v[4] = 23.8689;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261958;
  _v[0] = 1.0; 
  _v[1] = 3.0475;
  _v[2] = 35.2879;
  _v[3] = -2.79974;
  _v[4] = 25.5929;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261961;
  _v[0] = 1.0; 
  _v[1] = 1.98716;
  _v[2] = 36.5382;
  _v[3] = 1.0265;
  _v[4] = 28.4577;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 261964;
  _v[0] = 1.0; 
  _v[1] = 4.63184;
  _v[2] = 39.6125;
  _v[3] = -2.76996;
  _v[4] = 27.1387;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262056;
  _v[0] = 1.0; 
  _v[1] = 3.68974;
  _v[2] = 39.0009;
  _v[3] = -2.94454;
  _v[4] = 26.4224;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262111;
  _v[0] = 1.0; 
  _v[1] = 2.74763;
  _v[2] = 35.7634;
  _v[3] = -1.71476;
  _v[4] = 24.989;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262118;
  _v[0] = 1.0; 
  _v[1] = 4.29515;
  _v[2] = 39.9718;
  _v[3] = -4.18403;
  _v[4] = 25.5112;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262147;
  _v[0] = 1.0; 
  _v[1] = 3.5595;
  _v[2] = 34.5277;
  _v[3] = -2.88664;
  _v[4] = 24.9178;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 262169;
  _v[0] = 1.0; 
  _v[1] = 3.3197;
  _v[2] = 32.5613;
  _v[3] = -2.38504;
  _v[4] = 22.8417;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262170;
  _v[0] = 1.0; 
  _v[1] = 2.58123;
  _v[2] = 38.12;
  _v[3] = -1.06478;
  _v[4] = 26.2088;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262183;
  _v[0] = 1.0; 
  _v[1] = 3.48975;
  _v[2] = 34.7438;
  _v[3] = -2.48278;
  _v[4] = 26.2619;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262184;
  _v[0] = 1.0; 
  _v[1] = 2.78958;
  _v[2] = 38.6412;
  _v[3] = -1.42132;
  _v[4] = 27.2973;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262187;
  _v[0] = 1.0; 
  _v[1] = 0.466457;
  _v[2] = 41.5714;
  _v[3] = 4.46175;
  _v[4] = 27.0416;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262196;
  _v[0] = 1.0; 
  _v[1] = 3.37423;
  _v[2] = 36.6832;
  _v[3] = -2.91374;
  _v[4] = 25.0988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262221;
  _v[0] = 1.0; 
  _v[1] = 3.53104;
  _v[2] = 30.4926;
  _v[3] = -2.23089;
  _v[4] = 24.2729;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262225;
  _v[0] = 1.0; 
  _v[1] = 3.08931;
  _v[2] = 33.9874;
  _v[3] = -1.42369;
  _v[4] = 26.2442;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262226;
  _v[0] = 1.0; 
  _v[1] = 4.54168;
  _v[2] = 36.9537;
  _v[3] = -3.28054;
  _v[4] = 28.2282;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262276;
  _v[0] = 1.0; 
  _v[1] = 3.77302;
  _v[2] = 33.6476;
  _v[3] = -2.88525;
  _v[4] = 23.2898;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262279;
  _v[0] = 1.0; 
  _v[1] = 4.30602;
  _v[2] = 38.8988;
  _v[3] = -3.35709;
  _v[4] = 26.4956;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262297;
  _v[0] = 1.0; 
  _v[1] = 3.55405;
  _v[2] = 34.9146;
  _v[3] = -3.23732;
  _v[4] = 24.7126;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262327;
  _v[0] = 1.0; 
  _v[1] = 3.77204;
  _v[2] = 37.0953;
  _v[3] = -3.23397;
  _v[4] = 25.4309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262346;
  _v[0] = 1.0; 
  _v[1] = 3.30931;
  _v[2] = 43.751;
  _v[3] = -2.45456;
  _v[4] = 27.7761;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262357;
  _v[0] = 1.0; 
  _v[1] = 3.96686;
  _v[2] = 34.765;
  _v[3] = -3.50027;
  _v[4] = 24.5708;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262358;
  _v[0] = 1.0; 
  _v[1] = 2.36445;
  _v[2] = 39.8281;
  _v[3] = -0.710836;
  _v[4] = 26.5886;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262422;
  _v[0] = 1.0; 
  _v[1] = 4.22151;
  _v[2] = 34.1079;
  _v[3] = -3.99932;
  _v[4] = 24.8322;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262424;
  _v[0] = 1.0; 
  _v[1] = 3.09113;
  _v[2] = 38.2303;
  _v[3] = -1.99094;
  _v[4] = 27.7957;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262438;
  _v[0] = 1.0; 
  _v[1] = 3.09633;
  _v[2] = 37.3664;
  _v[3] = -2.69514;
  _v[4] = 25.0114;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262450;
  _v[0] = 1.0; 
  _v[1] = 3.2407;
  _v[2] = 34.3456;
  _v[3] = -2.44278;
  _v[4] = 24.5399;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262452;
  _v[0] = 1.0; 
  _v[1] = 3.68631;
  _v[2] = 40.3275;
  _v[3] = -2.95598;
  _v[4] = 27.6911;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262464;
  _v[0] = 1.0; 
  _v[1] = 3.31881;
  _v[2] = 34.9002;
  _v[3] = -2.62625;
  _v[4] = 25.2106;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262472;
  _v[0] = 1.0; 
  _v[1] = 3.12803;
  _v[2] = 39.2565;
  _v[3] = -1.81971;
  _v[4] = 28.1245;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262490;
  _v[0] = 1.0; 
  _v[1] = 2.85236;
  _v[2] = 35.1905;
  _v[3] = -1.07299;
  _v[4] = 24.1492;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262491;
  _v[0] = 1.0; 
  _v[1] = 2.47617;
  _v[2] = 40.5062;
  _v[3] = -0.720653;
  _v[4] = 25.3699;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262547;
  _v[0] = 1.0; 
  _v[1] = 3.67592;
  _v[2] = 36.9738;
  _v[3] = -3.47668;
  _v[4] = 25.0009;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262548;
  _v[0] = 1.0; 
  _v[1] = 3.90526;
  _v[2] = 41.2111;
  _v[3] = -2.48429;
  _v[4] = 29.2831;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262564;
  _v[0] = 1.0; 
  _v[1] = 3.27666;
  _v[2] = 30.0846;
  _v[3] = -2.84917;
  _v[4] = 23.0388;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262565;
  _v[0] = 1.0; 
  _v[1] = 3.26893;
  _v[2] = 39.8784;
  _v[3] = -2.60522;
  _v[4] = 25.1599;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262602;
  _v[0] = 1.0; 
  _v[1] = 3.29167;
  _v[2] = 35.0271;
  _v[3] = -2.30305;
  _v[4] = 24.4848;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262603;
  _v[0] = 1.0; 
  _v[1] = 2.46413;
  _v[2] = 43.5222;
  _v[3] = 0.35591;
  _v[4] = 27.0511;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262618;
  _v[0] = 1.0; 
  _v[1] = 2.22509;
  _v[2] = 29.6873;
  _v[3] = -1.01045;
  _v[4] = 22.2112;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262619;
  _v[0] = 1.0; 
  _v[1] = 3.56622;
  _v[2] = 37.072;
  _v[3] = -2.64372;
  _v[4] = 25.7368;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262652;
  _v[0] = 1.0; 
  _v[1] = 3.12419;
  _v[2] = 37.0494;
  _v[3] = -2.42648;
  _v[4] = 24.8066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262668;
  _v[0] = 1.0; 
  _v[1] = 2.83312;
  _v[2] = 32.2294;
  _v[3] = -1.17853;
  _v[4] = 23.3492;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262670;
  _v[0] = 1.0; 
  _v[1] = 2.27931;
  _v[2] = 35.022;
  _v[3] = -0.696191;
  _v[4] = 28.5961;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262671;
  _v[0] = 1.0; 
  _v[1] = 2.42139;
  _v[2] = 41.4132;
  _v[3] = -0.900953;
  _v[4] = 26.8352;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262687;
  _v[0] = 1.0; 
  _v[1] = 3.30648;
  _v[2] = 36.4445;
  _v[3] = -2.56941;
  _v[4] = 25.063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262759;
  _v[0] = 1.0; 
  _v[1] = 3.31351;
  _v[2] = 35.4295;
  _v[3] = -2.68778;
  _v[4] = 24.6817;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262776;
  _v[0] = 1.0; 
  _v[1] = 2.46444;
  _v[2] = 33.8438;
  _v[3] = -1.06383;
  _v[4] = 25.465;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262777;
  _v[0] = 1.0; 
  _v[1] = 4.04667;
  _v[2] = 39.9408;
  _v[3] = -4.0488;
  _v[4] = 25.967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262806;
  _v[0] = 1.0; 
  _v[1] = 3.45187;
  _v[2] = 33.6379;
  _v[3] = -2.75912;
  _v[4] = 25.1382;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262808;
  _v[0] = 1.0; 
  _v[1] = 3.87411;
  _v[2] = 39.0338;
  _v[3] = -3.87707;
  _v[4] = 27.5266;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262870;
  _v[0] = 1.0; 
  _v[1] = 3.79978;
  _v[2] = 35.5671;
  _v[3] = -3.06625;
  _v[4] = 24.7234;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262906;
  _v[0] = 1.0; 
  _v[1] = 3.10502;
  _v[2] = 29.8575;
  _v[3] = -2.37627;
  _v[4] = 23.2401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262910;
  _v[0] = 1.0; 
  _v[1] = 3.58422;
  _v[2] = 34.1368;
  _v[3] = -2.11624;
  _v[4] = 23.9186;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262947;
  _v[0] = 1.0; 
  _v[1] = 3.20685;
  _v[2] = 28.4873;
  _v[3] = -1.8305;
  _v[4] = 23.2748;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262953;
  _v[0] = 1.0; 
  _v[1] = 3.49477;
  _v[2] = 35.9825;
  _v[3] = -2.57848;
  _v[4] = 25.4947;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262956;
  _v[0] = 1.0; 
  _v[1] = 3.65065;
  _v[2] = 34.8443;
  _v[3] = -2.63016;
  _v[4] = 24.5964;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262957;
  _v[0] = 1.0; 
  _v[1] = 3.42053;
  _v[2] = 40.908;
  _v[3] = -2.51446;
  _v[4] = 27.2962;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262985;
  _v[0] = 1.0; 
  _v[1] = 3.37663;
  _v[2] = 34.4381;
  _v[3] = -2.94496;
  _v[4] = 23.292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 262988;
  _v[0] = 1.0; 
  _v[1] = 3.26784;
  _v[2] = 39.9757;
  _v[3] = -2.43636;
  _v[4] = 27.0167;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263005;
  _v[0] = 1.0; 
  _v[1] = 3.08367;
  _v[2] = 37.3337;
  _v[3] = -1.92325;
  _v[4] = 24.9084;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263034;
  _v[0] = 1.0; 
  _v[1] = -3.29413;
  _v[2] = 40.8463;
  _v[3] = 12.9742;
  _v[4] = 24.933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263057;
  _v[0] = 1.0; 
  _v[1] = 4.20196;
  _v[2] = 29.5605;
  _v[3] = -3.70781;
  _v[4] = 23.0944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263058;
  _v[0] = 1.0; 
  _v[1] = 3.57291;
  _v[2] = 32.8218;
  _v[3] = -2.2727;
  _v[4] = 25.5515;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263059;
  _v[0] = 1.0; 
  _v[1] = 5.12142;
  _v[2] = 36.9;
  _v[3] = -6.69251;
  _v[4] = 23.8721;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263103;
  _v[0] = 1.0; 
  _v[1] = 3.22415;
  _v[2] = 37.7219;
  _v[3] = -1.25271;
  _v[4] = 26.5502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263158;
  _v[0] = 1.0; 
  _v[1] = 3.51523;
  _v[2] = 33.6565;
  _v[3] = -2.73151;
  _v[4] = 24.0997;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263172;
  _v[0] = 1.0; 
  _v[1] = 4.04611;
  _v[2] = 37.7134;
  _v[3] = -3.9514;
  _v[4] = 25.7562;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263185;
  _v[0] = 1.0; 
  _v[1] = 4.74928;
  _v[2] = 39.6662;
  _v[3] = -4.51986;
  _v[4] = 27.2237;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263259;
  _v[0] = 1.0; 
  _v[1] = 3.03336;
  _v[2] = 37.1316;
  _v[3] = -2.05545;
  _v[4] = 26.1724;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263274;
  _v[0] = 1.0; 
  _v[1] = 2.83956;
  _v[2] = 32.29;
  _v[3] = -1.81664;
  _v[4] = 23.5607;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263312;
  _v[0] = 1.0; 
  _v[1] = 3.54375;
  _v[2] = 33.5537;
  _v[3] = -2.22424;
  _v[4] = 25.5923;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263347;
  _v[0] = 1.0; 
  _v[1] = 2.82775;
  _v[2] = 37.0429;
  _v[3] = -2.30282;
  _v[4] = 24.3341;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263370;
  _v[0] = 1.0; 
  _v[1] = 3.33609;
  _v[2] = 32.9207;
  _v[3] = -2.91034;
  _v[4] = 25.5519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263378;
  _v[0] = 1.0; 
  _v[1] = 3.01606;
  _v[2] = 39.381;
  _v[3] = -2.4448;
  _v[4] = 26.638;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263388;
  _v[0] = 1.0; 
  _v[1] = 2.96584;
  _v[2] = 42.4627;
  _v[3] = 2.63366;
  _v[4] = 30.4294;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263415;
  _v[0] = 1.0; 
  _v[1] = 3.04931;
  _v[2] = 36.7321;
  _v[3] = -2.23956;
  _v[4] = 25.0705;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263417;
  _v[0] = 1.0; 
  _v[1] = 4.3005;
  _v[2] = 42.0567;
  _v[3] = -6.34253;
  _v[4] = 27.052;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263442;
  _v[0] = 1.0; 
  _v[1] = 3.07154;
  _v[2] = 35.6857;
  _v[3] = -2.67853;
  _v[4] = 24.9077;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263446;
  _v[0] = 1.0; 
  _v[1] = 4.33989;
  _v[2] = 39.3313;
  _v[3] = -4.57593;
  _v[4] = 27.9197;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263472;
  _v[0] = 1.0; 
  _v[1] = 3.50106;
  _v[2] = 36.5321;
  _v[3] = -3.07659;
  _v[4] = 24.248;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263542;
  _v[0] = 1.0; 
  _v[1] = 2.90874;
  _v[2] = 32.0436;
  _v[3] = -2.87806;
  _v[4] = 22.2369;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263543;
  _v[0] = 1.0; 
  _v[1] = 3.61112;
  _v[2] = 36.6671;
  _v[3] = -2.64439;
  _v[4] = 25.9444;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263570;
  _v[0] = 1.0; 
  _v[1] = 3.87907;
  _v[2] = 26.6343;
  _v[3] = -3.26591;
  _v[4] = 24.5549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263571;
  _v[0] = 1.0; 
  _v[1] = 3.87907;
  _v[2] = 26.6343;
  _v[3] = -3.26591;
  _v[4] = 24.5549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263573;
  _v[0] = 1.0; 
  _v[1] = 3.87047;
  _v[2] = 35.8757;
  _v[3] = -3.24744;
  _v[4] = 25.6012;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263574;
  _v[0] = 1.0; 
  _v[1] = 1.22879;
  _v[2] = 59.6238;
  _v[3] = -1.33471;
  _v[4] = 21.0136;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263597;
  _v[0] = 1.0; 
  _v[1] = 3.23352;
  _v[2] = 38.382;
  _v[3] = -1.97877;
  _v[4] = 25.539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263611;
  _v[0] = 1.0; 
  _v[1] = 4.88619;
  _v[2] = 44.0475;
  _v[3] = -4.18469;
  _v[4] = 26.3578;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263672;
  _v[0] = 1.0; 
  _v[1] = 3.50744;
  _v[2] = 34.2043;
  _v[3] = -2.91449;
  _v[4] = 24.0723;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263693;
  _v[0] = 1.0; 
  _v[1] = 3.35644;
  _v[2] = 36.0014;
  _v[3] = -2.73299;
  _v[4] = 25.1852;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263718;
  _v[0] = 1.0; 
  _v[1] = 3.09383;
  _v[2] = 32.7288;
  _v[3] = -2.84738;
  _v[4] = 23.9409;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263746;
  _v[0] = 1.0; 
  _v[1] = 3.13256;
  _v[2] = 38.0459;
  _v[3] = -2.05927;
  _v[4] = 26.1789;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263761;
  _v[0] = 1.0; 
  _v[1] = 3.66267;
  _v[2] = 35.184;
  _v[3] = -3.34196;
  _v[4] = 24.6374;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263762;
  _v[0] = 1.0; 
  _v[1] = 1.18768;
  _v[2] = 37.9365;
  _v[3] = 0.137146;
  _v[4] = 29.0685;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263773;
  _v[0] = 1.0; 
  _v[1] = 3.60717;
  _v[2] = 28.1973;
  _v[3] = -2.37009;
  _v[4] = 23.9384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263774;
  _v[0] = 1.0; 
  _v[1] = 3.21781;
  _v[2] = 36.7467;
  _v[3] = -2.61353;
  _v[4] = 24.8165;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263776;
  _v[0] = 1.0; 
  _v[1] = 4.08486;
  _v[2] = 42.7686;
  _v[3] = -3.54648;
  _v[4] = 27.7378;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263791;
  _v[0] = 1.0; 
  _v[1] = 4.07952;
  _v[2] = 33.7561;
  _v[3] = -3.70854;
  _v[4] = 24.6183;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263845;
  _v[0] = 1.0; 
  _v[1] = 3.4912;
  _v[2] = 32.6294;
  _v[3] = -2.11635;
  _v[4] = 23.9572;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263846;
  _v[0] = 1.0; 
  _v[1] = 3.95555;
  _v[2] = 39.1044;
  _v[3] = -3.74689;
  _v[4] = 25.9872;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263861;
  _v[0] = 1.0; 
  _v[1] = 3.46348;
  _v[2] = 41.4029;
  _v[3] = -2.4347;
  _v[4] = 26.4438;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263877;
  _v[0] = 1.0; 
  _v[1] = 2.74132;
  _v[2] = 36.4802;
  _v[3] = -1.53664;
  _v[4] = 24.7463;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263912;
  _v[0] = 1.0; 
  _v[1] = 3.24714;
  _v[2] = 36.2317;
  _v[3] = -2.69696;
  _v[4] = 24.7066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263938;
  _v[0] = 1.0; 
  _v[1] = 3.61474;
  _v[2] = 36.9319;
  _v[3] = -2.47014;
  _v[4] = 25.2818;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263963;
  _v[0] = 1.0; 
  _v[1] = 3.36782;
  _v[2] = 38.5824;
  _v[3] = -2.55985;
  _v[4] = 24.6376;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263979;
  _v[0] = 1.0; 
  _v[1] = 3.16733;
  _v[2] = 30.2665;
  _v[3] = -2.24069;
  _v[4] = 23.6206;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263980;
  _v[0] = 1.0; 
  _v[1] = 3.6223;
  _v[2] = 35.1293;
  _v[3] = -2.42758;
  _v[4] = 25.4408;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263982;
  _v[0] = 1.0; 
  _v[1] = 3.02339;
  _v[2] = 40.4012;
  _v[3] = -2.80135;
  _v[4] = 25.8123;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 263995;
  _v[0] = 1.0; 
  _v[1] = 3.40795;
  _v[2] = 37.3482;
  _v[3] = -2.7294;
  _v[4] = 24.5656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264015;
  _v[0] = 1.0; 
  _v[1] = 3.73185;
  _v[2] = 35.9758;
  _v[3] = -2.83204;
  _v[4] = 24.4599;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264045;
  _v[0] = 1.0; 
  _v[1] = 3.78093;
  _v[2] = 32.7418;
  _v[3] = -2.26803;
  _v[4] = 24.9692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  //p19 data
  loEdge = 264105;
  _v[0] = 1.0; 
  _v[1] = 3.49746;
  _v[2] = 36.0379;
  _v[3] = -2.66673;
  _v[4] = 26.0847;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264162;
  _v[0] = 1.0; 
  _v[1] = 3.58603;
  _v[2] = 28.6797;
  _v[3] = -4.10463;
  _v[4] = 21.1195;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264163;
  _v[0] = 1.0; 
  _v[1] = 2.65351;
  _v[2] = 34.0063;
  _v[3] = -1.28004;
  _v[4] = 25.5793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264164;
  _v[0] = 1.0; 
  _v[1] = 3.16929;
  _v[2] = 40.8183;
  _v[3] = -1.81921;
  _v[4] = 27.1829;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264194;
  _v[0] = 1.0; 
  _v[1] = 3.31959;
  _v[2] = 36.1502;
  _v[3] = -2.73855;
  _v[4] = 24.0599;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264224;
  _v[0] = 1.0; 
  _v[1] = 2.70416;
  _v[2] = 38.9768;
  _v[3] = -1.52387;
  _v[4] = 24.2637;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264275;
  _v[0] = 1.0; 
  _v[1] = 5.04341;
  _v[2] = 36.6851;
  _v[3] = -3.24819;
  _v[4] = 28.922;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 264748;
  _v[0] = 1.0; 
  _v[1] = 0.947573;
  _v[2] = 46.759;
  _v[3] = 5.11204;
  _v[4] = 22.8717;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265121;
  _v[0] = 1.0; 
  _v[1] = 3.87441;
  _v[2] = 35.0866;
  _v[3] = -2.8808;
  _v[4] = 25.2667;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265137;
  _v[0] = 1.0; 
  _v[1] = 4.13492;
  _v[2] = 40.3957;
  _v[3] = -1.85296;
  _v[4] = 28.9403;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265201;
  _v[0] = 1.0; 
  _v[1] = 3.83095;
  _v[2] = 34.7552;
  _v[3] = -2.95252;
  _v[4] = 24.4676;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265212;
  _v[0] = 1.0; 
  _v[1] = -0.977777;
  _v[2] = -512.453;
  _v[3] = 1764.98;
  _v[4] = -25.1089;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265301;
  _v[0] = 1.0; 
  _v[1] = 2.16137;
  _v[2] = 31.7031;
  _v[3] = -0.21567;
  _v[4] = 24.0807;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265302;
  _v[0] = 1.0; 
  _v[1] = 4.68966;
  _v[2] = 34.6215;
  _v[3] = -3.00874;
  _v[4] = 27.4878;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265314;
  _v[0] = 1.0; 
  _v[1] = 3.2194;
  _v[2] = 38.8442;
  _v[3] = -1.9422;
  _v[4] = 26.2998;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265325;
  _v[0] = 1.0; 
  _v[1] = 3.90103;
  _v[2] = 40.3384;
  _v[3] = -5.27551;
  _v[4] = 26.6153;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265354;
  _v[0] = 1.0; 
  _v[1] = 3.00267;
  _v[2] = 37.7721;
  _v[3] = -2.28689;
  _v[4] = 24.3692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265373;
  _v[0] = 1.0; 
  _v[1] = 2.73575;
  _v[2] = 34.6525;
  _v[3] = -1.6567;
  _v[4] = 24.3628;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265375;
  _v[0] = 1.0; 
  _v[1] = 2.84847;
  _v[2] = 42.0162;
  _v[3] = -2.15426;
  _v[4] = 27.0613;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265394;
  _v[0] = 1.0; 
  _v[1] = 2.82971;
  _v[2] = 35.0684;
  _v[3] = -2.33136;
  _v[4] = 24.2579;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265460;
  _v[0] = 1.0; 
  _v[1] = 3.12164;
  _v[2] = 30.4809;
  _v[3] = -2.47188;
  _v[4] = 24.9584;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265466;
  _v[0] = 1.0; 
  _v[1] = 3.71865;
  _v[2] = 35.8474;
  _v[3] = -2.70213;
  _v[4] = 25.9212;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265472;
  _v[0] = 1.0; 
  _v[1] = 3.13975;
  _v[2] = 39.5531;
  _v[3] = -3.21848;
  _v[4] = 26.9;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265488;
  _v[0] = 1.0; 
  _v[1] = 3.68988;
  _v[2] = 28.8571;
  _v[3] = -2.84785;
  _v[4] = 23.2052;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265489;
  _v[0] = 1.0; 
  _v[1] = 3.19579;
  _v[2] = 35.3422;
  _v[3] = -2.64929;
  _v[4] = 25.0285;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265506;
  _v[0] = 1.0; 
  _v[1] = 3.15119;
  _v[2] = 40.2339;
  _v[3] = -2.27468;
  _v[4] = 25.7924;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265511;
  _v[0] = 1.0; 
  _v[1] = 2.64877;
  _v[2] = 44.4483;
  _v[3] = -1.00613;
  _v[4] = 27.5196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265521;
  _v[0] = 1.0; 
  _v[1] = 3.18065;
  _v[2] = 44.5184;
  _v[3] = -2.37163;
  _v[4] = 28.5678;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265527;
  _v[0] = 1.0; 
  _v[1] = 2.22972;
  _v[2] = 45.4148;
  _v[3] = -1.18474;
  _v[4] = 28.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265532;
  _v[0] = 1.0; 
  _v[1] = 4.51336;
  _v[2] = 46.484;
  _v[3] = -5.5865;
  _v[4] = 28.3571;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265551;
  _v[0] = 1.0; 
  _v[1] = 3.49204;
  _v[2] = 34.6802;
  _v[3] = -2.49961;
  _v[4] = 26.3919;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265565;
  _v[0] = 1.0; 
  _v[1] = 2.67757;
  _v[2] = 40.2223;
  _v[3] = 0.179765;
  _v[4] = 27.1295;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265582;
  _v[0] = 1.0; 
  _v[1] = 3.3803;
  _v[2] = 36.9418;
  _v[3] = -3.21223;
  _v[4] = 24.2284;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265658;
  _v[0] = 1.0; 
  _v[1] = 2.81462;
  _v[2] = 34.4279;
  _v[3] = -1.36311;
  _v[4] = 25.1573;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265701;
  _v[0] = 1.0; 
  _v[1] = 3.30701;
  _v[2] = 31.8584;
  _v[3] = -2.33758;
  _v[4] = 24.8418;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265779;
  _v[0] = 1.0; 
  _v[1] = 3.83829;
  _v[2] = 33.0123;
  _v[3] = -2.4106;
  _v[4] = 24.9664;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265780;
  _v[0] = 1.0; 
  _v[1] = 4.96716;
  _v[2] = 43.2058;
  _v[3] = -4.08027;
  _v[4] = 27.9479;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265786;
  _v[0] = 1.0; 
  _v[1] = 1.70357;
  _v[2] = 45.3584;
  _v[3] = 2.94094;
  _v[4] = 25.306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265828;
  _v[0] = 1.0; 
  _v[1] = 3.67357;
  _v[2] = 35.0554;
  _v[3] = -2.61566;
  _v[4] = 24.7359;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265834;
  _v[0] = 1.0; 
  _v[1] = 5.12349;
  _v[2] = 40.9147;
  _v[3] = -5.99794;
  _v[4] = 26.5518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265865;
  _v[0] = 1.0; 
  _v[1] = 3.29257;
  _v[2] = 35.8699;
  _v[3] = -2.4284;
  _v[4] = 24.6846;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265898;
  _v[0] = 1.0; 
  _v[1] = 3.60193;
  _v[2] = 34.7543;
  _v[3] = -2.30247;
  _v[4] = 25.0885;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265904;
  _v[0] = 1.0; 
  _v[1] = 3.67507;
  _v[2] = 42.1847;
  _v[3] = -2.33416;
  _v[4] = 26.9655;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265935;
  _v[0] = 1.0; 
  _v[1] = 3.40938;
  _v[2] = 37.3859;
  _v[3] = -2.31134;
  _v[4] = 24.2248;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265936;
  _v[0] = 1.0; 
  _v[1] = 5.16985;
  _v[2] = 40.7595;
  _v[3] = -3.08354;
  _v[4] = 28.8409;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265941;
  _v[0] = 1.0; 
  _v[1] = 3.36742;
  _v[2] = 44.0165;
  _v[3] = -1.5213;
  _v[4] = 28.3253;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265962;
  _v[0] = 1.0; 
  _v[1] = 3.49942;
  _v[2] = 35.5381;
  _v[3] = -1.96405;
  _v[4] = 24.2999;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265973;
  _v[0] = 1.0; 
  _v[1] = 5.52493;
  _v[2] = 39.068;
  _v[3] = -4.40341;
  _v[4] = 30.189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 265985;
  _v[0] = 1.0; 
  _v[1] = 3.54113;
  _v[2] = 40.8621;
  _v[3] = -0.0985422;
  _v[4] = 27.03;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266001;
  _v[0] = 1.0; 
  _v[1] = 3.57086;
  _v[2] = 36.3427;
  _v[3] = -2.16293;
  _v[4] = 24.9068;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266013;
  _v[0] = 1.0; 
  _v[1] = 1.96295;
  _v[2] = 31.5327;
  _v[3] = -0.50814;
  _v[4] = 24.2282;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266017;
  _v[0] = 1.0; 
  _v[1] = 2.52804;
  _v[2] = 36.8935;
  _v[3] = -1.30902;
  _v[4] = 24.6368;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266019;
  _v[0] = 1.0; 
  _v[1] = 2.95752;
  _v[2] = 39.2573;
  _v[3] = -2.43218;
  _v[4] = 25.3148;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266020;
  _v[0] = 1.0; 
  _v[1] = 3.5812;
  _v[2] = 41.3404;
  _v[3] = -2.86226;
  _v[4] = 27.6542;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266046;
  _v[0] = 1.0; 
  _v[1] = 3.4475;
  _v[2] = 29.5244;
  _v[3] = -4.83672;
  _v[4] = 21.2724;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266048;
  _v[0] = 1.0; 
  _v[1] = 3.24557;
  _v[2] = 30.2454;
  _v[3] = -2.46021;
  _v[4] = 25.34;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266049;
  _v[0] = 1.0; 
  _v[1] = 3.15434;
  _v[2] = 36.478;
  _v[3] = -1.91694;
  _v[4] = 26.7461;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266131;
  _v[0] = 1.0; 
  _v[1] = 3.00738;
  _v[2] = 39.3913;
  _v[3] = -2.22299;
  _v[4] = 26.2541;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266163;
  _v[0] = 1.0; 
  _v[1] = 2.87717;
  _v[2] = 39.0215;
  _v[3] = -2.95109;
  _v[4] = 24.7816;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266209;
  _v[0] = 1.0; 
  _v[1] = 3.05347;
  _v[2] = 34.7895;
  _v[3] = -3.03293;
  _v[4] = 23.934;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266210;
  _v[0] = 1.0; 
  _v[1] = 4.32998;
  _v[2] = 37.8415;
  _v[3] = -3.64951;
  _v[4] = 28.9797;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266216;
  _v[0] = 1.0; 
  _v[1] = 2.95173;
  _v[2] = 43.2073;
  _v[3] = -0.791758;
  _v[4] = 26.4119;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266266;
  _v[0] = 1.0; 
  _v[1] = 2.8961;
  _v[2] = 35.9196;
  _v[3] = -2.47068;
  _v[4] = 24.6337;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266270;
  _v[0] = 1.0; 
  _v[1] = 3.44058;
  _v[2] = 40.8929;
  _v[3] = -3.42757;
  _v[4] = 28.2855;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266278;
  _v[0] = 1.0; 
  _v[1] = 1.18952;
  _v[2] = 44.4626;
  _v[3] = -0.381975;
  _v[4] = 28.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266299;
  _v[0] = 1.0; 
  _v[1] = 2.39678;
  _v[2] = 27.9418;
  _v[3] = -1.48743;
  _v[4] = 24.8264;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266300;
  _v[0] = 1.0; 
  _v[1] = 3.73083;
  _v[2] = 33.139;
  _v[3] = -3.73852;
  _v[4] = 24.8302;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266302;
  _v[0] = 1.0; 
  _v[1] = 3.61897;
  _v[2] = 37.3498;
  _v[3] = -4.84811;
  _v[4] = 23.9163;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266398;
  _v[0] = 1.0; 
  _v[1] = 2.54765;
  _v[2] = 37.8417;
  _v[3] = -2.12826;
  _v[4] = 24.9677;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266405;
  _v[0] = 1.0; 
  _v[1] = 3.23777;
  _v[2] = 32.6974;
  _v[3] = -2.68179;
  _v[4] = 25.2897;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266488;
  _v[0] = 1.0; 
  _v[1] = 3.37222;
  _v[2] = 28.0543;
  _v[3] = -2.34298;
  _v[4] = 25.2697;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266490;
  _v[0] = 1.0; 
  _v[1] = 3.66679;
  _v[2] = 32.7008;
  _v[3] = -3.38595;
  _v[4] = 25.3979;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266494;
  _v[0] = 1.0; 
  _v[1] = 4.24629;
  _v[2] = 35.9855;
  _v[3] = -4.1681;
  _v[4] = 28.9361;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266509;
  _v[0] = 1.0; 
  _v[1] = 4.09777;
  _v[2] = 37.9548;
  _v[3] = -3.50112;
  _v[4] = 27.2155;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  //p20 data
  loEdge = 266528;
  _v[0] = 1.0; 
  _v[1] = 2.80482;
  _v[2] = 34.4644;
  _v[3] = -1.96906;
  _v[4] = 25.3925;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266529;
  _v[0] = 1.0; 
  _v[1] = 3.04983;
  _v[2] = 41.1986;
  _v[3] = -2.67994;
  _v[4] = 29.2892;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266544;
  _v[0] = 1.0; 
  _v[1] = 2.4014;
  _v[2] = 39.0229;
  _v[3] = -1.5917;
  _v[4] = 25.2494;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266562;
  _v[0] = 1.0; 
  _v[1] = 2.63665;
  _v[2] = 33.964;
  _v[3] = -1.52615;
  _v[4] = 26.0509;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266575;
  _v[0] = 1.0; 
  _v[1] = 3.40129;
  _v[2] = 40.6431;
  _v[3] = -1.42513;
  _v[4] = 27.6123;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266610;
  _v[0] = 1.0; 
  _v[1] = 5.56361;
  _v[2] = 37.9553;
  _v[3] = -5.73048;
  _v[4] = 28.156;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266618;
  _v[0] = 1.0; 
  _v[1] = 2.75588;
  _v[2] = 34.3194;
  _v[3] = -1.80998;
  _v[4] = 24.6369;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266686;
  _v[0] = 1.0; 
  _v[1] = 3.64297;
  _v[2] = 40.0276;
  _v[3] = -3.13373;
  _v[4] = 26.6085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266702;
  _v[0] = 1.0; 
  _v[1] = 2.53538;
  _v[2] = 33.0237;
  _v[3] = -2.2123;
  _v[4] = 24.0468;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266703;
  _v[0] = 1.0; 
  _v[1] = 2.64903;
  _v[2] = 39.7464;
  _v[3] = -2.10111;
  _v[4] = 27.9138;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266716;
  _v[0] = 1.0; 
  _v[1] = 3.07773;
  _v[2] = 30.5904;
  _v[3] = -3.33546;
  _v[4] = 24.0819;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266717;
  _v[0] = 1.0; 
  _v[1] = 3.33589;
  _v[2] = 38.6601;
  _v[3] = -2.76129;
  _v[4] = 26.1085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266733;
  _v[0] = 1.0; 
  _v[1] = 1.87215;
  _v[2] = 29.2687;
  _v[3] = -1.91952;
  _v[4] = 24.0168;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266734;
  _v[0] = 1.0; 
  _v[1] = 3.19207;
  _v[2] = 35.4469;
  _v[3] = -2.87436;
  _v[4] = 26.9263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266745;
  _v[0] = 1.0; 
  _v[1] = 1.78398;
  _v[2] = 40.8605;
  _v[3] = -1.68882;
  _v[4] = 27.3346;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266763;
  _v[0] = 1.0; 
  _v[1] = 2.09675;
  _v[2] = -32.571;
  _v[3] = -1.64861;
  _v[4] = 19.752;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266808;
  _v[0] = 1.0; 
  _v[1] = 2.95825;
  _v[2] = 28.2161;
  _v[3] = -2.73629;
  _v[4] = 25.5985;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266809;
  _v[0] = 1.0; 
  _v[1] = 2.66045;
  _v[2] = 36.0276;
  _v[3] = -2.22576;
  _v[4] = 26.1107;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266810;
  _v[0] = 1.0; 
  _v[1] = 2.96121;
  _v[2] = 43.4556;
  _v[3] = -2.15673;
  _v[4] = 26.4657;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266825;
  _v[0] = 1.0; 
  _v[1] = 2.99974;
  _v[2] = 38.0549;
  _v[3] = -2.87122;
  _v[4] = 25.3164;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266844;
  _v[0] = 1.0; 
  _v[1] = 2.20318;
  _v[2] = 32.7118;
  _v[3] = -2.33825;
  _v[4] = 24.8799;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266850;
  _v[0] = 1.0; 
  _v[1] = 3.56452;
  _v[2] = 37.1141;
  _v[3] = -2.40244;
  _v[4] = 27.4453;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266851;
  _v[0] = 1.0; 
  _v[1] = 2.87906;
  _v[2] = 37.4673;
  _v[3] = -4.14955;
  _v[4] = 25.8593;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266870;
  _v[0] = 1.0; 
  _v[1] = 3.01132;
  _v[2] = 35.7986;
  _v[3] = -2.75498;
  _v[4] = 25.8253;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266875;
  _v[0] = 1.0; 
  _v[1] = 3.73195;
  _v[2] = 39.3115;
  _v[3] = -2.03605;
  _v[4] = 28.5503;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266879;
  _v[0] = 1.0; 
  _v[1] = 3.6614;
  _v[2] = 45.8776;
  _v[3] = -3.69568;
  _v[4] = 26.5126;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266893;
  _v[0] = 1.0; 
  _v[1] = 2.46932;
  _v[2] = 30.9812;
  _v[3] = -1.62811;
  _v[4] = 24.9994;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266894;
  _v[0] = 1.0; 
  _v[1] = 2.50643;
  _v[2] = 39.2947;
  _v[3] = -1.45401;
  _v[4] = 26.9459;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266902;
  _v[0] = 1.0; 
  _v[1] = 4.00013;
  _v[2] = 56.6105;
  _v[3] = -11.0573;
  _v[4] = 22.3986;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266917;
  _v[0] = 1.0; 
  _v[1] = 2.49729;
  _v[2] = 37.1543;
  _v[3] = -1.66459;
  _v[4] = 25.6782;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266929;
  _v[0] = 1.0; 
  _v[1] = 2.85257;
  _v[2] = 35.4466;
  _v[3] = -2.4512;
  _v[4] = 26.2981;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266964;
  _v[0] = 1.0; 
  _v[1] = 3.09746;
  _v[2] = 37.3315;
  _v[3] = -2.04592;
  _v[4] = 27.2903;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266982;
  _v[0] = 1.0; 
  _v[1] = 3.02506;
  _v[2] = 37.5991;
  _v[3] = -3.05404;
  _v[4] = 25.9691;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266994;
  _v[0] = 1.0; 
  _v[1] = 4.03392;
  _v[2] = 31.6714;
  _v[3] = -4.30479;
  _v[4] = 23.459;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 266995;
  _v[0] = 1.0; 
  _v[1] = 3.28875;
  _v[2] = 42.5695;
  _v[3] = -3.17131;
  _v[4] = 26.707;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267009;
  _v[0] = 1.0; 
  _v[1] = 2.8589;
  _v[2] = 33.3571;
  _v[3] = -2.7996;
  _v[4] = 24.0132;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267011;
  _v[0] = 1.0; 
  _v[1] = 2.73768;
  _v[2] = 41.2804;
  _v[3] = -1.95928;
  _v[4] = 26.3422;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267025;
  _v[0] = 1.0; 
  _v[1] = 2.48201;
  _v[2] = 35.5876;
  _v[3] = -2.43991;
  _v[4] = 25.2904;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267027;
  _v[0] = 1.0; 
  _v[1] = 2.65236;
  _v[2] = 41.8567;
  _v[3] = -1.82615;
  _v[4] = 27.256;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267041;
  _v[0] = 1.0; 
  _v[1] = 2.81545;
  _v[2] = 36.9061;
  _v[3] = -2.52086;
  _v[4] = 25.2417;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267095;
  _v[0] = 1.0; 
  _v[1] = 2.73087;
  _v[2] = 32.6518;
  _v[3] = -2.26214;
  _v[4] = 27.6833;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267097;
  _v[0] = 1.0; 
  _v[1] = 3.47902;
  _v[2] = 37.185;
  _v[3] = -1.45289;
  _v[4] = 28.2826;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267127;
  _v[0] = 1.0; 
  _v[1] = 3.50498;
  _v[2] = 37.5336;
  _v[3] = -3.22831;
  _v[4] = 25.4378;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267166;
  _v[0] = 1.0; 
  _v[1] = 2.65008;
  _v[2] = 33.1154;
  _v[3] = -2.41026;
  _v[4] = 23.61;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267174;
  _v[0] = 1.0; 
  _v[1] = 3.23251;
  _v[2] = 39.0746;
  _v[3] = -1.67379;
  _v[4] = 27.9936;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267191;
  _v[0] = 1.0; 
  _v[1] = 3.16506;
  _v[2] = 37.4067;
  _v[3] = -2.48202;
  _v[4] = 25.5887;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267206;
  _v[0] = 1.0; 
  _v[1] = 3.01439;
  _v[2] = 31.7083;
  _v[3] = -2.79306;
  _v[4] = 24.704;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267208;
  _v[0] = 1.0; 
  _v[1] = 3.23673;
  _v[2] = 39.6584;
  _v[3] = -3.00357;
  _v[4] = 26.4499;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267211;
  _v[0] = 1.0; 
  _v[1] = 4.52162;
  _v[2] = 43.7582;
  _v[3] = -4.14629;
  _v[4] = 24.6809;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267228;
  _v[0] = 1.0; 
  _v[1] = 2.55396;
  _v[2] = 33.7296;
  _v[3] = -1.68728;
  _v[4] = 24.9439;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267257;
  _v[0] = 1.0; 
  _v[1] = 2.83443;
  _v[2] = 35.7314;
  _v[3] = -2.13101;
  _v[4] = 25.9724;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267273;
  _v[0] = 1.0; 
  _v[1] = 3.2613;
  _v[2] = 33.6427;
  _v[3] = -3.04919;
  _v[4] = 23.7542;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267274;
  _v[0] = 1.0; 
  _v[1] = 1.73571;
  _v[2] = 39.6716;
  _v[3] = -0.249111;
  _v[4] = 25.2169;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267294;
  _v[0] = 1.0; 
  _v[1] = 2.807;
  _v[2] = 36.3405;
  _v[3] = -2.68847;
  _v[4] = 24.7068;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267333;
  _v[0] = 1.0; 
  _v[1] = 2.76796;
  _v[2] = 36.0866;
  _v[3] = -2.14179;
  _v[4] = 24.7946;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267334;
  _v[0] = 1.0; 
  _v[1] = 4.50264;
  _v[2] = 41.1588;
  _v[3] = -3.93519;
  _v[4] = 28.3919;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267359;
  _v[0] = 1.0; 
  _v[1] = 3.53883;
  _v[2] = 35.9657;
  _v[3] = -3.08606;
  _v[4] = 26.0654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267371;
  _v[0] = 1.0; 
  _v[1] = 3.04671;
  _v[2] = 35.8553;
  _v[3] = -3.00813;
  _v[4] = 25.1644;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267393;
  _v[0] = 1.0; 
  _v[1] = 2.71312;
  _v[2] = 34.789;
  _v[3] = -2.58281;
  _v[4] = 24.6858;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267419;
  _v[0] = 1.0; 
  _v[1] = 3.08453;
  _v[2] = 29.109;
  _v[3] = -2.99274;
  _v[4] = 23.8482;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267420;
  _v[0] = 1.0; 
  _v[1] = 2.32652;
  _v[2] = 34.7237;
  _v[3] = -1.23747;
  _v[4] = 26.9425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267436;
  _v[0] = 1.0; 
  _v[1] = 3.11736;
  _v[2] = 34.4892;
  _v[3] = -2.87297;
  _v[4] = 24.1078;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267437;
  _v[0] = 1.0; 
  _v[1] = 3.98163;
  _v[2] = 38.4937;
  _v[3] = -2.9737;
  _v[4] = 26.7452;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267446;
  _v[0] = 1.0; 
  _v[1] = 3.27617;
  _v[2] = 32.7771;
  _v[3] = -2.96951;
  _v[4] = 25.9278;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267449;
  _v[0] = 1.0; 
  _v[1] = 1.92497;
  _v[2] = 38.3357;
  _v[3] = 1.426;
  _v[4] = 27.4717;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267471;
  _v[0] = 1.0; 
  _v[1] = 2.81573;
  _v[2] = 30.2203;
  _v[3] = -3.14954;
  _v[4] = 21.6338;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267473;
  _v[0] = 1.0; 
  _v[1] = 2.23719;
  _v[2] = 32.3886;
  _v[3] = -2.76487;
  _v[4] = 24.7531;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267499;
  _v[0] = 1.0; 
  _v[1] = 2.89589;
  _v[2] = 33.3453;
  _v[3] = -2.43757;
  _v[4] = 24.8755;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267500;
  _v[0] = 1.0; 
  _v[1] = 2.74863;
  _v[2] = 37.6347;
  _v[3] = -1.24284;
  _v[4] = 28.5063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267552;
  _v[0] = 1.0; 
  _v[1] = 2.91959;
  _v[2] = 32.451;
  _v[3] = -2.37274;
  _v[4] = 26.8052;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267563;
  _v[0] = 1.0; 
  _v[1] = 2.45463;
  _v[2] = 34.2533;
  _v[3] = -2.21496;
  _v[4] = 24.5661;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267582;
  _v[0] = 1.0; 
  _v[1] = 2.5;
  _v[2] = 35.4374;
  _v[3] = -2.41786;
  _v[4] = 26.3055;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267610;
  _v[0] = 1.0; 
  _v[1] = 2.53697;
  _v[2] = 36.9044;
  _v[3] = -1.4985;
  _v[4] = 26.4685;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267616;
  _v[0] = 1.0; 
  _v[1] = 4.05516;
  _v[2] = 42.1264;
  _v[3] = -3.86917;
  _v[4] = 27.6359;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267651;
  _v[0] = 1.0; 
  _v[1] = 5.63375;
  _v[2] = 32.8846;
  _v[3] = -5.42564;
  _v[4] = 28.7192;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));
  
  loEdge = 267656;
  _v[0] = 1.0; 
  _v[1] = 4.43246;
  _v[2] = 40.8712;
  _v[3] = -3.76163;
  _v[4] = 26.567;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

// p21-p25 data  
  loEdge = 268220;
  _v[0] = 1.0; 
  _v[1] = 3.48786;
  _v[2] = 35.9165;
  _v[3] = -2.45869;
  _v[4] = 27.0109;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268233;
  _v[0] = 1.0; 
  _v[1] = 3.37925;
  _v[2] = 26.5919;
  _v[3] = -1.65614;
  _v[4] = 26.0735;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268234;
  _v[0] = 1.0; 
  _v[1] = 3.37925;
  _v[2] = 26.5919;
  _v[3] = -1.65614;
  _v[4] = 26.0735;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268235;
  _v[0] = 1.0; 
  _v[1] = 3.54625;
  _v[2] = 33.9641;
  _v[3] = -3.22383;
  _v[4] = 25.0177;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268236;
  _v[0] = 1.0; 
  _v[1] = 3.57119;
  _v[2] = 37.4069;
  _v[3] = -2.54962;
  _v[4] = 26.502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268237;
  _v[0] = 1.0; 
  _v[1] = 3.57119;
  _v[2] = 37.4069;
  _v[3] = -2.54962;
  _v[4] = 26.502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268238;
  _v[0] = 1.0; 
  _v[1] = 3.57119;
  _v[2] = 37.4069;
  _v[3] = -2.54962;
  _v[4] = 26.502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268273;
  _v[0] = 1.0; 
  _v[1] = 3.16264;
  _v[2] = 34.2377;
  _v[3] = -1.68406;
  _v[4] = 26.4982;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268274;
  _v[0] = 1.0; 
  _v[1] = 3.16264;
  _v[2] = 34.2377;
  _v[3] = -1.68406;
  _v[4] = 26.4982;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268276;
  _v[0] = 1.0; 
  _v[1] = 3.16264;
  _v[2] = 34.2377;
  _v[3] = -1.68406;
  _v[4] = 26.4982;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268278;
  _v[0] = 1.0; 
  _v[1] = 3.16264;
  _v[2] = 34.2377;
  _v[3] = -1.68406;
  _v[4] = 26.4982;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268279;
  _v[0] = 1.0; 
  _v[1] = 2.64302;
  _v[2] = 37.1218;
  _v[3] = -0.821342;
  _v[4] = 28.9412;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268280;
  _v[0] = 1.0; 
  _v[1] = 2.64302;
  _v[2] = 37.1218;
  _v[3] = -0.821342;
  _v[4] = 28.9412;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268281;
  _v[0] = 1.0; 
  _v[1] = 2.64302;
  _v[2] = 37.1218;
  _v[3] = -0.821342;
  _v[4] = 28.9412;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268282;
  _v[0] = 1.0; 
  _v[1] = 4.04392;
  _v[2] = 45.1597;
  _v[3] = -5.37571;
  _v[4] = 26.9825;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268305;
  _v[0] = 1.0; 
  _v[1] = 3.61554;
  _v[2] = 35.9503;
  _v[3] = -2.16675;
  _v[4] = 27.7865;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268308;
  _v[0] = 1.0; 
  _v[1] = 4.13725;
  _v[2] = 39.1333;
  _v[3] = -6.05517;
  _v[4] = 27.4693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268310;
  _v[0] = 1.0; 
  _v[1] = 4.13725;
  _v[2] = 39.1333;
  _v[3] = -6.05517;
  _v[4] = 27.4693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268311;
  _v[0] = 1.0; 
  _v[1] = 4.13725;
  _v[2] = 39.1333;
  _v[3] = -6.05517;
  _v[4] = 27.4693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268356;
  _v[0] = 1.0; 
  _v[1] = 3.59847;
  _v[2] = 33.7209;
  _v[3] = -3.39448;
  _v[4] = 25.2913;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268358;
  _v[0] = 1.0; 
  _v[1] = 3.38411;
  _v[2] = 37.6493;
  _v[3] = -2.28081;
  _v[4] = 28.8126;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268359;
  _v[0] = 1.0; 
  _v[1] = 3.74584;
  _v[2] = 40.9084;
  _v[3] = -2.41645;
  _v[4] = 28.3517;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268360;
  _v[0] = 1.0; 
  _v[1] = 3.74584;
  _v[2] = 40.9084;
  _v[3] = -2.41645;
  _v[4] = 28.3517;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268395;
  _v[0] = 1.0; 
  _v[1] = 2.94278;
  _v[2] = 35.4952;
  _v[3] = -2.10178;
  _v[4] = 25.9113;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268398;
  _v[0] = 1.0; 
  _v[1] = 3.82704;
  _v[2] = 96.8693;
  _v[3] = -3.82103;
  _v[4] = 18.5383;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268417;
  _v[0] = 1.0; 
  _v[1] = 1.89176;
  _v[2] = 32.5375;
  _v[3] = -1.66508;
  _v[4] = 24.6944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268439;
  _v[0] = 1.0; 
  _v[1] = 3.2534;
  _v[2] = 34.3199;
  _v[3] = -2.63465;
  _v[4] = 24.5217;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268442;
  _v[0] = 1.0; 
  _v[1] = 2.82188;
  _v[2] = 42.2108;
  _v[3] = -2.72489;
  _v[4] = 27.6494;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268444;
  _v[0] = 1.0; 
  _v[1] = 2.82188;
  _v[2] = 42.2108;
  _v[3] = -2.72489;
  _v[4] = 27.6494;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268445;
  _v[0] = 1.0; 
  _v[1] = 3.37219;
  _v[2] = 42.9022;
  _v[3] = -3.94908;
  _v[4] = 27.6471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268449;
  _v[0] = 1.0; 
  _v[1] = 3.37219;
  _v[2] = 42.9022;
  _v[3] = -3.94908;
  _v[4] = 27.6471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268451;
  _v[0] = 1.0; 
  _v[1] = 3.37219;
  _v[2] = 42.9022;
  _v[3] = -3.94908;
  _v[4] = 27.6471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268452;
  _v[0] = 1.0; 
  _v[1] = 3.37219;
  _v[2] = 42.9022;
  _v[3] = -3.94908;
  _v[4] = 27.6471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268471;
  _v[0] = 1.0; 
  _v[1] = 2.68685;
  _v[2] = 36.732;
  _v[3] = -2.08902;
  _v[4] = 25.4189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268472;
  _v[0] = 1.0; 
  _v[1] = 2.68685;
  _v[2] = 36.732;
  _v[3] = -2.08902;
  _v[4] = 25.4189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268473;
  _v[0] = 1.0; 
  _v[1] = 2.283;
  _v[2] = 44.2526;
  _v[3] = -3.41264;
  _v[4] = 26.6617;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268492;
  _v[0] = 1.0; 
  _v[1] = 2.246;
  _v[2] = 29.7792;
  _v[3] = -1.74748;
  _v[4] = 22.5287;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268495;
  _v[0] = 1.0; 
  _v[1] = 2.246;
  _v[2] = 29.7792;
  _v[3] = -1.74748;
  _v[4] = 22.5287;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268496;
  _v[0] = 1.0; 
  _v[1] = 2.57104;
  _v[2] = 34.9171;
  _v[3] = -1.78283;
  _v[4] = 26.4795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268497;
  _v[0] = 1.0; 
  _v[1] = 4.78416;
  _v[2] = 39.4491;
  _v[3] = -4.64813;
  _v[4] = 26.6148;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268517;
  _v[0] = 1.0; 
  _v[1] = 4.78416;
  _v[2] = 39.4491;
  _v[3] = -4.64813;
  _v[4] = 26.6148;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268554;
  _v[0] = 1.0; 
  _v[1] = 2.16713;
  _v[2] = 35.0225;
  _v[3] = -1.55474;
  _v[4] = 25.7592;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268555;
  _v[0] = 1.0; 
  _v[1] = 2.16713;
  _v[2] = 35.0225;
  _v[3] = -1.55474;
  _v[4] = 25.7592;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268556;
  _v[0] = 1.0; 
  _v[1] = 3.39558;
  _v[2] = 36.9842;
  _v[3] = -2.0328;
  _v[4] = 29.5072;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268557;
  _v[0] = 1.0; 
  _v[1] = 3.39558;
  _v[2] = 36.9842;
  _v[3] = -2.0328;
  _v[4] = 29.5072;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268558;
  _v[0] = 1.0; 
  _v[1] = 1.66011;
  _v[2] = 41.0679;
  _v[3] = 0.518851;
  _v[4] = 28.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268559;
  _v[0] = 1.0; 
  _v[1] = 1.66011;
  _v[2] = 41.0679;
  _v[3] = 0.518851;
  _v[4] = 28.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268560;
  _v[0] = 1.0; 
  _v[1] = 1.66011;
  _v[2] = 41.0679;
  _v[3] = 0.518851;
  _v[4] = 28.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268561;
  _v[0] = 1.0; 
  _v[1] = 1.66011;
  _v[2] = 41.0679;
  _v[3] = 0.518851;
  _v[4] = 28.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268576;
  _v[0] = 1.0; 
  _v[1] = 2.38416;
  _v[2] = 30.4924;
  _v[3] = -1.77761;
  _v[4] = 23.7135;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268577;
  _v[0] = 1.0; 
  _v[1] = 2.38416;
  _v[2] = 30.4924;
  _v[3] = -1.77761;
  _v[4] = 23.7135;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268610;
  _v[0] = 1.0; 
  _v[1] = 1.69316;
  _v[2] = 29.8899;
  _v[3] = -0.317365;
  _v[4] = 25.7965;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268611;
  _v[0] = 1.0; 
  _v[1] = 3.85612;
  _v[2] = 36.2709;
  _v[3] = -3.86029;
  _v[4] = 26.6872;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268612;
  _v[0] = 1.0; 
  _v[1] = 2.9208;
  _v[2] = 38.0993;
  _v[3] = -1.24912;
  _v[4] = 28.3494;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268613;
  _v[0] = 1.0; 
  _v[1] = 3.73176;
  _v[2] = 42.9374;
  _v[3] = -4.6636;
  _v[4] = 24.9733;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268617;
  _v[0] = 1.0; 
  _v[1] = 3.73176;
  _v[2] = 42.9374;
  _v[3] = -4.6636;
  _v[4] = 24.9733;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268618;
  _v[0] = 1.0; 
  _v[1] = 3.73176;
  _v[2] = 42.9374;
  _v[3] = -4.6636;
  _v[4] = 24.9733;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268619;
  _v[0] = 1.0; 
  _v[1] = 3.73176;
  _v[2] = 42.9374;
  _v[3] = -4.6636;
  _v[4] = 24.9733;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268644;
  _v[0] = 1.0; 
  _v[1] = 2.47267;
  _v[2] = 31.2622;
  _v[3] = -2.49784;
  _v[4] = 25.0502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268645;
  _v[0] = 1.0; 
  _v[1] = 3.22591;
  _v[2] = 35.9049;
  _v[3] = -3.04032;
  _v[4] = 28.3762;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268647;
  _v[0] = 1.0; 
  _v[1] = 3.22591;
  _v[2] = 35.9049;
  _v[3] = -3.04032;
  _v[4] = 28.3762;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268653;
  _v[0] = 1.0; 
  _v[1] = 3.22591;
  _v[2] = 35.9049;
  _v[3] = -3.04032;
  _v[4] = 28.3762;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268662;
  _v[0] = 1.0; 
  _v[1] = 2.61891;
  _v[2] = 32.3431;
  _v[3] = -2.94696;
  _v[4] = 23.7688;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268667;
  _v[0] = 1.0; 
  _v[1] = 3.1125;
  _v[2] = 36.9173;
  _v[3] = -2.57415;
  _v[4] = 27.8004;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268668;
  _v[0] = 1.0; 
  _v[1] = 4.28776;
  _v[2] = 41.2642;
  _v[3] = -6.50012;
  _v[4] = 23.7594;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268676;
  _v[0] = 1.0; 
  _v[1] = 2.80209;
  _v[2] = 35.7825;
  _v[3] = -2.25917;
  _v[4] = 25.3303;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268677;
  _v[0] = 1.0; 
  _v[1] = 3.21372;
  _v[2] = 40.4209;
  _v[3] = -2.86538;
  _v[4] = 28.1541;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268755;
  _v[0] = 1.0; 
  _v[1] = 2.87402;
  _v[2] = 29.6109;
  _v[3] = -1.59381;
  _v[4] = 28.0794;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268756;
  _v[0] = 1.0; 
  _v[1] = 3.21586;
  _v[2] = 37.2034;
  _v[3] = -2.75149;
  _v[4] = 27.3727;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268757;
  _v[0] = 1.0; 
  _v[1] = 3.21586;
  _v[2] = 37.2034;
  _v[3] = -2.75149;
  _v[4] = 27.3727;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268761;
  _v[0] = 1.0; 
  _v[1] = 3.21586;
  _v[2] = 37.2034;
  _v[3] = -2.75149;
  _v[4] = 27.3727;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268804;
  _v[0] = 1.0; 
  _v[1] = 3.95035;
  _v[2] = 35.1029;
  _v[3] = -3.87661;
  _v[4] = 29.3683;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268819;
  _v[0] = 1.0; 
  _v[1] = 3.4644;
  _v[2] = 35.8702;
  _v[3] = -2.41207;
  _v[4] = 28.5211;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268852;
  _v[0] = 1.0; 
  _v[1] = 3.46846;
  _v[2] = 31.8954;
  _v[3] = -2.53317;
  _v[4] = 26.4419;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268879;
  _v[0] = 1.0; 
  _v[1] = 3.53063;
  _v[2] = 32.7809;
  _v[3] = -3.89811;
  _v[4] = 24.5598;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268880;
  _v[0] = 1.0; 
  _v[1] = 2.36975;
  _v[2] = 36.4463;
  _v[3] = -2.4381;
  _v[4] = 27.772;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268881;
  _v[0] = 1.0; 
  _v[1] = 1.69373;
  _v[2] = 39.6454;
  _v[3] = -0.165153;
  _v[4] = 27.6656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268882;
  _v[0] = 1.0; 
  _v[1] = 1.69373;
  _v[2] = 39.6454;
  _v[3] = -0.165153;
  _v[4] = 27.6656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268891;
  _v[0] = 1.0; 
  _v[1] = 3.20209;
  _v[2] = 29.1835;
  _v[3] = -3.46878;
  _v[4] = 23.1504;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268892;
  _v[0] = 1.0; 
  _v[1] = 1.81424;
  _v[2] = 35.2984;
  _v[3] = -1.10025;
  _v[4] = 22.9101;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268894;
  _v[0] = 1.0; 
  _v[1] = 1.81424;
  _v[2] = 35.2984;
  _v[3] = -1.10025;
  _v[4] = 22.9101;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268896;
  _v[0] = 1.0; 
  _v[1] = 4.60004;
  _v[2] = 33.3375;
  _v[3] = -3.08913;
  _v[4] = 28.5114;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268897;
  _v[0] = 1.0; 
  _v[1] = 3.44778;
  _v[2] = 41.0383;
  _v[3] = -3.25486;
  _v[4] = 27.3383;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268906;
  _v[0] = 1.0; 
  _v[1] = 2.62925;
  _v[2] = 36.3525;
  _v[3] = -2.41369;
  _v[4] = 24.7762;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268922;
  _v[0] = 1.0; 
  _v[1] = 2.90731;
  _v[2] = 30.073;
  _v[3] = -2.97335;
  _v[4] = 23.5071;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268923;
  _v[0] = 1.0; 
  _v[1] = 2.51601;
  _v[2] = 34.529;
  _v[3] = -1.93309;
  _v[4] = 26.4129;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268924;
  _v[0] = 1.0; 
  _v[1] = 4.25949;
  _v[2] = 38.1078;
  _v[3] = -3.99124;
  _v[4] = 26.3971;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268962;
  _v[0] = 1.0; 
  _v[1] = 3.12668;
  _v[2] = 35.9214;
  _v[3] = -3.56908;
  _v[4] = 25.313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268963;
  _v[0] = 1.0; 
  _v[1] = 3.12668;
  _v[2] = 35.9214;
  _v[3] = -3.56908;
  _v[4] = 25.313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268977;
  _v[0] = 1.0; 
  _v[1] = 2.516;
  _v[2] = 29.0328;
  _v[3] = -2.07827;
  _v[4] = 23.9207;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268978;
  _v[0] = 1.0; 
  _v[1] = 2.23712;
  _v[2] = 35.558;
  _v[3] = -1.70559;
  _v[4] = 24.5058;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268980;
  _v[0] = 1.0; 
  _v[1] = 2.23712;
  _v[2] = 35.558;
  _v[3] = -1.70559;
  _v[4] = 24.5058;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 268983;
  _v[0] = 1.0; 
  _v[1] = 3.28053;
  _v[2] = 38.9471;
  _v[3] = -2.548;
  _v[4] = 27.8218;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269002;
  _v[0] = 1.0; 
  _v[1] = 3.03615;
  _v[2] = 37.9468;
  _v[3] = -3.07545;
  _v[4] = 24.5787;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269003;
  _v[0] = 1.0; 
  _v[1] = 3.03615;
  _v[2] = 37.9468;
  _v[3] = -3.07545;
  _v[4] = 24.5787;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269007;
  _v[0] = 1.0; 
  _v[1] = 5.10075;
  _v[2] = 46.2022;
  _v[3] = -12.3973;
  _v[4] = 24.462;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269588;
  _v[0] = 1.0; 
  _v[1] = 3.46023;
  _v[2] = 35.2783;
  _v[3] = -2.6276;
  _v[4] = 24.6849;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269603;
  _v[0] = 1.0; 
  _v[1] = 2.91999;
  _v[2] = 35.7666;
  _v[3] = -2.76895;
  _v[4] = 23.9201;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269605;
  _v[0] = 1.0; 
  _v[1] = 3.53839;
  _v[2] = 41.9158;
  _v[3] = -3.96912;
  _v[4] = 25.7529;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269606;
  _v[0] = 1.0; 
  _v[1] = 6.98221;
  _v[2] = 41.948;
  _v[3] = -10.2828;
  _v[4] = 28.6892;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269622;
  _v[0] = 1.0; 
  _v[1] = 4.20383;
  _v[2] = 28.081;
  _v[3] = -3.35966;
  _v[4] = 24.1215;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269623;
  _v[0] = 1.0; 
  _v[1] = 3.53571;
  _v[2] = 38.0236;
  _v[3] = -3.01027;
  _v[4] = 25.7574;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269703;
  _v[0] = 1.0; 
  _v[1] = 3.11992;
  _v[2] = 30.3941;
  _v[3] = -2.27767;
  _v[4] = 24.0172;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269704;
  _v[0] = 1.0; 
  _v[1] = 3.37359;
  _v[2] = 37.749;
  _v[3] = -2.24104;
  _v[4] = 26.3293;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269726;
  _v[0] = 1.0; 
  _v[1] = 2.85613;
  _v[2] = 32.8474;
  _v[3] = -1.86318;
  _v[4] = 24.7187;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269727;
  _v[0] = 1.0; 
  _v[1] = 4.62977;
  _v[2] = 38.331;
  _v[3] = -4.78665;
  _v[4] = 27.6033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269730;
  _v[0] = 1.0; 
  _v[1] = 3.64294;
  _v[2] = 31.9826;
  _v[3] = -4.00622;
  _v[4] = 23.3719;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269731;
  _v[0] = 1.0; 
  _v[1] = 3.95782;
  _v[2] = 34.975;
  _v[3] = -1.61041;
  _v[4] = 26.257;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269732;
  _v[0] = 1.0; 
  _v[1] = 3.95782;
  _v[2] = 34.975;
  _v[3] = -1.61041;
  _v[4] = 26.257;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269734;
  _v[0] = 1.0; 
  _v[1] = 3.73737;
  _v[2] = 32.688;
  _v[3] = 0.253314;
  _v[4] = 27.9766;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269745;
  _v[0] = 1.0; 
  _v[1] = 2.93501;
  _v[2] = 33.5282;
  _v[3] = -1.77694;
  _v[4] = 25.6254;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269746;
  _v[0] = 1.0; 
  _v[1] = 3.35585;
  _v[2] = 38.8032;
  _v[3] = -2.3998;
  _v[4] = 27.5126;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269772;
  _v[0] = 1.0; 
  _v[1] = 2.29829;
  _v[2] = 31.2066;
  _v[3] = -1.14382;
  _v[4] = 22.7372;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269780;
  _v[0] = 1.0; 
  _v[1] = 3.89956;
  _v[2] = 34.7022;
  _v[3] = -3.51921;
  _v[4] = 25.3948;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269781;
  _v[0] = 1.0; 
  _v[1] = 4.78093;
  _v[2] = 36.5277;
  _v[3] = -4.85034;
  _v[4] = 26.4115;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269782;
  _v[0] = 1.0; 
  _v[1] = 4.78093;
  _v[2] = 36.5277;
  _v[3] = -4.85034;
  _v[4] = 26.4115;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269783;
  _v[0] = 1.0; 
  _v[1] = 3.86187;
  _v[2] = 45.373;
  _v[3] = -2.5561;
  _v[4] = 25.6918;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269784;
  _v[0] = 1.0; 
  _v[1] = 3.86187;
  _v[2] = 45.373;
  _v[3] = -2.5561;
  _v[4] = 25.6918;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269807;
  _v[0] = 1.0; 
  _v[1] = 2.60523;
  _v[2] = 31.79;
  _v[3] = -2.458;
  _v[4] = 23.3652;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269814;
  _v[0] = 1.0; 
  _v[1] = -1.14539;
  _v[2] = 35.2612;
  _v[3] = 4.67103;
  _v[4] = 26.0669;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269878;
  _v[0] = 1.0; 
  _v[1] = 3.06014;
  _v[2] = 29.0785;
  _v[3] = -3.27725;
  _v[4] = 22.0034;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269882;
  _v[0] = 1.0; 
  _v[1] = 3.84549;
  _v[2] = 34.0123;
  _v[3] = -4.03939;
  _v[4] = 25.4394;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269885;
  _v[0] = 1.0; 
  _v[1] = 3.44298;
  _v[2] = 38.0175;
  _v[3] = -1.5015;
  _v[4] = 26.9032;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269888;
  _v[0] = 1.0; 
  _v[1] = 3.44298;
  _v[2] = 38.0175;
  _v[3] = -1.5015;
  _v[4] = 26.9032;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269922;
  _v[0] = 1.0; 
  _v[1] = 2.20941;
  _v[2] = 31.9265;
  _v[3] = -2.5662;
  _v[4] = 22.0549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269923;
  _v[0] = 1.0; 
  _v[1] = 2.20941;
  _v[2] = 31.9265;
  _v[3] = -2.5662;
  _v[4] = 22.0549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269924;
  _v[0] = 1.0; 
  _v[1] = 4.06438;
  _v[2] = 34.4818;
  _v[3] = -3.4955;
  _v[4] = 25.5554;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269925;
  _v[0] = 1.0; 
  _v[1] = 4.06438;
  _v[2] = 34.4818;
  _v[3] = -3.4955;
  _v[4] = 25.5554;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269926;
  _v[0] = 1.0; 
  _v[1] = -6.35092;
  _v[2] = 555.803;
  _v[3] = 2301.06;
  _v[4] = 27.7858;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269943;
  _v[0] = 1.0; 
  _v[1] = 2.70764;
  _v[2] = 35.5337;
  _v[3] = -1.78875;
  _v[4] = 24.7706;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269944;
  _v[0] = 1.0; 
  _v[1] = 2.99419;
  _v[2] = 39.8335;
  _v[3] = -0.935481;
  _v[4] = 28.3336;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269966;
  _v[0] = 1.0; 
  _v[1] = 2.38411;
  _v[2] = 34.8178;
  _v[3] = -1.41869;
  _v[4] = 23.9922;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269967;
  _v[0] = 1.0; 
  _v[1] = 2.96816;
  _v[2] = 40.3505;
  _v[3] = -1.79573;
  _v[4] = 29.3157;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269968;
  _v[0] = 1.0; 
  _v[1] = 2.96816;
  _v[2] = 40.3505;
  _v[3] = -1.79573;
  _v[4] = 29.3157;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 269985;
  _v[0] = 1.0; 
  _v[1] = 2.76189;
  _v[2] = 35.9758;
  _v[3] = -1.9612;
  _v[4] = 23.7019;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270000;
  _v[0] = 1.0; 
  _v[1] = 3.28251;
  _v[2] = 31.8918;
  _v[3] = -2.46713;
  _v[4] = 23.9508;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270002;
  _v[0] = 1.0; 
  _v[1] = 2.8597;
  _v[2] = 35.9397;
  _v[3] = -1.98848;
  _v[4] = 25.5403;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270004;
  _v[0] = 1.0; 
  _v[1] = 2.8597;
  _v[2] = 35.9397;
  _v[3] = -1.98848;
  _v[4] = 25.5403;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270006;
  _v[0] = 1.0; 
  _v[1] = 3.5948;
  _v[2] = 38.7047;
  _v[3] = -3.46036;
  _v[4] = 26.9962;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270007;
  _v[0] = 1.0; 
  _v[1] = 3.5948;
  _v[2] = 38.7047;
  _v[3] = -3.46036;
  _v[4] = 26.9962;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270010;
  _v[0] = 1.0; 
  _v[1] = 3.5948;
  _v[2] = 38.7047;
  _v[3] = -3.46036;
  _v[4] = 26.9962;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270011;
  _v[0] = 1.0; 
  _v[1] = 1.72498;
  _v[2] = 36.037;
  _v[3] = -0.121878;
  _v[4] = 28.7673;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270026;
  _v[0] = 1.0; 
  _v[1] = 3.25176;
  _v[2] = 34.8864;
  _v[3] = -2.88353;
  _v[4] = 24.9953;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270028;
  _v[0] = 1.0; 
  _v[1] = -0.140191;
  _v[2] = 32933.4;
  _v[3] = 24293.2;
  _v[4] = 18.9762;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270045;
  _v[0] = 1.0; 
  _v[1] = 3.06062;
  _v[2] = 31.468;
  _v[3] = -2.97865;
  _v[4] = 23.472;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270046;
  _v[0] = 1.0; 
  _v[1] = 3.18654;
  _v[2] = 38.1606;
  _v[3] = -2.73833;
  _v[4] = 26.805;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270062;
  _v[0] = 1.0; 
  _v[1] = 2.42185;
  _v[2] = 36.4204;
  _v[3] = -1.85244;
  _v[4] = 25.2706;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270063;
  _v[0] = 1.0; 
  _v[1] = 2.42185;
  _v[2] = 36.4204;
  _v[3] = -1.85244;
  _v[4] = 25.2706;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270083;
  _v[0] = 1.0; 
  _v[1] = 2.22492;
  _v[2] = 30.3735;
  _v[3] = -1.69658;
  _v[4] = 24.2838;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270084;
  _v[0] = 1.0; 
  _v[1] = 2.93164;
  _v[2] = 40.4846;
  _v[3] = -2.53057;
  _v[4] = 27.4058;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270085;
  _v[0] = 1.0; 
  _v[1] = 2.93164;
  _v[2] = 40.4846;
  _v[3] = -2.53057;
  _v[4] = 27.4058;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270088;
  _v[0] = 1.0; 
  _v[1] = 3.77957;
  _v[2] = 44.9425;
  _v[3] = -3.21105;
  _v[4] = 28.7484;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270108;
  _v[0] = 1.0; 
  _v[1] = 3.02888;
  _v[2] = 36.8586;
  _v[3] = -2.44052;
  _v[4] = 25.5075;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270116;
  _v[0] = 1.0; 
  _v[1] = 2.77883;
  _v[2] = 36.9075;
  _v[3] = -2.62228;
  _v[4] = 25.8904;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270121;
  _v[0] = 1.0; 
  _v[1] = 2.17296;
  _v[2] = 45.0922;
  _v[3] = -2.73879;
  _v[4] = 26.8837;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270134;
  _v[0] = 1.0; 
  _v[1] = 2.29796;
  _v[2] = 29.8828;
  _v[3] = -1.15839;
  _v[4] = 23.9054;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270135;
  _v[0] = 1.0; 
  _v[1] = 2.29796;
  _v[2] = 29.8828;
  _v[3] = -1.15839;
  _v[4] = 23.9054;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270136;
  _v[0] = 1.0; 
  _v[1] = 2.29796;
  _v[2] = 29.8828;
  _v[3] = -1.15839;
  _v[4] = 23.9054;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270137;
  _v[0] = 1.0; 
  _v[1] = 3.04281;
  _v[2] = 34.5348;
  _v[3] = -3.52041;
  _v[4] = 24.7401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270138;
  _v[0] = 1.0; 
  _v[1] = 3.04281;
  _v[2] = 34.5348;
  _v[3] = -3.52041;
  _v[4] = 24.7401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270140;
  _v[0] = 1.0; 
  _v[1] = 1.7062;
  _v[2] = 39.8612;
  _v[3] = -1.18803;
  _v[4] = 26.1077;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270157;
  _v[0] = 1.0; 
  _v[1] = 3.90678;
  _v[2] = 26.6655;
  _v[3] = -2.51921;
  _v[4] = 24.0633;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270159;
  _v[0] = 1.0; 
  _v[1] = 3.48505;
  _v[2] = 32.0738;
  _v[3] = -2.38284;
  _v[4] = 26.1863;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270191;
  _v[0] = 1.0; 
  _v[1] = 2.97891;
  _v[2] = 35.8402;
  _v[3] = -2.83579;
  _v[4] = 24.99;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270192;
  _v[0] = 1.0; 
  _v[1] = 3.12851;
  _v[2] = 41.3324;
  _v[3] = -2.43021;
  _v[4] = 26.2749;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270193;
  _v[0] = 1.0; 
  _v[1] = 3.12851;
  _v[2] = 41.3324;
  _v[3] = -2.43021;
  _v[4] = 26.2749;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270194;
  _v[0] = 1.0; 
  _v[1] = 4.43837;
  _v[2] = 44.1557;
  _v[3] = -6.42471;
  _v[4] = 24.5656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270196;
  _v[0] = 1.0; 
  _v[1] = 4.43837;
  _v[2] = 44.1557;
  _v[3] = -6.42471;
  _v[4] = 24.5656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270197;
  _v[0] = 1.0; 
  _v[1] = 4.43837;
  _v[2] = 44.1557;
  _v[3] = -6.42471;
  _v[4] = 24.5656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270212;
  _v[0] = 1.0; 
  _v[1] = 2.97264;
  _v[2] = 31.6539;
  _v[3] = -2.49801;
  _v[4] = 24.3324;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270224;
  _v[0] = 1.0; 
  _v[1] = 3.4272;
  _v[2] = 40.0885;
  _v[3] = -2.62655;
  _v[4] = 26.7474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270228;
  _v[0] = 1.0; 
  _v[1] = 3.4272;
  _v[2] = 40.0885;
  _v[3] = -2.62655;
  _v[4] = 26.7474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270229;
  _v[0] = 1.0; 
  _v[1] = 3.4272;
  _v[2] = 40.0885;
  _v[3] = -2.62655;
  _v[4] = 26.7474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270230;
  _v[0] = 1.0; 
  _v[1] = 3.4272;
  _v[2] = 40.0885;
  _v[3] = -2.62655;
  _v[4] = 26.7474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270233;
  _v[0] = 1.0; 
  _v[1] = 3.4272;
  _v[2] = 40.0885;
  _v[3] = -2.62655;
  _v[4] = 26.7474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270257;
  _v[0] = 1.0; 
  _v[1] = 2.7909;
  _v[2] = 33.5917;
  _v[3] = -2.33974;
  _v[4] = 24.7806;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270300;
  _v[0] = 1.0; 
  _v[1] = 2.80587;
  _v[2] = 31.6229;
  _v[3] = -2.65514;
  _v[4] = 24.3852;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270302;
  _v[0] = 1.0; 
  _v[1] = 2.80587;
  _v[2] = 31.6229;
  _v[3] = -2.65514;
  _v[4] = 24.3852;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270349;
  _v[0] = 1.0; 
  _v[1] = 2.23935;
  _v[2] = 35.3536;
  _v[3] = -1.54619;
  _v[4] = 24.9843;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270350;
  _v[0] = 1.0; 
  _v[1] = 2.23935;
  _v[2] = 35.3536;
  _v[3] = -1.54619;
  _v[4] = 24.9843;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270372;
  _v[0] = 1.0; 
  _v[1] = 2.18532;
  _v[2] = 32.5363;
  _v[3] = -1.91052;
  _v[4] = 24.8182;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270373;
  _v[0] = 1.0; 
  _v[1] = 3.16381;
  _v[2] = 35.8246;
  _v[3] = -1.64857;
  _v[4] = 29.4996;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270374;
  _v[0] = 1.0; 
  _v[1] = 3.16381;
  _v[2] = 35.8246;
  _v[3] = -1.64857;
  _v[4] = 29.4996;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270395;
  _v[0] = 1.0; 
  _v[1] = 2.59188;
  _v[2] = 28.5292;
  _v[3] = -2.72818;
  _v[4] = 23.6914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270396;
  _v[0] = 1.0; 
  _v[1] = 2.59188;
  _v[2] = 28.5292;
  _v[3] = -2.72818;
  _v[4] = 23.6914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270397;
  _v[0] = 1.0; 
  _v[1] = 3.75726;
  _v[2] = 32.1276;
  _v[3] = -3.42565;
  _v[4] = 25.5697;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270398;
  _v[0] = 1.0; 
  _v[1] = 2.52458;
  _v[2] = 36.4931;
  _v[3] = -1.6521;
  _v[4] = 26.5146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270419;
  _v[0] = 1.0; 
  _v[1] = 2.94204;
  _v[2] = 34.0378;
  _v[3] = -3.00254;
  _v[4] = 24.67;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270433;
  _v[0] = 1.0; 
  _v[1] = 2.71729;
  _v[2] = 28.5374;
  _v[3] = -2.15646;
  _v[4] = 22.3004;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270434;
  _v[0] = 1.0; 
  _v[1] = 2.87917;
  _v[2] = 36.7085;
  _v[3] = -2.39726;
  _v[4] = 25.859;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270450;
  _v[0] = 1.0; 
  _v[1] = 1.64414;
  _v[2] = 31.9828;
  _v[3] = -1.11183;
  _v[4] = 24.5875;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270451;
  _v[0] = 1.0; 
  _v[1] = 3.17829;
  _v[2] = 39.9543;
  _v[3] = -2.53874;
  _v[4] = 27.0757;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270465;
  _v[0] = 1.0; 
  _v[1] = 2.53673;
  _v[2] = 34.9605;
  _v[3] = -1.84949;
  _v[4] = 23.9143;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270484;
  _v[0] = 1.0; 
  _v[1] = 2.33046;
  _v[2] = 34.3666;
  _v[3] = -1.68949;
  _v[4] = 25.6646;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270507;
  _v[0] = 1.0; 
  _v[1] = 2.51372;
  _v[2] = 36.7517;
  _v[3] = -2.17524;
  _v[4] = 25.217;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270523;
  _v[0] = 1.0; 
  _v[1] = 2.68293;
  _v[2] = 29.7392;
  _v[3] = -2.03603;
  _v[4] = 23.4263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270525;
  _v[0] = 1.0; 
  _v[1] = 2.94153;
  _v[2] = 37.6947;
  _v[3] = -2.54226;
  _v[4] = 25.5076;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270535;
  _v[0] = 1.0; 
  _v[1] = 2.19658;
  _v[2] = 33.0179;
  _v[3] = -1.27458;
  _v[4] = 24.5202;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270537;
  _v[0] = 1.0; 
  _v[1] = 2.96455;
  _v[2] = 38.6278;
  _v[3] = -3.90027;
  _v[4] = 25.8345;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270561;
  _v[0] = 1.0; 
  _v[1] = 2.59651;
  _v[2] = 35.7555;
  _v[3] = -2.0742;
  _v[4] = 25.2904;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270575;
  _v[0] = 1.0; 
  _v[1] = 3.00119;
  _v[2] = 36.0567;
  _v[3] = -2.85836;
  _v[4] = 24.3639;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270588;
  _v[0] = 1.0; 
  _v[1] = 2.66836;
  _v[2] = 33.681;
  _v[3] = -2.23025;
  _v[4] = 25.7719;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270624;
  _v[0] = 1.0; 
  _v[1] = 3.10181;
  _v[2] = 34.6109;
  _v[3] = -2.64427;
  _v[4] = 24.2963;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270628;
  _v[0] = 1.0; 
  _v[1] = 1.77232;
  _v[2] = 42.5383;
  _v[3] = -0.513903;
  _v[4] = 27.7384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270629;
  _v[0] = 1.0; 
  _v[1] = 1.77232;
  _v[2] = 42.5383;
  _v[3] = -0.513903;
  _v[4] = 27.7384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270630;
  _v[0] = 1.0; 
  _v[1] = 1.77232;
  _v[2] = 42.5383;
  _v[3] = -0.513903;
  _v[4] = 27.7384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270647;
  _v[0] = 1.0; 
  _v[1] = 2.6353;
  _v[2] = 30.9994;
  _v[3] = -1.5115;
  _v[4] = 25.4973;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270682;
  _v[0] = 1.0; 
  _v[1] = 2.67709;
  _v[2] = 31.7599;
  _v[3] = -2.02922;
  _v[4] = 23.7997;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270683;
  _v[0] = 1.0; 
  _v[1] = 2.67709;
  _v[2] = 31.7599;
  _v[3] = -2.02922;
  _v[4] = 23.7997;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270684;
  _v[0] = 1.0; 
  _v[1] = 3.56873;
  _v[2] = 37.159;
  _v[3] = -2.36116;
  _v[4] = 24.8212;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270685;
  _v[0] = 1.0; 
  _v[1] = 2.76672;
  _v[2] = 37.8243;
  _v[3] = -2.68828;
  _v[4] = 27.5642;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270686;
  _v[0] = 1.0; 
  _v[1] = 2.76672;
  _v[2] = 37.8243;
  _v[3] = -2.68828;
  _v[4] = 27.5642;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270690;
  _v[0] = 1.0; 
  _v[1] = 3.19153;
  _v[2] = 41.3684;
  _v[3] = -0.956266;
  _v[4] = 27.987;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270708;
  _v[0] = 1.0; 
  _v[1] = 4.2016;
  _v[2] = 31.4361;
  _v[3] = -2.95068;
  _v[4] = 25.1326;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270731;
  _v[0] = 1.0; 
  _v[1] = 2.56345;
  _v[2] = 36.1652;
  _v[3] = -2.20287;
  _v[4] = 25.8873;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270746;
  _v[0] = 1.0; 
  _v[1] = 1.99604;
  _v[2] = 34.2268;
  _v[3] = -2.19676;
  _v[4] = 24.7047;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270769;
  _v[0] = 1.0; 
  _v[1] = 2.14589;
  _v[2] = 36.4459;
  _v[3] = -2.5877;
  _v[4] = 25.6279;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270784;
  _v[0] = 1.0; 
  _v[1] = 1.99022;
  _v[2] = 32.5123;
  _v[3] = -1.73998;
  _v[4] = 25.2271;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270786;
  _v[0] = 1.0; 
  _v[1] = 3.65441;
  _v[2] = 41.2012;
  _v[3] = -3.53977;
  _v[4] = 27.6048;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270808;
  _v[0] = 1.0; 
  _v[1] = 1.74018;
  _v[2] = 36.0245;
  _v[3] = -1.40706;
  _v[4] = 25.1519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270823;
  _v[0] = 1.0; 
  _v[1] = 2.36658;
  _v[2] = 34.3351;
  _v[3] = -2.67138;
  _v[4] = 24.4911;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270824;
  _v[0] = 1.0; 
  _v[1] = 2.42279;
  _v[2] = 40.1927;
  _v[3] = -1.58086;
  _v[4] = 28.4394;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270844;
  _v[0] = 1.0; 
  _v[1] = 2.62074;
  _v[2] = 34.1568;
  _v[3] = -2.68141;
  _v[4] = 25.0518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270860;
  _v[0] = 1.0; 
  _v[1] = 2.24202;
  _v[2] = 35.0685;
  _v[3] = -2.11992;
  _v[4] = 25.1448;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270863;
  _v[0] = 1.0; 
  _v[1] = 2.24202;
  _v[2] = 35.0685;
  _v[3] = -2.11992;
  _v[4] = 25.1448;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270864;
  _v[0] = 1.0; 
  _v[1] = 2.24202;
  _v[2] = 35.0685;
  _v[3] = -2.11992;
  _v[4] = 25.1448;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270883;
  _v[0] = 1.0; 
  _v[1] = 2.24378;
  _v[2] = 30.9081;
  _v[3] = -2.29848;
  _v[4] = 24.059;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270884;
  _v[0] = 1.0; 
  _v[1] = 2.21973;
  _v[2] = 38.0337;
  _v[3] = -1.42063;
  _v[4] = 25.8738;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270894;
  _v[0] = 1.0; 
  _v[1] = 1.82633;
  _v[2] = 32.9405;
  _v[3] = -2.47688;
  _v[4] = 23.5171;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270919;
  _v[0] = 1.0; 
  _v[1] = 2.57789;
  _v[2] = 32.7196;
  _v[3] = -2.78433;
  _v[4] = 24.103;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270920;
  _v[0] = 1.0; 
  _v[1] = 2.75953;
  _v[2] = 39.3013;
  _v[3] = -2.53384;
  _v[4] = 25.6969;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270923;
  _v[0] = 1.0; 
  _v[1] = 2.75953;
  _v[2] = 39.3013;
  _v[3] = -2.53384;
  _v[4] = 25.6969;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 270979;
  _v[0] = 1.0; 
  _v[1] = 2.58758;
  _v[2] = 37.4164;
  _v[3] = -2.59952;
  _v[4] = 24.1592;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271004;
  _v[0] = 1.0; 
  _v[1] = 3.04851;
  _v[2] = 31.4306;
  _v[3] = -2.97647;
  _v[4] = 24.0455;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271006;
  _v[0] = 1.0; 
  _v[1] = 2.64161;
  _v[2] = 33.6354;
  _v[3] = -0.735775;
  _v[4] = 28.1097;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271007;
  _v[0] = 1.0; 
  _v[1] = 3.10197;
  _v[2] = 40.2401;
  _v[3] = -3.20377;
  _v[4] = 26.0235;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271024;
  _v[0] = 1.0; 
  _v[1] = 2.71145;
  _v[2] = 30.6384;
  _v[3] = -2.89757;
  _v[4] = 23.7828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271027;
  _v[0] = 1.0; 
  _v[1] = 2.50351;
  _v[2] = 38.1241;
  _v[3] = -1.52751;
  _v[4] = 26.542;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271039;
  _v[0] = 1.0; 
  _v[1] = 2.6038;
  _v[2] = 31.9079;
  _v[3] = -2.5549;
  _v[4] = 25.007;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271040;
  _v[0] = 1.0; 
  _v[1] = 2.69351;
  _v[2] = 37.3987;
  _v[3] = -1.79729;
  _v[4] = 25.2871;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271041;
  _v[0] = 1.0; 
  _v[1] = 2.69351;
  _v[2] = 37.3987;
  _v[3] = -1.79729;
  _v[4] = 25.2871;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271043;
  _v[0] = 1.0; 
  _v[1] = 2.69351;
  _v[2] = 37.3987;
  _v[3] = -1.79729;
  _v[4] = 25.2871;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271045;
  _v[0] = 1.0; 
  _v[1] = 0.639819;
  _v[2] = 41.3;
  _v[3] = 2.54158;
  _v[4] = 28.6971;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271047;
  _v[0] = 1.0; 
  _v[1] = 0.639819;
  _v[2] = 41.3;
  _v[3] = 2.54158;
  _v[4] = 28.6971;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271072;
  _v[0] = 1.0; 
  _v[1] = 2.32712;
  _v[2] = 36.9156;
  _v[3] = -2.02506;
  _v[4] = 25.2047;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271084;
  _v[0] = 1.0; 
  _v[1] = 1.94161;
  _v[2] = 36.9621;
  _v[3] = -1.78174;
  _v[4] = 24.215;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271104;
  _v[0] = 1.0; 
  _v[1] = 2.36801;
  _v[2] = 39.472;
  _v[3] = -1.74665;
  _v[4] = 25.0206;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271109;
  _v[0] = 1.0; 
  _v[1] = 5.09067;
  _v[2] = 40.8804;
  _v[3] = -5.54497;
  _v[4] = 29.6209;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271122;
  _v[0] = 1.0; 
  _v[1] = 2.56869;
  _v[2] = 37.093;
  _v[3] = -2.53985;
  _v[4] = 25.2194;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271124;
  _v[0] = 1.0; 
  _v[1] = 2.56869;
  _v[2] = 37.093;
  _v[3] = -2.53985;
  _v[4] = 25.2194;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271138;
  _v[0] = 1.0; 
  _v[1] = 0.634497;
  _v[2] = 30.0472;
  _v[3] = 0.211764;
  _v[4] = 21.2887;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271139;
  _v[0] = 1.0; 
  _v[1] = 2.5475;
  _v[2] = 37.6321;
  _v[3] = -2.0897;
  _v[4] = 25.4835;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271151;
  _v[0] = 1.0; 
  _v[1] = 1.90811;
  _v[2] = 34.3886;
  _v[3] = -1.31771;
  _v[4] = 24.6794;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271215;
  _v[0] = 1.0; 
  _v[1] = 2.69387;
  _v[2] = 35.4469;
  _v[3] = -2.07462;
  _v[4] = 26.5808;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271216;
  _v[0] = 1.0; 
  _v[1] = 2.16266;
  _v[2] = 41.1296;
  _v[3] = -1.68043;
  _v[4] = 27.9401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271217;
  _v[0] = 1.0; 
  _v[1] = 2.16266;
  _v[2] = 41.1296;
  _v[3] = -1.68043;
  _v[4] = 27.9401;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271267;
  _v[0] = 1.0; 
  _v[1] = 2.09901;
  _v[2] = 36.2376;
  _v[3] = -1.66739;
  _v[4] = 24.8908;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271279;
  _v[0] = 1.0; 
  _v[1] = 2.62422;
  _v[2] = 35.2996;
  _v[3] = -2.77692;
  _v[4] = 24.5958;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271280;
  _v[0] = 1.0; 
  _v[1] = 3.1905;
  _v[2] = 42.9525;
  _v[3] = -3.37337;
  _v[4] = 26.3934;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271281;
  _v[0] = 1.0; 
  _v[1] = 3.1905;
  _v[2] = 42.9525;
  _v[3] = -3.37337;
  _v[4] = 26.3934;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271296;
  _v[0] = 1.0; 
  _v[1] = 2.09645;
  _v[2] = 36.2301;
  _v[3] = -1.96489;
  _v[4] = 24.8787;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271307;
  _v[0] = 1.0; 
  _v[1] = 2.78036;
  _v[2] = 29.9036;
  _v[3] = -3.87029;
  _v[4] = 22.3172;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271308;
  _v[0] = 1.0; 
  _v[1] = 2.37288;
  _v[2] = 35.8043;
  _v[3] = -1.49395;
  _v[4] = 26.9044;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271309;
  _v[0] = 1.0; 
  _v[1] = 2.37288;
  _v[2] = 35.8043;
  _v[3] = -1.49395;
  _v[4] = 26.9044;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271326;
  _v[0] = 1.0; 
  _v[1] = 2.00356;
  _v[2] = 35.1323;
  _v[3] = -1.91684;
  _v[4] = 24.4688;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271328;
  _v[0] = 1.0; 
  _v[1] = 2.00356;
  _v[2] = 35.1323;
  _v[3] = -1.91684;
  _v[4] = 24.4688;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271361;
  _v[0] = 1.0; 
  _v[1] = 2.25586;
  _v[2] = 31.0081;
  _v[3] = -2.78585;
  _v[4] = 23.7759;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271362;
  _v[0] = 1.0; 
  _v[1] = 2.65484;
  _v[2] = 36.6621;
  _v[3] = -1.82068;
  _v[4] = 27.49;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271363;
  _v[0] = 1.0; 
  _v[1] = 3.29886;
  _v[2] = 45.6412;
  _v[3] = -7.18568;
  _v[4] = 24.3757;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271405;
  _v[0] = 1.0; 
  _v[1] = 2.62955;
  _v[2] = 36.8665;
  _v[3] = -1.79753;
  _v[4] = 26.102;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271408;
  _v[0] = 1.0; 
  _v[1] = 3.6173;
  _v[2] = 41.7316;
  _v[3] = -2.85055;
  _v[4] = 28.4933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271410;
  _v[0] = 1.0; 
  _v[1] = 3.6173;
  _v[2] = 41.7316;
  _v[3] = -2.85055;
  _v[4] = 28.4933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271413;
  _v[0] = 1.0; 
  _v[1] = 3.6173;
  _v[2] = 41.7316;
  _v[3] = -2.85055;
  _v[4] = 28.4933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271414;
  _v[0] = 1.0; 
  _v[1] = 3.6173;
  _v[2] = 41.7316;
  _v[3] = -2.85055;
  _v[4] = 28.4933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271437;
  _v[0] = 1.0; 
  _v[1] = 2.99933;
  _v[2] = 32.1932;
  _v[3] = -2.44872;
  _v[4] = 25.6413;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271439;
  _v[0] = 1.0; 
  _v[1] = 2.99933;
  _v[2] = 32.1932;
  _v[3] = -2.44872;
  _v[4] = 25.6413;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271440;
  _v[0] = 1.0; 
  _v[1] = 3.54501;
  _v[2] = 40.1928;
  _v[3] = -3.81249;
  _v[4] = 27.7341;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271456;
  _v[0] = 1.0; 
  _v[1] = 2.62002;
  _v[2] = 37.2869;
  _v[3] = -2.69639;
  _v[4] = 25.1766;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271482;
  _v[0] = 1.0; 
  _v[1] = 2.52497;
  _v[2] = 36.0131;
  _v[3] = -2.36472;
  _v[4] = 24.7271;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271496;
  _v[0] = 1.0; 
  _v[1] = 2.30507;
  _v[2] = 35.7797;
  _v[3] = -2.09944;
  _v[4] = 24.9189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271514;
  _v[0] = 1.0; 
  _v[1] = 2.22107;
  _v[2] = 29.0736;
  _v[3] = -1.49749;
  _v[4] = 22.5291;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271564;
  _v[0] = 1.0; 
  _v[1] = 2.40037;
  _v[2] = 33.0241;
  _v[3] = -2.10028;
  _v[4] = 24.0936;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271565;
  _v[0] = 1.0; 
  _v[1] = 2.40037;
  _v[2] = 33.0241;
  _v[3] = -2.10028;
  _v[4] = 24.0936;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271566;
  _v[0] = 1.0; 
  _v[1] = 2.31659;
  _v[2] = 38.0953;
  _v[3] = -2.00573;
  _v[4] = 27.2258;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271567;
  _v[0] = 1.0; 
  _v[1] = 3.94836;
  _v[2] = 51.984;
  _v[3] = -2.56068;
  _v[4] = 19.6373;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271568;
  _v[0] = 1.0; 
  _v[1] = 3.94836;
  _v[2] = 51.984;
  _v[3] = -2.56068;
  _v[4] = 19.6373;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271586;
  _v[0] = 1.0; 
  _v[1] = 2.86038;
  _v[2] = 35.658;
  _v[3] = -2.45217;
  _v[4] = 24.9504;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271599;
  _v[0] = 1.0; 
  _v[1] = 2.54912;
  _v[2] = 36.117;
  _v[3] = -2.42102;
  _v[4] = 24.6761;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271614;
  _v[0] = 1.0; 
  _v[1] = 2.35328;
  _v[2] = 39.2236;
  _v[3] = -2.62674;
  _v[4] = 25.3081;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271615;
  _v[0] = 1.0; 
  _v[1] = 2.35328;
  _v[2] = 39.2236;
  _v[3] = -2.62674;
  _v[4] = 25.3081;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271638;
  _v[0] = 1.0; 
  _v[1] = 2.26449;
  _v[2] = 34.2226;
  _v[3] = -1.98628;
  _v[4] = 24.6128;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271640;
  _v[0] = 1.0; 
  _v[1] = 3.07655;
  _v[2] = 40.0194;
  _v[3] = -2.54751;
  _v[4] = 27.5964;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271642;
  _v[0] = 1.0; 
  _v[1] = 3.07655;
  _v[2] = 40.0194;
  _v[3] = -2.54751;
  _v[4] = 27.5964;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271644;
  _v[0] = 1.0; 
  _v[1] = 3.07655;
  _v[2] = 40.0194;
  _v[3] = -2.54751;
  _v[4] = 27.5964;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271645;
  _v[0] = 1.0; 
  _v[1] = 3.07655;
  _v[2] = 40.0194;
  _v[3] = -2.54751;
  _v[4] = 27.5964;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271655;
  _v[0] = 1.0; 
  _v[1] = 0.923664;
  _v[2] = 30.2411;
  _v[3] = -0.403726;
  _v[4] = 24.0275;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271657;
  _v[0] = 1.0; 
  _v[1] = 0.923664;
  _v[2] = 30.2411;
  _v[3] = -0.403726;
  _v[4] = 24.0275;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271658;
  _v[0] = 1.0; 
  _v[1] = 2.61172;
  _v[2] = 38.827;
  _v[3] = -2.19004;
  _v[4] = 26.3869;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271659;
  _v[0] = 1.0; 
  _v[1] = 2.61172;
  _v[2] = 38.827;
  _v[3] = -2.19004;
  _v[4] = 26.3869;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271660;
  _v[0] = 1.0; 
  _v[1] = 1.99421;
  _v[2] = 41.5468;
  _v[3] = -0.650808;
  _v[4] = 29.7855;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271663;
  _v[0] = 1.0; 
  _v[1] = 2.70591;
  _v[2] = 42.8805;
  _v[3] = -4.89908;
  _v[4] = 27.1255;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271677;
  _v[0] = 1.0; 
  _v[1] = 4.24866;
  _v[2] = 28.1423;
  _v[3] = -2.83659;
  _v[4] = 25.2649;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271678;
  _v[0] = 1.0; 
  _v[1] = 2.82767;
  _v[2] = 36.532;
  _v[3] = -1.95783;
  _v[4] = 26.5833;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271682;
  _v[0] = 1.0; 
  _v[1] = 2.88636;
  _v[2] = 39.6032;
  _v[3] = -3.5159;
  _v[4] = 28.832;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271700;
  _v[0] = 1.0; 
  _v[1] = 2.61531;
  _v[2] = 37.0913;
  _v[3] = -2.62277;
  _v[4] = 25.6662;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271722;
  _v[0] = 1.0; 
  _v[1] = 2.60229;
  _v[2] = 36.3812;
  _v[3] = -2.2889;
  _v[4] = 25.9263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271738;
  _v[0] = 1.0; 
  _v[1] = 2.27035;
  _v[2] = 37.6303;
  _v[3] = -1.94787;
  _v[4] = 25.3241;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271746;
  _v[0] = 1.0; 
  _v[1] = 2.63463;
  _v[2] = 34.6571;
  _v[3] = -2.38804;
  _v[4] = 25.9405;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271747;
  _v[0] = 1.0; 
  _v[1] = 2.1588;
  _v[2] = 41.288;
  _v[3] = -1.66441;
  _v[4] = 27.96;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271749;
  _v[0] = 1.0; 
  _v[1] = 2.1588;
  _v[2] = 41.288;
  _v[3] = -1.66441;
  _v[4] = 27.96;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271750;
  _v[0] = 1.0; 
  _v[1] = 2.1588;
  _v[2] = 41.288;
  _v[3] = -1.66441;
  _v[4] = 27.96;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271751;
  _v[0] = 1.0; 
  _v[1] = 2.1588;
  _v[2] = 41.288;
  _v[3] = -1.66441;
  _v[4] = 27.96;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271791;
  _v[0] = 1.0; 
  _v[1] = 2.60858;
  _v[2] = 30.6948;
  _v[3] = -2.83164;
  _v[4] = 22.9733;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271792;
  _v[0] = 1.0; 
  _v[1] = 2.5526;
  _v[2] = 36.096;
  _v[3] = -2.9538;
  _v[4] = 25.8468;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271793;
  _v[0] = 1.0; 
  _v[1] = 2.5526;
  _v[2] = 36.096;
  _v[3] = -2.9538;
  _v[4] = 25.8468;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271794;
  _v[0] = 1.0; 
  _v[1] = 1.48697;
  _v[2] = 40.7141;
  _v[3] = -2.00824;
  _v[4] = 25.8795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271795;
  _v[0] = 1.0; 
  _v[1] = 1.48697;
  _v[2] = 40.7141;
  _v[3] = -2.00824;
  _v[4] = 25.8795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271796;
  _v[0] = 1.0; 
  _v[1] = 1.48697;
  _v[2] = 40.7141;
  _v[3] = -2.00824;
  _v[4] = 25.8795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271797;
  _v[0] = 1.0; 
  _v[1] = 1.48697;
  _v[2] = 40.7141;
  _v[3] = -2.00824;
  _v[4] = 25.8795;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271814;
  _v[0] = 1.0; 
  _v[1] = 2.03449;
  _v[2] = 32.0235;
  _v[3] = -2.25171;
  _v[4] = 23.9133;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271816;
  _v[0] = 1.0; 
  _v[1] = 2.48366;
  _v[2] = 37.6798;
  _v[3] = -2.00046;
  _v[4] = 27.1096;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271817;
  _v[0] = 1.0; 
  _v[1] = 1.68812;
  _v[2] = 39.2784;
  _v[3] = -1.61046;
  _v[4] = 29.0883;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271828;
  _v[0] = 1.0; 
  _v[1] = 2.3625;
  _v[2] = 36.1728;
  _v[3] = -2.14544;
  _v[4] = 25.4195;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271841;
  _v[0] = 1.0; 
  _v[1] = 2.24585;
  _v[2] = 36.1073;
  _v[3] = -2.01765;
  _v[4] = 24.9739;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271856;
  _v[0] = 1.0; 
  _v[1] = 1.16933;
  _v[2] = 28.7626;
  _v[3] = -0.271353;
  _v[4] = 24.7368;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271858;
  _v[0] = 1.0; 
  _v[1] = 2.29603;
  _v[2] = 38.941;
  _v[3] = -1.78041;
  _v[4] = 25.3999;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271860;
  _v[0] = 1.0; 
  _v[1] = 2.90947;
  _v[2] = 40.5003;
  _v[3] = -5.02035;
  _v[4] = 28.9217;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271861;
  _v[0] = 1.0; 
  _v[1] = 2.90947;
  _v[2] = 40.5003;
  _v[3] = -5.02035;
  _v[4] = 28.9217;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271889;
  _v[0] = 1.0; 
  _v[1] = 2.179;
  _v[2] = 37.092;
  _v[3] = -1.69436;
  _v[4] = 25.0979;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271892;
  _v[0] = 1.0; 
  _v[1] = 2.179;
  _v[2] = 37.092;
  _v[3] = -1.69436;
  _v[4] = 25.0979;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271893;
  _v[0] = 1.0; 
  _v[1] = 4.31776;
  _v[2] = 40.7165;
  _v[3] = -4.70768;
  _v[4] = 28.9219;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271894;
  _v[0] = 1.0; 
  _v[1] = 4.31776;
  _v[2] = 40.7165;
  _v[3] = -4.70768;
  _v[4] = 28.9219;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271895;
  _v[0] = 1.0; 
  _v[1] = 4.31776;
  _v[2] = 40.7165;
  _v[3] = -4.70768;
  _v[4] = 28.9219;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271896;
  _v[0] = 1.0; 
  _v[1] = 4.31776;
  _v[2] = 40.7165;
  _v[3] = -4.70768;
  _v[4] = 28.9219;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271919;
  _v[0] = 1.0; 
  _v[1] = 2.52333;
  _v[2] = 37.2243;
  _v[3] = -1.7506;
  _v[4] = 24.6672;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 271933;
  _v[0] = 1.0; 
  _v[1] = 2.0681;
  _v[2] = 29.9041;
  _v[3] = -2.13528;
  _v[4] = 21.8208;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272030;
  _v[0] = 1.0; 
  _v[1] = 1.53613;
  _v[2] = 31.6226;
  _v[3] = -1.39656;
  _v[4] = 24.3556;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272031;
  _v[0] = 1.0; 
  _v[1] = 2.12358;
  _v[2] = 36.6089;
  _v[3] = -0.733735;
  _v[4] = 27.0917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272099;
  _v[0] = 1.0; 
  _v[1] = 2.54257;
  _v[2] = 37.7531;
  _v[3] = -2.18676;
  _v[4] = 25.2292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272100;
  _v[0] = 1.0; 
  _v[1] = 2.54257;
  _v[2] = 37.7531;
  _v[3] = -2.18676;
  _v[4] = 25.2292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272104;
  _v[0] = 1.0; 
  _v[1] = -4.68806;
  _v[2] = 592.742;
  _v[3] = 2515.88;
  _v[4] = 21.0352;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272126;
  _v[0] = 1.0; 
  _v[1] = 2.7912;
  _v[2] = 36.2751;
  _v[3] = -2.11647;
  _v[4] = 26.1183;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272140;
  _v[0] = 1.0; 
  _v[1] = 2.28473;
  _v[2] = 30.7883;
  _v[3] = -2.18467;
  _v[4] = 23.8158;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272141;
  _v[0] = 1.0; 
  _v[1] = 2.57469;
  _v[2] = 34.9071;
  _v[3] = -2.45901;
  _v[4] = 24.8235;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272143;
  _v[0] = 1.0; 
  _v[1] = 2.57469;
  _v[2] = 34.9071;
  _v[3] = -2.45901;
  _v[4] = 24.8235;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272144;
  _v[0] = 1.0; 
  _v[1] = 2.57469;
  _v[2] = 34.9071;
  _v[3] = -2.45901;
  _v[4] = 24.8235;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272160;
  _v[0] = 1.0; 
  _v[1] = 2.69636;
  _v[2] = 31.5764;
  _v[3] = -2.61699;
  _v[4] = 23.1866;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272161;
  _v[0] = 1.0; 
  _v[1] = 3.76103;
  _v[2] = 37.4309;
  _v[3] = -3.91038;
  _v[4] = 25.8352;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272187;
  _v[0] = 1.0; 
  _v[1] = 2.7534;
  _v[2] = 37.2008;
  _v[3] = -2.69744;
  _v[4] = 25.1198;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272188;
  _v[0] = 1.0; 
  _v[1] = 2.7534;
  _v[2] = 37.2008;
  _v[3] = -2.69744;
  _v[4] = 25.1198;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272214;
  _v[0] = 1.0; 
  _v[1] = 2.7534;
  _v[2] = 37.2008;
  _v[3] = -2.69744;
  _v[4] = 25.1198;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272470;
  _v[0] = 1.0; 
  _v[1] = 2.44102;
  _v[2] = 31.1745;
  _v[3] = -2.2985;
  _v[4] = 23.9034;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272471;
  _v[0] = 1.0; 
  _v[1] = 3.24753;
  _v[2] = 34.214;
  _v[3] = -2.12342;
  _v[4] = 25.9285;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272472;
  _v[0] = 1.0; 
  _v[1] = 3.72715;
  _v[2] = 37.4596;
  _v[3] = -2.24273;
  _v[4] = 27.5112;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272487;
  _v[0] = 1.0; 
  _v[1] = 2.92969;
  _v[2] = 33.2386;
  _v[3] = -2.5928;
  _v[4] = 24.2502;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272497;
  _v[0] = 1.0; 
  _v[1] = 3.62361;
  _v[2] = 37.2618;
  _v[3] = -2.26159;
  _v[4] = 29.2243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272498;
  _v[0] = 1.0; 
  _v[1] = 3.62361;
  _v[2] = 37.2618;
  _v[3] = -2.26159;
  _v[4] = 29.2243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272499;
  _v[0] = 1.0; 
  _v[1] = 3.62361;
  _v[2] = 37.2618;
  _v[3] = -2.26159;
  _v[4] = 29.2243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272516;
  _v[0] = 1.0; 
  _v[1] = 3.41271;
  _v[2] = 35.3601;
  _v[3] = -2.6708;
  _v[4] = 24.4523;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272518;
  _v[0] = 1.0; 
  _v[1] = 2.33153;
  _v[2] = 38.8825;
  _v[3] = -0.985339;
  _v[4] = 28.1041;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272540;
  _v[0] = 1.0; 
  _v[1] = 3.16207;
  _v[2] = 36.573;
  _v[3] = -2.88512;
  _v[4] = 24.4536;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272556;
  _v[0] = 1.0; 
  _v[1] = 2.62273;
  _v[2] = 29.495;
  _v[3] = -1.56803;
  _v[4] = 23.3558;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272569;
  _v[0] = 1.0; 
  _v[1] = 0.42675;
  _v[2] = 37.5039;
  _v[3] = -3.46817;
  _v[4] = 25.7243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272586;
  _v[0] = 1.0; 
  _v[1] = 0.0708647;
  _v[2] = 35.0613;
  _v[3] = -2.99936;
  _v[4] = 24.7813;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272589;
  _v[0] = 1.0; 
  _v[1] = -0.507559;
  _v[2] = 42.4211;
  _v[3] = -1.92892;
  _v[4] = 25.8252;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272590;
  _v[0] = 1.0; 
  _v[1] = -0.507559;
  _v[2] = 42.4211;
  _v[3] = -1.92892;
  _v[4] = 25.8252;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272591;
  _v[0] = 1.0; 
  _v[1] = -0.507559;
  _v[2] = 42.4211;
  _v[3] = -1.92892;
  _v[4] = 25.8252;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272594;
  _v[0] = 1.0; 
  _v[1] = -0.81204;
  _v[2] = -315.704;
  _v[3] = -57.4694;
  _v[4] = -17.776;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272637;
  _v[0] = 1.0; 
  _v[1] = 0.213574;
  _v[2] = 37.8643;
  _v[3] = -2.75077;
  _v[4] = 25.2811;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272642;
  _v[0] = 1.0; 
  _v[1] = 0.213574;
  _v[2] = 37.8643;
  _v[3] = -2.75077;
  _v[4] = 25.2811;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272680;
  _v[0] = 1.0; 
  _v[1] = -0.434977;
  _v[2] = 36.463;
  _v[3] = -2.21825;
  _v[4] = 24.7689;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272702;
  _v[0] = 1.0; 
  _v[1] = -0.150333;
  _v[2] = 36.3907;
  _v[3] = -2.60576;
  _v[4] = 25.7032;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272716;
  _v[0] = 1.0; 
  _v[1] = -0.362974;
  _v[2] = 34.7023;
  _v[3] = -2.53389;
  _v[4] = 24.1796;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272826;
  _v[0] = 1.0; 
  _v[1] = -0.657199;
  _v[2] = 34.209;
  _v[3] = -1.28586;
  _v[4] = 25.1141;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272891;
  _v[0] = 1.0; 
  _v[1] = -0.1953;
  _v[2] = 34.8227;
  _v[3] = -2.39232;
  _v[4] = 24.0667;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272908;
  _v[0] = 1.0; 
  _v[1] = -0.391726;
  _v[2] = 36.4061;
  _v[3] = -2.42807;
  _v[4] = 24.475;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 272958;
  _v[0] = 1.0; 
  _v[1] = -0.287691;
  _v[2] = 34.7831;
  _v[3] = -2.6815;
  _v[4] = 24.9244;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273168;
  _v[0] = 1.0; 
  _v[1] = 0.114887;
  _v[2] = 33.5905;
  _v[3] = -2.04921;
  _v[4] = 25.5765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273171;
  _v[0] = 1.0; 
  _v[1] = 1.61236;
  _v[2] = -17224.2;
  _v[3] = -11875.8;
  _v[4] = 21.1518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273199;
  _v[0] = 1.0; 
  _v[1] = 0.329345;
  _v[2] = 30.6954;
  _v[3] = -3.77134;
  _v[4] = 23.1045;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273201;
  _v[0] = 1.0; 
  _v[1] = 0.0345167;
  _v[2] = 37.1184;
  _v[3] = -2.82066;
  _v[4] = 25.0213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273202;
  _v[0] = 1.0; 
  _v[1] = 0.0345167;
  _v[2] = 37.1184;
  _v[3] = -2.82066;
  _v[4] = 25.0213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273203;
  _v[0] = 1.0; 
  _v[1] = 0.0345167;
  _v[2] = 37.1184;
  _v[3] = -2.82066;
  _v[4] = 25.0213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273204;
  _v[0] = 1.0; 
  _v[1] = 0.0345167;
  _v[2] = 37.1184;
  _v[3] = -2.82066;
  _v[4] = 25.0213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273231;
  _v[0] = 1.0; 
  _v[1] = -0.336889;
  _v[2] = 34.3278;
  _v[3] = -1.98288;
  _v[4] = 24.289;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273232;
  _v[0] = 1.0; 
  _v[1] = -0.182187;
  _v[2] = 42.7735;
  _v[3] = -1.71552;
  _v[4] = 25.5777;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273249;
  _v[0] = 1.0; 
  _v[1] = -1.53397;
  _v[2] = 30.635;
  _v[3] = -0.397832;
  _v[4] = 22.2988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273251;
  _v[0] = 1.0; 
  _v[1] = -1.53397;
  _v[2] = 30.635;
  _v[3] = -0.397832;
  _v[4] = 22.2988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273252;
  _v[0] = 1.0; 
  _v[1] = -1.53397;
  _v[2] = 30.635;
  _v[3] = -0.397832;
  _v[4] = 22.2988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273253;
  _v[0] = 1.0; 
  _v[1] = -1.53397;
  _v[2] = 30.635;
  _v[3] = -0.397832;
  _v[4] = 22.2988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273254;
  _v[0] = 1.0; 
  _v[1] = 0.154021;
  _v[2] = 34.4417;
  _v[3] = -2.87017;
  _v[4] = 25.0593;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273255;
  _v[0] = 1.0; 
  _v[1] = -0.145672;
  _v[2] = 38.1549;
  _v[3] = -1.53852;
  _v[4] = 26.6042;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273278;
  _v[0] = 1.0; 
  _v[1] = -0.0966452;
  _v[2] = 34.3867;
  _v[3] = -2.60987;
  _v[4] = 24.0653;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273285;
  _v[0] = 1.0; 
  _v[1] = 0.580575;
  _v[2] = 36.4983;
  _v[3] = -2.86211;
  _v[4] = 29.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273286;
  _v[0] = 1.0; 
  _v[1] = 0.580575;
  _v[2] = 36.4983;
  _v[3] = -2.86211;
  _v[4] = 29.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273287;
  _v[0] = 1.0; 
  _v[1] = 0.580575;
  _v[2] = 36.4983;
  _v[3] = -2.86211;
  _v[4] = 29.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273288;
  _v[0] = 1.0; 
  _v[1] = 0.580575;
  _v[2] = 36.4983;
  _v[3] = -2.86211;
  _v[4] = 29.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273289;
  _v[0] = 1.0; 
  _v[1] = -0.345701;
  _v[2] = 42.3196;
  _v[3] = -0.859297;
  _v[4] = 25.1889;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273325;
  _v[0] = 1.0; 
  _v[1] = 0.445944;
  _v[2] = 36.3453;
  _v[3] = -3.12282;
  _v[4] = 25.6578;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273326;
  _v[0] = 1.0; 
  _v[1] = 0.445944;
  _v[2] = 36.3453;
  _v[3] = -3.12282;
  _v[4] = 25.6578;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273347;
  _v[0] = 1.0; 
  _v[1] = -0.122182;
  _v[2] = 37.5593;
  _v[3] = -2.81692;
  _v[4] = 24.3858;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273407;
  _v[0] = 1.0; 
  _v[1] = -0.065335;
  _v[2] = 36.2139;
  _v[3] = -1.98182;
  _v[4] = 24.5986;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273551;
  _v[0] = 1.0; 
  _v[1] = 0.728844;
  _v[2] = 31.314;
  _v[3] = -3.29434;
  _v[4] = 24.2958;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273555;
  _v[0] = 1.0; 
  _v[1] = 0.728844;
  _v[2] = 31.314;
  _v[3] = -3.29434;
  _v[4] = 24.2958;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273556;
  _v[0] = 1.0; 
  _v[1] = 1.05807;
  _v[2] = 37.12;
  _v[3] = -2.64667;
  _v[4] = 26.5243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273558;
  _v[0] = 1.0; 
  _v[1] = 0.0693604;
  _v[2] = 39.457;
  _v[3] = -1.63271;
  _v[4] = 27.3188;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273561;
  _v[0] = 1.0; 
  _v[1] = 0.0693604;
  _v[2] = 39.457;
  _v[3] = -1.63271;
  _v[4] = 27.3188;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273590;
  _v[0] = 1.0; 
  _v[1] = 0.703265;
  _v[2] = 35.5823;
  _v[3] = -3.30868;
  _v[4] = 25.2988;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273608;
  _v[0] = 1.0; 
  _v[1] = -0.45608;
  _v[2] = 34.8189;
  _v[3] = -1.55623;
  _v[4] = 24.1085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273694;
  _v[0] = 1.0; 
  _v[1] = -1.51353;
  _v[2] = 26.679;
  _v[3] = -0.959348;
  _v[4] = 24.463;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273695;
  _v[0] = 1.0; 
  _v[1] = 0.0780793;
  _v[2] = 35.0715;
  _v[3] = -2.94637;
  _v[4] = 24.5155;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273717;
  _v[0] = 1.0; 
  _v[1] = -0.160164;
  _v[2] = 36.2608;
  _v[3] = -2.43205;
  _v[4] = 24.6363;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273734;
  _v[0] = 1.0; 
  _v[1] = -1.18209;
  _v[2] = 32.2452;
  _v[3] = -1.42064;
  _v[4] = 23.1728;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273746;
  _v[0] = 1.0; 
  _v[1] = 0.341327;
  _v[2] = 37.7032;
  _v[3] = -2.92255;
  _v[4] = 26.963;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273747;
  _v[0] = 1.0; 
  _v[1] = 0.341327;
  _v[2] = 37.7032;
  _v[3] = -2.92255;
  _v[4] = 26.963;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273764;
  _v[0] = 1.0; 
  _v[1] = -0.547782;
  _v[2] = 33.1472;
  _v[3] = -1.86645;
  _v[4] = 23.893;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273854;
  _v[0] = 1.0; 
  _v[1] = 0.147832;
  _v[2] = 33.7859;
  _v[3] = -2.52782;
  _v[4] = 25.6225;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273878;
  _v[0] = 1.0; 
  _v[1] = -0.788374;
  _v[2] = 31.1245;
  _v[3] = -1.73525;
  _v[4] = 24.0987;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273879;
  _v[0] = 1.0; 
  _v[1] = 0.380957;
  _v[2] = 38.3296;
  _v[3] = -3.45544;
  _v[4] = 26.0521;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273881;
  _v[0] = 1.0; 
  _v[1] = 0.380957;
  _v[2] = 38.3296;
  _v[3] = -3.45544;
  _v[4] = 26.0521;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273894;
  _v[0] = 1.0; 
  _v[1] = 0.1304;
  _v[2] = 36.6365;
  _v[3] = -3.0873;
  _v[4] = 24.9181;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273909;
  _v[0] = 1.0; 
  _v[1] = -0.0314079;
  _v[2] = 34.7443;
  _v[3] = -2.63898;
  _v[4] = 24.6486;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273931;
  _v[0] = 1.0; 
  _v[1] = 0.160383;
  _v[2] = 36.8819;
  _v[3] = -3.09979;
  _v[4] = 24.1141;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273941;
  _v[0] = 1.0; 
  _v[1] = -0.0964543;
  _v[2] = 34.5326;
  _v[3] = -2.54118;
  _v[4] = 24.8522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273943;
  _v[0] = 1.0; 
  _v[1] = 0.0634308;
  _v[2] = 42.7331;
  _v[3] = -3.03422;
  _v[4] = 26.3557;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 273995;
  _v[0] = 1.0; 
  _v[1] = 0.0257446;
  _v[2] = 36.5229;
  _v[3] = -2.78062;
  _v[4] = 25.1905;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274012;
  _v[0] = 1.0; 
  _v[1] = -0.32009;
  _v[2] = 33.5312;
  _v[3] = -2.75514;
  _v[4] = 24.3515;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274013;
  _v[0] = 1.0; 
  _v[1] = 0.515889;
  _v[2] = 42.9786;
  _v[3] = -2.77806;
  _v[4] = 27.5257;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274014;
  _v[0] = 1.0; 
  _v[1] = 0.515889;
  _v[2] = 42.9786;
  _v[3] = -2.77806;
  _v[4] = 27.5257;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274015;
  _v[0] = 1.0; 
  _v[1] = 0.515889;
  _v[2] = 42.9786;
  _v[3] = -2.77806;
  _v[4] = 27.5257;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274018;
  _v[0] = 1.0; 
  _v[1] = 0.972355;
  _v[2] = 44.1763;
  _v[3] = -2.6994;
  _v[4] = 27.7512;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274036;
  _v[0] = 1.0; 
  _v[1] = 0.00457147;
  _v[2] = 36.3132;
  _v[3] = -2.64154;
  _v[4] = 24.3053;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274048;
  _v[0] = 1.0; 
  _v[1] = 0.0514275;
  _v[2] = 28.3925;
  _v[3] = -3.0587;
  _v[4] = 22.1499;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274049;
  _v[0] = 1.0; 
  _v[1] = 0.0514275;
  _v[2] = 28.3925;
  _v[3] = -3.0587;
  _v[4] = 22.1499;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274051;
  _v[0] = 1.0; 
  _v[1] = 0.570638;
  _v[2] = 32.83;
  _v[3] = -4.11321;
  _v[4] = 24.3415;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274055;
  _v[0] = 1.0; 
  _v[1] = 0.570638;
  _v[2] = 32.83;
  _v[3] = -4.11321;
  _v[4] = 24.3415;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274123;
  _v[0] = 1.0; 
  _v[1] = 0.00782447;
  _v[2] = 34.3233;
  _v[3] = -2.75272;
  _v[4] = 24.5134;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274124;
  _v[0] = 1.0; 
  _v[1] = -0.889043;
  _v[2] = 45.063;
  _v[3] = 0.0479156;
  _v[4] = 23.5678;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274139;
  _v[0] = 1.0; 
  _v[1] = -0.0862799;
  _v[2] = 29.9857;
  _v[3] = -3.39695;
  _v[4] = 24.1995;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274140;
  _v[0] = 1.0; 
  _v[1] = -0.0862799;
  _v[2] = 29.9857;
  _v[3] = -3.39695;
  _v[4] = 24.1995;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274141;
  _v[0] = 1.0; 
  _v[1] = -0.0862799;
  _v[2] = 29.9857;
  _v[3] = -3.39695;
  _v[4] = 24.1995;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274142;
  _v[0] = 1.0; 
  _v[1] = -0.0862799;
  _v[2] = 29.9857;
  _v[3] = -3.39695;
  _v[4] = 24.1995;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274143;
  _v[0] = 1.0; 
  _v[1] = 0.0141682;
  _v[2] = 34.0453;
  _v[3] = -2.24172;
  _v[4] = 26.1176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274144;
  _v[0] = 1.0; 
  _v[1] = 0.0141682;
  _v[2] = 34.0453;
  _v[3] = -2.24172;
  _v[4] = 26.1176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274145;
  _v[0] = 1.0; 
  _v[1] = 0.0141682;
  _v[2] = 34.0453;
  _v[3] = -2.24172;
  _v[4] = 26.1176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274146;
  _v[0] = 1.0; 
  _v[1] = 0.329833;
  _v[2] = 38.7871;
  _v[3] = -1.34407;
  _v[4] = 27.2128;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274150;
  _v[0] = 1.0; 
  _v[1] = 0.329833;
  _v[2] = 38.7871;
  _v[3] = -1.34407;
  _v[4] = 27.2128;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274196;
  _v[0] = 1.0; 
  _v[1] = -0.266252;
  _v[2] = 30.8891;
  _v[3] = -2.80033;
  _v[4] = 22.6045;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274216;
  _v[0] = 1.0; 
  _v[1] = 3.54546;
  _v[2] = 650.657;
  _v[3] = -2136.06;
  _v[4] = 15.9753;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274250;
  _v[0] = 1.0; 
  _v[1] = 0.628887;
  _v[2] = 36.0169;
  _v[3] = -2.99487;
  _v[4] = 25.3312;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274254;
  _v[0] = 1.0; 
  _v[1] = 0.994433;
  _v[2] = 37.7799;
  _v[3] = -1.13826;
  _v[4] = 31.6338;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274267;
  _v[0] = 1.0; 
  _v[1] = -0.33635;
  _v[2] = 30.0992;
  _v[3] = -1.978;
  _v[4] = 23.1388;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274269;
  _v[0] = 1.0; 
  _v[1] = 0.735374;
  _v[2] = 37.4865;
  _v[3] = -4.22396;
  _v[4] = 26.7002;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274272;
  _v[0] = 1.0; 
  _v[1] = 0.735374;
  _v[2] = 37.4865;
  _v[3] = -4.22396;
  _v[4] = 26.7002;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274282;
  _v[0] = 1.0; 
  _v[1] = 0.247258;
  _v[2] = 34.7551;
  _v[3] = -2.94289;
  _v[4] = 24.0254;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274311;
  _v[0] = 1.0; 
  _v[1] = -0.386188;
  _v[2] = 34.387;
  _v[3] = -2.35619;
  _v[4] = 24.341;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274323;
  _v[0] = 1.0; 
  _v[1] = 0.490463;
  _v[2] = 40.4592;
  _v[3] = -2.67016;
  _v[4] = 25.9573;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274339;
  _v[0] = 1.0; 
  _v[1] = 0.247212;
  _v[2] = 29.7953;
  _v[3] = -3.51806;
  _v[4] = 21.1669;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274340;
  _v[0] = 1.0; 
  _v[1] = 1.21329;
  _v[2] = 31.7356;
  _v[3] = -4.06267;
  _v[4] = 23.972;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274341;
  _v[0] = 1.0; 
  _v[1] = 0.0296641;
  _v[2] = 37.5573;
  _v[3] = -2.88086;
  _v[4] = 24.381;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274342;
  _v[0] = 1.0; 
  _v[1] = 0.0296641;
  _v[2] = 37.5573;
  _v[3] = -2.88086;
  _v[4] = 24.381;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274343;
  _v[0] = 1.0; 
  _v[1] = 0.887961;
  _v[2] = 39.3563;
  _v[3] = -2.99765;
  _v[4] = 27.1522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274404;
  _v[0] = 1.0; 
  _v[1] = -0.0987646;
  _v[2] = 33.7149;
  _v[3] = -2.37919;
  _v[4] = 24.176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274405;
  _v[0] = 1.0; 
  _v[1] = -0.0987646;
  _v[2] = 33.7149;
  _v[3] = -2.37919;
  _v[4] = 24.176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274435;
  _v[0] = 1.0; 
  _v[1] = 0.000577712;
  _v[2] = 38.2813;
  _v[3] = -2.17964;
  _v[4] = 26.0539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274436;
  _v[0] = 1.0; 
  _v[1] = 0.000577712;
  _v[2] = 38.2813;
  _v[3] = -2.17964;
  _v[4] = 26.0539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274437;
  _v[0] = 1.0; 
  _v[1] = 0.000577712;
  _v[2] = 38.2813;
  _v[3] = -2.17964;
  _v[4] = 26.0539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274438;
  _v[0] = 1.0; 
  _v[1] = 0.000577712;
  _v[2] = 38.2813;
  _v[3] = -2.17964;
  _v[4] = 26.0539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274439;
  _v[0] = 1.0; 
  _v[1] = 0.000577712;
  _v[2] = 38.2813;
  _v[3] = -2.17964;
  _v[4] = 26.0539;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274454;
  _v[0] = 1.0; 
  _v[1] = 0.501838;
  _v[2] = 35.9354;
  _v[3] = -3.26345;
  _v[4] = 24.8342;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274468;
  _v[0] = 1.0; 
  _v[1] = 0.448407;
  _v[2] = 37.21;
  _v[3] = -2.79148;
  _v[4] = 24.2926;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274499;
  _v[0] = 1.0; 
  _v[1] = 0.641121;
  _v[2] = 30.5265;
  _v[3] = -3.065;
  _v[4] = 23.9266;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274500;
  _v[0] = 1.0; 
  _v[1] = 0.641121;
  _v[2] = 30.5265;
  _v[3] = -3.065;
  _v[4] = 23.9266;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274506;
  _v[0] = 1.0; 
  _v[1] = -0.412154;
  _v[2] = 34.8697;
  _v[3] = -1.29795;
  _v[4] = 26.7801;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274517;
  _v[0] = 1.0; 
  _v[1] = -0.259344;
  _v[2] = 36.3203;
  _v[3] = -2.4282;
  _v[4] = 24.3834;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274555;
  _v[0] = 1.0; 
  _v[1] = -0.205237;
  _v[2] = 34.2902;
  _v[3] = -2.31202;
  _v[4] = 24.0987;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274569;
  _v[0] = 1.0; 
  _v[1] = 0.0903187;
  _v[2] = 34.957;
  _v[3] = -2.64201;
  _v[4] = 24.6108;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274589;
  _v[0] = 1.0; 
  _v[1] = 0.565469;
  _v[2] = 35.9538;
  _v[3] = -3.28759;
  _v[4] = 23.9739;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274599;
  _v[0] = 1.0; 
  _v[1] = -0.662183;
  _v[2] = 34.8068;
  _v[3] = -1.69608;
  _v[4] = 24.7641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274958;
  _v[0] = 1.0; 
  _v[1] = 1.30754;
  _v[2] = 29.6331;
  _v[3] = -3.71492;
  _v[4] = 24.3366;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274959;
  _v[0] = 1.0; 
  _v[1] = 0.0577819;
  _v[2] = 35.1065;
  _v[3] = -2.10591;
  _v[4] = 25.5498;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 274960;
  _v[0] = 1.0; 
  _v[1] = 0.0577819;
  _v[2] = 35.1065;
  _v[3] = -2.10591;
  _v[4] = 25.5498;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275000;
  _v[0] = 1.0; 
  _v[1] = 0.425993;
  _v[2] = 29.2327;
  _v[3] = -3.21919;
  _v[4] = 21.1879;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275002;
  _v[0] = 1.0; 
  _v[1] = 0.0865403;
  _v[2] = 34.0708;
  _v[3] = -2.72615;
  _v[4] = 25.2416;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275003;
  _v[0] = 1.0; 
  _v[1] = 0.0865403;
  _v[2] = 34.0708;
  _v[3] = -2.72615;
  _v[4] = 25.2416;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275011;
  _v[0] = 1.0; 
  _v[1] = 1.40093;
  _v[2] = 40.4475;
  _v[3] = -4.14372;
  _v[4] = 27.1109;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275021;
  _v[0] = 1.0; 
  _v[1] = 0.0055386;
  _v[2] = 33.2294;
  _v[3] = -2.34958;
  _v[4] = 23.3454;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275048;
  _v[0] = 1.0; 
  _v[1] = 0.237858;
  _v[2] = 36.6591;
  _v[3] = -2.10021;
  _v[4] = 25.5106;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275053;
  _v[0] = 1.0; 
  _v[1] = 1.30645;
  _v[2] = 43.7318;
  _v[3] = -2.21885;
  _v[4] = 26.662;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275062;
  _v[0] = 1.0; 
  _v[1] = 1.03666;
  _v[2] = 34.8621;
  _v[3] = -3.01113;
  _v[4] = 25.8923;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275092;
  _v[0] = 1.0; 
  _v[1] = -1.0028;
  _v[2] = 27.1895;
  _v[3] = -2.22106;
  _v[4] = 23.332;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275094;
  _v[0] = 1.0; 
  _v[1] = 0.0663998;
  _v[2] = 35.4237;
  _v[3] = -2.17529;
  _v[4] = 25.66;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275097;
  _v[0] = 1.0; 
  _v[1] = 0.0663998;
  _v[2] = 35.4237;
  _v[3] = -2.17529;
  _v[4] = 25.66;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275098;
  _v[0] = 1.0; 
  _v[1] = -0.147743;
  _v[2] = 41.7078;
  _v[3] = -1.42434;
  _v[4] = 25.9793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275116;
  _v[0] = 1.0; 
  _v[1] = 0.381925;
  _v[2] = 35.6116;
  _v[3] = -2.64671;
  _v[4] = 25.2184;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275117;
  _v[0] = 1.0; 
  _v[1] = 0.381925;
  _v[2] = 35.6116;
  _v[3] = -2.64671;
  _v[4] = 25.2184;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275120;
  _v[0] = 1.0; 
  _v[1] = 0.381925;
  _v[2] = 35.6116;
  _v[3] = -2.64671;
  _v[4] = 25.2184;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275164;
  _v[0] = 1.0; 
  _v[1] = -0.00476504;
  _v[2] = 35.9637;
  _v[3] = -2.30979;
  _v[4] = 25.0158;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275181;
  _v[0] = 1.0; 
  _v[1] = 0.911323;
  _v[2] = 29.6131;
  _v[3] = -3.24674;
  _v[4] = 22.8262;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275188;
  _v[0] = 1.0; 
  _v[1] = -0.0944481;
  _v[2] = 33.0511;
  _v[3] = -2.94655;
  _v[4] = 24.8225;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275189;
  _v[0] = 1.0; 
  _v[1] = -0.0944481;
  _v[2] = 33.0511;
  _v[3] = -2.94655;
  _v[4] = 24.8225;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275191;
  _v[0] = 1.0; 
  _v[1] = -0.40436;
  _v[2] = 38.1077;
  _v[3] = -1.39923;
  _v[4] = 26.8342;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275199;
  _v[0] = 1.0; 
  _v[1] = -0.40436;
  _v[2] = 38.1077;
  _v[3] = -1.39923;
  _v[4] = 26.8342;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275211;
  _v[0] = 1.0; 
  _v[1] = -0.373158;
  _v[2] = 29.8758;
  _v[3] = -1.74051;
  _v[4] = 23.0315;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275212;
  _v[0] = 1.0; 
  _v[1] = -0.373158;
  _v[2] = 29.8758;
  _v[3] = -1.74051;
  _v[4] = 23.0315;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275213;
  _v[0] = 1.0; 
  _v[1] = 0.0789361;
  _v[2] = 37.7634;
  _v[3] = -2.37489;
  _v[4] = 25.8031;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275227;
  _v[0] = 1.0; 
  _v[1] = 0.0535236;
  _v[2] = 35.0698;
  _v[3] = -2.68971;
  _v[4] = 24.0518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275254;
  _v[0] = 1.0; 
  _v[1] = 0.0903162;
  _v[2] = 35.0535;
  _v[3] = -2.51352;
  _v[4] = 24.3343;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275255;
  _v[0] = 1.0; 
  _v[1] = 0.0903162;
  _v[2] = 35.0535;
  _v[3] = -2.51352;
  _v[4] = 24.3343;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275256;
  _v[0] = 1.0; 
  _v[1] = 0.0903162;
  _v[2] = 35.0535;
  _v[3] = -2.51352;
  _v[4] = 24.3343;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275257;
  _v[0] = 1.0; 
  _v[1] = 1.38096;
  _v[2] = 40.5395;
  _v[3] = -2.27515;
  _v[4] = 27.6267;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275267;
  _v[0] = 1.0; 
  _v[1] = -0.0223897;
  _v[2] = 35.2496;
  _v[3] = -2.67924;
  _v[4] = 25.0866;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275290;
  _v[0] = 1.0; 
  _v[1] = 0.280459;
  _v[2] = 29.6804;
  _v[3] = -3.39419;
  _v[4] = 22.1249;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275292;
  _v[0] = 1.0; 
  _v[1] = 0.280459;
  _v[2] = 29.6804;
  _v[3] = -3.39419;
  _v[4] = 22.1249;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275295;
  _v[0] = 1.0; 
  _v[1] = -0.0422339;
  _v[2] = 35.2376;
  _v[3] = -2.10169;
  _v[4] = 24.8127;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275310;
  _v[0] = 1.0; 
  _v[1] = 0.149669;
  _v[2] = 34.7441;
  _v[3] = -2.92235;
  _v[4] = 23.9228;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275329;
  _v[0] = 1.0; 
  _v[1] = 1.59122;
  _v[2] = 557.094;
  _v[3] = -2145.17;
  _v[4] = 27.6604;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275344;
  _v[0] = 1.0; 
  _v[1] = -0.8903;
  _v[2] = 27.3821;
  _v[3] = -1.97161;
  _v[4] = 23.3085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275345;
  _v[0] = 1.0; 
  _v[1] = 0.3101;
  _v[2] = 32.9719;
  _v[3] = -2.8828;
  _v[4] = 25.1886;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275347;
  _v[0] = 1.0; 
  _v[1] = -0.841282;
  _v[2] = 38.4005;
  _v[3] = -1.46426;
  _v[4] = 26.6836;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275350;
  _v[0] = 1.0; 
  _v[1] = -0.841282;
  _v[2] = 38.4005;
  _v[3] = -1.46426;
  _v[4] = 26.6836;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275361;
  _v[0] = 1.0; 
  _v[1] = -0.00530827;
  _v[2] = 36.02;
  _v[3] = -2.42944;
  _v[4] = 24.6561;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275362;
  _v[0] = 1.0; 
  _v[1] = 2.36553;
  _v[2] = 38.1065;
  _v[3] = -6.0423;
  _v[4] = 25.5627;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275384;
  _v[0] = 1.0; 
  _v[1] = 0.226519;
  _v[2] = 36.9016;
  _v[3] = -2.93497;
  _v[4] = 24.2095;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275385;
  _v[0] = 1.0; 
  _v[1] = 0.226519;
  _v[2] = 36.9016;
  _v[3] = -2.93497;
  _v[4] = 24.2095;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275402;
  _v[0] = 1.0; 
  _v[1] = 0.252244;
  _v[2] = 35.2408;
  _v[3] = -2.75034;
  _v[4] = 24.4938;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275404;
  _v[0] = 1.0; 
  _v[1] = 0.252244;
  _v[2] = 35.2408;
  _v[3] = -2.75034;
  _v[4] = 24.4938;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275405;
  _v[0] = 1.0; 
  _v[1] = 0.125926;
  _v[2] = 42.6383;
  _v[3] = -2.86727;
  _v[4] = 27.2148;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275420;
  _v[0] = 1.0; 
  _v[1] = -0.0845064;
  _v[2] = 35.995;
  _v[3] = -2.34686;
  _v[4] = 24.3823;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275421;
  _v[0] = 1.0; 
  _v[1] = -0.0845064;
  _v[2] = 35.995;
  _v[3] = -2.34686;
  _v[4] = 24.3823;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275422;
  _v[0] = 1.0; 
  _v[1] = -0.0845064;
  _v[2] = 35.995;
  _v[3] = -2.34686;
  _v[4] = 24.3823;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275453;
  _v[0] = 1.0; 
  _v[1] = 0.3166;
  _v[2] = 28.5372;
  _v[3] = -2.33284;
  _v[4] = 21.6247;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275575;
  _v[0] = 1.0; 
  _v[1] = 0.596154;
  _v[2] = 35.3562;
  _v[3] = -3.1337;
  _v[4] = 24.5967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275576;
  _v[0] = 1.0; 
  _v[1] = 0.596154;
  _v[2] = 35.3562;
  _v[3] = -3.1337;
  _v[4] = 24.5967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275614;
  _v[0] = 1.0; 
  _v[1] = -0.190836;
  _v[2] = 33.032;
  _v[3] = -2.75459;
  _v[4] = 24.354;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275615;
  _v[0] = 1.0; 
  _v[1] = -0.190836;
  _v[2] = 33.032;
  _v[3] = -2.75459;
  _v[4] = 24.354;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275629;
  _v[0] = 1.0; 
  _v[1] = 0.799996;
  _v[2] = 30.3287;
  _v[3] = -4.22;
  _v[4] = 23.9082;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275630;
  _v[0] = 1.0; 
  _v[1] = 0.506206;
  _v[2] = 36.6523;
  _v[3] = -2.66612;
  _v[4] = 25.4139;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275649;
  _v[0] = 1.0; 
  _v[1] = 1.60966;
  _v[2] = 38.7152;
  _v[3] = -4.47608;
  _v[4] = 28.654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275652;
  _v[0] = 1.0; 
  _v[1] = 1.60966;
  _v[2] = 38.7152;
  _v[3] = -4.47608;
  _v[4] = 28.654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275654;
  _v[0] = 1.0; 
  _v[1] = 1.60966;
  _v[2] = 38.7152;
  _v[3] = -4.47608;
  _v[4] = 28.654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275665;
  _v[0] = 1.0; 
  _v[1] = 1.60966;
  _v[2] = 38.7152;
  _v[3] = -4.47608;
  _v[4] = 28.654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275681;
  _v[0] = 1.0; 
  _v[1] = 0.700216;
  _v[2] = 34.8949;
  _v[3] = -3.06044;
  _v[4] = 25.773;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275692;
  _v[0] = 1.0; 
  _v[1] = -0.174294;
  _v[2] = 50.0038;
  _v[3] = -6.36205;
  _v[4] = 22.1479;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275706;
  _v[0] = 1.0; 
  _v[1] = -0.054505;
  _v[2] = 31.4326;
  _v[3] = -2.4614;
  _v[4] = 23.6767;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275707;
  _v[0] = 1.0; 
  _v[1] = 0.115799;
  _v[2] = 37.2331;
  _v[3] = -2.69921;
  _v[4] = 25.7692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275708;
  _v[0] = 1.0; 
  _v[1] = 0.115799;
  _v[2] = 37.2331;
  _v[3] = -2.69921;
  _v[4] = 25.7692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275709;
  _v[0] = 1.0; 
  _v[1] = 0.115799;
  _v[2] = 37.2331;
  _v[3] = -2.69921;
  _v[4] = 25.7692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275710;
  _v[0] = 1.0; 
  _v[1] = 2.0766;
  _v[2] = 40.9051;
  _v[3] = -4.85768;
  _v[4] = 26.5818;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275711;
  _v[0] = 1.0; 
  _v[1] = 2.0766;
  _v[2] = 40.9051;
  _v[3] = -4.85768;
  _v[4] = 26.5818;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275726;
  _v[0] = 1.0; 
  _v[1] = 0.993889;
  _v[2] = 28.6193;
  _v[3] = -3.60461;
  _v[4] = 23.5021;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275727;
  _v[0] = 1.0; 
  _v[1] = 0.444897;
  _v[2] = 33.4432;
  _v[3] = -2.28858;
  _v[4] = 25.4821;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275728;
  _v[0] = 1.0; 
  _v[1] = 0.444897;
  _v[2] = 33.4432;
  _v[3] = -2.28858;
  _v[4] = 25.4821;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275729;
  _v[0] = 1.0; 
  _v[1] = 0.564406;
  _v[2] = 37.6512;
  _v[3] = -2.05013;
  _v[4] = 28.1306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275730;
  _v[0] = 1.0; 
  _v[1] = 0.564406;
  _v[2] = 37.6512;
  _v[3] = -2.05013;
  _v[4] = 28.1306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275731;
  _v[0] = 1.0; 
  _v[1] = 0.564406;
  _v[2] = 37.6512;
  _v[3] = -2.05013;
  _v[4] = 28.1306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275741;
  _v[0] = 1.0; 
  _v[1] = -0.493084;
  _v[2] = 34.0992;
  _v[3] = -2.03226;
  _v[4] = 24.1001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275787;
  _v[0] = 1.0; 
  _v[1] = -0.191876;
  _v[2] = 33.7625;
  _v[3] = -2.05933;
  _v[4] = 24.3772;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275804;
  _v[0] = 1.0; 
  _v[1] = -0.0352934;
  _v[2] = 35.9459;
  _v[3] = -2.31877;
  _v[4] = 24.5991;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275821;
  _v[0] = 1.0; 
  _v[1] = 0.0581691;
  _v[2] = 35.0832;
  _v[3] = -2.78111;
  _v[4] = 25.123;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275837;
  _v[0] = 1.0; 
  _v[1] = -0.552943;
  _v[2] = 28.8969;
  _v[3] = -2.70531;
  _v[4] = 21.6345;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275839;
  _v[0] = 1.0; 
  _v[1] = -0.552943;
  _v[2] = 28.8969;
  _v[3] = -2.70531;
  _v[4] = 21.6345;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275841;
  _v[0] = 1.0; 
  _v[1] = -0.552943;
  _v[2] = 28.8969;
  _v[3] = -2.70531;
  _v[4] = 21.6345;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275842;
  _v[0] = 1.0; 
  _v[1] = 1.32929;
  _v[2] = 31.9173;
  _v[3] = -4.36778;
  _v[4] = 24.1541;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275848;
  _v[0] = 1.0; 
  _v[1] = 0.92853;
  _v[2] = 38.1268;
  _v[3] = -3.45249;
  _v[4] = 27.2154;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275873;
  _v[0] = 1.0; 
  _v[1] = -0.237079;
  _v[2] = 35.595;
  _v[3] = -2.34486;
  _v[4] = 24.6283;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275906;
  _v[0] = 1.0; 
  _v[1] = 0.305027;
  _v[2] = 34.2627;
  _v[3] = -2.87318;
  _v[4] = 24.3345;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275907;
  _v[0] = 1.0; 
  _v[1] = -0.525686;
  _v[2] = 40.1414;
  _v[3] = -1.75814;
  _v[4] = 26.5477;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275960;
  _v[0] = 1.0; 
  _v[1] = -0.133526;
  _v[2] = 34.7117;
  _v[3] = -2.05513;
  _v[4] = 24.4131;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275961;
  _v[0] = 1.0; 
  _v[1] = -0.133526;
  _v[2] = 34.7117;
  _v[3] = -2.05513;
  _v[4] = 24.4131;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275962;
  _v[0] = 1.0; 
  _v[1] = 0.779201;
  _v[2] = 41.1654;
  _v[3] = -3.91032;
  _v[4] = 27.6574;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275980;
  _v[0] = 1.0; 
  _v[1] = -0.073917;
  _v[2] = 33.9214;
  _v[3] = -2.36809;
  _v[4] = 24.7505;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275983;
  _v[0] = 1.0; 
  _v[1] = 0.268659;
  _v[2] = 37.0914;
  _v[3] = -3.39045;
  _v[4] = 24.8883;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 275986;
  _v[0] = 1.0; 
  _v[1] = 0.268659;
  _v[2] = 37.0914;
  _v[3] = -3.39045;
  _v[4] = 24.8883;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276047;
  _v[0] = 1.0; 
  _v[1] = 0.932166;
  _v[2] = 35.999;
  _v[3] = -3.90522;
  _v[4] = 26.2919;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276067;
  _v[0] = 1.0; 
  _v[1] = -0.44436;
  _v[2] = 30.4496;
  _v[3] = -2.41845;
  _v[4] = 22.3894;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276069;
  _v[0] = 1.0; 
  _v[1] = -0.214649;
  _v[2] = 37.2895;
  _v[3] = -1.62507;
  _v[4] = 25.7109;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276086;
  _v[0] = 1.0; 
  _v[1] = -0.204348;
  _v[2] = 35.5942;
  _v[3] = -2.61437;
  _v[4] = 24.3045;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276105;
  _v[0] = 1.0; 
  _v[1] = 0.0321221;
  _v[2] = 41.9217;
  _v[3] = -2.49435;
  _v[4] = 27.7648;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276106;
  _v[0] = 1.0; 
  _v[1] = 0.0321221;
  _v[2] = 41.9217;
  _v[3] = -2.49435;
  _v[4] = 27.7648;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276119;
  _v[0] = 1.0; 
  _v[1] = 0.0735963;
  _v[2] = 32.4929;
  _v[3] = -3.06879;
  _v[4] = 23.4061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276120;
  _v[0] = 1.0; 
  _v[1] = -0.362846;
  _v[2] = 35.1771;
  _v[3] = -1.59097;
  _v[4] = 28.3362;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276121;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276122;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276123;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276124;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276125;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276126;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276129;
  _v[0] = 1.0; 
  _v[1] = 0.258737;
  _v[2] = 38.2448;
  _v[3] = -3.07751;
  _v[4] = 27.6203;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276152;
  _v[0] = 1.0; 
  _v[1] = -0.120381;
  _v[2] = 31.994;
  _v[3] = -1.99673;
  _v[4] = 23.7728;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276153;
  _v[0] = 1.0; 
  _v[1] = -0.216677;
  _v[2] = 35.6758;
  _v[3] = -1.35894;
  _v[4] = 28.2129;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276154;
  _v[0] = 1.0; 
  _v[1] = -0.216677;
  _v[2] = 35.6758;
  _v[3] = -1.35894;
  _v[4] = 28.2129;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276160;
  _v[0] = 1.0; 
  _v[1] = -0.255498;
  _v[2] = 40.123;
  _v[3] = -1.42789;
  _v[4] = 27.3719;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276161;
  _v[0] = 1.0; 
  _v[1] = -0.255498;
  _v[2] = 40.123;
  _v[3] = -1.42789;
  _v[4] = 27.3719;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276162;
  _v[0] = 1.0; 
  _v[1] = -0.255498;
  _v[2] = 40.123;
  _v[3] = -1.42789;
  _v[4] = 27.3719;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276180;
  _v[0] = 1.0; 
  _v[1] = 0.536453;
  _v[2] = 35.2895;
  _v[3] = -3.58455;
  _v[4] = 24.5016;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276181;
  _v[0] = 1.0; 
  _v[1] = 0.536453;
  _v[2] = 35.2895;
  _v[3] = -3.58455;
  _v[4] = 24.5016;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276271;
  _v[0] = 1.0; 
  _v[1] = 0.241757;
  _v[2] = 37.574;
  _v[3] = -1.97533;
  _v[4] = 26.9319;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276288;
  _v[0] = 1.0; 
  _v[1] = 0.0758476;
  _v[2] = 35.5401;
  _v[3] = -2.88907;
  _v[4] = 25.1021;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276289;
  _v[0] = 1.0; 
  _v[1] = 0.0758476;
  _v[2] = 35.5401;
  _v[3] = -2.88907;
  _v[4] = 25.1021;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276290;
  _v[0] = 1.0; 
  _v[1] = 0.0758476;
  _v[2] = 35.5401;
  _v[3] = -2.88907;
  _v[4] = 25.1021;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276304;
  _v[0] = 1.0; 
  _v[1] = -0.102021;
  _v[2] = 36.6405;
  _v[3] = -2.6351;
  _v[4] = 24.469;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276320;
  _v[0] = 1.0; 
  _v[1] = -0.411996;
  _v[2] = 39.0262;
  _v[3] = -1.93633;
  _v[4] = 25.0595;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276395;
  _v[0] = 1.0; 
  _v[1] = 1.06693;
  _v[2] = 36.4482;
  _v[3] = -3.5146;
  _v[4] = 25.6788;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276403;
  _v[0] = 1.0; 
  _v[1] = 1.06693;
  _v[2] = 36.4482;
  _v[3] = -3.5146;
  _v[4] = 25.6788;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276406;
  _v[0] = 1.0; 
  _v[1] = 1.06693;
  _v[2] = 36.4482;
  _v[3] = -3.5146;
  _v[4] = 25.6788;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276448;
  _v[0] = 1.0; 
  _v[1] = 0.824269;
  _v[2] = 32.6181;
  _v[3] = -3.23264;
  _v[4] = 26.4727;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276597;
  _v[0] = 1.0; 
  _v[1] = 0.397406;
  _v[2] = 32.4347;
  _v[3] = -2.48469;
  _v[4] = 25.7635;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276600;
  _v[0] = 1.0; 
  _v[1] = 1.58479;
  _v[2] = 35.0715;
  _v[3] = -4.72639;
  _v[4] = 29.3146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276601;
  _v[0] = 1.0; 
  _v[1] = 1.58479;
  _v[2] = 35.0715;
  _v[3] = -4.72639;
  _v[4] = 29.3146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276602;
  _v[0] = 1.0; 
  _v[1] = 1.58479;
  _v[2] = 35.0715;
  _v[3] = -4.72639;
  _v[4] = 29.3146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276633;
  _v[0] = 1.0; 
  _v[1] = 0.533798;
  _v[2] = 32.6611;
  _v[3] = -3.87245;
  _v[4] = 23.8709;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276634;
  _v[0] = 1.0; 
  _v[1] = 0.288802;
  _v[2] = 39.1436;
  _v[3] = -2.60035;
  _v[4] = 26.2056;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276647;
  _v[0] = 1.0; 
  _v[1] = 0.381148;
  _v[2] = 31.8933;
  _v[3] = -3.42904;
  _v[4] = 23.599;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276648;
  _v[0] = 1.0; 
  _v[1] = 0.404154;
  _v[2] = 36.5327;
  _v[3] = -2.79948;
  _v[4] = 27.83;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276649;
  _v[0] = 1.0; 
  _v[1] = 0.509602;
  _v[2] = 40.8717;
  _v[3] = -3.33512;
  _v[4] = 27.2348;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276650;
  _v[0] = 1.0; 
  _v[1] = -1.11931;
  _v[2] = 46.2814;
  _v[3] = -1.14456;
  _v[4] = 26.8046;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276651;
  _v[0] = 1.0; 
  _v[1] = -1.11931;
  _v[2] = 46.2814;
  _v[3] = -1.14456;
  _v[4] = 26.8046;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276652;
  _v[0] = 1.0; 
  _v[1] = -1.11931;
  _v[2] = 46.2814;
  _v[3] = -1.14456;
  _v[4] = 26.8046;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276665;
  _v[0] = 1.0; 
  _v[1] = -0.608836;
  _v[2] = 33.043;
  _v[3] = -1.892;
  _v[4] = 24.665;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276668;
  _v[0] = 1.0; 
  _v[1] = 0.909136;
  _v[2] = 38.3341;
  _v[3] = -4.1136;
  _v[4] = 27.9117;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276669;
  _v[0] = 1.0; 
  _v[1] = 0.909136;
  _v[2] = 38.3341;
  _v[3] = -4.1136;
  _v[4] = 27.9117;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276680;
  _v[0] = 1.0; 
  _v[1] = -0.12664;
  _v[2] = 36.5736;
  _v[3] = -2.6383;
  _v[4] = 24.6692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276695;
  _v[0] = 1.0; 
  _v[1] = 0.260779;
  _v[2] = 36.2375;
  _v[3] = -3.49474;
  _v[4] = 24.9321;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276769;
  _v[0] = 1.0; 
  _v[1] = 0.320079;
  _v[2] = 35.15;
  _v[3] = -3.05056;
  _v[4] = 24.5793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276782;
  _v[0] = 1.0; 
  _v[1] = -0.025086;
  _v[2] = 31.4597;
  _v[3] = -3.02881;
  _v[4] = 24.4002;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276783;
  _v[0] = 1.0; 
  _v[1] = 1.14794;
  _v[2] = 36.8133;
  _v[3] = -3.63341;
  _v[4] = 26.2913;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276801;
  _v[0] = 1.0; 
  _v[1] = 0.426318;
  _v[2] = 30.0335;
  _v[3] = -3.71814;
  _v[4] = 22.7579;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276857;
  _v[0] = 1.0; 
  _v[1] = 0.0356912;
  _v[2] = 28.9475;
  _v[3] = -3.54763;
  _v[4] = 22.4525;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276858;
  _v[0] = 1.0; 
  _v[1] = 0.0356912;
  _v[2] = 28.9475;
  _v[3] = -3.54763;
  _v[4] = 22.4525;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276859;
  _v[0] = 1.0; 
  _v[1] = 0.0356912;
  _v[2] = 28.9475;
  _v[3] = -3.54763;
  _v[4] = 22.4525;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276861;
  _v[0] = 1.0; 
  _v[1] = 0.0356912;
  _v[2] = 28.9475;
  _v[3] = -3.54763;
  _v[4] = 22.4525;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276862;
  _v[0] = 1.0; 
  _v[1] = 0.0356912;
  _v[2] = 28.9475;
  _v[3] = -3.54763;
  _v[4] = 22.4525;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276864;
  _v[0] = 1.0; 
  _v[1] = -0.626075;
  _v[2] = 38.4955;
  _v[3] = -1.78326;
  _v[4] = 23.4944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276866;
  _v[0] = 1.0; 
  _v[1] = -0.626075;
  _v[2] = 38.4955;
  _v[3] = -1.78326;
  _v[4] = 23.4944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276868;
  _v[0] = 1.0; 
  _v[1] = -0.626075;
  _v[2] = 38.4955;
  _v[3] = -1.78326;
  _v[4] = 23.4944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276869;
  _v[0] = 1.0; 
  _v[1] = -0.626075;
  _v[2] = 38.4955;
  _v[3] = -1.78326;
  _v[4] = 23.4944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276870;
  _v[0] = 1.0; 
  _v[1] = 0.84413;
  _v[2] = 39.2285;
  _v[3] = -2.24915;
  _v[4] = 26.7466;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276871;
  _v[0] = 1.0; 
  _v[1] = 0.84413;
  _v[2] = 39.2285;
  _v[3] = -2.24915;
  _v[4] = 26.7466;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276872;
  _v[0] = 1.0; 
  _v[1] = -0.239274;
  _v[2] = 39.7501;
  _v[3] = -1.68806;
  _v[4] = 23.9919;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276873;
  _v[0] = 1.0; 
  _v[1] = -0.239274;
  _v[2] = 39.7501;
  _v[3] = -1.68806;
  _v[4] = 23.9919;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276889;
  _v[0] = 1.0; 
  _v[1] = -0.373612;
  _v[2] = 31.2826;
  _v[3] = -2.20024;
  _v[4] = 23.2944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276892;
  _v[0] = 1.0; 
  _v[1] = -0.373612;
  _v[2] = 31.2826;
  _v[3] = -2.20024;
  _v[4] = 23.2944;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276893;
  _v[0] = 1.0; 
  _v[1] = 0.870806;
  _v[2] = 34.9321;
  _v[3] = -4.62215;
  _v[4] = 23.9928;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276895;
  _v[0] = 1.0; 
  _v[1] = 0.870806;
  _v[2] = 34.9321;
  _v[3] = -4.62215;
  _v[4] = 23.9928;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276896;
  _v[0] = 1.0; 
  _v[1] = 0.798327;
  _v[2] = 36.0452;
  _v[3] = -3.57246;
  _v[4] = 25.3914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276897;
  _v[0] = 1.0; 
  _v[1] = 0.798327;
  _v[2] = 36.0452;
  _v[3] = -3.57246;
  _v[4] = 25.3914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276899;
  _v[0] = 1.0; 
  _v[1] = -1.47142;
  _v[2] = 37.788;
  _v[3] = -0.668408;
  _v[4] = 27.4329;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276900;
  _v[0] = 1.0; 
  _v[1] = -1.47142;
  _v[2] = 37.788;
  _v[3] = -0.668408;
  _v[4] = 27.4329;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276918;
  _v[0] = 1.0; 
  _v[1] = -0.236899;
  _v[2] = 33.6984;
  _v[3] = -2.3907;
  _v[4] = 24.8587;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276961;
  _v[0] = 1.0; 
  _v[1] = 0.896937;
  _v[2] = 31.9069;
  _v[3] = -4.13657;
  _v[4] = 23.755;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276990;
  _v[0] = 1.0; 
  _v[1] = 0.896937;
  _v[2] = 31.9069;
  _v[3] = -4.13657;
  _v[4] = 23.755;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276991;
  _v[0] = 1.0; 
  _v[1] = -1.96044;
  _v[2] = 41.1773;
  _v[3] = 0.757062;
  _v[4] = 25.9313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276993;
  _v[0] = 1.0; 
  _v[1] = -1.96044;
  _v[2] = 41.1773;
  _v[3] = 0.757062;
  _v[4] = 25.9313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276994;
  _v[0] = 1.0; 
  _v[1] = -1.96044;
  _v[2] = 41.1773;
  _v[3] = 0.757062;
  _v[4] = 25.9313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 276996;
  _v[0] = 1.0; 
  _v[1] = -1.96044;
  _v[2] = 41.1773;
  _v[3] = 0.757062;
  _v[4] = 25.9313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277000;
  _v[0] = 1.0; 
  _v[1] = 1.8421;
  _v[2] = 41.8969;
  _v[3] = -3.54046;
  _v[4] = 30.8522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277001;
  _v[0] = 1.0; 
  _v[1] = 1.8421;
  _v[2] = 41.8969;
  _v[3] = -3.54046;
  _v[4] = 30.8522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277002;
  _v[0] = 1.0; 
  _v[1] = 1.8421;
  _v[2] = 41.8969;
  _v[3] = -3.54046;
  _v[4] = 30.8522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277005;
  _v[0] = 1.0; 
  _v[1] = 1.8421;
  _v[2] = 41.8969;
  _v[3] = -3.54046;
  _v[4] = 30.8522;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277038;
  _v[0] = 1.0; 
  _v[1] = 1.24119;
  _v[2] = 29.2949;
  _v[3] = -4.80785;
  _v[4] = 23.0672;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277040;
  _v[0] = 1.0; 
  _v[1] = 0.93549;
  _v[2] = 34.1737;
  _v[3] = -3.77389;
  _v[4] = 24.7013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277041;
  _v[0] = 1.0; 
  _v[1] = 0.93549;
  _v[2] = 34.1737;
  _v[3] = -3.77389;
  _v[4] = 24.7013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277042;
  _v[0] = 1.0; 
  _v[1] = -0.159624;
  _v[2] = 37.9677;
  _v[3] = -2.1244;
  _v[4] = 27.0431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277045;
  _v[0] = 1.0; 
  _v[1] = -0.159624;
  _v[2] = 37.9677;
  _v[3] = -2.1244;
  _v[4] = 27.0431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277057;
  _v[0] = 1.0; 
  _v[1] = -0.24535;
  _v[2] = 29.7954;
  _v[3] = -2.28571;
  _v[4] = 24.9118;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277105;
  _v[0] = 1.0; 
  _v[1] = -0.0223222;
  _v[2] = 33.509;
  _v[3] = -2.57293;
  _v[4] = 24.0181;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277106;
  _v[0] = 1.0; 
  _v[1] = 0.192328;
  _v[2] = 39.5942;
  _v[3] = -3.99678;
  _v[4] = 23.8292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277108;
  _v[0] = 1.0; 
  _v[1] = 0.192328;
  _v[2] = 39.5942;
  _v[3] = -3.99678;
  _v[4] = 23.8292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277170;
  _v[0] = 1.0; 
  _v[1] = -0.39608;
  _v[2] = 35.1767;
  _v[3] = -2.66866;
  _v[4] = 24.5485;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277195;
  _v[0] = 1.0; 
  _v[1] = -0.0401769;
  _v[2] = 30.7898;
  _v[3] = -2.56874;
  _v[4] = 24.84;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277196;
  _v[0] = 1.0; 
  _v[1] = -0.245698;
  _v[2] = 36.5812;
  _v[3] = -2.09521;
  _v[4] = 25.8567;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277197;
  _v[0] = 1.0; 
  _v[1] = 3.07843;
  _v[2] = 39.1033;
  _v[3] = -7.89737;
  _v[4] = 25.97;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277218;
  _v[0] = 1.0; 
  _v[1] = 0.96434;
  _v[2] = 27.3008;
  _v[3] = -3.71545;
  _v[4] = 23.9471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277288;
  _v[0] = 1.0; 
  _v[1] = -0.211868;
  _v[2] = 31.8862;
  _v[3] = -3.14211;
  _v[4] = 24.5464;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277290;
  _v[0] = 1.0; 
  _v[1] = 0.116029;
  _v[2] = 38.7072;
  _v[3] = -2.84853;
  _v[4] = 27.5226;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277291;
  _v[0] = 1.0; 
  _v[1] = 0.116029;
  _v[2] = 38.7072;
  _v[3] = -2.84853;
  _v[4] = 27.5226;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277292;
  _v[0] = 1.0; 
  _v[1] = 0.414294;
  _v[2] = 40.4008;
  _v[3] = -6.54624;
  _v[4] = 26.4606;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277312;
  _v[0] = 1.0; 
  _v[1] = 0.528138;
  _v[2] = 31.5643;
  _v[3] = -4.14694;
  _v[4] = 23.0899;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277317;
  _v[0] = 1.0; 
  _v[1] = 0.609301;
  _v[2] = 39.8909;
  _v[3] = -3.11972;
  _v[4] = 26.6654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277318;
  _v[0] = 1.0; 
  _v[1] = 0.609301;
  _v[2] = 39.8909;
  _v[3] = -3.11972;
  _v[4] = 26.6654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277319;
  _v[0] = 1.0; 
  _v[1] = -0.11443;
  _v[2] = 39.7034;
  _v[3] = -3.52601;
  _v[4] = 28.8024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277321;
  _v[0] = 1.0; 
  _v[1] = -0.186403;
  _v[2] = 41.8446;
  _v[3] = -2.56356;
  _v[4] = 29.1691;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277322;
  _v[0] = 1.0; 
  _v[1] = -1.14092;
  _v[2] = 926.271;
  _v[3] = 3156.76;
  _v[4] = 19.2318;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277338;
  _v[0] = 1.0; 
  _v[1] = -1.12357;
  _v[2] = 30.889;
  _v[3] = -1.89185;
  _v[4] = 20.6079;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277340;
  _v[0] = 1.0; 
  _v[1] = 0.112541;
  _v[2] = 35.053;
  _v[3] = -3.27081;
  _v[4] = 25.47;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277341;
  _v[0] = 1.0; 
  _v[1] = 0.112541;
  _v[2] = 35.053;
  _v[3] = -3.27081;
  _v[4] = 25.47;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277356;
  _v[0] = 1.0; 
  _v[1] = -0.0641342;
  _v[2] = 29.1427;
  _v[3] = -3.10573;
  _v[4] = 23.877;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277357;
  _v[0] = 1.0; 
  _v[1] = -0.176691;
  _v[2] = 35.3498;
  _v[3] = -2.85774;
  _v[4] = 25.1972;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277360;
  _v[0] = 1.0; 
  _v[1] = 0.344901;
  _v[2] = 37.871;
  _v[3] = -2.45084;
  _v[4] = 28.8869;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277371;
  _v[0] = 1.0; 
  _v[1] = 0.482142;
  _v[2] = 55.333;
  _v[3] = -9.45805;
  _v[4] = 21.3175;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277390;
  _v[0] = 1.0; 
  _v[1] = -0.396628;
  _v[2] = 34.8395;
  _v[3] = -2.56839;
  _v[4] = 23.9476;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277454;
  _v[0] = 1.0; 
  _v[1] = -0.311017;
  _v[2] = 35.8593;
  _v[3] = -3.16706;
  _v[4] = 24.1538;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277491;
  _v[0] = 1.0; 
  _v[1] = 0.795885;
  _v[2] = 34.6776;
  _v[3] = -3.75452;
  _v[4] = 25.6484;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277505;
  _v[0] = 1.0; 
  _v[1] = -0.239258;
  _v[2] = 34.4161;
  _v[3] = -2.70195;
  _v[4] = 23.9342;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277507;
  _v[0] = 1.0; 
  _v[1] = 0.0371352;
  _v[2] = 42.4696;
  _v[3] = -3.25061;
  _v[4] = 26.8872;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277509;
  _v[0] = 1.0; 
  _v[1] = 0.0371352;
  _v[2] = 42.4696;
  _v[3] = -3.25061;
  _v[4] = 26.8872;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 277511;
  _v[0] = 1.0; 
  _v[1] = 0.0371352;
  _v[2] = 42.4696;
  _v[3] = -3.25061;
  _v[4] = 26.8872;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 282976;
  _v[0] = 1.0; 
  _v[1] = 1.01802;
  _v[2] = 36.4995;
  _v[3] = -1.77121;
  _v[4] = 27.1859;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 282977;
  _v[0] = 1.0; 
  _v[1] = 1.01802;
  _v[2] = 36.4995;
  _v[3] = -1.77121;
  _v[4] = 27.1859;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 282978;
  _v[0] = 1.0; 
  _v[1] = 1.01802;
  _v[2] = 36.4995;
  _v[3] = -1.77121;
  _v[4] = 27.1859;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283130;
  _v[0] = 1.0; 
  _v[1] = -0.773883;
  _v[2] = 34.8737;
  _v[3] = -1.1216;
  _v[4] = 25.4836;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283132;
  _v[0] = 1.0; 
  _v[1] = -0.0339485;
  _v[2] = 31.8827;
  _v[3] = -1.4207;
  _v[4] = 26.075;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283133;
  _v[0] = 1.0; 
  _v[1] = -0.0313168;
  _v[2] = 32.5624;
  _v[3] = -0.590658;
  _v[4] = 28.0041;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283146;
  _v[0] = 1.0; 
  _v[1] = -0.570428;
  _v[2] = 38.1094;
  _v[3] = -1.57436;
  _v[4] = 26.5374;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283149;
  _v[0] = 1.0; 
  _v[1] = 0.129258;
  _v[2] = 32.2983;
  _v[3] = -2.24964;
  _v[4] = 23.8854;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283152;
  _v[0] = 1.0; 
  _v[1] = -1.1422;
  _v[2] = 35.835;
  _v[3] = 0.326501;
  _v[4] = 26.0213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283154;
  _v[0] = 1.0; 
  _v[1] = 1.35953;
  _v[2] = 39.3889;
  _v[3] = -3.22898;
  _v[4] = 28.2091;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283179;
  _v[0] = 1.0; 
  _v[1] = 0.304935;
  _v[2] = 33.3248;
  _v[3] = -1.88908;
  _v[4] = 27.1496;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283185;
  _v[0] = 1.0; 
  _v[1] = -0.149864;
  _v[2] = 37.3172;
  _v[3] = -1.92174;
  _v[4] = 27.3499;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283186;
  _v[0] = 1.0; 
  _v[1] = -0.162824;
  _v[2] = 35.5691;
  _v[3] = -1.7287;
  _v[4] = 22.9704;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283198;
  _v[0] = 1.0; 
  _v[1] = -0.231824;
  _v[2] = 34.1636;
  _v[3] = -1.73674;
  _v[4] = 24.474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283199;
  _v[0] = 1.0; 
  _v[1] = -0.00241389;
  _v[2] = 39.4255;
  _v[3] = -1.08352;
  _v[4] = 26.0139;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283218;
  _v[0] = 1.0; 
  _v[1] = -0.731736;
  _v[2] = 33.7767;
  _v[3] = -0.440384;
  _v[4] = 27.1874;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283243;
  _v[0] = 1.0; 
  _v[1] = -1.2905;
  _v[2] = 32.5232;
  _v[3] = -0.0939261;
  _v[4] = 26.0676;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283244;
  _v[0] = 1.0; 
  _v[1] = 4.21017;
  _v[2] = 34.377;
  _v[3] = -6.40077;
  _v[4] = 28.8396;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283250;
  _v[0] = 1.0; 
  _v[1] = 4.21017;
  _v[2] = 34.377;
  _v[3] = -6.40077;
  _v[4] = 28.8396;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283265;
  _v[0] = 1.0; 
  _v[1] = -0.391948;
  _v[2] = 33.6346;
  _v[3] = -0.671521;
  _v[4] = 22.6906;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283268;
  _v[0] = 1.0; 
  _v[1] = -0.391948;
  _v[2] = 33.6346;
  _v[3] = -0.671521;
  _v[4] = 22.6906;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283335;
  _v[0] = 1.0; 
  _v[1] = -1.03383;
  _v[2] = 30.5302;
  _v[3] = -0.771117;
  _v[4] = 25.8033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283341;
  _v[0] = 1.0; 
  _v[1] = 0.645014;
  _v[2] = 35.0126;
  _v[3] = -3.06626;
  _v[4] = 27.3206;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283346;
  _v[0] = 1.0; 
  _v[1] = -0.225898;
  _v[2] = 38.7967;
  _v[3] = -0.763092;
  _v[4] = 28.1701;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283385;
  _v[0] = 1.0; 
  _v[1] = 0.198976;
  _v[2] = 40.9094;
  _v[3] = -2.25306;
  _v[4] = 27.051;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283412;
  _v[0] = 1.0; 
  _v[1] = 0.478922;
  _v[2] = 38.7478;
  _v[3] = -2.71849;
  _v[4] = 28.3062;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283573;
  _v[0] = 1.0; 
  _v[1] = 9.14497;
  _v[2] = 49.7033;
  _v[3] = -62.8455;
  _v[4] = 21.7487;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283604;
  _v[0] = 1.0; 
  _v[1] = 0.957488;
  _v[2] = 36.8983;
  _v[3] = -4.29095;
  _v[4] = 27.1759;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283707;
  _v[0] = 1.0; 
  _v[1] = -0.59894;
  _v[2] = 33.9115;
  _v[3] = -2.12514;
  _v[4] = 27.828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283711;
  _v[0] = 1.0; 
  _v[1] = -0.59894;
  _v[2] = 33.9115;
  _v[3] = -2.12514;
  _v[4] = 27.828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283712;
  _v[0] = 1.0; 
  _v[1] = -0.59894;
  _v[2] = 33.9115;
  _v[3] = -2.12514;
  _v[4] = 27.828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283714;
  _v[0] = 1.0; 
  _v[1] = -0.59894;
  _v[2] = 33.9115;
  _v[3] = -2.12514;
  _v[4] = 27.828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283718;
  _v[0] = 1.0; 
  _v[1] = -0.210053;
  _v[2] = 34.8482;
  _v[3] = -2.87712;
  _v[4] = 26.2182;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283765;
  _v[0] = 1.0; 
  _v[1] = 0.119963;
  _v[2] = 40.9903;
  _v[3] = -2.70551;
  _v[4] = 26.3931;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283785;
  _v[0] = 1.0; 
  _v[1] = 0.390904;
  _v[2] = 33.182;
  _v[3] = -4.0973;
  _v[4] = 25.5031;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283786;
  _v[0] = 1.0; 
  _v[1] = 0.390904;
  _v[2] = 33.182;
  _v[3] = -4.0973;
  _v[4] = 25.5031;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283839;
  _v[0] = 1.0; 
  _v[1] = -1.26269;
  _v[2] = 32.0853;
  _v[3] = -0.439808;
  _v[4] = 25.1413;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283848;
  _v[0] = 1.0; 
  _v[1] = 0.621013;
  _v[2] = 35.0491;
  _v[3] = -3.69249;
  _v[4] = 27.5619;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283949;
  _v[0] = 1.0; 
  _v[1] = 1.41706;
  _v[2] = 36.4534;
  _v[3] = -5.44096;
  _v[4] = 26.7436;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283950;
  _v[0] = 1.0; 
  _v[1] = 1.41706;
  _v[2] = 36.4534;
  _v[3] = -5.44096;
  _v[4] = 26.7436;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283955;
  _v[0] = 1.0; 
  _v[1] = 0.147326;
  _v[2] = 34.5684;
  _v[3] = -3.77424;
  _v[4] = 23.0276;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283979;
  _v[0] = 1.0; 
  _v[1] = -0.658959;
  _v[2] = 34.4382;
  _v[3] = -1.74476;
  _v[4] = 25.1845;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283980;
  _v[0] = 1.0; 
  _v[1] = 0.0618228;
  _v[2] = 40.2116;
  _v[3] = -2.78979;
  _v[4] = 28.1349;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283981;
  _v[0] = 1.0; 
  _v[1] = 0.0618228;
  _v[2] = 40.2116;
  _v[3] = -2.78979;
  _v[4] = 28.1349;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283995;
  _v[0] = 1.0; 
  _v[1] = -0.640948;
  _v[2] = 37.2315;
  _v[3] = -1.10951;
  _v[4] = 24.6469;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 283996;
  _v[0] = 1.0; 
  _v[1] = 0.155849;
  _v[2] = 41.0647;
  _v[3] = -2.77068;
  _v[4] = 26.673;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284051;
  _v[0] = 1.0; 
  _v[1] = 1.50362;
  _v[2] = 38.1592;
  _v[3] = -3.90895;
  _v[4] = 25.9173;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284053;
  _v[0] = 1.0; 
  _v[1] = 1.50362;
  _v[2] = 38.1592;
  _v[3] = -3.90895;
  _v[4] = 25.9173;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284058;
  _v[0] = 1.0; 
  _v[1] = 0.536581;
  _v[2] = 40.2639;
  _v[3] = -2.11397;
  _v[4] = 28.1195;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284069;
  _v[0] = 1.0; 
  _v[1] = 0.536581;
  _v[2] = 40.2639;
  _v[3] = -2.11397;
  _v[4] = 28.1195;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284111;
  _v[0] = 1.0; 
  _v[1] = 0.0597821;
  _v[2] = 39.9769;
  _v[3] = -2.25191;
  _v[4] = 26.9476;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284142;
  _v[0] = 1.0; 
  _v[1] = -0.526665;
  _v[2] = 32.4556;
  _v[3] = -1.74145;
  _v[4] = 25.3413;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284145;
  _v[0] = 1.0; 
  _v[1] = -0.404675;
  _v[2] = 37.3671;
  _v[3] = -2.13563;
  _v[4] = 28.1367;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284170;
  _v[0] = 1.0; 
  _v[1] = 0.2272;
  _v[2] = 37.2963;
  _v[3] = -2.65978;
  _v[4] = 24.3262;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284173;
  _v[0] = 1.0; 
  _v[1] = 1.50959;
  _v[2] = 40.7552;
  _v[3] = -5.0343;
  _v[4] = 28.1796;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284175;
  _v[0] = 1.0; 
  _v[1] = 1.50959;
  _v[2] = 40.7552;
  _v[3] = -5.0343;
  _v[4] = 28.1796;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284189;
  _v[0] = 1.0; 
  _v[1] = -0.760939;
  _v[2] = 35.4971;
  _v[3] = -1.18761;
  _v[4] = 24.8597;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284246;
  _v[0] = 1.0; 
  _v[1] = 0.00662855;
  _v[2] = 36.3091;
  _v[3] = -2.89757;
  _v[4] = 25.6708;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284265;
  _v[0] = 1.0; 
  _v[1] = 0.0153859;
  _v[2] = 35.2313;
  _v[3] = -2.92392;
  _v[4] = 24.761;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284266;
  _v[0] = 1.0; 
  _v[1] = 0.0153859;
  _v[2] = 35.2313;
  _v[3] = -2.92392;
  _v[4] = 24.761;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284267;
  _v[0] = 1.0; 
  _v[1] = -0.029123;
  _v[2] = 37.7422;
  _v[3] = -3.20484;
  _v[4] = 28.466;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284285;
  _v[0] = 1.0; 
  _v[1] = 0.690302;
  _v[2] = 34.4974;
  _v[3] = -3.18495;
  _v[4] = 25.3949;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284286;
  _v[0] = 1.0; 
  _v[1] = -0.177202;
  _v[2] = 42.2625;
  _v[3] = -1.39177;
  _v[4] = 27.7545;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284302;
  _v[0] = 1.0; 
  _v[1] = 1.19483;
  _v[2] = 33.5393;
  _v[3] = -4.06795;
  _v[4] = 25.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284313;
  _v[0] = 1.0; 
  _v[1] = 1.19484;
  _v[2] = 39.9309;
  _v[3] = -3.03272;
  _v[4] = 27.7024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284314;
  _v[0] = 1.0; 
  _v[1] = 1.19484;
  _v[2] = 39.9309;
  _v[3] = -3.03272;
  _v[4] = 27.7024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284315;
  _v[0] = 1.0; 
  _v[1] = 1.19484;
  _v[2] = 39.9309;
  _v[3] = -3.03272;
  _v[4] = 27.7024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284317;
  _v[0] = 1.0; 
  _v[1] = 1.19484;
  _v[2] = 39.9309;
  _v[3] = -3.03272;
  _v[4] = 27.7024;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284325;
  _v[0] = 1.0; 
  _v[1] = -0.105806;
  _v[2] = 33.5576;
  _v[3] = -2.3105;
  _v[4] = 26.3706;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284358;
  _v[0] = 1.0; 
  _v[1] = -0.123829;
  _v[2] = 35.5404;
  _v[3] = -2.62106;
  _v[4] = 26.4322;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284387;
  _v[0] = 1.0; 
  _v[1] = 0.0600705;
  _v[2] = 35.479;
  _v[3] = -2.46753;
  _v[4] = 26.6706;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284396;
  _v[0] = 1.0; 
  _v[1] = -0.197744;
  _v[2] = 34.1571;
  _v[3] = -2.91389;
  _v[4] = 24.7361;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284398;
  _v[0] = 1.0; 
  _v[1] = -0.197744;
  _v[2] = 34.1571;
  _v[3] = -2.91389;
  _v[4] = 24.7361;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284400;
  _v[0] = 1.0; 
  _v[1] = -0.197744;
  _v[2] = 34.1571;
  _v[3] = -2.91389;
  _v[4] = 24.7361;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284406;
  _v[0] = 1.0; 
  _v[1] = -0.185631;
  _v[2] = 39.695;
  _v[3] = -2.49768;
  _v[4] = 28.007;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284434;
  _v[0] = 1.0; 
  _v[1] = 0.0404231;
  _v[2] = 31.753;
  _v[3] = -3.68483;
  _v[4] = 25.0485;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284435;
  _v[0] = 1.0; 
  _v[1] = -0.242302;
  _v[2] = 35.6068;
  _v[3] = -2.75731;
  _v[4] = 26.7543;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284436;
  _v[0] = 1.0; 
  _v[1] = -0.242302;
  _v[2] = 35.6068;
  _v[3] = -2.75731;
  _v[4] = 26.7543;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284438;
  _v[0] = 1.0; 
  _v[1] = -0.242302;
  _v[2] = 35.6068;
  _v[3] = -2.75731;
  _v[4] = 26.7543;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284439;
  _v[0] = 1.0; 
  _v[1] = -2.12065;
  _v[2] = 40.2419;
  _v[3] = -0.102096;
  _v[4] = 26.8445;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284448;
  _v[0] = 1.0; 
  _v[1] = -2.12065;
  _v[2] = 40.2419;
  _v[3] = -0.102096;
  _v[4] = 26.8445;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284471;
  _v[0] = 1.0; 
  _v[1] = 0.141136;
  _v[2] = 33.2703;
  _v[3] = -2.62503;
  _v[4] = 25.0953;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284511;
  _v[0] = 1.0; 
  _v[1] = -0.546312;
  _v[2] = 35.1204;
  _v[3] = -1.12508;
  _v[4] = 28.018;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284514;
  _v[0] = 1.0; 
  _v[1] = -0.546312;
  _v[2] = 35.1204;
  _v[3] = -1.12508;
  _v[4] = 28.018;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284515;
  _v[0] = 1.0; 
  _v[1] = -0.546312;
  _v[2] = 35.1204;
  _v[3] = -1.12508;
  _v[4] = 28.018;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284516;
  _v[0] = 1.0; 
  _v[1] = 1.14131;
  _v[2] = 40.8077;
  _v[3] = -4.06469;
  _v[4] = 28.3446;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284517;
  _v[0] = 1.0; 
  _v[1] = 1.14131;
  _v[2] = 40.8077;
  _v[3] = -4.06469;
  _v[4] = 28.3446;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284533;
  _v[0] = 1.0; 
  _v[1] = -0.17639;
  _v[2] = 34.2534;
  _v[3] = -2.81136;
  _v[4] = 25.5741;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284534;
  _v[0] = 1.0; 
  _v[1] = 0.0144433;
  _v[2] = 39.9101;
  _v[3] = -2.01728;
  _v[4] = 26.9705;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284548;
  _v[0] = 1.0; 
  _v[1] = 0.899068;
  _v[2] = 33.8977;
  _v[3] = -3.84314;
  _v[4] = 25.5561;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284550;
  _v[0] = 1.0; 
  _v[1] = -0.0291621;
  _v[2] = 37.94;
  _v[3] = -3.26022;
  _v[4] = 28.9294;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284553;
  _v[0] = 1.0; 
  _v[1] = -0.0291621;
  _v[2] = 37.94;
  _v[3] = -3.26022;
  _v[4] = 28.9294;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284594;
  _v[0] = 1.0; 
  _v[1] = 0.621394;
  _v[2] = 38.2993;
  _v[3] = -2.91312;
  _v[4] = 26.6439;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284597;
  _v[0] = 1.0; 
  _v[1] = 0.799092;
  _v[2] = 42.3323;
  _v[3] = -3.79961;
  _v[4] = 27.2266;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284614;
  _v[0] = 1.0; 
  _v[1] = 0.30704;
  _v[2] = 35.9853;
  _v[3] = -2.88854;
  _v[4] = 25.8775;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284621;
  _v[0] = 1.0; 
  _v[1] = 0.0196591;
  _v[2] = 40.156;
  _v[3] = -2.19981;
  _v[4] = 29.0597;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284622;
  _v[0] = 1.0; 
  _v[1] = -1.54014;
  _v[2] = 40.955;
  _v[3] = 1.70121;
  _v[4] = 28.2414;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284642;
  _v[0] = 1.0; 
  _v[1] = 0.538177;
  _v[2] = 35.6984;
  _v[3] = -3.33151;
  _v[4] = 24.3244;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284650;
  _v[0] = 1.0; 
  _v[1] = -0.424893;
  _v[2] = 38.9063;
  _v[3] = -1.32599;
  _v[4] = 27.0889;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284654;
  _v[0] = 1.0; 
  _v[1] = -0.424893;
  _v[2] = 38.9063;
  _v[3] = -1.32599;
  _v[4] = 27.0889;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284674;
  _v[0] = 1.0; 
  _v[1] = -1.45649;
  _v[2] = 30.9103;
  _v[3] = -0.94767;
  _v[4] = 25.3691;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284676;
  _v[0] = 1.0; 
  _v[1] = -0.589599;
  _v[2] = 34.4465;
  _v[3] = -1.29368;
  _v[4] = 28.0614;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284678;
  _v[0] = 1.0; 
  _v[1] = -0.143716;
  _v[2] = 37.3227;
  _v[3] = -2.64242;
  _v[4] = 27.7984;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284708;
  _v[0] = 1.0; 
  _v[1] = 0.245764;
  _v[2] = 30.1882;
  _v[3] = -2.55553;
  _v[4] = 25.9963;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284709;
  _v[0] = 1.0; 
  _v[1] = 0.310778;
  _v[2] = 35.9497;
  _v[3] = -2.69627;
  _v[4] = 26.0022;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284712;
  _v[0] = 1.0; 
  _v[1] = 0.98309;
  _v[2] = 39.1057;
  _v[3] = -4.05411;
  _v[4] = 27.4583;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284782;
  _v[0] = 1.0; 
  _v[1] = 0.307912;
  _v[2] = 31.2298;
  _v[3] = -3.19343;
  _v[4] = 26.2113;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284785;
  _v[0] = 1.0; 
  _v[1] = 1.91161;
  _v[2] = 37.5697;
  _v[3] = -4.91091;
  _v[4] = 28.7975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284786;
  _v[0] = 1.0; 
  _v[1] = 1.91161;
  _v[2] = 37.5697;
  _v[3] = -4.91091;
  _v[4] = 28.7975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284811;
  _v[0] = 1.0; 
  _v[1] = 0.256924;
  _v[2] = 33.9927;
  _v[3] = -3.66822;
  _v[4] = 23.4693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284813;
  _v[0] = 1.0; 
  _v[1] = 0.256924;
  _v[2] = 33.9927;
  _v[3] = -3.66822;
  _v[4] = 23.4693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284823;
  _v[0] = 1.0; 
  _v[1] = 0.49314;
  _v[2] = 38.6923;
  _v[3] = -4.24151;
  _v[4] = 27.8179;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284841;
  _v[0] = 1.0; 
  _v[1] = -0.783067;
  _v[2] = 38.1837;
  _v[3] = -1.3719;
  _v[4] = 24.9586;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284842;
  _v[0] = 1.0; 
  _v[1] = -0.783067;
  _v[2] = 38.1837;
  _v[3] = -1.3719;
  _v[4] = 24.9586;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284858;
  _v[0] = 1.0; 
  _v[1] = 0.167815;
  _v[2] = 36.6481;
  _v[3] = -2.76712;
  _v[4] = 26.0634;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284878;
  _v[0] = 1.0; 
  _v[1] = 0.0734078;
  _v[2] = 38.5606;
  _v[3] = -3.32961;
  _v[4] = 25.982;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284893;
  _v[0] = 1.0; 
  _v[1] = 0.453299;
  _v[2] = 32.2856;
  _v[3] = -3.72104;
  _v[4] = 25.0201;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284895;
  _v[0] = 1.0; 
  _v[1] = 0.14547;
  _v[2] = 40.039;
  _v[3] = -2.52821;
  _v[4] = 26.6199;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284897;
  _v[0] = 1.0; 
  _v[1] = 0.14547;
  _v[2] = 40.039;
  _v[3] = -2.52821;
  _v[4] = 26.6199;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284902;
  _v[0] = 1.0; 
  _v[1] = -1.18645;
  _v[2] = 38.7506;
  _v[3] = -0.368218;
  _v[4] = 24.9139;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284926;
  _v[0] = 1.0; 
  _v[1] = -0.868125;
  _v[2] = 36.0899;
  _v[3] = -1.57982;
  _v[4] = 25.393;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284930;
  _v[0] = 1.0; 
  _v[1] = 0.414173;
  _v[2] = 40.6595;
  _v[3] = -4.23606;
  _v[4] = 28.2656;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284944;
  _v[0] = 1.0; 
  _v[1] = 0.385734;
  _v[2] = 34.9918;
  _v[3] = -3.69752;
  _v[4] = 25.1668;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284987;
  _v[0] = 1.0; 
  _v[1] = 0.0801131;
  _v[2] = 33.1796;
  _v[3] = -2.40365;
  _v[4] = 27.548;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284989;
  _v[0] = 1.0; 
  _v[1] = 0.826304;
  _v[2] = 37.9232;
  _v[3] = -3.47456;
  _v[4] = 27.9889;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 284996;
  _v[0] = 1.0; 
  _v[1] = -1.74306;
  _v[2] = 38.7947;
  _v[3] = -1.1803;
  _v[4] = 24.8952;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285016;
  _v[0] = 1.0; 
  _v[1] = -0.4081;
  _v[2] = 38.1736;
  _v[3] = -2.91038;
  _v[4] = 27.0069;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285096;
  _v[0] = 1.0; 
  _v[1] = 0.786804;
  _v[2] = 37.2166;
  _v[3] = -3.60783;
  _v[4] = 29.8865;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285112;
  _v[0] = 1.0; 
  _v[1] = -0.622054;
  _v[2] = 36.9363;
  _v[3] = -1.49783;
  _v[4] = 29.6529;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285120;
  _v[0] = 1.0; 
  _v[1] = 1.94794;
  _v[2] = 34.1135;
  _v[3] = -5.04412;
  _v[4] = 27.1614;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285122;
  _v[0] = 1.0; 
  _v[1] = -0.00168456;
  _v[2] = 38.495;
  _v[3] = -2.94651;
  _v[4] = 27.8967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285125;
  _v[0] = 1.0; 
  _v[1] = -0.00168456;
  _v[2] = 38.495;
  _v[3] = -2.94651;
  _v[4] = 27.8967;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285142;
  _v[0] = 1.0; 
  _v[1] = -0.63538;
  _v[2] = 38.7122;
  _v[3] = -2.10667;
  _v[4] = 28.8626;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285163;
  _v[0] = 1.0; 
  _v[1] = -0.191636;
  _v[2] = 35.5432;
  _v[3] = -3.35291;
  _v[4] = 25.7664;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285180;
  _v[0] = 1.0; 
  _v[1] = 0.0486116;
  _v[2] = 36.6618;
  _v[3] = -3.25869;
  _v[4] = 26.5815;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285190;
  _v[0] = 1.0; 
  _v[1] = 0.0212509;
  _v[2] = 37.9;
  _v[3] = -3.02994;
  _v[4] = 27.7398;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285210;
  _v[0] = 1.0; 
  _v[1] = -2.03492;
  _v[2] = 33.1338;
  _v[3] = -1.598;
  _v[4] = 23.8148;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285220;
  _v[0] = 1.0; 
  _v[1] = -0.703158;
  _v[2] = 37.5712;
  _v[3] = -2.19517;
  _v[4] = 27.0471;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285222;
  _v[0] = 1.0; 
  _v[1] = -1.61118;
  _v[2] = 38.4022;
  _v[3] = -0.809523;
  _v[4] = 29.9935;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285223;
  _v[0] = 1.0; 
  _v[1] = -1.61118;
  _v[2] = 38.4022;
  _v[3] = -0.809523;
  _v[4] = 29.9935;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285235;
  _v[0] = 1.0; 
  _v[1] = -0.150689;
  _v[2] = 34.3181;
  _v[3] = -3.74385;
  _v[4] = 28.1687;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285236;
  _v[0] = 1.0; 
  _v[1] = -0.150689;
  _v[2] = 34.3181;
  _v[3] = -3.74385;
  _v[4] = 28.1687;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285271;
  _v[0] = 1.0; 
  _v[1] = -0.372375;
  _v[2] = 38.7379;
  _v[3] = -3.0298;
  _v[4] = 27.3179;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285272;
  _v[0] = 1.0; 
  _v[1] = -0.372375;
  _v[2] = 38.7379;
  _v[3] = -3.0298;
  _v[4] = 27.3179;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285284;
  _v[0] = 1.0; 
  _v[1] = -1.0496;
  _v[2] = 34.5897;
  _v[3] = -1.90025;
  _v[4] = 28.1041;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285285;
  _v[0] = 1.0; 
  _v[1] = -0.914999;
  _v[2] = 38.8754;
  _v[3] = -2.41541;
  _v[4] = 27.1439;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285298;
  _v[0] = 1.0; 
  _v[1] = -0.473069;
  _v[2] = 33.7809;
  _v[3] = -3.314;
  _v[4] = 25.7946;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285300;
  _v[0] = 1.0; 
  _v[1] = 0.922459;
  _v[2] = 38.8174;
  _v[3] = -5.2655;
  _v[4] = 26.9612;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285346;
  _v[0] = 1.0; 
  _v[1] = -0.468717;
  _v[2] = 35.7233;
  _v[3] = -2.58943;
  _v[4] = 27.907;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285351;
  _v[0] = 1.0; 
  _v[1] = 0.192222;
  _v[2] = 41.4638;
  _v[3] = -3.33058;
  _v[4] = 29.438;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285353;
  _v[0] = 1.0; 
  _v[1] = 0.192222;
  _v[2] = 41.4638;
  _v[3] = -3.33058;
  _v[4] = 29.438;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285366;
  _v[0] = 1.0; 
  _v[1] = -0.445616;
  _v[2] = 39.1405;
  _v[3] = -3.12051;
  _v[4] = 26.66;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285383;
  _v[0] = 1.0; 
  _v[1] = 0.0559612;
  _v[2] = 29.1891;
  _v[3] = -2.42783;
  _v[4] = 26.1788;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285384;
  _v[0] = 1.0; 
  _v[1] = 0.27513;
  _v[2] = 31.6858;
  _v[3] = -3.87628;
  _v[4] = 27.9746;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285416;
  _v[0] = 1.0; 
  _v[1] = -0.203121;
  _v[2] = 36.5091;
  _v[3] = -3.31812;
  _v[4] = 26.6393;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285441;
  _v[0] = 1.0; 
  _v[1] = 0.475959;
  _v[2] = 31.87;
  _v[3] = -4.45846;
  _v[4] = 26.0054;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285442;
  _v[0] = 1.0; 
  _v[1] = 0.610461;
  _v[2] = 35.6052;
  _v[3] = -4.54823;
  _v[4] = 28.5658;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285470;
  _v[0] = 1.0; 
  _v[1] = -0.614486;
  _v[2] = 35.6135;
  _v[3] = -2.82193;
  _v[4] = 27.4643;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285483;
  _v[0] = 1.0; 
  _v[1] = -0.884512;
  _v[2] = 38.0138;
  _v[3] = -2.13174;
  _v[4] = 27.6038;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285557;
  _v[0] = 1.0; 
  _v[1] = -0.0889784;
  _v[2] = 34.5398;
  _v[3] = -4.03265;
  _v[4] = 25.8397;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285569;
  _v[0] = 1.0; 
  _v[1] = 0.901551;
  _v[2] = 36.0877;
  _v[3] = -3.42048;
  _v[4] = 27.6316;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285584;
  _v[0] = 1.0; 
  _v[1] = 0.0490512;
  _v[2] = 36.212;
  _v[3] = -3.24265;
  _v[4] = 27.716;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285586;
  _v[0] = 1.0; 
  _v[1] = 0.0490512;
  _v[2] = 36.212;
  _v[3] = -3.24265;
  _v[4] = 27.716;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285697;
  _v[0] = 1.0; 
  _v[1] = 0.0592421;
  _v[2] = 38.7228;
  _v[3] = -3.44696;
  _v[4] = 26.7831;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285699;
  _v[0] = 1.0; 
  _v[1] = -0.623575;
  _v[2] = 42.6252;
  _v[3] = -2.11063;
  _v[4] = 28.8038;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285719;
  _v[0] = 1.0; 
  _v[1] = 1.56838;
  _v[2] = 44.2867;
  _v[3] = -6.02716;
  _v[4] = 30.0573;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285766;
  _v[0] = 1.0; 
  _v[1] = -0.475644;
  _v[2] = 35.2857;
  _v[3] = -2.07182;
  _v[4] = 27.9579;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285813;
  _v[0] = 1.0; 
  _v[1] = -0.0510601;
  _v[2] = 36.5097;
  _v[3] = -2.79182;
  _v[4] = 30.7384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285838;
  _v[0] = 1.0; 
  _v[1] = -0.111044;
  _v[2] = 35.7627;
  _v[3] = -3.63887;
  _v[4] = 27.9532;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285851;
  _v[0] = 1.0; 
  _v[1] = -1.13107;
  _v[2] = 37.7092;
  _v[3] = -1.56648;
  _v[4] = 27.2097;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285862;
  _v[0] = 1.0; 
  _v[1] = -0.424441;
  _v[2] = 36.587;
  _v[3] = -2.47178;
  _v[4] = 28.3375;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285896;
  _v[0] = 1.0; 
  _v[1] = -0.405308;
  _v[2] = 36.3043;
  _v[3] = -2.86465;
  _v[4] = 26.0781;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285945;
  _v[0] = 1.0; 
  _v[1] = 0.160083;
  _v[2] = 36.8576;
  _v[3] = -3.53416;
  _v[4] = 27.7063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285971;
  _v[0] = 1.0; 
  _v[1] = -1.88509;
  _v[2] = 33.4792;
  _v[3] = -0.494655;
  _v[4] = 25.8303;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 285988;
  _v[0] = 1.0; 
  _v[1] = -1.22674;
  _v[2] = 39.3899;
  _v[3] = -1.46385;
  _v[4] = 28.0125;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286041;
  _v[0] = 1.0; 
  _v[1] = -0.983566;
  _v[2] = 37.9477;
  _v[3] = -0.655641;
  _v[4] = 27.6703;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286042;
  _v[0] = 1.0; 
  _v[1] = -0.983566;
  _v[2] = 37.9477;
  _v[3] = -0.655641;
  _v[4] = 27.6703;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286057;
  _v[0] = 1.0; 
  _v[1] = -0.983566;
  _v[2] = 37.9477;
  _v[3] = -0.655641;
  _v[4] = 27.6703;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286065;
  _v[0] = 1.0; 
  _v[1] = -0.469017;
  _v[2] = 33.9813;
  _v[3] = -2.84601;
  _v[4] = 28.3379;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286072;
  _v[0] = 1.0; 
  _v[1] = -1.71174;
  _v[2] = 29.8176;
  _v[3] = -1.09653;
  _v[4] = 27.8441;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286087;
  _v[0] = 1.0; 
  _v[1] = -0.596767;
  _v[2] = 34.8134;
  _v[3] = -3.04474;
  _v[4] = 26.6899;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286105;
  _v[0] = 1.0; 
  _v[1] = 0.124857;
  _v[2] = 34.4474;
  _v[3] = -3.85286;
  _v[4] = 26.6166;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286144;
  _v[0] = 1.0; 
  _v[1] = -1.4196;
  _v[2] = 36.793;
  _v[3] = -1.30104;
  _v[4] = 27.3035;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286145;
  _v[0] = 1.0; 
  _v[1] = -1.63226;
  _v[2] = 35.376;
  _v[3] = -2.02478;
  _v[4] = 26.2269;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286150;
  _v[0] = 1.0; 
  _v[1] = 0.0317797;
  _v[2] = 39.4738;
  _v[3] = -4.3323;
  _v[4] = 29.3638;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286182;
  _v[0] = 1.0; 
  _v[1] = -0.768691;
  _v[2] = 34.7263;
  _v[3] = -2.02863;
  _v[4] = 27.3691;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286186;
  _v[0] = 1.0; 
  _v[1] = -0.0487463;
  _v[2] = 38.0815;
  _v[3] = -3.21191;
  _v[4] = 28.8336;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286198;
  _v[0] = 1.0; 
  _v[1] = 0.376808;
  _v[2] = 33.1901;
  _v[3] = -2.63278;
  _v[4] = 28.6133;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286207;
  _v[0] = 1.0; 
  _v[1] = 1.87609;
  _v[2] = 34.2909;
  _v[3] = -4.92825;
  _v[4] = 29.8161;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286217;
  _v[0] = 1.0; 
  _v[1] = -0.424465;
  _v[2] = 34.305;
  _v[3] = -3.49341;
  _v[4] = 27.6499;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286220;
  _v[0] = 1.0; 
  _v[1] = -0.416511;
  _v[2] = 40.3496;
  _v[3] = -2.95437;
  _v[4] = 27.9915;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286227;
  _v[0] = 1.0; 
  _v[1] = -0.713647;
  _v[2] = 37.5807;
  _v[3] = -2.65247;
  _v[4] = 27.2082;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286236;
  _v[0] = 1.0; 
  _v[1] = -0.685465;
  _v[2] = 37.4713;
  _v[3] = -3.51715;
  _v[4] = 24.3617;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286238;
  _v[0] = 1.0; 
  _v[1] = -0.685465;
  _v[2] = 37.4713;
  _v[3] = -3.51715;
  _v[4] = 24.3617;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286249;
  _v[0] = 1.0; 
  _v[1] = -0.207085;
  _v[2] = 38.5169;
  _v[3] = -3.48248;
  _v[4] = 26.6983;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286261;
  _v[0] = 1.0; 
  _v[1] = -0.684857;
  _v[2] = 36.7424;
  _v[3] = -2.73779;
  _v[4] = 25.792;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286263;
  _v[0] = 1.0; 
  _v[1] = -0.705529;
  _v[2] = 38.2015;
  _v[3] = -3.02282;
  _v[4] = 26.8492;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286278;
  _v[0] = 1.0; 
  _v[1] = -0.531914;
  _v[2] = 38.5886;
  _v[3] = -3.20134;
  _v[4] = 27.2285;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286291;
  _v[0] = 1.0; 
  _v[1] = -0.169827;
  _v[2] = 37.4385;
  _v[3] = -3.41273;
  _v[4] = 26.8798;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286331;
  _v[0] = 1.0; 
  _v[1] = -1.15567;
  _v[2] = 30.6457;
  _v[3] = -1.8635;
  _v[4] = 25.4326;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286364;
  _v[0] = 1.0; 
  _v[1] = -1.02856;
  _v[2] = 32.9521;
  _v[3] = -2.00753;
  _v[4] = 23.9532;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286367;
  _v[0] = 1.0; 
  _v[1] = 0.310824;
  _v[2] = 34.1892;
  _v[3] = -3.07285;
  _v[4] = 29.0954;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286368;
  _v[0] = 1.0; 
  _v[1] = -0.197956;
  _v[2] = 45.2822;
  _v[3] = -2.86336;
  _v[4] = 29.0815;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286371;
  _v[0] = 1.0; 
  _v[1] = -0.197956;
  _v[2] = 45.2822;
  _v[3] = -2.86336;
  _v[4] = 29.0815;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286397;
  _v[0] = 1.0; 
  _v[1] = -0.605133;
  _v[2] = 39.5592;
  _v[3] = -2.88388;
  _v[4] = 28.1903;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286409;
  _v[0] = 1.0; 
  _v[1] = -0.830332;
  _v[2] = 36.1763;
  _v[3] = -2.69984;
  _v[4] = 28.6201;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286443;
  _v[0] = 1.0; 
  _v[1] = -0.6522;
  _v[2] = 34.9901;
  _v[3] = -2.69872;
  _v[4] = 27.2659;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286450;
  _v[0] = 1.0; 
  _v[1] = -1.03491;
  _v[2] = 41.001;
  _v[3] = -2.53492;
  _v[4] = 29.1103;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286493;
  _v[0] = 1.0; 
  _v[1] = -1.4475;
  _v[2] = 30.8332;
  _v[3] = -3.02328;
  _v[4] = 25.5732;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286494;
  _v[0] = 1.0; 
  _v[1] = 0.167008;
  _v[2] = 39.0896;
  _v[3] = -3.91171;
  _v[4] = 29.5654;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286505;
  _v[0] = 1.0; 
  _v[1] = -1.65526;
  _v[2] = 33.4763;
  _v[3] = -2.03231;
  _v[4] = 26.2135;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286521;
  _v[0] = 1.0; 
  _v[1] = -0.204518;
  _v[2] = 37.5976;
  _v[3] = -4.0493;
  _v[4] = 27.1429;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286538;
  _v[0] = 1.0; 
  _v[1] = -1.34526;
  _v[2] = 37.0709;
  _v[3] = -2.15018;
  _v[4] = 27.2268;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286613;
  _v[0] = 1.0; 
  _v[1] = 0.489367;
  _v[2] = 34.8043;
  _v[3] = -3.42308;
  _v[4] = 23.9043;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286634;
  _v[0] = 1.0; 
  _v[1] = -1.03609;
  _v[2] = 35.9436;
  _v[3] = -3.07225;
  _v[4] = 28.041;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286648;
  _v[0] = 1.0; 
  _v[1] = -1.36342;
  _v[2] = 33.4355;
  _v[3] = -2.80536;
  _v[4] = 26.2027;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286649;
  _v[0] = 1.0; 
  _v[1] = -0.52612;
  _v[2] = 38.9845;
  _v[3] = -3.6177;
  _v[4] = 29.6292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286651;
  _v[0] = 1.0; 
  _v[1] = -0.52612;
  _v[2] = 38.9845;
  _v[3] = -3.6177;
  _v[4] = 29.6292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286652;
  _v[0] = 1.0; 
  _v[1] = -0.52612;
  _v[2] = 38.9845;
  _v[3] = -3.6177;
  _v[4] = 29.6292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286665;
  _v[0] = 1.0; 
  _v[1] = -0.278385;
  _v[2] = 36.4476;
  _v[3] = -3.36995;
  _v[4] = 27.0583;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286667;
  _v[0] = 1.0; 
  _v[1] = -1.91846;
  _v[2] = 35.1032;
  _v[3] = -2.02539;
  _v[4] = 23.9147;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286684;
  _v[0] = 1.0; 
  _v[1] = -0.209785;
  _v[2] = 37.5061;
  _v[3] = -3.74853;
  _v[4] = 26.6891;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286686;
  _v[0] = 1.0; 
  _v[1] = -0.364924;
  _v[2] = 36.4403;
  _v[3] = -3.49847;
  _v[4] = 27.3414;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286700;
  _v[0] = 1.0; 
  _v[1] = -0.415446;
  _v[2] = 32.9476;
  _v[3] = -3.3655;
  _v[4] = 26.9895;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286701;
  _v[0] = 1.0; 
  _v[1] = -1.45364;
  _v[2] = 38.8575;
  _v[3] = -2.13614;
  _v[4] = 27.2675;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286702;
  _v[0] = 1.0; 
  _v[1] = 1.15538;
  _v[2] = 38.104;
  _v[3] = -5.49776;
  _v[4] = 29.7876;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286724;
  _v[0] = 1.0; 
  _v[1] = -1.37179;
  _v[2] = 31.4417;
  _v[3] = -2.1815;
  _v[4] = 23.9415;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286725;
  _v[0] = 1.0; 
  _v[1] = -1.37179;
  _v[2] = 31.4417;
  _v[3] = -2.1815;
  _v[4] = 23.9415;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286728;
  _v[0] = 1.0; 
  _v[1] = -0.270087;
  _v[2] = 37.9344;
  _v[3] = -3.49537;
  _v[4] = 27.3026;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286732;
  _v[0] = 1.0; 
  _v[1] = 0.362666;
  _v[2] = 37.8637;
  _v[3] = -4.64462;
  _v[4] = 29.6107;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286746;
  _v[0] = 1.0; 
  _v[1] = -0.532095;
  _v[2] = 36.9114;
  _v[3] = -2.81611;
  _v[4] = 26.6324;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286755;
  _v[0] = 1.0; 
  _v[1] = -0.185351;
  _v[2] = 39.4802;
  _v[3] = -3.81124;
  _v[4] = 30.3068;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286777;
  _v[0] = 1.0; 
  _v[1] = -0.185351;
  _v[2] = 39.4802;
  _v[3] = -3.81124;
  _v[4] = 30.3068;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286791;
  _v[0] = 1.0; 
  _v[1] = -0.430199;
  _v[2] = 37.002;
  _v[3] = -3.49829;
  _v[4] = 27.2576;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286794;
  _v[0] = 1.0; 
  _v[1] = 0.419388;
  _v[2] = 41.261;
  _v[3] = -3.47907;
  _v[4] = 29.9263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286809;
  _v[0] = 1.0; 
  _v[1] = -1.85008;
  _v[2] = 42.0318;
  _v[3] = 0.295564;
  _v[4] = 29.4687;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286810;
  _v[0] = 1.0; 
  _v[1] = -0.861294;
  _v[2] = 37.6238;
  _v[3] = -2.82913;
  _v[4] = 26.5297;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286818;
  _v[0] = 1.0; 
  _v[1] = -0.390819;
  _v[2] = 37.2208;
  _v[3] = -3.42073;
  _v[4] = 27.0914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286822;
  _v[0] = 1.0; 
  _v[1] = -0.562925;
  _v[2] = 34.0155;
  _v[3] = -3.86351;
  _v[4] = 24.0884;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286825;
  _v[0] = 1.0; 
  _v[1] = -0.54327;
  _v[2] = 38.6731;
  _v[3] = -3.66181;
  _v[4] = 28.4799;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286839;
  _v[0] = 1.0; 
  _v[1] = -1.32921;
  _v[2] = 36.874;
  _v[3] = -2.59337;
  _v[4] = 26.9447;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286842;
  _v[0] = 1.0; 
  _v[1] = -3.09985;
  _v[2] = 33.0884;
  _v[3] = -0.710106;
  _v[4] = 27.4104;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286855;
  _v[0] = 1.0; 
  _v[1] = -1.29898;
  _v[2] = 34.6954;
  _v[3] = -2.21736;
  _v[4] = 25.5022;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286856;
  _v[0] = 1.0; 
  _v[1] = -1.60226;
  _v[2] = 38.7399;
  _v[3] = -1.91506;
  _v[4] = 28.1066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286857;
  _v[0] = 1.0; 
  _v[1] = -1.60226;
  _v[2] = 38.7399;
  _v[3] = -1.91506;
  _v[4] = 28.1066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286899;
  _v[0] = 1.0; 
  _v[1] = -1.3531;
  _v[2] = 36.3961;
  _v[3] = -1.94018;
  _v[4] = 24.4992;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286906;
  _v[0] = 1.0; 
  _v[1] = -1.3531;
  _v[2] = 36.3961;
  _v[3] = -1.94018;
  _v[4] = 24.4992;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286916;
  _v[0] = 1.0; 
  _v[1] = -0.714415;
  _v[2] = 39.9741;
  _v[3] = -2.58318;
  _v[4] = 27.2837;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286935;
  _v[0] = 1.0; 
  _v[1] = -0.969954;
  _v[2] = 37.4028;
  _v[3] = -2.32252;
  _v[4] = 26.33;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 286986;
  _v[0] = 1.0; 
  _v[1] = -0.00517267;
  _v[2] = 33.0516;
  _v[3] = -4.05791;
  _v[4] = 25.8663;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287019;
  _v[0] = 1.0; 
  _v[1] = -1.49611;
  _v[2] = 37.1253;
  _v[3] = -1.05498;
  _v[4] = 28.3616;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287022;
  _v[0] = 1.0; 
  _v[1] = 0.190709;
  _v[2] = 34.243;
  _v[3] = -4.73745;
  _v[4] = 25.5413;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287077;
  _v[0] = 1.0; 
  _v[1] = -0.817092;
  _v[2] = 34.0909;
  _v[3] = -2.19644;
  _v[4] = 26.9343;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287078;
  _v[0] = 1.0; 
  _v[1] = -0.347649;
  _v[2] = 38.2484;
  _v[3] = -2.7522;
  _v[4] = 28.0726;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287095;
  _v[0] = 1.0; 
  _v[1] = 0.276373;
  _v[2] = 29.2267;
  _v[3] = -4.43312;
  _v[4] = 24.9783;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287096;
  _v[0] = 1.0; 
  _v[1] = -0.233237;
  _v[2] = 36.4567;
  _v[3] = -3.71245;
  _v[4] = 27.223;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287104;
  _v[0] = 1.0; 
  _v[1] = 1.18962;
  _v[2] = 36.269;
  _v[3] = -4.4838;
  _v[4] = 30.9431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287110;
  _v[0] = 1.0; 
  _v[1] = 1.18962;
  _v[2] = 36.269;
  _v[3] = -4.4838;
  _v[4] = 30.9431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287126;
  _v[0] = 1.0; 
  _v[1] = -0.643637;
  _v[2] = 36.427;
  _v[3] = -3.13439;
  _v[4] = 26.7013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287141;
  _v[0] = 1.0; 
  _v[1] = -1.16987;
  _v[2] = 35.5222;
  _v[3] = -2.36317;
  _v[4] = 26.8624;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287187;
  _v[0] = 1.0; 
  _v[1] = 0.0677114;
  _v[2] = 37.9525;
  _v[3] = -2.90737;
  _v[4] = 29.133;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287190;
  _v[0] = 1.0; 
  _v[1] = 0.0677114;
  _v[2] = 37.9525;
  _v[3] = -2.90737;
  _v[4] = 29.133;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287230;
  _v[0] = 1.0; 
  _v[1] = -0.476724;
  _v[2] = 35.8507;
  _v[3] = -2.94882;
  _v[4] = 26.5278;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287254;
  _v[0] = 1.0; 
  _v[1] = -1.67742;
  _v[2] = 33.9384;
  _v[3] = -2.34206;
  _v[4] = 25.6615;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287257;
  _v[0] = 1.0; 
  _v[1] = -1.67742;
  _v[2] = 33.9384;
  _v[3] = -2.34206;
  _v[4] = 25.6615;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287260;
  _v[0] = 1.0; 
  _v[1] = -0.977549;
  _v[2] = 43.7967;
  _v[3] = -0.616142;
  _v[4] = 29.1825;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287261;
  _v[0] = 1.0; 
  _v[1] = -0.977549;
  _v[2] = 43.7967;
  _v[3] = -0.616142;
  _v[4] = 29.1825;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287295;
  _v[0] = 1.0; 
  _v[1] = -0.93594;
  _v[2] = 33.1987;
  _v[3] = -3.2107;
  _v[4] = 25.536;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287299;
  _v[0] = 1.0; 
  _v[1] = -0.641648;
  _v[2] = 39.1364;
  _v[3] = -2.61164;
  _v[4] = 30.4552;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287300;
  _v[0] = 1.0; 
  _v[1] = -0.641648;
  _v[2] = 39.1364;
  _v[3] = -2.61164;
  _v[4] = 30.4552;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287320;
  _v[0] = 1.0; 
  _v[1] = -0.120561;
  _v[2] = 38.1331;
  _v[3] = -2.87636;
  _v[4] = 27.6028;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287321;
  _v[0] = 1.0; 
  _v[1] = -0.120561;
  _v[2] = 38.1331;
  _v[3] = -2.87636;
  _v[4] = 27.6028;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287357;
  _v[0] = 1.0; 
  _v[1] = 0.209724;
  _v[2] = 29.6187;
  _v[3] = -2.44943;
  _v[4] = 26.4389;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287361;
  _v[0] = 1.0; 
  _v[1] = 0.209724;
  _v[2] = 29.6187;
  _v[3] = -2.44943;
  _v[4] = 26.4389;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287364;
  _v[0] = 1.0; 
  _v[1] = 0.209724;
  _v[2] = 29.6187;
  _v[3] = -2.44943;
  _v[4] = 26.4389;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287366;
  _v[0] = 1.0; 
  _v[1] = -0.333614;
  _v[2] = 36.9115;
  _v[3] = -2.79084;
  _v[4] = 28.2391;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287368;
  _v[0] = 1.0; 
  _v[1] = -0.333614;
  _v[2] = 36.9115;
  _v[3] = -2.79084;
  _v[4] = 28.2391;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287373;
  _v[0] = 1.0; 
  _v[1] = -0.379132;
  _v[2] = 36.5695;
  _v[3] = -2.41763;
  _v[4] = 26.4569;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287374;
  _v[0] = 1.0; 
  _v[1] = 0.249233;
  _v[2] = 40.0687;
  _v[3] = -2.04054;
  _v[4] = 31.2881;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287389;
  _v[0] = 1.0; 
  _v[1] = 0.385781;
  _v[2] = 35.7568;
  _v[3] = -3.74811;
  _v[4] = 26.0692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287393;
  _v[0] = 1.0; 
  _v[1] = 0.764315;
  _v[2] = 43.2255;
  _v[3] = -5.0474;
  _v[4] = 28.3086;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287396;
  _v[0] = 1.0; 
  _v[1] = 0.764315;
  _v[2] = 43.2255;
  _v[3] = -5.0474;
  _v[4] = 28.3086;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287416;
  _v[0] = 1.0; 
  _v[1] = -0.422791;
  _v[2] = 36.6161;
  _v[3] = -3.01843;
  _v[4] = 26.4317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287471;
  _v[0] = 1.0; 
  _v[1] = -1.21239;
  _v[2] = 28.6318;
  _v[3] = -2.422;
  _v[4] = 27.3932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287473;
  _v[0] = 1.0; 
  _v[1] = -1.21239;
  _v[2] = 28.6318;
  _v[3] = -2.422;
  _v[4] = 27.3932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287474;
  _v[0] = 1.0; 
  _v[1] = -1.21239;
  _v[2] = 28.6318;
  _v[3] = -2.422;
  _v[4] = 27.3932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287475;
  _v[0] = 1.0; 
  _v[1] = -1.21239;
  _v[2] = 28.6318;
  _v[3] = -2.422;
  _v[4] = 27.3932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287476;
  _v[0] = 1.0; 
  _v[1] = 0.157427;
  _v[2] = 39.5894;
  _v[3] = -2.87934;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287478;
  _v[0] = 1.0; 
  _v[1] = 0.157427;
  _v[2] = 39.5894;
  _v[3] = -2.87934;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287479;
  _v[0] = 1.0; 
  _v[1] = 0.157427;
  _v[2] = 39.5894;
  _v[3] = -2.87934;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287482;
  _v[0] = 1.0; 
  _v[1] = 0.157427;
  _v[2] = 39.5894;
  _v[3] = -2.87934;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287485;
  _v[0] = 1.0; 
  _v[1] = 0.157427;
  _v[2] = 39.5894;
  _v[3] = -2.87934;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287488;
  _v[0] = 1.0; 
  _v[1] = 4.58295;
  _v[2] = -67.6066;
  _v[3] = -33.5254;
  _v[4] = -21.18;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287513;
  _v[0] = 1.0; 
  _v[1] = -0.909988;
  _v[2] = 32.3942;
  _v[3] = -2.48254;
  _v[4] = 26.2099;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287516;
  _v[0] = 1.0; 
  _v[1] = 0.545174;
  _v[2] = 38.8369;
  _v[3] = -3.42578;
  _v[4] = 28.4806;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287519;
  _v[0] = 1.0; 
  _v[1] = 0.239495;
  _v[2] = 41.4528;
  _v[3] = -2.12393;
  _v[4] = 30.1248;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287534;
  _v[0] = 1.0; 
  _v[1] = 0.797025;
  _v[2] = 36.7979;
  _v[3] = -3.5919;
  _v[4] = 26.882;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287561;
  _v[0] = 1.0; 
  _v[1] = -0.44779;
  _v[2] = 35.7317;
  _v[3] = -2.51829;
  _v[4] = 26.2085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287587;
  _v[0] = 1.0; 
  _v[1] = -0.308283;
  _v[2] = 29.5813;
  _v[3] = -2.51812;
  _v[4] = 25.4222;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287594;
  _v[0] = 1.0; 
  _v[1] = -0.128704;
  _v[2] = 35.6496;
  _v[3] = -3.23353;
  _v[4] = 27.8488;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287595;
  _v[0] = 1.0; 
  _v[1] = -0.128704;
  _v[2] = 35.6496;
  _v[3] = -3.23353;
  _v[4] = 27.8488;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287596;
  _v[0] = 1.0; 
  _v[1] = -0.382147;
  _v[2] = 42.5698;
  _v[3] = -2.18982;
  _v[4] = 28.13;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287599;
  _v[0] = 1.0; 
  _v[1] = -0.186067;
  _v[2] = 47.874;
  _v[3] = -3.22258;
  _v[4] = 27.1703;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287608;
  _v[0] = 1.0; 
  _v[1] = 2.0375;
  _v[2] = 30.2963;
  _v[3] = -5.33762;
  _v[4] = 26.0159;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287609;
  _v[0] = 1.0; 
  _v[1] = 2.0375;
  _v[2] = 30.2963;
  _v[3] = -5.33762;
  _v[4] = 26.0159;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287612;
  _v[0] = 1.0; 
  _v[1] = 0.586199;
  _v[2] = 38.6919;
  _v[3] = -2.93615;
  _v[4] = 28.1537;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287629;
  _v[0] = 1.0; 
  _v[1] = 0.39158;
  _v[2] = 36.4194;
  _v[3] = -3.53995;
  _v[4] = 26.3189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287630;
  _v[0] = 1.0; 
  _v[1] = 0.39158;
  _v[2] = 36.4194;
  _v[3] = -3.53995;
  _v[4] = 26.3189;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287631;
  _v[0] = 1.0; 
  _v[1] = 1.15043;
  _v[2] = 45.8353;
  _v[3] = -4.7667;
  _v[4] = 26.2865;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287648;
  _v[0] = 1.0; 
  _v[1] = 0.709304;
  _v[2] = 34.3923;
  _v[3] = -2.80886;
  _v[4] = 25.5875;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287649;
  _v[0] = 1.0; 
  _v[1] = 0.709304;
  _v[2] = 34.3923;
  _v[3] = -2.80886;
  _v[4] = 25.5875;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287650;
  _v[0] = 1.0; 
  _v[1] = 0.709304;
  _v[2] = 34.3923;
  _v[3] = -2.80886;
  _v[4] = 25.5875;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287651;
  _v[0] = 1.0; 
  _v[1] = 0.709304;
  _v[2] = 34.3923;
  _v[3] = -2.80886;
  _v[4] = 25.5875;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287653;
  _v[0] = 1.0; 
  _v[1] = -0.00933587;
  _v[2] = 39.7091;
  _v[3] = -2.61003;
  _v[4] = 28.4361;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287679;
  _v[0] = 1.0; 
  _v[1] = -0.966131;
  _v[2] = 30.0455;
  _v[3] = -2.90316;
  _v[4] = 24.9425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287680;
  _v[0] = 1.0; 
  _v[1] = 0.287954;
  _v[2] = 37.1551;
  _v[3] = -3.73377;
  _v[4] = 28.1271;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287715;
  _v[0] = 1.0; 
  _v[1] = 0.0531287;
  _v[2] = 38.863;
  _v[3] = -2.9763;
  _v[4] = 27.1213;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287767;
  _v[0] = 1.0; 
  _v[1] = -0.399653;
  _v[2] = 36.2874;
  _v[3] = -2.73421;
  _v[4] = 25.9098;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287789;
  _v[0] = 1.0; 
  _v[1] = 0.0880051;
  _v[2] = 32.6099;
  _v[3] = -3.87972;
  _v[4] = 25.8969;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287885;
  _v[0] = 1.0; 
  _v[1] = -0.0834589;
  _v[2] = 35.0236;
  _v[3] = -3.68608;
  _v[4] = 26.2832;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287918;
  _v[0] = 1.0; 
  _v[1] = -0.0244659;
  _v[2] = 37.2767;
  _v[3] = -3.46266;
  _v[4] = 26.7769;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287924;
  _v[0] = 1.0; 
  _v[1] = -0.172521;
  _v[2] = 34.0862;
  _v[3] = -3.78806;
  _v[4] = 26.1573;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287925;
  _v[0] = 1.0; 
  _v[1] = -0.137466;
  _v[2] = 39.9957;
  _v[3] = -2.90952;
  _v[4] = 28.0624;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287926;
  _v[0] = 1.0; 
  _v[1] = -0.137466;
  _v[2] = 39.9957;
  _v[3] = -2.90952;
  _v[4] = 28.0624;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287938;
  _v[0] = 1.0; 
  _v[1] = -0.0655614;
  _v[2] = 32.2986;
  _v[3] = -3.74378;
  _v[4] = 25.0263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287944;
  _v[0] = 1.0; 
  _v[1] = -0.706066;
  _v[2] = 33.3533;
  _v[3] = -0.814124;
  _v[4] = 28.4414;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287945;
  _v[0] = 1.0; 
  _v[1] = -0.319649;
  _v[2] = 37.2867;
  _v[3] = -3.28425;
  _v[4] = 28.1563;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287948;
  _v[0] = 1.0; 
  _v[1] = -0.319649;
  _v[2] = 37.2867;
  _v[3] = -3.28425;
  _v[4] = 28.1563;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287955;
  _v[0] = 1.0; 
  _v[1] = 0.152448;
  _v[2] = 41.2506;
  _v[3] = -4.59303;
  _v[4] = 28.2136;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287958;
  _v[0] = 1.0; 
  _v[1] = 0.152448;
  _v[2] = 41.2506;
  _v[3] = -4.59303;
  _v[4] = 28.2136;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287977;
  _v[0] = 1.0; 
  _v[1] = 0.600096;
  _v[2] = 34.2007;
  _v[3] = -3.52776;
  _v[4] = 28.187;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 287978;
  _v[0] = 1.0; 
  _v[1] = 0.600096;
  _v[2] = 34.2007;
  _v[3] = -3.52776;
  _v[4] = 28.187;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288002;
  _v[0] = 1.0; 
  _v[1] = 0.00703154;
  _v[2] = 41.8556;
  _v[3] = -3.10734;
  _v[4] = 29.8878;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288268;
  _v[0] = 1.0; 
  _v[1] = 0.54473;
  _v[2] = 33.6449;
  _v[3] = -3.82239;
  _v[4] = 25.7693;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288356;
  _v[0] = 1.0; 
  _v[1] = 0.166962;
  _v[2] = 34.586;
  _v[3] = -3.12096;
  _v[4] = 26.4256;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288403;
  _v[0] = 1.0; 
  _v[1] = -0.0953057;
  _v[2] = 36.1703;
  _v[3] = -2.97099;
  _v[4] = 26.6135;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288442;
  _v[0] = 1.0; 
  _v[1] = 0.622485;
  _v[2] = 33.5756;
  _v[3] = -3.658;
  _v[4] = 26.4777;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288443;
  _v[0] = 1.0; 
  _v[1] = 0.153136;
  _v[2] = 38.6746;
  _v[3] = -2.50179;
  _v[4] = 28.3333;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288454;
  _v[0] = 1.0; 
  _v[1] = 0.224575;
  _v[2] = 28.9796;
  _v[3] = -3.13585;
  _v[4] = 25.9227;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288455;
  _v[0] = 1.0; 
  _v[1] = 0.137371;
  _v[2] = 37.3719;
  _v[3] = -2.87933;
  _v[4] = 26.9536;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288456;
  _v[0] = 1.0; 
  _v[1] = 0.137371;
  _v[2] = 37.3719;
  _v[3] = -2.87933;
  _v[4] = 26.9536;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288469;
  _v[0] = 1.0; 
  _v[1] = -2.06829;
  _v[2] = 29.4296;
  _v[3] = -1.19787;
  _v[4] = 26.2461;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288470;
  _v[0] = 1.0; 
  _v[1] = 0.469128;
  _v[2] = 38.5644;
  _v[3] = -4.06874;
  _v[4] = 26.7104;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288471;
  _v[0] = 1.0; 
  _v[1] = 0.469128;
  _v[2] = 38.5644;
  _v[3] = -4.06874;
  _v[4] = 26.7104;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288485;
  _v[0] = 1.0; 
  _v[1] = -0.54038;
  _v[2] = 36.037;
  _v[3] = -2.52156;
  _v[4] = 25.6876;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288496;
  _v[0] = 1.0; 
  _v[1] = -0.0136489;
  _v[2] = 31.4812;
  _v[3] = -3.79252;
  _v[4] = 26.5247;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288497;
  _v[0] = 1.0; 
  _v[1] = -0.0136489;
  _v[2] = 31.4812;
  _v[3] = -3.79252;
  _v[4] = 26.5247;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288498;
  _v[0] = 1.0; 
  _v[1] = -0.360306;
  _v[2] = 37.959;
  _v[3] = -2.61474;
  _v[4] = 28.0511;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288499;
  _v[0] = 1.0; 
  _v[1] = -0.360306;
  _v[2] = 37.959;
  _v[3] = -2.61474;
  _v[4] = 28.0511;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288519;
  _v[0] = 1.0; 
  _v[1] = -1.93518;
  _v[2] = 30.4035;
  _v[3] = -2.13499;
  _v[4] = 23.4886;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288522;
  _v[0] = 1.0; 
  _v[1] = -1.93518;
  _v[2] = 30.4035;
  _v[3] = -2.13499;
  _v[4] = 23.4886;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288523;
  _v[0] = 1.0; 
  _v[1] = 0.0534321;
  _v[2] = 36.6007;
  _v[3] = -3.16523;
  _v[4] = 25.8918;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288525;
  _v[0] = 1.0; 
  _v[1] = 0.0534321;
  _v[2] = 36.6007;
  _v[3] = -3.16523;
  _v[4] = 25.8918;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288558;
  _v[0] = 1.0; 
  _v[1] = -0.60787;
  _v[2] = 33.4789;
  _v[3] = -1.71309;
  _v[4] = 26.2436;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288569;
  _v[0] = 1.0; 
  _v[1] = 0.315305;
  _v[2] = 31.4149;
  _v[3] = -3.12443;
  _v[4] = 24.7618;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288584;
  _v[0] = 1.0; 
  _v[1] = 0.490974;
  _v[2] = 40.8986;
  _v[3] = -3.1276;
  _v[4] = 26.3726;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288585;
  _v[0] = 1.0; 
  _v[1] = 0.490974;
  _v[2] = 40.8986;
  _v[3] = -3.1276;
  _v[4] = 26.3726;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288587;
  _v[0] = 1.0; 
  _v[1] = -0.981974;
  _v[2] = 42.4242;
  _v[3] = -0.891488;
  _v[4] = 29.0612;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288588;
  _v[0] = 1.0; 
  _v[1] = -0.981974;
  _v[2] = 42.4242;
  _v[3] = -0.891488;
  _v[4] = 29.0612;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288589;
  _v[0] = 1.0; 
  _v[1] = -0.981974;
  _v[2] = 42.4242;
  _v[3] = -0.891488;
  _v[4] = 29.0612;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288593;
  _v[0] = 1.0; 
  _v[1] = -0.0778899;
  _v[2] = 44.1311;
  _v[3] = -2.69536;
  _v[4] = 29.5092;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288599;
  _v[0] = 1.0; 
  _v[1] = -0.0778899;
  _v[2] = 44.1311;
  _v[3] = -2.69536;
  _v[4] = 29.5092;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288693;
  _v[0] = 1.0; 
  _v[1] = 0.199151;
  _v[2] = 36.1164;
  _v[3] = -4.11021;
  _v[4] = 25.0994;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288698;
  _v[0] = 1.0; 
  _v[1] = 0.131661;
  _v[2] = 38.4311;
  _v[3] = -2.73722;
  _v[4] = 28.67;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288699;
  _v[0] = 1.0; 
  _v[1] = 0.131661;
  _v[2] = 38.4311;
  _v[3] = -2.73722;
  _v[4] = 28.67;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288721;
  _v[0] = 1.0; 
  _v[1] = -0.456464;
  _v[2] = 32.6742;
  _v[3] = -2.27606;
  _v[4] = 27.4332;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288745;
  _v[0] = 1.0; 
  _v[1] = -0.143168;
  _v[2] = 34.7941;
  _v[3] = -3.33157;
  _v[4] = 25.449;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288762;
  _v[0] = 1.0; 
  _v[1] = 0.445833;
  _v[2] = 34.049;
  _v[3] = -2.9871;
  _v[4] = 27.1878;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288815;
  _v[0] = 1.0; 
  _v[1] = 0.650558;
  _v[2] = 30.2968;
  _v[3] = -2.85988;
  _v[4] = 26.7613;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288817;
  _v[0] = 1.0; 
  _v[1] = 0.650558;
  _v[2] = 30.2968;
  _v[3] = -2.85988;
  _v[4] = 26.7613;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288820;
  _v[0] = 1.0; 
  _v[1] = 0.650558;
  _v[2] = 30.2968;
  _v[3] = -2.85988;
  _v[4] = 26.7613;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288822;
  _v[0] = 1.0; 
  _v[1] = -0.39108;
  _v[2] = 35.8969;
  _v[3] = -1.22872;
  _v[4] = 27.1232;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288823;
  _v[0] = 1.0; 
  _v[1] = -0.39108;
  _v[2] = 35.8969;
  _v[3] = -1.22872;
  _v[4] = 27.1232;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288825;
  _v[0] = 1.0; 
  _v[1] = -0.412592;
  _v[2] = 38.4248;
  _v[3] = -3.53135;
  _v[4] = 30.1389;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288845;
  _v[0] = 1.0; 
  _v[1] = -0.355038;
  _v[2] = 33.94;
  _v[3] = -2.61023;
  _v[4] = 25.1228;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288846;
  _v[0] = 1.0; 
  _v[1] = -0.689821;
  _v[2] = 39.9221;
  _v[3] = -2.37389;
  _v[4] = 27.8261;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288855;
  _v[0] = 1.0; 
  _v[1] = -1.10337;
  _v[2] = 35.9054;
  _v[3] = -1.88871;
  _v[4] = 26.2903;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288856;
  _v[0] = 1.0; 
  _v[1] = 0.369617;
  _v[2] = 46.9761;
  _v[3] = -3.69368;
  _v[4] = 27.085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288857;
  _v[0] = 1.0; 
  _v[1] = 0.369617;
  _v[2] = 46.9761;
  _v[3] = -3.69368;
  _v[4] = 27.085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288874;
  _v[0] = 1.0; 
  _v[1] = -1.03443;
  _v[2] = 35.1046;
  _v[3] = -2.08714;
  _v[4] = 25.8323;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288875;
  _v[0] = 1.0; 
  _v[1] = -0.180129;
  _v[2] = 38.6572;
  _v[3] = -1.57974;
  _v[4] = 28.8009;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288876;
  _v[0] = 1.0; 
  _v[1] = -1.89267;
  _v[2] = 40.6704;
  _v[3] = 0.213801;
  _v[4] = 29.6808;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288877;
  _v[0] = 1.0; 
  _v[1] = -1.89267;
  _v[2] = 40.6704;
  _v[3] = 0.213801;
  _v[4] = 29.6808;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288886;
  _v[0] = 1.0; 
  _v[1] = 0.312722;
  _v[2] = 35.1345;
  _v[3] = -4.1012;
  _v[4] = 26.2505;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288896;
  _v[0] = 1.0; 
  _v[1] = -0.875051;
  _v[2] = 42.6608;
  _v[3] = 0.81844;
  _v[4] = 28.6304;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288898;
  _v[0] = 1.0; 
  _v[1] = -0.875051;
  _v[2] = 42.6608;
  _v[3] = 0.81844;
  _v[4] = 28.6304;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288900;
  _v[0] = 1.0; 
  _v[1] = -0.875051;
  _v[2] = 42.6608;
  _v[3] = 0.81844;
  _v[4] = 28.6304;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288901;
  _v[0] = 1.0; 
  _v[1] = -0.875051;
  _v[2] = 42.6608;
  _v[3] = 0.81844;
  _v[4] = 28.6304;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288927;
  _v[0] = 1.0; 
  _v[1] = -0.806562;
  _v[2] = 31.6078;
  _v[3] = -2.73597;
  _v[4] = 24.4674;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288933;
  _v[0] = 1.0; 
  _v[1] = -0.718823;
  _v[2] = 36.1614;
  _v[3] = -2.61772;
  _v[4] = 28.2711;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288958;
  _v[0] = 1.0; 
  _v[1] = 0.0514763;
  _v[2] = 42.6259;
  _v[3] = -4.00091;
  _v[4] = 26.1593;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 288969;
  _v[0] = 1.0; 
  _v[1] = -0.795876;
  _v[2] = 36.3582;
  _v[3] = -2.56598;
  _v[4] = 26.3427;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289018;
  _v[0] = 1.0; 
  _v[1] = -0.935668;
  _v[2] = 32.1653;
  _v[3] = -2.64408;
  _v[4] = 23.6851;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289019;
  _v[0] = 1.0; 
  _v[1] = -0.935668;
  _v[2] = 32.1653;
  _v[3] = -2.64408;
  _v[4] = 23.6851;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289027;
  _v[0] = 1.0; 
  _v[1] = -1.96888;
  _v[2] = 37.0181;
  _v[3] = -0.204514;
  _v[4] = 27.8033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289028;
  _v[0] = 1.0; 
  _v[1] = -1.96888;
  _v[2] = 37.0181;
  _v[3] = -0.204514;
  _v[4] = 27.8033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289040;
  _v[0] = 1.0; 
  _v[1] = -1.96888;
  _v[2] = 37.0181;
  _v[3] = -0.204514;
  _v[4] = 27.8033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289043;
  _v[0] = 1.0; 
  _v[1] = -1.96888;
  _v[2] = 37.0181;
  _v[3] = -0.204514;
  _v[4] = 27.8033;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289057;
  _v[0] = 1.0; 
  _v[1] = 0.0064266;
  _v[2] = 32.2693;
  _v[3] = -3.51561;
  _v[4] = 26.3985;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289059;
  _v[0] = 1.0; 
  _v[1] = 1.12864;
  _v[2] = 38.2696;
  _v[3] = -5.20272;
  _v[4] = 28.6407;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289061;
  _v[0] = 1.0; 
  _v[1] = 1.12864;
  _v[2] = 38.2696;
  _v[3] = -5.20272;
  _v[4] = 28.6407;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289083;
  _v[0] = 1.0; 
  _v[1] = 0.987493;
  _v[2] = 29.5596;
  _v[3] = -4.36312;
  _v[4] = 27.9096;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289084;
  _v[0] = 1.0; 
  _v[1] = 0.429902;
  _v[2] = 36.7656;
  _v[3] = -4.16751;
  _v[4] = 27.1593;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289086;
  _v[0] = 1.0; 
  _v[1] = -0.343036;
  _v[2] = -63.3212;
  _v[3] = -1.52858;
  _v[4] = -20.2102;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289107;
  _v[0] = 1.0; 
  _v[1] = -0.311375;
  _v[2] = 30.9874;
  _v[3] = -3.1004;
  _v[4] = 25.49;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289130;
  _v[0] = 1.0; 
  _v[1] = 0.173672;
  _v[2] = 36.5603;
  _v[3] = -3.47901;
  _v[4] = 27.6096;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289134;
  _v[0] = 1.0; 
  _v[1] = -0.428724;
  _v[2] = 41.5946;
  _v[3] = -3.34647;
  _v[4] = 29.2451;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289135;
  _v[0] = 1.0; 
  _v[1] = -3.77548;
  _v[2] = 39.9203;
  _v[3] = 3.60359;
  _v[4] = 30.1232;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289150;
  _v[0] = 1.0; 
  _v[1] = 0.0172759;
  _v[2] = 36.8437;
  _v[3] = -4.01803;
  _v[4] = 27.1635;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289151;
  _v[0] = 1.0; 
  _v[1] = 0.0172759;
  _v[2] = 36.8437;
  _v[3] = -4.01803;
  _v[4] = 27.1635;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289161;
  _v[0] = 1.0; 
  _v[1] = 0.173518;
  _v[2] = 36.9102;
  _v[3] = -4.45195;
  _v[4] = 25.7503;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289162;
  _v[0] = 1.0; 
  _v[1] = 0.0216457;
  _v[2] = 43.4706;
  _v[3] = -2.38214;
  _v[4] = 28.5258;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289163;
  _v[0] = 1.0; 
  _v[1] = 0.0216457;
  _v[2] = 43.4706;
  _v[3] = -2.38214;
  _v[4] = 28.5258;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289181;
  _v[0] = 1.0; 
  _v[1] = -1.66047;
  _v[2] = 30.4956;
  _v[3] = -1.83513;
  _v[4] = 25.7179;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289182;
  _v[0] = 1.0; 
  _v[1] = -0.334949;
  _v[2] = 39.0906;
  _v[3] = -3.04248;
  _v[4] = 27.2095;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289183;
  _v[0] = 1.0; 
  _v[1] = -1.09362;
  _v[2] = 41.4615;
  _v[3] = -1.37428;
  _v[4] = 30.808;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289197;
  _v[0] = 1.0; 
  _v[1] = -0.196913;
  _v[2] = 37.3332;
  _v[3] = -3.32212;
  _v[4] = 26.5323;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289273;
  _v[0] = 1.0; 
  _v[1] = -1.03855;
  _v[2] = 37.9188;
  _v[3] = -2.45507;
  _v[4] = 27.3428;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289274;
  _v[0] = 1.0; 
  _v[1] = -1.03855;
  _v[2] = 37.9188;
  _v[3] = -2.45507;
  _v[4] = 27.3428;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289293;
  _v[0] = 1.0; 
  _v[1] = -1.03855;
  _v[2] = 37.9188;
  _v[3] = -2.45507;
  _v[4] = 27.3428;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289331;
  _v[0] = 1.0; 
  _v[1] = -0.264222;
  _v[2] = 37.5493;
  _v[3] = -3.46879;
  _v[4] = 26.1821;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289345;
  _v[0] = 1.0; 
  _v[1] = -0.834257;
  _v[2] = 29.625;
  _v[3] = -2.96936;
  _v[4] = 24.5038;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289346;
  _v[0] = 1.0; 
  _v[1] = -0.299234;
  _v[2] = 35.8224;
  _v[3] = -3.00609;
  _v[4] = 26.3063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289347;
  _v[0] = 1.0; 
  _v[1] = -0.299234;
  _v[2] = 35.8224;
  _v[3] = -3.00609;
  _v[4] = 26.3063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289348;
  _v[0] = 1.0; 
  _v[1] = -0.299234;
  _v[2] = 35.8224;
  _v[3] = -3.00609;
  _v[4] = 26.3063;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289349;
  _v[0] = 1.0; 
  _v[1] = -0.384064;
  _v[2] = 38.5596;
  _v[3] = -2.97518;
  _v[4] = 31.2941;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289365;
  _v[0] = 1.0; 
  _v[1] = -1.82508;
  _v[2] = 36.783;
  _v[3] = -0.076601;
  _v[4] = 26.5765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289366;
  _v[0] = 1.0; 
  _v[1] = -1.82508;
  _v[2] = 36.783;
  _v[3] = -0.076601;
  _v[4] = 26.5765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289367;
  _v[0] = 1.0; 
  _v[1] = -0.117812;
  _v[2] = 39.4141;
  _v[3] = -3.19453;
  _v[4] = 29.0053;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289368;
  _v[0] = 1.0; 
  _v[1] = -0.117812;
  _v[2] = 39.4141;
  _v[3] = -3.19453;
  _v[4] = 29.0053;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289381;
  _v[0] = 1.0; 
  _v[1] = 0.727757;
  _v[2] = 31.1044;
  _v[3] = -5.38549;
  _v[4] = 24.6568;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289384;
  _v[0] = 1.0; 
  _v[1] = -1.03744;
  _v[2] = 39.1171;
  _v[3] = -2.12766;
  _v[4] = 26.9043;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289434;
  _v[0] = 1.0; 
  _v[1] = -1.09949;
  _v[2] = 36.5344;
  _v[3] = -2.08846;
  _v[4] = 25.9384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289437;
  _v[0] = 1.0; 
  _v[1] = -1.09949;
  _v[2] = 36.5344;
  _v[3] = -2.08846;
  _v[4] = 25.9384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289438;
  _v[0] = 1.0; 
  _v[1] = -1.09949;
  _v[2] = 36.5344;
  _v[3] = -2.08846;
  _v[4] = 25.9384;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289461;
  _v[0] = 1.0; 
  _v[1] = -0.20635;
  _v[2] = 37.3616;
  _v[3] = -3.34941;
  _v[4] = 26.8031;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289503;
  _v[0] = 1.0; 
  _v[1] = -0.763124;
  _v[2] = 37.8245;
  _v[3] = -2.56094;
  _v[4] = 27.3265;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289507;
  _v[0] = 1.0; 
  _v[1] = -0.763124;
  _v[2] = 37.8245;
  _v[3] = -2.56094;
  _v[4] = 27.3265;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289524;
  _v[0] = 1.0; 
  _v[1] = -0.27497;
  _v[2] = 36.1371;
  _v[3] = -3.45775;
  _v[4] = 26.2913;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289538;
  _v[0] = 1.0; 
  _v[1] = -0.694743;
  _v[2] = 34.846;
  _v[3] = -3.29465;
  _v[4] = 26.3747;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289582;
  _v[0] = 1.0; 
  _v[1] = -0.965509;
  _v[2] = 40.0287;
  _v[3] = -0.556394;
  _v[4] = 27.4558;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289584;
  _v[0] = 1.0; 
  _v[1] = -0.965509;
  _v[2] = 40.0287;
  _v[3] = -0.556394;
  _v[4] = 27.4558;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289620;
  _v[0] = 1.0; 
  _v[1] = -0.260742;
  _v[2] = 37.143;
  _v[3] = -3.89266;
  _v[4] = 26.5568;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289632;
  _v[0] = 1.0; 
  _v[1] = -0.577999;
  _v[2] = 37.5462;
  _v[3] = -3.23746;
  _v[4] = 26.6375;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289650;
  _v[0] = 1.0; 
  _v[1] = -0.609221;
  _v[2] = 35.4476;
  _v[3] = -2.38746;
  _v[4] = 26.3408;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289651;
  _v[0] = 1.0; 
  _v[1] = -0.609221;
  _v[2] = 35.4476;
  _v[3] = -2.38746;
  _v[4] = 26.3408;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289652;
  _v[0] = 1.0; 
  _v[1] = -5.60318;
  _v[2] = 72.6731;
  _v[3] = 24.1617;
  _v[4] = 19.2333;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289699;
  _v[0] = 1.0; 
  _v[1] = -1.43717;
  _v[2] = 31.4159;
  _v[3] = -2.21141;
  _v[4] = 24.166;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289701;
  _v[0] = 1.0; 
  _v[1] = -0.694926;
  _v[2] = 38.8449;
  _v[3] = -2.96389;
  _v[4] = 27.2208;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289716;
  _v[0] = 1.0; 
  _v[1] = -0.802873;
  _v[2] = 39.4623;
  _v[3] = -2.69499;
  _v[4] = 26.4332;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289729;
  _v[0] = 1.0; 
  _v[1] = -1.13541;
  _v[2] = 34.6564;
  _v[3] = -2.46243;
  _v[4] = 26.9167;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289731;
  _v[0] = 1.0; 
  _v[1] = -1.13541;
  _v[2] = 34.6564;
  _v[3] = -2.46243;
  _v[4] = 26.9167;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289732;
  _v[0] = 1.0; 
  _v[1] = 0.414265;
  _v[2] = 39.6885;
  _v[3] = -4.35429;
  _v[4] = 28.9243;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289734;
  _v[0] = 1.0; 
  _v[1] = -0.272816;
  _v[2] = 40.708;
  _v[3] = -2.68821;
  _v[4] = 31.0453;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289737;
  _v[0] = 1.0; 
  _v[1] = -0.846883;
  _v[2] = 46.8491;
  _v[3] = -4.5897;
  _v[4] = 31.5314;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289807;
  _v[0] = 1.0; 
  _v[1] = -0.134181;
  _v[2] = 35.3872;
  _v[3] = -3.86967;
  _v[4] = 25.7183;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289824;
  _v[0] = 1.0; 
  _v[1] = 0.183107;
  _v[2] = 29.8342;
  _v[3] = -3.94592;
  _v[4] = 24.1854;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289826;
  _v[0] = 1.0; 
  _v[1] = -0.84472;
  _v[2] = 38.9201;
  _v[3] = -2.24917;
  _v[4] = 27.1275;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289843;
  _v[0] = 1.0; 
  _v[1] = -0.525616;
  _v[2] = 34.5615;
  _v[3] = -3.48901;
  _v[4] = 24.6809;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289847;
  _v[0] = 1.0; 
  _v[1] = -0.0985848;
  _v[2] = 44.1007;
  _v[3] = -3.94887;
  _v[4] = 27.8364;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289848;
  _v[0] = 1.0; 
  _v[1] = -0.0985848;
  _v[2] = 44.1007;
  _v[3] = -3.94887;
  _v[4] = 27.8364;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289866;
  _v[0] = 1.0; 
  _v[1] = -2.01134;
  _v[2] = 31.7485;
  _v[3] = -0.525059;
  _v[4] = 24.805;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289893;
  _v[0] = 1.0; 
  _v[1] = -0.328257;
  _v[2] = 36.1198;
  _v[3] = -2.54529;
  _v[4] = 26.4823;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289894;
  _v[0] = 1.0; 
  _v[1] = -0.328257;
  _v[2] = 36.1198;
  _v[3] = -2.54529;
  _v[4] = 26.4823;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289895;
  _v[0] = 1.0; 
  _v[1] = 0.767689;
  _v[2] = 41.1967;
  _v[3] = -4.19932;
  _v[4] = 29.6181;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289898;
  _v[0] = 1.0; 
  _v[1] = 1.2114;
  _v[2] = 59.6297;
  _v[3] = -5.91255;
  _v[4] = 23.3935;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289916;
  _v[0] = 1.0; 
  _v[1] = -0.196053;
  _v[2] = 32.486;
  _v[3] = -3.07338;
  _v[4] = 26.574;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289924;
  _v[0] = 1.0; 
  _v[1] = -0.130697;
  _v[2] = 44.852;
  _v[3] = -3.92325;
  _v[4] = 27.4306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289925;
  _v[0] = 1.0; 
  _v[1] = -0.130697;
  _v[2] = 44.852;
  _v[3] = -3.92325;
  _v[4] = 27.4306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289943;
  _v[0] = 1.0; 
  _v[1] = -1.60965;
  _v[2] = 28.7977;
  _v[3] = -1.79145;
  _v[4] = 23.1122;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289944;
  _v[0] = 1.0; 
  _v[1] = -1.60965;
  _v[2] = 28.7977;
  _v[3] = -1.79145;
  _v[4] = 23.1122;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289945;
  _v[0] = 1.0; 
  _v[1] = 1.35962;
  _v[2] = 31.0535;
  _v[3] = -5.49434;
  _v[4] = 26.1765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289946;
  _v[0] = 1.0; 
  _v[1] = 1.35962;
  _v[2] = 31.0535;
  _v[3] = -5.49434;
  _v[4] = 26.1765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289947;
  _v[0] = 1.0; 
  _v[1] = 1.35962;
  _v[2] = 31.0535;
  _v[3] = -5.49434;
  _v[4] = 26.1765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289949;
  _v[0] = 1.0; 
  _v[1] = 1.35962;
  _v[2] = 31.0535;
  _v[3] = -5.49434;
  _v[4] = 26.1765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289953;
  _v[0] = 1.0; 
  _v[1] = 1.35962;
  _v[2] = 31.0535;
  _v[3] = -5.49434;
  _v[4] = 26.1765;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289954;
  _v[0] = 1.0; 
  _v[1] = -0.713384;
  _v[2] = 36.6895;
  _v[3] = -2.92962;
  _v[4] = 26.827;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289955;
  _v[0] = 1.0; 
  _v[1] = -0.316944;
  _v[2] = 45.7315;
  _v[3] = -2.8025;
  _v[4] = 28.2077;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 289956;
  _v[0] = 1.0; 
  _v[1] = -0.316944;
  _v[2] = 45.7315;
  _v[3] = -2.8025;
  _v[4] = 28.2077;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290000;
  _v[0] = 1.0; 
  _v[1] = -0.620656;
  _v[2] = 33.2653;
  _v[3] = -2.65204;
  _v[4] = 27.313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290006;
  _v[0] = 1.0; 
  _v[1] = 0.0172895;
  _v[2] = 41.5788;
  _v[3] = -3.024;
  _v[4] = 27.6337;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290010;
  _v[0] = 1.0; 
  _v[1] = 0.393231;
  _v[2] = 43.3344;
  _v[3] = -5.69193;
  _v[4] = 28.3329;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290011;
  _v[0] = 1.0; 
  _v[1] = 0.393231;
  _v[2] = 43.3344;
  _v[3] = -5.69193;
  _v[4] = 28.3329;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290014;
  _v[0] = 1.0; 
  _v[1] = 0.877472;
  _v[2] = -99.0745;
  _v[3] = -35.022;
  _v[4] = -20.3209;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290060;
  _v[0] = 1.0; 
  _v[1] = -0.106901;
  _v[2] = 37.7375;
  _v[3] = -3.43673;
  _v[4] = 26.1858;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290076;
  _v[0] = 1.0; 
  _v[1] = 0.270745;
  _v[2] = 44.2347;
  _v[3] = -2.47561;
  _v[4] = 28.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290078;
  _v[0] = 1.0; 
  _v[1] = 0.270745;
  _v[2] = 44.2347;
  _v[3] = -2.47561;
  _v[4] = 28.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290082;
  _v[0] = 1.0; 
  _v[1] = 0.270745;
  _v[2] = 44.2347;
  _v[3] = -2.47561;
  _v[4] = 28.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290131;
  _v[0] = 1.0; 
  _v[1] = 0.195794;
  _v[2] = 28.4815;
  _v[3] = -3.64302;
  _v[4] = 25.8632;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290133;
  _v[0] = 1.0; 
  _v[1] = 0.195794;
  _v[2] = 28.4815;
  _v[3] = -3.64302;
  _v[4] = 25.8632;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290135;
  _v[0] = 1.0; 
  _v[1] = -1.66882;
  _v[2] = 38.6942;
  _v[3] = -1.33765;
  _v[4] = 27.119;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290160;
  _v[0] = 1.0; 
  _v[1] = 0.0588436;
  _v[2] = 31.5275;
  _v[3] = -4.0561;
  _v[4] = 23.4115;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290185;
  _v[0] = 1.0; 
  _v[1] = -0.182068;
  _v[2] = 36.5855;
  _v[3] = -2.77509;
  _v[4] = 27.5231;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290186;
  _v[0] = 1.0; 
  _v[1] = 2.74004;
  _v[2] = 38.459;
  _v[3] = -6.88047;
  _v[4] = 32.8639;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290206;
  _v[0] = 1.0; 
  _v[1] = 0.433321;
  _v[2] = 29.6728;
  _v[3] = -4.35907;
  _v[4] = 21.8055;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290209;
  _v[0] = 1.0; 
  _v[1] = 0.433321;
  _v[2] = 29.6728;
  _v[3] = -4.35907;
  _v[4] = 21.8055;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290211;
  _v[0] = 1.0; 
  _v[1] = -0.0954355;
  _v[2] = 33.1788;
  _v[3] = -3.43762;
  _v[4] = 25.4587;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290212;
  _v[0] = 1.0; 
  _v[1] = -0.683984;
  _v[2] = 39.615;
  _v[3] = -2.16449;
  _v[4] = 27.7079;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290247;
  _v[0] = 1.0; 
  _v[1] = -1.7241;
  _v[2] = 31.1595;
  _v[3] = -1.69891;
  _v[4] = 24.8292;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290268;
  _v[0] = 1.0; 
  _v[1] = 0.104931;
  _v[2] = 39.8169;
  _v[3] = -3.15927;
  _v[4] = 27.0519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290269;
  _v[0] = 1.0; 
  _v[1] = 0.104931;
  _v[2] = 39.8169;
  _v[3] = -3.15927;
  _v[4] = 27.0519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290290;
  _v[0] = 1.0; 
  _v[1] = 0.570257;
  _v[2] = 30.8526;
  _v[3] = -3.76669;
  _v[4] = 24.9066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290292;
  _v[0] = 1.0; 
  _v[1] = 0.570257;
  _v[2] = 30.8526;
  _v[3] = -3.76669;
  _v[4] = 24.9066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290293;
  _v[0] = 1.0; 
  _v[1] = 0.570257;
  _v[2] = 30.8526;
  _v[3] = -3.76669;
  _v[4] = 24.9066;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290294;
  _v[0] = 1.0; 
  _v[1] = -0.0779561;
  _v[2] = 38.1076;
  _v[3] = -3.00411;
  _v[4] = 27.6304;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290301;
  _v[0] = 1.0; 
  _v[1] = 0.0255419;
  _v[2] = 45.371;
  _v[3] = -2.96374;
  _v[4] = 28.5343;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290347;
  _v[0] = 1.0; 
  _v[1] = -0.103437;
  _v[2] = 33.1819;
  _v[3] = -3.57661;
  _v[4] = 26.1916;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290349;
  _v[0] = 1.0; 
  _v[1] = -0.125038;
  _v[2] = 40.5445;
  _v[3] = -2.36946;
  _v[4] = 29.6162;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290362;
  _v[0] = 1.0; 
  _v[1] = -0.857417;
  _v[2] = 32.8726;
  _v[3] = -2.62698;
  _v[4] = 24.7355;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290440;
  _v[0] = 1.0; 
  _v[1] = -0.687231;
  _v[2] = 36.7077;
  _v[3] = -2.15522;
  _v[4] = 26.4997;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290446;
  _v[0] = 1.0; 
  _v[1] = 0.533933;
  _v[2] = 41.3095;
  _v[3] = -3.96182;
  _v[4] = 29.9594;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290462;
  _v[0] = 1.0; 
  _v[1] = -0.575259;
  _v[2] = 39.5844;
  _v[3] = -2.94654;
  _v[4] = 27.2638;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290476;
  _v[0] = 1.0; 
  _v[1] = -0.581212;
  _v[2] = 38.1752;
  _v[3] = -2.79858;
  _v[4] = 26.3088;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290508;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290509;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290510;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290521;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290528;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290530;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290537;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290538;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290540;
  _v[0] = 1.0; 
  _v[1] = -1.24381;
  _v[2] = 38.5116;
  _v[3] = -3.27445;
  _v[4] = 24.7425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290543;
  _v[0] = 1.0; 
  _v[1] = 0.531469;
  _v[2] = 40.9247;
  _v[3] = -2.96651;
  _v[4] = 28.5713;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290563;
  _v[0] = 1.0; 
  _v[1] = -0.85159;
  _v[2] = 39.6271;
  _v[3] = -2.03012;
  _v[4] = 26.4814;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290587;
  _v[0] = 1.0; 
  _v[1] = 0.185476;
  _v[2] = 30.6489;
  _v[3] = -4.04514;
  _v[4] = 25.6641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290588;
  _v[0] = 1.0; 
  _v[1] = 0.185476;
  _v[2] = 30.6489;
  _v[3] = -4.04514;
  _v[4] = 25.6641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290590;
  _v[0] = 1.0; 
  _v[1] = -0.64766;
  _v[2] = 40.1108;
  _v[3] = -2.80646;
  _v[4] = 27.1454;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290591;
  _v[0] = 1.0; 
  _v[1] = -1.12492;
  _v[2] = 1.53126e+08;
  _v[3] = 3.02254e+07;
  _v[4] = 19.1933;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290605;
  _v[0] = 1.0; 
  _v[1] = -0.0913972;
  _v[2] = 32.8977;
  _v[3] = -3.75349;
  _v[4] = 25.1303;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290606;
  _v[0] = 1.0; 
  _v[1] = -0.0913972;
  _v[2] = 32.8977;
  _v[3] = -3.75349;
  _v[4] = 25.1303;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290607;
  _v[0] = 1.0; 
  _v[1] = -0.490965;
  _v[2] = 39.7592;
  _v[3] = -2.37897;
  _v[4] = 27.6146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290609;
  _v[0] = 1.0; 
  _v[1] = -0.490965;
  _v[2] = 39.7592;
  _v[3] = -2.37897;
  _v[4] = 27.6146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290653;
  _v[0] = 1.0; 
  _v[1] = -0.5892;
  _v[2] = 36.3877;
  _v[3] = -2.30907;
  _v[4] = 25.7196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290654;
  _v[0] = 1.0; 
  _v[1] = -0.5892;
  _v[2] = 36.3877;
  _v[3] = -2.30907;
  _v[4] = 25.7196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290656;
  _v[0] = 1.0; 
  _v[1] = -1.01397;
  _v[2] = 41.8864;
  _v[3] = -1.96822;
  _v[4] = 28.3391;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290697;
  _v[0] = 1.0; 
  _v[1] = -0.927191;
  _v[2] = 38.5618;
  _v[3] = -1.88688;
  _v[4] = 26.6183;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290698;
  _v[0] = 1.0; 
  _v[1] = -0.927191;
  _v[2] = 38.5618;
  _v[3] = -1.88688;
  _v[4] = 26.6183;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290719;
  _v[0] = 1.0; 
  _v[1] = -0.826675;
  _v[2] = 33.2323;
  _v[3] = -2.9397;
  _v[4] = 25.1082;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290720;
  _v[0] = 1.0; 
  _v[1] = -0.74233;
  _v[2] = 39.8633;
  _v[3] = -1.87821;
  _v[4] = 28.278;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290721;
  _v[0] = 1.0; 
  _v[1] = -0.74233;
  _v[2] = 39.8633;
  _v[3] = -1.87821;
  _v[4] = 28.278;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290722;
  _v[0] = 1.0; 
  _v[1] = -0.74233;
  _v[2] = 39.8633;
  _v[3] = -1.87821;
  _v[4] = 28.278;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290736;
  _v[0] = 1.0; 
  _v[1] = -0.378625;
  _v[2] = 32.0875;
  _v[3] = -3.34687;
  _v[4] = 26.1715;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290737;
  _v[0] = 1.0; 
  _v[1] = -0.378625;
  _v[2] = 32.0875;
  _v[3] = -3.34687;
  _v[4] = 26.1715;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290741;
  _v[0] = 1.0; 
  _v[1] = -0.0512424;
  _v[2] = 40.0969;
  _v[3] = -3.38702;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290755;
  _v[0] = 1.0; 
  _v[1] = -0.0151816;
  _v[2] = 33.5114;
  _v[3] = -3.24627;
  _v[4] = 26.6182;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290756;
  _v[0] = 1.0; 
  _v[1] = -0.0151816;
  _v[2] = 33.5114;
  _v[3] = -3.24627;
  _v[4] = 26.6182;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290757;
  _v[0] = 1.0; 
  _v[1] = 0.0089529;
  _v[2] = 41.2557;
  _v[3] = -4.06699;
  _v[4] = 28.1917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290761;
  _v[0] = 1.0; 
  _v[1] = 0.0089529;
  _v[2] = 41.2557;
  _v[3] = -4.06699;
  _v[4] = 28.1917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290776;
  _v[0] = 1.0; 
  _v[1] = -0.307506;
  _v[2] = 37.3057;
  _v[3] = -3.03995;
  _v[4] = 26.9674;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290809;
  _v[0] = 1.0; 
  _v[1] = 0.19274;
  _v[2] = 32.8152;
  _v[3] = -3.89983;
  _v[4] = 24.7167;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290813;
  _v[0] = 1.0; 
  _v[1] = 0.19274;
  _v[2] = 32.8152;
  _v[3] = -3.89983;
  _v[4] = 24.7167;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290814;
  _v[0] = 1.0; 
  _v[1] = 0.215071;
  _v[2] = 39.2309;
  _v[3] = -4.22456;
  _v[4] = 27.5917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290833;
  _v[0] = 1.0; 
  _v[1] = -0.251025;
  _v[2] = 38.6657;
  _v[3] = -2.94537;
  _v[4] = 25.9344;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290841;
  _v[0] = 1.0; 
  _v[1] = -0.932953;
  _v[2] = 44.8586;
  _v[3] = -1.18148;
  _v[4] = 27.4722;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290856;
  _v[0] = 1.0; 
  _v[1] = -0.932953;
  _v[2] = 44.8586;
  _v[3] = -1.18148;
  _v[4] = 27.4722;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290857;
  _v[0] = 1.0; 
  _v[1] = -0.932953;
  _v[2] = 44.8586;
  _v[3] = -1.18148;
  _v[4] = 27.4722;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290871;
  _v[0] = 1.0; 
  _v[1] = -0.0837158;
  _v[2] = 36.6487;
  _v[3] = -3.32298;
  _v[4] = 26.4975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290873;
  _v[0] = 1.0; 
  _v[1] = -0.0837158;
  _v[2] = 36.6487;
  _v[3] = -3.32298;
  _v[4] = 26.4975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290874;
  _v[0] = 1.0; 
  _v[1] = -0.0837158;
  _v[2] = 36.6487;
  _v[3] = -3.32298;
  _v[4] = 26.4975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290878;
  _v[0] = 1.0; 
  _v[1] = -0.888704;
  _v[2] = 41.6131;
  _v[3] = -0.756865;
  _v[4] = 30.689;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290879;
  _v[0] = 1.0; 
  _v[1] = -2.25039;
  _v[2] = 5636.58;
  _v[3] = 7408.22;
  _v[4] = 21.6048;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290903;
  _v[0] = 1.0; 
  _v[1] = -0.683558;
  _v[2] = 28.4717;
  _v[3] = -3.10623;
  _v[4] = 27.812;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290904;
  _v[0] = 1.0; 
  _v[1] = -0.240311;
  _v[2] = 33.4325;
  _v[3] = -2.51106;
  _v[4] = 27.8306;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290905;
  _v[0] = 1.0; 
  _v[1] = -0.23454;
  _v[2] = 39.6811;
  _v[3] = -2.77831;
  _v[4] = 28.9808;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290920;
  _v[0] = 1.0; 
  _v[1] = -0.999455;
  _v[2] = 32.7505;
  _v[3] = -2.69732;
  _v[4] = 25.4315;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290921;
  _v[0] = 1.0; 
  _v[1] = -0.999455;
  _v[2] = 32.7505;
  _v[3] = -2.69732;
  _v[4] = 25.4315;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290922;
  _v[0] = 1.0; 
  _v[1] = -1.16904;
  _v[2] = 33.9131;
  _v[3] = -1.73077;
  _v[4] = 28.9293;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290923;
  _v[0] = 1.0; 
  _v[1] = -1.16904;
  _v[2] = 33.9131;
  _v[3] = -1.73077;
  _v[4] = 28.9293;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290924;
  _v[0] = 1.0; 
  _v[1] = -1.3359;
  _v[2] = 38.699;
  _v[3] = -0.594027;
  _v[4] = 28.6519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290925;
  _v[0] = 1.0; 
  _v[1] = -1.3359;
  _v[2] = 38.699;
  _v[3] = -0.594027;
  _v[4] = 28.6519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290928;
  _v[0] = 1.0; 
  _v[1] = -1.3359;
  _v[2] = 38.699;
  _v[3] = -0.594027;
  _v[4] = 28.6519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290929;
  _v[0] = 1.0; 
  _v[1] = -1.3359;
  _v[2] = 38.699;
  _v[3] = -0.594027;
  _v[4] = 28.6519;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290930;
  _v[0] = 1.0; 
  _v[1] = -0.420889;
  _v[2] = 41.0479;
  _v[3] = -2.49349;
  _v[4] = 29.3687;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290947;
  _v[0] = 1.0; 
  _v[1] = -0.906107;
  _v[2] = 34.2962;
  _v[3] = -2.41549;
  _v[4] = 26.0553;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290948;
  _v[0] = 1.0; 
  _v[1] = -1.35628;
  _v[2] = 40.5951;
  _v[3] = -0.63581;
  _v[4] = 27.2311;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290949;
  _v[0] = 1.0; 
  _v[1] = -1.35628;
  _v[2] = 40.5951;
  _v[3] = -0.63581;
  _v[4] = 27.2311;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290950;
  _v[0] = 1.0; 
  _v[1] = -1.35628;
  _v[2] = 40.5951;
  _v[3] = -0.63581;
  _v[4] = 27.2311;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290951;
  _v[0] = 1.0; 
  _v[1] = -1.35628;
  _v[2] = 40.5951;
  _v[3] = -0.63581;
  _v[4] = 27.2311;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290969;
  _v[0] = 1.0; 
  _v[1] = -0.242071;
  _v[2] = 32.4675;
  _v[3] = -3.81316;
  _v[4] = 23.386;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290970;
  _v[0] = 1.0; 
  _v[1] = -0.242071;
  _v[2] = 32.4675;
  _v[3] = -3.81316;
  _v[4] = 23.386;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290973;
  _v[0] = 1.0; 
  _v[1] = -0.382705;
  _v[2] = 39.1897;
  _v[3] = -3.29946;
  _v[4] = 27.7581;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290974;
  _v[0] = 1.0; 
  _v[1] = -0.382705;
  _v[2] = 39.1897;
  _v[3] = -3.29946;
  _v[4] = 27.7581;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290975;
  _v[0] = 1.0; 
  _v[1] = -0.382705;
  _v[2] = 39.1897;
  _v[3] = -3.29946;
  _v[4] = 27.7581;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290991;
  _v[0] = 1.0; 
  _v[1] = -0.579387;
  _v[2] = 37.4515;
  _v[3] = -2.64819;
  _v[4] = 26.6718;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 290992;
  _v[0] = 1.0; 
  _v[1] = -0.579387;
  _v[2] = 37.4515;
  _v[3] = -2.64819;
  _v[4] = 26.6718;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291008;
  _v[0] = 1.0; 
  _v[1] = 0.0582438;
  _v[2] = 32.2457;
  _v[3] = -4.34448;
  _v[4] = 25.3274;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291010;
  _v[0] = 1.0; 
  _v[1] = -0.502584;
  _v[2] = 37.9498;
  _v[3] = -3.41225;
  _v[4] = 26.4517;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291011;
  _v[0] = 1.0; 
  _v[1] = 0.810661;
  _v[2] = 41.2319;
  _v[3] = -5.62697;
  _v[4] = 28.8972;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291025;
  _v[0] = 1.0; 
  _v[1] = -0.292251;
  _v[2] = 36.0959;
  _v[3] = -2.64008;
  _v[4] = 25.8285;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291294;
  _v[0] = 1.0; 
  _v[1] = 0.183602;
  _v[2] = 33.7135;
  _v[3] = -2.97881;
  _v[4] = 26.5432;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291295;
  _v[0] = 1.0; 
  _v[1] = -0.341038;
  _v[2] = 34.6817;
  _v[3] = -3.58897;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291296;
  _v[0] = 1.0; 
  _v[1] = -0.341038;
  _v[2] = 34.6817;
  _v[3] = -3.58897;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291297;
  _v[0] = 1.0; 
  _v[1] = -0.341038;
  _v[2] = 34.6817;
  _v[3] = -3.58897;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291298;
  _v[0] = 1.0; 
  _v[1] = -0.341038;
  _v[2] = 34.6817;
  _v[3] = -3.58897;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291299;
  _v[0] = 1.0; 
  _v[1] = -0.341038;
  _v[2] = 34.6817;
  _v[3] = -3.58897;
  _v[4] = 27.7061;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291300;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291301;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291302;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291303;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291305;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291307;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291308;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291310;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291311;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291312;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291313;
  _v[0] = 1.0; 
  _v[1] = 0.691916;
  _v[2] = 40.2046;
  _v[3] = -3.79099;
  _v[4] = 29.2377;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291314;
  _v[0] = 1.0; 
  _v[1] = -3.3087;
  _v[2] = 39.5563;
  _v[3] = 1.99006;
  _v[4] = 30.815;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291331;
  _v[0] = 1.0; 
  _v[1] = -0.0243331;
  _v[2] = 39.8557;
  _v[3] = -4.11917;
  _v[4] = 25.2772;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291334;
  _v[0] = 1.0; 
  _v[1] = -0.0243331;
  _v[2] = 39.8557;
  _v[3] = -4.11917;
  _v[4] = 25.2772;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291349;
  _v[0] = 1.0; 
  _v[1] = -0.60362;
  _v[2] = 39.4256;
  _v[3] = -2.42269;
  _v[4] = 27.2017;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291367;
  _v[0] = 1.0; 
  _v[1] = -0.161512;
  _v[2] = 32.0112;
  _v[3] = -3.57696;
  _v[4] = 24.4783;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291368;
  _v[0] = 1.0; 
  _v[1] = -0.279743;
  _v[2] = 41.7515;
  _v[3] = -3.0548;
  _v[4] = 27.5697;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291371;
  _v[0] = 1.0; 
  _v[1] = -0.491668;
  _v[2] = 47.059;
  _v[3] = -1.41401;
  _v[4] = 29.9785;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291372;
  _v[0] = 1.0; 
  _v[1] = -0.491668;
  _v[2] = 47.059;
  _v[3] = -1.41401;
  _v[4] = 29.9785;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291374;
  _v[0] = 1.0; 
  _v[1] = -0.491668;
  _v[2] = 47.059;
  _v[3] = -1.41401;
  _v[4] = 29.9785;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291433;
  _v[0] = 1.0; 
  _v[1] = 0.935994;
  _v[2] = 37.5398;
  _v[3] = -3.92782;
  _v[4] = 28.3937;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291434;
  _v[0] = 1.0; 
  _v[1] = -0.492586;
  _v[2] = 41.1189;
  _v[3] = -3.04506;
  _v[4] = 31.2424;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291435;
  _v[0] = 1.0; 
  _v[1] = -0.492586;
  _v[2] = 41.1189;
  _v[3] = -3.04506;
  _v[4] = 31.2424;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291450;
  _v[0] = 1.0; 
  _v[1] = 0.030282;
  _v[2] = 29.449;
  _v[3] = -3.44924;
  _v[4] = 24.3419;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291451;
  _v[0] = 1.0; 
  _v[1] = -2.00928;
  _v[2] = 32.0156;
  _v[3] = -1.03315;
  _v[4] = 26.8317;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291452;
  _v[0] = 1.0; 
  _v[1] = -0.227413;
  _v[2] = 37.8334;
  _v[3] = -3.30979;
  _v[4] = 27.9917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291453;
  _v[0] = 1.0; 
  _v[1] = -0.227413;
  _v[2] = 37.8334;
  _v[3] = -3.30979;
  _v[4] = 27.9917;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291455;
  _v[0] = 1.0; 
  _v[1] = -1.28264;
  _v[2] = 41.1863;
  _v[3] = -0.756608;
  _v[4] = 29.5577;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291528;
  _v[0] = 1.0; 
  _v[1] = 0.50576;
  _v[2] = 34.1827;
  _v[3] = -3.81236;
  _v[4] = 26.1318;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291543;
  _v[0] = 1.0; 
  _v[1] = 0.232223;
  _v[2] = 39.3439;
  _v[3] = -3.03049;
  _v[4] = 28.9158;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291544;
  _v[0] = 1.0; 
  _v[1] = 0.14877;
  _v[2] = 41.4694;
  _v[3] = -3.64924;
  _v[4] = 28.1835;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291546;
  _v[0] = 1.0; 
  _v[1] = 0.14877;
  _v[2] = 41.4694;
  _v[3] = -3.64924;
  _v[4] = 28.1835;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291548;
  _v[0] = 1.0; 
  _v[1] = 0.14877;
  _v[2] = 41.4694;
  _v[3] = -3.64924;
  _v[4] = 28.1835;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291554;
  _v[0] = 1.0; 
  _v[1] = -0.634342;
  _v[2] = 56.2453;
  _v[3] = -1.94181;
  _v[4] = 23.3842;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291633;
  _v[0] = 1.0; 
  _v[1] = -0.560243;
  _v[2] = 37.7549;
  _v[3] = -2.40437;
  _v[4] = 26.5741;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291650;
  _v[0] = 1.0; 
  _v[1] = -0.673939;
  _v[2] = 31.9442;
  _v[3] = -3.53742;
  _v[4] = 24.573;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291651;
  _v[0] = 1.0; 
  _v[1] = -0.80922;
  _v[2] = 38.7213;
  _v[3] = -1.67353;
  _v[4] = 27.179;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291652;
  _v[0] = 1.0; 
  _v[1] = 0.888834;
  _v[2] = 43.2605;
  _v[3] = -5.03858;
  _v[4] = 28.2416;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291667;
  _v[0] = 1.0; 
  _v[1] = -0.368708;
  _v[2] = 35.731;
  _v[3] = -3.26103;
  _v[4] = 24.9256;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291693;
  _v[0] = 1.0; 
  _v[1] = -1.15668;
  _v[2] = 30.7112;
  _v[3] = -2.52459;
  _v[4] = 23.1932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291694;
  _v[0] = 1.0; 
  _v[1] = -1.15668;
  _v[2] = 30.7112;
  _v[3] = -2.52459;
  _v[4] = 23.1932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291695;
  _v[0] = 1.0; 
  _v[1] = -0.709295;
  _v[2] = 35.898;
  _v[3] = -2.44141;
  _v[4] = 27.957;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291696;
  _v[0] = 1.0; 
  _v[1] = -0.709295;
  _v[2] = 35.898;
  _v[3] = -2.44141;
  _v[4] = 27.957;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291715;
  _v[0] = 1.0; 
  _v[1] = -0.619607;
  _v[2] = 33.0563;
  _v[3] = -2.90813;
  _v[4] = 25.6884;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291716;
  _v[0] = 1.0; 
  _v[1] = 0.842424;
  _v[2] = 36.3995;
  _v[3] = -4.85889;
  _v[4] = 27.2175;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291717;
  _v[0] = 1.0; 
  _v[1] = 0.842424;
  _v[2] = 36.3995;
  _v[3] = -4.85889;
  _v[4] = 27.2175;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291718;
  _v[0] = 1.0; 
  _v[1] = -0.0783547;
  _v[2] = 42.2876;
  _v[3] = -3.8243;
  _v[4] = 27.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291721;
  _v[0] = 1.0; 
  _v[1] = -0.0783547;
  _v[2] = 42.2876;
  _v[3] = -3.8243;
  _v[4] = 27.6309;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291735;
  _v[0] = 1.0; 
  _v[1] = -1.04214;
  _v[2] = 34.6709;
  _v[3] = -2.5628;
  _v[4] = 25.2527;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291736;
  _v[0] = 1.0; 
  _v[1] = -1.04214;
  _v[2] = 34.6709;
  _v[3] = -2.5628;
  _v[4] = 25.2527;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291740;
  _v[0] = 1.0; 
  _v[1] = -1.2646;
  _v[2] = 40.462;
  _v[3] = -0.705488;
  _v[4] = 28.5379;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291741;
  _v[0] = 1.0; 
  _v[1] = -1.2646;
  _v[2] = 40.462;
  _v[3] = -0.705488;
  _v[4] = 28.5379;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291758;
  _v[0] = 1.0; 
  _v[1] = -0.118178;
  _v[2] = 32.2422;
  _v[3] = -3.41309;
  _v[4] = 24.3181;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291762;
  _v[0] = 1.0; 
  _v[1] = -0.482942;
  _v[2] = 39.232;
  _v[3] = -2.32796;
  _v[4] = 27.4218;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291763;
  _v[0] = 1.0; 
  _v[1] = -0.482942;
  _v[2] = 39.232;
  _v[3] = -2.32796;
  _v[4] = 27.4218;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291774;
  _v[0] = 1.0; 
  _v[1] = -0.261834;
  _v[2] = 32.0594;
  _v[3] = -4.00748;
  _v[4] = 24.1068;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291778;
  _v[0] = 1.0; 
  _v[1] = -1.33169;
  _v[2] = 39.2271;
  _v[3] = -1.50039;
  _v[4] = 26.8259;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291832;
  _v[0] = 1.0; 
  _v[1] = -0.316435;
  _v[2] = 37.2052;
  _v[3] = -3.40801;
  _v[4] = 25.6518;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291838;
  _v[0] = 1.0; 
  _v[1] = -2.01101;
  _v[2] = 53.5301;
  _v[3] = 4.84602;
  _v[4] = 21.019;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291860;
  _v[0] = 1.0; 
  _v[1] = -0.228401;
  _v[2] = 33.8518;
  _v[3] = -3.24919;
  _v[4] = 25.2318;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291862;
  _v[0] = 1.0; 
  _v[1] = -0.228401;
  _v[2] = 33.8518;
  _v[3] = -3.24919;
  _v[4] = 25.2318;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291863;
  _v[0] = 1.0; 
  _v[1] = -1.08272;
  _v[2] = 39.1696;
  _v[3] = -1.76465;
  _v[4] = 27.5793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291881;
  _v[0] = 1.0; 
  _v[1] = -0.786987;
  _v[2] = 43.0938;
  _v[3] = -1.11751;
  _v[4] = 26.8085;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291910;
  _v[0] = 1.0; 
  _v[1] = -0.617981;
  _v[2] = 35.4696;
  _v[3] = -2.83431;
  _v[4] = 25.5215;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291916;
  _v[0] = 1.0; 
  _v[1] = 0.145607;
  _v[2] = 42.1714;
  _v[3] = -2.42369;
  _v[4] = 28.4897;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291919;
  _v[0] = 1.0; 
  _v[1] = 0.145607;
  _v[2] = 42.1714;
  _v[3] = -2.42369;
  _v[4] = 28.4897;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291935;
  _v[0] = 1.0; 
  _v[1] = -0.144967;
  _v[2] = 38.7222;
  _v[3] = -3.22632;
  _v[4] = 26.5237;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291937;
  _v[0] = 1.0; 
  _v[1] = -0.144967;
  _v[2] = 38.7222;
  _v[3] = -3.22632;
  _v[4] = 26.5237;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 291975;
  _v[0] = 1.0; 
  _v[1] = -0.203316;
  _v[2] = 36.3111;
  _v[3] = -3.21037;
  _v[4] = 26.1532;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292007;
  _v[0] = 1.0; 
  _v[1] = -0.23092;
  _v[2] = 32.3863;
  _v[3] = -3.12665;
  _v[4] = 25.1578;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292008;
  _v[0] = 1.0; 
  _v[1] = -0.650131;
  _v[2] = 34.8267;
  _v[3] = -1.38008;
  _v[4] = 28.02;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292009;
  _v[0] = 1.0; 
  _v[1] = -0.650131;
  _v[2] = 34.8267;
  _v[3] = -1.38008;
  _v[4] = 28.02;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292010;
  _v[0] = 1.0; 
  _v[1] = 0.166486;
  _v[2] = 40.7493;
  _v[3] = -4.20809;
  _v[4] = 28.1997;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292032;
  _v[0] = 1.0; 
  _v[1] = -0.543506;
  _v[2] = 36.0526;
  _v[3] = -2.43596;
  _v[4] = 25.6498;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292036;
  _v[0] = 1.0; 
  _v[1] = -0.535216;
  _v[2] = 34.5304;
  _v[3] = -3.4288;
  _v[4] = 24.7289;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292058;
  _v[0] = 1.0; 
  _v[1] = 0.482916;
  _v[2] = 41.9288;
  _v[3] = -3.63646;
  _v[4] = 28.2;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292063;
  _v[0] = 1.0; 
  _v[1] = 0.482916;
  _v[2] = 41.9288;
  _v[3] = -3.63646;
  _v[4] = 28.2;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292067;
  _v[0] = 1.0; 
  _v[1] = 0.482916;
  _v[2] = 41.9288;
  _v[3] = -3.63646;
  _v[4] = 28.2;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292071;
  _v[0] = 1.0; 
  _v[1] = -1.54157;
  _v[2] = 98.5163;
  _v[3] = -32.3604;
  _v[4] = 19.8836;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292082;
  _v[0] = 1.0; 
  _v[1] = -0.0324804;
  _v[2] = 30.8695;
  _v[3] = -3.88174;
  _v[4] = 24.5987;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292083;
  _v[0] = 1.0; 
  _v[1] = 0.57472;
  _v[2] = 34.3616;
  _v[3] = -6.2772;
  _v[4] = 24.1932;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292084;
  _v[0] = 1.0; 
  _v[1] = 0.297725;
  _v[2] = 39.0732;
  _v[3] = -3.95911;
  _v[4] = 25.8491;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292085;
  _v[0] = 1.0; 
  _v[1] = 0.297725;
  _v[2] = 39.0732;
  _v[3] = -3.95911;
  _v[4] = 25.8491;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292100;
  _v[0] = 1.0; 
  _v[1] = -2.16428;
  _v[2] = 40.6917;
  _v[3] = -0.9036;
  _v[4] = 26.616;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292103;
  _v[0] = 1.0; 
  _v[1] = -2.16428;
  _v[2] = 40.6917;
  _v[3] = -0.9036;
  _v[4] = 26.616;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292116;
  _v[0] = 1.0; 
  _v[1] = -2.16428;
  _v[2] = 40.6917;
  _v[3] = -0.9036;
  _v[4] = 26.616;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292117;
  _v[0] = 1.0; 
  _v[1] = -1.04161;
  _v[2] = 41.4719;
  _v[3] = 0.356236;
  _v[4] = 30.0828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292119;
  _v[0] = 1.0; 
  _v[1] = -1.04161;
  _v[2] = 41.4719;
  _v[3] = 0.356236;
  _v[4] = 30.0828;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292145;
  _v[0] = 1.0; 
  _v[1] = 0.0195184;
  _v[2] = 27.8572;
  _v[3] = -3.63919;
  _v[4] = 25.1422;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292252;
  _v[0] = 1.0; 
  _v[1] = -0.331046;
  _v[2] = 37.778;
  _v[3] = -2.91933;
  _v[4] = 26.4474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292253;
  _v[0] = 1.0; 
  _v[1] = -0.331046;
  _v[2] = 37.778;
  _v[3] = -2.91933;
  _v[4] = 26.4474;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292270;
  _v[0] = 1.0; 
  _v[1] = -0.345162;
  _v[2] = 38.7885;
  _v[3] = -2.94725;
  _v[4] = 24.6294;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292289;
  _v[0] = 1.0; 
  _v[1] = -0.58194;
  _v[2] = 31.5451;
  _v[3] = -2.19854;
  _v[4] = 24.6152;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292318;
  _v[0] = 1.0; 
  _v[1] = 0.22007;
  _v[2] = 38.3444;
  _v[3] = -3.45555;
  _v[4] = 26.5532;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292320;
  _v[0] = 1.0; 
  _v[1] = 2.28794;
  _v[2] = 40.9497;
  _v[3] = -7.85285;
  _v[4] = 28.0267;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292322;
  _v[0] = 1.0; 
  _v[1] = 2.28794;
  _v[2] = 40.9497;
  _v[3] = -7.85285;
  _v[4] = 28.0267;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292323;
  _v[0] = 1.0; 
  _v[1] = 2.28794;
  _v[2] = 40.9497;
  _v[3] = -7.85285;
  _v[4] = 28.0267;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292324;
  _v[0] = 1.0; 
  _v[1] = 2.28794;
  _v[2] = 40.9497;
  _v[3] = -7.85285;
  _v[4] = 28.0267;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292343;
  _v[0] = 1.0; 
  _v[1] = -0.711557;
  _v[2] = 37.7906;
  _v[3] = -2.20316;
  _v[4] = 25.9284;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292361;
  _v[0] = 1.0; 
  _v[1] = -0.530175;
  _v[2] = 38.9329;
  _v[3] = -2.75537;
  _v[4] = 25.6123;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292362;
  _v[0] = 1.0; 
  _v[1] = -0.530175;
  _v[2] = 38.9329;
  _v[3] = -2.75537;
  _v[4] = 25.6123;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292378;
  _v[0] = 1.0; 
  _v[1] = -0.0755145;
  _v[2] = 37.1809;
  _v[3] = -3.27898;
  _v[4] = 26.1622;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292379;
  _v[0] = 1.0; 
  _v[1] = -0.193811;
  _v[2] = 39.4148;
  _v[3] = -3.12366;
  _v[4] = 29.6692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292395;
  _v[0] = 1.0; 
  _v[1] = -0.261681;
  _v[2] = 33.02;
  _v[3] = -3.06771;
  _v[4] = 25.938;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292396;
  _v[0] = 1.0; 
  _v[1] = -0.261681;
  _v[2] = 33.02;
  _v[3] = -3.06771;
  _v[4] = 25.938;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292397;
  _v[0] = 1.0; 
  _v[1] = 3.21018;
  _v[2] = 34.335;
  _v[3] = -6.86726;
  _v[4] = 30.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292398;
  _v[0] = 1.0; 
  _v[1] = 3.21018;
  _v[2] = 34.335;
  _v[3] = -6.86726;
  _v[4] = 30.8692;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292419;
  _v[0] = 1.0; 
  _v[1] = 2.48764;
  _v[2] = 30.3875;
  _v[3] = -7.10636;
  _v[4] = 23.2678;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292420;
  _v[0] = 1.0; 
  _v[1] = -0.247924;
  _v[2] = 39.6202;
  _v[3] = -2.88382;
  _v[4] = 26.8176;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292435;
  _v[0] = 1.0; 
  _v[1] = -0.577548;
  _v[2] = 43.5621;
  _v[3] = -2.13165;
  _v[4] = 29.5391;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292455;
  _v[0] = 1.0; 
  _v[1] = 0.100318;
  _v[2] = 36.4974;
  _v[3] = -3.64359;
  _v[4] = 25.3064;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292488;
  _v[0] = 1.0; 
  _v[1] = -0.329032;
  _v[2] = 36.9043;
  _v[3] = -2.9497;
  _v[4] = 26.1576;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292491;
  _v[0] = 1.0; 
  _v[1] = -0.329032;
  _v[2] = 36.9043;
  _v[3] = -2.9497;
  _v[4] = 26.1576;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292492;
  _v[0] = 1.0; 
  _v[1] = -0.329032;
  _v[2] = 36.9043;
  _v[3] = -2.9497;
  _v[4] = 26.1576;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292503;
  _v[0] = 1.0; 
  _v[1] = -0.718537;
  _v[2] = 34.9478;
  _v[3] = -1.93481;
  _v[4] = 25.599;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292504;
  _v[0] = 1.0; 
  _v[1] = -0.0829408;
  _v[2] = 39.5586;
  _v[3] = -2.38881;
  _v[4] = 27.9322;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292532;
  _v[0] = 1.0; 
  _v[1] = 0.202801;
  _v[2] = 38.9771;
  _v[3] = -3.99384;
  _v[4] = 26.3263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292533;
  _v[0] = 1.0; 
  _v[1] = 0.202801;
  _v[2] = 38.9771;
  _v[3] = -3.99384;
  _v[4] = 26.3263;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292549;
  _v[0] = 1.0; 
  _v[1] = -0.945373;
  _v[2] = 30.0935;
  _v[3] = -2.44771;
  _v[4] = 24.7303;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292551;
  _v[0] = 1.0; 
  _v[1] = 0.163154;
  _v[2] = 40.3746;
  _v[3] = -3.76207;
  _v[4] = 26.9953;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292564;
  _v[0] = 1.0; 
  _v[1] = 0.501367;
  _v[2] = 30.6909;
  _v[3] = -4.40877;
  _v[4] = 23.3493;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292565;
  _v[0] = 1.0; 
  _v[1] = 0.501367;
  _v[2] = 30.6909;
  _v[3] = -4.40877;
  _v[4] = 23.3493;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292584;
  _v[0] = 1.0; 
  _v[1] = 1.16666;
  _v[2] = 33.9165;
  _v[3] = -4.28403;
  _v[4] = 26.6163;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292671;
  _v[0] = 1.0; 
  _v[1] = -0.24377;
  _v[2] = 39.1583;
  _v[3] = -2.06005;
  _v[4] = 26.9156;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292672;
  _v[0] = 1.0; 
  _v[1] = -0.24377;
  _v[2] = 39.1583;
  _v[3] = -2.06005;
  _v[4] = 26.9156;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292673;
  _v[0] = 1.0; 
  _v[1] = -0.24377;
  _v[2] = 39.1583;
  _v[3] = -2.06005;
  _v[4] = 26.9156;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292694;
  _v[0] = 1.0; 
  _v[1] = -0.24377;
  _v[2] = 39.1583;
  _v[3] = -2.06005;
  _v[4] = 26.9156;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292706;
  _v[0] = 1.0; 
  _v[1] = -0.188776;
  _v[2] = 39.5592;
  _v[3] = -3.36481;
  _v[4] = 25.8658;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292725;
  _v[0] = 1.0; 
  _v[1] = -0.659247;
  _v[2] = 29.1146;
  _v[3] = -3.97142;
  _v[4] = 23.8443;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292726;
  _v[0] = 1.0; 
  _v[1] = -0.0446796;
  _v[2] = 35.8156;
  _v[3] = -3.09397;
  _v[4] = 26.0622;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292727;
  _v[0] = 1.0; 
  _v[1] = -0.674717;
  _v[2] = 39.4696;
  _v[3] = -3.30565;
  _v[4] = 25.9456;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292728;
  _v[0] = 1.0; 
  _v[1] = -0.674717;
  _v[2] = 39.4696;
  _v[3] = -3.30565;
  _v[4] = 25.9456;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292750;
  _v[0] = 1.0; 
  _v[1] = -0.444432;
  _v[2] = 37.9023;
  _v[3] = -2.53899;
  _v[4] = 25.773;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292790;
  _v[0] = 1.0; 
  _v[1] = 0.239547;
  _v[2] = 34.3809;
  _v[3] = -3.68639;
  _v[4] = 26.793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292791;
  _v[0] = 1.0; 
  _v[1] = 0.239547;
  _v[2] = 34.3809;
  _v[3] = -3.68639;
  _v[4] = 26.793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292792;
  _v[0] = 1.0; 
  _v[1] = 0.239547;
  _v[2] = 34.3809;
  _v[3] = -3.68639;
  _v[4] = 26.793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292793;
  _v[0] = 1.0; 
  _v[1] = -0.642573;
  _v[2] = 39.7338;
  _v[3] = -1.56875;
  _v[4] = 26.8052;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292794;
  _v[0] = 1.0; 
  _v[1] = -0.642573;
  _v[2] = 39.7338;
  _v[3] = -1.56875;
  _v[4] = 26.8052;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292795;
  _v[0] = 1.0; 
  _v[1] = 0.0793071;
  _v[2] = 45.3272;
  _v[3] = -2.63214;
  _v[4] = 26.2914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292796;
  _v[0] = 1.0; 
  _v[1] = 0.0793071;
  _v[2] = 45.3272;
  _v[3] = -2.63214;
  _v[4] = 26.2914;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292814;
  _v[0] = 1.0; 
  _v[1] = -0.0102811;
  _v[2] = 30.342;
  _v[3] = -2.99982;
  _v[4] = 24.2425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292815;
  _v[0] = 1.0; 
  _v[1] = 0.0740181;
  _v[2] = 41.0164;
  _v[3] = -3.07877;
  _v[4] = 27.0462;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292819;
  _v[0] = 1.0; 
  _v[1] = -0.796341;
  _v[2] = 59.139;
  _v[3] = -3.57411;
  _v[4] = 23.4003;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292835;
  _v[0] = 1.0; 
  _v[1] = 0.28689;
  _v[2] = 31.4963;
  _v[3] = -3.36719;
  _v[4] = 25.146;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292843;
  _v[0] = 1.0; 
  _v[1] = 0.783822;
  _v[2] = 39.8486;
  _v[3] = -1.87587;
  _v[4] = 26.9013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292844;
  _v[0] = 1.0; 
  _v[1] = 0.783822;
  _v[2] = 39.8486;
  _v[3] = -1.87587;
  _v[4] = 26.9013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292845;
  _v[0] = 1.0; 
  _v[1] = 0.783822;
  _v[2] = 39.8486;
  _v[3] = -1.87587;
  _v[4] = 26.9013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292846;
  _v[0] = 1.0; 
  _v[1] = 0.783822;
  _v[2] = 39.8486;
  _v[3] = -1.87587;
  _v[4] = 26.9013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292847;
  _v[0] = 1.0; 
  _v[1] = 0.783822;
  _v[2] = 39.8486;
  _v[3] = -1.87587;
  _v[4] = 26.9013;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292913;
  _v[0] = 1.0; 
  _v[1] = 1.58467;
  _v[2] = 30.7698;
  _v[3] = -5.073;
  _v[4] = 26.8566;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292914;
  _v[0] = 1.0; 
  _v[1] = 1.58467;
  _v[2] = 30.7698;
  _v[3] = -5.073;
  _v[4] = 26.8566;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292915;
  _v[0] = 1.0; 
  _v[1] = 1.58467;
  _v[2] = 30.7698;
  _v[3] = -5.073;
  _v[4] = 26.8566;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292916;
  _v[0] = 1.0; 
  _v[1] = 1.87908;
  _v[2] = 34.1146;
  _v[3] = -4.8778;
  _v[4] = 27.7641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292917;
  _v[0] = 1.0; 
  _v[1] = 1.87908;
  _v[2] = 34.1146;
  _v[3] = -4.8778;
  _v[4] = 27.7641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292922;
  _v[0] = 1.0; 
  _v[1] = -0.309622;
  _v[2] = 38.7402;
  _v[3] = -3.52704;
  _v[4] = 29.0468;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 292997;
  _v[0] = 1.0; 
  _v[1] = 0.77424;
  _v[2] = 36.1157;
  _v[3] = -3.47588;
  _v[4] = 27.595;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293000;
  _v[0] = 1.0; 
  _v[1] = -0.315594;
  _v[2] = 40.6074;
  _v[3] = -2.77109;
  _v[4] = 30.0121;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293016;
  _v[0] = 1.0; 
  _v[1] = -1.07348;
  _v[2] = 32.129;
  _v[3] = -2.1353;
  _v[4] = 25.477;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293019;
  _v[0] = 1.0; 
  _v[1] = -0.571321;
  _v[2] = 39.4019;
  _v[3] = -2.4101;
  _v[4] = 28.8373;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293058;
  _v[0] = 1.0; 
  _v[1] = 0.039436;
  _v[2] = 33.9182;
  _v[3] = -3.60325;
  _v[4] = 25.3078;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293059;
  _v[0] = 1.0; 
  _v[1] = -0.425913;
  _v[2] = 39.1098;
  _v[3] = -1.37865;
  _v[4] = 27.9621;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293083;
  _v[0] = 1.0; 
  _v[1] = -0.127811;
  _v[2] = 35.5486;
  _v[3] = -3.34674;
  _v[4] = 26.0916;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293098;
  _v[0] = 1.0; 
  _v[1] = -0.301476;
  _v[2] = 33.9192;
  _v[3] = -3.26014;
  _v[4] = 25.453;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293108;
  _v[0] = 1.0; 
  _v[1] = 0.0662057;
  _v[2] = 37.9392;
  _v[3] = -2.96415;
  _v[4] = 28.3798;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293176;
  _v[0] = 1.0; 
  _v[1] = -0.697162;
  _v[2] = 30.5926;
  _v[3] = -2.71861;
  _v[4] = 24.793;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293199;
  _v[0] = 1.0; 
  _v[1] = -0.26628;
  _v[2] = 34.0699;
  _v[3] = -3.4906;
  _v[4] = 25.3975;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293213;
  _v[0] = 1.0; 
  _v[1] = -0.666407;
  _v[2] = 37.2643;
  _v[3] = -2.26594;
  _v[4] = 26.1561;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293230;
  _v[0] = 1.0; 
  _v[1] = -0.620383;
  _v[2] = 32.666;
  _v[3] = -3.10465;
  _v[4] = 26.1442;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293231;
  _v[0] = 1.0; 
  _v[1] = -0.137142;
  _v[2] = 37.6021;
  _v[3] = -2.90481;
  _v[4] = 29.092;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293244;
  _v[0] = 1.0; 
  _v[1] = -1.39128;
  _v[2] = 35.1452;
  _v[3] = -1.87443;
  _v[4] = 25.4205;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293260;
  _v[0] = 1.0; 
  _v[1] = -1.06911;
  _v[2] = 30.4218;
  _v[3] = -2.21414;
  _v[4] = 23.4065;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293262;
  _v[0] = 1.0; 
  _v[1] = -0.777794;
  _v[2] = 36.4537;
  _v[3] = -1.90797;
  _v[4] = 26.555;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293274;
  _v[0] = 1.0; 
  _v[1] = 0.383739;
  _v[2] = 32.7249;
  _v[3] = -4.45075;
  _v[4] = 23.4232;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293277;
  _v[0] = 1.0; 
  _v[1] = 0.383739;
  _v[2] = 32.7249;
  _v[3] = -4.45075;
  _v[4] = 23.4232;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293281;
  _v[0] = 1.0; 
  _v[1] = 0.32956;
  _v[2] = 37.3268;
  _v[3] = -3.44412;
  _v[4] = 28.9001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293282;
  _v[0] = 1.0; 
  _v[1] = 0.32956;
  _v[2] = 37.3268;
  _v[3] = -3.44412;
  _v[4] = 28.9001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293283;
  _v[0] = 1.0; 
  _v[1] = 0.32956;
  _v[2] = 37.3268;
  _v[3] = -3.44412;
  _v[4] = 28.9001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293284;
  _v[0] = 1.0; 
  _v[1] = 0.32956;
  _v[2] = 37.3268;
  _v[3] = -3.44412;
  _v[4] = 28.9001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293287;
  _v[0] = 1.0; 
  _v[1] = 0.32956;
  _v[2] = 37.3268;
  _v[3] = -3.44412;
  _v[4] = 28.9001;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293303;
  _v[0] = 1.0; 
  _v[1] = -0.889863;
  _v[2] = 35.9129;
  _v[3] = -2.59437;
  _v[4] = 25.6338;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293326;
  _v[0] = 1.0; 
  _v[1] = -0.540277;
  _v[2] = 34.5551;
  _v[3] = -3.51238;
  _v[4] = 25.9425;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293342;
  _v[0] = 1.0; 
  _v[1] = -0.161922;
  _v[2] = 36.3583;
  _v[3] = -3.41921;
  _v[4] = 26.0086;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293343;
  _v[0] = 1.0; 
  _v[1] = -0.161922;
  _v[2] = 36.3583;
  _v[3] = -3.41921;
  _v[4] = 26.0086;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293349;
  _v[0] = 1.0; 
  _v[1] = 1.14881;
  _v[2] = 42.9522;
  _v[3] = -6.7314;
  _v[4] = 30.0404;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293380;
  _v[0] = 1.0; 
  _v[1] = -0.147951;
  _v[2] = 29.7808;
  _v[3] = -4.68799;
  _v[4] = 25.4524;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293381;
  _v[0] = 1.0; 
  _v[1] = -0.147951;
  _v[2] = 29.7808;
  _v[3] = -4.68799;
  _v[4] = 25.4524;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293382;
  _v[0] = 1.0; 
  _v[1] = -0.147951;
  _v[2] = 29.7808;
  _v[3] = -4.68799;
  _v[4] = 25.4524;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293383;
  _v[0] = 1.0; 
  _v[1] = -1.116;
  _v[2] = 35.3667;
  _v[3] = -1.99912;
  _v[4] = 28.031;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293384;
  _v[0] = 1.0; 
  _v[1] = -0.920394;
  _v[2] = 39.5167;
  _v[3] = -1.42384;
  _v[4] = 28.5827;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293435;
  _v[0] = 1.0; 
  _v[1] = -0.992941;
  _v[2] = 35.6145;
  _v[3] = -2.71953;
  _v[4] = 25.8249;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293446;
  _v[0] = 1.0; 
  _v[1] = -1.33684;
  _v[2] = 32.4763;
  _v[3] = -2.08006;
  _v[4] = 25.3754;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293447;
  _v[0] = 1.0; 
  _v[1] = 0.0248115;
  _v[2] = 36.366;
  _v[3] = -3.31023;
  _v[4] = 28.4991;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293463;
  _v[0] = 1.0; 
  _v[1] = 0.135079;
  _v[2] = 27.1827;
  _v[3] = -3.90336;
  _v[4] = 25.7602;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293527;
  _v[0] = 1.0; 
  _v[1] = -0.377209;
  _v[2] = 30.1054;
  _v[3] = -2.89478;
  _v[4] = 24.6641;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293529;
  _v[0] = 1.0; 
  _v[1] = -0.860839;
  _v[2] = 34.8737;
  _v[3] = -2.24309;
  _v[4] = 28.0367;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293532;
  _v[0] = 1.0; 
  _v[1] = -0.860839;
  _v[2] = 34.8737;
  _v[3] = -2.24309;
  _v[4] = 28.0367;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293534;
  _v[0] = 1.0; 
  _v[1] = -0.860839;
  _v[2] = 34.8737;
  _v[3] = -2.24309;
  _v[4] = 28.0367;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293549;
  _v[0] = 1.0; 
  _v[1] = -0.796878;
  _v[2] = 30.5869;
  _v[3] = -3.59643;
  _v[4] = 24.1352;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293551;
  _v[0] = 1.0; 
  _v[1] = 0.325399;
  _v[2] = 34.6952;
  _v[3] = -4.39944;
  _v[4] = 26.2648;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293552;
  _v[0] = 1.0; 
  _v[1] = 0.325399;
  _v[2] = 34.6952;
  _v[3] = -4.39944;
  _v[4] = 26.2648;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293557;
  _v[0] = 1.0; 
  _v[1] = 0.468692;
  _v[2] = 38.3584;
  _v[3] = -3.75746;
  _v[4] = 28.9602;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293558;
  _v[0] = 1.0; 
  _v[1] = 0.468692;
  _v[2] = 38.3584;
  _v[3] = -3.75746;
  _v[4] = 28.9602;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293559;
  _v[0] = 1.0; 
  _v[1] = 0.468692;
  _v[2] = 38.3584;
  _v[3] = -3.75746;
  _v[4] = 28.9602;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293571;
  _v[0] = 1.0; 
  _v[1] = -0.410893;
  _v[2] = 35.5712;
  _v[3] = -3.77636;
  _v[4] = 26.0501;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293572;
  _v[0] = 1.0; 
  _v[1] = 0.3189;
  _v[2] = 38.3625;
  _v[3] = -3.45471;
  _v[4] = 29.8549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293589;
  _v[0] = 1.0; 
  _v[1] = -0.639121;
  _v[2] = 33.6416;
  _v[3] = -3.18257;
  _v[4] = 25.8281;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293590;
  _v[0] = 1.0; 
  _v[1] = -1.0229;
  _v[2] = 42.274;
  _v[3] = -2.88203;
  _v[4] = 28.371;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293591;
  _v[0] = 1.0; 
  _v[1] = -1.0229;
  _v[2] = 42.274;
  _v[3] = -2.88203;
  _v[4] = 28.371;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293592;
  _v[0] = 1.0; 
  _v[1] = -1.0229;
  _v[2] = 42.274;
  _v[3] = -2.88203;
  _v[4] = 28.371;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293628;
  _v[0] = 1.0; 
  _v[1] = -0.454054;
  _v[2] = 31.8631;
  _v[3] = -2.89017;
  _v[4] = 25.1169;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293648;
  _v[0] = 1.0; 
  _v[1] = 0.30401;
  _v[2] = 33.1321;
  _v[3] = -4.11559;
  _v[4] = 25.7705;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293650;
  _v[0] = 1.0; 
  _v[1] = 1.88008;
  _v[2] = 37.8684;
  _v[3] = -5.03975;
  _v[4] = 28.196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293655;
  _v[0] = 1.0; 
  _v[1] = 1.88008;
  _v[2] = 37.8684;
  _v[3] = -5.03975;
  _v[4] = 28.196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293656;
  _v[0] = 1.0; 
  _v[1] = 1.88008;
  _v[2] = 37.8684;
  _v[3] = -5.03975;
  _v[4] = 28.196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293657;
  _v[0] = 1.0; 
  _v[1] = 1.88008;
  _v[2] = 37.8684;
  _v[3] = -5.03975;
  _v[4] = 28.196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293658;
  _v[0] = 1.0; 
  _v[1] = 1.88008;
  _v[2] = 37.8684;
  _v[3] = -5.03975;
  _v[4] = 28.196;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293675;
  _v[0] = 1.0; 
  _v[1] = 0.21619;
  _v[2] = 30.7538;
  _v[3] = -2.64797;
  _v[4] = 23.0431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293676;
  _v[0] = 1.0; 
  _v[1] = 0.21619;
  _v[2] = 30.7538;
  _v[3] = -2.64797;
  _v[4] = 23.0431;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293679;
  _v[0] = 1.0; 
  _v[1] = -1.10097;
  _v[2] = 34.8347;
  _v[3] = -2.63904;
  _v[4] = 26.7313;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293682;
  _v[0] = 1.0; 
  _v[1] = 0.835222;
  _v[2] = 38.2151;
  _v[3] = -3.78628;
  _v[4] = 28.7296;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293700;
  _v[0] = 1.0; 
  _v[1] = 0.0901047;
  _v[2] = 29.7353;
  _v[3] = -3.96255;
  _v[4] = 24.7794;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293701;
  _v[0] = 1.0; 
  _v[1] = -1.44206;
  _v[2] = 32.6671;
  _v[3] = -2.05948;
  _v[4] = 26.7632;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293704;
  _v[0] = 1.0; 
  _v[1] = -1.44206;
  _v[2] = 32.6671;
  _v[3] = -2.05948;
  _v[4] = 26.7632;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293705;
  _v[0] = 1.0; 
  _v[1] = 0.89706;
  _v[2] = 36.5851;
  _v[3] = -3.92436;
  _v[4] = 26.5299;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293720;
  _v[0] = 1.0; 
  _v[1] = -0.598993;
  _v[2] = 33.8043;
  _v[3] = -2.79141;
  _v[4] = 26.6392;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293721;
  _v[0] = 1.0; 
  _v[1] = 1.28576;
  _v[2] = 45.8062;
  _v[3] = -6.00658;
  _v[4] = 22.1959;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293770;
  _v[0] = 1.0; 
  _v[1] = 1.4533;
  _v[2] = 29.5725;
  _v[3] = -5.67044;
  _v[4] = 24.0549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293771;
  _v[0] = 1.0; 
  _v[1] = 1.4533;
  _v[2] = 29.5725;
  _v[3] = -5.67044;
  _v[4] = 24.0549;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293772;
  _v[0] = 1.0; 
  _v[1] = 0.306103;
  _v[2] = 36.7974;
  _v[3] = -3.83563;
  _v[4] = 26.9696;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293773;
  _v[0] = 1.0; 
  _v[1] = 0.306103;
  _v[2] = 36.7974;
  _v[3] = -3.83563;
  _v[4] = 26.9696;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293775;
  _v[0] = 1.0; 
  _v[1] = 0.306103;
  _v[2] = 36.7974;
  _v[3] = -3.83563;
  _v[4] = 26.9696;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293797;
  _v[0] = 1.0; 
  _v[1] = -0.424826;
  _v[2] = 30.1976;
  _v[3] = -3.25421;
  _v[4] = 23.7011;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293798;
  _v[0] = 1.0; 
  _v[1] = -0.422488;
  _v[2] = 36.484;
  _v[3] = -3.08632;
  _v[4] = 27.3626;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));

  loEdge = 293800;
  _v[0] = 1.0; 
  _v[1] = -0.422488;
  _v[2] = 36.484;
  _v[3] = -3.08632;
  _v[4] = 27.3626;
  _dLdzMap.insert(parMap::value_type(loEdge, _v));


  if (_debug) {
    _v.clear();
    parMap::iterator iter;
    vector<double>::iterator vIter;
    std::cout << std::endl;
    std::cout << "  Contents of Vz Run Range Map" << std::endl
	      << "  ----------------------------" << std::endl;
    std::cout.width(8);
    std::cout << std::setprecision(6);
    for (iter = _dLdzMap.begin();
	 iter != _dLdzMap.end(); ++iter) {
      _v = iter->second;
      std::cout << iter->first << " --> ";
      for (vIter = _v.begin();
	   vIter != _v.end(); ++vIter) {
	std::cout << *vIter << "  ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  _v.clear();
  return 0;
}
