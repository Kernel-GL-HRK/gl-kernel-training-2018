#!/bin/bash

declare -i LOCALE

is_integer () {
	local re='^[0-9]+$'
	[[ $1 =~ $re ]]
	return $?
}

integer_to_zero_string () {
	local ret
	printf -v ret "%04d" $1
	echo "$ret"
}

check_locale() {
	local re=$(locale | grep 'LANG=uk_UA')
	if [ -z "$re" ]; then
		LOCALE=0
	else
		LOCALE=1
	fi
}

exit_with_help () {
	if (($LOCALE == 1)); then
		echo 'Usage_UKR:'
		echo '  ./task1.sh [-h|--help] [-n num] [file]'
		echo '     num --  amount of files'
		echo '     file -- path to file to store results'
	else
		echo 'Usage:'
		echo '  ./task1.sh [-h|--help] [-n num] [file]'
		echo '     num --  amount of files'
		echo '     file -- path to file to store results'
	fi
	exit $1
}

err_not_a_number() {
	if (($LOCALE == 1)); then
		echo "Error_UKR: $2 is not a number" >&2
	else
		echo "Error: $2 is not a number" >&2
	fi
	exit_with_help 1
}

err_greater_than() {
	if (($LOCALE == 1)); then
		echo "Error_UKR: $1 should be greater than 1" >&2
	else
		echo "Error: $1 should be greater than 1" >&2
	fi
	exit_with_help 1
}

err_unsupported_flag() {
	if (($LOCALE == 1)); then
		echo "Error_UKR: Unsupported flag $1" >&2;
	else
		echo "Error: Unsupported flag $1" >&2;
	fi
	exit_with_help 1
}

err_extra_parameter() {
	if (($LOCALE == 1)); then
		echo "Error_UKR: Extra paramemter $1" >&2;
	else
		echo "Error: Extra paramemter $1" >&2;
	fi
	exit_with_help 1
}

err_failed_to_create_directory() {
	if (($LOCALE == 1)); then
		echo "Error_UKR: Failed to create directory" >&2;
	else
		echo "Error: Failed to create directory" >&2;
	fi
	exit_with_help 1
}

#default values
declare -i N=0
FILE=~/bash/task1.out

check_locale

#parse parameters
while (( "$#" )); do
	case "$1" in
		-h|--help) #help
		exit_with_help 0
		;;

	-n) #files count
		#re='^[0-9]+$'
		#if ! [[ $2 =~ $re ]]; then
		if ! is_integer $2; then
			err_not_a_number $2
		elif ! (( $2 > 1 )); then
			err_greater_than $2
		fi
		N=$2
		shift 2
		;;

	-*|--*=) # unsupported flags
		err_unsupported_flag $1
		;;

	*) # preserve positional arguments
		FILE=$1
		shift
		if [[ "$1" != "" ]]; then
			err_extra_parameter $1
		fi
		;;
	esac
done


DIRECTORY=`dirname "$FILE"`
FILE_NAME=`basename "$FILE"`
DATE=`date '+%Y%m%d'`

#create directory if needed
if [ ! -d "$DIRECTORY" ]; then
	echo "Info: Created directory $DIRECTORY"
	mkdir "$DIRECTORY"
	if [ "$?" != '0' ]; then
		err_failed_to_create_directory
	fi
fi

#clear old files if needed
declare -i count=`ls -la $DIRECTORY | grep $FILE_NAME-$DATE | wc -l`
declare -i index
echo "Info: amount of files in directory: $count"
if (( count > N-1 && N != 0 )); then
	for (( i=$N; i<=$count; i++ ))
	do
		num=$(integer_to_zero_string $i)
		echo "Info: remove file $DIRECTORY/$FILE_NAME-$DATE-$num"
		rm "$DIRECTORY"/"$FILE_NAME-$DATE-$num"
	done
	index=$N
else
	index=$count+1
fi

#rename last file
if [ -f "$FILE" ]; then
	num=$(integer_to_zero_string $index)	
	echo "Info: Rename file $FILE to $FILE-$DATE-$num"
	mv "$FILE" "$FILE-$DATE-$num"
fi

#fill in file
echo "---- Hardware ----" > $FILE
echo $(sudo dmidecode -t processor | grep 'Version' | sed 's/Version: /CPU: \"/')\" >> $FILE
echo $(sudo dmidecode -t memory | grep 'Size: [0-9]' | sed 's/Size/RAM/') >> $FILE
echo $(sudo dmidecode -t baseboard | grep 'Version:' | sed 's/Version: /Motherboard: \"/')\", \
	$(sudo dmidecode -t baseboard | grep 'Product Name: ' | sed 's/Product Name: /\"/')\">> $FILE
echo $(sudo dmidecode -t system | grep 'Serial Number' | sed 's/Serial Number/System Serial Number/')>> $FILE

echo "---- System ----" >> $FILE
echo "OS Distribution: "$(uname -o) >> $FILE
echo "Kernel version: "$(uname -r) >> $FILE
echo "Installation date: "$(sudo head -n1 /var/log/installer/syslog | cut -f1 -d"u") >> $FILE
echo "Hostname: "$(hostname) >> $FILE
echo "Uptime: "$(uptime -p) >> $FILE
echo "Processes running: "$(ps aux --no-heading | wc -l) >> $FILE
echo "User logged in: "$(users | wc -w) >> $FILE

echo "---- Network ----" >> $FILE
for iface in $(ifconfig | cut -d ' ' -f1| tr ':' '\n' | awk NF)
do
	ip=$(ip -f inet address show $iface | grep "inet " | awk '{ print $2 }')
		if [ "$ipadress" == '' ]; then
			ipadress='-/-'
		fi
	echo "$iface: $ip" >> $FILE
done

echo "---- \"EOF\" ----" >> $FILE

