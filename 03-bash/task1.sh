#!/bin/bash

function usage()
{
	cat << HEREDOC
    Usage: $progname [-h|--help] [-n|--number NUM] [file]
    optional arguments:
    -h, --help          show this help message and exit
    -n, --number NUM 	max number files
    file                path output file
HEREDOC
}

error()
{
	printf "$1" >&2
	echo
	exit $2
}

# initialize variables
progname=$(basename $0)
FOUT=~/bash/task1.out

# use getopt and store the output into $OPTS
# note the use of -o for the short options, --long for the long name options
# and a : for any option that takes a parameter
OPTS=$(getopt -o "hn:" --long "help,num:" -n "$progname" -- "$@")
if [ $? != 0 ] ; then echo "Error in command line arguments." >&2 ; usage; exit 1 ; fi
eval set -- "$OPTS"

while true; do
  # uncomment the next line to see how shift is working
  # echo "\$1:\"$1\" \$2:\"$2\""
  case "$1" in
    -h | --help ) usage; exit; ;;
    -n | --num  ) NUM="$2"; shift 2 ;;
    --          ) shift; break ;;
    *           ) break ;;
  esac
done

# check
if [ -n "$1" ]
then
	FOUT=$1
fi

if [ -n "$NUM" ]; then
	if (( NUM <= 1 )); then
		error "NUM must be more 1" 1
	fi
fi

DIR="$(dirname "$FOUT")"
DATE=$(date -u +%Y%m%d)

# create dir
mkdir -p $DIR
if [ "$?" != "0" ]; then
	error "Create directory failed" 1
fi

# research
norigfile=$(ls -p $FOUT 2>/dev/null | wc -l)
nfilewithdate=$(ls $FOUT-$DATE-* 2>/dev/null | wc -l)

# remove
if [ -n "$NUM" ]; then
	if (( nfilewithdate >= NUM )); then
		deletfiles=$(ls -rt $FOUT-$DATE-* 2>/dev/null | head -$[$nfilewithdate - $NUM + 1])
		rm -f $deletfiles
	fi
fi

# create file
if (( norigfile > 0 )); then
	mv $FOUT $FOUT-$DATE-001
	FOUT=$FOUT-$DATE-002
elif (( nfilewithdate > 0 )); then
	index=$(ls -rt $FOUT-$DATE-* 2>/dev/null | tail -n 1 | grep -o '[0-9]*$' | sed 's/^0*//')
	index=$(printf "%03d" $[$index + 1])
	FOUT=$FOUT-$DATE-$index
fi
touch $FOUT

# write data to file
echo "---- Hardware ----" > $FOUT
echo $(sudo dmidecode -t processor | grep 'Version' | sed 's/Version: /CPU: \"/')\" >> $FOUT
echo $(sudo dmidecode -t memory | grep 'Size: [0-9]' | sed 's/Size/RAM/') >> $FOUT
echo $(sudo dmidecode -t baseboard | grep 'Version:' | sed 's/Version: /Motherboard: \"/')\", \
$(sudo dmidecode -t baseboard | grep 'Product Name: ' | sed 's/Product Name: /\"/')\">> $FOUT
echo $(sudo dmidecode -t system | grep 'Serial Number' | sed 's/Serial Number/System Serial Number/')>> $FOUT

echo "---- System ----" >> $FOUT
echo "OS Distribution: "$(uname -o) >> $FOUT
echo "Kernel version: "$(uname -r) >> $FOUT
echo "Installation date: "$(sudo head -n1 /var/log/installer/syslog | cut -f1 -d"u") >> $FOUT
echo "Hostname: "$(hostname) >> $FOUT
echo "Uptime: "$(uptime -p) >> $FOUT
echo "Processes running: "$(ps aux --no-heading | wc -l) >> $FOUT
echo "User logged in: "$(users | wc -w) >> $FOUT

echo "---- Network ----" >> $FOUT
for iface in $(ifconfig | cut -d ' ' -f1| tr ':' '\n' | awk NF)
do
	ip=$(ip -f inet address show $iface | grep "inet " | awk '{ print $2 }')
	if [ "$ipadress" == '' ]; then
		ipadress='-/-'
	fi
 	echo "$iface: $ip" >> $FOUT
done

echo "---- \"EOF\" ----" >> $FOUT

