#
# Use TclUtils package to change settings through environment variables  
#
if [ file exists $env(SRT_PRIVATE_CONTEXT)/TclUtils ] { 
  source $env(SRT_PRIVATE_CONTEXT)/TclUtils/scripts/getenv.tcl
} else { 
  source $env(SRT_PUBLIC_CONTEXT)/TclUtils/scripts/getenv.tcl
}

# LesGenType: Grappa/Alpgen/CompHep/MadGraph/MadGraph3/User
set LesGenType [ getenv LesGenType "Grappa" ]
set LesDatFile [ getenv LesDatFile \
		        "$env(CDFSOFT2_DIR)/LesHouches/example/grappa_w3j.data" ]
# LesPSModel: PYTHIA/HERWIG
set LesPSModel       [ getenv LesPSModel        "PYTHIA" ]
set firstEventToRead [ getenv firstEventToRead       "1" ]

#
# Set resonance particle (W/Z) in the event history 
# if lepton pair appears sequentialy in event.
set putResonance [ getenv putResonance "f" ]

# Set energy scale for the parton shower based on kt-clustering algorithm.
# In this case, the PS-scale is overwritten by a new scale recalculated by
# kt-cluster. Otherwise, the original scale in the event file is used.
set scaleOfKtClus [ getenv scaleOfKtClus "f" ]

5Bset useTAUOLA     [ getenv useTAUOLA     "f" ]

# Write option to STDHEP - if you read a STDHEP with "useStdhep set 2"
set useStdhep      [ getenv useStdhep               "0" ]
set StdhepFileName [ getenv StdhepFileName "stdhep.out" ]
set NumOfStdEvent  [ getenv NumOfStdEvent          "10" ]

mod enable LesHouchesModule
mod talk   LesHouchesModule

  LesGenType       set $LesGenType
  LesDatFile       set $LesDatFile
  LesPSModel       set $LesPSModel
  firstEventToRead set $firstEventToRead
  putResonance     set $putResonance
  scaleOfKtClus    set $scaleOfKtClus
  useTAUOLA        set $useTAUOLA
  useStdhep        set $useStdhep
  StdhepFileName   set $StdhepFileName
  NumOfStdEvent    set $NumOfStdEvent

#########################################################
# PYTHIA section
# ==============
# Most of all parameters are available in PYTHIA menu.
#########################################################
  PythiaMenu
    commonMenu

# choose PDF set:  (ex. CTEQ5L)
      set_mstp -index=41  -value=1
      set_mstp -index=42  -value=1
      set_mstp -index=51  -value=7
      set_mstp -index=52  -value=1
      set_mstp -index=58  -value=5

# first order running of alpha_s
#!! These must be always set here with appropriate value.!!
      set_mstu -index=101 -value=1
      set_mstu -index=111 -value=1
      set_mstu -index=112 -value=5
      set_paru -index=112 -value=0.146

# ISR/FSR/Hadronization/multiple interaction ON
      set_mstp -index=61  -value=1
      set_mstp -index=71  -value=1
      set_mstp -index=111 -value=1

#########################################################
# Rick's parameters:
#        Set A: Double Gaussian with more initial-state radiation
#
#  Initial-State Radiation Max Scale Factor
#  Old ISR setting with more initial-state radiation
      set_parp -index=67  -value=4.0
#  Underlying Event Parameters
      set_mstp -index=81  -value=1
# Double Gaussian
      set_mstp -index=82  -value=4
# MPI Cut-Off
      set_parp -index=82  -value=2.0
#  Warm-Core: 50% of matter in radius 0.4
      set_parp -index=83  -value=0.5
      set_parp -index=84  -value=0.4
# Almost Nearest Neighbor
      set_parp -index=85  -value=0.9
      set_parp -index=86  -value=0.95
# Energy Dependence with E0=1.8 TeV (do not vary this!)
      set_parp -index=89  -value=1800.0
      set_parp -index=90  -value=0.25
#########################################################

    exit
  exit

#########################################################
# HERWIG section
# ==============
# Most of all parameters are available in HERWIG menu.
#########################################################
  HerwigMenu

#!!! Do NOT change "Process" number.
    Process set 1705

    Randseed1 set 9999
    Randseed2 set 123457

#!!! Need to set the "Beam" and "Struc_Function" menu 
#    until upgrade to HERWIG 6.5.

    Beam
      Beam1 set "P"
      Beam2 set "PBAR"
      Pbeam1 set 980.
      Pbeam2 set 980.
    exit

    Struc_Function
      Autpdf set CTEQ
      Modpdf set 46
    exit

    Prints
      maxpr set 2
      prvtx set 0
    exit

    Hadronization
       Clpow set 1.26
    exit

  exit

exit
