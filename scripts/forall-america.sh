#!/bin/bash

for t in arg bol bra chi col ecu par per uru ven sao rio gau usa mex jap chn kor aus
do
  echo $t
  rcopy-current.sh $t
done
