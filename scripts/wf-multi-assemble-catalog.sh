#!/bin/bash

for ((y=$1; y<=$2; y++))
do
  echo $y
  wf-assemble-catalog.sh $y
done
