#!/bin/bash

YEAR=$1
LEAGUE=$(cat ln.dat)
PREV=$((${YEAR}-1))
SEASON=${PREV}-${YEAR}
CTTY=$(pwd | cut -d '/' -f 6)
NR=$2

cp ../a.${YEAR} data/

rm -f tmp/${YEAR}/t*
mkdir -p seasons/${YEAR}

wf-get-teams.sh ${SEASON}
wf-get-schedule.sh ${SEASON} ${NR}
wf-get-reports.sh ${YEAR}
wf-extract-all-names.sh ${YEAR}
