#
# Homework 6: Building module_time
#

obj-m :=module_time.o

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
