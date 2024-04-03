#!/bin/bash

IFILE="catalogs/c$1.dat"
OFILE="catalogs/z$1.dat"

#aanema,Aanesland,Markus,Markus Myre Aanesland,1998-04-18,NOR, ,NOR,markus-aanesland
#aanema,Aanesland,Markus,Markus Myre Aanesland,1998-04-18,NOR, ,NOR,markus-aanesland,-1

while read line
do
  mnem=$(echo ${line} | cut -d ',' -f 1)
  name=$(echo ${line} | cut -d ',' -f 2)
  if [ "${mnem}" = "${name}" ]; then
    echo "${line}"
  else
    zline=$(grep "${line}" ${OFILE})
    if [ -z "${zline}" ]; then
      echo "${line},-1"
    else
      echo "${zline}"
    fi
  fi
done < ${IFILE}
