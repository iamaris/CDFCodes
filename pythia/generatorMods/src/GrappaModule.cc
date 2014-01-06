// Grappa interface module to Pythia.
// (GRace At Prton-Prton/Anti-proton)
//
// created                  02/14/2002    Soushi Tsuno
// add TAUOLA               03/15/2002    Soushi Tsuno
// upgraded to PYTHIA 6.2   03/26/2002    Soushi Tsuno
// add W + 0,1,2 jets       06/14/2002    Soushi Tsuno
//

#include "generatorMods/GrappaModule.hh"
#include "pythia_i/Pythia.hh"
#include "stdhep_i/CdfHepevt.hh"

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

extern "C" {
  //  void pyupev_(int*, double*);

  void upinit_();
  void upevnt_();
  void grcgproc_(int*,int*,int*);

  //  void gtauini_(int*);
  //  void gtaumain_();
}

const char* GrappaModule::genId="Grappa";
const long GrappaModule::_defaultRandomSeed1 = 922883591;
const long GrappaModule::_defaultRandomSeed2 = 109735476;

//----------------
// Constructors --
//----------------
GrappaModule::GrappaModule() : 
  AbsGenModule( GrappaModule::genId, 
		"AC++ Grappa module"),
  _grcecm("CMEnergy",  this, 1960.),
  _grcpap("Beam",      this, "PAP"),
  _gptcut("gPtcut",    this,   5.0),
  _getacut("gEtacut",  this,   3.0),
  _grconcut("gRconcut",this,   0.1),
  _hmas("hmass",       this, 120.0),
  _hwid("hwidth",      this,   6.54e-3),
  _tmas("tmass",       this, 175.0),
  _twid("twidth",      this,   1.59),
  _bmas("bmass",       this,   4.8),
  _cmas("cmass",       this,   1.5),
  _ijetflv("JetFlv",   this,0, 8,    1,  0,   1),
  _igsub("iGsub",      this,0,50,  401,400, 550),
  _incall("iNcall",    this,0,50,100000,0,9999999),
  _ibswrt("iBswrt",    this,0,50,    0,  0,  12),
  _inpfl("iNpfl",      this,0,50,    4,  1,   5),
  _icoup("iCoup",      this,0,50,    1,  1,   6),
  _ifact("iFact",      this,0,50,    0,  0,   6),
  _isdep("iSdep",      this,0,50,    0,  0,   1),
  _grcq("GrcQ",        this,0,50,  0.0,0.0,9999.0),
  _grcfaq("GrcfaQ",    this,0,50,  0.0,0.0,9999.0),
  _grcfile("GrcFile",  this,0,50,"bases"),
//*  _useTAUOLA("useTAUOLA",this,false),
  _pythiaMenu( new PythiaMenu( this, 0, "PythiaMenu") ),
  _randomNumberMenu(),
  _randomSeed1("RandomSeed1",this,GrappaModule::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2",this,GrappaModule::_defaultRandomSeed2)
{
  _initializeTalkTo();
}

//--------------
// Destructor --
//--------------

GrappaModule::~GrappaModule()
{
  delete _pythiaMenu;
}

AppResult GrappaModule::genBeginRun(AbsEvent* aRun) {
  return AppResult::OK;
}

AppResult GrappaModule::genBeginJob() {

  const int ProcNum = 33;

  int iGrcSetParm[500],iGrcSetJet[8];
  double rGrcSetParm[500];
  char cGrcSetParm[ProcNum][60];
  double rGrcSetCut[8],rGrcSetMass[8];

  const char cGrcPar[9][9] = {
    "iNcall","iBswrt","iNpfl","iCoup","iFact","iSdep",
    "GrcQ","GrcfaQ","GrcFile"
  };

  //============
  // Initialize
  //============
  for (int i = 0; i < 8; ++i) { 
    rGrcSetCut[i]  = 0.0;
    rGrcSetMass[i] = 0.0;
    iGrcSetJet[i] = 0;
  }

  //======================
  //  Default Parameters
  //======================
  const int iGrcParDef[ProcNum][7] = {
    {400,  300000, 0, 4, 5, 0, 0},
    {401,   10000, 0, 4, 5, 0, 0},
    {402, 1000000, 0, 4, 5, 0, 0},
    {403,  200000, 0, 4, 5, 0, 0},
    {404,   20000, 0, 4, 5, 0, 0},
    {405,  100000, 0, 4, 2, 0, 0},
    {406,   10000, 0, 4, 2, 0, 0},
    {407,  100000, 0, 4, 5, 0, 0},
    {408,   10000, 0, 4, 5, 0, 0},
    {409,  500000, 0, 4, 5, 0, 0},
    {410,  200000, 0, 4, 5, 0, 1},
    {411,   10000, 0, 4, 5, 0, 1},
    {412,   10000, 0, 4, 5, 0, 0},
    {413,  300000, 0, 4, 5, 0, 0},
    {414,  200000, 0, 4, 5, 0, 1},
    {415,   10000, 0, 4, 5, 0, 1},
    {416,  100000, 0, 4, 5, 0, 0},
    {417,   50000, 0, 4, 5, 0, 0},
    {418,  500000, 0, 4, 5, 0, 0},
    {419,   10000, 0, 4, 6, 0, 0},
    {420,   10000, 0, 4, 6, 0, 0},
    {421,    5000, 0, 5, 5, 0, 0},
    {422,   50000, 0, 5, 5, 0, 0},
    {423,   50000, 0, 5, 5, 0, 0},
    {424,  200000, 0, 5, 5, 0, 0},
    {425, 1000000, 0, 5, 5, 0, 0},
    {426,  200000, 0, 5, 5, 0, 0},
    {427,  300000, 0, 5, 5, 0, 0},
    {428, 1000000, 0, 5, 5, 0, 0},
    {429,  200000, 0, 5, 5, 0, 0},
    {430,  300000, 0, 5, 5, 0, 0},
    {500,  100000, 0, 5, 5, 0, 0},
    {501, 1000000, 0, 5, 5, 0, 0}};

  const double rGrcParDef[ProcNum][3] = {
    {400, 120.0,   0},
    {401, 120.0,   0},
    {402,  91.188, 0},
    {403,  91.188, 0},
    {404, sqrt(120.0*120.0 + 91.188*91.188),   0},
    {405,   0.0,   0},
    {406,   0.0,   0},
    {407, sqrt(2.0)*91.188, 0},
    {408,  80.45,  0},
    {409, sqrt(175.0*175.0+80.45*80.45),  0},
    {410,  91.188, 0},
    {411,  91.188, 0},
    {412,  80.45,  0},
    {413,  80.45,  0},
    {414,  91.188, 0},
    {415,  91.188, 0},
    {416,  91.188, 0},
    {417,  91.188, 0},
    {418,  80.45,  0},
    {419,   0.0,   0},
    {420,   0.0,   0},
    {421,  80.45,  0},
    {422,  80.45,  0},
    {423,  80.45,  0},
    {424,  80.45,  0},
    {425,  80.45,  0},
    {426,  80.45,  0},
    {427,  80.45,  0},
    {428,  80.45,  0},
    {429,  80.45,  0},
    {430,  80.45,  0},
    {500,  80.45,  0},
    {501,  80.45,  0}};
      
  const char cGrcParDef[ProcNum][10] 
    = {"bases_400","bases_401","bases_402","bases_403","bases_404",
       "bases_405","bases_406","bases_407","bases_408","bases_409",
       "bases_410","bases_411","bases_412","bases_413","bases_414",
       "bases_415","bases_416","bases_417","bases_418","bases_419",
       "bases_420","bases_421","bases_422","bases_423","bases_424",
       "bases_425","bases_426","bases_427","bases_428","bases_429",
       "bases_430","bases_w1j","bases_w2j"};

  //=================================
  //  Set CM Energy, Beam Particles
  //=================================
  double GrcEcmval = _grcecm.value();
  const string& GrcBeam = _grcpap.value();
  rGrcSetParm[0] = GrcEcmval;

  rGrcSetCut[0] = _gptcut.value();
  rGrcSetCut[1] = _getacut.value();
  rGrcSetCut[2] = _grconcut.value();

  rGrcSetMass[0] = _hmas.value();
  rGrcSetMass[1] = _hwid.value();
  rGrcSetMass[2] = _tmas.value();
  rGrcSetMass[3] = _twid.value();
  rGrcSetMass[4] = _bmas.value();
  rGrcSetMass[5] = _cmas.value();

  //========================
  //  Set input parameters
  //========================
  AbsParmList<int>::ConstIterator igsub  = _igsub.begin();
  AbsParmList<int>::ConstIterator incall = _incall.begin();
  AbsParmList<int>::ConstIterator ibswrt = _ibswrt.begin();
  AbsParmList<int>::ConstIterator inpfl  = _inpfl.begin();
  AbsParmList<int>::ConstIterator icoup  = _icoup.begin();
  AbsParmList<int>::ConstIterator ifact  = _ifact.begin();
  AbsParmList<int>::ConstIterator isdep  = _isdep.begin();

  AbsParmList<double>::ConstIterator grcq     = _grcq.begin();
  AbsParmList<double>::ConstIterator grcfaq   = _grcfaq.begin();

  int nproc = 0;
  for (AbsParmList<int>::ConstIterator it = _igsub.begin(); 
       it != _igsub.end(); ++it) {
    ++nproc;
  }

  for (int i = 0; i < nproc; ++i, ++igsub, ++incall, ++ibswrt, 
	 ++inpfl, ++icoup, ++ifact, ++isdep, ++grcq, ++grcfaq) {
    iGrcSetParm[i]         = *igsub;
    iGrcSetParm[i+nproc]   = *incall;
    iGrcSetParm[i+2*nproc] = *ibswrt;
    iGrcSetParm[i+3*nproc] = *inpfl;
    iGrcSetParm[i+4*nproc] = *icoup;
    iGrcSetParm[i+5*nproc] = *ifact;
    iGrcSetParm[i+6*nproc] = *isdep;

    rGrcSetParm[i+1]         = *grcq;
    rGrcSetParm[i+nproc+1]   = *grcfaq;
  }

  int i = 0;
  for (AbsParmList<std::string>::ConstIterator grcfile = _grcfile.begin();
       grcfile != _grcfile.end(); ++grcfile, ++i) {
    std::string tmpbsfile = *grcfile;
    strcpy(cGrcSetParm[iGrcSetParm[i]-400],tmpbsfile.c_str());
  }

  //============
  //  Debug...
  //============
  //Set the size of each parameter.
  int iGrcParSize[12], isubtmp;

  iGrcParSize[0]  = _incall.size();
  iGrcParSize[1]  = _ibswrt.size();
  iGrcParSize[2]  = _inpfl.size();
  iGrcParSize[3]  = _icoup.size();
  iGrcParSize[4]  = _ifact.size();
  iGrcParSize[5]  = _isdep.size();
  iGrcParSize[6]  = _grcq.size();
  iGrcParSize[7]  = _grcfaq.size();
  iGrcParSize[8]  = _grcfile.size();

  std::cout << "Nr. of Process " << nproc << std::endl;

  for (int j = 0; j < 9; ++j) {
    if (iGrcParSize[j] > nproc) {
      std::cout << " Grappa::Error: different Nr. of " << cGrcPar[j] 
		<< " = " << iGrcParSize[j] << std::endl;
      abort();
    }
    else if (iGrcParSize[j] < nproc) {
      for (int i = iGrcParSize[j]; i < nproc; ++i) {

	if (iGrcSetParm[0] < 500) isubtmp = iGrcSetParm[i]-400;
	else                      isubtmp = iGrcSetParm[i]-469;

        if (j < 6) {
	  std::cout << " Use default parameter of " << cGrcPar[j] 
		    << " ( = " << iGrcParDef[(isubtmp)][j+1] 
		    << " ) in process = " << iGrcSetParm[i] << std::endl;

	  cout << iGrcParDef[(isubtmp)][j+1] << " " 
	       << iGrcParDef[isubtmp][j+1] << " " << isubtmp << endl;
	  
	  iGrcSetParm[i+(j+1)*nproc] = iGrcParDef[isubtmp][j+1];
        }
        else if (j < 8) {
          std::cout << " Use default parameter of " << cGrcPar[j] 
		    << " ( = " << rGrcParDef[isubtmp][j-5]
		    << " ) in process = " << iGrcSetParm[i] << std::endl;
	  
          rGrcSetParm[i+(j-6)*nproc+1] = rGrcParDef[isubtmp][j-5];
        }
        else if (j == 8) {
          std::cout << " Use default filename of " << cGrcPar[j] 
		    << " ( = " << cGrcParDef[isubtmp]
		    << " ) in process = " << iGrcSetParm[i] << std::endl;

          strcpy(cGrcSetParm[isubtmp],cGrcParDef[isubtmp]);
        }
      }
    }
  }
  // check jet flavor
  if (_ijetflv.size() > 8) {
    std::cout << " Grappa: too large of Nr. of jet flavors... "
	      << "Execution stopped." << std::endl;
    abort();
  }
  
  int itmp = 0;
  for (AbsParmList<int>::ConstIterator ijetflv = _ijetflv.begin();
       ijetflv != _ijetflv.end(); ++ijetflv) {
    iGrcSetJet[itmp] = *ijetflv;
    ++itmp;
  }

  // check NOT arrowed flavor
  if (iGrcSetJet[5] == 1) {
    std::cout << " Currently, Grappa does not support t-quark jets." 
	      << " ...proceeded as JetFlv[5] = 0..." << std::endl;
    iGrcSetJet[5] = 0;
  }

  // check process
  if (nproc > 1) {
    for (int j = 0; j < nproc; ++j) {
      if (iGrcSetParm[j] > 499) {
	std::cout << " Process::" << iGrcSetParm[j] 
		  << " is only arrowed for single process." << std::endl;
	std::cout << " Shoud not use other process numbers." << std::endl;
	abort();
      }
    }
  }

  //================
  //  To GR@PPA...
  //================
  grcgive_(&nproc,iGrcSetParm,iGrcSetJet,rGrcSetParm,rGrcSetCut,rGrcSetMass);

  for (int i = 0; i < nproc; ++i) {
    char* bsfile;
    if (iGrcSetParm[0] < 500) bsfile = cGrcSetParm[iGrcSetParm[i]-400];
    else                      bsfile = cGrcSetParm[iGrcSetParm[i]-469];

    int cGrcLen = strlen(bsfile);
    grcgivec_(bsfile,&cGrcLen,&iGrcSetParm[i]);
  }

  std::cout << "OK grcgive" << std::endl;

  //================
  //  To Pythia...
  //================
  const char* grccol = GrcBeam.c_str();

  Pythia* pythia = Pythia::Instance();

  // duplicate some stuff

  pythia->setEventListLevel( _pythiaMenu->eventlistlevel());
  pythia->setFirstListEvent( _pythiaMenu->firstlistevent());
  pythia->setLastListEvent (  _pythiaMenu->lastlistevent());

  // Read a decay table:
  const string& fileMode = _pythiaMenu->decayFileMode();
  const string& fileName = _pythiaMenu->decayFile();
  if (fileMode=="READ" || fileMode=="read") {
    if (fileMode.empty()) {
      ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	<< "GrappaModule: decay table read selected, but no filename set"
	<< endmsg;
    } else {
      int mode=2;
      int lun=37;
      if (pythia->Opdcay(fileName.c_str(),&lun,"R")) {
	ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	  << "GrappaModule: decay table file " 
	  << "\"" << fileName << "\"\n"
	  << "cannot be opened for read!" << endmsg;
      } else {
	pythia->Pyupda(&mode,&lun);
	pythia->Cldcay(&lun);
	ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_R]")
	  << "GrappaModule: read decay table file "
	  << "\"" << fileName << "\"" << endmsg;
      }
    }
  }
  
  const char* frame = pythia->frame().c_str();
  const char* beam = pythia->beam().c_str();
  double winval = _pythiaMenu->win();
  std::string target = _pythiaMenu->getTarget();
  int minlist   = _pythiaMenu->initlistlevel();
  pythia->pysubs().msel() = _pythiaMenu->msel();

  std::cout << "call pyinit" << std::endl;
  // Note the `extra' arguments to pyinit on the end of the argument
  // list which do *not* explicitly occur in the FORTRAN
  // implementation of the function -- one for each character
  // variable, in the same order (reference Randy Herber,
  // herber@fnal.gov). Note also that the `extern "C"' function
  // declaration matches the C usage rather than the FORTRAN
  // definition of the PYINIT function and therefore includes the 3
  // extra arguments.

  //=========================
  //  Inintialize of Pythia
  //=========================
  //*  std::cout << "Grappa: trying to force PYUPEV " << std::endl;
  pythia->setPythiaUid("Grappa");

  // Call Grappa directly.
  std::cout << "  GrappaModule::genBeginJob : passing cuts and parameters to Grappa ..." << std::endl;

  //===============
  //  Integration
  //===============
  for (int i = 0; i < nproc; ++i) {
    int newsub = iGrcSetParm[i];
    grcgproc_(&nproc,&i,&newsub);
  }

  const char* frame62 = "USER";
  const char* dummy62 = " ";
  double winval62 = 0.0;
  pythia->Pyinit(frame62,dummy62,&winval62);

  pythia -> Pylist(&minlist);
  // Temporary dummy call to pyofsh to force private version to be used
  pythia -> Pyofsh();
  pythia -> setEvents(0);
  
  // Write a new decay table:
  if (fileMode=="WRITE" || fileMode=="write") {
    if (fileMode.empty()) {
      ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	<< "GrappaModule: decay table write selected, but no filename set"
	<< endmsg;
    } else {
      int mode=1;
      int lun=37;
      if (pythia->Opdcay(fileName.c_str(),&lun,"W")) {
	ERRLOG(ELerror,"[PYTHIA_NO_DCYTBL]")
	  << "GrappaModule: decay table file " 
	  << "\"" << fileName << "\"\n"
	  << "cannot be opened for write!" << endmsg;
      } else {
	pythia->Pyupda(&mode,&lun);
	pythia->Cldcay(&lun);
	ERRLOG(ELsuccess,"[PYTHIA_DCYTBL_W]")
	  << "GrappaModule: wrote decay table file "
	  << "\"" << fileName << "\"" << endmsg;
      }
    }
  }


  //===================
  // Initialize TAUOLA
  //===================

//*  if (_useTAUOLA.value()) {
//*    std::cout << "call gtauini" << std::endl;
//*    int newtmp = 100;
//*    gtauini_(&newtmp);
//*  }

  std::cout << "End beginjob..." << std::endl;

  return AppResult::OK;
}


int GrappaModule::callGenerator(AbsEvent* anEvent) {

  Pythia* pythia = Pythia::Instance();

  // Generate the event
  pythia -> Pyevnt();
  // Update our event counter 
  pythia -> addEvent();

  // See if we are supposed to list this event  
  int mevlist= pythia->EventListLevel();
  if ( pythia -> events() >= pythia -> FirstListEvent() && 
       pythia -> events() <= pythia -> LastListEvent() ) {
    std::cout << "PYEVNT event no. " << pythia->events() << std::endl;
    pythia->Pylist(&mevlist); // List this event if required
  }
  
  return 0;
}

void GrappaModule::fillHepevt() {

  Pythia* pythia = Pythia::Instance();
  // Tell lunhep to convert from PYJETS to HEPEVT
  int mconv=1;				
  // Do the conversion
  //*  pythia->Pylist(&mconv);

  pythia->Lunhep(&mconv); // Use STDHEP translation routine 1999/01/27 CG
  CdfHepevt::Instance()->HepevtPtr()->NEVHEP = pythia->events();

}

AppResult GrappaModule::genEndRun(AbsEvent* aRun) {

  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  std::cout << "Call PYSTAT at endRun" << std::endl;
  pythia->Pystat(&mstat);

  return AppResult::OK;
}

AppResult GrappaModule::genEndJob() {

  for (AbsParmList<int>::ConstIterator it = _ibswrt.begin(); 
       it != _ibswrt.end(); ++it) {
    int ibspar = *it;
    grcendjob_(&ibspar);
  }

  Pythia* pythia = Pythia::Instance();
  int mstat = _pythiaMenu->statlistlevel();
  std::cout << "Call PYSTAT at endJob" << std::endl;
  pythia->Pystat(&mstat);
  
  return AppResult::OK;
}

