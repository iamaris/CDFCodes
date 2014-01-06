//--------------------------------------------------------------------------
// File and Version Information:
// 	StdhepInputModule.hh
//
// Description:
//	Read a fortran file created from the stdhep common
//	and add to ac++ framework
//
// Environment:
//	CDF Run 2
//
// Author List:
//	        Sebastian Carron, Duke University
//              Stan Thompson, Glasgow University    
//              Elena Gershtein, CMU
//
// CEV20110217    Modified by Oksana Tadevosyan to read list of files
//                for generation mode 3
//
// August 9 2011  Modified by Peter Dong with a method allows users to
//                change uncooperative particle codes with a talk-to.
//------------------------------------------------------------------------

#ifndef Hepevt2HepgModule_HH
#define Hepevt2HepgModule_HH

#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmEnum.hh"
#include "Framework/AbsParmFilename.hh"
#include "Framework/AbsParmList.hh"

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/AbsGenModule.hh"

#include <map>
#include <string>

//		---------------------
// 		-- Class Interface --
//		---------------------


class Hepevt2HepgModule : public AbsGenModule {

  //--------------------
  // Instance Members --
  //--------------------

public:

  static const char* genId;
  
  //
  // Constructors
  //
  Hepevt2HepgModule();


public:
  //
  // Destructor
  //
  virtual ~Hepevt2HepgModule( );

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

private:

  // input file stream
  int _lun;
  int _ntries;
  int _istr;
  int _endoffile;

  // various input parameters
  AbsParmGeneral<int>  _firstlistevent;
  AbsParmGeneral<int>  _lastlistevent;
  AbsParmGeneral<int>  _eventlistlevel;
  AbsParmGeneral<int>  _initlistlevel;
  AbsParmGeneral<int>  _eventStart;
  AbsParmGeneral<int>  _eventEnd;

  // event counter
  int _events ;

  AbsParmFilename _stdhepFile;
  AbsParmList<std::string>  _inputFileList;
  AbsParmList<std::string>::ConstIterator _currentFile;

  AbsParmGeneral<std::string> _particleIDChanges;

  // HEPEVT access
  CdfHepEvt _hepevt;  

protected:

private:
  Hepevt2HepgModule( const char* const theName, 
		const char* const theDescription) 
    : AbsGenModule( theName, theDescription),
    _stdhepFile("StdhepFile",this,""),
    _firstlistevent("Listfirst",    this, 1),
    _lastlistevent ("Listlast",    this, 1),
    _eventlistlevel("Evlistlevel",    this, 1),
      _initlistlevel("Inlistlevel",    this, 11),
      _eventStart("EventStart",    this, 1),
      _eventEnd("EventEnd",    this, 1),
      _inputFileList("InputFileList",  this,  0),
      _particleIDChanges("ParticleIDChanges", this, "")
  {}

  Hepevt2HepgModule( const Hepevt2HepgModule& m)
    : AbsGenModule( "x", "x"),
    _stdhepFile("StdhepFile",this,""),
    _firstlistevent("Listfirst",    this, 1),
    _lastlistevent ("Listlast",    this, 1),
    _eventlistlevel("Evlistlevel",    this, 1),
      _initlistlevel("Inlistlevel",    this, 11),
      _eventStart("EventStart",    this, 1),
      _eventEnd("EventEnd",    this, 1),
      _inputFileList("InputFileList",  this,  0),
      _particleIDChanges("ParticleIDChanges", this, "")
  {}

  std::map<int, int> _particleIDMap;

};
#endif
