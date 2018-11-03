#include <linux/init.h>		//Macros used to mark up functions
							//__init __exit

#include <linux/module.h>	//Core header for loading LKMs into the kernel
#include <linux/moduleparam.h>
#include <linux/kernel.h>	//Contains types, macros, functions
							//for the kernel
#include <linux/err.h>


static int debug;
module_param(debug, int, 0000);

static int __init glModule_init(void)
{
	pr_info("Hello, world!\n");
	return debug;
}

static void __exit glModule_exit(void)
{
	pr_info("Goodbye, world!\n");
}


module_init(glModule_init);
module_exit(glModule_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Nikishyn");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
