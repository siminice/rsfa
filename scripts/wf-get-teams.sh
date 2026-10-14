#!/bin/bash
# Rewritten for the 2026 redesign. The old "/players/<league>-<season>/"
# team-listing page is gone; team discovery now comes straight off the
# season's own standings table (WfTeams.java), one page fetch for the whole
# league instead of a link-grep page. Needs data/wf-coid (new numeric
# competition id) and data/ln (the NEW competition slug, e.g.
# "portugal-primeira-liga" -- NOT the old "por-primeira-liga" style) plus
# the season already resolved in data/wf-seasons.dat (wf-learn-seasons.sh).
#
# CURRENT squad only: the per-team fetch below uses
# "/teams/te<id>/<slug>/", which is the site's "current squad" page. For a
# season other than the current one this is almost certainly wrong -- by
# analogy with the team's other season-scoped subpages (e.g.
# ".../se<id>/<years>/statistics-matches/") the historical squad is
# INFERRED to live at ".../se<id>/<years>/squad/" (commented out below) but
# this has NOT been confirmed against a real sample. Verify it once before
# trusting a backfill run.

COID=$(cat data/wf-coid)
SLUG=$(cat data/ln)
SEASON=$1
ANNOTATION=$2
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

SEID=$(grep "^${SEASON}," data/wf-seasons.dat | cut -d ',' -f 2)
if [ -z "${SEID}" ]; then
  echo "wf-get-teams.sh: season ${SEASON} not in data/wf-seasons.dat -- run wf-learn-seasons.sh first." >&2
  exit 1
fi

curl -s "https://www.worldfootball.net/competition/${COID}/${SLUG}/${SEID}/${SEASON}/results-and-standings/" > tmp/${YEAR}/standings.html
jrun WfTeams tmp/${YEAR}/standings.html > tmp/${YEAR}/t.teams

rm -f tmp/${YEAR}/t.links
while IFS=',' read -r tid tslug tname
do
  TN=$(grep -a -n ",${tslug}" data/webteams.dat | head -n 1 | cut -d ':' -f 1)
  TID=$((${TN}-2))

  echo "Team ${tslug} (${tname}) = T#${TID}"
  echo ${tslug} >> tmp/${YEAR}/t.links

  if [ ! -f tmp/${YEAR}/${tslug}.html ]; then
    curl -s -X GET "https://www.worldfootball.net/teams/${tid}/${tslug}/" > tmp/${YEAR}/${tslug}.html
    # Historical season (inferred, unconfirmed -- see note above):
    # curl -s -X GET "https://www.worldfootball.net/teams/${tid}/${tslug}/${SEID}/${SEASON}/squad/" > tmp/${YEAR}/${tslug}.html
  fi
done < tmp/${YEAR}/t.teams

wf-get-appearances.sh ${SLUG} ${SEASON} ${ANNOTATION}
