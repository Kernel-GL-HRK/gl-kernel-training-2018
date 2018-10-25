// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int param;
module_param(param, int, 0444);
MODULE_PARM_DESC(param, "Return value for module_init. Default 0");

static int __init hello_init(void)
{
	pr_info("A Test module loaded!\n");
	pr_info("Lets try different log levels\n");

	pr_debug("This is DEBUG level\n");
	pr_info("This is INFO level\n");
	pr_notice("This is NOTICE level\n");
	pr_warn("This is WARNING level\n");
	pr_err("This is ERROR level\n");
	pr_crit("This is CRITICAL level\n");
	pr_alert("This is ALERT level\n");
	pr_emerg("This is EMERGENCY level\n");

	if (param > 0)
		param *= -1;

	return param;
}

static void __exit hello_exit(void)
{
	pr_info("A Test module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple module");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
