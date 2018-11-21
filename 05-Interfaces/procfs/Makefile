obj-m += proc_fs_module.o

all: check-env
	make -C ${BUILD_KERNEL} M=${PWD} modules

clean: check-env
	make -C ${BUILD_KERNEL} M=${PWD} clean

copy_to_vb: all
	scp proc_fs_module.ko myLinux:/home/user

check-env:
ifndef BUILD_KERNEL
 	$(error BUILD_KERNEL is undefined)
endif