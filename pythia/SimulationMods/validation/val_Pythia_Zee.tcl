##########################################################################
mod enable Pythia
# Pythia talk-to fragment
# Z production (on shell only) decaying to e+e-
mod talk Pythia
 PythiaMenu
   msel set 0
   commonMenu 
     for {set i 1} {$i<100} {incr i} {
       set_msub -index=$i -value=0
     }
     # Produce Zs only
     set_msub -index=15 -value=1
     set_msub -index=19 -value=0
     set_msub -index=30 -value=0
     show_msub
     # Only on shell Z produced (no gamma*)
     set_mstp -index=43 -value=2
     show_mstp
     # limit Z decays to Z->ee only
     for {set i 174} {$i<190} {incr i} {
       set_mdme -channelIndex=$i -decayType=0
     }
     set_mdme -channelIndex=182 -decayType=1
#     show_mdme
   exit
  exit
exit
##########################################################################
