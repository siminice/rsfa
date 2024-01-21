#!/bin/bash

YEAR=$1
DIR=$(pwd | cut -d '/' -f 6)

wf-more-coaches.sh ${YEAR}
wf-extract-all-reports.sh ${YEAR}
wf-lcopy-div-files ${DIR} ${YEAR}
