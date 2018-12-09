#!/bin/bash

overwrite() { 
	echo -e "\r\033[1A\033[0K$@"; 
	}

#Enable MPU6050 chip
sudo i2cset -y 0 0x68 0x6B 0

while sleep 1; 
do
    #Get raw data
    HB=$(sudo i2cget -y 0 0x68 0x41)
    LB=$(sudo i2cget -y 0 0x68 0x42)

    #Convert to signed int
    TEMP=$(( (HB<<8)+LB-65535 ))
    #Convert to MPU6050 units
    TEMP=$(( $TEMP*2941+36530000 ))
    #Get Left side C
    TEMPHC=$(( $TEMP/1000000 ))
    #Get Right side C
    TEMPLC=$(( ($TEMP-($TEMPHC*1000000))/10000 ))
    #Convert to F
    TEMPF=$(( ($TEMP*18)+320000000 ))
    #Get Left side F
    TEMPHF=$(( $TEMPF/10000000 ))
    #Get Right side F
    TEMPLF=$(( ($TEMPF-($TEMPHF*10000000))/100000 ))
    #Output
    overwrite Temp\(c\) = $TEMPHC.$TEMPLC   Temp\(F\) = $TEMPHF.$TEMPLF
done;