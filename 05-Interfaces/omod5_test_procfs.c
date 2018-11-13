// SPDX-License-Identifier: AFL-3.0
/*
 * Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * ProcFS kernel module for Capitalize text files.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

// 1. Create a simple string uppercase converter using procfs
// (write: input string, read: that string in uppercase).

// 3. Add read only attributes for both filesystems showing statistics
// (using cat): e.g. total calls, total characters processed,
// characters converted etc).


// --------------- MODULE PARAMETERS DESCRIPTION -------------------
//int iParam;
//module_param(iParam, int, 0);
//MODULE_PARM_DESC(iParam, "iParam: ReturnCode");

//char *sParam = "Default";
//module_param(sParam, charp, 0);
//MODULE_PARM_DESC(sParam, "sParam: string parameter");

//------------ GLOBAL MODULE DATA ---------------------
#define CONV_BUFF_MAX_LEN  65536
char ConvBuff[CONV_BUFF_MAX_LEN];
uint32_t ConvSize;
uint32_t ConvPos;

struct ConvStat {
	unsigned int TotalCalls;
	unsigned int TotalChars;
	unsigned int CharsConverted;
} ConvStat;

static struct proc_dir_entry *procfs_dir;
static struct proc_dir_entry *procfs_ent_rw;
static struct proc_dir_entry *procfs_ent_stat;

#define PROCFS_DIR_NAME   "omod5"
#define PROCFS_RW_NAME    "toupper"
#define PROCFS_STAT_NAME  "stat"

//--------------- PROCFS read/write functions ----------------------

static ssize_t procfs_read(struct file *file,
			char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	size_t s, p, not_copied;

	p = ConvPos;
	if (p > ConvSize)
		p = ConvSize;
	s = count;
	if (s > ConvSize - p)
		s = ConvSize - p;
	if (s) {
		not_copied = copy_to_user(pbuf, ConvBuff + p, s);
		s -= not_copied;
		ConvPos += s;
	}
	pr_info("omod5 read %lu bytes, result: %u\n", count, (uint32_t)s);
	return s;
}

static ssize_t procfs_write(struct file *file,
			const char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	char *p;
	uint32_t c, n;
	char ch;

	if (ConvPos < ConvSize)
		return 0;

	c = count;
	if (ConvSize + c > sizeof(ConvBuff))
		c = sizeof(ConvBuff) - ConvSize;

	p = ConvBuff + ConvSize;
	count -= copy_from_user(p, pbuf, c);
	ConvSize += c;

	for (n = 0; n < c; n++) {
		ch = toupper(p[n]);
		ConvStat.TotalChars++;
		if (p[n] != ch) {
			p[n] = ch;
			ConvStat.CharsConverted++;
		}
	}

	ConvPos = ConvSize;
	pr_info("omod5 write request %lu bytes, Stored: %u, In Buff: %u\n",
		count, c, ConvSize);
	proc_set_size(procfs_ent_rw, ConvSize);

	return c;
}

static loff_t procfs_llseek(struct file *file, loff_t ofs, int from)
{
	loff_t s;

	s = ConvPos;
	switch (from) {
	case SEEK_SET:
		s = ofs;
		break;
	case SEEK_CUR:
		s = ConvPos + ofs;
		break;
	case SEEK_END:
		s = ConvSize + ofs;
		break;
	}
	if (s > ConvSize)
		s = ConvSize;
	ConvPos = (uint32_t)s;

	pr_info("omod5 seek %u, %d, Result:%u\n", (uint32_t)ofs, from, ConvPos);

	return ConvPos;
}

//  https://linux-kernel-labs.github.io/master/labs/device_drivers.html
static int procfs_open(struct inode *in, struct file *f)
{
	pr_info("omod5 open. mode: %X (%s)\n", (uint32_t)f->f_mode,
		(f->f_mode & FMODE_WRITE) ? "WRITE" :
			(f->f_mode & FMODE_READ) ? "READ" : "?");
	if (f->f_mode & FMODE_WRITE) {
		ConvStat.TotalCalls++;
		if (f->f_flags & O_APPEND) {
			ConvPos = ConvSize;
		} else {
			ConvSize = ConvPos = 0;
		}
	} else if (f->f_mode & FMODE_READ) {
		ConvPos = 0;
	}
	return 0;
}

static const struct file_operations omod5_fops_rw = {
	.owner = THIS_MODULE,
	.read = procfs_read,
	.write = procfs_write,
	.llseek = procfs_llseek,
	.open = procfs_open,
};


static ssize_t procfs_stat_read(struct file *file,
			char __user *pbuf,
			size_t count,
			loff_t *ppos)
{
	static char TmpBuff[PAGE_SIZE];
	unsigned int N;

	if (memcmp(TmpBuff, "omod5", 5) == 0) {
		TmpBuff[0] = 0;
		return 0;
	}

	sprintf(TmpBuff, "omod5 ToUpper statistics:\n"
			"Total Calls: %u\n"
			"Total chars processed: %u\n"
			"Total chars converted to uppercase: %u\n",
			ConvStat.TotalCalls,
			ConvStat.TotalChars,
			ConvStat.CharsConverted);
	N = strlen(TmpBuff);
	copy_to_user(pbuf, TmpBuff, N);
	pr_info("omod5 stat read %lu bytes, result: %u\n", count, N);
	return N;
}

static const struct file_operations omod5_fops_stat = {
	.owner = THIS_MODULE,
	.read = procfs_stat_read,
};

static int __init omod_init(void)
{
	pr_info("omod5 ProcFS ToUpper v2 startup...\n");
	procfs_dir = proc_mkdir(PROCFS_DIR_NAME, NULL);
	if (procfs_dir == NULL) {
		pr_err("omod5: error creating proc-fs dir \'"
			PROCFS_DIR_NAME "\'\n");
		return -EEXIST;
	}

	procfs_ent_rw = proc_create(PROCFS_RW_NAME,
			0666, procfs_dir, &omod5_fops_rw);
	if (procfs_ent_rw == NULL) {
		pr_err("omod5: error creating proc-fs entry \'" PROCFS_DIR_NAME
			"\\" PROCFS_RW_NAME "\'\n");
		remove_proc_entry(PROCFS_DIR_NAME, NULL);
		return -ENOMEM;
	}

	procfs_ent_stat = proc_create(PROCFS_STAT_NAME,
			0666, procfs_dir, &omod5_fops_stat);
	if (procfs_ent_stat == NULL) {
		pr_err("omod5: error creating proc-fs entry \'" PROCFS_DIR_NAME
			"\\" PROCFS_STAT_NAME "\'\n");
		proc_remove(procfs_dir);
		return -ENOMEM;
	}

	pr_info("omod5 ProcFS ToUpper module loaded\n");
	return 0;
}

static void __exit omod_exit(void)
{
	proc_remove(procfs_dir);
	pr_info("omod5 ProcFS ToUpper normal shutdown.\n");
}

module_init(omod_init);
module_exit(omod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("OlegH Lesson05 module: test proc_fs");
MODULE_VERSION("0.2");

