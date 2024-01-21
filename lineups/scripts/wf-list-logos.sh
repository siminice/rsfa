#!/bin/bash

LEAGUE=$1
SSN=$2
URL="https://www.worldfootball.net/players/${LEAGUE}-${SSN}/"

wget -q "${URL}" -O teams.html
jrun WfLogo teams.html
