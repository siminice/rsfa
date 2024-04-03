#!/bin/bash

YEAR=$1
LEAGUE=$(cat ln.dat)
PREV=$((${YEAR}-1))
SEASON=${PREV}-${YEAR}
CTTY=$(pwd | cut -d '/' -f 6)
NR=$2

cp ../a.${YEAR} data/

wf-get-teams.sh ${SEASON}
wf-get-schedule.sh ${SEASON} ${NR}
wf-get-reports.sh ${YEAR}
wf-extract-all-names.sh ${YEAR}

wf-extract-coaches.sh ${YEAR}
wf-extract-referees.sh ${YEAR}
wf-get-stadiums.sh ${YEAR}

wf-batch-add-catalog.sh ${YEAR}
wf-psort

wf-extract-all-rosters.sh ${YEAR}
wf-assemble-catalog.sh ${YEAR}

wf-emptydb.sh ${YEAR}
wf-extract-all-reports.sh ${YEAR}

new-catalog ${YEAR}
correct-dates a.${YEAR}
cp data/a.${YEAR} ../

wf-lcopy-div-files ${CTTY} ${YEAR}
