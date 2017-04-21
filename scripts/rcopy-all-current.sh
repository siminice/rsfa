#!/bin/bash

FOLDER=$1

ctty=(alb and arg arm aus aut aze bel blr bol bos bra bul chi chn col cro cyp cze den ecu eng est fae fin fra gau geo ger gib gre hun irl isl isr ita jap kaz kor kos lat lit lux mac mal mex mnt mol ned nir nor par per pol por rio rsm rus sao sco slo spa sui svk swe tur ukr uru usa ven wal yug)
# + ddr

for t in ${!ctty[@]}
do
  echo $((t+1))/${#ctty[@]} " copying ${ctty[t]} ..."
#  copy-data-to-rsfa $t
  ./rcopy-current.sh ${ctty[t]} ${FOLDER} > /dev/null
done
