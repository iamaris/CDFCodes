##########################################################################
mod enable Pythia
# Pythia talk-to fragment
talk Pythia
   PythiaMenu
     msel set 5
     #TP added May 24th 2002
     cmEnergy set 1960

# SDA add this for gebnerator level eta cuts and 
# multiple interaction parameters.
# changed to CTEQ5L 10/09/02 SDA
#
     commonMenu
      set_ckin -index=3 -value=0.
      set_ckin -index=4 -value=-1.
# pseudotrapidity range soft wired 
      set_ckin -index=13 -value=-1.4
      set_ckin -index=14 -value=1.4
# PDFs - CTEQ Set 5L (LO)
#      set_mstp -index=51 -value=4046
#      set_mstp -index=52 -value=2
# Set ISR max scale factor parameter
      set_parp -index=67 -value=1.0
# Multiple Interaction parameters
# turn m.i. ON
      set_mstp -index=81 -value=1
# assume gaussian hadronic matter distr. turn off at
      set_mstp -index=82 -value=3
# turn off parameter
      set_parp -index=82 -value=1.6
      set_ckin -index=14 -value=1.4
# PDFs - CTEQ Set 5L (LO)
#      set_mstp -index=51 -value=4046
#      set_mstp -index=52 -value=2
# Set ISR max scale factor parameter
      set_parp -index=67 -value=1.0
# Multiple Interaction parameters
# turn m.i. ON
      set_mstp -index=81 -value=1
# assume gaussian hadronic matter distr. turn off at
      set_mstp -index=82 -value=3
# turn off parameter
      set_parp -index=82 -value=1.6
# probability of gg interaction with colour connection
      set_parp -index=85 -value=1.0
# total probability of gg interactions
      set_parp -index=86 -value=1.0
# reference energy scale for m.i.
      set_parp -index=89 -value=1960.
#set top mass 
      set_pmas -masscode=6 -mass=175. 
#shows
      show_ckin
      show_mstp
      show_parp
      show_mstu
      show_paru
      show_mstj
      show_parj
     exit
  exit
exit
##########################################################################
