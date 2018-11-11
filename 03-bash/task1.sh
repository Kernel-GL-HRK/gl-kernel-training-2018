#!/bin/bash
# Version 1.0.0

# print help information on to a screen
help_usage() {
	echo "Usage ./task1.sh [-h|--help] [-n num] [file] "
	echo "-h, --help - number of resulting file"
	echo "-n         - number of files with hardware info"
	echo "file       - file name with obtained hardware info"
	echo "For example, ./task1.sh -n 9 ./test/info/info.txt"
}

# check and create work directory
chk_work_dir() {
	if ! [ -d "$1" ]; then
		mkdir -p $1
		if ! [ -d "$1" ]; then
			echo >&2 "Error, working directory $1 was not created. Exit with error."
			exit 1
		fi
	fi
	if ! [ -f "$1/.value" ]; then
		echo 0 >"$1/.value"
	fi
}

# rename file
rename_file() {
	# файл-yyyymmdd-nnnn
	if [ -f "$1" ]; then
		mv "$1" "$1_$(date +%Y%m%d)_$2"
	fi
}

chk_is_number() {
	local re="^[1-9][0-9]{0,}$"
	if [ "$1" =~ $re ]; then
		return 0
	else
		return 1
	fi
}

parse_number() {
	local m_number=$2
	if chk_is_number $1 -eq 0; then
		eval $m_number=$1
	fi
}

parse_file_name() {
	local m_filename=$2
	local m_dirname=$3
	if ! [ -z "$1" ]; then
		eval $m_filename=$(basename $1)
		eval $m_dirname=$(dirname $1)
	fi
}

inc_value_in_file() {
	local cnt=0
	if [ -f "$1" ]; then
		read cnt <$1
		# echo "debug: in file stored: $COUNTER"
		echo $((cnt + 1)) >$1
	fi
}

get_current_value() {
	if [ -f "$1" ]; then
		read $2 <$1
		inc_value_in_file $1
	fi
	# echo "debug: get_current_value:: COUNTER: ${!2}"
	return 0
}

remove_files() {
	# $1 - dir
	# $2 - cnt
	if [ -d "$1" -a "$2" -gt 0 ]; then
		cd $1
		local cnt=$(ls $1 | wc -l)
		cnt=$( (cnt - $2) )
		cnt=${cnt#-}
		echo "cnt >> $cnt"
		ls $1 | sort -r | tail -$cnt | xargs rm
	fi
}

make_info_file() {
	local info_file=$1
	local m_date=$(date)
	echo "Date: $m_date" >$info_file
	echo "---- Hardware ----" >>$info_file
	local m_proc=$(cat /proc/cpuinfo | grep 'model name' | tail -n 1 | cut -d':' -f2- | xargs)
	echo "CPU: \"$m_proc\"" >>$info_file
	# local m_proc=$(sudo dmidecode -t processor | grep Version |  cut -d':' -f2- | xargs)
	local m_mem=$(cat /proc/meminfo | grep 'MemTotal:' | cut -d':' -f2 | xargs | cut -d' ' -f1)
	m_mem=$( (m_mem / 1024) )
	echo "RAM: $m_mem MB" >>$info_file
	local m_pr_name=$(sudo dmidecode -t baseboard | grep 'Product Name' | cut -d':' -f2- | xargs)
	local m_pr_mb=$(sudo dmidecode -t baseboard | grep 'Manufacturer' | cut -d':' -f2- | xargs)
	echo "Motherboard: $m_pr_mb, \"$m_pr_name\"" >>$info_file
	local m_serial=$(sudo dmidecode -t2 | grep 'Serial Number' | cut -d':' -f2 | xargs)
	echo "System Serial Number: $m_serial" >>$info_file
	echo "---- System ----" >>$info_file
	local m_release=$(lsb_release -d | cut -d':' -f2- | xargs)
	echo "OS Distribution: \"$m_release\"" >>$info_file
	local m_kernel=$(uname -v | awk '{print $4}')
	echo "Kernel version: $m_kernel" >>$info_file
	local m_install_date=$(/bin/ls -lGct /etc | tail -1 | awk '{print $5, $6, $7}')
	echo "Installation date: $m_install_date" >>$info_file
	local m_host=$(hostname)
	echo "Hostname: $m_host" >>$info_file
	local m_uptime=$(uptime | awk '{print $3}' | cut -d',' -f1)
	echo "Uptime: $m_uptime" >>$info_file
	local m_proc_cnt=$(ps aux | wc -l)
	echo "Processes running: $m_proc_cnt" >>$info_file
	local m_user_cnt=$(who -u | wc -l) # not work in debian 686?
	echo "User logged in: $m_user_cnt" >>$info_file
	echo "---- Network ----" >>$info_file
	local m_ifaces=$(ip addr show | grep "inet\b" | awk '{print $(NF) ": " $2}')
	echo "$m_ifaces" >>$info_file
	echo '----"EOF"----' >>$info_file
}

# variables
RES_DIR="$HOME/bash"
RES_FILE="$RES_DIR/task1.out"

COUNTER=0
U_NUMBER=0
U_FILENAME=""
U_DIRNAME=""

while [ "$1" != "" ]; do
	case $1 in
		-h | --help)
			help_usage
			exit 0
			;;
		-n)
			parse_number $2 U_NUMBER
			shift
			;;
		-* | --*)
			echo "Error, a parameter is wrong!"
			help_usage
			exit 0
			;;
		*)
			parse_file_name $1 U_FILENAME U_DIRNAME
			break
			;;
	esac
	shift
done

if ! [ -z "$U_FILENAME" ]; then
	RES_FILE=$U_DIRNAME/$U_FILENAME
	RES_DIR=$U_DIRNAME
fi

COUNTER_FILE="$RES_DIR/.value"

chk_work_dir $RES_DIR

get_current_value "$RES_DIR/.value" COUNTER

rename_file $RES_FILE $(printf "%04d" $COUNTER)

make_info_file $RES_FILE

remove_files $RES_DIR $U_NUMBER

echo "The file \"$RES_FILE\" was created. Done."

exit 0
