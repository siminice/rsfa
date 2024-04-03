#!/bin/bash

YEAR=$1

ru-get-reports.sh ${YEAR} | tee tmp/${YEAR}/rep.links
ru-fetch-reports.sh ${YEAR}
ukr-extract-reports.sh ${YEAR}
