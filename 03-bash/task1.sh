#!/bin/bash  -x

clear

function get_locale_name (){
 	local locale_name=$(locale | grep "LC_NAME=")
 	expr "$locale_name" : '\S*=\([^.]*\)'
}

function print_help(){	
	echo "./task1.sh [-h|--help] [-n num] [file]"
	if [[ $(get_locale_name) = "uk_UA" ]]
	then
		>&2 echo "де 'num' - кількість файлів із результатами,"
		>&2 echo "'file' - шлях та ім'я файла, у який треба записати результат"
		>&2 echo "наприклад: task1.sh -n 9 ./test/info/info.txt"
	else
		>&2 echo "where 'num' - number result files"
		>&2 echo "'file' - path to and  result file name "
		>&2 echo 'e.g. task1.sh -n 9 ./test/info/info.txt'
	fi
}
function print_invalid_arguments_number()
{
	if [[ $(get_locale_name) = "uk_UA" ]]
	then
		>&2 echo "некоректний параметр"
	else
		>&2 echo "invalid parameter"
	fi
}

function check_num_files()
{
	re='^[0-9]+$'
	if ! [[ $1 =~ $re ]]; then
  		print_invalid_arguments_number
  		exit -1;
  	fi

  	if (( $1 < 1 )); then 
  		print_invalid_arguments_number
  		exit -1;
  	fi
}

function print_system_info()
{
	#$1  filename
	#	---- Hardware ----
	#CPU: "Intel(R) Celeron(R) CPU E1400 @ 2.00GHz"
	#RAM: 2048 MB
	#Motherboard: "ASUSTeK Computer INC", "P5KPL-AM/PS"
	#System Serial Number:
	>>"$1" echo $(date "+Date: %a, %d %m %Y -%H:%M:%S %z")
	>>"$1" echo "============Hardware==================="
	>>"$1" echo $(sudo dmidecode -s  processor-version)
	    for mem in $( sudo dmidecode -t memory | grep -Ei 'Size:\s+[0-9]+\s+MB' | grep -oEi '[0-9]+' )
        do
            let total_mem=total_mem+$mem
        done
	>>"$1" echo "RAM: $total_mem MB"
	     MotherboardProducer=$( sudo dmidecode -s chassis-manufacturer )
	     MotherboardType=$( sudo dmidecode -s  baseboard-product-name )

	>>"$1" echo Motherboard: "\"$MotherboardProducer\", \"$MotherboardType\""
	>>"$1" echo "System Serial Number: $( sudo dmidecode -s  baseboard-serial-number )"

	#---- System ----
	#OS Distribution: "CentOS release 6.10 (Final)"
	#Kernel version: 2.6.32-754.el6.x86_64
	#Installation date: Wed Sep 9 14:35:22 2015
	#Hostname: ws267.se.nure.ua
	#Uptime: 1:49
	#Processes running: 194
	#User logged in: 3
	>>"$1" echo "---- System ----"
	>>"$1" echo "OS Distribution: $( cat /etc/*release | grep DISTRIB_DESCRIPTION= | sed 's/DISTRIB_DESCRIPTION=//' )"
    >>"$1" echo "Kernel version: $( uname -r ) "
    >>"$1" echo "Installation date: $( stat -c %z /var/log/installer/syslog )"
    >>"$1" echo "Hostname: $( hostname )"
    >>"$1" echo "Uptime: $( uptime -p )" 
    >>"$1" echo "Processes running: $( ps -ef | wc -l )"
	>>"$1" echo "User logged in: $(users | wc -w)"

	#---- Network ----
	#lo: 127.0.0.1/8
	#eth0: 192.168.123.231/24
	>>"$1" echo "---- Network ----"

	for link_name in $( ip link show  | grep -oEi "^[0-9]+:\s*\S+\:" | cut -f2 -d":" )
    do
	    link_status=$(ip -f inet address show $link_name | grep inet | awk '{ print $2 }')
        if [ ! -z $link_status ]
        then
           >>"$1" echo "$link_name: $link_status"
        else
           >>"$1"  echo "$link_name: -/-"
        fi
	done

	#----"EOF"----
	>>"$1" echo "----\"EOF\"----"
}


num_files="0"
file_path=""

#parse command line 
#./task1.sh [-h|--help] [-n num] [file]
while (( "$#" )); do 
  		if [[ "$1" = '-h' || "$1" = '--help' ]]
  		 then
			print_help
  			exit 0
  		fi

  		if [[ $1 = -n && $# > 2 ]]; then
  			num_files="$2"
  			check_num_files "$num_files"
  			shift; shift; continue;
  		fi

  		file_path="$1"

  shift 
done

echo "num_files $num_files"
print_system_info "$file_path"

