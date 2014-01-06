#include "generatorMods/AriadneSetCommands.hh"
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

#include "pythia_i/Ariadne.hh"
 

////////////////////////////////////////////////////////////////////////
// SetMstaCommand
////////////////////////////////////////////////////////////////////////

SetMstaCommand::SetMstaCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetMstaCommand::~SetMstaCommand() { }

bool SetMstaCommand::isShowable() const {
  return false; }

void SetMstaCommand::show() const {
}

int SetMstaCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->ardat1().lenMsta())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        // Change this for new commands
        ariadne->ardat1().msta(index) = value;
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

string SetMstaCommand::description() const {
  string description("Set MSTA parameters,e.g set_msta -index=22 -value=100");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetParaCommand
////////////////////////////////////////////////////////////////////////

SetParaCommand::SetParaCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetParaCommand::~SetParaCommand() { }

bool SetParaCommand::isShowable() const {
  return false; }

void SetParaCommand::show() const {
}

int SetParaCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->ardat1().lenPara())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->ardat1().para(index) = float(value);
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

string SetParaCommand::description() const {
  string description("Set PARA parameters,e.g set_para -index=12 -value=0.1");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetPqmasCommand
////////////////////////////////////////////////////////////////////////

SetPqmasCommand::SetPqmasCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetPqmasCommand::~SetPqmasCommand() { }

bool SetPqmasCommand::isShowable() const {
  return false; }

void SetPqmasCommand::show() const {
}

int SetPqmasCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->ardat2().lenPqmas())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->ardat2().pqmas(index) = value;
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

string SetPqmasCommand::description() const {
  string description("Set Pqmas parameters,e.g set_pqmas -index=6 -value=175.0");
  return description;
}


////////////////////////////////////////////////////////////////////////
// SetCaCommand
////////////////////////////////////////////////////////////////////////

SetCaCommand::SetCaCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetCaCommand::~SetCaCommand() { }

bool SetCaCommand::isShowable() const {
  return false; }

void SetCaCommand::show() const {
}

int SetCaCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->arpopa().lenCa())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->arpopa().ca(index) = value;
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

string SetCaCommand::description() const {
  string description("Set CA parameters,e.g set_ca -index=1 -value=6.38");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetPbCommand
////////////////////////////////////////////////////////////////////////

SetPbCommand::SetPbCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetPbCommand::~SetPbCommand() { }

bool SetPbCommand::isShowable() const {
  return false; }

void SetPbCommand::show() const {
}

int SetPbCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->arpopa().lenCa())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->arpopa().pb(index) = value;
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

string SetPbCommand::description() const {
  string description("Set PB parameters,e.g set_pb -index=2 -value=3.0");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetCfCommand
////////////////////////////////////////////////////////////////////////

SetCfCommand::SetCfCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetCfCommand::~SetCfCommand() { }

bool SetCfCommand::isShowable() const {
  return false; }

void SetCfCommand::show() const {
}

int SetCfCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->arpopa().lenCf())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->arpopa().cf(index) = value;
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

string SetCfCommand::description() const {
  string description("Set CF parameters,e.g set_cf -index=2 -value=1.5");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetXaCommand
////////////////////////////////////////////////////////////////////////

SetXaCommand::SetXaCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetXaCommand::~SetXaCommand() { }

bool SetXaCommand::isShowable() const {
  return false; }

void SetXaCommand::show() const {
}

int SetXaCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->arpopa().lenCf())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->arpopa().xa(index) = value;
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

string SetXaCommand::description() const {
  string description("Set XA parameters,e.g set_xa -index=6 -value=1.0");
  return description;
}

////////////////////////////////////////////////////////////////////////
// SetNbCommand
////////////////////////////////////////////////////////////////////////

SetNbCommand::SetNbCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

SetNbCommand::~SetNbCommand() { }

bool SetNbCommand::isShowable() const {
  return false; }

void SetNbCommand::show() const {
}

int SetNbCommand::handle(int argc, char* argv[]) {

  AbsInterp *theInterp = AbsInterp::theInterpreter();
  Ariadne* ariadne = Ariadne::Instance();

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
          (index > ariadne->arpopa().lenCf())) {
        std::cerr << command() << ": index out of range" << std::endl;
        result = AbsInterp::ERROR;
      } else {
        ariadne->arpopa().nb(index) = int(value);
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

string SetNbCommand::description() const {
  string description("Set NB parameters,e.g set_nb -index=0 -value=1");
  return description;
}
