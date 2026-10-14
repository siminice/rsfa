#!/bin/bash
# NEW for the 2026 worldfootball.net redesign.
#
# The new URL scheme needs a competition id (co<id>), a season id (se<id>)
# and a round id (ro<id>, constant for every matchday of one season) that
# didn't exist before and aren't stored anywhere in data/. This script is
# the bridge: point it at a competition (via data/wf-coid + data/ln) and it
# fetches ONE results-and-standings page, then has WfSeasons.java read both
# dropdowns the page ships with.
#
# One run (no args) against the CURRENT season:
#   - learns every se<id> this competition has ever had (confirmed to go
#     back to the competition's first season in one page -- run again only
#     when a new season starts) and appends new ones to data/wf-seasons.dat
#   - prints that current season's ROUND lines (round id + one path per
#     matchday) to stdout
#
# Run with "se<id> <years>" for a PAST season (its se<id> comes from
# data/wf-seasons.dat once the no-args run above has populated it) to get
# THAT season's ROUND lines instead.
#
# Needs: data/wf-coid  (e.g. "co123", read off any fetched page's URL by
#        hand once -- there's no way to discover it without fetching)
#        data/ln       (the NEW competition slug, e.g. "portugal-primeira-liga" --
#        note this is NOT the old "por-primeira-liga" style slug, so this
#        file needs a one-time update per league before this script works)

COID=$(cat data/wf-coid)
SLUG=$(cat data/ln)
SEID=$1
YEARS=$2

mkdir -p tmp/seasons
if [ -z "${SEID}" ]; then
  URL="https://www.worldfootball.net/competition/${COID}/${SLUG}/results-and-standings/"
  OUT="tmp/seasons/current.html"
else
  URL="https://www.worldfootball.net/competition/${COID}/${SLUG}/${SEID}/${YEARS}/results-and-standings/"
  OUT="tmp/seasons/${YEARS}.html"
fi

curl -s "${URL}" > "${OUT}"
jrun WfSeasons "${OUT}" > tmp/seasons/learned.txt

# WfSeasons prints the dropdown's own "2025/2026" slash form; the rest of
# this pipeline (SEASON args, data/wf-seasons.dat, the URL path itself) all
# use "2025-2026" with a hyphen, so translate on the way into the cache.
touch data/wf-seasons.dat
grep "^SEASON," tmp/seasons/learned.txt | cut -d ',' -f 2-3 | tr '/' '-' >> data/wf-seasons.dat
sort -u -o data/wf-seasons.dat data/wf-seasons.dat

grep "^ROUND," tmp/seasons/learned.txt | cut -d ',' -f 2-3
