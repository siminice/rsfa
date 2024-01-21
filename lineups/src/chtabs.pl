#!/bin/bash 

perl -e '($_ = join "",<>) =~ s/(\t)(\t)/\t_\t/g; print;' < $1 > $2
