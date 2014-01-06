//--------------------------------------------------------------------------
// File and Version Information:
//      FakeEv.hh
//
// Description:
//	Class FakeEv. This class generates a fake track or tracks
//      and adds the track(s) to either the GENP or HEPG banks.
//
// Environment:
//	Software developed for the CDF Detector at FNAL.
//
// Author List:
//	Marjorie Shapiro               LBL
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//
//------------------------------------------------------------------------
#ifndef __FAKEEV_HH__
#define __FAKEEV_HH__

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream>
#include <string>
#include "Framework/AbsParmFilename.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmList.hh"
#include "generatorMods/AbsGenModule.hh"

class APPCommand;
class APPMenu;
class AppResult;
class AbsEvent;
class FakeEv;

typedef AbsParmGeneral<long> AbsParmGeneral_long;

class FakeEvCommand : public APPCommand {
public:
  FakeEvCommand();
  FakeEvCommand(const char* name, FakeEv* target);
  virtual ~FakeEvCommand();
				// Handler
  virtual int    handle(int argc, char* argv[]);
  virtual void   show() const;
  virtual bool   isShowable() const;
  virtual std::string description () const;

protected:
  const FakeEv*  _thisModule;
};

class FakeEv : public AbsGenModule {
public:
  // Small class in which to save generation parameters for momentum,
  // eta, phi, etc, etc. Done to minimize code changes to FakeEv module.
  class GenPars {
  public:
    ~GenPars() {};
    GenPars() {};
    GenPars(float mean, float sigma, 
            float min, float max, float power, int mode)
      : 
      _mean(mean),
      _sigma(sigma),
      _min(min),
      _max(max),
      _power(power),
      _mode(mode)
      {
      }

    GenPars(const GenPars& right) 
      :
      _mean(right._mean),
      _sigma(right._sigma),
      _min(right._min),
      _max(right._max),
      _power(right._power),
      _mode(right._mode)
      {
      }
    
    inline void init(float mean, float sigma, 
                     float min, float max, 
                     float power, int mode) {
      setMean(mean);
      setSigma(sigma);
      setMin(min);
      setMax(max);
      setPower(power);
      setMode(mode);
    }

    inline void setMean(float mean) {_mean=mean;}
    inline void setSigma(float sigma) {_sigma=sigma;}
    inline void setMin(float min) {_min=min;}
    inline void setMax(float max) {_max=max;}
    inline void setPower(float power) {_power=power;}
    inline void setMode(int mode) {_mode=mode;}

    inline float mean(void) const {return _mean;}
    inline float sigma(void) const {return _sigma;}
    inline float min(void) const {return _min;}
    inline float max(void) const {return _max;}
    inline float power(void) const {return _power;}
    inline int mode(void) const {return _mode;}

    void print(std::ostream& os=std::cout) const;

  private:
    float _mean;
    float _sigma;
    float _min;
    float _max;
    float _power;
    int _mode;
  };

private:
  enum PolarAngleChoice { NOCHOICE = -1,
			  USE_THETA = 0,
			  USE_ETA   = 1,
			  USE_Y = 2 };
 

protected:

  int             NParticles;	// Number of particles to generate
  int             CdfCode;	// CDF code of particles generated
  bool            AddFakeEv;	// Turn on or off Fake generation
  bool            UsePt;	// Switch between Pt and Momentum generation
  bool            useMass;	// Switch to smear the mass of the generated particles
  PolarAngleChoice   PolarChoice;	// Switch between Eta/Theta/Rapidity generation
  FakeEvCommand   _useCommand;
  FakeEvCommand   _genCommand;

  // Random number menu
  APPMenu _randomNumberMenu;

  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

					// probability to generate antiparticle
					// default = 0
  AbsParmGeneral<float> _antiParticle;

  // Pt hist menu
  AbsParmGeneral<bool> _usePtHist;
  APPMenu _PtHistMenu;
  AbsParmGeneral<double> _lowPt;
  AbsParmGeneral<double> _highPt;
  AbsParmGeneral<int> _nbinPt;
  AbsParmList<double> _valPt;
  double generatePtUsingHist();
  double _myValPt[200];

  void _initializeRandomTalkTo(void);
  void _initializeUsePtHistTalkTo(void);

  GenPars  _genMass;	// Parameters for Generating Mass
  GenPars  _genMom;	// Parameters for Generating Momentum
  GenPars  _genPt;	// Parameters for Generating Pt
  GenPars  _genTheta;	// Parameters for Generating Theta
  GenPars  _genEta;	// Parameters for Generating Eta
  GenPars  _genPhi;	// Parameters for Generating Phi
  GenPars  _genY;	// Parameters for Generating Rapidity

public:
  static const char* genId;

  FakeEv();
  virtual    ~FakeEv( );

  friend class FakeEvCommand;

  AppResult  genBeginJob();
  AppResult  genBeginRun( AbsEvent* run);
  AppResult  genEndJob  ();
  AppResult  genEndRun  ( AbsEvent* run);
  int        callGenerator(AbsEvent* event);
  void       fillHepevt();

  void        menuHandler(char* menu, char* command, int np, char** par);

  GenPars*  genMass   () { return &_genMass;  }
  GenPars*  genMom    () { return &_genMom;   }
  GenPars*  genPt     () { return &_genPt;    }
  GenPars*  genTheta  () { return &_genTheta; }
  GenPars*  genEta    () { return &_genEta;   }
  GenPars*  genPhi    () { return &_genPhi;   }
  GenPars*  genY    ()   { return &_genY;   }

  float generateValue(const GenPars& pars) const;

  bool             getCdfCode() { return CdfCode;   }
  int              nParticles() { return NParticles; }

					// ****** set methods

  void  setUsePt   (int use_pt ) { UsePt  = use_pt;  }
  void  setUseTheta       (void) { PolarChoice = USE_THETA; }
  void  setUseEta         (void) { PolarChoice = USE_ETA; }
  void  setUseY           (void) { PolarChoice   = USE_Y;   }
  void  setUseMass (bool use_mass) { useMass = use_mass;   }
  inline bool  useY       (void) {  return (PolarChoice == USE_Y); }
  inline bool  useEta     (void) {  return (PolarChoice == USE_ETA); }
  inline bool  useTheta   (void) {  return (PolarChoice == USE_THETA); }
  void  setCdfCode    (int code) { CdfCode = code; }
  void  setNParticles   (int np) { NParticles = np; }

  void  Set(char* name, float mean, float sigma, float pmax, float pmin , 
	    float power, int mode);

					// ****** print methods

  void        PrintState(std::ostream& output = std::cout) const;
};

#endif

