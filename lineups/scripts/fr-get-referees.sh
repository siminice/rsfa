#!/bin/bash

BASEURL='https://www.footballdatabase.eu/'
YEAR=$1

grep Referee tmp/${YEAR}/r/*.dat | cut -d ' ' -f 2- | grep -v "^ " | sort | uniq > tmp/${YEAR}/referees.links

while read link
do
  pid=$(echo ${link} | sed -e 's/ /-/g')
  echo "${pid},${link},${link},0000-00-00, , , "
done < tmp/${YEAR}/referees.links
