#include <linux/init.h>      // Macros used to mark up functions  __init __exit
#include <linux/module.h>    // Core header for loading LKMs into the kernel
#include <linux/kernel.h>    // Contains types, macros, functions for the kernel

static int mycode = 10;
module_param(mycode, int, 0660);

static int __init toy_init(void)
{
	pr_warn("I've got into sandbox!\n");
	if (mycode > 100) {
		pr_warn("Code %d > 100 - I will not play here!\n", mycode);
		return -ERANGE;
	}
	return 0;
}

static void __exit toy_exit(void)
{
	pr_warn("I'm going home!\n");
}


module_init(toy_init);
module_exit(toy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksii Khmara");
MODULE_DESCRIPTION("A simple Linux driver - first toy in the sandbox");
MODULE_VERSION("0.1");
