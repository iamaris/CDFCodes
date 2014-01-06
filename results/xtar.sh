#! /bin/sh 

n=$1
m=$2

cd /home/aris/analysis/results

while [ $n -le $m ]; do

 #tar -xf mcdata_$n-897619.tar
 tar -xf results_$n-900203.tar

 n=`expr $n + 1`

done

echo " " ; echo " ... finishing the total $njobs ..." ;  echo " "
