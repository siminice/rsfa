#!/bin/bash
# Rewritten for the 2026 redesign. The old per-round URL
# ("/schedule/<league>-<season>-spieltag/<round>/") is gone; a matchday page
# now needs a round id (ro<id>, constant for the whole season) baked into
# its path, which wf-learn-seasons.sh/WfSeasons.java look up from the
# season's own results-and-standings page. This replaces wf-get-round-links.sh
# too -- round discovery is one page fetch for the whole season now, not one
# curl per round.

SEASON=$1
N=$2
ANNOTATION=$3

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

SEID=$(grep "^${SEASON}," data/wf-seasons.dat | cut -d ',' -f 2)
if [ -z "${SEID}" ]; then
  echo "wf-get-schedule.sh: season ${SEASON} not in data/wf-seasons.dat -- run wf-learn-seasons.sh first." >&2
  exit 1
fi

rm -f tmp/${YEAR}/rep.links
mkdir -p tmp/${YEAR}/md

ROUNDS=$(wf-learn-seasons.sh ${SEID} ${SEASON})

for ((r=1; r<=${N}; r++))
do
  echo "${SEASON} R$r"
  MDPATH=$(echo "${ROUNDS}" | grep "^${r}," | cut -d ',' -f 2)
  if [ -z "${MDPATH}" ]; then
    echo " !! no matchday link learned for round ${r} -- skipping" >&2
    continue
  fi
  if [ ! -f tmp/${YEAR}/md/r${r}.html ]; then
    curl -s "https://www.worldfootball.net${MDPATH}" > tmp/${YEAR}/md/r${r}.html
  fi
  grep -o '/match-report/[^"]*/' tmp/${YEAR}/md/r${r}.html | sort -u >> tmp/${YEAR}/rep.links
done
