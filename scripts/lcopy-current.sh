#!/bin/bash

CURR="2017"
PREV="2016"
FOLDER=${2:-"/var/www/html"}

cd $1
 cp -v [a-f]*.{${CURR},${PREV}} ${FOLDER}/$1/
 cp -v teams.dat alias.dat part.med ${FOLDER}/$1/
cd ..
