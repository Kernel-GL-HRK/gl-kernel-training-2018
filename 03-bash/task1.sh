#!/bin/bash -x

#init variables
# TOF: Task Out File
TOF=~/bash/task1.out
FNUM=1


function print_help()
{
	#cat << HEREDOC
	echo "Usage: $0 [-h|--help] [-n|--number NUM] [file]"
	echo " "
	echo "Collect and print to file information about hardware, OS and LAN"
	echo "optional arguments:"
	echo "  -h, --help		show this help message and exit"
	echo "  -n, --number NUM 	max number files [2..255]"
	echo "  file			path output file"
	#HEREDOC
}

print_error()
{
	echo "Wrong parameters!"
	echo "For more info use help: $0 [-h|--help]"
	printf "$1" >&2;
	exit $2
}

#detect params
if [[ $# > 0 ]]; then
	#check entered params
	for param in $@
	do
		case $param in
			-h | --help ) print_help; exit 0; ;;
			-n )
				if (( $2 > 1 )); then
					FNUM=$2
				else
					print_error "2-nd argument is not number or less than 2" 1;
				fi;
				# shift arg's for searching 3-rd arg
				shift 2;
				if [[ $1 = "" ]]; then
					# 3-rd arg is absent
					break;
				fi
				;;
			* )
				if [[ -n "$1" ]]; then
					TOF=$1
				else
					print_error "3-rd argument is not file or path" 1;
				fi
				break; ;;
		esac
	done
fi

#try to create dir if it not exist
DIR="$(dirname "$TOF")"
mkdir --parents $DIR
if [[ "$?" != "0" ]]; then
	print_error "Error during make directory" 1;
fi

#prepeare output file
touch $TOF

#file processing
if [[ $FNUM > 1 ]]; then
	#check if target file exist
	if [[ -e "$TOF" ]]; then
		#define current date by mask yyyymmdd
		TODAY=$(date -u +%Y%m%d)
		#define short file name
		FILE=$(basename "$TOF")
		#count number of files with reports
		FCOUNT=$(ls $DIR | grep "$FILE-........-[0-9]*$" | wc -w)
		#check if count of report files more than need, for deleting it
		if (( $FCOUNT >= $FNUM )); then
			#delete all 'old' files such number more than FNUM
			while [ $(ls $DIR 2>/dev/null | grep "$FILE-........-[0-9]*$" | wc -w) -gt $FNUM ]; do
				#take one old file
				oldfile=$(ls $DIR | grep "$FILE-........-[0-9]*$" | tail -n 1)
				#find index of one old file
				ofindex=$(printf "%0${zeros}g" ${oldfile:${#FILE}+10:4})
				#delete this old file if it index >= FNUM
				if (( $ofindex >= $FNUM )); then
					rm -fd "$DIR/$oldfile"
				fi
			done
		fi
		#mark files such need to rename
		for markfile in $(ls $DIR 2>/dev/null | grep "$FILE-........-[0-9]*$"); do
			mv "$DIR/$markfile" "$DIR/_$markfile"
		done
		#rename files
		MFILE="_"$FILE
		for oldfile in $(ls $DIR 2>/dev/null | grep "$MFILE-........-[0-9]*$"); do
			#find index of this file
			ofindex=$(printf "%0${zeros}g" ${oldfile:${#MFILE}+10:4})
			#increment index
			newindex=$(printf "%04d" $[$ofindex + 1])
			if (( $newindex >= $FNUM )); then
				rm -fd "$DIR/$oldfile"
			else
				#rename file
				mv "$DIR/$oldfile" "$DIR/$FILE-$TODAY-$newindex"
			fi
		done
		#rename 1-st file
		mv "$DIR/$FILE" "$DIR/$FILE-$TODAY-0000"
	fi
fi

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
uptimer=$(uptime -p)
if [[ $uptimer = "" ]]; then
	echo 'Unknown' >> $TOF
else
	echo $uptimer >> $TOF
fi
#Processings
echo -n "Processes running: " >> $TOF
str=$(ps | wc -l)
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
				255.0.0.0    ) echo "8"  >> $TOF;;
				255.255.0.0  ) echo "16" >> $TOF;;
				255.255.255.0) echo "24" >> $TOF;;
				*            ) echo "-"  >> $TOF;;
			esac
		fi
	fi
done

echo "----\"EOF\"----" >> $TOF

exit 0
