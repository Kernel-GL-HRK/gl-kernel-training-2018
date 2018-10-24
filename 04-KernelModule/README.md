<<<<<<< 22bd2a9fd2818021980cd8380c2ed5e9562f3e1f
# Kernel module

## Homework
=======
# Homework:
>>>>>>> 04-KernelModule: Add homework task

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
