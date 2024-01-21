#!/bin/bash

LEAGUE=$(cat data/xmln)
CTTY=$(cat data/xmct)
SEASON=$1
YEAR=$((${SEASON}+1))

wget "https://www.transfermarkt.us/${LEAGUE}/gesamtspielplan/wettbewerb/${CTTY}/saison_id/${SEASON}" -O tmp/${YEAR}/results.html

grep spielbericht tmp/${YEAR}/results.html  | cut -d '"' -f 10 > tmp/${YEAR}/rep.links
