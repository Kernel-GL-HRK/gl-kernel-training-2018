#!/bin/bash
sudo sh -c "echo 0x68 > /sys/bus/i2c/devices/i2c-0/delete_device"
sudo rmmod mpu6050.ko
