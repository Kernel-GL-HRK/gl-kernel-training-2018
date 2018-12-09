#!/bin/bash

overwrite() { echo -e "\r\033[1A\033[0K$@"; }

if [ ! -f /sys/class/mpu6050/accel_x ]; then
    echo "The MPU6050 driver is not loaded"
fi

while sleep 1; do
    AX=$(cat /sys/class/mpu6050/accel_x)
    AY=$(cat /sys/class/mpu6050/accel_y)
    AZ=$(cat /sys/class/mpu6050/accel_z)

    GX=$(cat /sys/class/mpu6050/gyro_x)
    GY=$(cat /sys/class/mpu6050/gyro_y)
    GZ=$(cat /sys/class/mpu6050/gyro_z)

    TEMP=$(cat /sys/class/mpu6050/temp)
    
    overwrite "AX=$AX   AY=$AY   AZ=$AZ   GA=$GX   GY=$GY   GZ=$GZ   TEMP=$TEMP"
    done;
