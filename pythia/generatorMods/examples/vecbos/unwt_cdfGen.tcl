#     Un-weighting example for W+jets: top group version 
#     Created: Jul 13 2001, by G.Velev 
#     Before start:
#       did you put the  correct max weight from Vecbos generation??? 
###############################################################################
path enable AllPath
path list

module input DHInput
talk         DHInput
  setInput noorder
  input file ./vecbos_top.dat
  report set 100
  exit

mod output FileOutput
talk FileOutput
  output create AA vecbos_unwt.dat
  output path   AA AllPath
  output keepList AA -add StorableBank
  output list
exit

module enable VecunwModule
module talk VecunwModule
   Seed set 65307836
   Weight set 0.0021098135
exit
ev begin -nev 10000
show timer
exit
