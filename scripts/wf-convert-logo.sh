#!/bin/bash

LINK=$1
TEAM=$2
EXT=${3:-jpg}

wget "${LINK}" -O html/logo/logo-${TEAM}.${EXT}
convert html/logo/logo-${TEAM}.${EXT} html/logo/logo-${TEAM}.png
