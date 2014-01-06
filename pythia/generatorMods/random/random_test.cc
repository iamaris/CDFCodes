//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: random_test.cc,v 1.1 2002/03/07 19:39:19 lena Exp $
//
// created Mar 07 2002, Elena Gerchtein, CMU
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Framework/APPUserBuild.hh"

#include "FrameMods/addCDFrequiredModules.hh"
#include "r_n/RandomModule.hh"
#include "RandomTestModule.hh"
#include "generatorMods/GenInputManager.hh"

static const char rcsid[] = "$Id: random_test.cc,v 1.1 2002/03/07 19:39:19 lena Exp $";

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  AppModule* aMod;
  addCDFrequiredModules( this );

  //gen input manager - input module
  add( new GenInputManager() );

  //random number generators manager
  aMod = new RandomModule("RandomGenManager", "Random number generators manager");
  add(aMod); aMod->setEnabled(true);

  // Generator filters
  aMod = new RandomTestModule();
  add( aMod );  aMod->setEnabled(true);
}

//--------------
// Destructor --
//--------------

AppUserBuild::~AppUserBuild( )
{
}
const char * AppUserBuild::rcsId( ) const
{
   return rcsid;
}



