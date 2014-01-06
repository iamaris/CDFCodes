#-----------------------------------------------------------------------
#  example TCL file to process 100 events
#-----------------------------------------------------------------------
set     WORK_DIR          $env(PWD)
source  $WORK_DIR/TclUtils/scripts/getenv.tcl
#
creator set               PROD
set     PROCESS_NAME      PROD
set     env(MC_JOB)       3  ; # MCJOB=1 MC with realistic beam offset 4.9.1
                               # MCJOB=2 MC with no beam offset 
                               # MCJOB=3 MC with realistic beam offset 5.5.3
#set     OUTPUT_STNTUPLE   ./testm.stn
set     OUTPUT_STNTUPLE   ${OUTFILE_NAME}
set     DO_EMBEDDING      ""
set     L3_SOURCE         ""; # TL3D
set     env(ALL_JETS)     ""; # 1= do additional jet collections and tags
#
module input DHInput
module talk  DHInput
#
#  change the line below to include the input file you need
#
#  set host root://fcdfdata033.fnal.gov
#  set dir  /cdf/scratch/cdfopr/validation_data/5.3.1pre2/wtaunu
#  include file ${host}/${dir}/wb0228e8.0002tau0
  dropList set SecVtxTracks SecVtxColl
#  include file /data/ncmu04/a/syjun/stn614_dev232/xi031fcc.04b3bmm0
  include file ${INFILE_NAME}
#  include file xi031e9c.0287bmm0
#  maxFiles set 1
#  include dataset $env(SAM_DATASET)
#  cache set SAM
exit

talk StntupleMaker
#-----------------------------------------------------------------------
# define output STNTUPLE file
#-----------------------------------------------------------------------
  maxSize set 1500
  histfile ${OUTFILE_NAME} 
exit

source $WORK_DIR/Stntuple/test/prod/stnmaker-prod-common.tcl

#
# module output DHOutput
# talk DHOutput
#   output create    AA ./stnmaker_prod.root
#   output path      AA  StntuplePath
#   output setstream AA  compress nodfc
#   output keepList  AA  CdfJetColl SecVtxColl
# exit
#                                   process 100 events
#ev begin -nev 1000
ev begin
show
exit

