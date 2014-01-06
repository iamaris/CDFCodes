//--------------------------------------------------------------------------
// File and Version Information:
// 	AppUserBuild.cc for example Wbbgen job
//
// Environment:
//	Software developed for the CDFII Detector.
//
//------------------------------------------------------------------------

#include "Framework/APPUserBuild.hh"

#include "FrameMods/APPFileInputModule.hh"
#include "FrameMods/F77InterfaceModule.hh"
#include "FrameMods/addCDFrequiredModules.hh"
#include "generatorMods/WbbgenModule.hh"

static const char rcsid[] = "$Id: wbbgen_build.cc,v 1.1 2001/08/28 15:16:40 lena Exp $" ;

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
  addCDFrequiredModules(this);

  add(new WbbgenModule());
  add(new F77InterfaceModule());
}

//--------------
// Destructor --
//--------------

AppUserBuild::~AppUserBuild( )
{
}

