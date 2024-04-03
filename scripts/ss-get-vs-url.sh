#!/bin/bash

YEAR=$1
H=$2
A=$3

grep ${H} tmp/${YEAR}/r*.links | grep ${A} | cut -d ':' -f 2-
