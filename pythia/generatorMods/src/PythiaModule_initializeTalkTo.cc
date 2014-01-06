#include "generatorMods/PythiaModule.hh"

void PythiaModule::_initializeTalkTo() {

  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle( " Pythia Menu " );
  commands()->append( _pythiaMenu );
}
