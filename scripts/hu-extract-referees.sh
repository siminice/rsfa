#!/bin/bash

YEAR=$1

hu-get-referees.sh ${YEAR} | tee tmp/${YEAR}/referees.in

zz-add-catalog catalogs/zreferees.dat tmp/${YEAR}/referees.in

zz-rsort
