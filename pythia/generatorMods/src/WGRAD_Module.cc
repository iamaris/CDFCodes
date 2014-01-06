//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class WGRAD_Module
//
//      Interface WGRAD, a program for simulating W production with O(alpha) 
//      EW radiative corrections. Ref. : hep-ph/9807417
//                                       Fermilab-Pub-98/164-T
// Author List:
//      Dave Waters, Oxford University
//
//  revision history:
//  -----------------
//  8.7.99  Dave Waters : first attempt.
// 14.8.99  Dave Waters : set talk-to parameters that will be used to
//                        run WGRAD directly from simulation.
// 17.8.99  Dave Waters : complete the integration of WGRAD. No more
//                        need for ascii file input.
//  3.9.99  Dave Waters : Pythia interface menu. Choice of bypassing
//                        Pythia altogether, with particle 4-momenta
//                        loaded into HEPEVT by hand.
// aug 29 2001 lena: clean up, remove gEvent. 
// aug 30 2001 lena: make include in standard way
// jan 14 2001 lena: register as pythia user at beginJob
//--------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "generatorMods/WGRAD_Module.hh"

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "BaBar/Cdf.hh"
#include "inc/bcs.h"
#include "evt/evt.h"
//#include "ParticleDB/hepevt.hh"
#include "stdhep_i/CdfHepevt.hh"
//#include "ParticleDB/CdfParticleDatabase.hh"
#include "pythia_i/Pythia.hh"

// extern "C" {
//  void pyupev_(int*, float*);
// }

extern "C" {
  void upinit_(int*, float*);
  void upevnt_();
}

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------
#include "r_n/CdfRn.hh"
#include <sstream>
using std::ostringstream ;

const long WGRAD_Module::_defaultRandomSeed1 = 9228;
const long WGRAD_Module::_defaultRandomSeed2 = 1097;

const char* WGRAD_Module::genId = "WGRAD";

//----------------
// Constructors --
//----------------

WGRAD_Module::WGRAD_Module()
  : AbsGenModule( WGRAD_Module::genId, 
		  "W production including O(alpha) EW radiative corrections"),
    _debug("debug",this,false),
    _usePythia("usePythia",this,true),
    _useLesHouchesModule("useLesHouchesModule",this,false),
    _beamType("BeamType",this,1),
    _W_charge("W_Charge",this,1),
    _widthTreatment("widthTreatment",this,2),
    _decayLepton("DecayLepton",this,2),
    _QED_radType("QED_RadType",this,2),
    _fsrType("fsrType",this,1),
    _smearingRecombination("SmearingRecombination",this,0),
    _order("Order",this,2),
    _scale("Scale",this,0),
    _fsrCollinearCut("fsrCollinearCut",this,0),
    _photonEnergyCut("PhotonEnergyCut",this,0.0001),
    _photonCollinearCut("PhotonCollinearCut",this,0.0001),
    _pdflibGroup("pdflibGroup",this,3),
    _pdflibSet("pdflibSet",this,54),
    _wMass("W_Mass",this,80.3),
    _wWidth("W_Width",this,2.1),
    _CME("CME",this,2000.0),
    _lepton_ptmin("Lepton_ptmin",this,0.),
    _lepton_ymax("Lepton_ymax",this,100.),
    _neutrino_ptmin("Neutrino_ptmin",this,0.),
    _neutrino_ymax("Neutrino_ymax",this,100.),
    _photon_ptmin("Photon_ptmin",this,0.),
    _photon_ymax("Photon_ymax",this,100.),
    _tmass_low("mT_low",this,0.),
    _tmass_high("mT_high",this,2000.),
    _itmaxGrid("itmaxGrid",this,1),
    _ncallGrid("ncallGrid",this,2000),
    _itmaxPrecision("itmaxPrecision",this,2),
    _ncallPrecision("ncallPrecision",this,5000),
    _rndmSeed("randomSeed",this,4567),
    // No longer the correct way to setup Pythia : 
    //_mstj_41("mstj(41)",this,-1),
    //_mrpy_1("mrpy(1)",this,-1),
    //_mstp_81("mstp(81)",this,-1),
    //_mstp_91("mstp(91)",this,-1),
    //_mstp_111("mstp(111)",this,-1),
    //_parp_91("parp(91)",this,-1.0),
    //_parp_92("parp(92)",this,-1.0),
    //_parp_93("parp(93)",this,-1.0),
    _randomSeed1("RandomSeed1",this,WGRAD_Module::_defaultRandomSeed1),
    _randomSeed2("RandomSeed2",this,WGRAD_Module::_defaultRandomSeed2),
    _pythiaMenu(new PythiaMenu(this,0,"PythiaMenu"))
{
  ostringstream debugDesc;
  debugDesc  << "\tPrint out debug information ?";
  _debug.addDescription(debugDesc.str());
  commands()->append(&_debug);

  ostringstream usePythiaDesc;
  usePythiaDesc << "\tUse Pythia for PS and underlying event sim. ?";
  _usePythia.addDescription(usePythiaDesc.str());
  _useLesHouchesModule.addDescription("\t Use Les Houches Modules for all interfacing");
  commands()->append(&_usePythia);
  commands()->append(&_useLesHouchesModule);

  //---------------------------------------------------------------------
  // Cards Menu :
  //---------------------------------------------------------------------
  _cardsMenu.initialize("cardsMenu",this);
  _cardsMenu.initTitle("cardsMenu");
  commands()->append(&_cardsMenu);

  _cardsMenu.commands()->append(&_beamType);
  _cardsMenu.commands()->append(&_W_charge);
  _cardsMenu.commands()->append(&_widthTreatment);
  _cardsMenu.commands()->append(&_decayLepton);
  _cardsMenu.commands()->append(&_QED_radType);
  _cardsMenu.commands()->append(&_fsrType);
  _cardsMenu.commands()->append(&_smearingRecombination);
  _cardsMenu.commands()->append(&_order);
  _cardsMenu.commands()->append(&_scale);
  _cardsMenu.commands()->append(&_fsrCollinearCut);
  _cardsMenu.commands()->append(&_photonEnergyCut);
  _cardsMenu.commands()->append(&_photonCollinearCut);
  _cardsMenu.commands()->append(&_pdflibGroup);
  _cardsMenu.commands()->append(&_pdflibSet);
  _cardsMenu.commands()->append(&_wMass);
  _cardsMenu.commands()->append(&_wWidth);
  _cardsMenu.commands()->append(&_CME);
  _cardsMenu.commands()->append(&_lepton_ptmin);
  _cardsMenu.commands()->append(&_lepton_ymax);
  _cardsMenu.commands()->append(&_neutrino_ptmin);
  _cardsMenu.commands()->append(&_neutrino_ymax);
  _cardsMenu.commands()->append(&_photon_ptmin);
  _cardsMenu.commands()->append(&_photon_ymax);
  _cardsMenu.commands()->append(&_tmass_low);
  _cardsMenu.commands()->append(&_tmass_high);
  _cardsMenu.commands()->append(&_itmaxGrid);  
  _cardsMenu.commands()->append(&_ncallGrid);
  _cardsMenu.commands()->append(&_itmaxPrecision);
  _cardsMenu.commands()->append(&_ncallPrecision);
  _cardsMenu.commands()->append(&_rndmSeed);

  //---------------------------------------------------------------------
  // Pythia Interface Menu :
  //---------------------------------------------------------------------
  // No longer the correct way to setup Pythia : 
  //_pythiaInterfaceMenu.initialize("pythiaInterfaceMenu",this);
  //_pythiaInterfaceMenu.initTitle("pythiaInterfaceMenu");
  //commands()->append(&_pythiaInterfaceMenu);
  //_pythiaInterfaceMenu.commands()->append(&_mstj_41);
  //_pythiaInterfaceMenu.commands()->append(&_mrpy_1);
  //_pythiaInterfaceMenu.commands()->append(&_mstp_81);
  //_pythiaInterfaceMenu.commands()->append(&_mstp_91);
  //_pythiaInterfaceMenu.commands()->append(&_mstp_111);
  //_pythiaInterfaceMenu.commands()->append(&_parp_91);
  //_pythiaInterfaceMenu.commands()->append(&_parp_92);
  //_pythiaInterfaceMenu.commands()->append(&_parp_93);

  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle("Pythia Menu");
  commands()->append(_pythiaMenu);
  
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

//--------------
// Destructor --
//--------------

WGRAD_Module::~WGRAD_Module() 
{ 
  delete _pythiaMenu;
}

//--------------
// Operations --
//--------------

AppResult WGRAD_Module::genBeginJob( )
{
  Pythia* pythia = Pythia::Instance();
  // Set the random number seeds :
    CdfRn* rn = CdfRn::Instance();
    if ( !rn->isReadingFromFile() ) {
      rn->SetEngineSeeds(_randomSeed1.value(), _randomSeed2.value(),"WGRAD");
    }

  // Call WGRAD directly.
  std::cout << "  WGRAD_Module::genBeginJob : passing cuts and parameters to WGRAD ..." << std::endl;

  // Set the WGRAD internal parameters from the talk-to parameters :
  wgrad_set_external_();

  // Use Pythia or not ?
  int wgrad_use_pythia[1];
  if (_usePythia.value())
    {
      wgrad_use_pythia[0]=1;
    }
  else
    {
      wgrad_use_pythia[0]=0;
    }
  wgrad_use_pythia_(wgrad_use_pythia);

  // Set some basic flags and switches :
  int wgrad_switches[10];
  wgrad_switches[0] = _beamType.value();
  wgrad_switches[1] = _W_charge.value();
  wgrad_switches[2] = _decayLepton.value();
  wgrad_switches[3] = _QED_radType.value();
  wgrad_switches[4] = _fsrType.value();
  wgrad_switches[5] = _smearingRecombination.value();
  wgrad_switches[6] = _order.value();
  wgrad_switches[7] = _scale.value();
  wgrad_switches[8] = _fsrCollinearCut.value();
  wgrad_switches[9] = _widthTreatment.value();
  
  wgrad_set_switches_(wgrad_switches);
  
  // Set soft and collinear photon cut-offs :
  double wgrad_soft_collinear_cuts[2];
  wgrad_soft_collinear_cuts[0] = _photonEnergyCut.value();
  wgrad_soft_collinear_cuts[1] = _photonCollinearCut.value();
  
  wgrad_set_soft_collinear_cuts_(wgrad_soft_collinear_cuts);
  
  // Set choice of structure functions :
  int wgrad_pdflib[2];
  wgrad_pdflib[0] = _pdflibGroup.value();
  wgrad_pdflib[1] = _pdflibSet.value();
  
  wgrad_set_pdflib_(wgrad_pdflib);
  
  // Set W mass, width and center of mass energy :
  double wgrad_properties[3];
  wgrad_properties[0]=_wMass.value();
  wgrad_properties[1]=_wWidth.value();
  wgrad_properties[2]=_CME.value();
  
  wgrad_set_properties_(wgrad_properties);
  
  // Set kinematic cuts :
  double wgrad_kinematic_cuts[8];
  wgrad_kinematic_cuts[0] = _lepton_ptmin.value();
  wgrad_kinematic_cuts[1] = _lepton_ymax.value();
  wgrad_kinematic_cuts[2] = _neutrino_ptmin.value();
  wgrad_kinematic_cuts[3] = _neutrino_ymax.value();
  wgrad_kinematic_cuts[4] = _photon_ptmin.value();
  wgrad_kinematic_cuts[5] = _photon_ymax.value();
  wgrad_kinematic_cuts[6] = _tmass_low.value();
  wgrad_kinematic_cuts[7] = _tmass_high.value();

  wgrad_set_kinematic_cuts_(wgrad_kinematic_cuts);
  
  // Set parameters for grid search and precision calculation :
  int wgrad_grid[4];
  wgrad_grid[0] = _itmaxGrid.value();
  wgrad_grid[1] = _ncallGrid.value();
  wgrad_grid[2] = _itmaxPrecision.value();
  wgrad_grid[3] = _ncallPrecision.value();
  
  wgrad_set_grid_pars_(wgrad_grid);
  
  // Set the random number seed :
  int wgrad_rndm_seed[1];
  wgrad_rndm_seed[0] = _rndmSeed.value();
  wgrad_set_rndm_seed_(wgrad_rndm_seed);
  
  // Set the information in the Pythia common blocks for the WGRAD-Pythia interface :
  // int wgrad_pythia_mstj[1];
  // wgrad_pythia_mstj[0]=_mstj_41.value();
  // wgrad_pythia_set_mstj_(wgrad_pythia_mstj);
  // int wgrad_pythia_mrpy[1];
  // wgrad_pythia_mrpy[0]=_mrpy_1.value();
  // wgrad_pythia_set_mrpy_(wgrad_pythia_mrpy);
  // int wgrad_pythia_mstp[3];
  // wgrad_pythia_mstp[0]=_mstp_81.value();
  // wgrad_pythia_mstp[1]=_mstp_91.value();
  // wgrad_pythia_mstp[2]=_mstp_111.value();
  // wgrad_pythia_set_mstp_(wgrad_pythia_mstp);
  // double wgrad_pythia_parp[3];
  // wgrad_pythia_parp[0]=_parp_91.value();
  // wgrad_pythia_parp[1]=_parp_92.value();
  // wgrad_pythia_parp[2]=_parp_93.value();
  // wgrad_pythia_set_parp_(wgrad_pythia_parp);
  
  wgrad_begin_();

  std::cout << "  WGRAD_Module::genBeginJob : WGRAD initialised. " << std::endl;      
  // AST, 17.03.2003. Only call PYTHIA directly if LesHouches is not used
  if(!_useLesHouchesModule.value()) {
  // DSW, 23.07.2002. Use new Pythia singleton interface.
    pythia->setPythiaUid("WGRAD");

  // Call with special value for frame in order to trigger UPINIT :
    std::string special_frame = "USER";
    std::string target = "dummy";
    double winval = 1;
    std::cout << "  WGRAD_Module::genBeginJob : calling pythia->Pyinit " << std::endl;  
    pythia->Pyinit(special_frame,target,&winval);
  }

  return AppResult::OK;
}

AppResult WGRAD_Module::genBeginRun( AbsEvent* run )
{
  return AppResult::OK;
}

AppResult WGRAD_Module::genEndJob()
{
  // In case this gets called without WGRAD being initialised,
  // set some flags which control termination :
  wgrad_set_external_();
  // Call WGRAD subroutine to print out summary information :
  wgrad_end_();

  return AppResult::OK;
}

AppResult WGRAD_Module::genEndRun( AbsEvent* run )
{
  return AppResult::OK;
}

int WGRAD_Module::callGenerator( AbsEvent* anEvent ) 
{
  // Call WGRAD directly.
  wgrad_evt_();

  // Clear up HEPEVT before filling it again :
  CdfHepevt* hepevt = CdfHepevt::Instance();
  hepevt->clear();
  hepevt->clearCommon();

  // Extract the event weight from WGRAD :
  double wgrad_event_weight[1];
  wgrad_get_event_weight_(wgrad_event_weight);

  // AST, 17.03.2003. If using Les Houches Module let it do the rest       
  if(_useLesHouchesModule.value()) return 0;

  // Pythia or not
  if (!_usePythia.value()) 
    {
      // Extract the charged lepton, neutrino and photon four momenta from WGRAD :
      int wgrad_particle_codes[3];
      wgrad_get_particle_codes_(wgrad_particle_codes);
      double wgrad_charged_lepton[4];
      wgrad_get_charged_lepton_(wgrad_charged_lepton);
      double wgrad_neutrino[4];
      wgrad_get_neutrino_(wgrad_neutrino);
      double wgrad_photon[4];
      wgrad_get_photon_(wgrad_photon);

      if (_debug.value())
	{
	  std::cout << " WGRAD_Module::callGenerator : Charged Lepton code, px, py, pz, E = " << wgrad_particle_codes[0] << " , " 
	       << wgrad_charged_lepton[0] << " , " << wgrad_charged_lepton[1] << " , " 
	       << wgrad_charged_lepton[2] << " , " << wgrad_charged_lepton[3] << std::endl;
	  std::cout << " WGRAD_Module::callGenerator : Neutrino code, px, py, pz, E       = " << wgrad_particle_codes[1] << " , " 
	       << wgrad_neutrino[0] << " , " << wgrad_neutrino[1] << " , " 
	       << wgrad_neutrino[2] << " , " << wgrad_neutrino[3] << std::endl;
	  std::cout << " WGRAD_Module::callGenerator : Photon code, px, py, pz, E         = " << wgrad_particle_codes[2] << " , " 
	       << wgrad_photon[0] << " , " << wgrad_photon[1] << " , " 
	       << wgrad_photon[2] << " , " << wgrad_photon[3] << std::endl;
	}


      // Determine whether 2 or 3 body final state :
      int nPart = 3;
      if (wgrad_photon[3] == 0.0) nPart = 2;
      if (_debug.value()) std::cout << " WGRAD_Module::callGenerator : nPart = " << nPart << std::endl;

      CdfHepevt* hepevt = CdfHepevt::Instance();

      for (int iPart = 0; iPart < nPart; iPart++)
	{         
	  hepevt->HepevtPtr()->IDHEP[iPart]     = wgrad_particle_codes[iPart];
		  
	  hepevt->HepevtPtr()->ISTHEP[iPart]    = 1;   // status code for final state particles
	  hepevt->HepevtPtr()->JMOHEP[iPart][0] = 0;   // mother position 
	  hepevt->HepevtPtr()->JMOHEP[iPart][1] = 0;   // second mother position
		  
	  hepevt->HepevtPtr()->JDAHEP[iPart][0] = 0;   // first daughter
	  hepevt->HepevtPtr()->JDAHEP[iPart][1] = 0;   // last daughter
		  
	  if (iPart==0)
	    {
	      hepevt->HepevtPtr()->PHEP[iPart][0]   = wgrad_charged_lepton[0];
	      hepevt->HepevtPtr()->PHEP[iPart][1]   = wgrad_charged_lepton[1];
	      hepevt->HepevtPtr()->PHEP[iPart][2]   = wgrad_charged_lepton[2];
	      hepevt->HepevtPtr()->PHEP[iPart][3]   = wgrad_charged_lepton[3];
	    }
	  else if (iPart==1)
	    {
	      hepevt->HepevtPtr()->PHEP[iPart][0]   = wgrad_neutrino[0];
	      hepevt->HepevtPtr()->PHEP[iPart][1]   = wgrad_neutrino[1];
	      hepevt->HepevtPtr()->PHEP[iPart][2]   = wgrad_neutrino[2];
	      hepevt->HepevtPtr()->PHEP[iPart][3]   = wgrad_neutrino[3];
	    }
	  else if (iPart==2)
	    {
	      hepevt->HepevtPtr()->PHEP[iPart][0]   = wgrad_photon[0];
	      hepevt->HepevtPtr()->PHEP[iPart][1]   = wgrad_photon[1];
	      hepevt->HepevtPtr()->PHEP[iPart][2]   = wgrad_photon[2];
	      hepevt->HepevtPtr()->PHEP[iPart][3]   = wgrad_photon[3];
	    }

	  hepevt->HepevtPtr()->VHEP[iPart][0]   = 0.;  // production vertex x-position
	  hepevt->HepevtPtr()->VHEP[iPart][1]   = 0.;  // production vertex y-position
	  hepevt->HepevtPtr()->VHEP[iPart][2]   = 0.;  // production vertex z-position
	  hepevt->HepevtPtr()->VHEP[iPart][3]   = 0.;  // production vertex time
	}      	      
      hepevt->HepevtPtr()->NHEP         = nPart;   // number of particles
    }
  else
    {
      // DSW, 23.07.2002. Use new Pythia singleton interface.
      Pythia* pythia = Pythia::Instance();
      // std::cout << "  WGRAD_Module::callGenerator : calling pythia->Pyevnt()" << std::endl;
      // Let Pythia do it's stuff :
      pythia->Pyevnt();
      // Update the event counter :
      pythia->addEvent();
      int mevlist = pythia->EventListLevel();
      if (pythia->events() >= _pythiaMenu->firstlistevent() &&
	  pythia->events() <= _pythiaMenu->lastlistevent())
	{
	  std::cout << "PYEVNT event number " << pythia->events() << std::endl;
	  pythia->Pylist(&mevlist);
	}
      // Tell lunhep to convert from PYJETS to HEPEVT
      int mconv = 1;
      // Fill HEPEVT :
      pythia->Lunhep(&mconv);
    }
  
  return 0;
}

void WGRAD_Module::fillHepevt() {
  // this is done somewhere else, should be moved here
}

