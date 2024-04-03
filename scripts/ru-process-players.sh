#!/bin/bash

YEAR=$1

ru-get-all-players.sh ${YEAR}
ru-fetch-players.sh ${YEAR}
ru-extract-players.sh ${YEAR} | tee tmp/${YEAR}/players.in
nano tmp/${YEAR}/players.in
rz-add-catalog catalogs/zplayers.dat tmp/${YEAR}/players.in
#ru-add-catalog catalogs/cplayers.dat tmp/${YEAR}/players.in
#wf-psort
