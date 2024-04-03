#!/bin/bash

YEAR=$1

while read t
do
  wf-extract-names.sh tmp/${YEAR}/${t}.html
done < tmp/${YEAR}/t.links
