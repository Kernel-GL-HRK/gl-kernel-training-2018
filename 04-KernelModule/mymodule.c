// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
		// Macros used to mark up functions  __init __exit
#include <linux/module.h>
		// Core header for loading LKMs into the kernel
#include <linux/kernel.h>
		// Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eugene.Ovdiyuk");
MODULE_DESCRIPTION("Simple Kernel module");
MODULE_VERSION("0.1");

char *parameter = "OK";

module_param(parameter, charp, 0);
		// < Param desc. charp=char ptr, S_IRUGO can be read
MODULE_PARM_DESC(parameter, "OK | Error: Only if parameter is Ok, this module will be loaded");

static int __init hello_init(void)
{
	static int code;
	static char *parameterOk = "OK";
	static char *parameterError = "Error";

	if (!strcmp(parameter, parameterOk))
		pr_info("mymodule: ********  OK: Module started    *********\n");
	if (!strcmp(parameter, parameterError)) {
		pr_info("mymodule: ********  Error: Module failed  *********\n");
		code = -38;  //* Function not implemented */
		}
pr_info("mymodule: parameter is: %s\n", parameter);

return code;
}

static void __exit hello_exit(void)
{
	pr_info("mymodule: *********Module closed**********\n");
}

module_init(hello_init);
module_exit(hello_exit);
