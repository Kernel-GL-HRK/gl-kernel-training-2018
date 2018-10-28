#!/bin/bash

# Author: Illia Iorin	illia.iorin@gmail.com
echo $1
echo $(cat $1 | sed 's/^/cutted /' ) > $1
