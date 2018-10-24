#!/bin/bash
# check lang
LNG='en'
SYS_LANG=${LANG::5}
if [ "$SYS_LANG" = "uk_UA" ]
then
   LNG='ua'
fi

# output file name
FNAME=~/bash/task1.out

declare -i NFILES=0

# check and parse command line arguments
for arg
do
  if [ $arg = '-h' ] || [ $arg = '--help' ]
  then
    if [ $LNG = 'ua' ]
    then
      echo 'Завдання03: учбовий скрип shell: інформація про систему'
      echo 'arguments:'
      echo '  -h or --help - this help'
      echo '  -n N         - max number of output files'
      echo '  filename     - path and name of file to store system information'
    else
      echo 'Lesson03: educative shell script: system info'
      echo 'arguments:'
      echo '  -h or --help - this help'
      echo '  -n N         - max number of output files'
      echo '  filename     - path and name of file to store system information'
    fi
    exit 0
  elif [ $NKEY ]
  then
    NKEY=''
    if [ $arg -ge "1" ]
    then
      NFILES=$arg
    else
      if [ $LNG = 'ua' ]
      then
        echo 'Треба вказа3ти ціле число файлів після ключа -n' 1>&2
      else
        echo 'Expected integer max file number after -n key.' 1>&2
      fi
      exit 1
    fi
  elif [ $arg = '-n' ]
  then
    NKEY='1'
  else
    FNAME=$arg
  fi
done

DIR="$(dirname $FNAME)"
echo "DIR is $DIR"
mkdir -p "$DIR"
if [ $? -ne 0 ]
then
  if [ $LNG = 'ua' ]
  then
    echo "Неможливо збудувати повний путь '$DIR'." 1>&2
  else
    echo "Can't create path: '$DIR'." 1>&2
  fi
  exit 2
fi

if [ -f "$FNAME" ]
then
  echo "File '$FNAME' exists. Renaming..."
  # loop
  NN='0000'
  while [ "0" ]
  do
    FNAME2="$FNAME-"`date +%Y%m%d`-`printf "%04d" $NN`
    #echo $FNAME2
    if [ ! -f "$FNAME2" ]
    then
      break;
    fi
    let NN=NN+1
  done
  mv $FNAME $FNAME2
fi

if (( $NFILES > 0 ))
then
  echo "NFiles = $NFILES"
#delete old files (not only for current date)
FLIST=$(ls -1 -t $FNAME-????????-????)
#skip NFILES lines, else (if any) - remove
#echo $FLIST
FILEN=0
while read -r line
do
  let FILEN=FILEN+1
  if [[ $FILEN -gt $NFILES ]]
  then
    echo "Removing old file '$line'..."
    rm $line
  fi
done <<< $FLIST
fi

echo "Using output file '$FNAME'..."
echo -n "Date: " > $FNAME
date "+%a, %d %h %Y %T %z" >> $FNAME
echo "---- Hardware ----" >> $FNAME
CPU=$(sudo dmidecode -s processor-version)
echo "CPU: \"$CPU\"" >> $FNAME
MEMGB=$(sudo dmidecode -q | grep -e "Size: [0-9][0-9][0-9][0-9] MB")
MEMGB=$(cut -f2 -d":" <<< $MEMGB)
echo "RAM:$MEMGB" >> $FNAME
MBOARD="\"$(sudo dmidecode -s baseboard-manufacturer)\", \"$(sudo dmidecode -s baseboard-product-name)\""
echo "Motherboard: $MBOARD" >> $FNAME
SSN=$(sudo dmidecode -s system-serial-number)
echo "System serial number: $SSN" >> $FNAME

echo "---- System ----" >> $FNAME
OS_ABOUT=$(hostnamectl | grep "Operating System")
OS_ABOUT=${OS_ABOUT:20}
echo "OS Distribution: \"$OS_ABOUT\"" >> $FNAME
KERNEL_VER=$(uname -r)
echo "Kernel Version: $KERNEL_VER" >> $FNAME
HOST_NAME=$(hostname)
echo "Hostname: $HOST_NAME" >> $FNAME

echo "---- Network ----" >> $FNAME
IFLIST=$(ls -1 /sys/class/net/)

while read -r line
do
  echo -n $line >> $FNAME
  IPLINE=$(ip address show dev $line | grep "inet ")
  if [ -n "$IPLINE" ]
  then
    #IPADDR=$(cut -f6 -d" " <<< $IPLINE)
    IPADDR=$(awk '{ print $2}' <<< $IPLINE)
  else
    IPADDR="-/-"
  fi
  echo ": $IPADDR" >> $FNAME
done <<< $IFLIST


echo "---- \"EOF\" ----" >> $FNAME
echo "Done.Buy!"
