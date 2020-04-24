#
# Homework 5: Building module_procfs and module_sysfs
#

obj-m :=module_procfs.o module_sysfs.o

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
