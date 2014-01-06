////////////////////////////////////////////////////////////////////////
//
// File: cdfIntegrate.cc
// $Id: cdfIntegrate.cc,v 1.1 2001/04/09 14:54:34 paterno Exp $
// Purpose: This is the AppUserBuild file for the material integration
// program.
//
// Author:  Marc Paterno
// Created: 29 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

#include "Framework/APPUserBuild.hh"
#include "FrameMods/addCDFrequiredModules.hh"
#include "SimulationMods/SimInitManager.hh"
#include "SimulationMods/IntegrationControl.hh"
#include "generatorMods/FakeEv.hh"

static const char rcsid[] = "$Id: cdfIntegrate.cc,v 1.1 2001/04/09 14:54:34 paterno Exp $";


AppUserBuild::AppUserBuild( AppFramework* theFramework ) : 
	AppBuild( theFramework )
{
   addCDFrequiredModules( this );
   add(new SimInitManager());   // Declare geometry to G3
   add(new FakeEv());
   add(new sim::IntegrationControl()); // Make the integrator
}

AppUserBuild::~AppUserBuild( ) { }

const char * AppUserBuild::rcsId( ) const { return rcsid; }

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

