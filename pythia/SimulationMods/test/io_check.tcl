# to be executed with ac++ dump
path enable AllPath
set output_dir  $env(VAL_DATA_DIR)

mod input FileInput
mod enable FileInput
mod talk FileInput
  input file $output_dir/isajet_qq_today.root
exit

mod enable EventDump
talk EventDump
#  bankList set  LRIH EVCL HEPG TL2D OBSP OBSV COTD CMPD CMUD CSPD CMXD IMUE SIXD MSVX ISLD MISL MCOT TOFD COTQ 
#  classList set MuonXHitColl MuonTHitColl MuonXZHitColl
exit

talk GeometryManager
# Use the defaults but substitute the detailed for the simple Si detector
  DetectorMenu
     enableSimpleSvx set f
     enableSvx set t
  exit
exit

begin -nev 10

exit
