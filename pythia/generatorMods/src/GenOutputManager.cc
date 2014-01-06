//--------------------------------------------------------------------------
// Description:
//	Class GenOutputManager
//
// this module checks for generator sequence configuration
// and writes HEPG bank from hepevt common block
//
//  created July 26 2001, Elena Gerchtein (CMU)
//------------------------------------------------------------------------
#include <iostream>
#include "generatorMods/GenOutputManager.hh"
#include "generatorMods/HadScatGenSequence.hh"
#include "generatorMods/DecayPackageSequence.hh"
#include "generatorMods/QQModule.hh"
#include "generatorMods/IsajetModule.hh"
#include "generatorMods/EvtGenMod.hh"
#include "generatorMods/HerwigModule.hh"
#include "generatorMods/LesHouchesModule.hh"
#include "generatorMods/VecbosModule.hh"
#include "generatorMods/VecherModule.hh"
#include "generatorMods/TauolaModule.hh"
#include "generatorMods/UnwtModule.hh"
#include "generatorMods/WGRAD_Module.hh"

#include "stdhep_i/CdfHepevt.hh"
#include "stdhep_i/Hepevt.hh"
#include "stdhep_i/Tauevt.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh" 
#include "SimulationObjects/TAUG_StorableBank.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "SimulationObjects/HEP4_StorableBank.hh"
#include "Framework/APPFramework.hh"
#include "Framework/APPExecNode.hh"
#include "Framework/APPExecutable.hh"
#include "Framework/APPSequence.hh"
#include "r_n/CdfRn.hh"
#include <iostream>
#include <string>
using namespace std;

GenOutputManager::GenOutputManager(const char* const theName, 
    const char* const theDescription )
  : AppModule( theName, theDescription ),
    _output(true),
    _stopIt(false),
    here_is_tauola(0)
{
}

GenOutputManager::~GenOutputManager()
{
}

AppResult GenOutputManager::beginJob(AbsEvent* event) {

  AppResult ar;
  list<APPExecNode*>* nodes; 
  
  std::string en_name;   // name of the last enabled module
  std::string str1, str2, str3;
  int number_of_en =0;   // number of enabled modules
  int isajet_herwig = 0; // isajet and herwig counter
  int vecbos_vecunw = 0; // vecbos and vecunw counter
  int vecbos_vecher = 0; // vecbos and vecher counter
  int lesHouches    = 0; // LesHouches counter
  int number_of_dec = 0;
  here_is_tauola = 0;
  //-->--> find had. scat.  generator sequence

  list<APPSequence*>* secl = framework()->sequences()->stlList();
  for ( list<APPSequence*>::iterator s = (*secl).begin();
	  s != (*secl).end(); ++s ) {
    if ( strcmp(((APPExecutable*)(*s))->name(),HardScatGenSequence::name) == 0 ) {
      nodes = ((APPSequence*)(*s))->nodes()->stlList();
      for ( list<APPExecNode*>::iterator gen = (*nodes).begin();
	    gen !=  (*nodes).end(); ++gen ) {
	APPExecutable* target = (*gen)->target();
	if ( target->isEnabled() ) {
	  number_of_en++;
	  en_name = (target->name());
	  str1 = IsajetModule::genId;
	  str2 = HerwigModule::genId;
	  if ( en_name == str1  || en_name == str2 ) {
	    isajet_herwig++;
	  }
	  str1 = VecbosModule::genId;
	  str2 = VecunwModule::genId;	  
	  if ( en_name == str1  || en_name == str2 ) {
	    vecbos_vecunw++;
	  }
	  str1 = VecbosModule::genId;
	  str2 = VecherModule::genId;	  	  
	  if ( en_name == str1  || en_name == str2 ) {
	    vecbos_vecher++;
 	  }
          str1 = LesHouchesModule::genId;  	  
	  if ( en_name == str1 )lesHouches++;
	}
      }      
    } else if ( strcmp(((APPExecutable*)(*s))->name(),DecayPackageSequence::name) == 0 ) {
      nodes = ((APPSequence*)(*s))->nodes()->stlList();
      for ( list<APPExecNode*>::iterator gen = (*nodes).begin();
	    gen !=  (*nodes).end(); ++gen ) {
	APPExecutable* target = (*gen)->target();
	if ( target->isEnabled() ) {
	  en_name = (target->name());
	  str1 = EvtGenMod::genId;
	  str2 = QQModule::genId;
	  str3 = TauolaModule::genId;
	  if ( en_name == str1  || en_name == str2 ) {
	    number_of_dec++;
	  }
	  if ( en_name == str3 ) here_is_tauola = 1;
	}
      }      
    }
  }
  
  if ( number_of_dec > 1 ) {
    std::cout << " QQModule and EvtGenMod are enabled at the same time. "
	 << " Only one is allowed. "
	 << endl;    
    _stopIt = true;
    std::cout << " stop is requested " << endl;
    ar = AppResult::OK;
  }

  //-->--> allow only one enabled had.scat. generator
  //-->--> if not Isajet+Herwig
  //-->--> or Vecbos+Vecunw
  //-->--> or Vecbos+Vecher
  
  if ( number_of_en > 1 && (isajet_herwig < 2 && !lesHouches) ) { 
    std::cout << " More than one had. scat. generator is enabled "
	      << " Only one is allowed ( except Pythia+Herwig " 
              <<  "or if using LesHouchesModule)). "
	      << endl;
    _stopIt = true;
    std::cout << " stop is requested " << endl;
    ar = AppResult::OK;
  } else if ( (number_of_en == 0) && (number_of_dec > 0 ) ) {
    std::cout << " ****| GENERATORS WARNING: " << std::endl 
	      << " Decay package is enabled without had. scat. generator; " 
	      << std::endl
	      << " Input file is required. "
	      << " Set mode 1 in decay module. "
	      << " Add HEPG_StorableBank:UNUSED to dropList "
	      << std::endl;
  } else if ( (number_of_en == 0) && 
	      (number_of_dec == 0) &&
	      (here_is_tauola == 0) ) {
    std::cout << " No had. scat. generator or decay package are enabled " <<
      std::endl;
    _output = false;
    ar = AppResult::OK;
  } else {
    ar = AppResult::OK;
  }
  
  if ( !_stopIt ) { 
    std::cout << "****| GENERATORS: generator configuration check PASSED |*****" << endl;
  } 
  return ar;
}

AppResult GenOutputManager::endJob(AbsEvent* event) {

  return AppResult::OK;
}

AppResult GenOutputManager::endRun(AbsEvent* event) {
  return AppResult::OK;
}

AppResult GenOutputManager::beginRun(AbsEvent* event) {
  if ( _stopIt ) framework( )->requestStop();
  return AppResult::OK;
}

AppResult GenOutputManager::event(AbsEvent* event) {
  
  CdfHepevt* hepevt = CdfHepevt::Instance();

  if( _output ) {

    std::list<Tauevt*>::iterator itau = hepevt->contentTauevt().begin();
    for ( std::list<Hepevt*>::iterator i = hepevt->contentHepevt().begin();
	  i !=  hepevt->contentHepevt().end(); ++i ) {
      
      // make HEPG from hepevt; 
      HEPG_StorableBank* hepg = (*i)->makeHEPG(event);
      if (!hepg || hepg->is_invalid()) {
	ERRLOG(ELsevere2,"[GEN_BAD_HEPG]")
	  << "GenOutputManager: can't create a valid "
	  << "HEPG_StorableBank"
	  << endmsg;
	return AppResult::OK;
      }
      Handle<HEPG_StorableBank> h(hepg);
      if ((event->append(h)).is_null()) {
	ERRLOG(ELsevere2,"[GEN_BAD_HEPG]")
	  << "GenOutputManager: can't add"
	  << "HEPG_StorableBank to the event."
	  << endmsg;
	return AppResult::OK;
      }
      /* */

      HEP4_StorableBank* hep4 = (*i)->makeHEP4(event);
      if (!hep4 || hep4->is_invalid()) {
	ERRLOG(ELsevere2,"[GEN_BAD_HEP4]")
	  << "GenOutputManager: can't create a valid "
	  << "HEP4_StorableBank"
	  << endmsg;
	return AppResult::OK;
	}
      Handle<HEP4_StorableBank> h4(hep4);
      if ((event->append(h4)).is_null()) {
	ERRLOG(ELsevere2,"[GEN_BAD_HEP4]")
	  << "GenOutputManager: can't add"
	  << "HEP4_StorableBank to the event."
	  << endmsg;
	return AppResult::OK;
      }

      /*  */
      // make TAUG from TAUPOLARHEP
      // this is done only
      // if TauolaModule was enabled and list of Tauevt list is filled


      if ( here_is_tauola ) {
	if ( *itau ) {
	  TAUG_StorableBank* taug = (*itau)->makeTAUG(event);
	  if (!taug || taug->is_invalid()) {
	    if ( verbose() ) {
	      ERRLOG(ELsevere2,"[GEN_BAD_TAUG]")
		<< "GeneratorModule::tauevt2Taug(): can't create a valid "
		<< "TAUG_StorableBank"
		<< endmsg;
	      return AppResult::OK;
	    }
	  }      
	  Handle<TAUG_StorableBank> t(taug);
	  if ((event->append(t)).is_null()) {
	    ERRLOG(ELsevere2,"[GEN_BAD_TAUG]")
	      << "GeneratorModule::tauevt2Taug(): can't add"
	      << "TAUG_StorableBank to the event."
	      << endmsg;
	    return AppResult::OK;
	  }
	  itau++;
	}
      }
      
      hepevt->clearCommon(); // overkill...
    }
  } else {
    hepevt->clear();
    hepevt->clearCommon();
    return AppResult::OK;
  }
  hepevt->clear();
  hepevt->clearCommon();
  return AppResult::OK;
}
    
AppResult GenOutputManager::abortJob(AbsEvent* event) {
  return AppResult::OK;
}








