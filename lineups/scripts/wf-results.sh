#!/bin/bash

LEAGUE=$1   # yug=prva-liga
SEASON=$2   # 1989-1990
FILE=$3     # a.1990
FOLDER=$4   # lineups/tmp/1990
NR=$5       # 34

wf-fetch-schedule.sh ${LEAGUE} ${SEASON} ${NR}
wf-extract-results ${FILE} ${FOLDER} ${NR}
