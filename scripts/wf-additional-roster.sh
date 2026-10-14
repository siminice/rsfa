#!/bin/bash
# tmp/${YEAR}/${TEAM}.links is now 4 fields (post,num,id,slug -- see
# WfRoster.java / wf-extract-names.sh), so a fallback line appended here
# needs 4 fields too (no id available from .app, left blank). Note
# tmp/${YEAR}/${TEAM}.app itself comes from wf-get-appearances.sh, which is
# still broken by the redesign (old "/team_performance/.../" URL, no sample
# seen yet to fix it against) -- it will just produce an empty .app file,
# so this loop is currently always a no-op in practice, not a crash.

YEAR=$1
TEAM=$2

while read n
do
  if grep -q $n tmp/${YEAR}/${TEAM}.links
  then
    i=0
  else
    echo " , , ,$n" >> tmp/${YEAR}/${TEAM}.links
  fi
done < tmp/${YEAR}/${TEAM}.app
