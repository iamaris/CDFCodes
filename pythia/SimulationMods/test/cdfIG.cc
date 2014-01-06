//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: cdfIG.cc,v 1.1 2001/03/08 23:18:51 lena Exp $
//
// Description:
//	Class AppUserBuild. This class must be provided by the user of
//	the framework in order to build an application. It must define
//	the modules that are to form the basis of the application.
//
// Environment:
//	Software developed for the CDF Detector
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "Framework/APPUserBuild.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "FrameMods/addCDFrequiredModules.hh"
#include "SimulationMods/InteractiveGeant.hh"
#include "SimulationBase/FactoryMacros.hh"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
// We need to make a class to hold the registry declarations.  As a
// kludge for now, we will just add the SIM_REGISTRY_DECL for each 
// digitizer class here.
//
////////////////////////////////////////////////////////////////////////
//namespace cot
//{
//   SIM_REGISTRY_DECL(CotDigitizer);
//}
//namespace svx
//{
//   SIM_REGISTRY_DECL(SvxDigitizer);
//}
//namespace muon
//{  // as defined in MuonDigi.cc
//        SIM_REGISTRY_DECL(MuonDigiCMU);
//        SIM_REGISTRY_DECL(MuonDigiCMP);
//        SIM_REGISTRY_DECL(MuonDigiCMX);
//}

static const char rcsid[] = "$Id: cdfIG.cc,v 1.1 2001/03/08 23:18:51 lena Exp $";

//----------------
// Constructors --
//----------------

AppUserBuild::AppUserBuild( AppFramework* theFramework )
    : AppBuild( theFramework )
{
   addCDFrequiredModules( this );

   add(new InteractiveGeant());  

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
