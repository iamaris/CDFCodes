#include "generatorMods/TauolaModule.hh"
#include <sstream>
#include <iostream> 

void TauolaModule::_initializeTauolaTalkTo() {
   commands()->append( &_nevprt);
   _nevprt.addDescription("Max No. of tau decays displayed..Dump of HEPEVT before and after cleanup");
   commands()->append( &_nprt);
   _nprt.addDescription("Tauola Printing On..Level 0-2");
   commands()->append( &_jak1);
   _jak1.addDescription("decay mode for tau+");
   commands()->append( &_jak2);
   _jak1.addDescription("decay mode for tau-");
   commands()->append( &_xk0dec);
   _xk0dec.addDescription("radiative tau decay Soft Photon cut parameter");
   commands()->append( &_itdkrc);
   _itdkrc.addDescription("QED corrections switch(0/1)");
   commands()->append( &_tauchirality);
   _tauchirality.addDescription("Choice of TAU Chirality..0 if fixed by mother (0/+1/-1)");

  // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Random number menu");
  commands()->append(&_randomNumberMenu);

  // Add the commands to the menu
  _randomNumberMenu.commands()->append(&_randomSeed1);
  _randomNumberMenu.commands()->append(&_randomSeed2);

  commands()->append( &_disableLeptonicDecays);

  std::ostringstream tmpSstream1;
  std::ostringstream tmpSstream2;

  // Describe them
  tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed1.value() << ").";
  _randomSeed1.addDescription(tmpSstream1.str());
  tmpSstream2 << "      \t\t\tSeed #2 for the random number generator"
              << "\n\t\t\t(default " << _randomSeed2.value() << ").";  
  _randomSeed2.addDescription(tmpSstream2.str());

}
