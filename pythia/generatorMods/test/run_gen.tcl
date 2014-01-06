# test setup for cdfGen

path enable AllPath

# input module for generators
module input GenInputManager
module talk GenInputManager
# default run number
  run_number set 151435
exit 

# enable generator ( no generators are enabled bu default )
mod enable FAKE_EVENT
mod talk FAKE_EVENT
  verbose set false
  use PT
  use THETA
  use PHI
  generate PT  10. 0. 10. 10. 0. 1.
  generate PHI  180. 0. 360. 270. 0. 2
  generate THETA 90. 10. 45. 135. 1.
# Generate mu-
  generate CDFCODE 13
  generate NPARTICLES 1
  PtHistMenu
    usePtHist set false
    lowPt set 4.
    highPt set 20.
    nbinPt set 4
    valPt set 20. 15. 4. 1.
 exit
#  show
exit

exec rm -f run_gen_test.root
module enable HepRootManager
mod talk HepRootManager
  verbose set true
  histfile set run_gen_test.root
  createHistoFile set  f
exit


talk FileOutput
  output create main_stream cdfGen.root
  output path main_stream AllPath
  output keepList main_stream \
                    LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank 
exit

begin -nev 100
show timer

exit






