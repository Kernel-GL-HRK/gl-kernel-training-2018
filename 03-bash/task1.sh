#!/bin/bash  -x

clear

function get_locale_name (){
 	local locale_name=$(locale | grep "LC_NAME=")
 	expr "$locale_name" : '\S*=\([^.]*\)'
}

function print_help(){	
	echo "./task1.sh [-h|--help] [-n num] [file]"
	if [[ $(get_locale_name) = "uk_UA" ]]
	then
		>&2 echo "де 'num' - кількість файлів із результатами,"
		>&2 echo "'file' - шлях та ім'я файла, у який треба записати результат"
		>&2 echo "наприклад: task1.sh -n 9 ./test/info/info.txt"
	else
		>&2 echo "where 'num' - number result files"
		>&2 echo "'file' - path to and  result file name "
		>&2 echo 'e.g. task1.sh -n 9 ./test/info/info.txt'
	fi
}


num_files = "0"
file_path =""

cur_local=$(get_locale_name)


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

