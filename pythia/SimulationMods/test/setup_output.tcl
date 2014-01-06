#######################################################################
module output FileOutput
talk FileOutput
  output create main_stream $OUTPUT_FILE 
  output path   main_stream $outputPath
  source $SIM_TCL_DIR/defaultSimOutputKeepList.tcl
  defaultSimOutputKeepList main_stream
  # S.Behari May-2010> For B group (used by svtsim)
  if { $DFC_BOOK == "cdfpbot" } {
    output keepList main_stream -add SIXD_StorableBank:"CORRECTED"
  }
  if {$COT_Matching} then {
    output keepList  main_stream -add COTM_StorableObject
  }
  # Assumes if use wants silicon matching they also want cot matching
  if {$SI_Matching} then {
    output keepList  main_stream -add MISL_StorableBank
    output keepList  main_stream -add MSVX_StorableBank
    output keepList  main_stream -add COTM_StorableObject
  }
  # Store UserInfoColl("MCInfo")
  if {$STORE_USERINFO} then {
    output keepList  main_stream -add UserInfoColl
   }
exit

#- S.Behari 05/30/2010> Check for $env(HEP_HIST_FILE) and set histfile
#-                      appropriately for HepgFilter module to act sane.
#-                      User is expected to set this env in the filter
#-                      script, if using HepgFilter.
set HEP_HIST_FILE [ getenv HEP_HIST_FILE "None" ]
module enable HepRootManager
module talk   HepRootManager
  verbose           set t
  if { $HEP_HIST_FILE == "None" } {
    histfile        set run_sim_test.hist
    createHistoFile set f
  } else {
    histfile        set $HEP_HIST_FILE
    createHistoFile set t
  }
exit

#######################################################################
