#!/bin/bash
for ((y=1956; y<=2012; y++)) 
do 
 w=$((y+1))
 grep $y- euro.tab > euro-$w.tab 
done
