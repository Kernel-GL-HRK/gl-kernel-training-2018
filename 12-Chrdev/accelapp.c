#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "mpu6050-regs.h"

typedef short int s16; 
#include "ioctl.h"

#define BUFF_SIZE 100
char buffer[BUFF_SIZE];

int main(void) {
    int fd;
    int ret;
    unsigned long int arg;
    MPU6050_RAW_DATA_STRUCT data;
    MPU6050_STAT_STRUCT stat;

    printf("\n *** MPU6050 accelerometer user application ***\n\n");

    //Open
    fd = open("/dev/accel", O_RDWR);
    if(fd < 0) {
        printf("Error while open /dev/accel device. Err: %d\n", fd);
        return -1;
    }

    //Read
    memset(buffer, 0, sizeof(char) * BUFF_SIZE);
    printf("Try to read from device:\n");
    ret = read(fd, buffer, sizeof(char));    
    if(ret >=0) {
        printf("---> %s\n", buffer);
    } else {
        printf("Error: %d\n", ret);
    }

    //Write
    memset(buffer, 0xaa, sizeof(char) * BUFF_SIZE);
    printf("Try to write to device:\n");
    ret = write(fd, buffer, BUFF_SIZE);
    if(ret >=0) {
        printf("---> Success\n");
    } else {
        printf("Error: %s\n", strerror(errno));
    }

    //IOCTLs
    //IOCTL_GET_RAW_DATA
    memset(buffer, 0, sizeof(MPU6050_RAW_DATA_STRUCT));
    printf("\n*** Read RAW accel data ***\n");
    ret = ioctl(fd, IOCTL_GET_RAW_DATA, &data);

    printf("Accle.x = %d\n", data.accel_raw_x);
    printf("Accle.y = %d\n", data.accel_raw_y);
    printf("Accle.z = %d\n", data.accel_raw_z);

    printf("Gyro.x = %d\n", data.gyro_raw_x);
    printf("Gyro.y = %d\n", data.gyro_raw_y);
    printf("Gyro.z = %d\n", data.gyro_raw_z);

    printf("Temp = %2.2f\n\n", ((float)data.temp_raw)/100);

    //IOCTL_GET_STAT
    memset(buffer, 0, sizeof(MPU6050_STAT_STRUCT));
    printf("\n*** Read RAW statistic data ***\n");
    ret = ioctl(fd, IOCTL_GET_STAT, &stat);

    printf("IRQ handled = %d\n", stat.irq_handled);
    printf("IRQ per Sec = %d\n", stat.irq_per_sec);


    //IOCTL_READ_REG
    arg = (REG_WHO_AM_I << 8);
    printf("\n*** Read reg: 0x%X ***\n", REG_WHO_AM_I);
    ret = ioctl(fd, IOCTL_READ_REG, arg);
    printf("Value: 0x%X\n", ret);





    //IOCTL_READ_REG
    arg = (REG_ACCEL_CONFIG << 8);
    printf("\n*** Read reg: 0x%X ***\n", REG_ACCEL_CONFIG);
    ret = ioctl(fd, IOCTL_READ_REG, arg);
    printf("Value: 0x%X\n", ret);

    //IOCTL_WRITE_REG
    arg = (REG_ACCEL_CONFIG << 8) | (ret | 0x08);
    printf("\n*** Write modified reg: 0x%X ***\n", REG_ACCEL_CONFIG);
    ret = ioctl(fd, IOCTL_WRITE_REG, arg);

    //IOCTL_READ_REG
    arg = (REG_ACCEL_CONFIG << 8);
    printf("\n*** Read back reg: 0x%X ***\n", REG_ACCEL_CONFIG);
    ret = ioctl(fd, IOCTL_READ_REG, arg);
    printf("Value: 0x%X\n", ret);


    //Close
    close(fd);

    printf("\n *** Exit *** \n");
    return 0;
}