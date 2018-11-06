#!/bin/bash
TARGET_FILE=task1.sh
INSTALL_PATH=/usr/local/bin
LOCALE=US

function usage()
{
    echo
    echo "USAGE: task2.sh [-h|--help]"
    echo 
    echo "-h|--help:  this page"
    echo
}

function printLocalizedMessages()
{
    if [[ "$LOCALE" == UA ]]; then
    case "$1" in
        "MSG1") echo "Невірній ключ" >&2;;
        "MSG2") echo "Неможливо встановити додаток" >&2;;
    esac
    else
    case "$1" in 
        "MSG1") echo "Wrong key" >&2;;
        "MSG2") echo "Could not install app" >&2;;
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

function install()
{
    local FILE_PATH=/etc/enviroment
    declare -i NEW_NUM=0
    cp $TARGET_FILE $INSTALL_PATH
    LAST_FILENAME=$(ls -rv $(dirname $FILE_PATH) | grep -E "$(basename $FILE_PATH)-[0-9]+-[0-9]+" | head -n 1)
    if [ ! -e $LAST_FILENAME ]; then
		let NEW_NUM=$[10#$(cut -d '-' -f3 <<< $LAST_FILENAME)]+1		
		$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
	else
		$(cp $FILE_PATH $(dirname $FILE_PATH)/$(basename $FILE_PATH)-$(date '+%Y%m%d')-$(printf %04d $NEW_NUM))
	fi
    if [[ "$(cat $FILE_PATH | grep -E "$INSTALL_PATH")" == "" ]]; then
        cat $FILE_PATH | sed '1,/RE/s/"/$(dirname $INSTALL_PATH):/' > $FILE_PATH
    fi
    chmod uog+rx-w $DEST_DIR/$TARGET_FILE
}

function createDir()
{
    if [ ! -d  $DEST_DIR ]
    then
        $(mkdir $DEST_DIR)
        if (( $? ))
        then
            printLocalizedMessages "MSG2"
            exit 1
        fi
    fi
}

function args()
{
    while [ -n "$1" ]
    do
        case "$1" in
            -h|--help)  usage
                        exit 0;;
                    *)  print_localized_message "MSG1"
                        exit 1;;
        esac
        shift
    done
}

function main()
{
    Locale
    args "$@"
    createDir
    install
    exit 0
}

if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

main "$@"