#!/bin/bash

export MALLOC_CHECK_=0

ctty=(alb and arg arm aus aut aze bel blr bol bos bra bul chi chn col cro cyp cze den ecu eng est fae fin fra gau geo ger gib gre hun irl isl isr ita jap kaz kor kos lat lit lux mac mal mex mnt mol ned nir nor par per pol por rio rsm rus sao sco slo spa sui svk swe tur ukr uru usa ven wal yug)
# + ddr

for t in ${!ctty[@]}
do
  echo " "
  echo $((t+1))/${#ctty[@]} " ***** ${ctty[t]} ***** "
#  cd $t
#    transload > webteams.dat
#    part
#    html-index
#    mv -f index.html html/
#  cd ..
  current-season.sh ${ctty[t]} > /dev/null 2>&1
done


#    html-streak -n 10 -t -1
#    mv html/consecutive--1.html html/consecutive.html
#    html-streak -n 10 -d 1 -t -1
#    mv html/consecutive--1.html html/consecutive-a.html
