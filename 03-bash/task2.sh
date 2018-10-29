#!/bin/bash

integer_to_zero_string () {
	local ret
	printf -v ret "%04d" $1
	echo "$ret"
}

declare -i LOCALE

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
		echo '  ./task2.sh [-h|--help]'
	else
		echo 'Usage:'
		echo '  ./task2.sh [-h|--help]'
	fi
	exit $1
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

check_locale

#parse parameters
while (( "$#" )); do
	case "$1" in
		-h|--help) #help
		exit_with_help 0
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

FILE='/usr/local/bin/task1.sh'
DIRECTORY=`dirname "$FILE"`
FILE_NAME=`basename "$FILE"`
DATE=`date '+%Y%m%d'`

#create directory if needed
if [ ! -d "$DIRECTORY" ]; then
	echo "Info: Created directory $DIRECTORY"
	sudo mkdir "$DIRECTORY"
	if [ "$?" != '0' ]; then
		err_failed_to_create_directory
	fi
fi

#copy file to directory and make it read-only
sudo cp "$FILE_NAME" "$FILE"
sudo chmod uog+rx-w $FILE

#backup last enviroment file
CONF_DIRECTORY='/etc'
CONF_FILE='environment'
declare -i count=`ls -la $CONF_DIRECTORY | grep $CONF_FILE-$DATE | wc -l`
declare -i index=$count+1
echo "$CONF_DIRECTORY/$CONF_FILE"
if [ -f "$CONF_DIRECTORY/$CONF_FILE" ]; then
	num=$(integer_to_zero_string $index)	
	echo "Info: Rename file $CONF_DIRECTORY/$CONF_FILE to $CONF_DIRECTORY/$CONF_FILE-$DATE-$num"
	sudo cp "$CONF_DIRECTORY/$CONF_FILE" "$CONF_DIRECTORY/$CONF_FILE-$DATE-$num"
fi

#update enviroment file
STR=$(cat /etc/environment | grep $FILE)
if [ "$STR" == '' ]; then
	sudo sed -e "s|PATH=\"\\(.*\\)\"|PATH=\"$FILE:\\1\"|g" -i /etc/environment
fi

