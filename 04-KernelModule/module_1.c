// SPDX-License-Identifier: GL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int param;
module_param(param, int, 0);
MODULE_PARM_DESC(param, "macro used for giving the description of variable");

static int __init hello_init(void)
{
	pr_info("module loadd\n");

	if (param > 0)
		pr_err("param > 0: ERROR %d", param);
		else if
			(param < 0) pr_err("param < 0: ERROR %d", param);
			else
				pr_debug("param=0: module successfully loaded");
	return param;
}

static void __exit hello_exit(void)
{
	pr_info("module unloaded\n");
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Dmitriy.Ganshin");
MODULE_DESCRIPTION("lesson_4 module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");



