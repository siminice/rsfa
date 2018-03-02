#!/bin/bash

CURR=$2
PREV=$((${CURR}-1))
FOLDER=${3:-"/var/www/html"}

cd $1
 cp -v [a-f]*.{${CURR},${PREV}} ${FOLDER}/$1/
 cp -v teams.dat alias.dat webteams.dat part.med ${FOLDER}/$1/
cd ..
