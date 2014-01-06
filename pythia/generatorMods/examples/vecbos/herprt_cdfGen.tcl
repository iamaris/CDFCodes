#     Herprt example for W+jets: top group version 
#     Created: Jul 17 2001, by G.Velev 
#     Before start:
###############################################################################
path enable AllPath
path list

module disable VecbosModule

module input DHInput
talk         DHInput
  setInput noorder
  input file ./vecbos_unwt.dat
  report set 1
  exit

module output FileOutput
module enable FileOutput
talk FileOutput
  output create AA ./vecbos_herprt.dat
  output path   AA AllPath
  output keepList AA LRIH_StorableBank EVCL_StorableBank HEPG_StorableBank
  output list
exit

module enable Vecher
module talk Vecher
   leptonType set 2
   Process set 1100
   Masses
     top set 175
   exit
   Struc_Function
      Autpdf set MRS
      Modpdf set 30
   exit
   Prints
    maxpr set 1
    prvtx set 0
   exit
   Read_write
     readTable  set 0
     writeTable set 0
   exit
exit
ev begin -nev 10000
show timer
exit












