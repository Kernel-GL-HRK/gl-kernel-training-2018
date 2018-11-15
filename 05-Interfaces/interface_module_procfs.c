#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DIR_NAME  "my_if_module"
#define ENT1_NAME "conv"
#define ENT2_NAME "stat"

#define STR_LENTH PAGE_SIZE/8

static char procfs_char[PAGE_SIZE];
static ssize_t procfs_size;

static struct statistic {
	long char_processed;
	long char_returned;
	long char_received;
	long total_write_calls;
	long total_read_calls;
} procfs_stat;


ssize_t if_module_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	pr_info("procfs read func called\n");
	procfs_stat.total_read_calls++;

	ssize_t num, not_copied;

	num = min_t(ssize_t, procfs_size, count);
	if (num) {
		not_copied = copy_to_user(buff, procfs_char, num);
		num -= not_copied;
	}

	procfs_size = 0;
	return num;
}

ssize_t if_module_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	pr_info("procfs write func called\n");
	procfs_stat.total_write_calls++;

	ssize_t not_copied;

	char *msg = kmalloc(PAGE_SIZE, GFP_KERNEL);

	not_copied = copy_from_user(msg, buff, count);

	char *target = procfs_char;

	while (*msg) {
		procfs_stat.char_received++;
		if (islower(*msg)) {
			*target = toupper(*msg);
			procfs_stat.char_processed++;
		} else {
			*target = *msg;
		}
		target++;
		msg++;
	}

	procfs_size = count - not_copied;

	return procfs_size;
}

ssize_t if_stat_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	pr_info("procfs stat read func called\n");

	char line[STR_LENTH] = { 0 };
	char out[STR_LENTH] = { 0 };

	sprintf(line, "my_interface char processed: %ld,\n", procfs_stat.char_processed);
	strcat(out, line);
	sprintf(line, "             char returned: %ld,\n", procfs_stat.char_returned);
	strcat(out, line);
	sprintf(line, "             char received: %ld,\n", procfs_stat.char_received);
	strcat(out, line);
	sprintf(line, "             total write calls: %ld,\n", procfs_stat.total_write_calls);
	strcat(out, line);
	sprintf(line, "             total read calls: %ld\n", procfs_stat.total_read_calls);
	strcat(out, line);

	pr_info("%ld, %lld, %ld", count, *offp, strlen(out));

	if (strlen(out) > *offp) {
		copy_to_user(buff, out, strlen(out));
		return strlen(out);
	} else {
		return 0;
	}

}

static struct file_operations procfs_conv_fops = {
	.owner = THIS_MODULE,
	.read = if_module_read,
	.write = if_module_write,
};

static struct file_operations procfs_stat_fops = {
	.owner = THIS_MODULE,
	.read = if_stat_read,
};

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;

static int __init init_if_proc_module(void)
{
	pr_info("Init interface module\n");

	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating procfs directory\n");
		return -ENOMEM;
	}

	ent1 = proc_create(ENT1_NAME, 0666, dir, &procfs_conv_fops);
	if (ent1 == NULL) {
		pr_err("mymodule: error creating procfs conv\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	ent2 = proc_create(ENT2_NAME, 0444, dir, &procfs_stat_fops);
	if (ent2 == NULL) {
		pr_err("mymodule: error creating procfs stat\n");
		remove_proc_entry(ENT1_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void __exit exit_if_proc_module(void)
{

	proc_remove(dir);

	pr_info("mymodule: module exited\n");
	pr_info("Goodbye, cruel world!\n");
}

module_init(init_if_proc_module);
module_exit(exit_if_proc_module);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Perederii Serhii");
MODULE_DESCRIPTION("A module for procfs string processing");
MODULE_VERSION("0.2");
