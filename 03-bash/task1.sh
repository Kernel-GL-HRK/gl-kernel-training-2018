#!/bin/bash -x

PARAM_N=1
FILE_PATH=~/bash_out/task1.out
FLAG_INSIDE=0
LOCALE=US

function print_localized_message()
{
    case "$1" in
        "MESSAGE1") if [ "$LOCALE" = UA ]
                    then
                        echo "-n параметр мусить доривнювати або бiлше за 1" >&2
                    else
                        echo "-n parameter must be greter or equal 1" >&2
                    fi;;
        "MESSAGE2") if [ "$LOCALE" = UA ]
                    then
                        echo "-n параметр мусить бути цiлым числом" >&2
                    else
                        echo "-n parameter must be an integer value" >&2
                    fi;;
        "MESSAGE3") if [ "$LOCALE" = UA ]
                    then
                        echo "Забагато аргументiв" >&2
                    else
                        echo "Too many arguments" >&2
                    fi;;
        "MESSAGE4") if [ "$LOCALE" = UA ]
                    then
                        echo "Не вдалося створити папку виводу" >&2
                    else
                        echo "Failed to create output folder" >&2
                    fi;;
    esac
}

function check_locale()
{
    if [ "$LANG" = "uk_UA.UTF-8" ]
    then
        LOCALE=UA
    fi
}

function get_hw_info()
{
    local TOTAL_MEMORY=0
    local MANUFACRTURER
    local PRODUCT

    echo "---- Hardware ----" >> $FILE_PATH
#CPU
    $(dmidecode -t processor | grep "Version:" | sed 's/.Version/CPU/' >> $FILE_PATH)
#RAM
    for item in $(dmidecode -t memory | grep "Size:")
    do
        if [[ $item =~ ^[0-9]+$ ]]
        then
            let TOTAL_MEMORY=TOTAL_MEMORY+$item
        fi
    done
    echo "RAM: $TOTAL_MEMORY MB" >> $FILE_PATH
#MB
    MANUFACRTURER=$(dmidecode -t baseboard | grep "Manufacturer" | sed 's/.Manufacturer/Motherboard/')
    PRODUCT=$(dmidecode -t baseboard | grep "Product Name" | sed 's/.Product Name://')
    echo "$MANUFACRTURER,$PRODUCT" >> $FILE_PATH
#SN
    $(dmidecode -t system | grep "Serial Number:" | sed 's/.Serial Number/System Serial Number/' >> $FILE_PATH)

    echo "" >> $FILE_PATH
}

function get_sys_info()
{
    echo "---- System ----" >> $FILE_PATH
#Distr name
    echo "OS Distribution: $(cat /etc/os-release | grep VERSION= | sed 's/VERSION=//')" >> $FILE_PATH
#Kernel Ver
    echo "Kernel version: $(uname -r)" >> $FILE_PATH
#Install date
    echo "Installation date: "$(stat -c %z /var/log/installer/syslog) >> $FILE_PATH
#Hostname
    echo "Hostname: $(hostname)" >> $FILE_PATH
#Uptime
    echo "Uptime: $(uptime -p)" >> $FILE_PATH
#Processes running, without threads
    echo "Processes running: $( ps -A --no-headers | wc -l)" >> $FILE_PATH
#Number of users
    echo "User logged in: $(users | wc -w)" >> $FILE_PATH

    echo "" >> $FILE_PATH
}

function get_network_info()
{
    local result

    echo "---- Network ----" >> $FILE_PATH

    for item in $(ifconfig | cut -d" " -f1| awk NF | cut -d":" -f1)
    do
	    result=$(ip -f inet address show $item | grep inet | awk '{ print $2 }')
        if [ ! -z $result ]
        then
            echo "$item: $result" >> $FILE_PATH
        else
            echo "$item: -/-" >> $FILE_PATH
        fi
    done

    echo "" >> $FILE_PATH
}

function create_output_file()
{
	local LAST_FILENAME=
	declare -i NEW_NUM=0

	if [ -e  $FILE_PATH ]; then
		#find last file in a chain
		for item in $(ls $(dirname $FILE_PATH))
		do
			if [[ $(basename $item) =~ $(basename $FILE_PATH)-$(date '+%Y%m%d')- ]]; then					
				LAST_FILENAME=$item
			fi
		done
		#move target file in the end
		if [ ! -e $LAST_FILENAME ]; then
			let NEW_NUM=$[10#$(cut -d '-' -f3 <<< $LAST_FILENAME)]+1		
			$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
		else
			$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
		fi
		#delete files older than N
		for (( i=$NEW_NUM - $PARAM_N; i >= 0; i-- ))
		do
			$(rm -f $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $i))
		done
    else
        $(mkdir -p "$(dirname "$FILE_PATH")" && touch "$FILE_PATH")
        if (( $? ))
        then
            print_localized_message "MESSAGE4"
            exit 1
        fi
    fi
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
            print_localized_message "MESSAGE1"
            exit 1
        fi
    else
        print_localized_message "MESSAGE2"
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
        print_localized_message "MESSAGE3"
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
    check_locale

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