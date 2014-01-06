//--------------------------------------------------------------------------
// Description:
//	Class RandomTestModule
//
// this module fills /HEPEVT/ from binary/ascii file and
// and writes HEPG bank from /HEPEVT/ common block
//
//  created Jan 17 2002, Elena Gerchtein (CMU)
//------------------------------------------------------------------------

#include "RandomTestModule.hh"
#include "r_n/CdfRn.hh"
#include "CLHEP/Random/RandFlat.h"
#include "SimulationObjects/RandomEngineStateColl.hh"
#include "Edm/Handle.hh"
#include "Edm/GenericConstHandle.hh"
#include <sstream>


const long RandomTestModule::_defaultRandomSeed1 = 9223591;
const long RandomTestModule::_defaultRandomSeed2 = 109736;
const double RandomTestModule::_breakPoint = 0.450797;

RandomTestModule::RandomTestModule(const char* name                      ,
		     const char* title ) 
  : AppModule( name, title ),
    _randomSeed1("EvilSeed1",this,RandomTestModule::_defaultRandomSeed1),
    _randomSeed2("EvilSeed2",this,RandomTestModule::_defaultRandomSeed2)
{
 // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Random number menu");
  commands()->append(&_randomNumberMenu);

  // Add the commands to the menu
  _randomNumberMenu.commands()->append(&_randomSeed1);
  _randomNumberMenu.commands()->append(&_randomSeed2);

  std::ostringstream tmpSstream1;

  // Describe them
  tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed1.value() << ").";
  _randomSeed1.addDescription(tmpSstream1.str());
  tmpSstream1 << "      \t\t\tSeed #2 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed2.value() << ").";  
  _randomSeed2.addDescription(tmpSstream1.str());
}

RandomTestModule::~RandomTestModule() {}
 
AppResult RandomTestModule::beginJob(AbsEvent* event) {

  std::cout << " RandomTestModule engine is initialized. " << std::endl;
  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"RANDOM_TEST");
  }
  return AppResult::OK;
}

AppResult RandomTestModule::endJob(AbsEvent* event) {
  return AppResult::OK;
}

AppResult RandomTestModule::endRun(AbsEvent* event) {
  return AppResult::OK;
}

AppResult RandomTestModule::beginRun(AbsEvent* event) {
  return AppResult::OK;
}  

AppResult RandomTestModule::event(AbsEvent* event) {

  HepRandomEngine* engine = CdfRn::Instance()->GetEngine("RANDOM_TEST");
  std::cout << " RandomTestModule::event() : processing... " << std::endl;
  for ( int i = 0; i<10; ++i ) {
    double x = RandFlat::shoot(engine);
    std::cout << " random = " << x << std::endl; 
    if ( x < 0.002 ) { assert(0);}
  }

  // another test: RandomEngineStateColl 
  
  std::vector<RandomEngineState> states_vec = 
    CdfRn::Instance()->GetState();

  RandomEngineStateColl* resc = new RandomEngineStateColl(states_vec);
  Handle<RandomEngineStateColl> resc_handle(resc);
  GenericConstHandle write_hc_handle(event->append(resc_handle));

  return AppResult::OK;
}
        
AppResult RandomTestModule::abortJob(AbsEvent* event) {
  return AppResult::OK;
}
  




