//
// created 11/18/2001 Elena Gerchtein CMU
//

#include "generatorMods/GrappaModule.hh"

void GrappaModule::_initializeTalkTo() {

  // all other grappa stuff
  commands() -> append(&_grcecm);
  commands() -> append(&_grcpap);
  commands() -> append(&_hmas);
  commands() -> append(&_hwid);
  commands() -> append(&_tmas);
  commands() -> append(&_twid);
  commands() -> append(&_bmas);
  commands() -> append(&_cmas);
  commands() -> append(&_gptcut);
  commands() -> append(&_getacut);
  commands() -> append(&_grconcut);
  commands() -> append(&_ijetflv);

  _subprocessMenu.initialize("subprocessMenu",this);
  _subprocessMenu.initTitle("subprocessMenu");
  commands() -> append(&_subprocessMenu);

  _subprocessMenu.commands() -> append(&_igsub);
  _subprocessMenu.commands() -> append(&_incall);
  _subprocessMenu.commands() -> append(&_ibswrt);
  _subprocessMenu.commands() -> append(&_inpfl);
  _subprocessMenu.commands() -> append(&_icoup);
  _subprocessMenu.commands() -> append(&_ifact);
  _subprocessMenu.commands() -> append(&_isdep);
  _subprocessMenu.commands() -> append(&_grcq);
  _subprocessMenu.commands() -> append(&_grcfaq);
  _subprocessMenu.commands() -> append(&_grcfile);

  //*  commands() -> append(&_useTAUOLA);

  // Add descriptions for AbsParms

  _grcecm.addDescription(
  "      \t\t\tCM Energy (GeV) (= sqrt(s))");
  _grcpap.addDescription(
  "      \t\t\tCollision particles PP/PAP");
  _gptcut.addDescription(
  "      \t\t\tPt cut applied for all Final State Particles");
  _getacut.addDescription(
  "      \t\t\tEta cut applied for all Final State Particles");
  _grconcut.addDescription(
  "      \t\t\tRcone cut applied for all Final State Particles");
  _hmas.addDescription(
  "      \t\t\tHiggs mass (GeV)");
  _hwid.addDescription(
  "      \t\t\tHiggs width (GeV)");
  _tmas.addDescription(
  "      \t\t\ttop-quark mass (GeV)");
  _twid.addDescription(
  "      \t\t\ttop-quark width (GeV)");
  _bmas.addDescription(
  "      \t\t\tb-quark mass (GeV)");
  _cmas.addDescription(
  "      \t\t\tc-quark mass (GeV)");

  _ijetflv.addDescription(
  "      \t\t\tActivation switch Jet flavor of : on/off = 1/0(d/u/s/c/b/t/g)");

  _igsub.addDescription(
  "      \t\t\tProcess ID\n"
  "       \t\t\t[400] : gg -> h0(bb)+bb(Higgs2,QCD2)\n"
  "       \t\t\t[401] : qq -> h0(bb)+bb(Higgs2,QCD2)\n"
  "       \t\t\t[402] : gg -> Z/A(bb)+bb(ELWK2,QCD2)\n"
  "       \t\t\t[403] : qq -> Z/A(bb)+bb(ELWK2,QCD2)\n"
  "       \t\t\t[404] : qq -> bbbb (Higgs2,ELWK2)   \n"
  "       \t\t\t[405] : gg -> bbbb (QCD4)           \n"
  "       \t\t\t[406] : qq -> bbbb (QCD4)           \n"
  "       \t\t\t[407] : qq -> Z/A(bb)+Z/A(bb)(ELWK4)\n"
  "       \t\t\t[408] : qq -> W(2f) + bb(ELWK1,QCD2)\n"
  "       \t\t\t[409] : qq -> W + b + b (ELWK3)     \n"
  "       \t\t\t[410] : gg -> Z + b + b (ELWK1,QCD2)\n"
  "       \t\t\t[411] : qq -> Z + b + b (ELWK1,QCD2)\n"
  "       \t\t\t[412] : qq -> W(2f) + cc(ELWK1,QCD2)\n"
  "       \t\t\t[413] : qq -> W + c + c (ELWK3)     \n"
  "       \t\t\t[414] : gg -> Z + c + c (ELWK1,QCD2)\n"
  "       \t\t\t[415] : qq -> Z + c + c (ELWK1,QCD2)\n"
  "       \t\t\t[416] : gg -> tautau+bb (ELWK2,QCD2)\n"
  "       \t\t\t[417] : qq -> tautau+bb (ELWK2,QCD2)\n"
  "       \t\t\t[418] : qq -> W + bbbb (ELWK1,QCD4) \n"
  "       \t\t\t[419] : gg ->(tt)-> 6f (ELWK4,QCD2) \n"
  "       \t\t\t[420] : qq ->(tt)-> 6f (ELWK4,QCD2) \n"
  "       \t\t\t[421] : qq -> W -> 2f (ELWK2)       \n"
  "       \t\t\t[422] : qg -> W(2f) + q (ELWK2,QCD1)\n"
  "       \t\t\t[423] : qq -> W(2f) + g (ELWK2,QCD1)\n"
  "       \t\t\t[424] : qg -> W(2f) + qg(ELWK2,QCD2)\n"
  "       \t\t\t[425] : qq -> W(2f) + qq(ELWK2,QCD2)\n"
  "       \t\t\t[426] : qq -> W(2f) + gg(ELWK2,QCD2)\n"
  "       \t\t\t[427] : gg -> W(2f) + qq(ELWK2,QCD2)\n"
  "       \t\t\t[428] : qq -> W(2f) + qq(ELWK2,QCD2)\n"
  "       \t\t\t[429] : qq -> W(2f) + qq(ELWK2,QCD2)\n"
  "       \t\t\t[430] : qq -> W(2f) + qq(ELWK2,QCD2)\n"
  "       \t\t\t[500] : pp(bar) -> W(2f) + 1 jet  (ELWK2,QCD1)\n"
  "       \t\t\t[501] : pp(bar) -> W(2f) + 2 jets (ELWK2,QCD2)\n");

  _incall.addDescription(
  "      \t\t\tiNcall : Number of sampling points per iteration");
  _ibswrt.addDescription(
  "      \t\t\tiBswrt = 0/1 : Write output file/Already exist");
  _inpfl.addDescription(
  "      \t\t\tNumber of flavor from PDF. (Default = 4)");
  _icoup.addDescription(
  "      \t\t\tSet Energy Scale (Coupling Q^2)(renormalization scale)");
  _ifact.addDescription(
  "      \t\t\tChoice of the factorization scale");
  _isdep.addDescription(
  "      \t\t\t= 1 : s^-dependence of the resonance production");
  _grcq.addDescription(
  "      \t\t\tEnergy scale (GeV)(renormalization scale)");
  _grcfaq.addDescription(
  "      \t\t\tFactorization scale (GeV)");
  _grcfile.addDescription(
  "      \t\t\tOutput file name for BASES/SPRING");
//*  _useTAUOLA.addDescription(
//*  "      \t\t\tUse TAUOLA");

  // Pythia submenu

  _pythiaMenu->initialize("PythiaMenu",this);
  _pythiaMenu->initTitle( " Pythia Menu " );
  commands()->append( _pythiaMenu );


  // Initialize the relevant submenu
  _randomNumberMenu.initialize("RandomNumberMenu",this);
  _randomNumberMenu.initTitle("Grappa random number menu");
  commands()->append(&_randomNumberMenu);
 
  // Add the commands to the menu
  _randomNumberMenu.commands()->append(&_randomSeed1);
  _randomNumberMenu.commands()->append(&_randomSeed2);
  ostringstream tmpSstream1;
  ostringstream tmpSstream2;
 
  // Describe them
  tmpSstream1 << "      \t\t\tSeed #1 for the random number generator"
               << "\n\t\t\t(default " << _randomSeed1.value() << ").";
  _randomSeed1.addDescription(tmpSstream1.str());
  tmpSstream2 << "      \t\t\tSeed #2 for the random number generator"
               << "\n\t\t\t(default " << _randomSeed2.value() << ").";  
  _randomSeed2.addDescription(tmpSstream2.str());

}
