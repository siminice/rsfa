#!/bin/bash

CURLOPTS="-H 'accept-language: en-US' -H 'user-agent: Mozilla/5.0' --compressed"

LEAGUE=$(cat data/xmln)
CTTY=$(cat data/xmct)
SEASON=$1
YEAR=$((${SEASON}+1))

echo "Downloading https://www.transfermarkt.us/${LEAGUE}/startseite/wettbewerb/${CTTY}/plus/?saison_id=${SEASON} ..."
echo 'curl -s "https://www.transfermarkt.us/'"${LEAGUE}"'/startseite/wettbewerb/'"${CTTY}"'/plus/?saison_id='"${SEASON}"'" '"${CURLOPTS}"' > "tmp/'"${YEAR}"'/teams.html"'
curl -s "https://www.transfermarkt.us/${LEAGUE}/startseite/wettbewerb/${CTTY}/plus/?saison_id=${SEASON}" -H 'accept-language: en-US' -H 'user-agent: Mozilla/5.0' --compressed > "tmp/${YEAR}/teams.html"

#echo "Downloading https://www.transfermarkt.us/${LEAGUE}/startseite/wettbewerb/${CTTY}/saison_id/${SEASON} ..."
#wget "https://www.transfermarkt.us/${LEAGUE}/startseite/wettbewerb/${CTTY}/saison_id/${SEASON}" -O "tmp/${YEAR}/teams.html"

jrun XmTeams tmp/${YEAR}/teams.html > tmp/${YEAR}/t.links
