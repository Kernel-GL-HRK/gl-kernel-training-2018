#!/bin/bash

help_usage(){
	echo "Usage ./task2.sh [-h|--help] [file] "
	echo "-h, --help - number of resulting file"
	echo "file       - file name with obtained hardware info"
	echo "For example, ./task2.sh ./test/info/task1.sh"
}


chk_work_dir(){
	if ! [[ -d "$1" ]]; then
	mkdir -p $1
		if ! [ -d "$1" ]; then
			>&2 echo "Error, working directory $1 was not created. Exit  with error."
			exit 1
		fi
	else
		echo "Directoy $1 is exists."
	fi
}


INSTALL_DIR="/usr/local/bin"
INSTALL_FILE="task1.sh"

if [[ -z "$1" ]] ; then
	INSTALL_FILE=$1
fi	

chk_work_dir $INSTALL_DIR

cp $INSTALL_FILE $INSTALL_DIR

chmod u+s $INSTALL_DIR/$INSTALL_FILE

chmod 755 $INSTALL_DIR/$INSTALL_FILE
