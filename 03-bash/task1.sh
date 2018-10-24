#!/bin/bash  -x

clear

function print_help(){
	  	echo "print help"
}
num_files = "0"
file_path =""
#parse command line 
#./task1.sh [-h|--help] [-n num] [file]
while (( "$#" )); do 
  		if [[ "$1" = '-h' || "$1" = '--help' ]]
  		 then
			print_help
  			exit 0
  			#statements
  		fi

  		if [[ $1 = -n && $# > 2 ]]; then
  			num_files="$2"
  			shift; shift; continue;
  		fi

  		file_path="$1"

  shift 
done

echo "num_files $num_files"
echo "file_path $file_path"

