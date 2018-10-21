#!/bin/bash

#truncate all command-line options to normal length
for LAST_ARG ; do
  shift
  case "$LAST_ARG" in
  "--help") set -- "$@" "-h" ;;
  *) set -- "$@" "$LAST_ARG";;
  esac
done

#trick to get last option
echo "$LAST_ARG"

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


if [ "$LAST_ARG" = "$FILE_NUM" ]; then
  FILE_PATH='~/bash/task1.out'
else
  FILE_PATH="$LAST_ARG"
fi

echo Use path "$FILE_PATH"

exit 0


echo "--------------Hardware---------------------"
echo CPU: \"$(dmidecode -t 4 | grep Version | cut -d' ' -f2-)\"
echo RAM: $( sudo dmidecode --type memory | awk '/Size: [0-9]+ MB$/ { total += $2; } END { print total }') MB
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




