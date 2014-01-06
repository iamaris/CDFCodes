##########################################################################
module enable Bgenerator
module talk   Bgenerator
  Bgenerator
    EPS-b set 0.006
    BMASS     set 4.75 
    CMASS     set 1.5
    RAPMIN-b1 set -1.0
    RAPMAX-b1 set +1.0
    PT-MIN-b1 set  2.0
    PT-MIN-b2 set  0.0
    MNR       -gentype=1
    BMESON  -gencode=3 -mixu=0.389 -mixd=0.389 -mixs=0.107 -mixl=0.116 -mixc=0.007
    show
  exit
exit

#set env(QQ_USER_FILE) "decayuser.CDF"
mod enable QQModule
mod talk QQModule
  Decay_B_Baryons set 1
  Decay_Bc set 1
  show
exit

##########################################################################
