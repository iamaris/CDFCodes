module enable Pythia
module talk Pythia
  PythiaMenu
    cmEnergy set 1960
# generate W
    msel set 12
    inListLevel set 0
    evListLevel set 1
    commonMenu

# Setup the decay of the W->mu nu : decayType=0(1): Channel off(on)
# don't touch decayType = -1!!!!
# W decay table turning off all allowed decays:
            set_mdme -channelIndex=190 -decayType=0
            set_mdme -channelIndex=191 -decayType=0
            set_mdme -channelIndex=192 -decayType=0
            set_mdme -channelIndex=194 -decayType=0
            set_mdme -channelIndex=195 -decayType=0
            set_mdme -channelIndex=196 -decayType=0
            set_mdme -channelIndex=198 -decayType=0
            set_mdme -channelIndex=199 -decayType=0
            set_mdme -channelIndex=200 -decayType=0
            set_mdme -channelIndex=206 -decayType=0
            set_mdme -channelIndex=207 -decayType=0
            set_mdme -channelIndex=208 -decayType=0

# W turning on muon channel:
            set_mdme -channelIndex=207 -decayType=1

    exit
  exit
exit
#-----------------------------------------------------------------------
# Pythia tunings
#-----------------------------------------------------------------------
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_WZPt_tune.tcl
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_pdf_CTEQ5L.tcl
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_underlying_event_A.tcl
