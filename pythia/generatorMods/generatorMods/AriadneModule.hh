 //--------------------------------------------------------------------------
// File and Version Information:
// 	
//
// Description:
//	Class definition for AriadneModule, the main interface
//	to ARIADNE 4.12 generator.
//
// Environment:
//	CDF Run 2
//
// Author List:
//	Stan Thompson, Glasgow University    
//
// Copyright Information:
//     
//------------------------------------------------------------------------

#ifndef ARIADNEMODULE_HH
#define ARIADNEMODULE_HH

#include "Framework/APPMenu.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmEnum.hh"
#include "Framework/AbsParmFilename.hh"

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/AriadneSetCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"

#include <string>

//		---------------------
// 		-- Class Interface --
//		---------------------


class AriadneModule : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;
  
  //
  // Constructors
  //
  AriadneModule();


public:
  //
  // Destructor
  //
  virtual ~AriadneModule( );

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
  //  void hepevt2Hepg(AbsEvent* event, int next);

private:

  PythiaMenu* _pythiaMenu;

  AbsParmGeneral<std::string> _initMode;
  AbsParmGeneral<std::string> _tune;
  AbsParmGeneral<float>       _popaTotsig;
  AbsParmGeneral<float>       _popaPpow;

  SetMstaCommand _setMsta;
  SetParaCommand _setPara;
  SetPqmasCommand _setPqmas;
  SetCaCommand _setCa;
  SetPbCommand _setPb;
  SetCfCommand _setCf;
  SetXaCommand _setXa;
  SetNbCommand _setNb;
  

protected:

private:
  AriadneModule( const char* const theName, 
		const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
      _initMode("associateProgram",this,"PYTHIA"),
      _tune("parameterTune",this,"4.12"),
      _popaTotsig("poweronTotSigma",this,2.3),
      _popaPpow("pomeronExponent",this,2.),
      _setMsta("set_msta",this),
      _setPara("set_para",this),
      _setPqmas("set_pqmas",this),
      _setCa("set_ca",this),
      _setPb("set_pb",this),
      _setCf("set_cf",this),
      _setXa("set_xa",this),
      _setNb("set_nb",this),
      _pythiaMenu()
  {}

  AriadneModule( const AriadneModule& m)
    : AbsGenModule( "x", "x"),
      _initMode("associateProgram",this,"PYTHIA"),
      _tune("parameterTune",this,"4.12"),
      _popaTotsig("poweronTotSigma",this,2.3),
      _popaPpow("pomeronExponent",this,2.),
      _setMsta("set_msta",this),
      _setPara("set_para",this),
      _setPqmas("set_pqmas",this),
      _setCa("set_ca",this),
      _setPb("set_pb",this),
      _setCf("set_cf",this),
      _setXa("set_xa",this),
      _setNb("set_nb",this),
      _pythiaMenu()
  {}


};
#endif
