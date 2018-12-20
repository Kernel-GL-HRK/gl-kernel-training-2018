// SPDX-License-Identifier: GPL-2.0
#define DEBUG 1
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int param = 5;
module_param(param, int, 0444);
MODULE_PARM_DESC(param, "Delay time. Default 5 sec");



static int __init tmod_init(void)
{
	pr_debug("Loading the test module\n");

	pr_debug("Test module loaded\n");
	return 0;
}




static void __exit tmod_exit(void)
{
	pr_debug("Test module unloaded\n");
}


module_init(tmod_init);
module_exit(tmod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Thread test module");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
