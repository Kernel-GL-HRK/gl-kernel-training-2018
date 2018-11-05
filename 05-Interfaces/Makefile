obj-m += interface_module_sysfs.o
obj-m += interface_module_procfs.o

all:
	make -C ${HEADERS_PATH} M=$(PWD) modules

clean:
	make -C ${HEADERS_PATH} M=$(PWD) clean
