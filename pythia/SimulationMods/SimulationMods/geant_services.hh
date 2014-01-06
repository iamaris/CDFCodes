#ifndef SIMULATIONMODS_GEANT_SERVRICES_HH
#define SIMULATIONMODS_GEANT_SERVRICES_HH
////////////////////////////////////////////////////////////////////////
//
// File: geant_services.hh
// $Id: geant_services.hh,v 1.4 2001/07/18 23:47:26 lena Exp $
// Purpose: Present several GEANT3-related functions to be used by
// SimulationControl and IntegrationControl.
//
// Maybe these should be moved to geant_i?
//
// Author:  Marc Paterno
// Created: 30 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////

extern "C" 
{
 	void gpcxyz_(); 
	void gpmate_(int* i);
}
void geantGukine();
void geantGuphad();
void geantGuhadr();
void syncWithG3ParticlesAndTrack();

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

#endif // #ifndef SIMULATIONMODS_GEANT_SERVRICES_HH
