#
# mpu6050 accelerometer & gyroscope
#
KERNELDIR+=/lib/modules/4.14.84-sunxi/build
CURDIR:=$(shell pwd)
obj-m := mpu6050.o

# export ARCH = arm
# export CROSS_COMPILE ?= arm-linux-gnueabihf-

.PHONY: all clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

