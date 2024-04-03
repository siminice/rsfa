#!/bin/bash

YEAR=$1

ru-get-reports.sh ${YEAR} | tee tmp/${YEAR}/rep.links
ru-fetch-reports.sh ${YEAR}
ru-extract-reports.sh ${YEAR}
