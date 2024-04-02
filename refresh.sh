#!/bin/bash

c=$1

echo 'rm -f ${c}/[a-f]*.[1-2]???'
rm -f ${c}/[a-f]*.[1-2]???
echo 'rm -f ${c}/lineups/catalogs/*.dat'
rm -f ${c}/lineups/catalogs/*.dat
echo 'rm -f ${c}/lineups/data/*.dat'
rm -f ${c}/lineups/data/*.dat
echo 'rm -f ${c}/lineups/data/[a-f]*.???'
rm -f ${c}/lineups/data/[a-f]*.???
echo 'rm -f ${c}/lineups/db/*.db'
rm -f ${c}/lineups/db/*.db

cd ~/rsssf/sat

echo "cp $c/[a-f]*.[1-2]??? ~/github/rsfa/$c/"
cp $c/[a-f]*.[1-2]??? ~/github/rsfa/$c/
echo "cp $c/lineups/catalogs/*.dat ~/github/rsfa/$c/lineups/catalogs/"
cp $c/lineups/catalogs/*.dat ~/github/rsfa/$c/lineups/catalogs/
echo "cp $c/lineups/data/*.dat ~/github/rsfa/$c/lineups/data/"
cp $c/lineups/data/*.dat ~/github/rsfa/$c/lineups/data/
echo "cp $c/lineups/data/[a-f]*.???? ~/github/rsfa/$c/lineups/data/"
cp $c/lineups/data/[a-f]*.???? ~/github/rsfa/$c/lineups/data/
echo "cp $c/lineups/db/*.db ~/github/rsfa/$c/lineups/db/"
cp $c/lineups/db/*.db ~/github/rsfa/$c/lineups/db/
