module enable Pythia
module talk Pythia
                PythiaMenu
                   cmEnergy set 1960
# generate gamma*/Z
                   msel set 11
         inListLevel set 0
         evListLevel set 1
                        
          commonMenu

# Setup the decay of the gamma*/Z->mumu : 
# decayType=0(1): Channel off(on)
# don't touch decayType = -1!!!!

# Z decay table turning off all allowed decays:
            set_mdme -channelIndex=174 -decayType=0
            set_mdme -channelIndex=175 -decayType=0
            set_mdme -channelIndex=176 -decayType=0
            set_mdme -channelIndex=177 -decayType=0
            set_mdme -channelIndex=178 -decayType=0
            set_mdme -channelIndex=179 -decayType=0
            set_mdme -channelIndex=182 -decayType=0
            set_mdme -channelIndex=183 -decayType=0
            set_mdme -channelIndex=184 -decayType=0
            set_mdme -channelIndex=185 -decayType=0
            set_mdme -channelIndex=186 -decayType=0
            set_mdme -channelIndex=187 -decayType=0

# Z/gamma turning on muon channel:
            set_mdme -channelIndex=184 -decayType=1

# Set Minimum Invariant Mass
         set_ckin -index=1 -value=30
    exit
  exit
exit
#-----------------------------------------------------------------------
# Pythia tunings
#-----------------------------------------------------------------------
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_WZPt_tune.tcl
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_pdf_CTEQ5L.tcl
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_underlying_event_A.tcl
