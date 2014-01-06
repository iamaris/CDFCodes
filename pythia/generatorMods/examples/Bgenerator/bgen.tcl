#-------------------------------------------------------------------------------
# set some useful B meson production:
#  - produce only one single b quarks (NDE)
#  - just the b not even bbar         (-gencode=1)
#  - produce only Bs mesons           (-bmeson=3)
#-------------------------------------------------------------------------------
module enable  Bgenerator
module talk    Bgenerator
  Bgenerator
    RAPMIN-b1 set -5.0
    RAPMAX-b1 set +5.0
    PT-MIN-b1 set  0.0
    PT-MIN-b2 set  0.0
    NDE       -gentype=1
    BMESON    -gencode=1 -bmeson=3
  exit
exit
