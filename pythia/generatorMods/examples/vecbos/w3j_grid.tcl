#############################################################################
# to be used with Vecbos executable
# Thu Jul 12 16:15:12 CDT 2001 G.Velev W+3jets example
# check generatorMods/examples/vecbos/start script for setup
#     Before start:
#     setenv   VECBOS_INITDATA $CDFSOFT2_DIR/vecbos_i/initdata
#     setenv   CDFVEGIN      your_file_with_saved_grid
# or  setenv   CDFVEGOUT     your_file_where_to_save_grid
##############################################################################

path enable AllPath

module input GenInputManager
talk GenInputManager
  run_number set 1
exit

module enable  VecbosModule

module talk VecbosModule
  njets set 3
  Process
    chargeOfW set 0
  exit
# jets
  PrimaryCuts
   jetMinPt set 8.
   jetEtaMax set 2.5
   jetEtaSep set 0.4
# lepton
   lepMinPt set 12.0
   lepMaxEta set 2.5
  exit

  noOfLeptons set 1

# beam check this
# 0 = pp
# 1 = ppbar
   beam set 1
# structure function
  StructureFunction
     strFunct set 6
     pdfGroup set 3
     pdfSet set 30
  exit
# 
  Important
     important set 1
     samplingMode set 2
  exit
  AlphaPt set 0.03
  RandomNumberMenu
    RandomSeed1 set 3589
    RandomSeed2 set 4537
  exit

  leptonType set 1

# write the grid file
  VegasParam
    rwGrid  set 2
  exit
exit


ev begin -nev 10000
show timer
exit









