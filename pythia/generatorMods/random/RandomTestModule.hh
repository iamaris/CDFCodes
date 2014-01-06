//--------------------------------------------------------------------------
// Description:
//      random state save/restore test
//
// created Mar 7 2002, Elena Gerchtein (CMU)
//
//

#ifndef RANDOMTESTMODULE_HH__
#define RANDOMTESTMODULE_HH__

#include "Framework/APPModule.hh"
#include "BaBar/Cdf.hh" 
#include "Framework/AbsParmGeneral.hh"
#include <string>
typedef AbsParmGeneral<long> AbsParmGeneral_long;
 
class RandomTestModule : public  AppModule {

public:
  RandomTestModule( const char* name = "RandomTestModule",
		     const char* title = "Random test module");
  virtual ~RandomTestModule( );
 
  AppResult   beginRun( AbsEvent* run);
  AppResult   beginJob( AbsEvent* job);
  AppResult   endRun  ( AbsEvent* run);
  AppResult   endJob  ( AbsEvent* job);
  AppResult   event   ( AbsEvent* event);
  AppResult   abortJob( AbsEvent* job);

private:

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;
  static const double _breakPoint;  

};

#endif
