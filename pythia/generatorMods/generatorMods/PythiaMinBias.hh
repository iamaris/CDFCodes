 //--------------------------------------------------------------------------
// File and Version Information:
// 	PythiaMinBias.hh
//
// Description:
//	MinBias module based on PYTHIA
//
// Environment:
//	CDF Run 2
//
// Author List: Elena Gerchtein CMU
//

#ifndef PYTHIAMINBIAS_HH
#define PYTHIAMINBIAS_HH



#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "generatorMods/AbsGenModule.hh"
#include "generatorMods/PythiaMenu.hh"

#include <string>

class PythiaMinBias : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;
  PythiaMinBias();
  virtual ~PythiaMinBias( );

  void _initializeTalkTo();

  // Inherited functions
  AppResult genBeginJob( );
  AppResult genBeginRun(AbsEvent* aRun);
  AppResult genEndRun(AbsEvent* aRun);
  AppResult genEndJob( );
  int callGenerator(AbsEvent* anEvent);
  void fillHepevt();
  // min bias event solution - this is called in fillHepevt
  void        writeHEPGbank(); 

private:

  PythiaMenu* _pythiaMenu;

protected:

private:
  PythiaMinBias( const char* const theName, 
		const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
      _pythiaMenu()
  {}

  PythiaMinBias( const PythiaMinBias& m)
    : AbsGenModule( "x", "x"),
      _pythiaMenu()
  {}

};
#endif
