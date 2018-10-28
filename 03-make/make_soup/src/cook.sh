#!/bin/bash

# Author: Illia Iorin	illia.iorin@gmail.com

case $1 in
    fry)
        echo $(cat $2 | sed 's/^/fried /')
        echo $(cat $3 | sed 's/^/fried /')
        ;;
    boil)
        echo $(cat $2 | sed 's/^/boiled /')
        echo $(cat $3 | sed 's/^/boiled /')
        ;;
    *)
        ;;
esac
