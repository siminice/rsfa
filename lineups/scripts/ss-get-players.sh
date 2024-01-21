#!/bin/bash

YEAR=$1

while read t
do
  ss-extract-names.sh tmp/${YEAR}/${t}.html
done < tmp/${YEAR}/t.links
