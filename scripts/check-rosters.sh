#!/bin/bash

YEAR=$1

echo "----------------------------------------"
cat db/lineups-${YEAR}.db | cut -d ',' -f 10-20 > hrosters
cat db/lineups-${YEAR}.db | cut -d ',' -f 10-20 | grep -n "      :0" | cut -d ':' -f 1 > hlines
while read l
do
  line=$(cat hrosters | head -n ${l} | tail -n 1)
  echo "L${l}: ${line}"
done < hlines
echo "----------------------------------------"
cat db/lineups-${YEAR}.db | cut -d ',' -f 33-43 > arosters
cat db/lineups-${YEAR}.db | cut -d ',' -f 33-43 | grep -n "      :0" | cut -d ':' -f 1 > alines
while read l
do
  line=$(cat arosters | head -n ${l} | tail -n 1)
  echo "L${l}: ${line}"
done < hlines
echo "----------------------------------------"
