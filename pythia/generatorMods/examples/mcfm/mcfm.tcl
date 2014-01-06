#####################################################################
# Dave Waters, 30.07.2001                                           #
# =======================                                           #
# Example .tcl file for generating and simulating MCFM events       #
# using cdfGen                                                      #
#                                                                   #
# NOTE : MCFM is still reading most of it's input parameters from   #
#        local options files such as options.DAT. The full set of   #
#        flat files needed by the program exist in mcfm/Bin. These  #
#        must exist in the directory from which the AC++ job is     #
#        run. Future enhancements should include the possibility to #
#        specify options through talk-to parameters.                #
#                                                                   #
#####################################################################

path enable AllPath
creator set NSIM

# Following two lines sometimes useful for basic testing :
useRCP set false
# mod disable HepgFilter

set fRandomSeedGRNDM1  124695
set fRandomSeedGRNDM2  2140
set fRandomSeedPYTHIA1 458028
set fRandomSeedPYTHIA2 1473
set fRandomSeedHERWIG1 384177
set fRandomSeedHERWIG2 2217
set fRandomSeedTAUOLA1 2807
set fRandomSeedTAUOLA2 7379
talk RandomGenManager
  RandomNumberMenu
    RandomSeedGRNDM1 set $fRandomSeedGRNDM1
    RandomSeedGRNDM2 set $fRandomSeedGRNDM2
    RandomSeedPYTHIA1 set $fRandomSeedPYTHIA1
    RandomSeedPYTHIA2 set $fRandomSeedPYTHIA2
    RandomSeedHERWIG1 set $fRandomSeedHERWIG1
    RandomSeedHERWIG2 set $fRandomSeedHERWIG2
  exit
exit

mod input GenInputManager

mod disable Pythia
mod disable Isajet
mod disable herwig
mod disable HeavyQuarkGen
mod disable Bgenerator
mod disable FAKE_EVENT
mod disable WGRAD
mod disable Wbbgen
mod disable VecbosModule
mod disable VecunwModule
mod disable Vecher
mod disable herwig

mod enable MCFM
mod talk MCFM
  debug set f

  randomSeed set 123456

  PythiaMenu
    commonMenu
#     Fragmentation and decay : 0 - off, 1 - on (default) :
#     -----------------------------------------------------
#     set_mstp -index=111 -value=1
#
#     QCD and QED showers.
#     0 - no showers, 1 - QCD only, 2 - QCD+QED (default) :
#     -------------------------------------------------------
#     set_mstj -index=41  -value=2
#
#     Master switch for initial state showering :
#     0 - no showering, 1 - QCD only, 2 - QCD+QED (default) :
#     -------------------------------------------------------
#     set_mstp -index=61  -value=2
#
#     Master switch for final state showering :
#     0 - off, 1 - on (default) :
#     ---------------------------
#     set_mstp -index=71  -value=1
#
#     Random number seed :
#     --------------------
#     set_mrpy -index=1   -value=280273
#
#     Multiple interactions : 0 - off, 1 - on (default) :
#     ---------------------------------------------------
#     set_mstp -index=81  -value=0
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

#     Set PDF's used by Pythia. Should these always 
#     be the same as specified for use by MCFM ?
#     ------------------------------------------
#     set_mstp -index=52 -value=2
#     set_mstp -index=51 -value=3067
    exit
    listLast set 10
  exit
exit

# mod disable SimInitManager
# mod disable SimulationControlMod
# mod disable SimValModule

###################################################################
# TAUOLA                                                          #
# ======                                                          #
# Enable TauolaModule if MCFM is being run in a mode such         #
# that taus can be substituted for electrons. Currently this      #
# is done for WW production in mcfm/src/User/fill_hepeup.f        #
# Pythia will decay the taus. However Tauola will track down all  #
# the tau decay products, and re-decay the taus according to its  #
# own tables. In particular, it should discover that the tau is   #
# the daughter of a W, since this is the parentage that is        #
# assigned by hand in the fill_hepeup routine. Tauola will then   #
# setup the decaying tau with the correct helicity.               #
# For more details, see CDF-5719.                                 #
###################################################################
mod enable TauolaModule
mod talk TauolaModule
  Display_max_taudecays set 4
  Display_tauola_output set 3
  RandomNumberMenu
    RandomSeed1 set $fRandomSeedTAUOLA1
    RandomSeed2 set $fRandomSeedTAUOLA1
  exit
exit
# second enable the one module you want, and talk to it:
mod enable QQModule
talk QQModule
# note qq default is to decay only Bu Bd Bs (as set in CDF interface to qq)
# Herwig default (set in HerwigInterface.F) is to NOT decay hadrons that
#  contain 1 or more b or c quarks - let qq do ALL such decays. So for
#  most running the next 4 lines are necessary.
  Decay_Bc set           1
  Decay_Upsilon set      1
  Decay_B_Baryons set    1
  Decay_prompt_charm set 1
  show
exit


talk FileOutput
  output create main_stream mcfm_ww_hepg_10k_4.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank 
exit

# action on "Name Action"

begin -nev 100
show timer

exit
