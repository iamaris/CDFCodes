#####################################################################
# Dave Waters, 19.06.2001                                           #
# =======================                                           #
# Example .tcl file for generating and simulating WGRAD events      #
# using cdfGen                                                      #         
# To use with cdfSim add GeometryManager, SimInitManager and        #
# SimulationControlMod setup                                        #
#                                                                   #
# 02 Oct 2001 lena: updated for cdfGen use                          #
# 25 Jul 2002 dwaters : use standard PythiaMenu                     #
#                                                                   #
#####################################################################

path enable AllPath
creator set NSIM

module enable GenInputManager
mod input GenInputManager
talk GenInputManager
  report set 10
  first_event set 1
  show
exit

mod disable Pythia
mod disable Isajet
mod disable herwig
mod disable Bgenerator
mod disable FAKE_EVENT
mod disable WGRAD
mod disable Wbbgen
mod disable VecbosModule
mod disable VecunwModule
mod disable Vecher

mod disable TauolaModule
mod disable QQModule
mod disable EvtGen

module enable GenOutputManager

mod enable WGRAD
mod talk WGRAD
  debug set f
# usePythia = f : WGRAD final state only - lepton, neutrino, (photon).           
# usePythia = t : WGRAD+PYTHIA - including parton showers and underlying event.
  usePythia set t
  cardsMenu
#   1: p pbar collider 2: pp collider :
    BeamType set 1
#   1: W+ production, 2: W- production :
    W_Charge set 1
#   1: constant, 2: s-dependent W width :
    widthTreatment set 2
#   1: electron, 2: muon :
    DecayLepton set 1
#   1: initial, 2: final, 4: all :
    QED_RadType set 2
#   W mass (GeV) :
    W_Mass set 80.4
#   W width (GeV) :
    W_Width set 2.1
#   center of mass energy (GeV) :
    CME set 1960.0
#   0:LO only; 1:NLO-LO ; 2:NLO :
    Order set 2
#   0:mu=MW, 1: mu=.5*MW, 2: mu=2*MW. :
    Scale set 0
#   final state: 1: full, 2: a la Berends et al. :
    fsrType set 1
#   final state: without coll. cut (0), with (1) :
    fsrCollinearCut set 0
#   delta_s (photon energy cut) :
    PhotonEnergyCut set 0.01
#   delta_c (collinear cut) :
    PhotonCollinearCut set 0.001
#   0: no s.,no r., 1: with s,no r., 2: with s.,with r. :
    SmearingRecombination set 0
#   ptmin and ymax for lepton :
    Lepton_ptmin set 5.0
    Lepton_ymax set 5.0
#   ptmin and ymax for neutrino :
    Neutrino_ptmin set 5.0
    Neutrino_ymax set 100.0
#   ptmin and yman for photon :
    Photon_ptmin set 0.0
    Photon_ymax set 100.0
#   low, high transverse mass range :
    mT_low set 0.0
    mT_high set 2000.0
#   pdflib group :
    pdflibGroup set 3
#   pdflib choice :
    pdflibSet set 54
#   itmax and ncall for grid and precision calculation :
    itmaxGrid set 1
    ncallGrid set 5000
    itmaxPrecision set 2
    ncallPrecision set 10000
#   Use "RandomNumberMenu" below.
#   random number seed :
#   randomSeed set 8910
  exit
  PythiaMenu
    commonMenu
#     Fragmentation and decay : 0 - off, 1 - on (default) :
#     -----------------------------------------------------
#     set_mstp -index=111 -value=1
#
#     QCD and QED showers. QED showers must be turned off
#     if WGRAD is run in NLO mode, to avoid double counting.
#     0 - no showers, 1 - QCD only, 2 - QCD+QED (default) :
#     -------------------------------------------------------
      set_mstj -index=41  -value=1
#
#     Master switch for initial state showering :
#     0 - no showering, 1 - QCD only, 2 - QCD+QED (default) :
#     -------------------------------------------------------
      set_mstp -index=61  -value=1
#
#     Master switch for final state showering :
#     0 - off, 1 - on (default) :
#     ---------------------------
      set_mstp -index=71  -value=1
#
#     Random number seed :
#     --------------------
#     set_mrpy -index=1   -value=280273
#     Multiple interactions : 0 - off, 1 - on (default) :
#     ---------------------------------------------------
      set_mstp -index=81  -value=0
#
#     Intrinsic kT : 0 - none, 1 - Gaussian (default), 2 - exponential :
#     ------------------------------------------------------------------
#     set_mstp -index=91  -value=1
#
#     kT Gaussian width (GeV) :
#     -------------------------
#     set_parp -index=91  -value=4.0
#
#     kT exponential width (GeV) :
#     ----------------------------
#     set_parp -index=92  -value=1.63
#
#     Upper bound on intrinsic kT (GeV) :
#     -----------------------------------
#     set_parp -index=93  -value=20.0
    exit
  exit
  RandomNumberMenu
    RandomSeed1 set 1234
    RandomSeed2 set 5678
  exit
exit

mod output FileOutput
talk FileOutput
  output create main_stream cdfSim.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank 
exit

module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set wgrad.hist
  createHistoFile set  f
exit

show

path list

action on "Name Action"

begin -nev 10
show timer

exit




