#!/bin/bash


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




