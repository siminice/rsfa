#!/bin/bash

function join_by { local IFS="$1"; shift; echo "$*"; }

while IFS=',' read -ra TK; do
  for i in "${!TK[@]}"; do
    s=${TK[i]}
    if [[ $s = ~* ]]
    then
      echo $s > tmp
      TK[i]=$(i2u tmp)
    fi
  done
  join_by , "${TK[@]}"
done
