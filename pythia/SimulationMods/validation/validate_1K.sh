#!/bin/sh 
#
# usage:
#
simdir="$CDFSOFT2_DIR/SimulationMods"
valdir="$simdir/validation"
processlist=`cat ${valdir}/validation.list |awk '{print $1}'`

#default setup
def_tcl="$simdir/test/run_cdfSim.tcl"
def_nev="cdfSim_NUM_EVENTS 10"
def_out="cdfSim_test.root"
def_gen="\(\$TCL_LOCATION\)\/setup_fakeev.tcl"

#define substitution
val_nev="cdfSim_NUM_EVENTS 1000"
prod_tcl="$CDFSOFT2_DIR/Production/ProductionExe.tcl"

cat /proc/cpuinfo
for  process in $processlist ; do
  val_tcl="run_cdfSim_${process}.tcl"
  val_log="run_cdfSim_${process}.log"
  val_out="cdfSim_${process}.root"
  val_gen="\$env\(CDFSOFT2_DIR\)\/SimulationMods\/validation\/val_${process}.tcl"

  sed 's/'"${def_nev}"'/'"${val_nev}"'/' $def_tcl | \
  sed 's/'"${def_out}"'/'"${val_out}"'/' | \
  sed 's/'"${def_gen}"'/'"${val_gen}"'/' > $val_tcl

  echo " ... processing cdfSim $process"
  cdfSim $val_tcl > $val_log 2>&1

  prod_out="prod_${process}.root"
  prod_log="prod_${process}.log"
  echo " ... processing ProductionExe $process"
  ProductionExe ${prod_tcl} -i ${val_out} -o ${prod_out} > ${prod_log} 2>&1
done

exit 