#-----------------------------------------------------------------------
# Specify alignment table
#set ALIGN_TABLE default
#
# The reason why the alignment is set by hand:
#    Geant creates the aligned geometry before the run number is known 
#    and the used set is available - an order of execution problem.
#    "default" uses the used set to determine the right aligment for a
#    particular run number which is not known when Geant is initialized.
#    For CdfSim, the only possibility is a specific table by hand. 
#-----------------------------------------------------------------------
if { $USE_FRONTIER_DB } {
  set dbname "frontier"
} else {
  set dbname "ofotl_prd_read"
}

if { $RUN_NUMBER < 149717 } {
    set ALIGN_TABLE  "$dbname     100050 2 GOOD"
} elseif { $RUN_NUMBER < 158636 } {
    set ALIGN_TABLE  "$dbname     150050 1 GOOD"
} elseif { $RUN_NUMBER < 174656 } {
    set ALIGN_TABLE  "$dbname     160050 1 GOOD"
} elseif { $RUN_NUMBER < 179056 } {
    set ALIGN_TABLE  "$dbname     170050 1 GOOD"
} elseif { $RUN_NUMBER < 190000 } {
    set ALIGN_TABLE  "$dbname     180050 1 GOOD"
} elseif { $RUN_NUMBER < 212430 } {
    set ALIGN_TABLE  "$dbname     190050 1 GOOD"
} elseif { $RUN_NUMBER < 226087 } {
    set ALIGN_TABLE  "$dbname     220050 1 GOOD"
} elseif { $RUN_NUMBER < 237796 } {
    set ALIGN_TABLE  "$dbname     225050 1 GOOD"
} elseif { $RUN_NUMBER < 246232 } {
    set ALIGN_TABLE  "$dbname     240050 1 GOOD"
} elseif { $RUN_NUMBER < 291025 } {
    set ALIGN_TABLE  "$dbname     250050 1 GOOD"
} elseif { $RUN_NUMBER < 301303 } {
    set ALIGN_TABLE  "$dbname     290050 1 GOOD"
} else {
    ## Need to add whatever table will be used after shutdown
    ## The code will freeze if no alignment table is given!!!
    set ALIGN_TABLE  ""
    echo "*********************************************************"
    echo "Severe Error: missing silicon alignment table, check     "       
    echo " http://www-cdf.fnal.gov/internal/upgrades/align/db.html "
    echo "*********************************************************"
}
