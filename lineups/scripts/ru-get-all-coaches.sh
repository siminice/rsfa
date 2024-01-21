#!/bin/bash

YEAR=$1

rm -rf tmp/${YEAR}/co.links

for f in tmp/${YEAR}/r/*.dat
do
  ru-get-coaches.sh $f >> tmp/${YEAR}/co.links
done

sort tmp/${YEAR}/co.links | uniq > tmp/${YEAR}/unique.co.links

