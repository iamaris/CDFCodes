//--------------------------------------------------------------------------
// File and Version Information:
// 	LesHouchesModule.hh
//
// Description:
//	Class definition for LesHouchesModule, the main interface
//
// Environment:
//	CDF Run 2
//
// Author List:
//	Soushi Tsuno (tsuno@fnal.gov) Tsukuba Univ.
//
// Copyright Information:
//
//------------------------------------------------------------------------

#ifndef LESHOUCHESMODULE_HH
#define LESHOUCHESMODULE_HH

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

#include "generatorMods/HerwigMenu.hh"

#include <string>

//		---------------------
// 		-- Class Interface --
//		---------------------
class LesHouchesModule : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;

  //
  // Constructors
  //
  LesHouchesModule();

public:
  //
  // Destructor
  //
  virtual ~LesHouchesModule( );

  //
  // Operations
  //

  // Inherited functions
  AppResult genBeginJob( );
  AppResult genBeginRun(AbsEvent* aRun);
  AppResult genEndRun(AbsEvent* aRun);
  AppResult genEndJob( );
  int callGenerator(AbsEvent* anEvent);
  void fillHepevt();
  void Upinit();
  void Upevnt();

  static LesHouchesModule* Instance();

  void setLesHouchesUid( std::string uid ) { _uid = uid; }
  std::string getLesHouchesUid() const { return _uid; }

private:

  AbsParmGeneral<std::string> _lesgentype;
  AbsParmGeneral<std::string> _lesdatfile;
  AbsParmGeneral<std::string> _lespsmodel;
  AbsParmGeneral<std::string> _stddatfile;

  AbsParmGeneral<int>  _firstEventToRead;
  AbsParmGeneral<int>  _putResonance;
  AbsParmList<int>     _skipResonance;
  AbsParmGeneral<bool> _scaleOfKtClus;
  AbsParmGeneral<bool> _useTAUOLA;
  AbsParmGeneral<int>  _useSTDHEP;
  AbsParmGeneral<int>  _useStdEvent;

  PythiaMenu* _pythiaMenu;
  HerwigMenu* _herwigMenu;

  static LesHouchesModule* _instance;
  struct Cleaner { ~Cleaner(); };

  friend struct Cleaner;

  // LesHouches "user" id - to be used in user defined calls
  std::string _uid;

  // event counter
  int _events;

  // for STDHEP
  int _mxevt;

protected:

private:

  LesHouchesModule( const char* const theName, 
		    const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
      _lesgentype("LesGenType", this, "Grappa"),
      _lesdatfile("LesDatFile", this, "bases"),
      _lespsmodel("LesPSModel", this, "PYTHIA"),
      _firstEventToRead("firstEventToRead", this, 1),
      _putResonance("putResonance", this, 1),
      _skipResonance("skipResonance", this,0,6,0,-30,30),
      _scaleOfKtClus("scaleOfKtClus", this, false),
      _useTAUOLA("useTAUOLA",this,false),
      _useSTDHEP("useStdhep",this,0),
      _stddatfile("StdhepFileName", this, "stdhep.dat"),
      _useStdEvent("NumOfStdEvent", this, 10),
      _pythiaMenu(),
      _herwigMenu()
  {}

  LesHouchesModule( const LesHouchesModule& m)
    : AbsGenModule( "x", "x"),
      _lesgentype("LesGenType", this, "Grappa"),
      _lesdatfile("LesDatFile", this, "bases"),
      _lespsmodel("LesPSModel", this, "PYTHIA"),
      _firstEventToRead("firstEventToRead", this, 1),
      _putResonance("putResonance", this, 1),
      _skipResonance("skipResonance", this,0,6,0,-30,30),
      _scaleOfKtClus("scaleOfKtClus", this, false),
      _useTAUOLA("useTAUOLA",this,false),
      _useSTDHEP("useStdhep",this,0),
      _stddatfile("StdhepFileName", this, "stdhep.dat"),
      _useStdEvent("NumOfStdEvent", this, 10),
      _pythiaMenu(),
      _herwigMenu()
  {}

};
#endif

