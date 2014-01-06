//--------------------------------------------------------------------------
// Description:
//      generator (I) output manager
//
// created July 26 2001, Elena Gerchtein (CMU)
//
//

#ifndef GENOUTPUTMANAGER_HH__
#define GENOUTPUTMANAGER_HH__

class AbsEvent;

#include "Framework/APPModule.hh"
#include "BaBar/Cdf.hh" 
 
class GenOutputManager : public  AppModule {

public:

  GenOutputManager( const char* const theName = "GenOutputManager", 
		    const char* const theDescription = "Generator Output Manager" );
  virtual ~GenOutputManager( );
 
  virtual AppResult   beginRun( AbsEvent* event);
  virtual AppResult   beginJob( AbsEvent* event);
  virtual AppResult   endRun  ( AbsEvent* event);
  virtual AppResult   endJob  ( AbsEvent* event);
  virtual AppResult   event   ( AbsEvent* event);
  virtual AppResult   abortJob( AbsEvent* event);

private:

  //write HEPG bank if true
  bool _output;
  bool _stopIt;
  int here_is_tauola;
  
};

#endif


















