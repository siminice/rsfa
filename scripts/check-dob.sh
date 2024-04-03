#!/bin/bash

#echo "Usage: check-dob.sh <year> <tid>"

get-mnem.sh $1 $2 > temp

while read mnem
do
 grep ${mnem} catalogs/players.dat
done < temp
