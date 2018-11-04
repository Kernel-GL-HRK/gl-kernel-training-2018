// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>

#define MODULE_TAG			"LowerCase"

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
MODULE_DESCRIPTION("A to-lower-case converter module for sysfs");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
