#!/bin/bash

INDEX=$1
TEAM=$2

wget "https://s.hs-data.com/bilder/wappen/mittel/${INDEX}.gif" -O html/logo/logo-${TEAM}.gif
convert html/logo/logo-${TEAM}.gif html/logo/logo-${TEAM}.png
