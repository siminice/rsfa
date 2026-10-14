#!/bin/bash
# SUPERSEDED by wf-get-schedule.sh (which now does round discovery itself,
# via wf-learn-seasons.sh/WfSeasons.java) as of the 2026 redesign. The old
# "/schedule/.../-spieltag/<round>/" URL below is dead and this script is no
# longer called from wfx-the-works.sh. Left in place in case another
# country's pipeline still calls it, but it will not work against the new
# site. See the write-up.

LEAGUE=$1
SEASON=$2
ROUND=$3
ANNOTATION=$4

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

if [ ! -f tmp/${YEAR}/r${ROUND}.html ]; then
curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}${ANNOTATION}-spieltag/${ROUND}/" > tmp/${YEAR}/r${ROUND}.html
#curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}${ANNOTATION}-spieltag_2/${ROUND}/" > tmp/${YEAR}/r${ROUND}.html
fi

cat tmp/${YEAR}/r${ROUND}.html | grep "/report/" | cut -d '"' -f 2
