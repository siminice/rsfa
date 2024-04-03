#!/bin/bash

cat $1 | head -n 21 | tail -n 1 | cut -d ',' -f 10-31,33-54 | tokenize , | grep -v Using | grep -v ' :0' | cut -d ':' -f 1
