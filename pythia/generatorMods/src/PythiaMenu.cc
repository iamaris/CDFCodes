//--------------------------------------------------------------------
// PythiaMenu.cc
//
// created Nov 26 2001, Elena Gerchtein (CMU)
//

#include "generatorMods/PythiaMenu.hh"
#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"

#include <string>
#include <list>

PythiaMenu::PythiaMenu( AppModule* target,
			APPMenu* topMenu,
			std::string menuName)
  :
  APPMenu(),
  _msel         ("msel",        target, 1),
  _firstlistevent("listFirst",    target, 1),
  _lastlistevent ("listLast",    target, 1),
  _eventlistlevel("evListLevel",    target, 1),
  _initlistlevel("inListLevel",    target, 11),
  _statlistlevel("statListLevel",    target, 1),
  //  _randomseed("seed",    target, 19780503),
  _win       ("cmEnergy",    target, 2000.),
  _target    ("target",      target, "PBAR  "),
  _frame     ("frame" ,      target, "CMS" ),
  _pygiveFile("pygiveFile",  target, "" ),
  _pyeditAction("pyeditAction",target, -1, -1),
  _allowBMixing("allowBMixing",target, false),
  _tuneUE("tuneUE",target, 0),

  // Decay table handling
  _decayFileMode("decayFileMode",target,"none"),
  _decayFile("decayFile",target,""),
  _decayTableMode("readDecayTableMode",target, 2, 2, 3),
  _selectPdgCode("selectPdgCode",target, 0),
  _expWeight("exponentialWeight",target, 0., -30., 30),
  _weightParam("weightParameter",target, 1, 1, 4),

  // PYSUBS menu
  _pysubsMenu(),
  _msubCommand("set_msub",target),
  _ckinCommand("set_ckin",target),
  _kfinCommand("set_kfin",target),
  _msubShowCommand("show_msub",target),
  _ckinShowCommand("show_ckin",target),
  _kfinShowCommand("show_kfin",target),

  // PYDAT1
  _commonMenu(),
  _mstuCommand("set_mstu",target),
  _paruCommand("set_paru",target),
  _mstjCommand("set_mstj",target),
  _parjCommand("set_parj",target),
  _mstuShowCommand("show_mstu",target),
  _paruShowCommand("show_paru",target),
  _mstjShowCommand("show_mstj",target),
  _parjShowCommand("show_parj",target),

  // PYPARS
  _pyparsMenu(),
  _mstpCommand("set_mstp",target),
  _parpCommand("set_parp",target),
  _mstpShowCommand("show_mstp",target),
  _parpShowCommand("show_parp",target),

  //PYDAT2
  _pmasCommand("set_pmas",target),
  _pmasShowCommand("show_pmas",target),

  //PYDAT3
  _mdcyCommand("set_mdcy",target),
  _mdmeCommand("set_mdme",target),
  _bratCommand("set_brat",target),
  _kfdpCommand("set_kfdp",target),
  _mdcyShowCommand("show_mdcy",target),
  _mdmeShowCommand("show_mdme",target),
  _bratShowCommand("show_brat",target),
  _kfdpShowCommand("show_kfdp",target),

  //PYINT2
  _isetCommand("set_iset",target),
  _kfprCommand("set_kfpr",target),
  _coefCommand("set_coef",target),
  _isetShowCommand("show_iset",target),
  _kfprShowCommand("show_kfpr",target),
  _coefShowCommand("show_coef",target),


  // SUSY
  _susyMenu(),
  _imssCommand("set_imss",target),
  _rmssCommand("set_rmss",target),
  _imssShowCommand("show_imss",target),
  _rmssShowCommand("show_rmss",target),

  // Technicolour

  _itcmCommand("set_itcm",target),
  _rtcmCommand("set_rtcm",target),
  _itcmShowCommand("show_itcm",target),
  _rtcmShowCommand("show_rtcm",target)
{
  // this menu
  initialize("PythiaMenu",target,topMenu);
  initTitle("Pythia menu");
  InitPythiaMenu();
}

void PythiaMenu::InitPythiaMenu() {


  // add and describe other submenus and commands
  // Define Parameters for the Talk-to

  _selectPdgCode.addDescription(
		"      \t\t\t set Pdg code of particle to be selected at generation time, default 0(nothing)");
  
  _msel.addDescription(
		       "      \t\t\tMain Process selection for the generation");  
  _frame.addDescription(
		      "      \t\t\tExperiment frame, CMS by default");  
  _win.addDescription(
		      "      \t\t\tCentre of Mass Energy available for the process");  
  _target.addDescription(
			 "      \t\t\tTarget, PBAR by default");  
  _firstlistevent.addDescription(
				 "      \t\t\tFirst event to list");  
  _lastlistevent.addDescription(
				"      \t\t\tLast event to list");  
  _eventlistlevel.addDescription(
				 "      \t\t\tEvent listing level");  
  _initlistlevel.addDescription(
				"      \t\t\tLevel of Particle information to be given at run start");    
  _statlistlevel.addDescription(
				"      \t\t\tLevel of Statistics information to be given at run end");  
  //  _randomseed.addDescription(
  //			     "      \t\t\tSet random seed (in fact MRLU(1))");
  _decayFileMode.addDescription(
				"      \t\t\tPossible values ``READ'', ``WRITE'', ``NONE''");
  _decayFile.addDescription(
			    "      \t\t\tDecay table filename");
  _decayTableMode.addDescription(
			    "      \t\t\tMode for reading decay table, 2 or 3");
  _pygiveFile.addDescription(
			    "      \t\t\tPYGIVE parameters filename");
  _pyeditAction.addDescription(
			    "      \t\t\tPYEDIT action to be performed");
  _expWeight.addDescription(
			    "      \t\t\tExponent to given parameter to provide event weight");
  _weightParam.addDescription(
			    "      \t\t\tParameter used to provide weight;1=pT**2,2=shat,3=that,4=uhat");
  _allowBMixing.addDescription(
			    "      \t\t\tAllow Pythia to mix B0 mesons, should be off to let EvtGen do this");
  _tuneUE.addDescription(
			    "      \t\t\tUnderlying Event and Min Bias tunings");
  // Initialize our AbsParmEnum
  std::list<std::string> fileModes;
  fileModes.push_back("NONE");
  fileModes.push_back("READ");
  fileModes.push_back("WRITE"); 
  fileModes.push_back("none");
  fileModes.push_back("read");

  fileModes.push_back("write");
  _decayFileMode.initValidList(fileModes);
  
  commands()->append(&_msel);
  commands()->append(&_selectPdgCode);
  commands()->append(&_frame);
  commands()->append(&_win);
  commands()->append(&_target);
  commands()->append(&_firstlistevent);
  commands()->append(&_lastlistevent);
  commands()->append(&_eventlistlevel);
  commands()->append(&_initlistlevel);
  commands()->append(&_statlistlevel);
//  commands()->append(&_randomseed);
  commands()->append(&_decayFileMode);
  commands()->append(&_decayTableMode);
  commands()->append(&_decayFile);
  commands()->append(&_pygiveFile);
  commands()->append(&_pyeditAction);
  commands()->append(&_expWeight);
  commands()->append(&_weightParam);
  commands()->append(&_allowBMixing);
  commands()->append(&_tuneUE);
  
  // main COMMON submenu
  _commonMenu.initialize("commonMenu",target(),this);
  _commonMenu.initTitle("Menu for control of parameter sets in PYTHIA COMMONS");
  
  // Add it to the main menu
  commands()->append(&_commonMenu); 
  
  // Add commands to the pydat1 submenu
  _commonMenu.commands()->append(&_msubCommand);
  _commonMenu.commands()->append(&_msubShowCommand);
  _commonMenu.commands()->append(&_ckinCommand);
  _commonMenu.commands()->append(&_ckinShowCommand);
  _commonMenu.commands()->append(&_kfinCommand);
  _commonMenu.commands()->append(&_kfinShowCommand);
  _commonMenu.commands()->append(&_mstpCommand);
  _commonMenu.commands()->append(&_mstpShowCommand);
  _commonMenu.commands()->append(&_parpCommand);
  _commonMenu.commands()->append(&_parpShowCommand);
  _commonMenu.commands()->append(&_mstuCommand);
  _commonMenu.commands()->append(&_mstuShowCommand);
  _commonMenu.commands()->append(&_paruCommand);
  _commonMenu.commands()->append(&_paruShowCommand);
  _commonMenu.commands()->append(&_mstjCommand);
  _commonMenu.commands()->append(&_mstjShowCommand);
  _commonMenu.commands()->append(&_parjCommand);
  _commonMenu.commands()->append(&_parjShowCommand);
  _commonMenu.commands()->append(&_pmasCommand);
  _commonMenu.commands()->append(&_pmasShowCommand);
  _commonMenu.commands()->append(&_mdcyCommand);
  _commonMenu.commands()->append(&_mdcyShowCommand);
  _commonMenu.commands()->append(&_mdmeCommand);
  _commonMenu.commands()->append(&_mdmeShowCommand);
  _commonMenu.commands()->append(&_bratCommand);
  _commonMenu.commands()->append(&_bratShowCommand);
  _commonMenu.commands()->append(&_kfdpCommand);
  _commonMenu.commands()->append(&_kfdpShowCommand);
  _commonMenu.commands()->append(&_isetCommand);
  _commonMenu.commands()->append(&_isetShowCommand);
  _commonMenu.commands()->append(&_kfprCommand);
  _commonMenu.commands()->append(&_kfprShowCommand);
  _commonMenu.commands()->append(&_coefCommand);
  _commonMenu.commands()->append(&_coefShowCommand);
  _commonMenu.commands()->append(&_itcmCommand);
  _commonMenu.commands()->append(&_itcmShowCommand);
  _commonMenu.commands()->append(&_rtcmCommand);
  _commonMenu.commands()->append(&_rtcmShowCommand);
  
  // SUSY submenu
  _susyMenu.initialize("susyMenu",(AppModule*)target(),this);
  _susyMenu.initTitle("Menu for control of SUSY parameters");
  commands()->append(&_susyMenu);
  _susyMenu.commands()->append(&_imssCommand);
  _susyMenu.commands()->append(&_imssShowCommand);
  _susyMenu.commands()->append(&_rmssCommand);
  _susyMenu.commands()->append(&_rmssShowCommand);
  
}
