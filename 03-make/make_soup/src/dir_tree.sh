#!/bin/bash

# Author: Illia Iorin	illia.iorin@gmail.com
ROOT=$(pwd)
mkdir kitchen dinning-room food-market
cd kitchen
mkdir table knife
cd table
mkdir pot spoon
cd $ROOT
tree -d
