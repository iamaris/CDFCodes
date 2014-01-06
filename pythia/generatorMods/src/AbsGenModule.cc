//--------------------------------------------------------------------------
// AbsGenModule
//
// created July 27 2001, Elena Gerchtein (CMU)
//
//------------------------------------------------------------------------
#include <math.h>
#include <string>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <iostream>
#include <list>

#include "Experiment/Experiment.hh"

#include "stdhep_i/CdfHepevt.hh"
#include "stdhep_i/Hepevt.hh"


#include "ErrorLogger_i/gERRLOG.hh"
#include "HepTuple/HepHist1D.h"
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandomEngine.h"
#include "ParticleDB/hepevt.hh"
#include "evt/evt.h"
#include "generatorMods/AbsGenModule.hh"
#include "stdhep_i/Heplun.hh"
#include "AbsEnv/AbsEnv.hh"

int  const AbsGenModule::FIXEDMODE    = 1;
int  const AbsGenModule::POISSONMODE  = 2;
int  const AbsGenModule::TRUNCPOISSON = 3;

HepRandomEngine* AbsGenModule::absGenEngine = 0;

class NameEqual {
public:
	NameEqual(const std::string& name) : hist(name) {}
	bool operator()(HepObj* ptr) const { return hist==ptr->title(); }
	std::string hist;
};

//______________________________________________________________________________
AbsGenModule::AbsGenModule(const char* name, const char* title)
  : HepHistModule(name,title),
    _mean          ("MEAN"            , this, 1.),
    _generationMode("MODE"            , this, "FIXED"),
    _nGeneratedEvents(0)
{
   commands()->append(&_mean);
   commands()->append(&_generationMode);

   std::list< std::string > enumNames;

   enumNames.clear( );
   enumNames.push_back( "FIXED" );
   enumNames.push_back( "POISSON" );
   enumNames.push_back( "TRUNCPOISSON" );
   _generationMode.initValidList( enumNames );

   // make sure pointers to FORTRAN common
   // blocks are initialized
   InitOfflineCommonBlocks();

}

//______________________________________________________________________________
AbsGenModule::~AbsGenModule() 
{ }

//______________________________________________________________________________
AppResult AbsGenModule::beginJob( AbsEvent* job ) {

  _initStdhepLun(6);
  AbsGenModule::absGenEngine = CdfRn::Instance()->GetEngine("SIMULATION");

  return genBeginJob();  
}

//______________________________________________________________________________
AppResult AbsGenModule::beginRun( AbsEvent* aRun ) 
{
  return genBeginRun(aRun);
}

AppResult AbsGenModule::endRun( AbsEvent* aRun ) 
{
  return genEndRun(aRun);
}

AppResult AbsGenModule::endJob( AbsEvent* aJob ) 
{
  return genEndJob();
}

//______________________________________________________________________________
AppResult AbsGenModule::event(AbsEvent* event) {

  int nev, rc;
  // define number of primary interactions
  // to be generated

  if (_generationMode.value() == "FIXED") {
    nev = (int) _mean.value();
    if ( verbose() ) {
      Plot("nevents", nev*1., 11, -1., 10.);
    }
  }
  else if (_generationMode.value() == "POISSON") {
    HepRandomEngine* engine = AbsGenModule::absGenEngine;
    nev = RandPoisson::shoot(engine,_mean.value());
    if ( verbose() ) {
      Plot("nevents", nev*1., 11, -1., 10.);
    }
  }
  else if (_generationMode.value() == "TRUNCPOISSON") {
    HepRandomEngine* engine =  AbsGenModule::absGenEngine;
    // Dave Waters, 29 AUG 2004. Truncated Poisson - Poisson(n,mu) for n >= 1
    nev = 0;
    while (nev == 0) {
      nev = RandPoisson::shoot(engine,_mean.value());
    }
    if ( verbose() ) {
      Plot("nevents", nev*1., 11, -1., 10.);
    }
  }
  else {
    assert(0.); 
  }
					// ... and just tell the real generator
					// to generate given number of events
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->clearCommon();                // overkill...
  Hepevt* cur;
  for (int i=0; i<nev; i++) {    
    do {      
      rc = this->callGenerator(event);
       				        // different generators produce their 
					// output in different format, convert
					// everything to /HEPEVT/
      this->fillHepevt();
      cur = new Hepevt( *hepevt->HepevtPtr(), *hepevt->Hepev4Ptr() );
      hepevt->contentHepevt().push_back( cur );
      hepevt->clearCommon();  
    } while (rc);
  }
  return AppResult::OK;
}

//_____________________________________________________________________________  
void AbsGenModule::_initStdhepLun(int lun) {
  Heplun heplun;
  if (!heplun.lnhout()) heplun.lnhout()=6;
}

int AbsGenModule::generationMode()
{
  if ( _generationMode.value() == "FIXED" )
    {
      return FIXEDMODE;
    }
  else if ( _generationMode.value() == "POISSON" )
    {
      return POISSONMODE;
    }
  else if ( _generationMode.value() == "TRUNCPOISSON" )
    {
      return TRUNCPOISSON;
    }
  // This can't happen
  assert(0);
  return 0;
}


int AbsGenModule::outputEvent( AbsEvent* event ) {
  return 0;
}

void AbsGenModule::Plot(const std::string& title, 
			const float& var, 
			const int& nb, 
			const float& low, 
			const float& high, float weight) 
{  

	std::vector<HepObj*>::iterator result = std::find_if(_histList.begin(), 
						   _histList.end(),
						   NameEqual(title));
	HepHist1D* h;
	if (result == _histList.end()) 
	{
		h = &fileManager()->hist1D(title,nb,low,high);
		_histList.push_back(h);
	}
	else 
		h = &fileManager()->retrieveHist1D(title);
	h->accumulate(var,weight);
}
