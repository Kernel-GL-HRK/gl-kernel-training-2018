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

function create_dir_if_not_exists_for_file()
{
	dir_path=$(dirname "$1")
	if ! [ -d "$dir_path" ]
	then
		echo "dir %dir_path non exists, create new one..."
		mkdir -p "$1"
	fi
}

function rename_old_file()
{
	if [ -z "$1" ]; then
		echo "fatal error rename_old_file"
		exit -2;
	fi

	local f_name=$(basename "$1")
	local file_ext=$([[ "$f_name" = *.* ]] && echo ".${f_name##*.}" || echo '')
	local f_name="${f_name%.*}"
	local dir_path=$(dirname "$1")

	local cur_date=$(date +%Y%m%d)

	#find all files that were created today 
	local file_last=$(ls 2>/dev/null $dir_path/$f_name-$cur_date-*$file_ext | sort -r | head -1)
   	local lastid=$(echo "$file_last" | cut -d "-" -f3 | cut -d "." -f1)
	
	lastid=$((lastid+1))

	#rename existingfile

	local new_file_name="$dir_path/$f_name-$cur_date-$lastid$file_ext"

	mv "$1" "$new_file_name"

	echo " old report $1  was renamed to $new_file_name"	
}

function clean_up_working_dir()
{
	#$1 num files
	#$2 file path

	if [ [ -z "$1" ] or [ -z "$2"] ] ; then
		echo "fatal error clan_up_working_dir"
		exit -2;
	fi

	local file_num="$1";
	local f_name=$(basename "$2")
	local f_name="${f_name%.*}"
	local dir_path=$(dirname "$2")
	local cur_date=$(date +%Y%m%d)


	local files_to_be_deleted=($(ls 2>/dev/null $dir_path/$f_name-$cur_date-* | sort))
	local total_number_of_files=${#files_to_be_deleted[@]}

	for (( i=0; i<$total_number_of_files-$file_num; i++ ));
	do
		local file_will_be_deleted=${files_to_be_deleted[$i]}

		read -p "$file_will_be_deleted  will be deleted press ENTER to continue"
     	rm "$file_will_be_deleted"
    done

}

function safe_create_result_file()
{
	if [ -z "$1" ]
	then
		result_file='~/bash/task1.out'
	else
		result_file="$1"
	fi

	if ! [ -e "$result_file" ]; then
		create_dir_if_not_exists_for_file		
	else
		rename_old_file "$result_file"
	fi

	file_path="$result_file"


}

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

safe_create_result_file "$file_path"

if [ -n  "$num_files" ]
then
	clean_up_working_dir "$num_files" "$file_path"
fi

print_system_info "$file_path"
