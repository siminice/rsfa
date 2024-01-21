#!/bin/bash

YEAR=$1

find tmp/${YEAR}/r -size 0 | cut -d '/' -f 4 | cut -d '.' -f 1 > zmids
while read mid; do grep ${mid} tmp/${YEAR}/r[0-9]*.links | cut -d ':' -f 2-; done < zmids
rm -f zmids
