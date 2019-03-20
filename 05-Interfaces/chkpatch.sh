#!/bin/bash

curdir=$(pwd)
cd ~/kernel/linux
./scripts/checkpatch.pl --no-tree --no-signoff --ignore LONG_LINE,LONG_LINE_COMMENT,LONG_LINE_STRING -f $curdir/$1
cd $curdir
