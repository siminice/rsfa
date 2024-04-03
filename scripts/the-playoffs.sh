#!/bin/bash

LEAGUE=$(cat ln.dat)
SEASON=$1
shift
ROUNDS=$*

CTTY=$(pwd | cut -d '/' -f 6)
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi


mkdir tmp/${YEAR}/r/rs
mkdir tmp/${YEAR}/r/po
mv tmp/${YEAR}/r/*.html* tmp/${YEAR}/r/rs/

cp db/lineups-${YEAR}.db db/rs-lineups-${YEAR}.db
cp db/events-${YEAR}.db db/rs-events-${YEAR}.db

cp ../a.${YEAR} data/
wf-emptydb.sh ${YEAR}

wf-get-playoff-schedule.sh ${SEASON} playoffs ${ROUNDS}
wf-get-reports.sh ${YEAR}
wf-extract-all-reports.sh ${YEAR}
wf-lcopy-div-files ${CTTY} ${YEAR}


cp tmp/${YEAR}/r/*.html* tmp/${YEAR}/r/po/
cp db/lineups-${YEAR}.db /var/www/html/${CTTY}/po-lineups-${YEAR}.db
cp db/events-${YEAR}.db /var/www/html/${CTTY}/po-events-${YEAR}.db
