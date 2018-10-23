#!/bin/bash

#truncate all command-line options to normal length
#also trick to get last option
for LAST_ARG ; do
  shift
  case "$LAST_ARG" in
  "--help") set -- "$@" "-h" ;;
  *) set -- "$@" "$LAST_ARG";;
  esac
done

#parse commandline options
while getopts ":n:h" OPT; do
  case $OPT in
  n) echo "Found  argument for option $OPT - $OPTARG"
     FILE_NUM=$OPTARG
     if ! [[ "$FILE_NUM" =~ ^[0-9]+$ ]]; then
       echo "Number of files should be positive integer"
       exit 1
     fi
     if [[ $FILE_NUM < 1 ]]; then
       echo "Number of files is less than 1"
       exit 1
     fi;;
  h) echo "Short guide will be displayed here"; exit 0;;
  *) echo "Wrong commandline options specified, see --help for details"; exit 1;;
  esac
done

#set default file 
if [ "$LAST_ARG" = "$FILE_NUM" ]; then
  FILE_PATH='~/bash/task1.out'
else
  FILE_PATH="$LAST_ARG"
fi

FILE_DIR=$(dirname "$FILE_PATH")

#check if desired directory exists
if ! [ -d "$FILE_DIR" ]; then
  echo directory not exists
  mkdir -p "$FILE_DIR"
fi

#see if file with name like this exists
if [ -f "$FILE_PATH" ]; then
  
  FILE_NAME=$(basename "$FILE_PATH")
  FILE_NAME_WITHOUT_EXT=$(echo "$FILE_NAME" | cut -d"." -f1 )
  FILE_NAME_EXT=$(echo "$FILE_NAME" | cut -d"." -f2 )

  #ugly way to see, if user specified extension
  if [ "$FILE_NAME_WITHOUT_EXT" = "$FILE_NAME_EXT" ]; then
    FILE_NAME_EXT=""
  fi
  
  #read all files and find id for last of them
  CURR_DATE=$(date +%Y%m%d)

  CURR_DATE_LAST_ID=0

  #find last id
  for FILE in $(ls 2>/dev/null "$FILE_DIR/$FILE_NAME_WITHOUT_EXT-$CURR_DATE-"* | sort -n -t - -k3); do
    CURR_DATE_LAST_ID=$(echo "$FILE" | cut -d "-" -f3 | cut -d "." -f1)
  done

  CURR_DATE_LAST_ID=$((CURR_DATE_LAST_ID+1))

  #perform rename
  RENAMED_FILE="$FILE_DIR/$FILE_NAME_WITHOUT_EXT-$CURR_DATE-$CURR_DATE_LAST_ID"

  if [ -n "$FILE_NAME_EXT" ]; then
    echo "Extension: $FILE_NAME_EXT"
    RENAMED_FILE="$RENAMED_FILE.$FILE_NAME_EXT"
  fi

  mv "$FILE_PATH" "$RENAMED_FILE" 
  
  #delete some files
  if [ -n "$FILE_NUM" ]; then
    echo "Will drop some files"
    #list and drop
    for FILE in $(ls 2>/dev/null "$FILE_DIR/$FILE_NAME_WITHOUT_EXT-"* | sort -n -r -t - -k2 -k3); do
      FILE_NUM=$((FILE_NUM-1))
      if (("$FILE_NUM" < "0")); then
        rm "$FILE"
      fi
    done
  fi
fi

touch "$FILE_PATH"

echo Use path "$FILE_PATH"

exit 0


echo "--------------Hardware---------------------"
echo CPU: \"$(dmidecode -t 4 | grep Version | cut -d' ' -f2-)\"
echo RAM: $( sudo dmidecode --type memory | awk '
/Size: [0-9]+ MB$/ { 
  total += $2; 
  } END { 
  print total 
}') MB
echo Motherboard: \"$(dmidecode -s baseboard-manufacturer)\", \"$(dmidecode -s baseboard-product-name)\"
echo System Serial Number: $(dmidecode -s system-serial-number)

echo "--------------System-----------------------"
echo $(lsb_release -a 2>/dev/null | grep Description | awk '{sub(/Description/,"OS Distribution",$0); print $0}')
echo Kernel version: $(uname -r)
echo Hostname: $(uname -n)
echo Uptime: $(uptime | awk '{sub(/,/," ",$0); for (i=0; i<=NF; i++){if ($i == "up"){print $(i+1);exit}}}')
echo Processes running: $(ps -A | wc -l)
echo Users logged in: $(who | wc -l)

echo "--------------Network-----------------------"
ip addr | awk '
/^[0-9]+:/ {
  sub(/:/,"",$2); iface=$2  
}
/inet / { 
  print iface" : "$2""
}'
echo "-------------EOF----------------------------"




