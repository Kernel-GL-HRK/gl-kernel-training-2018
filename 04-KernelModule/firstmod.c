#include <linux/init.h>	// Macros used to mark up functions  __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Storozhenko");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");

static int value;
module_param(value, int, S_IRUGO);

static int __init hello_init(void)
{
	printk(KERN_DEBUG "Hello, world!\n");
	printk(KERN_DEBUG "value: %i\n", value);

	if (value > 0)
		return -EINVAL;
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_DEBUG "Goodbye, world!\n");
}


module_init(hello_init);
module_exit(hello_exit);

