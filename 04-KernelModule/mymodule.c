#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>

int myparamint = EPERM;
module_param(myparamint, int, 0000);

static int __init init_mymodule(void)
{
	pr_info("init mymodule with internal param value=%d\n", myparamint);
	return -myparamint;
}

static void __exit exit_mymodule(void)
{
{
	pr_info("Goodbye, cruel world!\n");
}

module_init(init_mymodule);
module_exit(exit_mymodule);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Perederii Serhii");
MODULE_DESCRIPTION("A simple module with exit code as param");
MODULE_VERSION("0.1");
