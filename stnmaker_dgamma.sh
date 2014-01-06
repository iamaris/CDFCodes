#!/bin/bash 
#
#
# Usage: ./stnmaker_dgamma.sh $ (1 njobs) for CafGui
#   
# tar -hzcf dgamma_pythia.tar Production SimulationMods Stntuple bin bin1 shlib \
#           ./GNUmakefile ./.base_release ./
#        submit Caf jobs
#        CafCom dgamma_pythia.tar syjun@ncmu08.fnal.gov:/home/syjun/spool/dgamma\$.tar \
#               syjun@fnal.gov long 
#               1 100 ./stnmaker_dgamma.sh \$
#arguments
export JOBS=$1

# cdf2 environments to use pythia6.4 and shlib
source ~cdfsoft/cdf2.shrc
setup cdfsoft2 development
setup pythia v6_409 -f Linux+2.4-2.3.2 -q GCC3_4_3

# environment variables
WORKDIR="${SRT_LOCAL}"
EXEC1="${SRT_LOCAL}/bin.dev/Linux2.6-GCC_4_1"

#run cdfSim with pythia v6_409
export OUTPUT_FILE="cdfSim_dgamma_${JOBS}.root"
export RANDOM_SEED_INPUT="${SRT_LOCAL}/SimulationMods/random_seed/setup_random_seed_${JOBS}.dat"

$EXEC1/cdfSim $WORKDIR/run_cdfSim_dgamma.tcl >& cdfSim_dgamma_${JOBS}.log

setup cdfsoft2 6.1.6.m
#which ProductionExe
#run official Production with 6.1.6.m
EXEC1="${SRT_LOCAL}/bin/Linux2.6-GCC_4_1"
#cp ./.base_release1 ./.base_release 

export PROD_on_MC=t
$EXEC/ProductionExe ./Production/ProductionExe.tcl -i cdfSim_dgamma_${JOBS}.root -o prod_dgamma_${JOBS}.root >& prod_dgamma_${JOBS}.log 

#run stntuple
export MC_JOB=1
export INFILE_NAME="prod_dgamma_${JOBS}.root"
export OUTFILE_NAME="pythia_dgamma_${JOBS}.stn.01"

LOG_FILE="pythia_dijet20_${JOBS}.log"
$EXEC/stnmaker_prod.exe ${WORKDIR}/run_stnmaker_prod.tcl >& ./${LOG_FILE} 

#do some clean-up here before ftp'ing
rm -f  ./GNUmakefile
rm -rf ./Production
rm -rf ./SimulationMods
rm -rf ./Stntuple
rm -rf ./bin
rm -rf ./doc
rm -rf ./include
rm -rf ./lib
rm -rf ./man
rm -rf ./results
rm -rf ./tmp
rm -rf ./ups
rm -f ./prod_dgamma_${JOBS}.root
rm -f ./cdfSim_dgamma_${JOBS}.root
rm -f ./*.tcl
rm -f ./*.sh
rm -f ./core.*

status=$?
exit $status
