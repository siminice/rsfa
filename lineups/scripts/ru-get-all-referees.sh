#!/bin/bash

YEAR=$1

rm -rf tmp/${YEAR}/ref.links

for f in tmp/${YEAR}/r/*.dat
do
  ru-get-referees.sh $f >> tmp/${YEAR}/ref.links
done

sort tmp/${YEAR}/ref.links | uniq > tmp/${YEAR}/unique.ref.links

