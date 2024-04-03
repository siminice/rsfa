#!/bin/bash

LEAGUE=$(cat data/xmln)
CTTY=$(pwd | cut -d '/' -f 6)
SEASON=$1
YEAR=$((${SEASON}+1))
OPT=$2

oxm-get-teams.sh ${SEASON}
oxm-fetch-rosters.sh ${SEASON}
oxm-process-players.sh ${YEAR} ${OPT}
oxm-assemble-catalog.sh ${YEAR}

oxm-fetch-schedule.sh ${SEASON}
oxm-fetch-reports.sh ${YEAR}
oxm-process-reports.sh ${YEAR}

oxm-process-coaches.sh ${YEAR}
oxm-process-referees.sh ${YEAR}

oxm-check-stadiums.sh ${YEAR}

wf-emptydb.sh ${YEAR}
oxm-add-all-lineups.sh ${YEAR}
redo-catalog.sh ${YEAR}
check-rosters.sh ${YEAR}

if [ -z "${ANNOTATION}" ]; then correct-dates a.${YEAR}; fi
cp data/a.${YEAR} ../

wf-lcopy-div-files ${CTTY} ${YEAR}
