// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#define MODULE_TAG			"Timer"
#define CLASS_NAME			"Timer"


static int __init mod_init(void)
{
	pr_info("%s: module loaded\n", MODULE_TAG);
	return 0;
}

static void __exit mod_exit(void)
{
	pr_info("%s: module unloaded\n", MODULE_TAG);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A timer module for sysfs");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
