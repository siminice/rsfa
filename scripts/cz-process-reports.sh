#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)

for f in tmp/${YEAR}/r/*.html
do
  echo $f
  java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Report $f > $f.dat
done
