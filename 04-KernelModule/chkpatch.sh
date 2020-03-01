#!/bin/bash

curdir=$(pwd)
cd ~/kernel/linux
./scripts/checkpatch.pl --no-tree --no-signoff -f $curdir/$1
cd $curdir
