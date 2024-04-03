#!/bin/bash
 
cat players.dat | cut -d ',' -f 5 | sort | uniq -c | sort -rn
