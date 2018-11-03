KERNEL_DIR=$(BUILD_KERNEL)
TARGET=to_upper_case

obj-m:= $(TARGET).o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) modules
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -f .cache.mk
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	