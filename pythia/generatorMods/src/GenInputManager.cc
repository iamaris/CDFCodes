//--------------------------------------------------------------------------
// Description:
//	Class GenInputManager
//
// this module writes header banks to EventRecord;
// /HEPEVT/ is initialized here. 
//
//  created July 31 2001, Elena Gerchtein (CMU)
//------------------------------------------------------------------------

#include "AbsEnv/AbsEnv.hh"
#include "generatorMods/GenInputManager.hh"
#include "stdhep_i/CdfHepevt.hh"
#include "Edm/Handle.hh"
#include "Edm/ConstHandle.hh"
#include "Edm/EventRecord.hh" 
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "HeaderObjects/LRIH_StorableBank.hh"
#include "HeaderObjects/LRIH_StorableBank_utilities.hh"
#include "Framework/APPFramework.hh"
#include "Framework/APPExecNode.hh"
#include "Framework/APPExecutable.hh"
#include "Framework/APPSequence.hh"
#include "Framework/AppStop.hh"
#include "Framework/AppFrame.hh"
#include "r_n/CdfRn.hh"

using namespace std;

GenInputManager::GenInputManager(const char* const theName, 
    const char* const theDescription )
  : APPGenInputModule( theName, theDescription ),
    _restoreRandomStream("RestoreRandomStream", this, true),
    _runSectionLength("RunSectionLength", this, 1000, 1, 1000000 ),
    _runSectionOffset("RunSectionOffset", this, 0, 0, 1000000 ),
    _lumiWtRunsFromDB("LumiWtRunsFromDB", this, false),
    _rewriteLRIH("RewriteLRIH", this, false),
    _eventNumber(0),
    _mcRun(this), _initialized(false)
{
  commands()->append(&_runSectionLength);
  commands()->append(&_runSectionOffset);
  commands()->append(&_lumiWtRunsFromDB);
  commands()->append(&_rewriteLRIH);
  _mcRun.appendCommands( commands() );

  _rewriteLRIH.addDescription("Change the run number in LRIH of input file.\nThis allows you to change the run number of a file with only GENP info, for example.");
}

GenInputManager::~GenInputManager()
{
}

AppResult GenInputManager::childBeginJob(AbsEvent* event) {

  // provides /HEPEVT/ for other fortran code 
  CdfHepevt* hepevt = CdfHepevt::Instance();

  // can not get eventNumberOffset...
  _eventNumber += 0;

  if (! _initialized) {
    _initialized = true;
    if (_lumiWtRunsFromDB.value()) {

      // Calibration manager isn't ready yet, so
      // hijack framework and initialize calibration manager
      if (framework()->fetchModule("CalibrationManager")) {
	framework()->fetchModule("CalibrationManager")->beginJob(event);
      }

      // Throw tcl switches in base class which will activate use of _runInfo
      _specifyRangeOfRuns.set(true);
      _beginRunRequested.set(true);
      
      // Initialize MC Run and event numbers
      if (! _mcRun.beginJob( framework() )) return AppResult::ERROR;
      
      // Copy the run and event info from MCRunNumberFromLumi to protected
      // data in baseclass APPGenInputModule
      _runInfo.clear();
      for (int i=0; i<_mcRun.runInfo().size(); i++) {
	_runInfo.push_back( APPGenRunInfo(_mcRun.runInfo()[i].run, 
					  _mcRun.runInfo()[i].section, 
					  _mcRun.runInfo()[i].Nevent) );
      }
      _iterRunInfo = _runInfo.begin();
      _runNumber.set(_iterRunInfo->runNum());
    }
    else { // Use the baseclass' method for setting run numbers
      APPGenInputModule::childBeginJob(event);
    }
  }
  return AppResult::OK;
}

void
GenInputManager::generateNewRunSection() {
  AppStop* generatedStop = new AppStop( theRecord(), AppStopType::begin_run_section, 0);
  
  ERRLOG( ELinfo, "generateBeginRunSectionFrame called." )
    << "  A stop with a stop type of hash key: " 
    << generatedStop->stopType().getStopTypeKey() 
    << " has been generated"<<endmsg;
  
  // When the Stop is stacked it gives ownership of the the Stop to the Frame
  theFrame( )->stackStop(generatedStop,1); 
  // narrow frame: stack one deep max (keep latest instance other instances
  // will be deleted by this call)

}

AppResult
GenInputManager::initEventHeader( )
{

  static int oldSection = 1;
   // This one is very similar to the base class implementation but it adds
   // the information about the correct run section number
   _eventNumber++;
   int section = _eventNumber / _runSectionLength.value() + 1 + 
      _runSectionOffset.value();
   if (section != oldSection) {
     generateNewRunSection();
     oldSection = section;
   }
   if ( verbose() ) 
   {
      std::cerr << "GenInputManager::initEventHeader, runsection number = " 
		<< section << std::endl;
   }
   // Create the LRIH bank and check its validity
   Handle<LRIH_StorableBank> h_lrih = createLRIH( );
   if (h_lrih->is_invalid()) {
      ERRLOG(ELsevere2,"[GEN_INPUT_BAD_LRIH]")
	 << "APPGenInputModule::nextEvent(): can't create a valid "
	 << "LRIH_StorableBank"
	 << endmsg;
      framework( )->requestStop( );
      return AppResult::ERROR;   
   }
   else
   {
      // Use the growing section number not the constant one
      h_lrih->set_section(section);
      // Use handle to hand the StorableBank to the event
      if ((theRecord()->append(h_lrih)).is_null()) {
	 ERRLOG(ELsevere2,"[GEN_INPUT_BAD_LRIH]")
	    << "APPGenInputModule::nextEvent(): can't add "
	    << "LRIH_StorableBank"
	    << "to the event."
	    << endmsg;
	 // request a stop here
	 framework( )->requestStop( );
	 return AppResult::ERROR;
      }
   }
   return AppResult::OK;
}

AppResult GenInputManager::event(AbsEvent* event) {

   if ( runType( ) == LRIH_run_type::monte_carlo_embedded ||
	(runType( ) == LRIH_run_type::monte_carlo_realistic && _rewriteLRIH.value()) ||
	(runType( ) == LRIH_run_type::monte_carlo_run && _rewriteLRIH.value()) )
   {
      // runType is selected by talk-to in the base class, if it is embedded
      // then the LRIH bank read in from the input file must be rewritten
      // because it is no longer real data if track embedding is going on.
      // This module must be the very first module in the path so that it can 
      // override the LRIH bank that came in from the input module before anyone
      // else sees the event.
      if ( verbose() ) 
      {
	 std::cerr << "GenInputManager::event: Creating new LRIH bank"
		   << std::endl;
      }
      EventRecord::ConstIterator LRIH_iter( event, "LRIH_StorableBank" );
      ConstHandle<LRIH_StorableBank> LRIH_ch( LRIH_iter );
      const LRIH_StorableBank* lrih_cp = LRIH_ch.operator->();

      // Casting away const is a nasty thing to do but changing the description of
      // the old LRIH makes it obvious to users that the new LRIH takes precidence.
      // The UNUSED version is intended to be dropped on output
      LRIH_StorableBank* LRIH = const_cast<LRIH_StorableBank*> (lrih_cp);

      // Instead make a new one based on the values in the 
      // generator talk-to, note that AbsEnv will also be updated
      Handle<LRIH_StorableBank> lrih = createLRIH( );


      lrih->set_trigger_number( LRIH_ch->trigger_number() );

      if (runType( ) == LRIH_run_type::monte_carlo_embedded) {
	// Copy these from the original bank (so we don't mess up any needed
	// calibrations)
	lrih->set_run_number( LRIH_ch->run_number() );
	lrih->set_section( LRIH_ch->section() );
	lrih->set_description( "L3Filter" );
      } 
      else {
	// Use run and section numbers from this module
	// which come from run_number or LumiWtRunsFromDB talk-tos
	lrih->set_run_number( _runNumber.value() ); 
	//	lrih->set_section( _sectionNumber.value() ); // Can't do this until it is protected
	lrih->set_section( LRIH_ch->section() );
	lrih->set_description( LRIH_ch->description() );
      }

      // Make sure the environment agrees
      setGlobalRunNumber( lrih->run_number() );
      setGlobalTrigNumber( lrih->trigger_number() );
      setAbsEnvRunSection( lrih->run_section() );

      LRIH->set_description( "Original" );      

      // Now append it to the event
      if ((event->append(lrih)).is_null()) 
      {
	 ERRLOG(ELsevere2,"[GEN_INPUT_BAD_LRIH]")
	    << "GenInputManager::event(): can't add "
	    << "new LRIH_StorableBank"
	    << "to the event."
	    << endmsg;
	 return AppResult::ERROR;
      }
   }
   return AppResult::OK;    
}


AppResult GenInputManager::childBeginRun(AbsEvent* event) {
  // Need to change beginRun record as well
  return GenInputManager::event(event);
}

int GenInputManager::eventRecordType() const {
  AbsEnv* env = AbsEnv::instance();  
  int recordType = LRIH_record_type::physics_event;
  if (env) {
    if (env->currentRecordType() == edm::RecordType::BeginOfRun) {
      recordType = LRIH_record_type::begin_run;
    }
  }
  return recordType;
}
