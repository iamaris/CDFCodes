#!/bin/bash 
#
#
# Usage: ./stnmaker_dgamma.sh $ (1 njobs) for CafGui
#   
# tar -hzcf dgamma_ntuple.tar ArisAna shlib ./.base_release 
#
#        submit Caf jobs
#        CafCom dgamma_pythia.tar syjun@ncmu08.fnal.gov:/home/syjun/spool/dgamma\$.tar \
#               syjun@fnal.gov long 
#               1 100 ./stntuple_to_aristuple.sh \$ arguments
export JOBS=$1

# cdf2 environments to use pythia6.4 and shlib
source ~cdfsoft/cdf2.shrc
setup cdfsoft2 6.1.6.m

# environment variables
WORKDIR="${SRT_LOCAL}"
EXEC="${SRT_LOCAL}/bin/Linux2.6-GCC_4_1"

#run stntuple
export MC_JOB=1
export INFILE_NAME="prod_dgamma_${JOBS}.root"
export OUTFILE_NAME="pythia_dgamma_${JOBS}.stn.01"

export INFILE_NAME=yyyy_${JOBS}

LOG_FILE="pythia_dijet20_${JOBS}.log"
sed
awk

root.exe -b -q -l ./ArisAna/scripts/xxxx.C



#do some clean-up here before ftp'ing
rm -f  ./GNUmakefile
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
