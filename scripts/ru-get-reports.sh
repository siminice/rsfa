#!/bin/bash

YEAR=$1

grep /match tmp/${YEAR}/season.html | cut -d "'" -f 6 | grep -v href
