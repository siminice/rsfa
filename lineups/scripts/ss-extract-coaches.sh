#!/bin/bash

YEAR=$1

ss-get-coaches.sh ${YEAR} | tee tmp/${YEAR}/coaches.in

zz-add-catalog catalogs/zcoaches.dat tmp/${YEAR}/coaches.in

zz-csort
