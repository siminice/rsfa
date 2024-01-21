#!/bin/bash

YEAR=$1

new-catalog ${YEAR} -f 2> log
cat log >> catalogs/catalog-${YEAR}.dat
new-catalog ${YEAR} -f
cat log
