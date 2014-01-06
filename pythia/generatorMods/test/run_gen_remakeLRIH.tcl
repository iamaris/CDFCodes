# test setup for cdfGen

#path enable AllPath
path disable AllPath

module disable HepgFilter GenPrimVertModule
module enable HepRootManager

# input module for generators
talk GenInputManager
  verbose set t
  run_number set 151435
#  run_type set REALISTIC_MC
  run_type set MC
  RewriteLRIH set true
  report set 1
  show
exit

module enable FileInput
module input FileInput
talk FileInput
  verbose set t
#  input file /home/aarond/data/grappa_bbbb_pyth_qcd_nocut.gen
  input file /home/aarond/data/grappa_bbbb_pyth_elwk_nocut.gen
  report set 1
  show
exit

module enable GenInputManager

path create myPath GenInputManager ManagerSequence HepRootManager RandomGenManager HardScatGenSequence DecayPackageSequence GenOutputManager GenPrimVert GenTrigSequence
path enable myPath
path list

talk FileOutput
  output create main_stream grappa_bbbb_pyth_elwk_nocut_151435.gen
  output path main_stream myPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank
  output dropList main_stream LRIH_StorableBank:Original
  main_stream
    writeSingleBranch set t
  exit
exit

begin -nev 100
show timer

exit






