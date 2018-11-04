#!/bin/bash
# echo ">>>>>>>>>> dir >>>>>>>>>>> $1"
if ! [ -d $1 ]; then 
	mkdir -p $1
fi
