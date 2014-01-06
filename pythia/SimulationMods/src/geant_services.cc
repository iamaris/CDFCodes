////////////////////////////////////////////////////////////////////////
//
// File: geant_services.cc
// $Id: geant_services.cc,v 1.11 2002/01/22 03:06:38 rico Exp $
// Purpose: Present several functions to be used by SimulationControl
// and IntegrationControl.
//
// Maybe these should be moved to geant_i?
//
// Author:  Marc Paterno
// Created: 30 Mar 2001
// Modified:
//
////////////////////////////////////////////////////////////////////////
#include "geant_i/TGeant3.h"

// It isn't clear which of several global variables named gMcEvent
// should be used. We'll just have to guess.
// The choices (as of 30 March 2001) were (from the code browser)
//    evt/Event.hh, line 137 -- extern MC_EVENT* gMcEvent;
//    gnt/mc_event.hh, line 199 -- extern MC_EVENT* gMcEvent;
//    TofAlgs/src/CheatT0Calculator.cc, line 52 -- extern MC_EVENT* gMcEvent;
//    TofAlgs/src/TofMCAssoc.cc, line 101 -- extern MC_EVENT* gMcEvent;
// Aren't global variables lovely?
#include "SimulationUtils/McEvent.hh"    // for global MC_EVENT* gMcEvent
#include "ParticleDB/ParticleDb.hh" 
#include "ErrorLogger/ErrorLog.h"

namespace
{
  const size_t MAX_U_WORDS = 10;
  ErrorLog gsErrlog("SimulationMods: geant_services");
}

//
//  This is the function which GEANT3 will call as ``GUTRAK''
//

void syncWithG3ParticlesAndTrack() 
{
	TGeant3* g3 = TGeant3::Instance();

  // Most of this was taken from gnt/gutrak.cc 1.18 on 2000/04/25 with
  // some changes to tidy up and remove globals. No real effort to
  // completely overhaul this as yet: it's not a priority
  
  float vert[3];  // vector origin of the track
  float pvert[4]; // 4-momentum at the origin
  int ipart;      // particle type (=0 if non-existent)
  int nvert;      // vertex number 
  float ubuf[MAX_U_WORDS]; // user words may be stored with the track
  int nubuf;      // actual number of user words retrieved;

  if (g3->Gckine()->ISTAK) 
		{ 
			// particle was on the temporary stack
			g3->InitCurrentParticle(NULL);
		} 
	else
		{ 
			// particle is in JKINE/JVERTX and is not on the temporary stack
			int itra = g3->Gckine()->ITRA;
			
			// Retrieve the particle parameters
			g3->Gfkine(itra,vert,pvert,ipart,nvert,ubuf,nubuf);
			assert (!(nubuf>MAX_U_WORDS)); // safety check
			
			// We're interested in the first user word:
			if(!0==nubuf) {
				int ip = (int) ubuf[0];
				g3->InitCurrentParticle(McEvent::Instance()->Particle(ip));
			}
		} //   if (g3->Gckine()->ISTAK)	

  // According to Pasha, this should be done by GEANT and is not:
  g3->Gctrak()->IGNEXT = 0;

  // Now track the particle:
  g3->Gtrack();

  // Finish
  g3->CurrentParticle()->TerminateTracing();
}

// gheisha - fluka switch
extern "C" {
	void gheish_();
  void flufin_();
	void gpghei_();
  void fldist_();
}

void geantGuphad()
{
  if ( TGeant3::Instance()->GetHadronicShowerPackage() ) { 
		//		std::cout << " hshPakage = " << TGeant3::Instance()->GetHadronicShowerPackage() <<" using GEANT/FLUKA  in GUPHAD"<< std::endl; 
		fldist_(); 
	} else { 
		// std::cout << " hshPakage = " << TGeant3::Instance()->GetHadronicShowerPackage() <<" using GEANT/GHEISHA in GUPHAD" << std::endl; 
		gpghei_(); 
	}
}

void geantGuhadr()
{
  if ( TGeant3::Instance()->GetHadronicShowerPackage() ) { 
		//		std::cout << " hshPakage = " << TGeant3::Instance()->GetHadronicShowerPackage() <<" using GEANT/FLUKA in GUHADR" << std::endl; 
		flufin_(); 
	} else { 
		//		std::cout << " hshPakage = " << TGeant3::Instance()->GetHadronicShowerPackage() <<" using GEANT/GHEISHA in GUHADR" << std::endl; 
		gheish_(); 
	}
}


//  moved from gnt
#include "inc/bcs.h"
#include "inc/obs203.hh"
//#include "gnt/mc_event.hh"

extern "C" {
  void  qhep2obs_(float *vx, float *vy, float *vz, float *time);
  void  obsppt_(int *ierr);
  void  obsbtc_(int *ierr);
}


void geantGukine()
{
  float      xyz[4], plab[4];
  int        nvtx, nt, g3_code;
  float      upar[10];

  if ( McEvent::Instance()->InitEvent() < 0 ) {
		gsErrlog( ELerror, "geantGukine" )
			<< "McEvent is not properly initialized."
			<< endmsg;
  }

	//	McEvent::Instance()->Print(std::cout,"");

//-----------------------------------------------------------------------
//  read the kinematics of event to be simulated from OBS banks and put
//  it to the GEANT3 kinematical tree
//-----------------------------------------------------------------------
  TGeant3* gnt = TGeant3::Instance();
                                        // loop over the MC vertices

  for (int i=0; i<McEvent::Instance()->NVertices(); i++) {
    McVertex* v = McEvent::Instance()->Vertex(i);
					// put the vertex into G3 kinematic
                                        // structure; don't forget that all
                                        // the float gsvert parameters are 
                                        // real*4 and all the indices are 
                                        // C++ (FORTRAN-1)
    v->V3::get(xyz);
					// tell GEANT about vertex production 
					// time
    gGctrak->TOFG  = v->time();
    gnt->Gsvert(xyz,v->ParentNumber()+1,0,upar,0,nvtx);

                                        // now loop over the particles
                                        // produced in this vertex and
                                        // put them into the GEANT kinematic
                                        // structure

    for (int j=v->FirstDaughter(); j<=v->LastDaughter(); j++) {
      McParticle* p = McEvent::Instance()->Particle(j);

					// retrieve particle momentum and get
					// its G3 code
      p->Momentum()->get(plab);

			ParticleDb* pdb = ParticleDb::Instance();
			g3_code = pdb->GeantCodeOfCdfParticle( p->CdfCode() );

					// store pointer to McParticle
					// there is no (in general) one-to-one
					// correspondence between GEANT and CDF
					// particle numbers: for example, we 
					// don't want to store ALL the particles
					// in GEANT showers, so there will be
					// GEANT particles which will not be 
					// stored in CDF structures
      upar[0] = p->Number();
      if (g3_code >= 0) {
					// do not trace the particle (K0S, Lam0)
					// if it has already been decayed by 
					// the MC generator
	if (p->TerminationCode() == 0) {
					// store only known particles, so do not
					// trace and decay B-mesons, for example
					// also cut on particle Pt and Eta to

	  if ( (p->Pt()        > McEvent::Instance()->GeantPtCut ()) && 
	       (fabs(p->Eta()) < McEvent::Instance()->GeantEtaCut())    ) {
	    gnt->Gskine(plab,g3_code,nvtx,upar,1,nt);
	  }
	  else {
					// do not trace this particle - its Pt 
					// or Eta is outside requested limits

	    gnt->Gskine(plab,54,nvtx,upar,1,nt);
	  }
	}
 	else {
					// set for particles decayed by MC event
					// generators to dummy

	  gnt->Gskine(plab,54,nvtx,upar,1,nt);
	}
      }
      else {
					// set for particles unknown to GEANT to
					// dummy
	gnt->Gskine(plab,54,nvtx,upar,1,nt);
      }
    }
  }

  int debug = 0;

  if (debug) {
    gnt->Gprint("VERT",0);
    gnt->Gprint("KINE",0);
    McEvent::Instance()->Print(std::cout);
  }
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

