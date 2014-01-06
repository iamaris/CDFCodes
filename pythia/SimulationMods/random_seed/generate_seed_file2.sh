#!/bin/sh

n=0
while [ $n -le 99 ]; do
 seed1=`expr $n \* 1000000  + 300000000`
 seed2=`expr $n \* 1000000  + 400000000`
 echo $seed1 $seed2
 m=`expr $n + 100`
 sed 's/XXXXXXXXXX/'"$seed1"'/' xxxx_random_seed_xxx.dat > temp_$m.dat
 sed 's/YYYYYYYYYY/'"$seed2"'/' temp_$m.dat > setup_random_seed_$m.dat
 rm temp_$m.dat
 n=`expr $n + 1`
done





