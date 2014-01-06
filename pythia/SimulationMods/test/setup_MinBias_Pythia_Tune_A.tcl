############################################################################
# Tcl file to overlay additional min bias events using Pythia Tune A.
# Note, PythiaMinBiasModule CANNOT be run together with PythiaModule but
# works with all other generators for the hardscattering.
############################################################################
mod enable PythiaMinBias
mod talk PythiaMinBias
   MEAN set $ADD_MINBIAS_MEAN 
   MODE set $ADD_MINBIAS_MODE
############################################################################
# Rick Field tune "A" for improved underlying event, Oct 23, 2002
# good for 6.x versions of Pythia, but only for CTEQ5L pdf
# This is a preliminary recomendation, the study is continuing
#
  PythiaMenu
   commonMenu
#   Use PDFLIB :
     set_mstp -index=52 -value=2
#   CTEQ 5L (group 4 set 46):
     set_mstp -index=51 -value=4046
#  Initial-State Radiation Max Scale Factor
#  Old ISR setting with more initial-state radiation
     set_parp -index=67 -value=4.0
#
#  Underlying Event Parameters
     set_mstp -index=81 -value=1
# Double Gaussian
     set_mstp -index=82 -value=4
# MPI Cut-Off
     set_parp -index=82 -value=2.0
#  Warm-Core: 50% of matter in radius 0.4
     set_parp -index=83 -value=0.5
     set_parp -index=84 -value=0.4
# Almost Nearest Neighbor
     set_parp -index=85 -value=0.9
     set_parp -index=86 -value=0.95
# Energy Dependence with E0=1.8 TeV
# (do not vary this! - it is not beam E)
     set_parp -index=89 -value=1800.0
     set_parp -index=90 -value=0.25
     show_mstj
     show_mstp
     show_parp
   exit
  exit
  show
exit

############################################################################
