##########################################################################
mod enable Pythia
# Pythia talk-to fragment
# Talk-to fragment for J/psi production decaying to mu,mu
mod talk Pythia
 PythiaMenu
   msel set 0
   commonMenu 
     for {set i 1} {$i<100} {incr i} {
       set_msub -index=$i -value=0
     }
     # gg -> psi g
     set_msub -index=86 -value=1
     set_msub -index=87 -value=0
     set_msub -index=88 -value=0
     set_msub -index=89 -value=0
     show_msub
     # psi->mumu
     set_mdme -channelIndex=858 -decayType=0
     set_mdme -channelIndex=859 -decayType=1
     set_mdme -channelIndex=860 -decayType=0
#     show_mdme
   exit
  exit
exit
##########################################################################
