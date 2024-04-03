#!/bin/bash

url=$(curl -s "http://us.soccerway.com/search/?q=$1+$2" | grep "players/" | tail -n 1 | cut -d '"' -f 4)
echo "http://us.soccerway.com${url}" > url.info
curl -s "http://us.soccerway.com${url}" > player.html

grep "First name" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Last name" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Nationality" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Date of birth" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Place of birth" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Country of birth" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
grep "Position" player.html -A 1 | tail -n 1 | cut -d '<' -f 2 | cut -d '>' -f 2
