#!/bin/bash
 
cat players.dat | cut -d ',' -f 7 | sort | uniq -c | sort -rn
