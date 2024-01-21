#!/bin/bash

cat $1 | head -n 21 | tail -n 1 | cut -d ',' -f 32,55 | grep -v "Using" | tokenize ,
