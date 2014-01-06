//  
// PythiaMenu.hh - pythia menu to be used in any module that uses pythia;
//                 contains also default pythia values.
//
// author: Elena Gerchtein, CMU
//
// created: Nov 26 2001

#ifndef PYTHIAMENU_HH
#define PYTHIAMENU_HH

#include "Framework/APPMenu.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmEnum.hh"
#include "Framework/AbsParmFilename.hh"

#ifdef CDF
#include "BaBar/Cdf.hh"
#endif

#include "generatorMods/PythiaSetCommands.hh"
#include "generatorMods/PythiaShowCommands.hh"
#include "pythia_i/Pythia.hh"


class PythiaMenu : public APPMenu {

public:
  PythiaMenu ( AppModule* target=0,
	       APPMenu* topMenu=0,
	       std::string menuName = "PythiaMenu" );
  ~PythiaMenu() {}

  void InitPythiaMenu() ;

  // accessors:
  // for PythiaModule:
  const std::string decayFileMode() const { return _decayFileMode.value(); }
  const std::string decayFile()     const { return _decayFile.value(); }
  const std::string getTarget()     const { return _target.value(); }
  const std::string frame()         const { return _frame.value(); }
  const std::string pygiveFile()    const { return _pygiveFile.value(); }
  int msel()           const { return _msel.value(); }
  int selectPdg()           const { return _selectPdgCode.value(); }
  int firstlistevent() const { return _firstlistevent.value(); }
  int lastlistevent()  const { return _lastlistevent.value(); }
  int eventlistlevel() const { return _eventlistlevel.value(); }
  int initlistlevel()  const { return _initlistlevel.value(); }
  int statlistlevel()  const { return _statlistlevel.value(); }
  //  int randomseed()     const { return _randomseed.value(); }
  int decayTableMode() const{ return _decayTableMode.value(); }
  double win()         const  { return _win.value(); }
  double expWeight()   const  { return _expWeight.value(); }
  int weightParam()    const  { return _weightParam.value(); }
  int pyeditAction()    const  { return _pyeditAction.value(); }
  bool allowBMixing()    const  { return _allowBMixing.value(); }
  int tuneUE()    const  { return _tuneUE.value(); }

private: 
  
  // various input parameters
  AbsParmGeneral<int>  _msel;
  AbsParmGeneral<int>  _selectPdgCode;
  AbsParmGeneral<int>  _firstlistevent;
  AbsParmGeneral<int>  _lastlistevent;
  AbsParmGeneral<int>  _eventlistlevel;
  AbsParmGeneral<int>  _initlistlevel;
  AbsParmGeneral<int>  _statlistlevel;
  AbsParmGeneral<int>  _decayTableMode;
  AbsParmGeneral<int>  _weightParam;
  AbsParmGeneral<int>  _pyeditAction;
  AbsParmGeneral<double>  _expWeight;
  //  AbsParmGeneral<int>  _randomseed;
  AbsParmGeneral<double>  _win;
  AbsParmGeneral<std::string>  _target;
  AbsParmGeneral<std::string>  _frame;
  AbsParmGeneral<std::string>  _pygiveFile;
  AbsParmGeneral<bool>  _allowBMixing;
  AbsParmGeneral<int>  _tuneUE;

  // Pysubs data, methods and talk-to stuff 
  // msel handled alone.
  APPMenu _pysubsMenu;
  SetMsubCommand _msubCommand;
  SetCkinCommand _ckinCommand;
  SetKfinCommand _kfinCommand;
  ShowMsubCommand _msubShowCommand;
  ShowCkinCommand _ckinShowCommand;
  ShowKfinCommand _kfinShowCommand;

  // Decay table handling
  AbsParmEnum _decayFileMode;
  AbsParmFilename _decayFile;

  // Pydat1 data, methods and talk-to stuff
  APPMenu _commonMenu;
  SetMstuCommand _mstuCommand;
  SetParuCommand _paruCommand;
  SetMstjCommand _mstjCommand;
  SetParjCommand _parjCommand;  
  ShowMstuCommand _mstuShowCommand;
  ShowParuCommand _paruShowCommand;
  ShowMstjCommand _mstjShowCommand;
  ShowParjCommand _parjShowCommand;  

  // Pypars data, methods and talk-to stuff
  APPMenu _pyparsMenu;
  SetMstpCommand _mstpCommand;
  SetParpCommand _parpCommand;
  ShowMstpCommand _mstpShowCommand;
  ShowParpCommand _parpShowCommand;

  // Pydat2 data, methods and talk-to stuff
  SetPmasCommand _pmasCommand;
  ShowPmasCommand _pmasShowCommand;

  // Pydat3 data, methods and talk-to stuff
  SetMdcyCommand _mdcyCommand;
  SetMdmeCommand _mdmeCommand;
  SetBratCommand _bratCommand;
  SetKfdpCommand _kfdpCommand;
  ShowMdcyCommand _mdcyShowCommand;
  ShowMdmeCommand _mdmeShowCommand;
  ShowBratCommand _bratShowCommand;
  ShowKfdpCommand _kfdpShowCommand;

  // Pyint2 data, methods and talk-to stuff
  SetIsetCommand _isetCommand;
  SetKfprCommand _kfprCommand;
  SetCoefCommand _coefCommand;
  ShowIsetCommand _isetShowCommand;
  ShowKfprCommand _kfprShowCommand;
  ShowCoefCommand _coefShowCommand;

  // Pytcsm data, methods and talk-to stuff
  SetItcmCommand _itcmCommand;
  SetRtcmCommand _rtcmCommand;
  ShowItcmCommand _itcmShowCommand;
  ShowRtcmCommand _rtcmShowCommand;

  // SUSY access
  APPMenu _susyMenu;
  SetImssCommand _imssCommand;
  SetRmssCommand _rmssCommand;
  ShowImssCommand _imssShowCommand;
  ShowRmssCommand _rmssShowCommand;
  

};

#endif // PYTHIAMENU_HH
