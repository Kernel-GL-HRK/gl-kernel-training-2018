#!/bin/bash

PARAM_N=1
FILE_PATH=~/bash_out/task1.out
FLAG_INSIDE=0

function get_hw_info()
{
    echo "---- Hardware ----" >> $FILE_PATH

    echo "" >> $FILE_PATH
}

function get_sys_info()
{
    echo "---- System ----" >> $FILE_PATH
    echo "" >> $FILE_PATH
}

function get_network_info()
{
    echo "---- Network ----" >> $FILE_PATH
    echo "" >> $FILE_PATH
}

function create_output_file()
{
   $(mkdir -p "$(dirname "$FILE_PATH")" && touch "$FILE_PATH")
}

function usage()
{
    echo
    echo "USAGE: task1.sh [-h|--help] [-n num] [file]"
    echo 
    echo "-h|--help:  this page,"
    echo "-n num:     count of output files,"
    echo "file:       the output file location, default ~/bash/task1.out"
    echo
    echo
}

function set_count()
{
    if [[ "$1" =~ ^[0-9]+$ ]]
    then
        if [ "$1" -ge 1 ]
        then
            PARAM_N=$1
        else
            echo "-n parameter must be greter or equal 1" >&2
            exit 1
        fi
    else
        echo "-n parameter must be an integer value" >&2
        exit 1
    fi
}

function set_path()
{
    FILE_PATH=$1
}

function parse_args()
{
#Check count of arguments. It can't be greater than 4
    if [ "$#" -gt 3 ]
    then
        echo "Too many arguments"
        exit 1
    fi

    while [ -n "$1" ]
    do
        case "$1" in
            -h|--help)  usage
                        exit 0;;
            -n)         set_count "$2"
                        shift;;
            *)          set_path "$1";;
        esac
        shift
    done
}

function main()
{
    parse_args "$@"

    create_output_file

    echo $(date -R) > $FILE_PATH
    echo "" >> $FILE_PATH
    get_hw_info
    get_sys_info
    get_network_info
    echo "---- \"EOF\" ----" >> $FILE_PATH




    echo "Param n = $PARAM_N"
    echo "File path = $FILE_PATH"
}

if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

main "$@"