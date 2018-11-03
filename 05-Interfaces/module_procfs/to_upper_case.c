// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>

static int __init mod_init(void)
{
	pr_info("To-upper-case module loaded\n");
	return 0;
}

static void __exit mod_exit(void)
{
	pr_info("To-upper-case module unloaded\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A to-upper-case converter module for procfs");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
