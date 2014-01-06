#ifndef TAUOLAMODULE_HH
#define TAUOLAMODULE_HH
/*************************************************************************
* A module to handle tau decays using TAUOLA package
* Created  Jul 05 2001 .. T.Moulik 
**************************************************************************/
// rev:  aug 29 2001 lena: removed _outputMode member. There is no output from
//                         this module
//       aug 30 2001 lena: added ct and genId
//       dec 07 2001 lena: inherited AbsDecpackModule to handle /HEPEVT/

#include "generatorMods/AbsDecpackModule.hh"
#include "Framework/AbsParmGeneral.hh"
#include "Framework/AbsParmString.hh"
#include "Framework/AbsParmBool.hh"
#include "Framework/AbsParmEnum.hh"
#include "tauola_i/tauola_i.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

#include <string>
using std::string;
#include <list>
using std::list;
//              ---------------------
//              -- Class Interface --
//              ---------------------
class TRY_Abstract_Record;

class TauolaModule : public AbsDecpackModule {
  
  //--------------------
  // Instance Members --
  //--------------------
  
public:

  static const char* genId;
  //
  // Constructors
  //
  
  TauolaModule();

  //
  // Destructor
  //
  virtual ~TauolaModule();
  
  // Operations
  AppResult genBeginJob();
  AppResult genEndJob();
  AppResult genBeginRun( AbsEvent* anEvent );
  AppResult genEndRun( AbsEvent* anEvent );
  int       callGenerator( AbsEvent* anEvent );

protected :

  AbsParmGeneral<int>    _nevprt;
  AbsParmGeneral<int>    _nprt;
  AbsParmGeneral<int>    _tauchirality;
  AbsParmGeneral<int>    _jak1;
  AbsParmGeneral<int>    _jak2;
  AbsParmGeneral<double> _xk0dec;
  AbsParmGeneral<int>    _itdkrc;
  AbsParmGeneral<bool>   _disableLeptonicDecays;

  // Random number menu
  APPMenu _randomNumberMenu;
  
  // Initial random number seeds (2) (ignored if restoreState is requested)
  AbsParmGeneral_long _randomSeed1;
  static const   long _defaultRandomSeed1;
  AbsParmGeneral_long _randomSeed2;
  static const   long _defaultRandomSeed2;
  
private:
  void _initializeTauolaTalkTo();
  void _absParm2Tauola();
  void _tauola2AbsParm();

};

#endif //TAUOLAMODULE_HH
