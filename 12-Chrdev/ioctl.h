#ifndef __MPU6050_IOCTL_H__
#define __MPU6050_IOCTL_H__

typedef struct mpu6050_raw_data {
	s16 accel_raw_x;
	s16 accel_raw_y;
	s16 accel_raw_z;
	s16 gyro_raw_x;
	s16 gyro_raw_y;
	s16 gyro_raw_z;
	s16 temp_raw;
} MPU6050_RAW_DATA_STRUCT;

typedef struct mpu6050_stat {
	ssize_t irq_handled;
	ssize_t irq_per_sec;
} MPU6050_STAT_STRUCT;

#define IOC_MAGIC 'M'
#define IOCTL_GET_RAW_DATA  _IOR( IOC_MAGIC, 1, MPU6050_RAW_DATA_STRUCT )
#define IOCTL_GET_STAT      _IOR( IOC_MAGIC, 2, MPU6050_STAT_STRUCT )
#define IOCTL_WRITE_REG     _IOW( IOC_MAGIC, 3, unsigned long int )
#define IOCTL_READ_REG      _IOWR( IOC_MAGIC, 4, unsigned long int  )


#endif