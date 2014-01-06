#!/bin/sh

n=0
while [ $n -le 99 ]; do
 seed1=`expr $n \* 1000000  + 100000000`
 seed2=`expr $n \* 1000000  + 200000000`
 echo $seed1 $seed2
 sed 's/XXXXXXXXXX/'"$seed1"'/' xxxx_random_seed_xxx.dat > temp_$n.dat
 sed 's/YYYYYYYYYY/'"$seed2"'/' temp_$n.dat > setup_random_seed_$n.dat
 rm temp_$n.dat
 n=`expr $n + 1`
done





