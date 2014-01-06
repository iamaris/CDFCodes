//--------------------------------------------------------------------------
// File and Version Information:
// 	GrappaModule.hh
//
// Description:
//	Class definition for GrappaModule, the main interface
//
// Environment:
//	CDF Run 2
//
// Author List:
//      Elena Gerchtein CMU
//	Soushi Tsuno (tsuno@fnal.gov) Tsukuba Univ.
//
// Copyright Information:
//
//------------------------------------------------------------------------

#ifndef GRAPPAMODULETEST_HH
#define GRAPPAMODULETEST_HH

//----------------------
// Base Class Headers --
//----------------------
#include "Framework/APPMenu.hh"
#include "Framework/APPModule.hh"
#include "Framework/AbsParmList.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"

#include <string>

//------------------------------------------
// Fortran routines called from this code --
//--------------------------------------------
extern "C" {
  void grcgive_(int*, int*, int*, double*, double*, double*);
  void grcgivec_(char*, int*, int*);
  void grcpygen_(const char*, int*, int*, double*);
  void grcendjob_(int*);
}


//		---------------------
// 		-- Class Interface --
//		---------------------
class GrappaModule : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;
  
  //
  // Constructors
  //
  GrappaModule();

public:
  //
  // Destructor
  //
  virtual ~GrappaModule( );

  //
  // Operations
  //

  void _initializeTalkTo();

  // Inherited functions
  AppResult genBeginJob( );
  AppResult genBeginRun(AbsEvent* aRun);
  AppResult genEndRun(AbsEvent* aRun);
  AppResult genEndJob( );
  int callGenerator(AbsEvent* anEvent);
  void fillHepevt();

private:

  AbsParmGeneral<double>      _grcecm;
  AbsParmGeneral<std::string> _grcpap;

  AbsParmGeneral<double>      _gptcut;
  AbsParmGeneral<double>      _getacut;
  AbsParmGeneral<double>      _grconcut;

  AbsParmGeneral<double>      _hmas;
  AbsParmGeneral<double>      _hwid;
  AbsParmGeneral<double>      _tmas;
  AbsParmGeneral<double>      _twid;
  AbsParmGeneral<double>      _bmas;
  AbsParmGeneral<double>      _cmas;

  AbsParmList<int>            _ijetflv;

  APPMenu _subprocessMenu;
  AbsParmList<int>            _igsub;
  AbsParmList<int>            _incall;
  AbsParmList<int>            _ibswrt;
  AbsParmList<int>            _inpfl;
  AbsParmList<int>            _icoup;
  AbsParmList<int>            _ifact;
  AbsParmList<int>            _isdep;
  AbsParmList<double>         _grcq;
  AbsParmList<double>         _grcfaq;
  AbsParmList<std::string>    _grcfile;

  //*  AbsParmGeneral<bool> _useTAUOLA;

  PythiaMenu* _pythiaMenu;

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;

protected:

private:
  GrappaModule( const char* const theName, 
		const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
      _grcecm("CMEnergy",  this, 1960.),
      _grcpap("Beam",      this, "PAP"),
      _gptcut("gPtcut",    this,   5.0),
      _getacut("gEtacut",  this,   3.0),
      _grconcut("gRconcut",this,   0.1),
      _hmas("hmass",       this, 120.0),
      _hwid("hwidth",      this,   6.54e-3),
      _tmas("tmass",       this, 175.0),
      _twid("twidth",      this,   1.59),
      _bmas("bmass",       this,   4.8),
      _cmas("cmass",       this,   1.5),
      _ijetflv("JetFlv",   this,0, 8,    1,  0,   1),
      _igsub("iGsub",      this,0,50,  401,400, 450),
      _incall("iNcall",    this,0,50,100000,0,9999999),
      _ibswrt("iBswrt",    this,0,50,    0,  0,   1),
      _inpfl("iNpfl",      this,0,50,    4,  1,   5),
      _icoup("iCoup",      this,0,50,    1,  1,   6),
      _ifact("iFact",      this,0,50,    0,  0,   6),
      _isdep("iSdep",      this,0,50,    0,  0,   1),
      _grcq("GrcQ",        this,0,50,  0.0,0.0,9999.0),
      _grcfaq("GrcfaQ",    this,0,50,  0.0,0.0,9999.0),
      _grcfile("GrcFile",  this,0,50,"bases"),
      //*      _useTAUOLA("useTAUOLA",this,false),
      _pythiaMenu(),
      _randomNumberMenu(),
      _randomSeed1("RandomSeed1",this,GrappaModule::_defaultRandomSeed1),
      _randomSeed2("RandomSeed2",this,GrappaModule::_defaultRandomSeed2)
  {}

  GrappaModule( const GrappaModule& m)
    : AbsGenModule( "x", "x"),
      _grcecm("CMEnergy",  this, 1960.),
      _grcpap("Beam",      this, "PAP"),
      _gptcut("gPtcut",    this,   5.0),
      _getacut("gEtacut",  this,   3.0),
      _grconcut("gRconcut",this,   0.1),
      _hmas("hmass",       this, 120.0),
      _hwid("hwidth",      this,   6.54e-3),
      _tmas("tmass",       this, 175.0),
      _twid("twidth",      this,   1.59),
      _bmas("bmass",       this,   4.8),
      _cmas("cmass",       this,   1.5),
      _ijetflv("JetFlv",  this,0, 8,    1,  0,   1),
      _igsub("iGsub",      this,0,50,  401,400, 450),
      _incall("iNcall",    this,0,50,100000,0,9999999),
      _ibswrt("iBswrt",    this,0,50,    0,  0,   1),
      _inpfl("iNpfl",      this,0,50,    4,  1,   5),
      _icoup("iCoup",      this,0,50,    1,  1,   6),
      _ifact("iFact",      this,0,50,    0,  0,   6),
      _isdep("iSdep",      this,0,50,    0,  0,   1),
      _grcq("GrcQ",        this,0,50,  0.0,0.0,9999.0),
      _grcfaq("GrcfaQ",    this,0,50,  0.0,0.0,9999.0),
      _grcfile("GrcFile",  this,0,50,"bases"),
      //*      _useTAUOLA("useTAUOLA",this,false),
      _pythiaMenu(),
      _randomNumberMenu(),
      _randomSeed1("RandomSeed1",this,GrappaModule::_defaultRandomSeed1),
      _randomSeed2("RandomSeed2",this,GrappaModule::_defaultRandomSeed2)
  {}


};
#endif
