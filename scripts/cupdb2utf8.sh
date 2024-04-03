#!/bin/bash

mv $1 $1.iso
cat $1.iso | ./utf8substr.sh > $1
