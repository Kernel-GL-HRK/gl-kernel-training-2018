#
# Homework 4: Building module
#
BUILD_KERNEL=/lib/modules/$(shell uname -r)/build

obj-m :=module_1.o

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
