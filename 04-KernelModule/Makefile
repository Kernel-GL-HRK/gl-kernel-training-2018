obj-m+=mymodule.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

# obj-m+=mymodule.o

# all:
# 	make -C $(BUILD_KERNEL) M=$(PWD) modules
# clean:	
# 	make -C $(BUILD_KERNEL) M=$(PWD) clean