#!/bin/bash

path_to_add=/usr/local/bin

if [ -n "$1" ]
then
  path_to_add=$1
fi

nnn=1
while [ 0 ]
do
  pathn=$(cut -f$nnn -d":" <<< $PATH)
  if [ ! -n "$pathn" ]; then
    break
  fi
  if [ $pathn = "$path_to_add" ]
  then
    echo "$path_to_add already in the PATH."
    exit 0
  fi
  let nnn=nnn+1
done

echo "Adding $path_to_add to PATH..."
export PATH=$PATH:${path_to_add}

if [ -n "$2" ]
then
  cp $0 "/etc/profile.d/$2"
fi
