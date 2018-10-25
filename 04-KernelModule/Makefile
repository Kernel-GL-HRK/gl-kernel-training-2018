KERNEL_DIR = /lib/modules/$(shell uname -r)/build
TARGET = testmodule

obj-m:= $(TARGET).o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) modules
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -f .cache.mk
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	