#
# mpu6050 accelerometer & gyroscope
#
ifneq ($(KERNELRELEASE),)

obj-m := mpu6050.o

else

export KERNELDIR = /usr/src/linux-headers-4.14.84-sunxi

#KERNELDIR ?= $(BBB_KERNEL)

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-

.PHONY: all clean

all:
ifeq ($(KERNELDIR),)
ifeq ($(BBB_KERNEL),)
	$(error Path to kernel tree - KERNELDIR or BBB_KERNEL variable is not defined!)
endif
endif
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
ifeq ($(KERNELDIR),)
ifeq ($(BBB_KERNEL),)
	$(error Path to kernel tree - KERNELDIR or BBB_KERNEL variable is not defined!)
endif
endif
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean


install:
	#sudo cp mpu6050.ko /lib/modules/4.14.84-sunxi/kernel/
	#sudo cp mpu6050.ko /lib/modules/4.14.84-sunxi/kernel/drivers/
	sudo cp mpu6050.ko /lib/modules/4.14.84-sunxi/kernel/drivers/i2c/
	sudo depmod -a

endif
