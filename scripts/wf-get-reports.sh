#!/bin/bash
# Updated for the 2026 redesign's match-report URL shape:
#   /match-report/co<id>/<comp-slug>/ma<id>/<home-slug>_<away-slug>/
# field 3 used to be the match's own slug (old "/report/<slug>/..."); the
# stable per-match token now is field 5, "ma<id>".

YEAR=$1
BASEURL="https://www.worldfootball.net"

while read link
do
 echo ${link}
 m=$(echo ${link} | cut -d '/' -f 5)
 if [ ! -f tmp/${YEAR}/r/${m}.html ]; then
   curl -s -X GET "${BASEURL}${link}" > tmp/${YEAR}/r/${m}.html
 fi
done < tmp/${YEAR}/rep.links
