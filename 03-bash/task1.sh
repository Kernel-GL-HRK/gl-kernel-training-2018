#!/bin/bash
PARAM_N=1
FILE_PATH=~/bash/task1.out
LOCALE=US

function usage()
{
    echo
    echo "USAGE: task1.sh [-h|--help] [-n num] [file]"
    echo 
    echo "-h|--help:  this page,"
    echo "-n num:     output files to be left"
    echo "file:       the output file location, default ~/bash/task1.out"
    echo
}

function printLocalizedMessages()
{
    if [[ "$LOCALE" == UA ]]; then
    case "$1" in
        "MSG1") echo "-n параметр повинен доривнювати або бути бiльше за одиницю" >&2;;
        "MSG2") echo "-n параметр повинен бути цiлим числом" >&2;;
        "MSG3") echo "Забагато аргументiв" >&2;;
        "MSG4") echo "Не вдалося створити папку виводу" >&2;;
        "MSG5") echo "Не вдалося створити файл" >&2;;
    esac
    else
    case "$1" in 
        "MSG1") echo "-n parameter must be equal or greater than 1" >&2;;
        "MSG2") echo "-n parameter must be an integer value" >&2;;
        "MSG3") echo "Too many arguments" >&2;;
        "MSG4") echo "Failed to create output folder" >&2;;
        "MSG5") echo "Failed to create output file" >&2;;
    esac;
    fi
}

function Locale()
{
    if [ "$LANG" = "uk_UA.UTF-8" ]
    then
        LOCALE=UA
    fi
}

function checkN() 
{
    if [[ "$1" =~ ^[0-9]+$ ]]
    then
        if [ "$1" -ge 1 ]
        then
            PARAM_N=let "$1-1"
        else
            printLocalizedMessages "MSG1"
            exit 1
        fi
    else
        printLocalizedMessages "MSG2"
        exit 1
    fi
}

function args()
{
    if [ "$#" -gt 3 ]
    then
        printLocalizedMessages "MSG3"
        exit 1
    fi
    while [ -n "$1" ]
    do
        case "$1" in
            -h|--help)  usage
                        exit 0;;
            -n)         PARAM_N=checkN $2
                        shift;;
            *)          FILE_PATH=$1;;
        esac
        shift
    done
}

function getHWINFO()
{
    local result

    set -o pipefail

    echo -e "---- Hardware ----"  > ${FILE_PATH}
    echo -e "CPU: \"$(dmidecode -s processor-version || echo "Unknown")\"" >> ${FILE_PATH}
    echo -e $(dmidecode -t 17 | awk '( /Size/ && $2 ~ /^[0-9]+$/ ) { x+=$2 } END{ print "RAM: " x "MB" }' || echo "RAM: Unknown") >> ${FILE_PATH}
    echo -e "Motherboard: \"$(dmidecode -s baseboard-manufacturer || echo "Unknown")\", \"$(dmidecode -s baseboard-product-name || echo "Unknown")\"" >> ${FILE_PATH}
    echo -e "System Serial Number: $(dmidecode -s system-serial-number || echo "Unknown")" >> ${FILE_PATH}
    echo -e "---- System ----" >> ${FILE_PATH}
    echo -e "OS Distribution: $(cat /etc/*-release | grep -i description | sed 's\DISTRIB_DESCRIPTION=\\g' || echo "Unknown")" >> ${FILE_PATH}
    echo -e "Kernel version: $(uname -r || echo "Unknown")" >> ${FILE_PATH}
    echo -e "Installation date: $(tune2fs -l $(df / | tail -1 | cut -f1 -d' ') | grep -i "filesystem created" | sed 's/Filesystem created:       //g' || echo "Unknown")" >> ${FILE_PATH}
    echo -e "Hostname: $(hostname || echo "Unknown")" >> ${FILE_PATH}
    echo -e "Uptime: $(uptime -p || echo "Unknown")" >> ${FILE_PATH}
    echo -e "Processes running: $(ps aux --no-heading | wc -l || echo "Unknown")" >> ${FILE_PATH}
    echo -e "User logged in: $(who | wc -l || echo "Unknown")" >> ${FILE_PATH}
    echo -e "---- Network ----" >> ${FILE_PATH}
    for item in $(ifconfig | cut -d" " -f1 | awk NF | cut -d":" -f1)
    do
	    result=$(ip -f inet address show $item | grep inet | awk '{ print $2 }')
        if [ ! -z $result ]
        then
            echo "$item: $result" >> $FILE_PATH
        else
            echo "$item: -/-" >> $FILE_PATH
        fi
    done
    echo -e "----\"EOF\"----" >> ${FILE_PATH}
}

function createOutputFileandFolder()
{
	local LAST_FILENAME=
	declare -i NEW_NUM=0
 	if [ -e $FILE_PATH ]; then
		LAST_FILENAME=$(ls -rv $(dirname $FILE_PATH) | grep -E "$(basename $FILE_PATH)-[0-9]+-[0-9]+" | head -n 1)
        if [ ! -e $LAST_FILENAME ]; then
			let NEW_NUM=$[10#$(cut -d '-' -f3 <<< $LAST_FILENAME)]+1		
			$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
		else
			$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
		fi
		for item in $(ls -rv $(dirname $FILE_PATH) | grep -E "$(basename $FILE_PATH)-[0-9]+-[0-9]+" | tail -n +$PARAM_N)
		do
			$(rm -f $(dirname $FILE_PATH)/$item)
		done
    else
        $(mkdir -p $(dirname $FILE_PATH) && touch $FILE_PATH)
        if (( $? ))
        then
            printLocalizedMessages "MSG4"
            exit 1
        fi
    fi
}

function main()
{
    Locale
    args "$@"
    createOutputFileandFolder
    getHWINFO
    
    exit 0
}

if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

main "$@"