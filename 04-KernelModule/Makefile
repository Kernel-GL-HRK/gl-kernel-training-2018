obj-m += simple_kernel_module.o

all: check-env
	make -C ${BUILD_KERNEL} M=${PWD} modules
clean: check-env
	make -C ${BUILD_KERNEL} M=${PWD} clean

check-env:
ifndef BUILD_KERNEL
 	$(error BUILD_KERNEL is undefined)
endif