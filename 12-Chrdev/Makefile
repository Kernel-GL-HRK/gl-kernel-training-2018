obj-m += tic120bmp.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


install:
	sudo cp tic120bmp.ko /lib/modules/4.14.84-sunxi/kernel/drivers/i2c/
	sudo depmod -a




