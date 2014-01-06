##########################################################################
mod enable Pythia
mod talk Pythia
   PythiaMenu
        msel set 0
        cmEnergy set 1960.
        listFirst set 1
        listLast set 2
        commonMenu
#
# qq processes
#
# ----------------------
              set_msub -index=11 -value=1
              set_msub -index=12 -value=1
              set_msub -index=13 -value=1
              set_msub -index=28 -value=1
              set_msub -index=53 -value=1
              set_msub -index=68 -value=1
              set_msub -index=96 -value=1
# -----------------------
#
# kinematics
# Ptmin (18 GeV)
#
              set_ckin -index=3 -value=18.
#
# mother scheme for fragmenting partines
#
#              set_mstu -index=16 -value=2
#
#
# PDFs - CTEQ5L(1000*NGROUP + NSET)
#
#              set_mstp -index=51 -value=4046
#              set_mstp -index=52 -value=2

              set_mstp -index=51 -value=7

# String fragmentation
#              set_mstj -index=1  -value=1
# Parameteres framgentation
#              set_mstj -index=11 -value=3
#              set_mstj -index=12 -value=3
#              set_mstj -index=24 -value=2
# Parton Showers
#              set_mstj -index=41 -value=1
#              set_mstj -index=42 -value=2
#              set_mstj -index=47 -value=2
#              set_mstj -index=101 -value=5
# Initial State Rad
              set_parp -index=67 -value=4.0

      show_parp
     exit
    exit
  exit
# ==================
#
# Rick Field tune "A" for improved underlying event, Oct 23, 2002
# good for 6.x versions of Pythia, but only for CTEQ5L pdf
# This is a preliminary recomendation, the study is continuing
#
talk Pythia
  PythiaMenu
   commonMenu
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
   exit
  exit
exit
##########################################################################
