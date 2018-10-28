obj-m += solution.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

load:
	sudo insmod solution.ko will_return=0
	sudo dmesg -c

load_fail:
	-sudo insmod solution.ko will_return=-1
	sudo dmesg -c

unload:
	sudo rmmod solution.ko
	sudo dmesg -c