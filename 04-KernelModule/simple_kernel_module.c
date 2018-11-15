#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konstantin E Felix");
MODULE_DESCRIPTION("First simple linux kenel module.");
MODULE_VERSION("0.1");

static int skm_param = -1;
module_param(skm_param, int, 0);

static int __init first_module(void){
		if(skm_param <0 || skm_param>5)
			return -EINVAL;

		printk(KERN_SOH"Run simple kernel module");
		return 0;
}

static void __exit first_module_exit(void){
		pr_warn("Unload simple kernel module");
}

module_init(first_module);
module_exit(first_module_exit);
