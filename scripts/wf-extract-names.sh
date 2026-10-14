#!/bin/bash
# Updated for the 2026 redesign. wf-get-roster (raw C, line-based strstr())
# can't cope with the new site's minified single-line HTML regardless of
# selector fixes, so roster extraction moved to WfRoster.java (jsoup). Its
# output grew a 4th field: "post,num,id,slug" instead of "post,num,name",
# because the new player URL needs the numeric id ("/person/pe<id>/<slug>/")
# while the catalog (cplayers.dat) still keys off the bare slug. Likewise
# wf-get-player (same strstr()-on-minified-HTML problem, and the old
# player_summary page is gone anyway) is replaced by WfPlayer.java, already
# fixed for the new unified /person/ page and called the same way
# wfx-process-coaches.sh already calls it for coaches.

FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -a -n ",${TEAM}" data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))
echo " .${TEAM} = t${TN}"

jrun WfRoster ${FILE} > tmp/${YEAR}/${TEAM}.links
grep "^C," tmp/${YEAR}/${TEAM}.links > tmp/${YEAR}/${TEAM}.coach
grep -v "^C," tmp/${YEAR}/${TEAM}.links > cppl
mv -f cppl tmp/${YEAR}/${TEAM}.links

wf-additional-roster.sh ${YEAR} ${TEAM}

while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  id=$(echo ${link} | cut -d ',' -f 3)
  slug=$(echo ${link} | cut -d ',' -f 4)
  if [ -z "${slug}" ]; then continue; fi
  if [ ! -f p/${slug}.html ]; then
    curl -s "https://www.worldfootball.net/person/${id}/${slug}/" > p/${slug}.html
  fi
  jrun WfPlayer p/${slug}.html ${slug}
done <  tmp/${YEAR}/${TEAM}.links | tee tmp/${YEAR}/t${TID}.in
