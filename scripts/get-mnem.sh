#!/bin/bash

#echo "Usage: get-mnem.sh <year> <tid>"

cat seasons/$1/$2.dat | cut -d ',' -f 1
