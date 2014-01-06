# GR@PPA (GRace At PP/Anti-proton)
#==================================
# For parameter sets, see CDF note 5932.
# (any questions to tsuno@fnal.gov, Sep.17.2002)
#
# This example generates "W + 2 jets" processes including b-quark flavor.
#

path enable AllPath
show path

# input module for generators
module input GenInputManager

mod enable Grappa
mod talk Grappa
  CMEnergy set 1960
  Beam set PAP

# allowed jet flavor: d/u/s/c/b/t/g/other
  JetFlv set 1 1 1 1 1 0 1 0

# this is "minimum-cut requirements".
  gPtcut   set    5.0
  gEtacut  set    5.0
  gRconcut set    0.1

# choose subprocess number. (type "help" shows all processes.)
  subprocessMenu
    iGsub   set 501

#!! once users make grid-optimized integration file,
#!! users need not to make it again if "iBswrt=1".
    iBswrt  set   0

#!! The file name is set in "GrcFile".
#   GrcFile set "bases_w2jets"

    show
  exit

  PythiaMenu

#!! should be set again.
    cmEnergy set 1960
    msel set 0

    commonMenu

# PDF set: CTEQ5L
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
      set_mstp -index=81  -value=1
      set_mstp -index=111 -value=1

# underlying event model (I recommend...)
      set_mstp -index=82  -value=3

# forced W decay to electron(positron), W -> e nu
      set_mdme -channelIndex=190 -decayType=0
      set_mdme -channelIndex=191 -decayType=0
      set_mdme -channelIndex=194 -decayType=0
      set_mdme -channelIndex=195 -decayType=0
      set_mdme -channelIndex=198 -decayType=0
      set_mdme -channelIndex=199 -decayType=0
      set_mdme -channelIndex=206 -decayType=1
      set_mdme -channelIndex=207 -decayType=0
      set_mdme -channelIndex=208 -decayType=0

      show_msub
    exit
  exit
exit

talk FileOutput
  output create main_stream grcpyth_w2jet.gen
  output path main_stream AllPath
  output enable main_stream
  output keepList main_stream EVCL_StorableBank HEPG_StorableBank
  output list
exit

begin -nev 100
show timer

exit
