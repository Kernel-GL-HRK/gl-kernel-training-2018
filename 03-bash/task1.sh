#!/bin/bash
declare locale=$(locale | grep LANGUAGE)
filename="~/bash/task1.out"
declare -i numfile=1
curr_date=`date -R`

main (){
    parse_args $@ 
    $(mkdir -p "$(dirname "$filename")" && touch "$filename")
    echo "Date: $curr_date">$filename
    hw_info
    sys_info
    net_info
    echo "----"EOF"----">>$filename
    exit 0
}

parse_args (){
 
    for i in "$@"
    do
    case $i in
        -h|--help)
        print_help
        exit 0;
        ;;
        -n)
        shift
        if [[ "$1" =~ ^[0-9]+$ ]]; then
            if (($1>1)); then
                numfile=$1
            else
                echo "-n must be greater than 1">&2
                exit 1;
            fi
        else
            echo "-n must be int greater than 1">&2
            exit 1;
        fi
        ;;
        *)
        if [ ! -z $1 ]; then
            filename=$1
        fi
        ;;
    esac
    shift
    done
}

print_help (){
    echo
    echo "./task1.sh [-h|--help] [-n num] [file]"
    echo
    echo "-h|--help:  help page"
    echo "-n num:     number of output files. must be bigger than 1"
    echo "file:       output file name. default $filename"
    echo
}

hw_info (){
    echo "---- Hardware ----">>$filename
    cpu_ver="CPU: \"`sudo dmidecode -s processor-version`\""
    mb_vendor=`sudo dmidecode -s baseboard-manufacturer`
    mb_name=`sudo dmidecode -s baseboard-product-name`
    sys_sn=`sudo dmidecode -s system-serial-number`
    ram_cnt="MEM: `echo $(($(getconf _PHYS_PAGES) * $(getconf PAGE_SIZE) / (1024 * 1024)))` MB"
    echo $cpu_ver>>$filename
    echo $ram_cnt>>$filename
    echo "Motherboard: \"$mb_vendor\", \"$mb_name\"">>$filename
    echo "System Serial Number: $sys_sn">>$filename
}

sys_info (){
    echo ---- System ---->>$filename
    os_dist=$(lsb_release -sd)
    kernel_ver=$(uname -r)
    fs_created=$(stat -c %z /var/log/installer/syslog)
    hostname=$(cat /etc/hostname)
    uptime=$(uptime -p)
    pr_cnt=$(ps -A --no-headers | wc -l)
    usr_cnt=$(users | wc -w)
    echo OS Distribution: \"$os_dist\">>$filename
    echo Kernel version: $kernel_ver>>$filename
    echo Installation date: $fs_created>>$filename
    echo Hostname: $hostname>>$filename
    echo Uptime: $uptime>>$filename
    echo Processes running: $pr_cnt>>$filename
    echo User logged in: $usr_cnt>>$filename
}

net_info() {
    echo ---- Network ---->>$filename
    for iface in $(ifconfig | cut -d ' ' -f1| tr ':' '\n' | awk NF)
    do
        echo -n "$iface: ">>$filename
        ip_info=$(ip address show $iface| grep "inet " | awk '{ print $2 }')
        #echo $ip_info
        if [ -z $ip_info ]; then
            echo "-/-">>$filename
        else
            echo $ip_info>>$filename
        fi
    done
}

main $@
