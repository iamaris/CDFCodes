path enable AllPath
creator set NSIM


mod input FAKE_EVENT
talk FAKE_EVENT
  use PT
  use THETA
  use PHI
# PT flat in range (10.0, 10.0)
  generate PT      0.  0.  10.0  10.0 0 2
# PHI flat in range (7.5, 7.5)
  generate PHI     0.  0.  7.5  7.5   0 2
# THETA flat in range (90.0, 90.0)
  generate THETA   0.  0.  30.0 80.0  0 2
# Generate nue
  generate CDFCODE 201
  generate NPARTICLES 2
#  show
exit


#
# I'm not sure you really have to say anything to the GeometryManager;
# this was taken from SimulationControl
#
talk GeometryManager
# Use the defaults but substitute the detailed for the simple Si detector
  DetectorMenu
     enableSimpleSvx set f
     enableSvx set t
  exit
exit

#
# The muon detector is turned off by default, so that the calorimeter
# output is easier to study.
#
talk SimInitManager
  DetectorMenu
     declareSvx set t 
     declareCot set t
     declareMuon set f
     declareCalor set t
     declareCPR set f
   exit
exit

talk IntegrationControlMod
  DebugMenu
    DebugLevel set 0
  exit
  show add
exit

begin -nev 3
show timer

exit
