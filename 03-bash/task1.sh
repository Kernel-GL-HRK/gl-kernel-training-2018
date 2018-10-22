#!/bin/bash

#########################
#       GLOBAL VAR      #
#########################

local_lang=$(locale | grep -w LANG | cut -f1 -d "_" | sed 's/LANG=/''/')

#########################
#        Functions      #
#########################

get_info() {
	#tmpfile=$(mktemp)  # Create a temporal file in the default temporal folder of the system

	#Hardware, System, Network
	#Date: Sat, 07 Jul 2018 16:18:06 +0300
	date -R 1>>$tmpfile

	#---- Hardware ----
	echo ---- Hardware ---- 1>>$tmpfile
	#CPU: "Intel(R) Celeron(R) CPU E1400 @ 2.00GHz"
	echo $(sudo dmidecode -t processor | grep Version: | sed 's/Version:/CPU:/') 1>>$tmpfile
	#RAM: 2048 MB
	echo $(sudo dmidecode -t memory | grep Size: | sed 's/Size:/RAM:/') 1>>$tmpfile
	#Motherboard: "ASUSTeK Computer INC", "P5KPL-AM/PS"
	echo $(sudo dmidecode -t baseboard | grep Manufacturer: | sed 's/Manufacturer:/Motherboard:/')', '$(sudo dmidecode -t baseboard | grep 'Product Name:' | sed 's/Product Name://') 1>>$tmpfile
	#System Serial Number: MT7089K12013428
	echo $(sudo dmidecode -t baseboard | grep 'Serial Number:') 1>>$tmpfile

	#---- System ----
	echo ---- System ---- 1>>$tmpfile
	#OS Distribution: "CentOS release 6.10 (Final)"
	echo 'OS Distribution: '$(uname -o) 1>>$tmpfile
	#Kernel version: 2.6.32-754.el6.x86_64
	echo 'Kernel version: '$(uname -r) 1>>$tmpfile
	#Installation date: Wed Sep 9 14:35:22 2015
	echo 'Installation date: '$(sudo head -n1 /var/log/installer/syslog | cut -f1 -d"u") 1>>$tmpfile
	#Hostname: ws267.se.nure.ua
	echo 'Hostname: '$(hostname) 1>>$tmpfile
	#Uptime: 1:49
	echo 'Uptime: '$(uptime -p) 1>>$tmpfile
	#Processes running: 194
	echo 'Processes running: '$(ps aux --no-heading | wc -l) 1>>$tmpfile
	#User logged in: 3
	echo 'User logged in: '$(users | wc -w) 1>>$tmpfile

	#---- Network ----
	echo ---- Network ---- 1>>$tmpfile
	#lo: 127.0.0.1/8
	#eth0: 192.168.123.231/24
	iface_arr=($(ls /sys/class/net | tr " " "\n"))
	for interface in "${iface_arr[@]}"; do
		ipadress=$(ip addr show dev $interface | grep "inet " | awk '{ print $2 }')
		if [ "$ipadress" == '' ]; then
			ipadress='-/-'
		fi
		echo $interface': '$ipadress 1>>$tmpfile
	done

	#----"EOF"----
	echo 'EOF' >>$tmpfile
}
sort_file() {
   #If a FileName exists, rename it sort_file [Num, File]
	sort_file=$1
	search_dir=${sort_file%/*}
	DATE=`date +%Y%m%d`
	if [ -f $1 ]; then
		i=0
		after_trim='0000'
		for entry in `ls $search_dir`; do
			if [ -f $1-$DATE-$after_trim ]; then
				true $(( i++ ))
				before_trim='0000'$i
				after_trim=${before_trim:$((${#before_trim}-4))}
			else
				mv $1 $1-$DATE-$after_trim
				break
			fi
		done
	fi
}
write_file() {
	if [ ! -d $1 ]; then
		mkdir -p $1 2>&1
	fi

	if [ ! -d $1 ]; then
		exit 1
	fi
	touch $1'/'$2
	cat $tmpfile > $1'/'$2
}
delete_file() {
	in_file=$1
	clean_dir=${in_file%/*}
	in_file_name=${file##*/}
	DATE=`date +%Y%m%d`
	i=1
	for entry in `ls -t $clean_dir | grep "$in_file_name-$DATE-"`; do
		if [ "$i" -gt "$2" ]; then
			rm $clean_dir'/'$entry
		fi
		true $(( i++ ))
	done

}
display_help() {
	case "$local_lang" in
		en )
			echo "Usage: ./task1.sh [-h|--help] [-n num] [-f file]"
			echo " where: "
			echo "   num -- numbers of file with results,"
			echo "   file -- path and filename, you like to save the result;"
			echo "   Example: ./task1.sh -n 9 ./test/info/info.txt"
		;;
		uk )
			echo "Як користуватись: ./task1.sh [-h|--help] [-n num] [-f file]"
			echo " де: "
			echo "   num -- кількість файлів із результатами,"
			echo "   file -- шлях та ім'я файла, у який треба записати результат;"
			echo "   Наприклад: ./task1.sh -n 9 ./test/info/info.txt"
		;;
	esac
	exit 1
}

error_exit()
{
	echo "$1" 1>&2
	exit 1
}

################################
# Check if parameters options  #
# and aasign dfault variables  #
################################

case "$1" in
	-n )
		case "$#" in
			2 | 3)
				integerS2=1
				if [ "$2" -eq $(expr "$2" + 0) ]; then
					integerS2=0
				fi
				if [ "$2" -lt "1" ] || [ "$integerS2" -eq "1" ]; then
					case "$local_lang" in
						en )
							error_exit "Error: Nuber is less then 1 or not integer! Aborting."
						;;
						uk )
							error_exit "Помилка: Число менше 1 чи не цiле! Вихiд."
						;;
					esac
				else
					file_num=$2
				fi
				if [ -z "$3" ]; then
					file="$HOME/bash/task1.out"
				else
					file=$3
				fi
			;;
			* )
				case "$local_lang" in
					en )
						error_exit "Error: Incorrect imput! Aborting."
					;;
					uk )
						error_exit "Помилка: Не коректний ввiд! Вихiд."
					;;
				esac
			;;
		esac
		sort_file $file
		delete_file $file $2
	;;
	-h | --help)
		display_help  # Call your function
		exit 0
	;;
	-*)
		case "$local_lang" in
		en )
			error_exit "Error: Unknown option! Aborting."
		;;
		uk )
			error_exit "Помилка: Невизначена опцiя! Вихiд."
		;;
		esac
	;;
	*)  # No more options
		file_num=1
		file="$HOME/bash/task1.out"
	;;
esac

#########################
#           MAIN        #
#########################

file_name=${file##*/}
file_path=${file%/*}

tmpfile=$(mktemp)

get_info
#display_help # - for debuging

# write_file [file path] [file name]
sort_file $file
write_file $file_path $file_name

#cat $file_path'/'$file_name # - for debuging

rm "$tmpfile"
