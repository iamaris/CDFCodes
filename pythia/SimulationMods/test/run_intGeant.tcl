# 
# This script is to run with cdfIG executable.
#
# Options: set detectors true or false in 
#   InteractiveGeant's DetectorMenu 
#
path enable AllPath
path list

talk GeometryManager
  DetectorMenu
    enableAll set t
  exit
exit

talk InteractiveGeant
  DetectorMenu
    declareCot  set t
    declareSvx  set t 
    declareTof  set t
    declareMuon set t
  exit
exit

begin 

exit

