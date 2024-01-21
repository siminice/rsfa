#!/bin/bash

for ((y=$1; y<=$2; y++))
do
  touch cup-lineups-$y.db
  touch cup-events-$y.db
  touch euro-lineups-$y.db
  touch euro-events-$y.db
  touch nat-lineups-$y.db
  touch nat-events-$y.db
  touch catalog-$y.dat
  touch ncatalog-$y.dat
  touch cupa-$y.dat
  touch ncupa-$y.dat
  touch euro-$y.dat
  touch neuro-$y.dat
  touch nat-$y.dat
  touch nnat-$y.dat
done
