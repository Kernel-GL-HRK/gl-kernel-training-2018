#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>

static int par = -1;
module_param(par, int, 0444);

static int mymodule_init(void)
{
	pr_info("mymodule: module loaded\n");
	pr_info("mymodule: myparam = %d\n", par);
	return par;
}

static void mymodule_exit(void)
{
	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_DESCRIPTION("Very sample module");
MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmai.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

