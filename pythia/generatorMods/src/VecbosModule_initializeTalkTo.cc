//--------------------------------------------------------------------------
// Description:
//	Belongs to class VecbosModule
//
// Wed Jul 11 16:09:46 CDT 2001, G.Velev  fix a bug with W or Z selection and 
//                                        add vegas read/write flag
//-------------------------------------------------------------------------- 
#include "generatorMods/VecbosModule.hh"

void VecbosModule::_initializeTalkTo() {

   // Init the menus

  _structureFunctionMenu.initialize("StructureFunction",this);
  _structureFunctionMenu.initTitle("Pdf Menu: internal/exatarnal (group and set)");

  _importantMenu.initialize("Important",this);
  _importantMenu.initTitle(" Importance sampling for jet Pt");

  _isubMenu.initialize("Subprocesses",this);
  _isubMenu.initTitle(" 2, 4 or 6 quark subprocess");
  
  _processMenu.initialize("Process",this);
  _processMenu.initTitle("Select the process");

  _primaryCutsMenu.initialize("PrimaryCuts",this);
  _primaryCutsMenu.initTitle("Select Primary Cuts");

  _vegasParamMenu.initialize("VegasParam",this); 
  _vegasParamMenu.initTitle("Vegas Menu, only for an advanced user");

 // Descriptions of the Parameters
  _debug.addDescription("\t\t\tDebug flag\n\
    \t\t\t Increase Vegas print flag from 0 to 1 [0]\n\
    \t\t\t Syntax: debug set <t/f>");

  _beam_type.addDescription("\t\t\tBeam type\n\
     \t\t\t Specify beam type: pp (0) or ppbar (1) [0]\n\
     \t\t\t Syntax: beam set <beam>");
  
  _which_boson.addDescription("\t\t\tBoson type\n\
     \t\t\t Select W (1) or Z (2) Bosons [1]    \n\
     \t\t\tSyntax: wORz set <number>");

  _decay_boson.addDescription("\t\t\tDecay the boson\n\
     \t\t\t  Decay the W or Z boson: (t/f) [t] \n\
     \t\t\t Syntax: decayWorZ  set <t/f>");

  _charge_of_W.addDescription("\t\t\tW charge\n\
      \t\t\t Generate W with charge -1,+1 or 0 for both [0] \n\
      \t\t\t Warning: Choosing to sum over charges\n\
      \t\t\t          in W events will produce\n\
      \t\t\t          leptons of both signs, but no\n\
      \t\t\t          charge specific analysis\n\
      \t\t\t          should be done on this event\n\
      \t\t\t          sample! (See documentation).\n\
      \t\t\t Syntax: chargeOfW set <charge>");

  _howto_decay_Z.addDescription("\t\t\tZDecays\n\
     \t\t\t Z decays via ll (1) or nunu (X)  [1]  \n\
     \t\t\t Syntax: zDecayMode set <decaymode>");

  _njets.addDescription("\t\t\tNoOfJets\n\
     \t\t\t Specify number of jets (min 0-max 4) [3]   \n\
  \t\t\t Syntax: njets set <njets>");

  _structure_function.addDescription("\t\t\t StructureFunction\n\
    \t\t\t Specify the structure function (1-6) [6], \n\
    \t\t\t 1 - CTEQ1_M, \n\
    \t\t\t 2 - CTEQ1MS, \n\
    \t\t\t 3 - KMRSS0 , \n\
    \t\t\t 4 - KMRSD0 , \n\
    \t\t\t 5 - KMRSDM , \n\
    \t\t\t 6 use the PDF library \n\
    \t\t\t Syntax: strFunct set <number>");

  _pdf_group.addDescription("\t\t\tPDFGroup\n\
    \t\t\t If use PDF lib specify PDF group [3]   \n\
   \t\t\t Syntax: pdfGroup set <number>");

  _pdf_set.addDescription("\t\t\tPDFSet\n\
     \t\t\t If use PDF lib specify PDF set [30]   \n\
    \t\t\t Syntax: pdfSet set <number>");

  _run_number.addDescription("\t\t\tRunNumber\n\
     \t\t\t Set the run number [1]. Please  use\n\
     \t\t\t GenInputManager to set the run number correctly.\n\
     \t\t\t Syntax: runNumber set <number>");

  _cm_energy.addDescription("\t\t\tEnergyCM\n\
    \t\t\t Set the CM energy in GeV/c^2 [1960]   \n\
     \t\t\t Syntax: enrgyCM set <number>");

  _qcd_scale.addDescription("\t\t\tQCDScale\n\
 \t\t\t Set QCD scale(1-6) [1] \n\
 \t\t\t For [1] Q = PT-average \n\
 \t\t\t For [2] Q = Total invariant mass \n\
 \t\t\t For [3] Q = Average inv-mass of two jets \n\
 \t\t\t For [4] Q = Mass of vector boson \n\
 \t\t\t For [5] Q = Pt-max in event \n\
 \t\t\t For [6] Q = PT-average / 2 \n\
 \t\t\t For [7] Q = PT-average / 4 (min 2.5 GeV) \n\
 \t\t\t For [8] Q = PT-average * 2 \n\
 \t\t\t For [9] Q = Rt(M(b)**2 + Pt(b)**2) \n\
 \t\t\t For [10] Q = Rt(0.5*(M(b)**2 + Pt(b)**2)) \n\
 \t\t\t For [11] Q = Rt(4*(M(b)**2 + Pt(b)**2)) \n\
   \t\t\t Syntax: qcdScale set <number>");

  _helicity.addDescription("\t\t\tHelicity\n\
    \t\t\t Perform MC over helicities (in Q^2)? [0]   \n\
    \t\t\t Syntax: helicity set <number>");

  _force_bbbar.addDescription("\t\t\tbbBarMode\n\
     \t\t\t Force to generate bbbar final state (0-1) [0]   \n\
     \t\t\t Syntax: bbBar set <number>");

  _important.addDescription("\t\t\tImportance\n\
     \t\t\t Use importance sampling for jet Pt (0-1) [1]  \n\
     \t\t\t Syntax: important set <number>");

  _sampling_mode.addDescription("\t\t\tSamplingMode\n\
     \t\t\t Choose importance  sampling mode, if importance is selected \n\
     \t\t\t 1:  Pt^(-alpha),        alpha should be set to ~ 2.0 \n\
     \t\t\t 2:  e^(-alpha*Pt)/Pt,   alpha should be set to ~ 0.02 \n\
     \t\t\t Syntax: samplingMode set <number>");

  _lepton_type.addDescription("\t\t\tLeptonType\n\
     \t\t\t Specify the lepton type (1-electron,2-muon,3-tau) [1]   \n\
     \t\t\t Syntax: leptonType set <number>");

  _matrix_ele.addDescription("\t\t\tMatrixElement\n\
     \t\t\t Matrix elements: Exact (1), M^2=1 (2), 'M^2=1/Shat^Njets (3)? [1] \n\
     \t\t\t Syntax: matrixElement set <number>");

  _alpha_jet_generation.addDescription("\t\t\tAlphaJets\n\
     \t\t\t Set the alpha for the jet generation [0.019]   \n\
     \t\t\t Syntax: AlphaPt set <value>");

  _subprocesses.addDescription("\t\t\tSubProcesses\n\
    \t\t\t Set subprocesses (100,110,111) [100]  \n\
    \t\t\t Include 2-quark subprocess - 100  \n\
    \t\t\t Include 4-quark subprocess - 110  \n\
    \t\t\t Include 6-quark subprocess - 111  \n\
    \t\t\t Syntax: subprocesses set <number>");

  _make_jets.addDescription("\t\t\tMakeJets\n\
     \t\t\t Force boson to decay to quarks? \n\
     \t\t\t true will force decay products to be light quarks. \n\
     \t\t\t This is done by changing particle types in the output. \n\
     \t\t\t No correction is made to the weights. \n\
     \t\t\t Make Jets  (t/f) [f]  \n\
     \t\t\t Syntax: makeJets set <t/f>");

  _jet_min_pt.addDescription("\t\t\tJetMinPt\n\
     \t\t\t Set the minimum jet Pt in GeV [8.0]   \n\
     \t\t\t Syntax: jetMinPt set <value>");

  _jet_sum_pt.addDescription("\t\t\tJetSumPt\n\
     \t\t\t Set sum of jet Pt in GeV [0.0]   \n\
     \t\t\t Syntax: jetMinPt set <value>");

  _jet_eta_max.addDescription("\t\t\tJetEtaMax\n\
     \t\t\t Set the maximal value of abs(eta) [2.5]   \n\
     \t\t\t Syntax:jetEtaMax set <value> ");

  _jet_eta_separation.addDescription("\t\t\tJetEtaSeparation\n\
     \t\t\t  Set the separation between the jets eta [0.4]  \n\
     \t\t\t Syntax: jetEtaSep set <value>");

  _boson_min_pt.addDescription("\t\t\tBosonMinPt\n\
     \t\t\t Boson minimum Pt [0.0]  \n\
     \t\t\t Syntax: bosonMinPt set <value>");

  _no_of_leptons.addDescription("\t\t\tNumberOfLeptons\n\
     \t\t\t Number of leptonss required to pass cuts [1]  \n\
     \t\t\t Syntax: noOfLeptons set <nleptons>");

  _lep_max_eta.addDescription("\t\t\tLeptonMaxEta\n\
    \t\t\t Lepton maximum abs(eta) [2.5]  \n\
    \t\t\t Syntax: lepMaxEta set <value>");

  _lep_min_pt.addDescription("\t\t\tLeptonMinPt\n\
     \t\t\t Lepton minimum Pt(GeV)  [12.0]   \n\
     \t\t\t Syntax: lepMinPt set <value>");

  _mis_pt_min.addDescription("\t\t\tMissingPtMin\n\
    \t\t\t Missing Pt cut [0.0]  \n\
    \t\t\t Syntax: misPtMin  set <value>");

  _min_weght.addDescription("\t\t\tMinimalWeigth\n\
     \t\t\t Minimum event weight to pass [0.0]  \n\
     \t\t\t Syntax: minWeigth  set <value>");

  _vegas_rw_flag.addDescription("\t\t\tReadWriteFlag\n\
     \t\t\t Flag to read(1)/write(2) vegas grid [1] (0-no action)\n\
     \t\t\t Syntax: rwGrid  set <value>");

  _vegas_inp_grid_file.addDescription("\t\t\tGridFileInputName\n\
     \t\t\t Input VEGAS grid from a file.  \n\
     \t\t\t Use enviroment CDFVEGIN to specify the input file.");
  //     \t\t\t Syntax: gridInputName  set <string>");

  _vegas_out_grid_file.addDescription("\t\t\tGridFileOutputName\n\
    \t\t\t  Output VEGAS grid from a file.  \n\
    \t\t\t Use enviroment CDFVEGOUT to specify the output file.");
  //    \t\t\t Syntax:  gridOutputName  set <string>");

  _vegas_print.addDescription("\t\t\tVegasPrint\n\
     \t\t\t VEGAS output flag: (0=none ,1) [0]   \n\
     \t\t\t Syntax: vegasPrint set <flag> ");

  _vegas_n_inter.addDescription("\t\t\tVegasNInterations\n\
     \t\t\t Number of init. grid iterations [5] \n\
    \t\t\t Syntax: vegasNInter set <value>");

  _vegas_n_calls1.addDescription("\t\t\tVegasNCalls1\n\
     \t\t\t  Number of init. grid calls [max(1000,nevent/5)] \n\
     \t\t\t Syntax: vegasNCalls1 set <value>");

  _vegas_n_calls2.addDescription("\t\t\tTVegasNCalls2\n\
    \t\t\t Number of generation grid calls [NEVENT/2]   \n\
    \t\t\t Syntax: vegasNCalls2 set <value>");

  // Connect all menus and parameters 
  // First menus 

  commands( )->append( &_structureFunctionMenu);
  _structureFunctionMenu.commands( )->append(&_structure_function);
  _structureFunctionMenu.commands( )->append(&_pdf_group);
  _structureFunctionMenu.commands( )->append(&_pdf_set);

  commands( )->append( &_importantMenu);
  _importantMenu.commands( )->append(&_important);
  _importantMenu.commands( )->append(&_sampling_mode);

  commands( )->append( &_isubMenu);
  _isubMenu.commands( )->append(&_subprocesses);

  commands( )->append( &_processMenu);
  _processMenu.commands( )->append( &_boson_min_pt);
  _processMenu.commands( )->append( &_charge_of_W);
  _processMenu.commands( )->append( &_howto_decay_Z);

  commands( )->append( &_primaryCutsMenu);  
  _primaryCutsMenu.commands( )->append(&_lep_max_eta);
  _primaryCutsMenu.commands( )->append(&_lep_min_pt);
  _primaryCutsMenu.commands( )->append (&_jet_min_pt);
  _primaryCutsMenu.commands( )->append (&_jet_eta_max);
  _primaryCutsMenu.commands( )->append (&_jet_eta_separation);
  _primaryCutsMenu.commands( )->append (&_mis_pt_min);
  _primaryCutsMenu.commands( )->append (&_min_weght);

  commands( )->append( &_vegasParamMenu);
  _vegasParamMenu.commands( )->append (&_vegas_print);
  _vegasParamMenu.commands( )->append (&_vegas_n_inter);
  _vegasParamMenu.commands( )->append (&_vegas_n_calls1);
  _vegasParamMenu.commands( )->append (&_vegas_n_calls2); 
  _vegasParamMenu.commands( )->append (&_vegas_inp_grid_file);
  _vegasParamMenu.commands( )->append (&_vegas_out_grid_file);
  _vegasParamMenu.commands( )->append (&_vegas_rw_flag);
  // Parameters 

  commands( )->append(&_debug);
  commands( )->append(&_which_boson); 
  commands( )->append(&_decay_boson); 
  commands( )->append(&_make_jets);
  commands( )->append(&_beam_type); 
  commands( )->append(&_njets);
  commands( )->append(&_run_number); 
  commands( )->append(&_cm_energy);
  commands( )->append(&_qcd_scale); 
  commands( )->append(&_helicity);
  commands( )->append(&_force_bbbar); 
  commands( )->append(&_lepton_type);
  commands( )->append(&_matrix_ele); 
  commands( )->append(&_no_of_leptons);
  commands( )->append(&_alpha_jet_generation); 
  commands( )->append(&_cm_energy);


}
