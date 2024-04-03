#!/bin/bash

SEASON=$1
NR=$2
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

#Download season.html

ru-process-reports.sh ${YEAR}
ru-process-players.sh ${YEAR}
#ru-add-catalog catalogs/cplayers.dat tmp/${YEAR}/players.in
ru-process-coaches.sh ${YEAR}
#ru-add-catalog catalogs/ccoaches.dat tmp/${YEAR}/coaches.in
ru-process-referees.sh ${YEAR}
#ru-add-catalog catalogs/creferees.dat tmp/${YEAR}/referees.in

ru-check-stadiums.sh ${YEAR}
wf-emptydb.sh ${YEAR}
ru-add-all-lineups.sh ${YEAR}
new-catalog ${YEAR}

wf-lcopy-div-files ${CTTY} ${YEAR}
