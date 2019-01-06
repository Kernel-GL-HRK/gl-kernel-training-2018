 # !/bin/sh
# xxxxx$ reset<CR>  And all will be corrected...

draw_ui () {
       # Set up the screen per scan and prepare for the bargraph.
    clear
    printf $WOB" GYRO \n"
    printf $WOB"    -30        -20       -10        0         10        20        30    x1000\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n"
    printf $YOB"X(|                                                                   |) $GYRO_X\n"
    printf $GOB"Y(|                                                                   |) $GYRO_Y\n"
    printf $ROB"Z(|                                                                   |) $GYRO_Z\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n\n"
    printf $WOB" ACCEL \n"
    printf $WOB"    -30        -20       -10        0         10        20        30    x1000\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n"
    printf $YOB"X(|                                                                   |) $ACCEL_X\n"
    printf $GOB"Y(|                                                                   |) $ACCEL_Y\n"
    printf $ROB"Z(|                                                                   |) $ACCEL_Z\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n\n"
    printf $WOB" TEMP \n"
    printf $WOB"    -20         0         20        40        60        80        100   x1\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n"
    printf $WOB"T(|                                                                   |) $TEMP\n"
    printf $WOB"   ---+----+----+----+----+----+----+----+----+----+----+----+----+---\n\n"
}

random_data () {
    # Mock data for tests
    GYRO_X=$[($RANDOM % (64/1))]
    GYRO_X=$(( ($GYRO_X-32)*1000 ))

    GYRO_Y=$[($RANDOM % (64/1))]
    GYRO_Y=$(( ($GYRO_Y-32)*1000 ))

    GYRO_Z=$[($RANDOM % (64/1))]
    GYRO_Z=$(( ($GYRO_Z-32)*1000 ))

    ACCEL_X=$[($RANDOM % (64/1))]
    ACCEL_X=$(( ($ACCEL_X-32)*1000 ))

    ACCEL_Y=$[($RANDOM % (64/1))]
    ACCEL_Y=$(( ($ACCEL_Y-32)*1000 ))

    ACCEL_Z=$[($RANDOM % (64/1))]
    ACCEL_Z=$(( ($ACCEL_Z-32)*1000 ))

    TEMP=$[($RANDOM % (64/1))]
    TEMP=$(( ($TEMP-13)*2 ))
}

read_data () {
    GYRO_X=$(cat /sys/class/mpu6050/gyro_x)
    GYRO_Y=$(cat /sys/class/mpu6050/gyro_y)
    GYRO_Z=$(cat /sys/class/mpu6050/gyro_z)
    
    ACCEL_X=$(cat /sys/class/mpu6050/accel_x)
    ACCEL_Y=$(cat /sys/class/mpu6050/accel_y)
    ACCEL_Z=$(cat /sys/class/mpu6050/accel_z)
    
    TEMP=$(cat /sys/class/mpu6050/temp)
}

draw_graph () {
    GYRO_VAL=$1
    LINE_NUM=$2
    COLOR1=$3
    COLOR2=$4

    BARGRAPH_GYRO=$COLOR1"\x1B[$LINE_NUM;2f(|"$COLOR2
    for color in $(seq 1 "$GYRO_VAL")
    do
        BARGRAPH_GYRO=$BARGRAPH_GYRO" "
    done
    printf "$BARGRAPH_GYRO"$COLOR1"\n"
}

###############
# GLOBAL VARs #
###############

# White On Black.
WOB="\x1B[1;37;40m"
#Bleck on white
BOW="\x1B[1;30;47m"
# Black On Green.
BOG="\x1B[1;30;42m"
# Black On Yellow.
BOY="\x1B[1;30;43m"
# Black On red.
BOR="\x1B[1;30;41m"
# Green On Black.
GOB="\x1B[1;32;40m"
# Yellow On Black.
YOB="\x1B[1;33;40m"
# Red On Black.
ROB="\x1B[1;31;40m"

# Set the pseudo value to zero.
DEPTH=0
GYRO_X=0
GYRO_Y=0
GYRO_Z=0
ACCEL_X=0
ACCEL_Y=0
ACCEL_Z=0
TEMP=0

################
# Main Script ##
################

# Do a clear screen to White On Black.
printf $WOB
clear

while true
do
    # random_data # mock random data generating
    read_data # real data read
    draw_ui

    #Scale values to bar size
    GYRO_X=$(( ${GYRO_X%%.*}/1000+33 ))
    GYRO_Y=$(( ${GYRO_Y%%.*}/1000+33 ))
    GYRO_Z=$(( ${GYRO_Z%%.*}/1000+33 ))
    ACCEL_X=$(( ${ACCEL_X%%.*}/1000+33 ))
    ACCEL_Y=$(( ${ACCEL_Y%%.*}/1000+33 ))
    ACCEL_Z=$(( ${ACCEL_Z%%.*}/1000+33 ))
    TEMP=$(( ${TEMP%%.*}/2+13 ))

    #Draw bars
    draw_graph $GYRO_X 4 $YOB $BOY
    draw_graph $GYRO_Y 5 $GOB $BOG
    draw_graph $GYRO_Z 6 $ROB $BOR
    draw_graph $ACCEL_X 12 $YOB $BOY
    draw_graph $ACCEL_Y 13 $GOB $BOG
    draw_graph $ACCEL_Z 14 $ROB $BOR
    draw_graph $TEMP 20 $WOB $BOW

    sleep 0.05
done
