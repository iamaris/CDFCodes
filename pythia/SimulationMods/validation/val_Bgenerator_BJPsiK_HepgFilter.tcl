##########################################################################
module enable Bgenerator
module talk   Bgenerator
  Bgenerator
    EPS-b set 0.006
    BMASS     set 4.75 
    CMASS     set 1.5
    RAPMIN-b1 set -1.0
    RAPMAX-b1 set +1.0
    PT-MIN-b1 set  6.0
    PT-MIN-b2 set  0.0
    NDE     -gentype=1
    BMESON  -gencode=1 -bmeson=1
    show
  exit
exit

mod enable QQModule
mod talk QQModule
  show
exit

module enable HepRootManager
talk HepRootManager
  histfile set HepHist.dat
  createHistoFile set t
exit

module clone   HepgFilter MuonP

module enable  HepgFilter
module talk    HepgFilter
  HepgFilter
    Reject   set    1
    CodePDG  set   -13
    AbsPDG   set    0
    PtMin    set  1.5
    Ancestor set  443
    EtaMin set -1.1
    EtaMax set  1.1
  exit
exit

module enable  HepgFilter-MuonP
module talk    HepgFilter-MuonP
  HepgFilter-MuonP
    Reject   set    1
    CodePDG  set    13
    AbsPDG   set    0
    PtMin    set  1.5
    Ancestor set  443
    EtaMin set -1.1
    EtaMax set  1.1
  exit
exit
##########################################################################
