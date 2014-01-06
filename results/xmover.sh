#! /bin/sh 

n=$1
m=$2

cd /home/aris/analysis/results

while [ $n -le $m ]; do

 #tar -xf results_$n-892071.tar
 mv results_$n-894886.tar_FILES/TDarkPhoton_Ntuple_$n.root .
 rm -r results_$n-894886.tar_FILES

 n=`expr $n + 1`

done

echo " " ; echo " ... finishing the total $njobs ..." ;  echo " "
