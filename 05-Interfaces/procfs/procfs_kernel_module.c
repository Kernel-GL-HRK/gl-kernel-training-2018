// SPDX-License-Identifier: GPL-1.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

enum eStat {CALLS, PROCESSED, CONVERTED};

struct module_data {
	char msg[PAGE_SIZE];
	ssize_t msg_size;
} module_data_t;

struct module_stat {
	ssize_t stat_calls;
	ssize_t stat_processed;
	ssize_t stat_converted;
	bool stat_updated;
} module_stat_t;

static struct module_data data;
static struct module_stat stat;

static void inc_stat(int type)
{
	switch (type) {
	case CALLS:
		++stat.stat_calls;
	break;
	case PROCESSED:
		++stat.stat_processed;
	break;
	case CONVERTED:
		++stat.stat_converted;
	break;
	}
	stat.stat_updated = true;
}

static void reset_stat(void)
{
	stat.stat_calls = 0;
	stat.stat_processed = 0;
	stat.stat_converted = 0;
	stat.stat_updated = false;
}

static char to_upper(const char lower)
{
	char upper;

	if (lower >= 'a' && lower <= 'z') {
		upper = lower + 'A' - 'a';
		inc_stat(CONVERTED);
	} else {
		upper = lower;
	}
	return upper;
}

static ssize_t data_read(struct file *file, char __user *pbuf, size_t count,
			loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("mymodule: myread: count=%d\n", count);

	num = min_t(ssize_t, data.msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, data.msg, num);
		num -= not_copied;
	}

	data.msg_size = 0; /* Indicate EOF on next read */

	pr_info("mymodule: myread: return=%d\n", num);
	return num;
}

static ssize_t data_write(struct file *file, const char __user *pbuf,
			size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	int i;

	pr_info("mymodule: mywrite: count=%d\n", count);
	not_copied = copy_from_user(data.msg, pbuf, count);
	data.msg_size = count - not_copied;
	pr_info("mymodule: mywrite: msg_size=%d\n", data.msg_size);

	inc_stat(CALLS);
	for (i = 0; i < data.msg_size - 1; i++)	{
		data.msg[i] = to_upper(data.msg[i]);
		inc_stat(PROCESSED);
	}

	return data.msg_size;
}

static ssize_t stat_read(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;
	char stat_msg[PAGE_SIZE];
	size_t stat_msg_size = 0;

	if (!stat.stat_updated)
		return 0;

	pr_info("mymodule: myread: count=%d\n", count);

	stat_msg_size = sprintf(stat_msg, "Calls:     %d\n"
				"Processed: %d\n"
				"Converted: %d \n",
				stat.stat_calls,
				stat.stat_processed,
				stat.stat_converted);

	num = min_t(ssize_t, stat_msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, stat_msg, stat_msg_size);
		num -= not_copied;
	}

	stat.stat_updated = false; // Indicate EOF on next read

	pr_info("mymodule: myread: return=%d\n", num);
	return num;
}

static const struct file_operations rw_ops = {
	.owner = THIS_MODULE,
	.read = data_read,
	.write = data_write,
};

static const  struct file_operations r_ops = {
	.owner = THIS_MODULE,
	.read = stat_read,
	.write = NULL,
};

#define DIR_NAME	"procfs_kernel_module"
#define ENT1_NAME	"data"
#define ENT2_NAME	"stat"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent_data, *ent_stat;

static int __init procfs_kernel_init(void)
{
	reset_stat();
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating procfs directory\n");
		return -ENOMEM;
	}

	ent_data = proc_create(ENT1_NAME, 0444, dir, &rw_ops);
	if (ent_data == NULL) {
		pr_err("mymodule: error creating procfs entry 1\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	ent_stat = proc_create(ENT2_NAME, 0444, dir, &r_ops);
	if (ent_stat == NULL) {
		pr_err("mymodule: error creating procfs entry 2\n");
		remove_proc_entry(ENT1_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void __exit procfs_kernel_exit(void)
{
	proc_remove(dir);
	printk(KERN_INFO "mymodule: Module exited.\n");
}


module_init(procfs_kernel_init);
module_exit(procfs_kernel_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple kernel module.");
MODULE_VERSION("0.1");
