//--------------------------------------------------------------------------
// Description:
//	Class VecbosModule
//
// Wed Jul 11 16:08:15 CDT 2001, G.Velev fix a bug in calculation of PDF set
//                               add vegas read/write flag
//                               return calls to ybos_init_framework which Lena
//                               removed, need to write correctly VECW bank
//
// rev.  aug 30 2001 lena: added  

#include "generatorMods/VecbosModule.hh"

#include "Bbos/bank_interface.hh"
#include "Framework/APPFramework.hh"
#include "ParticleDB/hepevt.hh"
#include "vecbos_i/Vecbos.hh"
#include "ParticleDB/ParticleDb.hh"
#include "evt/Event.hh"
#include "r_n/CdfRn.hh"
#include <sstream>
using std::ostringstream ;

const long VecbosModule::_defaultRandomSeed1 = 9228;
const long VecbosModule::_defaultRandomSeed2 = 1097;

const char* VecbosModule::genId = "VecbosModule";

VecbosModule::VecbosModule() : 
  AbsGenModule( VecbosModule::genId, "Vecbos module"),
  _debug("debug",this,false),
  _beam_type("beam",this,1),   
  _which_boson("wORz",this,1),
  _decay_boson("decayWorZ",this,true),
  _charge_of_W("chargeOfW",this,0),
  _howto_decay_Z("zDecayMode",this,1),
  _njets("njets",this,3),
  _structure_function("strFunct",this,6),
  _pdf_group("pdfGroup",this,3),
  _pdf_set("pdfSet",this,30),
  _run_number("runNumber",this,1),
  _cm_energy("energyCM",this,1960.),
  _qcd_scale("qcdScale",this,1),
  _helicity("helicity",this,0),
  _force_bbbar("bbBar",this,0),
  _important("important",this,1),
  _sampling_mode("samplingMode",this,2),
  _lepton_type("leptonType",this,1),
  _matrix_ele("matrixElement",this,1),
  _alpha_jet_generation("AlphaPt",this,0.019),
  _subprocesses("subprocesses",this,100),
  _make_jets("makeJets",this,false),
  _jet_min_pt("jetMinPt",this,8.0),
  _jet_sum_pt("jetsumPt",this,0.0),
  _jet_eta_max("jetEtaMax",this,2.5),
  _jet_eta_separation("jetEtaSep",this,0.4),
  _boson_min_pt("bosonMinPt",this,0.0),
  _no_of_leptons("noOfLeptons",this,1),
  _lep_max_eta("lepMaxEta",this,2.5),
  _lep_min_pt("lepMinPt",this,12.),
  _mis_pt_min("misPtMin",this,0.0),
  _min_weght("minWeigth",this,0.0),
  _vegas_rw_flag("rwGrid",this,1),
  _vegas_inp_grid_file("gridInputName",this,"input_test.grid"),
  _vegas_out_grid_file("gridOutputName",this,"output_test.grid"),
  _vegas_print("vegasPrint",this,0),
  _vegas_n_inter("vegasNInter",this,5),
  _vegas_n_calls1("vegasNCalls1",this,0),
  _vegas_n_calls2("vegasNCalls2",this,0),
  _randomSeed1("RandomSeed1",this,VecbosModule::_defaultRandomSeed1),
  _randomSeed2("RandomSeed2",this,VecbosModule::_defaultRandomSeed2)
{	
 
  //initialize talk-to
  _initializeTalkTo();

  // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Fake event random number menu");
  commands()->append(&_randomNumberMenu);

  // Add the commands to the menu
  _randomNumberMenu.commands()->append(&_randomSeed1);
  _randomNumberMenu.commands()->append(&_randomSeed2);

  ostringstream tmpSstream1;
  ostringstream tmpSstream2;

  // Describe them
  tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed1.value() << ").";
  _randomSeed1.addDescription(tmpSstream1.str());
  tmpSstream2 << "      \t\t\tSeed #2 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed2.value() << ").";  
  _randomSeed2.addDescription(tmpSstream2.str());

}


VecbosModule::~VecbosModule() {
}

//_____________________________________________________________________________
AppResult VecbosModule::genBeginJob() {

  CdfRn* rn = CdfRn::Instance();
  if ( !rn->isReadingFromFile() ) {
    rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(), "VECBOS");
  }

  // initialize FORTRAN VECBOS code
  Vecbos* fEventGenerator = Vecbos::Instance();
  fEventGenerator->Init();

  // communicate with common blocks
  putParametersToVecbos();
  getParametersFromVecbos();

  //->-> end moved

//  _hepevt.init();
  _events = 0;
                             // init Vecbos Parameters from  Absparms
  putParametersToVecbos();
  Vecbos::Instance()->Vecjnk()->NEVENT = _framework->lastEventRequest();
  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult VecbosModule::genEndJob() {
 
  Vecbos::Instance()->RunEnd();
  Vecbos::Instance()->Fin();

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult VecbosModule::genEndRun(AbsEvent* event) {

  if(event) {
    ybos_init_framework(event);
    Vecbos::Instance()->RunEnd();
    ybos_init_framework(0);
  }   

  return AppResult::OK;
}

//_____________________________________________________________________________
AppResult VecbosModule::genBeginRun(AbsEvent* event) {
  
  if(event) {
    ybos_init_framework(event);
    Vecbos::Instance()->RunBeg();
    ybos_init_framework(0);
  }
  return AppResult::OK;
}

//_____________________________________________________________________________
int VecbosModule::callGenerator(AbsEvent* event) {

   ybos_init_framework(event);
   int rc = Vecbos::Instance()->GenerateEvent();
   ++_events;
   return rc;
}

//_____________________________________________________________________________
void  VecbosModule::fillHepevt() {
  // VecbosModule does not fill hepevt
}

void VecbosModule::putParametersToVecbos() {

  // puts the AbsParms to the Vecbos common blocks
  // debug flag

  int _iflag;
  int _iflag1;

  if (_debug.value() ) {
    Vecbos::Instance()->Bveg1()->NPRN = 1;
  } 
  else {
    Vecbos::Instance()->Bveg1()->NPRN = 0;
  }

  // beam 
  Vecbos::Instance()->Genera()->IPPBAR = _beam_type.value();

  // W or Z
  Vecbos::Instance()->Genera()->IVEC = _which_boson.value();

  // decay the boson 
  if (!_decay_boson.value()) {
    std::cout<<" Despite request, W will be decayed!"<<std::endl;
  }
	
  // charge of W ????
  if(Vecbos::Instance()->Genera()->IVEC == 1) {
    Vecbos::Instance()->Genera()->IWHICH = 1;
    _iflag1 = _charge_of_W.value();
    if(_iflag1 == -1) Vecbos::Instance()->Genera()->IWHICH=2;
    if(_iflag1 ==  0) Vecbos::Instance()->Genera()->IWHICH=3;
  }
  else {
    Vecbos::Instance()->Genera()->IWHICH = 0;
  }

  // Z decay 
  Vecbos::Instance()->Genera()->IDECAY = _howto_decay_Z.value();

  // no of jets 
  Vecbos::Instance()->Genera()->NJET = _njets.value();

  //  Structure function
  Vecbos::Instance()->Strucv()->IUSE = _structure_function.value();
  
  // PDF group and set 
  if (_structure_function.value() == 6) {
    //    Vecbos::Instance()->Vpdfsf()->NUMPDF = 100000 + 1000*_pdf_group.value() + _pdf_set.value();
    Vecbos::Instance()->Vpdfsf()->NUMPDF = 1000*_pdf_group.value() + _pdf_set.value();
  }
  else {
    Vecbos::Instance()->Vpdfsf()->NUMPDF = -1;
  } 

  // Run Number
  Vecbos::Instance()->Vecrun()->RUNNUM = _run_number.value();

  // Energy in CM
  Vecbos::Instance()->Genera()->W = _cm_energy.value();

  // QCD Scale 
  Vecbos::Instance()->Genera()->IQCDDS = _qcd_scale.value();

  // Helicity 
  Vecbos::Instance()->Genera()->IHELRN = _helicity.value();

  // Force bbbar in final state 
  Vecbos::Instance()->Genera()->IMQ = _force_bbbar.value();

  // use importance sampling for jet Pt
  _iflag = _important.value();
  Vecbos::Instance()->Imppar()->IYES = _iflag;

  // sampling mode 
  if (_iflag == 1) {
    Vecbos::Instance()->Imppar()->IS = _sampling_mode.value();
  }
  else {
    Vecbos::Instance()->Imppar()->IS = 0;
  }

  // Lepton type 
  Vecbos::Instance()->Vecjnk()->LEPTON_TYPE = _lepton_type.value();

  // Matrix element
  Vecbos::Instance()->Inter()->IMANN1 = _matrix_ele.value();

  // Alpha jet generaion
  Vecbos::Instance()->Imppar()->ALPHA = _alpha_jet_generation.value();

  // Make  jets from boson decay
  Vecbos::Instance()->Initvg()->MKJTS = _make_jets.value();

  // Jet minimum Pt
  Vecbos::Instance()->Cutval()->PTMINJ = _jet_min_pt.value();

  // Jet sum Pt
  Vecbos::Instance()->Vecjnk()->ETOTMN = _jet_sum_pt.value();

  // Jet eta maximum
  Vecbos::Instance()->Vecjnk()->ETAMAX = _jet_eta_max.value();

  // Jet eta separation
  Vecbos::Instance()->Vecjnk()->ETASEP = _jet_eta_separation.value();

  // Min Pt  of the  boson
  Vecbos::Instance()->Vhrdct()->BOSPT = _boson_min_pt.value();

  // No of leptons 
  Vecbos::Instance()->Vhrdct()->MIN_ELE = _no_of_leptons.value();

  // Lepton maximum abs(eta)
  Vecbos::Instance()->Vhrdct()->ELE_ETAMAX = (int)_lep_max_eta.value();

  // Lepton minimum Pt
  Vecbos::Instance()->Vhrdct()->ELE_PTMIN = (int)_lep_min_pt.value();

  // Missing Et(Pt) Cut
  Vecbos::Instance()->Vhrdct()->MISSPT = (int)_mis_pt_min.value();

  // Minimal event weight
  Vecbos::Instance()->Vecjnk()->WT_MIN = _min_weght.value();

  // Read write vegas grid file  flag
  _iflag = _vegas_rw_flag.value();
  Vecbos::Instance()->Vcenct()->I_READIN = _iflag%2;
  Vecbos::Instance()->Vcenct()->I_WRITEOUT = _iflag/2;
    

  // Input file for VEGAS grid 
  //envir = _vegas_inp_grid_file.value();
  //setenv("CDFVEGIN",envir);
  //if (envir != NULL) {
  //  setenv("CDFVEGIN",envir);
  //}
  //else {
  //  setenv("CDFVEGIN","empty");
  //}

  // Output file for VEGAS grid
  //  envir = _vegas_out_grid_file.value();
  //  if (envir != NULL) {
  //    setenv("CDFVEGOUT",envir);
  // }
  //else {
  //  setenv("CDFVEGOUT""empty");
  //}

  // VEGAS print 
  Vecbos::Instance()->Bveg1()->NPRN = _vegas_print.value();

  // VEGAS no of iterations
  Vecbos::Instance()->Inter()->ITMX1 = _vegas_n_inter.value();

  // VEGAS  no of calls 1
  Vecbos::Instance()->Inter()->NCALL1 = _vegas_n_calls1.value();

  // VEGAS  no of calls 2
  Vecbos::Instance()->Inter()->NCALL2 = _vegas_n_calls2.value();  
}


void VecbosModule::getParametersFromVecbos() {
  // takes the AbsParms from the Vecbos common blocks

  int _iflag;
  int _iflag1;
  int _iflag2;  
  char* envir;

  // debug flag
  _iflag = Vecbos::Instance()->Bveg1()->NPRN;
  if (_iflag == 0) {
    _debug.set(false);
  } 
  else {
    _debug.set(true);
  }

  // beam 
  _beam_type.set(Vecbos::Instance()->Genera()->IPPBAR);

  // W or Z
  _iflag = Vecbos::Instance()->Genera()->IVEC;
  _which_boson.set(_iflag);

  // decay the boson 
  _decay_boson.set(true);
	
  // charge of W
  if(_iflag == 1) {
    _iflag1 = Vecbos::Instance()->Genera()->IWHICH;
    if(_iflag1 == 2) _iflag1 = -1;
    if(_iflag1 == 3) _iflag1 = 0; 
    _charge_of_W.set(_iflag1);
  }
  else {
     _charge_of_W.set(0);
  }

  // Z decay 
  _howto_decay_Z.set(Vecbos::Instance()->Genera()->IDECAY);

  // no of jets 
  _njets.set(Vecbos::Instance()->Genera()->NJET);

  //  Structure function
  _iflag = Vecbos::Instance()->Strucv()->IUSE;
  _structure_function.set(_iflag);
  
  // PDF group and set
  if (_iflag == 6) {
    _iflag2 = (Vecbos::Instance()->Vpdfsf()->NUMPDF)/100000;
    _iflag1 = ((Vecbos::Instance()->Vpdfsf()->NUMPDF)-_iflag2*100000)/1000;
    _pdf_group.set(_iflag1);
    _iflag1 = ((Vecbos::Instance()->Vpdfsf()->NUMPDF)-_iflag2*100000)%1000;
    _pdf_set.set(_iflag1);
  }
  else {
    _pdf_group.set(-1);
    _pdf_set.set(-1);
  } 

  // Run Number
  _run_number.set(Vecbos::Instance()->Vecrun()->RUNNUM);

  // Energy in CM
  _cm_energy.set(Vecbos::Instance()->Genera()->W);

  // QCD Scale 
  _qcd_scale.set(Vecbos::Instance()->Genera()->IQCDDS);

  // Helicity 
  _helicity.set(Vecbos::Instance()->Genera()->IHELRN);

  // Force bbbar in final state 
  _force_bbbar.set(Vecbos::Instance()->Genera()->IMQ);

  // use importance sampling for jet Pt
  _iflag = Vecbos::Instance()->Imppar()->IYES;
  _important.set(_iflag);

  // sampling mode 
  if (_iflag == 1) {
    _sampling_mode.set(Vecbos::Instance()->Imppar()->IS);
  }
  else {
    _sampling_mode.set(0); 
  }

  // Lepton type 
  _lepton_type.set(Vecbos::Instance()->Vecjnk()->LEPTON_TYPE);

  // Matrix element
  _matrix_ele.set(Vecbos::Instance()->Inter()->IMANN1);

  // Alpha jet generaion
  _alpha_jet_generation.set(Vecbos::Instance()->Imppar()->ALPHA);

  // Make  jets from boson decay
  _iflag = Vecbos::Instance()->Initvg()->MKJTS;
  _make_jets.set(_iflag);

  // Jet minimum Pt
  _jet_min_pt.set(Vecbos::Instance()->Cutval()->PTMINJ);

  // Jet sum Pt
  _jet_sum_pt.set(Vecbos::Instance()->Vecjnk()->ETOTMN);

  // Jet eta maximum
  _jet_eta_max.set(Vecbos::Instance()->Vecjnk()->ETAMAX);

  // Jet eta separation
  _jet_eta_separation.set(Vecbos::Instance()->Vecjnk()->ETASEP);

  // Min Pt  of the  boson
  _boson_min_pt.set(Vecbos::Instance()->Vhrdct()->BOSPT);

  // No of leptons 
  _no_of_leptons.set(Vecbos::Instance()->Vhrdct()->MIN_ELE);

  // Lepton maximum abs(eta)
  _lep_max_eta.set(Vecbos::Instance()->Vhrdct()->ELE_ETAMAX);

  // Lepton minimum Pt
  _lep_min_pt.set(Vecbos::Instance()->Vhrdct()->ELE_PTMIN);

  // Missing Et(Pt) Cut
  _mis_pt_min.set(Vecbos::Instance()->Vhrdct()->MISSPT);

  // Minimal event weight
  _min_weght.set(Vecbos::Instance()->Vecjnk()->WT_MIN);

   // Read write vegas grid file  flag
  _iflag  = Vecbos::Instance()->Vcenct()->I_READIN;
  _iflag1 = Vecbos::Instance()->Vcenct()->I_WRITEOUT;
  _vegas_rw_flag.set(_iflag+2*_iflag1);

  // Input file for VEGAS grid 
  envir = getenv("CDFVEGIN");
  if (envir != NULL) {
    _vegas_inp_grid_file.set(envir);
  }
  else {
    _vegas_inp_grid_file.set("empty");
  }

   // Output file for VEGAS grid
  envir = getenv("CDFVEGOUT");
  if (envir != NULL) {
    _vegas_out_grid_file.set(envir);
  }
  else {
    _vegas_out_grid_file.set("empty");
  }

  // VEGAS print 
  _iflag = Vecbos::Instance()->Bveg1()->NPRN;
  _vegas_print.set(_iflag);

  // VEGAS no of iterations
  _vegas_n_inter.set(Vecbos::Instance()->Inter()->ITMX1);

  // VEGAS  no of calls 1
  _vegas_n_calls1.set(Vecbos::Instance()->Inter()->NCALL1);

  // VEGAS  no of calls 2
  _vegas_n_calls2.set(Vecbos::Instance()->Inter()->NCALL2);
  
}

