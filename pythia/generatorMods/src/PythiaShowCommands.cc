#include "generatorMods/PythiaShowCommands.hh"
#include "Framework/APPModule.hh"
#include "pythia_i/Pythia.hh"

#include <iostream> 
#include <iomanip>
#include <sstream>
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
using std::string;
using std::ios;

////////////////////////////////////////////////////////////////////////
// ShowMstuCommand
////////////////////////////////////////////////////////////////////////

ShowMstuCommand::ShowMstuCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMstuCommand::~ShowMstuCommand() { }

bool ShowMstuCommand::isShowable() const {
  return false; }

void ShowMstuCommand::show() const {
}

int ShowMstuCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of MSTU: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pydat1().lenMstu(); ++i) {
    if (showAll || pythia->pydat1().mstu(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "MSTU(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pydat1().mstu(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMstuCommand::description() const {
  string description("Show MSTU parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowParuCommand
////////////////////////////////////////////////////////////////////////

ShowParuCommand::ShowParuCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowParuCommand::~ShowParuCommand() { }

bool ShowParuCommand::isShowable() const {
  return false; }

void ShowParuCommand::show() const {
}

int ShowParuCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();  
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of PARU: " << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pydat1().lenParu(); ++i) {
    if (showAll || pythia->pydat1().paru(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "PARU(" << setw(3) << i << ") = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pydat1().paru(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowParuCommand::description() const {
  string description("Show PARU parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowMstjCommand
////////////////////////////////////////////////////////////////////////

ShowMstjCommand::ShowMstjCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMstjCommand::~ShowMstjCommand() { }

bool ShowMstjCommand::isShowable() const {
  return false; }

void ShowMstjCommand::show() const {
}

int ShowMstjCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of MSTJ: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pydat1().lenMstj(); ++i) {
    if (showAll || pythia->pydat1().mstj(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "MSTJ(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pydat1().mstj(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMstjCommand::description() const {
  string description("Show MSTJ parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowParjCommand
////////////////////////////////////////////////////////////////////////

ShowParjCommand::ShowParjCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowParjCommand::~ShowParjCommand() { }

bool ShowParjCommand::isShowable() const {
  return false; }

void ShowParjCommand::show() const {
}

int ShowParjCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of PARJ: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pydat1().lenParj(); ++i) {
    if (showAll || pythia->pydat1().parj(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "PARJ(" << setw(3) << i << ") = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pydat1().parj(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowParjCommand::description() const {
  string description("Show PARJ parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowMstpCommand
////////////////////////////////////////////////////////////////////////

ShowMstpCommand::ShowMstpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMstpCommand::~ShowMstpCommand() { }

bool ShowMstpCommand::isShowable() const {
  return false; }

void ShowMstpCommand::show() const {
}

int ShowMstpCommand::handle(int argc, char* argv[]) {
  
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of MSTP: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pypars().lenMstp(); ++i) {
    if (showAll || pythia->pypars().mstp(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "MSTP(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pypars().mstp(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMstpCommand::description() const {
  string description("Show MSTP parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowParpCommand
////////////////////////////////////////////////////////////////////////

ShowParpCommand::ShowParpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowParpCommand::~ShowParpCommand() { }

bool ShowParpCommand::isShowable() const {
  return false; }

void ShowParpCommand::show() const {
}

int ShowParpCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of PARP: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pypars().lenParp(); ++i) {
    if (showAll || pythia->pypars().parp(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "PARP(" << setw(3) << i << ") = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pypars().parp(i) << "  ";
      ++ii;
    }
  }  
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowParpCommand::description() const {
  string description("Show PARP parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowMsubCommand
////////////////////////////////////////////////////////////////////////

ShowMsubCommand::ShowMsubCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMsubCommand::~ShowMsubCommand() { }

bool ShowMsubCommand::isShowable() const {
  return false; }

void ShowMsubCommand::show() const {
}

int ShowMsubCommand::handle(int argc, char* argv[]) {
  
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of MSUB: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pysubs().lenMsub(); ++i) {
    if (showAll || pythia->pysubs().msub(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "MSUB(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pysubs().msub(i) << "  ";
      ++ii;
    }
  }  
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMsubCommand::description() const {
  string description("Show MSUB parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowCkinCommand
////////////////////////////////////////////////////////////////////////

ShowCkinCommand::ShowCkinCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowCkinCommand::~ShowCkinCommand() { }

bool ShowCkinCommand::isShowable() const {
  return false; }

void ShowCkinCommand::show() const {
}

int ShowCkinCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " elements of CKIN: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pysubs().lenCkin(); ++i) {
    if (showAll || pythia->pysubs().ckin(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "CKIN(" << setw(3) << i << ") = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pysubs().ckin(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowCkinCommand::description() const {
  string description("Show CKIN parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowKfinCommand
////////////////////////////////////////////////////////////////////////

ShowKfinCommand::ShowKfinCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowKfinCommand::~ShowKfinCommand() { }

bool ShowKfinCommand::isShowable() const {
  return false; }

void ShowKfinCommand::show() const {
}

int ShowKfinCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  //  AbsInterp *theInterp = AbsInterp::theInterpreter();
  //AbsInterp::StatusCode result = 
  //  theInterp->parseArgs("-all",argc,argv);
  //bool showAll = theInterp->present("all");
  const int flavPerLine = 4;
  std::cout << std::endl << "All elements of KFIN: flavour(beam/target) then anti-flavour(beam/target) " 
            << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pysubs().lenKfin()/2; ++i) {
    if (!(ii%flavPerLine)) std::cout << std::endl;
    std::cout << "CKIN(" << setw(2) << i << ")= " 
           <<  pythia->pysubs().kfin(1,i) << " "
           <<  pythia->pysubs().kfin(2,i) << "  "
           <<  pythia->pysubs().kfin(1,-i) << " "
           <<  pythia->pysubs().kfin(2,-i) << " ";
      ++ii;
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowKfinCommand::description() const {
  string description("Show KFIN parameters");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowPmasCommand
////////////////////////////////////////////////////////////////////////

ShowPmasCommand::ShowPmasCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowPmasCommand::~ShowPmasCommand() { }

bool ShowPmasCommand::isShowable() const {
  return false; }

void ShowPmasCommand::show() const {
}

int ShowPmasCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 1;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " mass elements of PMAS: " << std::endl;

  int ii=0;
  for (int i=1; i<= pythia->pydat2().lenPmas(); ++i) {
    if (showAll || pythia->pydat2().pmas(i,1)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "Particle KC " << setw(3) << i << ": mass " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           << pythia->pydat2().pmas(i,1) << " Gev, width "
           << pythia->pydat2().pmas(i,2) << " GeV, maxdev "
           << pythia->pydat2().pmas(i,3) << " GeV";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowPmasCommand::description() const {
  string description("Show PMAS parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowMdcyCommand
////////////////////////////////////////////////////////////////////////

ShowMdcyCommand::ShowMdcyCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMdcyCommand::~ShowMdcyCommand() { }

bool ShowMdcyCommand::isShowable() const {
  return false; }

void ShowMdcyCommand::show() const {
  }

int ShowMdcyCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " mass elements of MDCY: " << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pydat3().lenMdcy(); ++i) {
    if (showAll || pythia->pydat3().mdcy(i,1)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "Mdcy(" << setw(3) << i << ",1) = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pydat3().mdcy(i,1) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMdcyCommand::description() const {
  string description("Show MDCY parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowMdmeCommand
////////////////////////////////////////////////////////////////////////

ShowMdmeCommand::ShowMdmeCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowMdmeCommand::~ShowMdmeCommand() { }

bool ShowMdmeCommand::isShowable() const {
  return false; }

void ShowMdmeCommand::show() const {
}

int ShowMdmeCommand::handle(int argc, char* argv[]) {
  
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 2;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " mass elements of MDME: " << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pydat3().lenMdme(); ++i) {
    if (showAll || pythia->pydat3().mdme(i,1) || 
        pythia->pydat3().mdme(i,2)) { 
      if (!(ii%valsPerLine)) std::cout << std::endl;
      for (int j=1; j<=2; ++j) {
        std::cout << "Mdme(" << setw(3) << i << "," << j << ") = " 
             << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
             <<  pythia->pydat3().mdme(i,j) << "  ";
      }
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowMdmeCommand::description() const {
  string description("Show MDME parameters");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowBratCommand
////////////////////////////////////////////////////////////////////////

ShowBratCommand::ShowBratCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowBratCommand::~ShowBratCommand() { }

bool ShowBratCommand::isShowable() const {
  return false; }

void ShowBratCommand::show() const {
}  

int ShowBratCommand::handle(int argc, char* argv[]) {
  
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " mass elements of BRAT: " << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pydat3().lenBrat(); ++i) {
    if (showAll || pythia->pydat3().brat(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "Brat(" << setw(3) << i << ") = " 
           << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
           <<  pythia->pydat3().brat(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}  

string ShowBratCommand::description() const {
  string description("Show branching ratios for decay table entries, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowKfdpCommand
////////////////////////////////////////////////////////////////////////

ShowKfdpCommand::ShowKfdpCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowKfdpCommand::~ShowKfdpCommand() { }

bool ShowKfdpCommand::isShowable() const {
  return false; }

void ShowKfdpCommand::show() const {
}  

int ShowKfdpCommand::handle(int argc, char* argv[]) {
    
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 1;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " mass elements of KFDP: " << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pydat3().lenKfdp(); ++i) {
    if (showAll || pythia->pydat3().kfdp(i,1) ) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      int j=0;
      std::cout << "Kfdp(" << setw(3) << i << ") = ";
      while (++j <= pythia->pydat3().depthKfdp()) {
        std::cout << pythia->pydat3().kfdp(i,j) << "  ";
      }
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}  

string ShowKfdpCommand::description() const {
  string description("Show KF codes of decay products for decay table entries, usage ");
  description += command() + string(" [-all]");
  return description;
}

////////////////////////////////////////////////////////////////////////
// ShowIsetCommand
////////////////////////////////////////////////////////////////////////

ShowIsetCommand::ShowIsetCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowIsetCommand::~ShowIsetCommand() { }

bool ShowIsetCommand::isShowable() const {
  return false; }

void ShowIsetCommand::show() const {
}  

int ShowIsetCommand::handle(int argc, char* argv[]) {
    
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 5;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " kinematical scheme ISET: " 
            << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pyint2().lenIset(); ++i) {
    if (showAll || pythia->pysubs().msub(i) ) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "Iset(" << setw(3) << i << ") = ";
      std::cout << setw(2) << pythia->pyint2().iset(i) << " ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}  

string ShowIsetCommand::description() const {
  string description("Show kinematical selection scheme for processes, usage ");
  description += command() + string(" [-all]");
  return description;
}

////////////////////////////////////////////////////////////////////////
// ShowKfprCommand
////////////////////////////////////////////////////////////////////////

ShowKfprCommand::ShowKfprCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowKfprCommand::~ShowKfprCommand() { }

bool ShowKfprCommand::isShowable() const {
  return false; }

void ShowKfprCommand::show() const {
}  

int ShowKfprCommand::handle(int argc, char* argv[]) {
    
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 2;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " flavour codes set in KFPR: " 
            << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pyint2().lenKfpr(); ++i) {
    if (showAll || pythia->pysubs().msub(i) ) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      int j=0;
      std::cout << "Kfpr(" << setw(3) << i << ") = ";
      while (++j <= pythia->pyint2().depthKfpr()) {
        std::cout << setw(9) << pythia->pyint2().kfpr(i,j) << "  ";
      }
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}  

string ShowKfprCommand::description() const {
  string description("Show flavour codes for products produced in a process, usage ");
  description += command() + string(" [-all]");
  return description;
}

////////////////////////////////////////////////////////////////////////
// ShowCoefCommand
////////////////////////////////////////////////////////////////////////

ShowCoefCommand::ShowCoefCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowCoefCommand::~ShowCoefCommand() { }

bool ShowCoefCommand::isShowable() const {
  return false; }

void ShowCoefCommand::show() const {
}  

int ShowCoefCommand::handle(int argc, char* argv[]) {
    
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 1;
  std::cout << std::endl << (showAll?"All":"Non-zero") << "factors set in COEF: " 
            << std::endl;
  
  int ii=0;
  for (int i=1; i<= pythia->pyint2().lenCoef(); ++i) {
    if (showAll || pythia->pysubs().msub(i) ) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      int j=0;
      std::cout << "Coef(" << setw(3) << i << ") = ";
      while (++j <= pythia->pyint2().depthCoef()) {
        std::cout << pythia->pyint2().coef(i,j) << "  ";
      }
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}  

string ShowCoefCommand::description() const {
  string description("Show factors used in the Jacobian, normalised relative strengths, usage ");
  description += command() + string(" [-all]");
  return description;
}

////////////////////////////////////////////////////////////////////////
// ShowImssCommand
////////////////////////////////////////////////////////////////////////

ShowImssCommand::ShowImssCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) 
{
}

ShowImssCommand::~ShowImssCommand() 
{ 
}

bool ShowImssCommand::isShowable() const {
  return false; 
}

void ShowImssCommand::show() const {
}

int ShowImssCommand::handle(int argc, char* argv[]) {
 
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " parameter settings of IMSS: " << std::endl;

  int ii=0;
  for (int i=0; i< pythia->pymssm().lenImss(); ++i) {
    if (showAll || pythia->pymssm().imss(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "IMSS(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pymssm().imss(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowImssCommand::description() const {
  string description("Show IMSS parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowRmssCommand
////////////////////////////////////////////////////////////////////////

ShowRmssCommand::ShowRmssCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowRmssCommand::~ShowRmssCommand() { }

bool ShowRmssCommand::isShowable() const {
  return false; }

void ShowRmssCommand::show() const {
}

int ShowRmssCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " parameter settings of RMSS: " << std::endl;

  int ii=0;
  for (int i=0; i< pythia->pymssm().lenRmss(); ++i) {
    if (showAll || pythia->pymssm().rmss(i)) {
    if (!(ii%valsPerLine)) std::cout << std::endl;
    std::cout << "RMSS(" << setw(3) << i << ") = " 
         << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
         <<  pythia->pymssm().rmss(i) << "  ";
    ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowRmssCommand::description() const {
  string description("Show RMSS parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowItcmCommand
////////////////////////////////////////////////////////////////////////

ShowItcmCommand::ShowItcmCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) 
{
}

ShowItcmCommand::~ShowItcmCommand() 
{ 
}

bool ShowItcmCommand::isShowable() const {
  return false; 
}

void ShowItcmCommand::show() const {
}

int ShowItcmCommand::handle(int argc, char* argv[]) {
 
  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;

  std::cout << std::endl << (showAll?"All":"Non-zero") << " parameter settings of ITCM: " << std::endl;

  int ii=0;
  for (int i=0; i< pythia->pytcsm().lenItcm(); ++i) {
    if (showAll || pythia->pytcsm().itcm(i)) {
      if (!(ii%valsPerLine)) std::cout << std::endl;
      std::cout << "IMSS(" << setw(3) << i << ") = " 
           << setw(5) << pythia->pytcsm().itcm(i) << "  ";
      ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowItcmCommand::description() const {
  string description("Show ITCM parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


////////////////////////////////////////////////////////////////////////
// ShowRtcmCommand
////////////////////////////////////////////////////////////////////////

ShowRtcmCommand::ShowRtcmCommand(const char* const theCommand, 
                               AppModule* theTarget) 
  : APPCommand(theCommand, theTarget) { }

ShowRtcmCommand::~ShowRtcmCommand() { }

bool ShowRtcmCommand::isShowable() const {
  return false; }

void ShowRtcmCommand::show() const {
}

int ShowRtcmCommand::handle(int argc, char* argv[]) {

  Pythia* pythia = Pythia::Instance();
  AbsInterp *theInterp = AbsInterp::theInterpreter();
  AbsInterp::StatusCode result = 
    theInterp->parseArgs("-all",argc,argv);
  bool showAll = theInterp->present("all");
  const int valsPerLine = 4;
  std::cout << std::endl << (showAll?"All":"Non-zero") << " parameters settings of RTCM: " << std::endl;

  int ii=0;
  for (int i=0; i< pythia->pytcsm().lenRtcm(); ++i) {
    if (showAll || pythia->pytcsm().rtcm(i)) {
    if (!(ii%valsPerLine)) std::cout << std::endl;
    std::cout << "RTCM(" << setw(3) << i << ") = " 
         << setw(10) << setprecision(3) << setiosflags(ios::showpoint)
         <<  pythia->pytcsm().rtcm(i) << "  ";
    ++ii;
    }
  }
  std::cout << std::endl << std::endl;
  return AbsInterp::OK;
}

string ShowRtcmCommand::description() const {
  string description("Show RTCM parameters, usage ");
  description += command() + string(" [-all]");
  return description;
}


