#
# Homework 5: Building module_procfs
#

obj-m :=module_procfs.o

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
