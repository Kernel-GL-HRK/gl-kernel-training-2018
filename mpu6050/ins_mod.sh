#!/bin/bash
 sudo insmod mpu6050.ko
 sudo sh -c "echo mpu6050 0x68 > /sys/bus/i2c/devices/i2c-0/new_device"
 