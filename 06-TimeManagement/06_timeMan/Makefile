#!/usr/bin/make
obj-m+=timeMan.o
all: kernel_path_check.o
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean

kernel_path_check.o:
	if [ -z "${BUILD_KERNEL}" ]; \
	then \
		echo 'BUILD_KERNEL is not set. abort'>&2; \
		exit 1; \
	else \
		echo 'BUILD_KERNEL is set. continue'; \
	fi
