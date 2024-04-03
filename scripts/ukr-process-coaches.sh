#!/bin/bash

YEAR=$1

ru-get-all-coaches.sh ${YEAR}
ru-fetch-coaches.sh ${YEAR}
ukr-extract-coaches.sh ${YEAR} | tee tmp/${YEAR}/coaches.in
nano tmp/${YEAR}/coaches.in
rz-add-catalog catalogs/zcoaches.dat tmp/${YEAR}/coaches.in
#ru-add-catalog catalogs/ccoaches.dat tmp/${YEAR}/coaches.in
#wf-csort
