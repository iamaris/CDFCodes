 //--------------------------------------------------------------------------
// File and Version Information:
// 	Pythia_main.hh
//
// Description:
//	Class definition for PythiaModule, the main interface
//	to PYTHIA 6.1 generator.
//
// Environment:
//	CDF Run 2
//
// Author List:
//  Chris Green, Purdue University
//	Stan Thompson, Glasgow University    
//
// Copyright Information:
//
// rev. aug 30 2001 lena: added genId and ct.; removed include of GeneratorModule;
// rev. nov 26 2001 lena: pythia talk-tos moved to PythiaMenu class to be shared;
//                        pythia common blocks are accessed through pythia
//                        singleton from pythia_i.         
     
//------------------------------------------------------------------------

#ifndef PYTHIAMODULE_HH
#define PYTHIAMODULE_HH



#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"

#include <string>

//		---------------------
// 		-- Class Interface --
//		---------------------


class PythiaModule : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;
  
  //
  // Constructors
  //
  PythiaModule();


public:
  //
  // Destructor
  //
  virtual ~PythiaModule( );

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

protected:

private:
  PythiaModule( const char* const theName, 
		const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
      _pythiaMenu()
  {}

  PythiaModule( const PythiaModule& m)
    : AbsGenModule( "x", "x"),
      _pythiaMenu()
  {}


};
#endif
