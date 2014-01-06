//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: hepevt2hepg.cc,v 1.1 2002/01/31 18:50:36 lena Exp $
//
// created Jan 17 2002, Elena Gerchtein (CMU)
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Framework/APPUserBuild.hh"

#include "FrameMods/addCDFrequiredModules.hh"
#include "FrameMods/root/HepRootManager.hh"

#include "Hepevt2HepgModule.hh"
#include "generatorMods/GenInputManager.hh"

static const char rcsid[] = "$Id: hepevt2hepg.cc,v 1.1 2002/01/31 18:50:36 lena Exp $";

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  AppModule* aMod;
  addCDFrequiredModules( this );

  // this should be before filters
  aMod = new HepRootManager(); add(aMod); aMod->setEnabled(false);

  //gen input manager - input module
  add( new GenInputManager() );

    add( new Hepevt2HepgModule() );
  // add( new testMod() );
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

