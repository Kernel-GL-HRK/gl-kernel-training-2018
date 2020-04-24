// SPDX-License-Identifier: GPL-3.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define FOLDER_NAME  "mymodule_procfs"
#define ENT_NAME "rw"
#define STAT_NAME "statistic"
#define MAXLEN PAGE_SIZE

static char strValue[MAXLEN];
static char msg[MAXLEN];
static ssize_t msg_size;
 static int statistic_show_read_flag;

static struct obj_statisic
{
	int total_calls;
	int total_characters;
	int characters_not_converted;
	int characters_converted;
}
statisic;

static ssize_t rw_store(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	int i;
	size_t cpy_count;

	statisic.total_calls++;
	pr_info("mymodule_procfs: rw_store: count=%d\n", count);
	not_copied = copy_from_user(msg, buf, count);
	msg_size = count - not_copied;
	pr_info("mymodule_procfs: rw_store: msg_size=%d\n", msg_size);
	cpy_count = (count < MAXLEN) ? count : MAXLEN;
	strncpy(strValue, buf, cpy_count);
	pr_info("mymodule_procfs: rw_store: called with %s\n", strValue);
	strValue[cpy_count] = '\0';
	i=1;
	while (strValue[i]) {
		strValue[i] = toupper(strValue[i]);
		statisic.total_characters++;
		if ((strValue[i] >= 'a' && strValue[i] <= 'z') || \
			(strValue[i] >= 'A' && strValue[i] <= 'Z'))
				statisic.characters_converted++;
		i++;
	}
	statisic.characters_not_converted =
			statisic.total_characters - statisic.characters_converted;

	pr_info("mymodule_procfs: rw_store: value = %s\n", strValue);

	return msg_size;
}

static ssize_t rw_show(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	statisic.total_calls++;
	pr_info("mymodule_procfs: rw_show: count=%d\n", count);
	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(buf, msg, num);
		num -= not_copied;
	}
	msg_size = 0; /* Indicate EOF on next read */
	pr_info("mymodule_procfs: rw_show: return=%d\n", num);
	pr_info("mymodule_procfs: rw_show: value = %s\n", strValue);
	sprintf(buf, "%s\n", strValue);
	return num;
}

static ssize_t statistic_show(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	static char temp_buf[MAXLEN];
	
	if (statistic_show_read_flag)
		return 0;

	statisic.total_calls++;
	pr_info("mymodule_sysfs: statistic show");
	sprintf(temp_buf, 
	"		.total_calls = %d\n"
	"		.total_characters = %d\n"
	"		.characters_converted = %d\n"
	"		.characters_not_converted = %d\n",
	statisic.total_calls, statisic.total_characters, 
	statisic.characters_converted, statisic.characters_not_converted);
	//strcat(buf, temp_buf);
	if (copy_to_user(buf, temp_buf, strlen(temp_buf)))
		return -EFAULT;
 	statistic_show_read_flag = 1;
	return strlen(buf);
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = rw_show,
	.write = rw_store,
};
static struct file_operations mystat =
{
	.owner = THIS_MODULE,
	.read = statistic_show,
};

static struct proc_dir_entry *folder_ent;
static struct proc_dir_entry *ent;
static struct proc_dir_entry *ent_stat;

static int mymodule_init(void)
{
	folder_ent = proc_mkdir(FOLDER_NAME, NULL);
	if (folder_ent == NULL) {
		pr_err("mymodule_procfs: error creating mymodule_procfs folder\n");
	}
	ent = proc_create(ENT_NAME, 0666, folder_ent, &myops);
	if (ent == NULL) {
		pr_err("mymodule_procfs: error creating procfs entry\n");
		remove_proc_entry(FOLDER_NAME, NULL);
		return -ENOMEM;
	}
	ent_stat = proc_create(STAT_NAME, 0444, folder_ent, &mystat);
	if (ent_stat == NULL) {
		pr_err("mymodule_procfs: error creating procfs statisic\n");
		remove_proc_entry(STAT_NAME, NULL);
		remove_proc_entry(FOLDER_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule_procfs: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(ent);
	proc_remove(ent_stat);
	proc_remove(folder_ent);
	pr_info("mymodule_procfs: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Eugene.Ovdiyuk <ovdiyuk@gmail.com>");
MODULE_DESCRIPTION("procfs simple Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
