// SPDX-License-Identifier: 123
/* Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.
 *
 * first test kernel module
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init oleghfm_init(void)
{
	printk(KERN_DEBUG "Hello world! from olegh first module!\n");
	return 0;
}

static void __exit oleghfm_exit(void)
{
	printk(KERN_DEBUG "Olegh first kernel module says to you: Goodbye!\n");
}

module_init(oleghfm_init);
module_exit(oleghfm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("Olegh first linux driver");
MODULE_VERSION("0.1");

