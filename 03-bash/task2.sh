#! /bin/bash -x
#
# lesson_3 homework bash task2.sh
#

TARGET_DIR=/usr/local/bin
TARGET_FILE=task1.sh
DATA=`date +%y%m%d`
DIR=$(pwd)

#  Check command line parameters
if [ "$#" -gt 1 ] ; then 
	echo "ERROR arguments more then 1" >$2
	exit 1 
fi

# processing command line parameters
# ./task1.sh [-h|--help]
help=0
err=0

while [[ -n $1 ]]; do
	case $1 in 
		-h | --help)	
			help=1	;;
		*)	
			err=1;;
	esac
	shift
done

# Help info key: -h or --help
if [ $help -eq 1 ]; then 
	cat <<- _EOF_
	./task2.sh [-h|--help]
	key: [-h | --help] - Help info page  
	_EOF_
	exit 0
else help=0
fi

if [ $err -eq 1 ]; then 
	echo "ERROR: Incorrect parameters commandline" >$2
	exit 1
else err=0
fi

# Creating directory /usr/local/bin
if [ ! -d $TARGET_DIR ]; then 
	echo "Created directory: $TARGET_DIR"
$(mkdir $TARGET_DIR)
	if (($?)); then 
	echo "ERROR: Failed to create $TARGET_DIR" >$2
	exit 1
	fi
fi

# file task1.sh is copied to the   /usr/local/bin
$(cp "$TARGET_FILE" "$TARGET_DIR")
count=$(ls -l $DIR | grep environment | wc -l)
	if [ $count -lt 1 ]; then 
	count=$(printf %04d 0)
	else 
	count=$(printf %04d $count)
	fi
$(cp /etc/environment $DIR/environment-$DATA-$count)	
echo "PATH="'$PATH'":$TARGET_DIR" >> /etc/environment
$(chmod 755 $TARGET_DIR/$TARGET_FILE)
echo "--- Done ---"
