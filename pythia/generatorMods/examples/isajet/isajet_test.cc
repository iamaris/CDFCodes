//--------------------------------------------------------------------------
// File and Version Information:
// 	AppUserBuild.cc for mymods
//
// Description:
//	Class AppUserBuild. This class must be provided by the user of
//	the framework in order to build an application. It must define
//	the modules that are to form the basis of the application.
//
// Environment:
//	Software developed for the CDFII Detector.
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Framework/APPUserBuild.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "generatorMods/IsajetModule.hh"
#include "FrameMods/APPFileInputModule.hh"
#include "FrameMods/F77InterfaceModule.hh"
#include "FrameMods/addCDFrequiredModules.hh"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------
//static const char rcsid[] = "$Id: isajet_test.cc,v 1.11 1999/10/27 21:18:32 greenc Exp $" ;

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{

  addCDFrequiredModules(this);

  add(new IsajetModule());
  add(new F77InterfaceModule());

}

//--------------
// Destructor --
//--------------

AppUserBuild::~AppUserBuild( )
{
}
