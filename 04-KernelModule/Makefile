BUILD_KERNEL=/lib/modules/$(shell uname -r)/build
obj-m+=simpleModule.o
all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
