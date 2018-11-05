KERNEL_DIR=$(BUILD_KERNEL)
TARGET=to_lower_case

obj-m:= $(TARGET).o

all:
	@if [ -z $(KERNEL_DIR) ]; then\
        echo "Export BUILD_KERNEL first";\
	else \
		$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) modules;\
    fi
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -f .cache.mk
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	