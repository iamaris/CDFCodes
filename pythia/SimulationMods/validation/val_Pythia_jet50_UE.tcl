##########################################################################
mod enable Pythia
mod talk Pythia
   PythiaMenu
      msel set 1
      cmEnergy set 1960.
      listFirst set 1
      listLast set 2
      commonMenu
#
# Set MSEL=1 (above) [qcd group decides to use msel=1]
# kinematics Ptmin (50 GeV)
# 
         set_ckin -index=3 -value=50.0
         show_parp
         show_ckin
      exit
   exit
exit
#-----------------------------------------------------------------------
# Pythia tunings
#-----------------------------------------------------------------------
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_pdf_CTEQ5L.tcl
source $env(CDFSOFT2_DIR)/SimulationMods/validation/val_Pythia_underlying_event_A.tcl
##########################################################################
