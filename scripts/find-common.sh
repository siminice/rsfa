#!/bin/bash

cat europlayers.dat | cut -d ',' -f 1 > mnem-euro.dat
cat players.dat | cut -d ',' -f 1 > mnem-rom.dat
comm -1 -2 mnem-rom.dat mnem-euro.dat > mnem-common.dat
while read line
do
  grep $line players.dat
  grep $line europlayers.dat
  echo "-----------"
done < mnem-common.dat
wc -l mnem-common.dat
