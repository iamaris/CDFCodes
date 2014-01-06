//------------------------------------------------------------------------------
// Description:
//	Base class for generator modules
//
//   REQUIREMENTS:
//      -DummyInput required before generators ( that will be checked 
//    in GenOutputManager )
//      -HEPEVT has to be filled by derived class
//   
//   PROVIDES:
//      -inits HEPEVT for derived class
//      -inits rn stream for derived class
//      -manages multiple primary interactions generation
//        ( FIXEDMODE/POISSONMODE options )  
//
// created July 27 2001, Elena Gerchtein (CMU)
//       
//----------------------------------------------------------------------------
#ifndef ABSGENMODULE_HH__
#define ABSGENMODULE_HH__

#include "BaBar/Cdf.hh"
#include "Framework/APPMenu.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmEnum.hh"
#include "Framework/AbsParmGeneral.hh"
#include "FrameMods/HepHistModule.hh"
#include "stdhep_i/CdfHepEvt.hh"
#include <vector>
#include <string>

class HepRandomEngine;

class AbsGenModule : public HepHistModule {

public:

  AbsGenModule(const char* name  = "AbsGenModule", 
	       const char* title = "AbsGenModule");

  AbsGenModule(const AbsGenModule& m);

  virtual ~AbsGenModule();
					// ****** operations
    
  AppResult          beginJob      ( AbsEvent* job);
  AppResult          beginRun      ( AbsEvent* run);
  AppResult          endRun        ( AbsEvent* run);
  AppResult          endJob        ( AbsEvent* job);
  AppResult          event         ( AbsEvent* event );

					// virtual functions to be overwritten 
					// in the derived classes
  
  virtual AppResult  genBeginJob() = 0;
  virtual AppResult  genEndJob  () = 0;
  virtual AppResult  genBeginRun(AbsEvent* run) = 0;
  virtual AppResult  genEndRun  (AbsEvent* run) = 0;
  virtual int        callGenerator(AbsEvent* event) = 0;
  virtual void       fillHepevt  () = 0;

    					// ****** access functions
  double mean               () { return _mean.value(); }
  int    nGeneratedEvents   () { return _nGeneratedEvents; }
  int    generationMode     ();

public:
  static HepRandomEngine* absGenEngine;

protected:

  static const char* genId;             // should be defined in derived class
					// constants defining the generation mode
  static int const FIXEDMODE;
  static int const POISSONMODE;
  static int const TRUNCPOISSON;
					// defines whether fixed or 
					// Poisson-distributed number of events
					// will be generated 
  AbsParmEnum         _generationMode;
					// total number of events generated for
					// current run
  int                 _nGeneratedEvents;
					// mean number of events to be generated
                                        // per call (default = 1)
  AbsParmGeneral<double> _mean;
  void                   _initStdhepLun(int lun);

  // Subclasses should only override this if they provide new output methods
  virtual int outputEvent(AbsEvent* event); 

  void Plot(const std::string&, 
	    const float& var, 
	    const int& nbins, 
	    const float& low, 
	    const float& high, float weight=1);
  
private:
  std::vector<HepObj*> _histList;
};

#endif
