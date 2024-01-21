#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)

curl -s "https://www.csfotbal.cz/prvni-liga/sezona/${SEASON}/kluby" > tmp/${YEAR}/kluby.html

java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Clubs tmp/${YEAR}/kluby.html | tee tmp/${YEAR}/teams.dat
