#include <linux/init.h>      // Macros used to mark up functions  __init __exit
#include <linux/module.h>    // Core header for loading LKMs into the kernel
#include <linux/kernel.h>    // Contains types, macros, functions for the kernel
#include <linux/moduleparam.h>

static int will_return;

module_param(will_return, int, 0400);

static int __init hello_init(void)
{
	if (will_return)
		pr_err("Module load will fail\n");
	else
		pr_info("Module loaded\n");

	return will_return;
}

static void __exit hello_exit(void)
{
	pr_info("Module removed\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("Linux kernel module example");
MODULE_VERSION("0.1");

module_init(hello_init);
module_exit(hello_exit);
