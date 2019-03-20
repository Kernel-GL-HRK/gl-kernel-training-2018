// SPDX-License-Identifier: AFL-3.0
/*
 * Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * SysFS kernel module for LowerCase text files
 *
 * doc links:
 *   https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

// 2. Create a simple string lowercase converter using sysfs
// (store: input string, show: that string in lowercase).

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
char ConvBuff[PAGE_SIZE+1];

static struct class *attr_class;

struct ConvStat {
	unsigned int TotalCalls;
	unsigned int TotalChars;
	unsigned int CharsConverted;
} ConvStat;

#define SysFS_DIR_NAME   "omod5"
#define SysFS_RW_NAME    "tolower"
#define SysFS_STAT_NAME  "stat"

//--------------- SysFS read/write functions ----------------------

static ssize_t tolower_show(struct class *cl,
			struct class_attribute *attr,
			char *buf)
{
	int L;

	L = strlen(ConvBuff);
	pr_info("omod5 show (L=%d)\n", L);
	sprintf(buf, "%s\n", ConvBuff);
	return L;
}

static ssize_t tolower_store(struct class *cl,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	size_t s;
	int n;
	char ch;

	s = count;
	if (s >= sizeof(ConvBuff))
		s = sizeof(ConvBuff) - 1;

	memcpy(ConvBuff, buf, s);
	ConvBuff[s] = 0;

	//for (n = 0; n < s; n++)
	//	ConvBuff[n] = tolower(ConvBuff[n]);
	for (n = 0; n < s; n++) {
		ch = tolower(ConvBuff[n]);
		ConvStat.TotalChars++;
		if (ConvBuff[n] != ch) {
			ConvBuff[n] = ch;
			ConvStat.CharsConverted++;
		}
	}
	ConvStat.TotalCalls++;

	pr_info("omod5 store %lu bytes\n", count);

	return s;
}


static struct class_attribute class_attr_tolower = {
	.attr = { .name = SysFS_RW_NAME, .mode = 0666 },
	.show = tolower_show,
	.store = tolower_store
};


static ssize_t stat_show(struct class *cl,
			struct class_attribute *attr,
			char *buf)
{
	unsigned int L;

	sprintf(buf, "omod5 ToUpper statistics:\n"
			"Total Calls: %u\n"
			"Total chars processed: %u\n"
			"Total chars converted to uppercase: %u\n",
			ConvStat.TotalCalls,
			ConvStat.TotalChars,
			ConvStat.CharsConverted);

	L = strlen(buf);

	pr_info("omod5 stat show (L=%d)\n", L);
	return L;
}

static struct class_attribute class_attr_stat = {
	.attr = { .name = SysFS_STAT_NAME, .mode = 0444 },
	.show = stat_show,
};


static int __init omod_init(void)
{
	int ret;

	pr_info("omod5 SysFS ToLower v2 startup...\n");

	attr_class = class_create(THIS_MODULE, SysFS_DIR_NAME);
	if (attr_class == NULL) {
		pr_err("omod5: error creating sysfs class\n");
		return -EEXIST;
	}

	ret = class_create_file(attr_class, &class_attr_tolower);
	if (ret) {
		pr_err("omod5: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("omod5: error creating sysfs class stat attribute\n");
		class_remove_file(attr_class, &class_attr_tolower);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("omod5 SysFS ToLower module loaded.\n");
	return 0;
}

static void __exit omod_exit(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_tolower);
	class_destroy(attr_class);

	pr_info("omod5 SysFS ToUpper normal shutdown.\n");
}

module_init(omod_init);
module_exit(omod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("OlegH Lesson05 module: test sys_fs");
MODULE_VERSION("0.2");

