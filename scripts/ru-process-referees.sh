#!/bin/bash

YEAR=$1

ru-get-all-referees.sh ${YEAR}
ru-fetch-referees.sh ${YEAR}
ru-extract-referees.sh ${YEAR} | tee tmp/${YEAR}/referees.in
nano tmp/${YEAR}/referees.in
rz-add-catalog catalogs/zreferees.dat tmp/${YEAR}/referees.in
#ru-add-catalog catalogs/creferees.dat tmp/${YEAR}/referees.in
#wf-rsort
