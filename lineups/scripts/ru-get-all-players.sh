#!/bin/bash

YEAR=$1

rm -rf tmp/${YEAR}/pl.links

for f in tmp/${YEAR}/r/*.dat
do
  ru-get-lineup.sh $f >> tmp/${YEAR}/pl.links
done

sort tmp/${YEAR}/pl.links | uniq > tmp/${YEAR}/unique.pl.links

