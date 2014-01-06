
module enable HepRootManager

talk GeometryManager
  DetectorMenu
     enableTof set true
     enablePassive set true
     enableBeampipeC set true
     show
  exit
  SiliconGeometryMenu
    BuildPassive set true 
  exit
exit

module enable SimInitManager
talk SimInitManager
  DetectorMenu
     declareSvx set t
     declareCot set f
     declareMuon set f
     declareCalor set f
     declareTof  set f
     declarePassive set f
  exit
exit

talk G3MineSweeper
  verbose set t
  GridMenu
    gridRMin set 0.
    gridRMax set 0.
    gridLength set 0.
  exit
  ProbeMenu
    probeRadius set 30.
    probeCosThetaStep set 0.1 
  exit
 show
exit

path create g3xtpat \
	ManagerSequence \
	SimInitManager \
	G3MineSweeper

path disable AllPath
path enable g3xtpat

begin -nev 1
show timer
quit
