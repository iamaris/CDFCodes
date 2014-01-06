#! /bin/sh 

n=$1
m=$2

while [ $n -le $m ]; do

 echo "      chain->Add(\"./TDarkPhoton_Ntuple_$n.root/M4Finder/M4Finder\");"
 n=`expr $n + 1`

done

echo " " ; echo " ... finishing the total $njobs ..." ;  echo " "
