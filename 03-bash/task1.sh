#! /bin/bash -x
#
# lesson_3 homework bash task1.sh
#

FILE=~/bash/task1.out
DIR_F=$(dirname "$FILE")

#  Check command line parameters
if [ "$#" -gt 4 ] ; then 
	echo "ERROR arguments more then 3" >$2
	exit 1 
fi

# processing command line parameters
# ./task1.sh [-h|--help] [-n num] [file] 
help=0
num=1

while [[ -n $1 ]]; do
	case $1 in 
		-h | --help)	
			help=1	;;
		-n)	
			num=$2
			shift 	;;
		*)	
			FILE=$1;;
	esac
	shift
done

# Check parameter num
while [ $num -lt 1 ] || [ -z $num ]; do 
	echo "./task1.sh [-h|--help] [-n num] [file]"	
	read -p "Enter num >= 1 :" num  	
done	

# Help info key: -h or --help
if [ $help -eq 1 ]; then 
	cat <<- _EOF_
	./task1.sh [-h|--help] [-n num] [file]
	where:
	num - number of file whith results  	
	file - path and name of the file in which you want to record the result 
	eg: task.sh -n 9 /test/info/info.txt
	_EOF_
	exit 0
else help=0
fi


# Delete old file 
file_count=$( ls -l $DIR_F | grep -c task )

while [ $file_count -ge $num ]; do
	echo "delete old file: $DIR_F/ $(ls -rtl $DIR_F | awk '{print $9}'| sed -n '2p')"	
	$(cd $DIR_F ;
	$(find -name ""| ls -rtl | sed -n '2p' | awk '{print $9}'| xargs rm -f))
	file_count=$((--file_count))
done

# Creating files and directories by date and number 
if [ -e $FILE ]; then 
	DATA=`date +%y%m%d`	
	true $((++i))
	count=$(printf %04d $i)

	touch ${FILE%.*}-$DATA-$count.${FILE#*.}
	FILE=${FILE%.*}-$DATA-$count.${FILE#*.}
else
	$(mkdir -p "$DIR_F" && touch "$FILE")
fi

# parameter checking function for Hardware info  
check_param(){
if [ -z $1 ]; then 
	echo "Unknow"
else 
	echo $@
fi
}

#Date:
echo "Date: $(date +'%a, %d %b %Y %T %z')" >>$FILE

# --------------- Hardware ---------------
echo "---- Hardware ----" >>$FILE

#CPU
	CPU=$(dmidecode -s processor-version | sed -n '1p')
	echo "CPU: \"$(check_param $CPU)\"" >>$FILE

#RAM
	RAM=$(dmidecode -t memory | grep 'Size: [0-9]' | sed 's/\tSize://')
	echo "RAM:$(check_param $RAM)" >>$FILE

#Motherboard
	BMAN=$(dmidecode -s baseboard-manufacturer | sed -n '1p'); 
	BPN=$(dmidecode -s baseboard-product-name | sed -n '1p');  
	echo "Motherboard: \"$(check_param $BMAN)\", \"$(check_param $BPN)\"" >>$FILE

#System Serial Number
	SN=$(dmidecode -s system-serial-number | sed -n '1p')
	echo "System Serial Number:$(check_param $SN)" >>$FILE

# --------------- System ---------------
echo "---- System ----" >>$FILE

#Distribution name
	echo "OS Distribution: $(cat /etc/os-release | grep PRETTY_NAME= |
sed 's/PRETTY_NAME=//')" >>$FILE

#Kernel Version
	echo "Kernel version: $(uname -r)" >>$FILE

#Install Date
	echo "Installation date: $(tune2fs -l $(df / | tail -1 | 
cut -f1 -d' ') | grep created: | sed 's/Filesystem created:       //')" >>$FILE

#Hostname
	echo "Hostname: $(hostname)" >>$FILE

#Uptime
	echo "Uptime: $(uptime -p)" >>$FILE

#Processes running, without threads
	echo "Processes running: $( ps -A --no-headers | wc -l)" >>$FILE

#Number of users
	echo "User logged in: $(users | wc -w)" >>$FILE
	
# --------------- Network ---------------
echo "---- Network ----" >>$FILE

for net_inter in $(ifconfig | cut -d" " -f1 | awk NF | cut -d":" -f1); do
ip_adr=$(ip -f inet address show $net_inter | grep inet | awk '{ print $2 }')
if [ -z $ip_adr ]; then
	echo "$net_inter: -/-" >>$FILE
else
        echo "$net_inter: $ip_adr" >>$FILE
fi
done
echo "----\"EOF\"----" >>$FILE
   
echo "file is written to: $FILE"
