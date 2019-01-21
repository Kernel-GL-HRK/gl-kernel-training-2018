KERNEL_DIR=$(BUILD_KERNEL)
TARGET_ARCH=arm
TARGET_CC=arm-linux-gnueabihf-
TARGET=mpu6050

obj-m:= $(TARGET).o

all:
	@if [ -z $(KERNEL_DIR) ]; then\
        echo "Export BUILD_KERNEL first";\
	else \
		$(MAKE) ARCH=$(TARGET_ARCH) CROSS_COMPILE=$(TARGET_CC) -C $(KERNEL_DIR) M=$(shell pwd) modules;\
    fi
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -f .cache.mk
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	