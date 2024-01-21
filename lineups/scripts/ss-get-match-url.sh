#!/bin/bash

YEAR=$1
N=$2

grep ${N} tmp/${YEAR}/r*.links | cut -d ':' -f 2-
