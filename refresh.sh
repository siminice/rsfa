#!/bin/bash

c=$1

cd ~/rsssf/sat

echo "cp $c/[a-f]*.[1-2]??? ~/github/rsfa/$c/"
/bin/cp -fv $c/[a-f]*.[1-2]??? ~/github/rsfa/$c/
echo "cp $c/*.dat ~/github/rsfa/$c/"
/bin/cp -fv $c/[a-f]*.[1-2]??? ~/github/rsfa/$c/
echo "cp $c/lineups/catalogs/*.dat ~/github/rsfa/$c/lineups/catalogs/"
/bin/cp -fv $c/lineups/catalogs/*.dat ~/github/rsfa/$c/lineups/catalogs/
echo "cp $c/lineups/data/* ~/github/rsfa/$c/lineups/data/"
/bin/cp -fv $c/lineups/data/*.dat ~/github/rsfa/$c/lineups/data/
echo "cp $c/lineups/db/*.db ~/github/rsfa/$c/lineups/db/"
/bin/cp -fv $c/lineups/db/*.db ~/github/rsfa/$c/lineups/db/
