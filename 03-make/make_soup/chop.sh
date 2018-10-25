#!/bin/bash

INGRED_NAME=$(basename $1)

echo "Chop $3 pieces of $INGRED_NAME into $2"

SOURCE=$(cat $1)

if [ "$SOURCE" -le "$3" ] ; then
  DEST=$SOURCE
  rm $1
else
  DEST=$3
  SOURCE=$((SOURCE - $3))
  echo "$SOURCE" > $1
fi

echo "$3 chopped $INGRED_NAME" >> $2

exit 0