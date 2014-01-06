//--------------------------------------------------------------------------
// File and Version Information:
//      FakeEv.hh
//
// Description:
//      Class FakeEv. This class generates a fake track or tracks
//      and adds the track(s) to either the GENP or HEPG banks.
//
// Environment:
//      Software developed for the CDF Detector at FNAL.
//
// Author List:
//      Marjorie Shapiro               LBL
//
// Modification History:
//      Jan 19, 1998: M. Shapiro  Make this and Input Module
//      May 08, 1998: M. Shapiro  Add Talk-to on main branch
//      Aug. 29 2001: lena   remove __CINT__; other fixes
// Converted to ZOOM CLHEP random number use 1999/01/25 CG
//
// Copyright Information:
//      Copyright (C) 1997              Lawrence Berkeley Laboratory
//
//------------------------------------------------------------------------
#ifndef __FAKEEV_HH__
#define __FAKEEV_HH__

#ifdef __GNUG__
#pragma interface
#endif

//----------------------
// Base Class Headers --
//----------------------
#include <iostream>
#include <string>

class APPCommand;
class APPMenu;
class AppResult;
class HepRandomEngine;

#include "Framework/AbsParmFilename.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmBool.hh"


typedef AbsParmGeneral<long> AbsParmGeneral_long;

#include "generatorMods/AbsGenModule.hh"

// CHLEP Random number classes
//class HepRandomEngine;
//class RandFlat;
//class RandGauss;

// Related class declarations:
class AbsEvent;
class MixFakeEv;

class MixFakeEvCommand : public APPCommand {
public:
                                // Constructors and destructor
  MixFakeEvCommand();
  MixFakeEvCommand(const char* name, MixFakeEv* target);
  virtual ~MixFakeEvCommand();
                                // Handler
  virtual int    handle(int argc, char* argv[]);
  virtual void   show() const;
  virtual bool   isShowable() const;
  virtual std::string description () const;

protected:
  const MixFakeEv*  _thisModule;
};

class MixFakeEv : public AbsGenModule {
//------------------------------------------------------------------------------
// Data Members
//------------------------------------------------------------------------------
public:
  // Small class in which to save generation parameters for momentum,
  // eta, phi, etc, etc. Done to minimize code changes to MixFakeEv module.
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

protected:
  int             NParticles;   // Number of particles to generate
  int             CdfCode;      // CDF code of particles generated
  bool            AddFakeEv;    // Turn on or off Fake generation
  bool            UsePt;        // Switch between Pt and Momentum generation
  bool            UseEta;       // Switch between Eta and Theta generation
  MixFakeEvCommand   _useCommand;
  MixFakeEvCommand   _genCommand;

  // Random number menu
  APPMenu _randomNumberMenu;

  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

  // Add options to menu, describe options, etc, etc.
  void _initializeRandomTalkTo(void);

  GenPars  _genMom;     // Parameters for Generating Momentum
  GenPars  _genPt;      // Parameters for Generating Pt
  GenPars  _genTheta;   // Parameters for Generating Theta
  GenPars  _genEta;     // Parameters for Generating Eta
  GenPars  _genPhi;     // Parameters for Generating Phi

public:
  static const char* genId;
  static HepRandomEngine* mixFakeEvEngine;
                                        // ****** constructors and destructor
  MixFakeEv();

  virtual    ~MixFakeEv( );

  friend class MixFakeEvCommand;

  // Framework begin and end job methods:

  AppResult  genBeginJob();
  AppResult  genBeginRun( AbsEvent* run);
  AppResult  genEndJob  ();
  AppResult  genEndRun  ( AbsEvent* run);
  int        callGenerator(AbsEvent* event);
  void       fillHepevt();
//dt
//  void        writeHEPGbank(); 
//dt

  void        menuHandler(char* menu, char* command, int np, char** par);

                                        // ****** accessor functions

  GenPars*  genMom    () { return &_genMom;   }
  GenPars*  genPt     () { return &_genPt;    }
  GenPars*  genTheta  () { return &_genTheta; }
  GenPars*  genEta    () { return &_genEta;   }
  GenPars*  genPhi    () { return &_genPhi;   }

  float generateValue(const GenPars& pars) const;


  bool             getCdfCode() { return CdfCode;   }
  int              nParticles() { return NParticles; }

                                        // ****** set methods

  void  setUsePt   (int use_pt ) { UsePt  = use_pt;  }
  void  setUseEta  (int use_eta) { UseEta = use_eta; }
  void  setCdfCode    (int code) { CdfCode = code; }
  void  setNParticles   (int np) { NParticles = np; }

  void  Set(char* name, float mean, float sigma, float pmax, float pmin , 
            float power, int mode);

                                        // ****** print methods

  void        PrintState(std::ostream& output = std::cout) const;

private: 
  MixFakeEv(const char* name, const char* description)  :
    AbsGenModule( name, description ),
    _useCommand("use",this),
    _genCommand("generate",this),
    _randomSeed1("RandomSeed1",this,MixFakeEv::_defaultRandomSeed1),
    _randomSeed2("RandomSeed2",this,MixFakeEv::_defaultRandomSeed2),
    _genMom(50.,0.,  1.,100.,0.,1), // default parameters
    _genPt(50.,0.,  1.,100.,0.,1),
    _genTheta(90.,0.,-45., 45.,0.,1),
    _genEta(0.0,0., -1.,  1.,0.,2),
    _genPhi(7.5,0.,  0.,360.,0.,2)
  {}
  
  
  MixFakeEv(const MixFakeEv&)  :
    AbsGenModule( "x", "x" ),
    _useCommand("use",this),
    _genCommand("generate",this),
    _randomSeed1("RandomSeed1",this,MixFakeEv::_defaultRandomSeed1),
    _randomSeed2("RandomSeed2",this,MixFakeEv::_defaultRandomSeed2),
    _genMom(50.,0.,  1.,100.,0.,1), // default parameters
    _genPt(50.,0.,  1.,100.,0.,1),
    _genTheta(90.,0.,-45., 45.,0.,1),
    _genEta(0.0,0., -1.,  1.,0.,2),
    _genPhi(7.5,0.,  0.,360.,0.,2)
  {}

};

#endif

