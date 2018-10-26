// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/init.h>	// Macros used to mark up functions  __init __exit
#include <linux/module.h>// Core header for loading LKMs into the kernel
#include <linux/kernel.h>// Contains types, macros, functions for the kernel

static int simple_param;
module_param(simple_param, int, 0);
MODULE_PARM_DESC(simple_param, "Return value for module_init. Default 0");

static int __init simple_init(void)
{
	pr_info("simple module loaded. param = %d\n", simple_param);
	if (simple_param > 0)
		simple_param *= -1;
	return simple_param;
}

static void __exit simple_exit(void)
{
	pr_info("Simple module unloaded\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Siruk");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
