#!/bin/bash

CURR=2017
PREV=2016

export MALLOC_CHECK_=0

  echo $1
  cd $1
    transload > webteams.dat
    for f in [a-f]*.{${CURR},${PREV}}
    do
      html-season $f
      html-season $f -c
    done
    part
    html-index
    mv index.html html/
    html-podium
    html-team
#    html-palmares
  cd ..


#    html-streak -n 10 -t -1
#    mv html/consecutive--1.html html/consecutive.html
