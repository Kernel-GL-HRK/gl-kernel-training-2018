#!/bin/bash -x

LOCALE=US
DEST_DIR=/usr/local/bin
TARGET_FILE=task1.sh

function print_localized_message()
{
    case "$1" in
        "MESSAGE1") if [ "$LOCALE" = UA ]
                    then
                        echo "Цей ключ не пiдтримуеться" >&2
                    else
                        echo "unsupported key" >&2
                    fi;;
        "MESSAGE2") if [ "$LOCALE" = UA ]
                    then
                        echo "Не вдалося створити папку" >&2
                    else
                        echo "Failed to create folder" >&2
                    fi;;
    esac
}

function install()
{
    cp $TARGET_FILE $DEST_DIR
    cp /etc/profile /etc/profile-$(date +%Y%m%d)
    echo "PATH="'$PATH'":$DEST_DIR" >> /etc/profile
    chmod uog+rx-w $DEST_DIR/$TARGET_FILE
}

function create_dir()
{
    if [ ! -d  $DEST_DIR ]
    then
        $(mkdir $DEST_DIR)
        if (( $? ))
        then
            print_localized_message "MESSAGE2"
            exit 1
        fi
    fi
}

function check_locale()
{
    if [ "$LANG" = "uk_UA.UTF-8" ]
    then
        LOCALE=UA
    fi
}

function usage()
{
    echo
    echo "USAGE: task2.sh [-h|--help]"
    echo 
    echo "-h|--help:  this page,"
    echo
    echo
}

function parse_args()
{
    while [ -n "$1" ]
    do
        case "$1" in
            -h|--help)  usage
                        exit 0;;
                    *)  print_localized_message "MESSAGE1"
                        exit 1;;
        esac
        shift
    done
}

function main()
{
    check_locale
    parse_args "$@"
    create_dir
    install

    exit 0
}

if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

main "$@"