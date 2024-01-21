#!/bin/bash

#cp data/webteams.dat data/zteams.dat
echo 'zz-dup-catalog players > catalogs/zplayers.dat'
zz-dup-catalog players > catalogs/zplayers.dat
echo 'zz-dup-catalog coaches > catalogs/zcoaches.dat'
zz-dup-catalog coaches > catalogs/zcoaches.dat
echo 'zz-dup-catalog referees > catalogs/zreferees.dat'
zz-dup-catalog referees > catalogs/zreferees.dat
echo 'cp catalogs/stadium.dat catalogs/zstadium.dat'
cp catalogs/stadium.dat catalogs/zstadium.dat
