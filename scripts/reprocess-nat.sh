#!/bin/bash

nat-clubs $1 > temp
nano temp
mv temp nat-$1.dat
new-catalog $1 3
nano nnat-$1.dat
