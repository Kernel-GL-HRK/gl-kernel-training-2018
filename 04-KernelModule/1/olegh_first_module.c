// SPDX-License-Identifier: AFL 3.0
/* Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * first test kernel module - just debug messages
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init oleghfm_init(void)
{
	pr_debug("Hello world! from olegh first module!\n");
	return 0;
}

static void __exit oleghfm_exit(void)
{
	pr_debug("Olegh first kernel module says to you: Goodbye!\n");
}

module_init(oleghfm_init);
module_exit(oleghfm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("Olegh first linux driver");
MODULE_VERSION("0.1");

