// SPDX-License-Identifier: (GPL-2.0 OR MIT)
//
// mymodule.c
//
// (C) Copyright 2018
// Kharkiv Ukraine, Ivan Parfyonov
//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>
//#include <linux/printk.h>

static int myparam = -1;
module_param(myparam, int, 0);

static int __init mymodule_init(void)
{
	pr_info("mymodule: module loaded\n");
	pr_info("mymodule: myparam = %d\n", myparam);
	return myparam;
}

static void __exit mymodule_exit(void)
{
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Ivan Parfonov <ivan.parfonov@globallogic.com>");
MODULE_DESCRIPTION("Simple Kernel module.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
