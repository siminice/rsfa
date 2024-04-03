#!/bin/bash

SEASON=$1
LEAGUE=$(cat ln.dat)

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

CTTY=$(pwd | cut -d '/' -f 6)
NR=$2
ANNOTATION=$3

cp ../a.${YEAR} data/

rm -f tmp/${YEAR}/t*
mkdir -p tmp/${YEAR}/r
mkdir -p seasons/${YEAR}

wf-get-teams.sh ${SEASON} ${ANNOTATION}
wf-get-schedule.sh ${SEASON} ${NR} ${ANNOTATION}
wf-get-reports.sh ${YEAR}
wf-extract-all-names.sh ${YEAR}

wf-extract-referees.sh ${YEAR}
wf-get-stadiums.sh ${YEAR}

wf-batch-add-catalog.sh ${YEAR}
wf-psort

wf-extract-all-rosters.sh ${YEAR}
wf-assemble-catalog.sh ${YEAR}
if [ -z "${ANNOTATION}" ]; then
  wf-emptydb.sh ${YEAR}
fi

wfx-process-reports.sh ${YEAR}
wfx-process-coaches.sh ${YEAR}
wfx-extract-all-reports.sh ${YEAR} ${ANNOTATION}

new-catalog ${YEAR}
if [ -z "${ANNOTATION}" ]; then
#  correct-dates a.${YEAR}
  x=0
fi
cp data/a.${YEAR} ../
check-rosters.sh ${YEAR}

wf-lcopy-div-files ${CTTY} ${YEAR}
