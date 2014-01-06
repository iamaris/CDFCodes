#include "generatorMods/PythiaSetCommands.hh"
#include "Framework/APPModule.hh"
#include "ErrorLogger_i/gERRLOG.hh"

#include <iostream> 
#include <iomanip>
#include <sstream>
#include <string>
using std::ostream;
using std::istringstream;
#include <list>
using std::ostream;
using std::setw;
using std::setiosflags;
using std::setprecision;
using std::cout;
using std::endl;
using std::list;
using std::cerr;
using std::string;
using std::ios;

#include "pythia_i/Pythia.hh"
 

////////////////////////////////////////////////////////////////////////
// SetMstuCommand
////////////////////////////////////////////////////////////////////////

SetMstuCommand::SetMstuCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMstuCommand::~SetMstuCommand() { }

bool SetMstuCommand::isShowable() const {
  return false; }

void SetMstuCommand::show() const {
}

int SetMstuCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pydat1().lenMstu())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        pythia->pydat1().mstu(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetMstuCommand::description() const {
  string description("Set MSTU parameters,e.g set_mstu -index=22 -value=100");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetParuCommand
////////////////////////////////////////////////////////////////////////

SetParuCommand::SetParuCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetParuCommand::~SetParuCommand() { }

bool SetParuCommand::isShowable() const {
  return false; }

void SetParuCommand::show() const {
}

int SetParuCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pydat1().lenParu())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pydat1().paru(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetParuCommand::description() const {
  string description("Set PARU parameters,e.g set_paru -index=12 -value=0.1");
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetMstjCommand
////////////////////////////////////////////////////////////////////////

SetMstjCommand::SetMstjCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMstjCommand::~SetMstjCommand() { }

bool SetMstjCommand::isShowable() const {
  return false; }

void SetMstjCommand::show() const {
}

int SetMstjCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pydat1().lenMstj())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        pythia->pydat1().mstj(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetMstjCommand::description() const {
  string description("Set MSTJ parameters,e.g set_mstj -index=26 -value=1");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetParjCommand
////////////////////////////////////////////////////////////////////////

SetParjCommand::SetParjCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetParjCommand::~SetParjCommand() { }

bool SetParjCommand::isShowable() const {
  return false; }

void SetParjCommand::show() const {
}

int SetParjCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pydat1().lenParj())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pydat1().parj(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetParjCommand::description() const {
  string description("Set PARJ parameters,e.g set_parj -index=21 -value=0.4");
  return description;
}
////////////////////////////////////////////////////////////////////////
// SetMstpCommand
////////////////////////////////////////////////////////////////////////

SetMstpCommand::SetMstpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMstpCommand::~SetMstpCommand() { }

bool SetMstpCommand::isShowable() const {
  return false; }

void SetMstpCommand::show() const {
}

int SetMstpCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          (index > pythia->pypars().lenMstp())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        pythia->pypars().mstp(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetMstpCommand::description() const {
  string description("Set MSTP parameters,e.g set_mstp -index=12 -value=1");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetParpCommand
////////////////////////////////////////////////////////////////////////

SetParpCommand::SetParpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetParpCommand::~SetParpCommand() { }

bool SetParpCommand::isShowable() const {
  return false; }

void SetParpCommand::show() const {
}

int SetParpCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pypars().lenParp())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pypars().parp(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetParpCommand::description() const {
  string description("Set PARP parameters,e.g set_parp -index=14 -value=0.1");
  return description;
}
////////////////////////////////////////////////////////////////////////
// SetMsubCommand
////////////////////////////////////////////////////////////////////////

SetMsubCommand::SetMsubCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMsubCommand::~SetMsubCommand() { }

bool SetMsubCommand::isShowable() const {
  return false; }

void SetMsubCommand::show() const {
}

int SetMsubCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pysubs().lenMsub())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        if (value < 0 || value > 1 ) {
          std::cout << "WARNING non-zero MSUB value is reset to 1" << std::endl;
          value = 1; }
        pythia->pysubs().msub(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetMsubCommand::description() const {
  string description("Set MSUB parameters,e.g set_msub -index=26 -value=1");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetCkinCommand
////////////////////////////////////////////////////////////////////////

SetCkinCommand::SetCkinCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetCkinCommand::~SetCkinCommand() { }

bool SetCkinCommand::isShowable() const {
  return false; }

void SetCkinCommand::show() const {
}

int SetCkinCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pysubs().lenCkin())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pysubs().ckin(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetCkinCommand::description() const {
  string description("Set CKIN parameters,e.g set_ckin -index=41 -value=10.");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetKfinCommand
////////////////////////////////////////////////////////////////////////

SetKfinCommand::SetKfinCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetKfinCommand::~SetKfinCommand() { }

bool SetKfinCommand::isShowable() const {
  return false; }

void SetKfinCommand::show() const {
}

int SetKfinCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int flavour = 0;
  int side = 0; 
  int onoff = 0;
  int nvalid=0;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-flavour -side -onoff",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("flavour")) {
      theInterp->getInt("flavour",flavour);
      int halfLenKfin =  pythia->pysubs().lenKfin()/2;
      if (flavour < -halfLenKfin || flavour > halfLenKfin) {
        std::cerr << command() << ": flavour error - " << flavour <<std::endl;
        return AbsInterp::ERROR;
      }
      if ( theInterp->present("side") ) {
        theInterp->getInt("side",side); 
        if (side!=2 && side != 1) {
          std::cerr << command() << ": side value (" << side 
               << ") should be 1 or 2 only!" << std::endl;
          return AbsInterp::ERROR;
	}
        if ( theInterp->present("onoff") ) {
          theInterp->getInt("onoff",onoff); 
          if (onoff && onoff != 1) {
            std::cerr << command() << ": onoff value (" << onoff 
               << ") should be 0 or 1 only!" << std::endl;
            return AbsInterp::ERROR;
	  }
          pythia->pysubs().kfin(side,flavour)=onoff;
          nvalid++;
        }
      }
    }
    if (!nvalid) {
      std::cerr << target()->name() << "::" << command() 
           << " requires a flavour, a beam or target side " 
           << " and on/off switch" << std::endl;
      return AbsInterp::ERROR;
    }    
  }
  
  return AbsInterp::OK;
}

string SetKfinCommand::description() const {
  string description("Set KFIN parameters, usage:");
  description += "set_kfin -flavour=m -side=0|1] -onoff=0|1";
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetPmasCommand
////////////////////////////////////////////////////////////////////////

SetPmasCommand::SetPmasCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetPmasCommand::~SetPmasCommand() { }

bool SetPmasCommand::isShowable() const {
  return false; }

void SetPmasCommand::show() const {
}

int SetPmasCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int masscode =0;
  double mass = 0.; 
  double width = 0.; 
  double maxdev = 0.; 
  bool valid = false;
  if (theInterp->parseArgs("-masscode -mass -width -maxdev -value",argc,argv)
      == AbsInterp::ERROR) {
    ERRLOG(ELwarning,"[PMAS_BAD_INPUT]")
      << "Parsing of `" << command() << "' failed for module "
      << target()->name() << endmsg;
    return AbsInterp::ERROR;
  }
  if ( theInterp->present("masscode") ) {
    theInterp->getInt("masscode",masscode);
    index = pythia->Pycomp(&masscode);
    if ((index < 1) || 
        (index > pythia->pydat2().lenPmas())) {
      ERRLOG(ELwarning,"[PMAS_BAD_KC]")
             << target()->name() << "::" << command() 
             << ": KF code" << masscode << " does not appear to translate to "
             << " a valid KC code." << endmsg;
      return AbsInterp::ERROR;
    } 
    if (theInterp->present("value")) {
      valid=true;
      ERRLOG(ELwarning,"[PMAS_BAD_INPUT]")
        << target()->name() << "::" << command() 
        << ": ``value'' keyword is deprecated. Use ``mass'' instead."
        << endmsg;
      theInterp->getDouble("value",mass); 
      pythia->pydat2().pmas(index,1) = mass;
    }
    if (theInterp->present("mass")) {
      valid=true;
      theInterp->getDouble("mass",mass); 
      pythia->pydat2().pmas(index,1) = mass;
    }
    if (theInterp->present("width")) {
      valid=true;
      theInterp->getDouble("width",width); 
      pythia->pydat2().pmas(index,2) = width;
    }
    if (theInterp->present("maxdev")) {
      valid=true;
      theInterp->getDouble("maxdev",maxdev); 
      pythia->pydat2().pmas(index,3) = maxdev;
    }
  }
  if (index==0 || !valid) {
    ERRLOG(ELerror,"[PMAS_BAD_INPUT]")
      << target()->name() << "::" << command() 
      << ": masscode key is obligatory, followed by at least one of mass, " 
      << "width or maxdev in GeV." << endmsg;
    return AbsInterp::ERROR;
  }

  return AbsInterp::OK;

}

string SetPmasCommand::description() const {
  string description("Set PMAS parameters. Requires -masscode=KF with at ");
  description+="\n\t\t\tleast one keyword from mass, width and maxdev.";
  description+="\n\t\t\tThese correspond to PMAS(c,1), PMAS(c,2) and";
  description+="\n\t\t\tPMAS(c,3) respectively.";
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetMdcyCommand
////////////////////////////////////////////////////////////////////////

SetMdcyCommand::SetMdcyCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMdcyCommand::~SetMdcyCommand() { }

bool SetMdcyCommand::isShowable() const {
  return false; }

void SetMdcyCommand::show() const {
}

int SetMdcyCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int masscode = 0;
  int decay = 0; 
  int decayTableEntry = 0;
  int nDecayChannels = 0;
  int nvalid=0;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-masscode -decay -decayTableEntry -nDecayChannels",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("masscode")) {
      theInterp->getInt("masscode",masscode);
      index = pythia->Pycomp(&masscode);
      if ((index < 1) || 
          (index > pythia->pydat3().lenMdcy())) {
        std::cerr << command() << ": mass code error - " << masscode << std::endl;
        return AbsInterp::ERROR;
      }
      if ( theInterp->present("decay") ) {
        theInterp->getInt("decay",decay); 
        if (decay && decay != 1) {
          std::cerr << command() << ": decay value (" << decay 
               << ") should be 0 or 1 only!" << std::endl;
          return AbsInterp::ERROR;
        } else {
          pythia->pydat3().mdcy(index,1)=decay;
          nvalid++;
        }
      }
      if ( theInterp->present("decayTableEntry") ) {
        theInterp->getInt("decayTableEntry",decayTableEntry);
        if (decayTableEntry<=0) {
          std::cerr << command() << ": decayTableEntry value (" << decayTableEntry
               << ") should be a positive nonzero index in the decay table!"
               << std::endl;
          return AbsInterp::ERROR;
        } else {
          pythia->pydat3().mdcy(index,2)=decayTableEntry;
          // If the current number of decay channels is 0, set it to one
          if (pythia->pydat3().mdcy(index,3)==0)
            pythia->pydat3().mdcy(index,3)=1;
          nvalid++;
        }
      }
      if ( theInterp->present("nDecayChannels") ) {
        theInterp->getInt("nDecayChannels",nDecayChannels);
        if (nDecayChannels<=0) {
          std::cerr << command() << ": nDecayChannels value (" << nDecayChannels
               << ") should be a positive nonzero value!" << std::endl;
          return AbsInterp::ERROR;
        } else {
          pythia->pydat3().mdcy(index,3)=nDecayChannels;
        }
        nvalid++;
      }
    }
    if (!nvalid) {
      std::cerr << target()->name() << "::" << command() 
           << " requires a `masscode' key and *at least* one other key " 
           << " from `decay', `decayTableEntry' and `nDecayChannels'" << std::endl;
      return AbsInterp::ERROR;
    }    
  }
  
  return AbsInterp::OK;
}

string SetMdcyCommand::description() const {
  string description("Set MDCY parameters, usage:");
  description += "set_mdcy -masscode=m [-decay=0|1] [-decayTableEntry=i]";
  description += " [-nDecayChannels=n].";
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetMdmeCommand
////////////////////////////////////////////////////////////////////////

SetMdmeCommand::SetMdmeCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMdmeCommand::~SetMdmeCommand() { }

bool SetMdmeCommand::isShowable() const {
  return false; }

void SetMdmeCommand::show() const {
}

int SetMdmeCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int masscode = 0;
  int massIndex = 0;
  int decayType = 0; 
  int matrix = 0;
  int nvalid=0;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-channelIndex -decayType -matrix -masscode",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    ERRLOG(ELerror,"[MDME_BAD_INPUT]")
      << target()->name() << "::" << command() 
      << ": parsing failed." << endmsg;
  } else {
    if ( theInterp->present("masscode")) {
      theInterp->getInt("masscode",masscode);
      massIndex = pythia->Pycomp(&masscode);
      if ((massIndex < 1) || 
          (massIndex > pythia->pydat2().lenPmas())) {
        ERRLOG(ELwarning,"[PMAS_BAD_KC]")
             << target()->name() << "::" << command() 
             << ": KC code" << masscode << "does not appear to translate to "
             << " a valid KF code." << endmsg;
	return AbsInterp::ERROR;
      }
    }
    if ( theInterp->present("channelIndex")) {
      theInterp->getInt("channelIndex",index);
      bool indexError=false;
      if (massIndex) {
	if (index < -1 || index >= pythia->pydat3().mdcy(massIndex,3)) 
          indexError=true;
      } else {
        if (index <  1 || index >  pythia->pydat3().lenMdme()) indexError=true;
      }
      if (indexError) {
        ERRLOG(ELerror,"[MDME_BAD_INPUT]")
        << target()->name() << "::" << command() 
        << ": channel index error - " << index << endmsg;
        return AbsInterp::ERROR;
      }
      if ( theInterp->present("decayType") ) {
        theInterp->getInt("decayType",decayType); 
        if (decayType < -1 || decayType > 5) {
          ERRLOG(ELerror,"[MDME_BAD_INPUT]")
            << target()->name() << "::" << command() 
            << ": Bad decayType (" << decayType
            << ") should be -1->5! See Pythia manual" << endmsg;
          return AbsInterp::ERROR;
        } else {
	  // if the mass code is given the index is the relative position for
	  // that particle, a value of -1 sets all decay channels
          if(massIndex) {
            int start = pythia->pydat3().mdcy(massIndex,2);
            if(index == -1) {
              for(int i = start ;
		      i < start + pythia->pydat3().mdcy(massIndex,3); i++)
		pythia->pydat3().mdme(i,1)=decayType;
	    } else  pythia->pydat3().mdme(start+index,1)=decayType;
	  } else pythia->pydat3().mdme(index,1)=decayType;
          nvalid++;
        }
      }
      if ( theInterp->present("matrix") ) {
        theInterp->getInt("matrix",matrix); 
        // For allowed values of this parameter, see the Pythia Manual
        // and updates
        switch (matrix) {
        case   0:
        case   1:
        case   2:
        case   3:
        case   4:
        case  11:
        case  12:
        case  13:
        case  14:
        case  15:
        case  22:
        case  23:
        case  24:
        case  25:
        case  26:
        case  27:
        case  28:
        case  29:
        case  30:
        case  31:
        case  32:
        case  33:
        case  41:
        case  42:
        case  43:
        case  44:
        case  45:
        case  46:
        case  48:
        case  50:
        case  51:
        case  52:
        case  53:
        case  54:
        case  55:
        case  56:
        case  57:
        case  58:
        case  59:
        case  84:
        case  85:
        case  86:
        case  87:
        case  88:
        case 101:
        case 102:
	  // if the mass code is given the index is the relative position for
	  // that particle, a value of -1 sets all decay channels
          if(massIndex) {
            int start = pythia->pydat3().mdcy(massIndex,2);
            if(index == -1) {
              for(int i = start ;
		      i < start + pythia->pydat3().mdcy(massIndex,3); i++)
		pythia->pydat3().mdme(i,2)=matrix;
	    } else  pythia->pydat3().mdme(start+index,2)=matrix;
	  } else pythia->pydat3().mdme(index,2)=matrix;
          ++nvalid;
          break;   
        default:
      ERRLOG(ELerror,"[MDME_BAD_INPUT]")
        << target()->name() << "::" << command() 
        << ": Bad matrix element treatment selector(" << matrix
        << ")! See Pythia manual for allowed values" << endmsg;
          return AbsInterp::ERROR;
        };
      }
    }
    if (!nvalid) {
      ERRLOG(ELerror,"[MDME_BAD_INPUT]")
        << target()->name() << "::" << command() 
        << " requires a ``channelIndex'' key and *at least* one other key " 
        << " from ``decayType'' and ``matrix''" << endmsg;
      return AbsInterp::ERROR;
    }    
  }
  
  return AbsInterp::OK;
}

string SetMdmeCommand::description() const {
  string description("Set MDME parameters, usage:");
  description += "set_mdme -channelIndex=idc \n";
  description += "\t\t\t[-decayType={-1,5} (MDME(n,1))]\n";
  description += "\t\t\t[-matrix=n (MDME(n,2))]\n";
  description += "\t\t\t[-masscode=KF (PDGcode),if given the index is taken\n";
  description += "\t\t\t to be relative to the first decay channel,an index\n";
  description += "\t\t\t of -1 sets all decay channels]";
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetBratCommand
////////////////////////////////////////////////////////////////////////

SetBratCommand::SetBratCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetBratCommand::~SetBratCommand() { }

bool SetBratCommand::isShowable() const {
  return false; }

void SetBratCommand::show() const {
}  


int SetBratCommand::handle(int argc, char* argv[]) {
  
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double brRatio = 0; 
  int nvalid=0;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-channelIndex -brRatio",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("channelIndex")) {
      theInterp->getInt("channelIndex",index);
      if ((index < 1) || 
          (index > pythia->pydat3().lenBrat())) {
        std::cerr << command() << ": channelIndex error - " << index << std::endl;
        return AbsInterp::ERROR;
      }
      if ( theInterp->present("brRatio") ) {
        theInterp->getDouble("brRatio",brRatio); 
        if (brRatio < 0.0 || brRatio > 1.0) {
          std::cerr << command() << ": Nonsensical branching ratio (" 
               << setiosflags(ios::showpoint) << brRatio
               << ") should be {0.,1.} See Pythia manual" << std::endl;
          return AbsInterp::ERROR;
        } else {
          pythia->pydat3().brat(index)=brRatio;
          nvalid++;
        }
      }
    }
    if (!nvalid) {
      std::cerr << target()->name() << "::" << command() 
           << " requires *both* a `channelIndex' key and a valid value for `brRatio'."
           << std::endl;
      return AbsInterp::ERROR;
    }    
  }
  
  return AbsInterp::OK;
}

string SetBratCommand::description() const {
  string description("Set branching ratios for decay table entries, usage:");
  description += "set_brat -channelIndex=idc -brRatio={0.,1.}";
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetKfdpCommand
////////////////////////////////////////////////////////////////////////

SetKfdpCommand::SetKfdpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetKfdpCommand::~SetKfdpCommand() { }

bool SetKfdpCommand::isShowable() const {
  return false; }

void SetKfdpCommand::show() const {
}

int SetKfdpCommand::handle(int argc, char* argv[]) {
  
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int nproducts = 0;
  string tmpString;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-channelIndex -products",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("channelIndex")) {
      theInterp->getInt("channelIndex",index);
      if ((index < 1) || 
          (index > pythia->pydat3().lenKfdp())) {
        std::cerr << command() << ": channelIndex error - " << index << std::endl;
        return AbsInterp::ERROR;
      }
      if ( theInterp->present("products") ) {
        list<int> tmpList;
        if ( AbsInterp::ERROR == theInterp->getList("products",tmpList)) {
          std::cerr << command() 
               << ": problems getting products values. Check input."
               << std::endl;
          return AbsInterp::ERROR;
        }
        nproducts = tmpList.size();
        if (nproducts<1 || nproducts>pythia->pydat3().depthKfdp()) {
          std::cerr << command() << ": illegal number of decay products ("
               << nproducts << "), minimum 1, maximum " 
               << pythia->pydat3().depthKfdp() << std::endl;
          return AbsInterp::ERROR;
        }
        int n=1;
        for (list<int>::const_iterator i = tmpList.begin();
             i != tmpList.end(); ++i,++n) {
          pythia->pydat3().kfdp(index,n)=*i;
        } 
      }
    }
    if (!nproducts) {
      std::cerr << target()->name() << "::" << command() 
           << " requires a `channelIndex' key and at least one value for `products'."
           << std::endl;
      return AbsInterp::ERROR;
    }    
  }
  return AbsInterp::OK;
}

string SetKfdpCommand::description() const {
  string description("Set KF codes of decay products  for decay table entries, usage:");
  description += "set_kfdp -channelIndex=idc -products=kf1,kf2,...";
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetIsetCommand
////////////////////////////////////////////////////////////////////////

SetIsetCommand::SetIsetCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetIsetCommand::~SetIsetCommand() { }

bool SetIsetCommand::isShowable() const {
  return false; }

void SetIsetCommand::show() const {
}

int SetIsetCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pyint2().lenIset())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        if (value < 0 || value > 1 ) {
          std::cout << "WARNING non-zero MSUB value is reset to 1" << std::endl;
          value = 1; }
        pythia->pyint2().iset(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetIsetCommand::description() const {
  string description("Set ISET parameters,e.g set_iset -index=171 -value=2");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetKfprCommand
////////////////////////////////////////////////////////////////////////

SetKfprCommand::SetKfprCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetKfprCommand::~SetKfprCommand() { }

bool SetKfprCommand::isShowable() const {
  return false; }

void SetKfprCommand::show() const {
}

int SetKfprCommand::handle(int argc, char* argv[]) {
  
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int product = 0;
  int flavourCode;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -product -flavourCode",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index")   &&
         theInterp->present("product") &&
         theInterp->present("flavourCode") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("product",product);
      theInterp->getInt("flavourCode",flavourCode);
      if ((index < 1) || 
          (index > pythia->pyint2().lenKfpr())) {
        std::cerr << command() << ": index error - " << index << std::endl;
        return AbsInterp::ERROR;
      }
      theInterp->getInt("product",product);
      if (product<1 || product>pythia->pyint2().depthKfpr()) {
        std::cerr << command() << ": illegal process product "
                  << product << std::endl;
        return AbsInterp::ERROR; 
      }
      pythia->pyint2().kfpr(index,product) = flavourCode;
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires all keys `index',`product' and 'flavourCode" << std::endl;
      result = AbsInterp::ERROR;
    }    
  }
  return AbsInterp::OK;
}

string SetKfprCommand::description() const {
  string description("Set KF codes of products produced in sub process ");
  description += "set_kfpr -index=isub -product=j  -flavourCode=kf";
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetCoefCommand
////////////////////////////////////////////////////////////////////////

SetCoefCommand::SetCoefCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetCoefCommand::~SetCoefCommand() { }

bool SetCoefCommand::isShowable() const {
  return false; }

void SetCoefCommand::show() const {
}

int SetCoefCommand::handle(int argc, char* argv[]) {
  
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int factor = 0;
  double coefficient;
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -factor -coefficient",
                         argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index")   &&
         theInterp->present("factor") &&
         theInterp->present("coefficient") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("factor",factor);
      theInterp->getDouble("coefficient",coefficient);
      if ((index < 1) || 
          (index > pythia->pyint2().lenCoef())) {
        std::cerr << command() << ": index error - " << index << std::endl;
        return AbsInterp::ERROR;
      }
      theInterp->getInt("factor",factor);
      if (factor<1 || factor>pythia->pyint2().depthCoef()) {
        std::cerr << command() << ": illegal process factor "
                  << factor << std::endl;
        return AbsInterp::ERROR; 
      }
      pythia->pyint2().coef(index,factor) = coefficient;
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires all keys `index',`factor' and `coefficient'" << std::endl;
      result = AbsInterp::ERROR;
    }    
  }
  return AbsInterp::OK;
}

string SetCoefCommand::description() const {
  string description("Set Jacobian factors for given subprocess, usage:");
  description += "set_coef -index=isub -factor=j -coefficient=coefficient";
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetImssCommand
////////////////////////////////////////////////////////////////////////

SetImssCommand::SetImssCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) 
{
}

SetImssCommand::~SetImssCommand() 
{ 
}

bool SetImssCommand::isShowable() const {
  return false ;
}

void SetImssCommand::show() const {
}

int SetImssCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pymssm().lenImss())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        pythia->pymssm().imss(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetImssCommand::description() const {
  string description("Set IMSS parameters,e.g set_imss -index=22 -value=100");
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetRmssCommand
////////////////////////////////////////////////////////////////////////

SetRmssCommand::SetRmssCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetRmssCommand::~SetRmssCommand() { }

bool SetRmssCommand::isShowable() const {
  return false; }

void SetRmssCommand::show() const {
}

int SetRmssCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pymssm().lenRmss())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pymssm().rmss(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetRmssCommand::description() const {
  string description("Set RMSS parameters,e.g set_rmss -index=12 -value=0.1");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetItcmCommand
////////////////////////////////////////////////////////////////////////

SetItcmCommand::SetItcmCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) 
{
}

SetItcmCommand::~SetItcmCommand() 
{ 
}

bool SetItcmCommand::isShowable() const {
  return false ;
}

void SetItcmCommand::show() const {
}

int SetItcmCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  int value = 0; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name() << ": " ;
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getInt("value",value); 
      if ((index < 1) || 
          // Change this for new commands:
          (index > pythia->pytcsm().lenItcm())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        pythia->pytcsm().itcm(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }
  return result;
}

string SetItcmCommand::description() const {
  string description("Set ITCM parameters,e.g set_itcm -index=22 -value=100");
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetRtcmCommand
////////////////////////////////////////////////////////////////////////

SetRtcmCommand::SetRtcmCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetRtcmCommand::~SetRtcmCommand() { }

bool SetRtcmCommand::isShowable() const {
  return false; }

void SetRtcmCommand::show() const {
}

int SetRtcmCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Pythia* pythia = Pythia::Instance();

  int index = 0;
  double value = 0.; 

  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-index -value",argc,argv);

  if (result == AbsInterp::ERROR) {
    std::cerr << "Parsing of `" << command() << "' failed for module "
         << target()->name();
  } else {
    if ( theInterp->present("index") &&
         theInterp->present("value") ) {
      theInterp->getInt("index",index);
      theInterp->getDouble("value",value); 
      if ((index < 1) || 
          (index > pythia->pytcsm().lenRtcm())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        pythia->pytcsm().rtcm(index) = value;
        result = AbsInterp::OK;
      }
    } else {
      std::cerr << target()->name() << "::" << command() 
           << " requires both keys `index' *and* `value'" << std::endl;
      result = AbsInterp::ERROR;
    }
  }

  return result;
}

string SetRtcmCommand::description() const {
  string description("Set RTCM parameters,e.g set_rtcm -index=12 -value=0.1");
  return description;
}


