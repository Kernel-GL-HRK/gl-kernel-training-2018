#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>


static char msg[PAGE_SIZE];
static ssize_t msg_size;


struct usageStatistic {
	int convertedCharacters;
	int numcallRead;
};

static struct usageStatistic stats;

static ssize_t mywrite(struct file *file,
			const char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	ssize_t not_copied;
	int index = 0;
	char character;

	pr_info("mymodule: mywrite: count=%d\n", count);

	not_copied = copy_from_user(msg, pbuf, count);

	msg_size = count - not_copied;

	while (msg[index]) {
		character = msg[index];
		msg[index] = (char)toupper(character);
		++index;
		++stats.convertedCharacters;
	}

	pr_info("mymodule: mywrite: msg_size=%d\n", msg_size);
	return msg_size;
}

static ssize_t myread(struct file *file,
			char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("mymodule: myread: count=%d\n", count);

	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		num -= not_copied;
	}

	msg_size = 0; /* Indicate EOF on next read */

	++stats.numcallRead;
	return num;
}

static ssize_t readstat(struct file *file,
			char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	char statisticMsg[512];

	sprintf(statisticMsg, "Lowercase converter statistics:\n"
	"Characters was converted: %d\n"
	"Show was called: %d\n",
	stats.convertedCharacters, stats.numcallRead);
	int len = strlen(statisticMsg);


	if (*ppos > 0 || count < len)
		return 0;

	if (copy_to_user(pbuf, statisticMsg, len))
		return -EFAULT;

	*ppos = len;

	return len;
}


static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static const struct file_operations statOps = {
	.owner = THIS_MODULE,
	.read = readstat,
};

static struct proc_dir_entry *ent;
static struct proc_dir_entry *stat;
static struct proc_dir_entry *parent;

static int mymodule_init(void)
{
	parent = proc_mkdir("strConverter", NULL);
	if (parent == NULL) {
		pr_err("mymodule: error creating procfs entry\n");
		return -ENOMEM;
	}

	ent = proc_create("procfs_upstring", 0666, parent, &myops);
	if (ent == NULL) {
		pr_err("mymodule: error creating procfs entry\n");
		return -ENOMEM;
	}

	stat = proc_create("stat", 0444, parent, &statOps);
	if (ent == NULL) {
		pr_err("mymodule: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(ent);
	proc_remove(stat);
	proc_remove(parent);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("Simple Kernel module use procfs");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
