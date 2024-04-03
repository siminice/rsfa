#!/bin/bash

YEAR=$1

echo "---------------- H ------------------"
cat db/lineups-${1}.db | cut -d ',' -f 32 | sort | uniq -c
echo "---------------- A ------------------"
cat db/lineups-${1}.db | cut -d ',' -f 55 | sort | uniq -c
echo "-------------------------------------"
