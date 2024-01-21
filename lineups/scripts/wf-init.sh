#!/bin/bash

CTTY=$1
FY=${2:-2020}
LY=${3:-2020}
LN=${4:-?}

cd /home/radu/rsssf/sat/${CTTY}
mkdir lineups
cd lineups

mkdir catalogs
mkdir data
mkdir db
mkdir seasons
mkdir tmp
mkdir p
mkdir c
mkdir r

echo "${LN}" > ln.dat
echo "${LN}" > data/ln
cp /home/radu/rsssf/sat/eng/lineups/catalogs/country.dat catalogs/

echo "1" > catalogs/players.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/players.dat >> catalogs/players.dat
echo "1" > catalogs/coaches.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/players.dat >> catalogs/coaches.dat
echo "1" > catalogs/referees.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/players.dat >> catalogs/referees.dat
echo "1" > catalogs/cplayers.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/cplayers.dat >> catalogs/cplayers.dat
echo "1" > catalogs/ccoaches.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/cplayers.dat >> catalogs/ccoaches.dat
echo "1" > catalogs/creferees.dat
tail -n 1 /home/radu/rsssf/sat/spa/lineups/catalogs/cplayers.dat >> catalogs/creferees.dat

echo "0" > catalogs/city.dat
echo "0" > catalogs/stadium.dat
echo "0" > catalogs/cstadium.dat

for ((y=${FY}; y<=${LY}; y++))
do
  echo $y
  mkdir seasons/$y
  mkdir tmp/$y
  mkdir tmp/$y/r
done

cp ~/rsssf/sat/${CTTY}/a.???? data/
cp ~/rsssf/sat/${CTTY}/{teams,alias,webteams}.dat data/

mkdir /var/www/html/${CTTY}/html/jucatori
mkdir /var/www/html/${CTTY}/html/antrenori
mkdir /var/www/html/${CTTY}/html/arbitri
mkdir /var/www/html/${CTTY}/html/data
mkdir /var/www/html/${CTTY}/html/reports

cd /var/www/html/nor
wf-touch.sh ${FY} ${LY}

for ((y=${FY}; y<=${LY}; y++))
do
  mkdir /var/www/html/${CTTY}/html/reports/${y}
done

cp /var/www/html/spa/country.dat /var/www/html/${CTTY}/
mkdir /var/www/html/${CTTY}/data
mkdir /var/www/html/${CTTY}/html/logo
cp /var/www/html/rom/html/*.{png,gif} /var/www/html/${CTTY}/html/

