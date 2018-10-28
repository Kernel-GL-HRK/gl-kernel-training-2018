#!/bin/bash

usage()
{   
    echo "Usage: ./task1.sh [-key]… [value]… [file_result]"
    echo "Example:"
    echo "./task1.sh  -n 9 ./test/info/info.txt"
    echo -e  "\n"
    echo "Selection key and its interpretation:"
    echo -e "\t-h, --help \t\t show help and finish work"
    echo -e "\t-n [=num] [=file] \t [=num] number of files with results"
    echo -e "\t\t\t\t [=file] file path to record the result"
}
isInteger() 
{
	[[ $1 =~ ^[+-]?[0-9]+$ ]]
}


ValidationParam()
{
	
	if ! isInteger $numfile; then
		echo "Input ERROR: $1 must be an integer">&2
		exit 1
	fi

	if ! [ "$1" -gt 1 ];then
		echo "Input ERROR: $1 must be greater than 1">&2
		exit 1
	fi
}

#parse args
while [ -n "$1" ]
do
case "$1" in
-h | --help) 
		usage
        exit 0
        ;;

-n) 	numfile="$2"
		pathFile="$3"
		break
		;;

*)  	echo "$1 is unknown parameter. Please run ./task1.sh -h for information on supported options">&2
		exit 1
		;;
esac
done

ValidationParam $numfile