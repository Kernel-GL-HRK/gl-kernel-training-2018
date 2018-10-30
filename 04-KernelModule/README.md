# Homework:

Create a simple Kernel module:

1. Create a simple module (source and makefile).
2. Add some debug prints.
3. Check your source with the checkpatch.pl
4. Add module parameter and implement different module_init return codes dependent on this parameter (OK/Error).
5. Get kernel logs for both cases. And add them to projects.

Example of log:
```
$ sudo dmesg -c
...
< start log >
$ sudo insmod mymodule.ko
$ sudo lsmod | grep mymodule
$ sudo rmmod mymodule
$ sudo dmesg
< end log >
```

# Finished homework:

To build the module on Ubuntu 16.04:
1. Execute following command to install all necessary tools and libs
```
sudo apt-get install build-essential linux-headers-`uname -r`
```
2. Run "make" in this folder
3. Run "insmod mymodule.ko myparamint=0" to check
