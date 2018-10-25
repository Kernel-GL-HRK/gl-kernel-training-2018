#!/bin/bash

echo  "Moving $3 pieces from $1 to $2"

SOURCE=$(cat $1)


if [ "$SOURCE" -le "$3" ] ; then
  DEST=$SOURCE
  rm $1
else
  DEST=$3
  SOURCE=$((SOURCE - $3))
  echo "$SOURCE" > $1
fi

touch $2 && echo "$DEST" > $2

exit 0