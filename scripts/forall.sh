#!/bin/bash

RSFA="192.168.1.156"
WEB="/var/www/html"
LOC="/home/radu/rsssf/sat"
URL="radu@${RSFA}:${LOC}"

for t in alb and arg arm aus aut aze bel blr bol bos bra bul chi chn col cro cyp cze ddr den ecu eng est fae fin fra gau geo ger gib gre hun irl isl isr ita jap kaz kor kos lat lit lux mac mal mex mnt mol ned nir nor par per pol por rio rsm rus sao sco slo spa sui svk swe tur ukr uru usa ven wal yug
do
  echo $t
#  rcopy-current.sh $t
#  cd $t
#  transload > webteams.dat
#  cd ../
  cp "${WEB}/${t}/winter.dat" "${t}/"
done
