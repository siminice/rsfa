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

rm -f tmp/${YEAR}/r*.html

wf-get-schedule.sh ${SEASON} ${NR} ${ANNOTATION}
wf-get-reports.sh ${YEAR}
wf-extract-all-reports.sh ${YEAR} ${ANNOTATION}

new-catalog ${YEAR}

wf-lcopy-div-files ${CTTY} ${YEAR}
