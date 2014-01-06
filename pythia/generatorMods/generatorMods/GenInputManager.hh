//--------------------------------------------------------------------------
// Description:
//      generator (I) output manager
//
// created July 31 2001, Elena Gerchtein (CMU)
//
//

#ifndef GENINPUTMANAGER_HH__
#define GENINPUTMANAGER_HH__

class AbsEvent;

#include "FrameMods/APPGenInputModule.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "BaBar/Cdf.hh" 
#include "SimulationObjects/MCRunNumberFromLumi.hh"
 
class GenInputManager : public  APPGenInputModule {

public:

  GenInputManager( const char* const theName = "GenInputManager", 
		    const char* const theDescription = "Generator Input Manager" );
  virtual ~GenInputManager( );
 
  virtual AppResult   childBeginJob( AbsEvent* event);
  virtual AppResult   childBeginRun( AbsEvent* event);
  virtual AppResult   event        ( AbsEvent* event);
  virtual AppResult   initEventHeader( );

  virtual int eventRecordType() const;

private:

  void generateNewRunSection();

  int  _eventNumber;
  AbsParmGeneral<int> _runSectionLength;
  AbsParmGeneral<int> _runSectionOffset;
  
  AbsParmBool _restoreRandomStream;

  AbsParmBool _lumiWtRunsFromDB;
  MCRunNumberFromLumi _mcRun;
  bool _initialized; // t after first beginRun
  
  // Create new LRIH bank and change run number from the LRIH in file input
  AbsParmBool _rewriteLRIH;
  
};

#endif


















