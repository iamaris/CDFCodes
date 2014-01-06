#! /bin/sh 

if [ $# -ne 2 ]; then
  echo " "
  echo "Usage: run_cdfSim.sh begin_job_number end_job_number" 
  echo " "
  exit 1
fi

#arguments
n=$1
m=$2

njobs=`expr $m - $n + 1`

#number of events for each job
export NEVENTS=1000

if [ $njobs -lt 0 ]; then
  echo " "
  echo "end_job_number should be equal or greater than begin_job_number"
  echo " ... bailing out !!! ... "
  echo " "
  exit 1
else
  echo " "
  echo " ... processing the total $njobs from $1 to $2 for each $NEVENTS events ..."
  echo " "
fi

# cdf2 environments to use pythia6.4
source ~cdfsoft/cdf2.shrc
setup cdfsoft2 development
setup pythia v6_409 -f Linux+2.4-2.3.2 -q GCC3_4_3

# environment variables
WORKDIR="${SRT_LOCAL}"
EXEC="${SRT_LOCAL}/bin/Linux2.6-GCC_4_1"

while [ $n -le $m ]; do

 export OUTPUT_FILE="cdfSim_dgamma_$n.root"
 export RANDOM_SEED_INPUT="${SRT_LOCAL}/SimulationMods/random_seed/setup_random_seed_$n.dat"
 LOGFILE="cdfSim_dgamma_$n.log"
 echo "starting cdfSim with $OUTPUT_FILE and $RANDOM_SEED_INPUT"

 [ -f ${OUTPUT_FILE} ] && rm ${OUTPUT_FILE}
 [ -f $LOGFILE ] && rm $LOGFILE 

 $EXEC/cdfSim $WORKDIR/run_cdfSim_dgamma.tcl >& $LOGFILE
 echo "finishing cdfSim for the job number: $n"

 n=`expr $n + 1`

done

echo " " ; echo " ... finishing the total $njobs for each $NEVENTS events ..." ;  echo " "
