// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/ctype.h>

#define MODULE_TAG			"UpperCase"

#define PROC_FS_FOLDER_NAME	"to_upper_case"
#define PROC_FS_BUFFER_NAME	"buffer"
#define PROC_FS_STAT_NAME	"stat"

struct module_statistic {
	int use_cnt;
	int chars_processed;
	int chars_converted;
	int chars_alphabetic;
	int chars_numeric;
};

struct module_statistic *statistic;
static char *msg_buffer, *stat_buffer;
static int msg_size, stat_size;

static void convert_to_uppercase(void)
{
	int i;
	char temp;

	for (i = 0; i < msg_size; i++) {
		temp = toupper(msg_buffer[i]);
		if (temp != msg_buffer[i]) {
			statistic->chars_converted++;
			msg_buffer[i] = temp;
		}
		if (isalpha(temp))
			statistic->chars_alphabetic++;
		else if (isdigit(temp))
			statistic->chars_numeric++;
	}
	statistic->chars_processed += msg_size;
	statistic->use_cnt++;
}

static void update_statisctic(void)
{
	stat_size = snprintf((char *)stat_buffer, PAGE_SIZE,
		"\n\"To-Upper-Case\" module usage:\n");
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Used count:       %d\n", statistic->use_cnt);
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Processed chars:  %d\n", statistic->chars_processed);
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Converted chars:  %d\n", statistic->chars_converted);
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Alphabetic chars: %d\n", statistic->chars_alphabetic);
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Numeric chars:    %d\n", statistic->chars_numeric);
	stat_size += snprintf((char *)stat_buffer + stat_size, PAGE_SIZE - stat_size,
		"Other chars:      %d\n\n",
		statistic->chars_processed - statistic->chars_numeric - statistic->chars_alphabetic);
}

static int stat_file_open(struct inode *inode, struct file *file)
{
	update_statisctic();
	return 0;
}

static ssize_t fswrite(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t not_readed;

	not_readed = copy_from_user(msg_buffer, pbuf, count);
	msg_size = count - not_readed;

	convert_to_uppercase();

	return msg_size;
}

static ssize_t fsread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;
	int check;
	char *pmessage = NULL;
	int *psize = NULL;

	check = strncmp(file->f_path.dentry->d_iname, PROC_FS_BUFFER_NAME, sizeof(PROC_FS_BUFFER_NAME));

	if (!check) {
		pmessage = msg_buffer;
		psize = &msg_size;
	} else {
		pmessage = stat_buffer;
		psize = &stat_size;
	}

	num = min_t(ssize_t, *psize, count);
	if (num) {
		not_copied = copy_to_user(pbuf, pmessage, num);
		num -= not_copied;
	}
	*psize = 0;

	return num;
}


static const struct file_operations stat_ops = {
	.owner = THIS_MODULE,
	.open = stat_file_open,
	.read = fsread,
};

static const struct file_operations buffer_ops = {
	.owner = THIS_MODULE,
	.read = fsread,
	.write = fswrite,
};

static int __init mod_init(void)
{
	static struct proc_dir_entry *folder_entry;
	static struct proc_dir_entry *stat_entry;
	static struct proc_dir_entry *buffer_entry;

	folder_entry = proc_mkdir(PROC_FS_FOLDER_NAME, NULL);
	if (folder_entry == NULL) {
		pr_err("%s: Error while creating proc_fs entry: %s\n",
			MODULE_TAG, PROC_FS_FOLDER_NAME);
		goto fail;
	}

	stat_entry = proc_create(PROC_FS_STAT_NAME, 0444, folder_entry, &stat_ops);
	if (stat_entry == NULL) {
		pr_err("%s: Error while creating proc_fs entry: %s\n",
			MODULE_TAG, PROC_FS_STAT_NAME);
		goto fail;
	}

	buffer_entry = proc_create(PROC_FS_BUFFER_NAME, 0666, folder_entry, &buffer_ops);
	if (buffer_entry == NULL) {
		pr_err("%s: Error while creating proc_fs entry: %s\n",
			MODULE_TAG, PROC_FS_BUFFER_NAME);
		goto fail;
	}

	statistic = kzalloc(sizeof(struct module_statistic), GFP_KERNEL);
	if (statistic == NULL) {
		pr_err("%s: Memory allocation error\n", MODULE_TAG);
		goto fail;
	}

	msg_buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (msg_buffer == NULL) {
		pr_err("%s: Memory allocation error\n", MODULE_TAG);
		goto fail;
	}

	stat_buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (stat_buffer == NULL) {
		pr_err("%s: Memory allocation error\n", MODULE_TAG);
		goto fail;
	}

	pr_info("%s: module loaded\n", MODULE_TAG);
	return 0;

fail:
	remove_proc_subtree(PROC_FS_FOLDER_NAME, NULL);
	kfree(statistic);
	kfree(msg_buffer);
	kfree(stat_buffer);
	return -ENOMEM;
}

static void __exit mod_exit(void)
{
	remove_proc_subtree(PROC_FS_FOLDER_NAME, NULL);
	kfree(statistic);
	kfree(msg_buffer);
	kfree(stat_buffer);

	pr_info("%s: module unloaded\n", MODULE_TAG);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A to-upper-case converter module for procfs");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
