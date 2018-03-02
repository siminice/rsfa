#!/bin/bash

FOLDER="/var/www/html"
CURR=2018
PREV=2017

cttys=(alb and arg arm aus aut aze bel bos bul cro cyp cze den eng fra ger gib gre hun isr ita kos lux mac mal mex mnt mol ned nir pol por rsm rus sco slo spa sui svk tur ukr wal yug)
cttyw=(blr est fae fin geo isl irl kaz lat lit nor swe bol bra chi col ecu par per uru ven chn jap kor usa sao rio gau min)
# + ddr

echo "Fall/Spring ${#cttys[@]}"
for t in ${!cttys[@]}
do
  printf "%02d/%02d copying %s ...\n" $((t+1)) "${#cttys[@]}" "${cttys[t]}"
#  echo $((t+1))/${#cttys[@]} " copying ${cttys[t]} ..."
  lcopy-current.sh ${cttys[t]} ${CURR} ${FOLDER} > /dev/null
done

echo "Summer ${#cttyw[@]}"
for t in ${!cttyw[@]}
do
  printf "%02d/%02d copying %s ...\n" $((t+1)) "${#cttyw[@]}" "${cttyw[t]}"
#  echo $((t+1))/${#cttyw[@]} " copying ${cttyw[t]} ..."
  lcopy-current.sh ${cttyw[t]} ${CURR} ${FOLDER} > /dev/null
  lcopy-current.sh ${cttyw[t]} ${PREV} ${FOLDER} > /dev/null
done
