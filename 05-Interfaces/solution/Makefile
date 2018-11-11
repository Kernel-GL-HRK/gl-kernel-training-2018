obj-m += convert_module.o

host:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

host_clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

all: check-env
	make -C ${BUILD_KERNEL} M=${PWD} modules

clean: check-env
	make -C ${BUILD_KERNEL} M=${PWD} clean

check-env:
ifndef BUILD_KERNEL
  $(error BUILD_KERNEL is undefined)
endif
