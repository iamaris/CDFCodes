##########################################################################
mod enable herwig
# Herwig talk-to fragment
talk herwig
 help
  
 #===Process ID Numbers ===============
 #
 # W->enu        : 1451
 # W->munu       : 1452
 # Z/gamma->ee   : 1351 
 # Z/gamma->mumu : 1353
 #
 #======================================  

 Process set 1452
 Masses
  #top set 175
  show
 exit
 
 # PY added on Mar-07-02:
 Hards
   MassMin set 40
 exit

 Beam
  Beam1 set P
  Beam2 set PBAR
  Pbeam1 set 980.
  Pbeam2 set 980.
  show
 exit

# Following 6 lines lead to a print (internal to herwig) of 1 event,
 Prints
   maxpr set 1
   prvtx set 0
   show
 exit

# set pi0 decay
 Decays
   help
   show
 exit
# Kinematic cuts
 Hards
 Ptmin set 0.
 Ptmax set 900.
 exit

# Struc_Function  MRS G  (group 3 set 41)
 Struc_Function
   Modpdf set 41
 exit
# Hadronization: set baryon number properly
 Hadronization
   Clpow set 1.26
 exit
 Maxer set 1000000
exit
##########################################################################