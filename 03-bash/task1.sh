#!/bin/bash -x

#init variables
# TOF: Task Out File
#TOF=~/bash/task1.out
TOF=task1.out
#str=''


#prepeare output file
touch $TOF

#write data to the file
echo $(date -R) > $TOF
#--------------------------------------
echo "---- Hardware ----" >> $TOF
#CPU:
echo -n "CPU: " >> $TOF;
str=$(sudo dmidecode -t processor | grep 'Version:')
if [[ $str = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo -n "\"" >> $TOF
	echo -n ${str#*'Version:'} >> $TOF
	echo "\"" >> $TOF
fi
#RAM:
echo -n "RAM: " >> $TOF;
str=$(sudo dmidecode -t memory | grep 'Size: [0-9]')
if (( ${#str} == 0 )); then
	echo 'Unknown' >> $TOF
else
	echo ${str#*'Size:'} >> $TOF
fi
#MB:
echo -n "Motherboard: " >> $TOF;
str=$(sudo dmidecode -t baseboard | grep 'Manufacturer:')
if (( ${#str} == 0 )); then
	echo -n 'Unknown' >> $TOF
else
	echo -n "\"" >> $TOF;
	echo -n ${str#*'Manufacturer:'} >> $TOF;
	echo -n "\"" >> $TOF;
fi
str=$(sudo dmidecode -t system | grep 'Product Name:')
if (( ${#str} == 0 )); then
	echo '' >> $TOF
else
	echo -n ", \"" >> $TOF;
	echo -n ${str#*'Product Name:'} >> $TOF
	echo "\"" >> $TOF;
fi
#System S/N
echo -n "System Serial Number: " >> $TOF
str=$(sudo dmidecode -t system | grep 'Serial Number:')
if (( ${#str} == 0 )); then
	echo 'Unknown' >> $TOF
else
	echo ${str#*'Serial Number:'} >> $TOF
fi
#--------------------------------------
echo "---- System ----" >> $TOF
#OS
echo -n "OS Distribution: " >> $TOF
if [[ $(uname -o) = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo -n "\"" >> $TOF
	echo -n $(uname -o) >> $TOF
	echo "\"" >> $TOF
fi
#Kernel
echo -n "Kernel version: " >> $TOF
if [[ $(uname -r) = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo $(uname -r) >> $TOF
fi
#Inst date
echo -n "Installation date: " >> $TOF
str=$(sudo head -n1 /var/log/installer/syslog | cut -f1 -d"u")
if (( ${#str} == 0 )); then
	echo 'Unknown' >> $TOF
else
	echo ${str} >> $TOF
fi
#Host
echo -n "Hostname: " >> $TOF
if [[ $(hostname) = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo $(hostname) >> $TOF
fi
#Uptime
echo -n "Uptime: " >> $TOF
if [[ $(uptime -p) = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo $(uptime -p) >> $TOF
fi
#Processings
echo -n "Processes running: " >> $TOF
str=$(ps x | wc -l)
if (( $str == 0 )); then
	echo 'Unknown' >> $TOF
else
	echo ${str} >> $TOF
fi
#User
echo -n "User logged in: " >> $TOF
if [[ $(users) = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo $(users | wc -w) >> $TOF
fi
#--------------------------------------
echo "---- Network ----" >> $TOF
for interface in $(ifconfig | cut -f1 -d" ")
do
	echo -n "$interface: " >> $TOF

	ip=$(ifconfig $interface | grep "inet addr" | cut -f2 -d":" | cut -f1 -d" ")
	if (( ${#ip} == 0 )); then
		echo "-/-" >> $TOF
	else
		echo -n ${ip#*'inet addr'} >> $TOF
		echo -n "/" >> $TOF
		mask=$(ifconfig $interface | grep "Mask")
		if (( ${#mask} == 0 )); then
			echo "-" >> $TOF
		else
			bitmask=${mask#*'Mask:'}
			case $bitmask in
				255.0.0.0    ) echo "24" >> $TOF;;
				255.255.0.0  ) echo "16" >> $TOF;;
				255.255.255.0) echo "8"  >> $TOF;;
				*            ) echo "-"  >> $TOF;;
			esac
		fi
	fi
done

echo "----\"EOF\"----" >> $TOF

exit 0
