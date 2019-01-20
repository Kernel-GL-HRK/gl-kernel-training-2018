#!/bin/bash

#check WHO_AM_I register
WAMI=$(sudo i2cget -y 0 0x68 0x75)
if [ "$WAMI" != "0x68" ]
then
  echo "MPU6050 not found ($WAMI)"
  exit 1
fi

#start conversion: set PWR_MGMT_1 to 0
PM1=$(sudo i2cget -y 0 0x68 0x6B)
if [ "$PM1" != "0x00" ]
then
  sudo i2cset -y 0 0x68 0x6B 0
  echo "MPU6050: start conversion..."
  sleep 1
fi

while true
do
 echo -n "MPU6050 T: "
 TH=$(sudo i2cget -y 0 0x68 0x41)
 echo -n "$TH,"

 TL=$(sudo i2cget -y 0 0x68 0x42)
 echo -n "$TL :  "

 T=$(./cvt-mpu6050-t $TH $TL)
 echo "$T degC"

 sleep 1
done
