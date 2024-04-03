#!/bin/bash

IFILE="catalogs/c$1"
OFILE="catalogs/z$1"

cp ${IFILE}.dat ${IFILE}.old
cp ${OFILE}.dat ${OFILE}.old

zz-backfill.sh $1 > ${OFILE}.new
cp ${OFILE}.new ${OFILE}.dat

