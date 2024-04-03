#!/bin/bash

LEAGUE=$(cat data/xmln)
CTTY=$(pwd | cut -d '/' -f 6)
SEASON=$1
YEAR=$((${SEASON}+1))
OPT=$2

xm-get-teams.sh ${SEASON}
xm-fetch-rosters.sh ${SEASON}
xm-process-players.sh ${YEAR} ${OPT}
xm-assemble-catalog.sh ${YEAR}

xm-fetch-schedule.sh ${SEASON}
xm-fetch-reports.sh ${YEAR}
xm-process-reports.sh ${YEAR}

xm-process-coaches.sh ${YEAR}
xm-process-referees.sh ${YEAR}

xm-check-stadiums.sh ${YEAR}

wf-emptydb.sh ${YEAR}
xm-add-all-lineups.sh ${YEAR}
redo-catalog.sh ${YEAR}
check-rosters.sh ${YEAR}

if [ -z "${ANNOTATION}" ]; then correct-dates a.${YEAR}; fi
cp data/a.${YEAR} ../

wf-lcopy-div-files ${CTTY} ${YEAR}
