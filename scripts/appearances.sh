#!/bin/bash

PID=$1

grep "$1" db/lineups-*.db | cut -d ':' -f 1 | sort | uniq -c
grep "$1" db/events-*.db | cut -d ':' -f 1 | sort | uniq -c
grep "$1" catalogs/ncatalog-*.dat
