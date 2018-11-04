#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>


static int converter_init(void)
{
	pr_info("module loaded\n");
	return 0;
}

static void converter_exit(void)
{
	pr_info("module removed from kernel\n");
}

module_init(converter_init);
module_exit(converter_exit);

MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("String converter kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
