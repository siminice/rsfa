#!/bin/bash

YEAR=$1

grep /match tmp/${YEAR}/season.html | cut -d '"' -f 8 | grep -v href
