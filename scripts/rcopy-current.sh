#!/bin/bash

URL='192.168.1.156'
CURR="2017"
PREV="2016"
FOLDER=${2:-"/var/www/html"}

cd $1
 scp [a-f]*.{${CURR},${PREV}} radu@${URL}:${FOLDER}/$1/
 scp teams.dat alias.dat part.med radu@${URL}:${FOLDER}/$1/
cd ..
