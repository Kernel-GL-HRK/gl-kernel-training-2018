#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Illia Iorin");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");

static int myparam = -1;
module_param(myparam, int, 0);

static int __init hello_init(void)
{
	if (myparam > -1 && myparam < 8)
		printk(KERN_SOH"%d Hello, world! my param=%d\n",
		 myparam, myparam);
	return -myparam;
}

static void __exit hello_exit(void)
{
	pr_warn("Goodbye, world!\n");
}


module_init(hello_init);
module_exit(hello_exit);
