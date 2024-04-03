#!/bin/bash

YEAR=$1
FILE=$2
TN=$3
TID=$4

LEAGUE=$(cat data/xmln)
CTTY=$(cat data/xmct)

URL="https://www.transfermarkt.us"

while read line
do
#443528,Gabriel Almeida Moreira,A,/gabriel-almeida-moreira/profil/spieler/443528
  pid=$(echo ${line} | cut -d ',' -f 1)
  pos=$(echo ${line} | cut -d ',' -f 3)
  link=$(echo ${line} | cut -d ',' -f 4)
  mnem=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f "p/${mnem}-${pid}.html" ]; then
#    wget -q "${URL}${link}" -O "p/${mnem}-${pid}.html"
    curl -s "${URL}${link}" -H 'accept-language: en-US' -H 'user-agent: Mozilla/5.0' --compressed > "p/${mnem}-${pid}.html"
  fi
  jrun XmPlayer "p/${mnem}-${pid}.html" "${mnem}/${pid}"
done < ${FILE}
