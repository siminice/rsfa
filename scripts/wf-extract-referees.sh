#!/bin/bash
# Rewritten for the 2026 redesign. The old "referee_summ" link grep against
# raw match-report HTML is dead (no such pattern on the new pages). The
# referee's slug -- and, since WfReport.java was extended to print it, their
# numeric id too -- is already sitting in the "Referee: <slug> <id>" line
# every processed report prints, so this reads that instead of re-grepping
# HTML. wf-get-referee (raw C, strstr() against pretty-printed HTML) can't
# parse the new site's minified markup anyway, and the old referee page
# itself is gone -- replaced by WfPlayer.java against the new unified
# /person/ page, same as for players.

YEAR=$1

grep -h "^Referee: " tmp/${YEAR}/r/*.html.dat | sed 's/^Referee: //' | awk 'NF==2' | sort -u -k1,1 > tmp/${YEAR}/refs.dat

while read -r slug id
do
  if [ -z "${slug}" ] || [ -z "${id}" ]; then continue; fi
  if [ ! -f p/ref-${slug}.html ]; then
    curl -s "https://www.worldfootball.net/person/${id}/${slug}/" > p/ref-${slug}.html
  fi
  jrun WfPlayer p/ref-${slug}.html ${slug}
done < tmp/${YEAR}/refs.dat | tee tmp/${YEAR}/refs.in

wf-add-referees.sh ${YEAR}
wf-rsort
