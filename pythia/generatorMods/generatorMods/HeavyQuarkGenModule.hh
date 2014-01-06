//------------------------------------------------------------------------------
// Description:
// -----------
//	Class HeavyQuarkGenModule
//
// Author List: S.Gromoll, Ch.Paus
//
//  revision history:
//  -----------------
// *0001 Apr 25 2002 S.Gromoll+Ch,Paus created
// *0002 Apr 14 2003 G. Giurgiu, M. Paulini add D*0, D*+ and conjugates
//------------------------------------------------------------------------------
#ifndef _HEAVYQUARKGENMODULE_H_
#define _HEAVYQUARKGENMODULE_H_

#include "generatorMods/AbsGenModule.hh"
#include "Framework/AbsParmGeneral.hh"
typedef AbsParmGeneral<long> AbsParmGeneral_long;

class HeavyQuarkGenModule;

//------------------------------------------------------------------------------//------------------------------------------------------------------------------//
// HQDefineCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQDefineCommand : public APPCommand
{
public:
  // Constructors
  HQDefineCommand(const char* comd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

private:
  std::string            _cmd;      // defaults command string
  int                    _idhep;    // HEP particle identifier
  std::string            _name;     // name of particle (user defined)
  std::string            _spec_file;// spectrum file name
  std::string            _spec_name;// name of spectrum in file
  double                 _mass;     // particle mass
  double                 _lifetime; // particle lifetime
  double                 _epsilon;  // peterson epsilon for fragmentation
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetFractionCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQSetFractionCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  int                    _hepfrom;     // HEP id of fragmenting particle
  int                    _hepto;       // HEP id of fragmented particle
  double                 _fraction;    // fraction of the particle in the group
  int                    _anti;        // should anti particle fractions also
                                       // be set?  defaults to non-zero (true)
  
public:
  // Constructors
  HQSetFractionCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetDMixtureCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQSetDMixtureCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  double                 _mixu;        // D0     admixture
  double                 _mixd;        // D+     admixture
  double                 _mixs;        // Ds     admixture
  double                 _mixl;        // Lambda admixture
  double                 _mixustar;    // D*0    admixture
  double                 _mixdstar;    // D*+    admixture

  
public:
  // Constructors
  HQSetDMixtureCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetBMixtureCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQSetBMixtureCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  double                 _mixu;        // Bu     admixture
  double                 _mixd;        // Bd     admixture
  double                 _mixs;        // Bs     admixture
  double                 _mixc;        // Bc     admixture
  double                 _mixl;        // Lambda admixture

  
public:
  // Constructors
  HQSetBMixtureCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetDEpsCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQSetDEpsCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  double                 _epsu;        // D0     peterson eps
  double                 _epsd;        // D+     peterson eps
  double                 _epss;        // Ds     peterson eps
  double                 _epsl;        // Lambda peterson eps
  double                 _epsustar;    // D*0     peterson eps
  double                 _epsdstar;    // D*+     peterson eps
  
public:
  // Constructors
  HQSetDEpsCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// HQSetBEpsCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQSetBEpsCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  double                 _epsu;        // B+     peterson eps
  double                 _epsd;        // B0     peterson eps
  double                 _epsc;        // Bc     peterson eps
  double                 _epss;        // Bs     peterson eps
  double                 _epsl;        // Lambda peterson eps

  
public:
  // Constructors
  HQSetBEpsCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};



//------------------------------------------------------------------------------//------------------------------------------------------------------------------//
// HQMixedCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQMixesCommand : public APPCommand
{
private:
  std::string            _cmd;         // defaults command string
  int                    _idhep;       // HEP id of an existing particle
  double                 _delta_m;     // mixing parameter
  int                    _mixes_to;    // what the particle mixes to (defaults
                                       //   to -hep_id)
  
public:
  // Constructors
  HQMixesCommand(const char* cmd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;

};

//------------------------------------------------------------------------------//------------------------------------------------------------------------------//
// HQGenerateCommand
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HQGenerateCommand : public APPCommand {

private:
  std::string            _cmd;      // defaults command string
  int                    _idhep;    // particle to generate
  double                 _fraction; // minimum allowed pt for particle
  double                 _pt_min;   // minimum allowed pt for particle
  double                 _pt_max;   // maximum allowed pt for particle
  double                 _rap_min;  // minimum allowed rapidity for particle
  double                 _rap_max;  // maximum allowed rapidity for particle

public:
  // Constructors
  HQGenerateCommand(const char* comd, HeavyQuarkGenModule* targ);
  
  // Handler
  virtual int         handle(int argc, char* argv[]);
  virtual void        show() const;
  virtual bool        isShowable() const;
  virtual std::string description () const;
};

class HeavyQuarkGenModule : public AbsGenModule {

protected:

  // Data members

  APPMenu                _HQgMenu;

  AbsParmGeneral<int>    _Debug;
  AbsParmGeneral<int>    _Verbose;

  AbsParmGeneral<bool>   _doFrag;
  AbsParmGeneral<bool>   _doMix;
  HQDefineCommand        _Define;
  HQGenerateCommand      _Generate;
  HQMixesCommand         _Mixes;
  HQSetFractionCommand   _SetFraction;
  HQSetDMixtureCommand   _SetDMixture;
  HQSetBMixtureCommand   _SetBMixture;
  HQSetDEpsCommand       _SetDEps;
  HQSetBEpsCommand       _SetBEps;

  // Random number menu
  APPMenu                _randomNumberMenu;
  AbsParmGeneral<long>   _rndmSeed1;
  AbsParmGeneral<long>   _rndmSeed2;

  static const long      _defaultRandomSeed1;
  static const long      _defaultRandomSeed2;

public:

  // Constructors
  HeavyQuarkGenModule(const char* name = "HeavyQuarkGen", 
                      const char* desc = "HeavyQuarkGen Module");

  // Destructor
  virtual ~HeavyQuarkGenModule( );

  const char*      rcsId           () const;

  int              callGenerator   (AbsEvent* event);
  AppResult        genBeginJob     ();
  AppResult        genBeginRun     (AbsEvent* run);
  AppResult        genEndRun       (AbsEvent* job);
  AppResult        genEndJob       ();

  void             fillHepevt();
};
#endif
