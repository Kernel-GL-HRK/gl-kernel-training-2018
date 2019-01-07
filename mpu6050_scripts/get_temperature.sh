#!/bin/bash
#Init
sudo i2cset -y 0 0x68 0x6B 0

while true
do

#Read data
UB=$(sudo i2cget -y 0 0x68 0x41)
LB=$(sudo i2cget -y 0 0x68 0x42)

#Upper Bit + Lower Bit
TEMP=$(( (UB<<8)+LB-65535 ))

# Calc C value
TEMP_C=$(bc << EOF
    scale = 2
    a = ( $TEMP / 340 )
    a + 36.53
EOF
)

# Convert C to F value
TEMP_F=$(bc << EOF
    scale = 2
    a = ( $TEMP_C * 9 )
    b = ( a / 5 )
    b + 32
EOF
)

clear
printf "\n  Temp C = $TEMP_C ; Temp F = $TEMP_F"
sleep 1

done
