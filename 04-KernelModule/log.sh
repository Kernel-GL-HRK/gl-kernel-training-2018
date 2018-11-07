make
for var1 in 0 1 2 3 4 5 6 7 8
do
	insmod hello_printk.ko myparam=$var1
	rmmod hello_printk.ko
	dmesg -c
done

