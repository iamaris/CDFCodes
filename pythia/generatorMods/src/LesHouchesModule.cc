// LesHouches interface module to Pythia.
//
// created                  10/14/2002    Soushi Tsuno
// ...
// uses Pythia 6.2 & Herwig 6.5    10/05/2004 Soushi Tsuno
// add JIMMY 4.1                   11/12/2004 Soushi Tsuno
//
//---------------
// C++ Headers -- MUST use defects !!!!
//---------------
#include <iostream>
#include <iomanip>
#include <math.h>
using std::cout;
using std::endl;
using std::setw;
using std::string;

#include "AbsEnv/AbsEnv.hh"
#include "stdhep_i/Hepevt.hh"
#include "stdhep_i/Heplun.hh"
#include "stdhep_i/Tauevt.hh"

#include "Edm/Handle.hh"
#include "SimulationObjects/HEPG_StorableBank.hh"
#include "SimulationObjects/TAUG_StorableBank.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "generatorMods/LesHouchesModule.hh"
#include "pythia_i/Pythia.hh"
#include "herwig_i/Herwig.hh"
#include "evt/Event.hh"
#include "stdhep_i/CdfHepevt.hh"

#include "generatorMods/HerwigMenu.hh"

extern "C" {
  void upinit_();
  void upevnt_();

  void lesbeginjob_(const char*, int*, int*);
  void lesendjob_();
  void lescheck_();

  void les_tauini_(int*);
  void les_taumain_();

  //================
  // PYTHIA section
  //================

  //================
  // HERWIG section
  //================
  // Initialize for HERWIG.
  void lestoherw_();
  void hwlini_();
  void hweini_();

  // CALL UPINIT
  void hwigup_();
  // generate hard process.
  void hwupro_();
  void hwuinc_();
  void hwuine_();
  void hwbgen_();

  //  v6.1  void hwdhqk_();
  void hwdhob_();
  void hwcfor_();
  void hwcdec_();
  void hwdhad_();
  void hwdhvy_();
  void hwmevt_();
  void hwufne_(); 
  void hwanal0_(int *ihwcod0);
  void hwanal_(int *ihwcod);
  void hwefin_();
  void jmefin_();
  void hwaend_();
  // In Stdhep
  void hwghep_(int *mconv);

  //========
  // STDHEP
  //========
  void stdbeginjob_(const char*,const char*,int*,int*,int*,int*);
  void stdrunwrt_(int*);
  void stdrunred_();
  void stdrunend_(int*,int*);

  //  void stdxwrt_(int*,int*,int*);
}

const char* LesHouchesModule::genId="LesHouchesModule";

LesHouchesModule* LesHouchesModule::_instance = 0;

LesHouchesModule* LesHouchesModule::Instance() {
  static LesHouchesModule::Cleaner cleaner;
  if ( _instance == 0 ) _instance = new LesHouchesModule();
  return _instance;
}

static int init=0;

//----------------
// Constructors --
//----------------
LesHouchesModule::LesHouchesModule() : 
  AbsGenModule( LesHouchesModule::genId, 
		"AC++ LesHouches interface module"),
  _lesgentype("LesGenType", this, "Grappa"),
  _lesdatfile("LesDatFile", this, "test.dat"),
  _lespsmodel("LesPSModel", this, "PYTHIA"),
  _firstEventToRead("firstEventToRead", this, 1),
  _putResonance("putResonance", this, 1),
  _skipResonance("skipResonance", this,0,6,0,-30,30),
  _scaleOfKtClus("scaleOfKtClus", this, false),
  _useTAUOLA("useTAUOLA",this,false),
  _useSTDHEP("useStdhep",this,0),
  _stddatfile("StdhepFileName", this, "stdhep.dat"),
  _useStdEvent("NumOfStdEvent", this, 10),
  _pythiaMenu( new PythiaMenu( this, 0, "PythiaMenu") ),
  _herwigMenu( new HerwigMenu( this, 0, "HerwigMenu") )
{
  // all other grappa stuff
  commands() -> append(&_lesgentype);
  commands() -> append(&_lesdatfile);
  commands() -> append(&_lespsmodel);
  commands() -> append(&_firstEventToRead);
  commands() -> append(&_putResonance);
  commands() -> append(&_skipResonance);
  commands() -> append(&_scaleOfKtClus);
  commands() -> append(&_useTAUOLA);
  commands() -> append(&_useSTDHEP);
  commands() -> append(&_stddatfile);
  commands() -> append(&_useStdEvent);

  // Add descriptions for AbsParms
  _lesgentype.addDescription(
  "      \t\t\tThe name of ME-generator: AlpGen/CompHep/MadGraph/Grappa/"
  "      \t\t\tWgrad/MCFM/PYXTRA-MADGRAPH/MadGraph3/BaurMC/MCatNLO/User.");

  _lesdatfile.addDescription(
  "      \t\t\tInput data file name of a ME-generator.");

  _lespsmodel.addDescription(
  "      \t\t\tModel of PS-generator: PYTHIA/HERWIG.");

  _firstEventToRead.addDescription(
  "      \t\t\tSet the first event number to read from event file.");

  _putResonance.addDescription(
  "      \t\t\tSet resonance particles (W or Z) in the event history,\n"
  "      \t\t\tif lepton pair apears sequentialy in event. 1/0:ON/OFF \n"
  "      \t\t\tThe advanced option to set photon mother info. is available \n"
  "      \t\t\t                    by 2 without resonance, 3 with resonance.");

  _skipResonance.addDescription(
  "      \t\t\tSkip resonance particles (top, W, or Z) in the event history.\n"
  "      \t\t\tSet its particle ID code. (ex.6,-6,23,24,-24,25)");

  _scaleOfKtClus.addDescription(
  "      \t\t\tSet energy scale for the parton shower based on kt-clustering\n"
  "      \t\t\talgorithm. In this case, the PS-scale is overwritten by a new\n"
  "      \t\t\tscale recalculated by kt-cluster. Otherwise, the original\n"
  "      \t\t\tscale in the event file is used. true/false:ON/OFF");

  _useTAUOLA.addDescription(
  "      \t\t\tUse TAUOLA : (bool) true/false:ON/OFF.");
  _useSTDHEP.addDescription(
  "      \t\t\tWRITE or READ the STDHEP event file :\n"
  "      \t\t\t\t 0 for nothing,\n"
  "      \t\t\t\t 1 for WRITE,\n"
  "      \t\t\t\t 2 for READ.\n");
  _stddatfile.addDescription(
  "      \t\t\tOutput file name with STDHEP format.");
  _useStdEvent.addDescription(
  "      \t\t\tNumber of events produced/read in STDHEP output file.");

  // Pythia submenu
  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle( " Pythia Menu " );
  commands()->append( _pythiaMenu );

  // Herwig submenu
  _herwigMenu->initialize("HerwigMenu",this);
  _herwigMenu->initTitle( " Herwig Menu " );
  commands()->append( _herwigMenu );


  // Map Herwig Common blocks to global C structures
  //  InitHerwigCommonBlocks(); 
  // Put contructor initialized  values of Absparms into Herwig Commons

  _herwigMenu -> Absparm2Her();

  // Call Fortran Routine to Initialize Herwig Common Block
  lestoherw_();

  // Transfer numbers from Herwig Common Blocks to Absparms
  _herwigMenu -> Her2Absparm();

}

//--------------
// Destructor --
//--------------
LesHouchesModule::~LesHouchesModule()
{
  delete _pythiaMenu;
  delete _herwigMenu;
}

AppResult LesHouchesModule::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}

AppResult LesHouchesModule::genBeginJob() {

  //========================
  //  Set Input parameters
  //========================
  const char* LesGenType = _lesgentype.value().c_str();
  const char* LesDatFile = _lesdatfile.value().c_str();
  const char* LesPSModel = _lespsmodel.value().c_str();

  _uid = LesGenType;

  LesHouchesModule* lesId = LesHouchesModule::Instance();
  lesId -> setLesHouchesUid(LesGenType);

  //================================================
  // Pass parameter from c++ side to fortran world.
  //================================================
  int iLesPar[9] = {0,0,0,0,0,0,0,0,0};
  //  if (_putResonance.value())  { iLesPar[0] = 1; }
  iLesPar[0] = _putResonance.value();
  if (_scaleOfKtClus.value()) { iLesPar[1] = 1; }

  int nskip = 0;
  for (AbsParmList<int>::ConstIterator it = _skipResonance.begin();
       it != _skipResonance.end(); ++it) {
    iLesPar[nskip+3] = *it;
    ++nskip;
  }
  iLesPar[2] = nskip;

  int cLesLen = strlen(LesDatFile);
  lesbeginjob_(LesDatFile,&cLesLen,&iLesPar[0]);

  // Initialize event counter.
  _events = 0;

  //================
  //  To Pythia...
  //================
  if (_useSTDHEP.value() < 2) {
    if (_lespsmodel.value() == "PYTHIA" 
	|| _lespsmodel.value() == "Pythia") {

      Pythia* pythia = Pythia::Instance();
      if(Pythia::Instance()->pydat3().lenMdme() == 8000 && 
	 Pythia::Instance()->pypars().mstp(182) < 200) {
	errlog(ELfatal,"[PYTHIA_VERS_ERRO]")
	  << "PythiaModule: Common blocks are incorrect for Pythia version,"
	  << "please delete binaries, setup lund v6_203 (or later) and rebuild." 
	  << endmsg;
      }
      // set longer lived Ks and lambdas stable to be decayed in detector
      int index;
      int massCode[] = {310,3112,3122,3222,3312,3322,3334};
      for (int i = 0; i<7; i++) {
	index = Pythia::Instance()->Pycomp(&massCode[i]);
	Pythia::Instance()->pydat3().mdcy(index,1) = 0;
      }
      std::cout<< "PythiaModule: Please note the following long lived strange " 
	       << "particles have been" << std::endl;
      std::cout<< "              set stable and cannot be set to decay"<<std::endl;
      std::cout<< "              K_0S, Lambda, Sigma, Xi and Omega." <<std::endl;
  
      // Prevent pile-up
      if (pythia->pypars().mstp(131)) {
	pythia->pypars().mstp(131)=0;
	ERRLOG(ELerror,"[PYTHIA_NO_PILEUP]")
	  << "PythiaModule: Pile up selected but is not currently supported, "
	  << "now turned off"
	  << endmsg;
      }
  
      // Read a decay table:
      std::string fileMode = _pythiaMenu->decayFileMode();
      std::string fileName = _pythiaMenu->decayFile();
      if (!fileName.empty()) {
	if (fileMode=="READ" || fileMode=="read") {
	  int mode=_pythiaMenu->decayTableMode();
	  int lun=37;
	  if (pythia->Opdcay(fileName,&lun,"R")) {
	    ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	      << "PythiaModule: decay table file " 
	      << "\"" << fileName << "\"\n"
	      << "cannot be opened for read!" << endmsg;
	  } else {
	    pythia->Pyupda(&mode,&lun);
	    pythia->Cldcay(&lun);
	    ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R")
	      << "PythiaModule: read decay table file "
	      << "\"" << fileName << "\"" << endmsg;
	  }
	}
      } 
      // if non zero value of mstp(142) set up weighting
      if(pythia->pypars().mstp(142)) {
	double expwt = _pythiaMenu->expWeight();
	int param = _pythiaMenu->weightParam();
	pythia->Pyevwt(&param, &expwt);
      }
      std::string frame  = _pythiaMenu->frame();
      std::string target = _pythiaMenu->getTarget();
      double winval = _pythiaMenu->win();
      int minlist   = _pythiaMenu->initlistlevel();
      pythia->pysubs().msel() = _pythiaMenu->msel();
      //  pythia->pydatr().mrpy(1) = _pythiaMenu->randomseed();
      std::string pygiveFile = _pythiaMenu->pygiveFile();
      if(pygiveFile.size()) {
	std::cout << "Pygive file  " << pygiveFile << std::endl;
	pythia->Pygive(pygiveFile);
      }
      std::cout << "call pyinit" << std::endl;
      //=========================
      //  Inintialize of Pythia
      //=========================
      frame = "USER";
      pythia->Pyinit(frame,target,&winval);

      pythia->setEvents(0);
      pythia->setEventListLevel(minlist);
      pythia->Pylist(&minlist);
      // Temporary dummy call to pyofsh to force private version to be used
      pythia->Pyofsh();

      // Write a new decay table:
      if (!fileName.empty()) {
	if (fileMode=="WRITE" || fileMode=="write") {
	  int mode=1;
	  int lun=37;
	  if (pythia->Opdcay(fileName,&lun,"W")) {
	    ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	      << "PythiaModule: decay table file " 
	      << "\"" << fileName << "\"\n"
	      << "cannot be opened for write!" << endmsg;
	  } else {
	    pythia->Pyupda(&mode,&lun);
	    pythia->Cldcay(&lun);
	    ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_W]")
	      << "PythiaModule: wrote decay table file "
	      << "\"" << fileName << "\"" << endmsg;
	  }
	}
      }
      pythia->setFirstListEvent( _pythiaMenu->firstlistevent() );
      pythia->setLastListEvent ( _pythiaMenu->lastlistevent() );
      pythia->setEventListLevel(_pythiaMenu->eventlistlevel());
      std::cout << " Event list level " 
		<< _pythiaMenu->eventlistlevel()
		<< std::endl;
    }
    //================
    //  To Herwig...
    //================
    else if (_lespsmodel.value() == "HERWIG" 
	     || _lespsmodel.value() == "Herwig") {
      // Since we may have changed parameters via Talk-to,
      // we need to refill Herwig Common blocks from the 
      // AbsParms

      _herwigMenu -> Absparm2Her();

      std::cout << "Initialize HERWIG.AAAAAAA" << std::endl;
      hwlini_();
      std::cout << "End HERWIG.AAAAAAA" << std::endl;

      //preset event counter
      init = 1;
    }
    else {
      std::cout << " Error::LesHouchesModule: Wrong name of PS-generator: " 
		<< LesPSModel << " ." << std::endl;
      std::cout << " Type PYTHIA or HERWIG." << std::endl;
      std::cout << " Execution stopped." << std::endl;
      abort();    
    }

    //===================
    // Initialize TAUOLA
    //===================
    if (_useTAUOLA.value()) {
      std::cout << "call tauini" << std::endl;
      int newtmp = 100;
      les_tauini_(&newtmp);
    }
  }

  //===================
  // Initialize STDHEP
  //===================
  if (_useSTDHEP.value()) {
    // initialize HEP logical units
    Heplun heplun;
    heplun.lnhwrt() = 23;
    heplun.lnhrd()  =  0;
    heplun.lnhdcy() =  0;
    heplun.lnhout() =  6;

    char lstdblk[100];
    sprintf(lstdblk,"StdHep/%s/%s",LesPSModel,LesGenType);

    int lmxevt = _useStdEvent.value();
    int cStdDatFile = strlen(_stddatfile.value().c_str());
    int cLstdBlk    = strlen(lstdblk);
    int iredorwrt   = _useSTDHEP.value();

    if (iredorwrt == 1) {
      // Set output STDHEP file.
      std::cout << "Output STDHEP file name : " 
		<< _stddatfile.value().c_str() << std::endl;
    }
    else if (iredorwrt == 2) {
      // Set input STDHEP file.
      std::cout << "Input STDHEP file name : "
		<< _stddatfile.value().c_str() << std::endl;
    }
    else {
      std::cout << " Error::LesHouchesModule: Wrong integer of : useStdhep " 
		<< iredorwrt << " ." << std::endl;
      std::cout << " Set 0 for nothing, 1 for WRITE, or 2 for READ." 
		<< std::endl;
      std::cout << " Execution stopped." << std::endl;
      abort();
    }

    // Call STDHEP routines.
    stdbeginjob_(_stddatfile.value().c_str(),lstdblk,
		 &cStdDatFile,&cLstdBlk,&lmxevt,&iredorwrt);
    _mxevt = lmxevt;
  }

  //====================================
  // Skip N events from the event file.
  //====================================
  std::cout << "The events are stored in HEPG from " 
	    << _firstEventToRead.value() << "-th event." << std::endl;
  for (int i = 1; i < _firstEventToRead.value(); i++) {
    if (_useSTDHEP.value() == 2) { stdrunred_(); }
    else { Upevnt(); }
    if (i%500 == 0) { std::cout << "skip " << i << " events..." << std::endl; }
  }
  std::cout << "End beginjob..." << std::endl;

  return AppResult::OK;
}

int LesHouchesModule::callGenerator(AbsEvent* anEvent) {

  CdfHepevt* hepevt = CdfHepevt::Instance();

  if (_useSTDHEP.value() == 2) {
    if (_events <= _mxevt) {
      stdrunred_();
    }
    else {
      std::cout << "Total number of events ("
		<< _mxevt << ") in the input file was exhausted, "
		<< "but Run will be continued until proper endjob." 
		<< std::endl;
      std::cout << "HEPG was not created in this event. " 
		<< _events << std::endl;
    }
  }
  else {
    if (_lespsmodel.value() == "PYTHIA" 
	|| _lespsmodel.value() == "Pythia") {

      Pythia* pythia = Pythia::Instance();

      // Generate the event
      pythia->Pyevnt();
      bool eventList = ( pythia->events() >= pythia->FirstListEvent() && 
			 pythia->events() <= pythia->LastListEvent() );

      if (_pythiaMenu->selectPdg()!=0) {
	int code = _pythiaMenu->selectPdg();
	int icount = 0;
	while(pythia->findPdgCode(&code)==false) {
	  if ( verbose() ) {
	    if (++icount%100==0 && eventList) { 
	      std::cout << name() << "::callGenerator generated " 
			<< icount << std::endl;
	    }
	  }
	  pythia->Pyevnt();
	}
      }

      // Update our event counter 
      pythia->addEvent();  // See if we are supposed to list this event  
      int mevlist= pythia->EventListLevel();
      if ( eventList ) {
	std::cout << "PYEVNT event no. " << pythia->events() 
		  <<" level " << mevlist << std::endl;
	pythia->Pylist(&mevlist); // List this event if required
      }
    }
    else if (_lespsmodel.value() == "HERWIG" 
	     || _lespsmodel.value() == "Herwig") {

      // generates single event with HERWIG
      //* CdfHepevt* hepevt = CdfHepevt::Instance();

      const int mxerr = 100;
      int nterr = 0;
      bool goodev = false;
      
      if (init == 0) {
	std::cout << " Herwig not properly initialized" << std::endl;
      }

      //*      while (!goodev) {    
      hepevt -> HepevtPtr() -> NEVHEP = _events;
      //*	hwuine_();  // Initialize event

      //==================
      //  Event gneration
      //==================
      hwupro_();
      goodev=true;
    }

    //============
    // use TAUOLA
    //============
    if (_useTAUOLA.value()) {
      les_taumain_();  
    }
  }
  _events++;

  lescheck_();
  
  return 0;
}

void LesHouchesModule::fillHepevt() {

  if (_useSTDHEP.value() == 2) {
  }
  else {
    if (_lespsmodel.value() == "PYTHIA" 
	|| _lespsmodel.value() == "Pythia") {
      Pythia* pythia = Pythia::Instance();
      // Tell lunhep to convert from PYJETS to HEPEVT
      int mconv=1;                          
      // Do the conversion
      pythia->Lunhep(&mconv); // Use STDHEP translation routine 1999/01/27 CG
      CdfHepevt::Instance()->HepevtPtr()->NEVHEP = pythia->events();

      // fill in some HEPEV4 info, courtesy of Dmitri Tsybychev
      CdfHepevt::Instance()->Hepev4Ptr()->SCALELH[5] =
	pythia->pypars().pari(17);

      CdfHepevt::Instance()->Hepev4Ptr()->EVENTWEIGHTLH=
	pythia->pypars().pari(10);
      //dt.end

      // Fill Stdhep common block 1
      if (_useSTDHEP.value() == 1) {
	if (_events <= _mxevt) {
	  int ipsgen = 1;
	  stdrunwrt_(&ipsgen);
	}
      }
    }
    else if (_lespsmodel.value() == "HERWIG" 
	     || _lespsmodel.value() == "Herwig") {
      int mconv = 1;
      hwghep_(&mconv);

      Herwig* herwig = Herwig::Instance();
      CdfHepevt::Instance()->Hepev4Ptr()->EVENTWEIGHTLH = 
	herwig->Hwevnt()->evwgt;
      int i = 0;
      while ((CdfHepevt::Instance()->HepevtPtr()->ISTHEP[i] != 123)  
	     &&  (i < CdfHepevt::Instance()->HepevtPtr()->NHEP)) {
	i++;
      }

      float px = CdfHepevt::Instance()->HepevtPtr()->PHEP[i][0]; 
      float py = CdfHepevt::Instance()->HepevtPtr()->PHEP[i][1]; 

      float pthat = px*px+py*py;
      if ( pthat > 0.0 ) {
	pthat=std::sqrt(px*px+py*py);
      }
      else {
	pthat = 0.;
      }
      CdfHepevt::Instance()->Hepev4Ptr()->SCALELH[5]  = pthat;

      // Fill Stdhep common block 1
      if (_useSTDHEP.value() == 1) {
	if (_events <= _mxevt) {
	  int ipsgen = 2;
	  stdrunwrt_(&ipsgen);
	}
      }

    }
  }
}

AppResult LesHouchesModule::genEndRun(AbsEvent* aRun) {

  if (_lespsmodel.value() == "PYTHIA" 
      || _lespsmodel.value() == "Pythia") {
    Pythia* pythia = Pythia::Instance();
    int mstat = _pythiaMenu->statlistlevel();
    std::cout << "Call PYSTAT at endRun" << std::endl;
    pythia->Pystat(&mstat);
  }

  return AppResult::OK;
}

AppResult LesHouchesModule::genEndJob() {

  lesendjob_();

  if (_useSTDHEP.value() == 2) {
    int ipsgen = 3;
    stdrunend_(&ipsgen,&_mxevt);
  }
  else {
    if (_lespsmodel.value() == "PYTHIA" 
	|| _lespsmodel.value() == "Pythia") {
      Pythia* pythia = Pythia::Instance();
      int mstat = _pythiaMenu->statlistlevel();
      std::cout << "Call PYSTAT at endJob" << std::endl;
      pythia->Pystat(&mstat);

      // Write Stdhep end-run record
      if (_useSTDHEP.value() == 1) {
	int ipsgen = 1;
	stdrunend_(&ipsgen,&_mxevt);
      }
    }
    else if (_lespsmodel.value() == "HERWIG" 
	     || _lespsmodel.value() == "Herwig") {
      CdfHepevt* hepevt = CdfHepevt::Instance();
      hepevt->HepevtPtr()->NEVHEP = _events;

      hwefin_();
      jmefin_();
      // User terminal calculations if wanted
      hwaend_();

      // Write Stdhep end-run record
      if (_useSTDHEP.value() == 1) {
	int ipsgen = 2;
	stdrunend_(&ipsgen,&_mxevt);
      }
    }
  }

  return AppResult::OK;
}

void LesHouchesModule::Upinit() {upinit_(); }

void LesHouchesModule::Upevnt() {upevnt_(); }

LesHouchesModule::Cleaner::~Cleaner()
{
  delete LesHouchesModule::_instance;
  LesHouchesModule::_instance = 0;
}
