#ifndef _MCFM_MODULE_HH
#define _MCFM_MODULE_HH

// Declarations for Fortran routines usesed by this class
extern "C" {
  // Note the double underscore at the end. Not sure why
  // this is : something to do with how mcfm library is 
  // being compiled (separate .o files for each .f file).
  void mcfm_cdf_init_();
  void mcfm_cdf_setseed_(int rndm_seed[]);
  void mcfm_cdf_vegas_prepare_();
  void mcfm_cdf_getevent_();
  void mcfm_cdf_stdhep_();
  void mcfm_cdf_exit_();
}

//----------------------
// Base Class Headers --
//----------------------
#include "Framework/APPModule.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmFilename.hh"
#include "Framework/APPMenu.hh"
#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif
//		---------------------
// 		-- Class Interface --
//		---------------------

class MCFM_Module : public AbsGenModule {

//--------------------
// Instance Members --
//--------------------

public:

  static const char* genId;

  // Constructors
  MCFM_Module();
  
 // Destructor
  virtual ~MCFM_Module();
  
  // Operations
  AppResult  genBeginJob  ();
  AppResult  genBeginRun  (AbsEvent* run);
  AppResult  genEndJob    ();
  AppResult  genEndRun    (AbsEvent* run );
  int        callGenerator(AbsEvent* event);
  void fillHepevt();

private:

  // Debug flag :
  AbsParmBool _debug;

  // Use Pythia ?
  AbsParmBool _usePythia;

  PythiaMenu* _pythiaMenu;

  // Set random number seed :
  AbsParmGeneral<int> _rndmSeed;


private:

  MCFM_Module(const char* name, 
	      const char* description) :
    AbsGenModule( MCFM_Module::genId, 
		  "MCFM"),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _pythiaMenu(),
    _rndmSeed("randomSeed",this,1089)
  {}

  MCFM_Module(const MCFM_Module& m)
    :
    AbsGenModule( "x", "x" ),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _pythiaMenu(),
    _rndmSeed("randomSeed",this,1089)
  {}
  
};

#endif

