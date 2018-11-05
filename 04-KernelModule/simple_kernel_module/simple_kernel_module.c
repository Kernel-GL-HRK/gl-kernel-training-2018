// SPDX-License-Identifier: GPL-1.0+

#include <linux/init.h>    // Macros used to mark up functions  __init __exit
#include <linux/module.h>  // Core header for loading LKMs into the kernel
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple kernel module.");
MODULE_VERSION("0.1");

static char *str = "default";
module_param(str, charp, 0);
MODULE_PARM_DESC(str, "String paramenter");

static int num = -1;
module_param(num, int, 0);
MODULE_PARM_DESC(num, "Integer paramenter greater than 0");

static int __init simple_kernel_init(void)
{
	int ret = 0;
	// KERN_EMERG KERN_ALERT KERN_CRIT KERN_ERR
	// KERN_WARNING KERN_NOTICE KERN_INFO KERN_DEBUG
	#define msg "Simple: debug print.\n"
	printk(KERN_EMERG "KERN_EMERG " msg);
	printk(KERN_ALERT "KERN_ALERT " msg);
	printk(KERN_CRIT "KERN_CRIT " msg);
	printk(KERN_ERR "KERN_ERR " msg);
	printk(KERN_WARNING "KERN_WARNING " msg);
	printk(KERN_NOTICE "KERN_NOTICE " msg);
	printk(KERN_DEBUG "KERN_DEBUG " msg);
	printk(KERN_INFO "KERN_INFO " msg);

	printk(KERN_INFO "Str param: %s\n", str);
	printk(KERN_INFO "Num param: %d\n", num);

	if (num <= 0) {
		printk(KERN_INFO "Invalid param: %d\n", num);
		ret = -EINVAL;
	} else {
		printk(KERN_INFO "Module loaded with param: %d\n", num);
	}
	return ret;
}

static void __exit simple_kernel_exit(void)
{
	printk(KERN_INFO "Simple: Module exited.\n");
}


module_init(simple_kernel_init);
module_exit(simple_kernel_exit);
