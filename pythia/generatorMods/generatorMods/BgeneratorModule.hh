//------------------------------------------------------------------------------
// Description:
// -----------
//	Class BgeneratorModule
//
// Author List: K.Anikeev, P.Murat, Ch.Paus
//
//  revision history:
//  -----------------
// *0001 Feb 08 1999 K.Anikeev:      created
// *0002 Jul 14 1999 Ch.Paus:        created AC++ talk-to
// *0003 Feb 26 2001 Ch.Paus:        ability to switch off fragmentation
// *0004 Mar 06 2001 Ch.Paus:        package Bgenerator
// *0005 Mar 20 2002 Lena/Christoph: unified random numbers
//------------------------------------------------------------------------------
#ifndef _BGENERATORMODULE_H_
#define _BGENERATORMODULE_H_

#include "generatorMods/AbsGenModule.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class BgeneratorModule;

class DefineCommand : public APPCommand {

private:
  std::string            _cmd;     // defaults command string
  int                    _ipart;   // particle number
  int                    _bidhep;  // B HEP particle identifier
  std::string            _name;    // name of particle (user defined)
  std::string            _aname;   // name of anti particle (user defined)
  float                  _bm;      // particle mass
  float                  _bl;      // particle lifetime
  
public:
  // Constructors
  DefineCommand(const char* comd, BgeneratorModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

};

class NdeCommand : public APPCommand {

private:
  std::string            _cmd;     // defaults command string
  int                    _gentype; // generation type
  std::string            _file;    // file name for input spectrum
  int                    _ihist;   // histogram number
  float                  _bxsec;   // total b cross section
  
public:
  // Constructors
  NdeCommand(const char* comd, BgeneratorModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

};

class MnrCommand : public APPCommand {

private:
  std::string            _cmd;     // defaults command string
  int                    _gentype; // generation type
  std::string            _file;    // file name for input spectrum
  int                    _ihistpp; // histogram number Ptmin vs Ptmax
  int                    _ihistdp; // histogram number delta-phi
  int                    _ihistyy; // histogram number Y* vs Yboost
  
public:
  // Constructors
  MnrCommand(const char* comd, BgeneratorModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

};

class BMesonCommand : public APPCommand {

private:
  std::string            _cmd;     // defaults command string
  int                    _gencode; // generation code
  int                    _bmeson;  // bmeson to generate
  float                  _mixu;    // mix for Bu
  float                  _mixd;	   // mix for Bd
  float                  _mixs;	   // mix for Bs
  float                  _mixc;	   // mix for Bc
  float                  _mixl;	   // mix for Lambda b
  float                  _mixx;    // mix for user defined particle

public:
  // Constructors
  BMesonCommand(const char* comd, BgeneratorModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};

class CMesonCommand : public APPCommand {

private:
  std::string            _cmd;     // defaults command string
  bool                   _active;  // is CMESON option active?

public:
  // Constructors
  CMesonCommand(const char* comd, BgeneratorModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};

class BgeneratorModule : public AbsGenModule {

protected:

  // Data members

  APPMenu                _BgMenu;

  AbsParmGeneral<int>    _VERBOSE;

  AbsParmGeneral<float>  _PT_MIN_B1;
  AbsParmGeneral<float>  _PT_MIN_B2;
  AbsParmGeneral<float>  _RAPMIN;
  AbsParmGeneral<float>  _RAPMAX;
  AbsParmGeneral<int>    _FRAGMENT;
  AbsParmGeneral<float>  _EPS;
  AbsParmGeneral<float>  _MASS_BQ;
  AbsParmGeneral<float>  _MASS_CQ;
  AbsParmGeneral<bool>   _DOMIX;
  AbsParmGeneral<float>  _X_D;
  AbsParmGeneral<float>  _X_S;

  DefineCommand          _DEFINE;
  NdeCommand             _NDE;
  MnrCommand             _MNR;
  BMesonCommand          _BMESON;
  CMesonCommand          _CMESON;

  // Random number menu
  APPMenu                _randomNumberMenu;
  AbsParmGeneral<long>   _rndmSeed1;
  AbsParmGeneral<long>   _rndmSeed2;

  static const long      _defaultRandomSeed1;
  static const long      _defaultRandomSeed2;

public:

  // Constructors
  BgeneratorModule(const char* name = "Bgenerator", 
		   const char* desc = "Bgenerator Module");
  BgeneratorModule(const BgeneratorModule& m);

  // Destructor
  virtual ~BgeneratorModule( );

  void             Bgen2AbsParm    ();
  void             AbsParm2Bgen    ();
  const char*      rcsId           () const;

  int              callGenerator   (AbsEvent* event);
  AppResult        genBeginJob     ();
  AppResult        genBeginRun     (AbsEvent* run);
  AppResult        genEndRun       (AbsEvent* job);
  AppResult        genEndJob       ();

  void             fillHepevt();
};
#endif
