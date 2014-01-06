//------------------------------------------------------------------------------
// Description:
// ------------
//      Build vecbos generator
//
//   author Elena Gerchtein, CMU
//   06/26/2001
//   History:
//   Thu Jul 12 14:27:30 CDT 2001, G.Velev add the F77 module, need for ybos
//------------------------------------------------------------------------------

#include "Framework/APPUserBuild.hh"

// Collaborating header files
#include "generatorMods/VecbosModule.hh"
#include "generatorMods/FakeEv.hh"
#include "Framework/APPInputModule.hh"
#include "FrameMods/addCDFrequiredModules.hh"
#include "FrameMods/DH/DHInputModule.hh"
#include "FrameMods/DH/DHOutputModule.hh"
//#include "FrameMods/F77InterfaceModule.hh"
#include "FrameMods/YbosDiskFileInputModule.hh"
#include "generatorMods/UnwtModule.hh"
#include "generatorMods/VecherModule.hh"
// Local macros, typedefs etc...
static const char rcsid[] = "";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
AppUserBuild::AppUserBuild(AppFramework* Framework) : AppBuild(Framework) {


  addCDFrequiredModules(this);

  // Additional input modules

  APPInputModule*   iMod;
  iMod = new VecbosModule();       add(iMod); iMod->setEnabled(true);
  //  iMod = new FakeEv();          add(iMod); iMod->setEnabled(false);

  iMod = new YbosDiskFileInputModule("Ybos","Default ybos Interface");
  add(iMod);
  iMod->setEnabled(false); 

  APPOutputModule*  oMod;
  oMod = new DHOutputModule();     add(oMod); oMod->setEnabled(true);
 
  AppModule* module;
  //  module = new F77InterfaceModule("F77_MODULE","Default Fortran Interface");
  //  add(module);
  //  module->setEnabled(module);


  module = new UnwtModule("Unwt", "UnWeighted Module for VECBOS");
  add(module);
  module->setEnabled(false);

  module = new VecherModule("VecHer","Vecbos to Herwig interface with hadronization");
  add(module);
  module->setEnabled(false);

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
AppUserBuild::~AppUserBuild() {
  std::cout << endl << "Application ended: " << rcsid << std::endl;
  return;
}


