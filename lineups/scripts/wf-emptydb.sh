#!/bin/bash

YEAR=$1

cd data
emptydb a.${YEAR}
cd ..
mv data/lineups-${YEAR}.db db/

nl=$(wc -l db/lineups-${YEAR}.db  | cut -d ' ' -f 1)

rm db/events-${YEAR}.db
for ((i=0; i<${nl}; i++))
do
  cat ../../l30 >> db/events-${YEAR}.db
done
