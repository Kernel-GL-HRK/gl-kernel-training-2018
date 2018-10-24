#!/bin/bash

DIRS='./dining-room/ ./kitchen/fridge/'

for DIR in $DIRS; do
  if ! [ -d "$DIR" ]; then
    mkdir -p "$DIR"
  fi
done
