#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int iParam;
module_param(iParam, int, 0);
MODULE_PARM_DESC(iParam, "iParam: ReturnCode - must be zero for successful module load");

char *sParam = "Default";
module_param(sParam, charp, 0);
MODULE_PARM_DESC(sParam, "sParam: string parameter");


static int __init oleghfm_init(void)
{
	printk(KERN_DEBUG "OlegH_Mod2: Hello world! from second module!\n");
	printk(KERN_DEBUG "OlegH_Mod2: sParam: %s\n", sParam);
	printk(KERN_DEBUG "OlegH_Mod2: iParam = %d\n", iParam);
	return iParam;
}

static void __exit oleghfm_exit(void)
{
	printk(KERN_DEBUG "OlegH_Mod2: Goodbye!\n");
}

module_init(oleghfm_init);
module_exit(oleghfm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("Olegh second linux module");
MODULE_VERSION("0.1");

