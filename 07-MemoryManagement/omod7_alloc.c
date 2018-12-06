// SPDX-License-Identifier: AFL-3.0
/*
 * Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * Module for testing kernel memory allocators
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

#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/list.h>


/*
 * 1. Update uppercase converter from the [lesson #05 (Interfaces)](/05-Interfaces)
 *   to dynamically allocate memory for each writing  using SLAB allocator.
 * 2. Update the module to preserve all previously written values:
 *    * Consecuent readings without writing should return converted values in a row starting from the last one.
 * 3. Transform the module to use memory pool for allocation.
 * 4. Implement dynamic changing of buffer size:
 *    * Set it to the size of the first written value (aligned to the nearest power of 2);
 *    * When written value exeeds element size - create new pool and move all existent elements to it.
 */

// --------------- MODULE PARAMETERS DESCRIPTION -------------------
//int iParam;
//module_param(iParam, int, 0);
//MODULE_PARM_DESC(iParam, "iParam: ReturnCode");

//char *sParam = "Default";
//module_param(sParam, charp, 0);
//MODULE_PARM_DESC(sParam, "sParam: string parameter");

//------------ GLOBAL MODULE DATA ---------------------
struct kmem_cache *pmycache;

#define SLAB_BUFF_SIZE   256
#define SLAB_NAME     "omod7"

struct store_fragment {
	struct list_head fragment;
	struct list_head block;
	size_t datasize;
	char data[1];
};

#define BLOCK_MAX_DATA_SIZE  (SLAB_BUFF_SIZE - (sizeof(struct store_fragment) - 1))

struct store_fragment *fragments;

//SysFS
static struct class *attr_class;

#define SysFS_DIR_NAME   "omod7"
#define SysFS_RW_NAME    "tolower"
#define SysFS_STAT_NAME  "stat"

//statistics
struct ConvStat {
	unsigned int TotalCalls;
	unsigned int TotalChars;
	unsigned int CharsConverted;
} ConvStat;

//--------------- SysFS read/write functions ----------------------

void FreeAllFragments(void)
{
	struct store_fragment *fp, *fpb, *next, *bnext;

	fp = fragments;
	while (fp) {
		next = list_entry(fp->fragment.next, struct store_fragment, fragment);
		fpb = fp;
		for (;;) {
			bnext = list_entry(fpb->block.next, struct store_fragment, block);
			kmem_cache_free(pmycache, fpb);
			fpb = bnext;
			if (fpb == fp)
				break;
		}
		fp = next;
		if (fp == fragments)
			break;
	}

	fragments = NULL;
}


static ssize_t tolower_show(struct class *cl,
			struct class_attribute *attr,
			char *buf)
{
	size_t s = 0;
	struct store_fragment *fp, *fpb;

	fp = fragments;
	while (fp) {
		fpb = fp;
		for (;;) {
			if (s + fpb->datasize > PAGE_SIZE) {
				pr_err("omod7 show ret too long!\n");
				return s;
			}
			memcpy(buf, fpb->data, fpb->datasize);
			buf += fpb->datasize;
			s += fpb->datasize;
			fpb = list_entry(fpb->block.next, struct store_fragment, block);
			if (fpb == fp)
				break;
		}
		fp = list_entry(fp->fragment.next, struct store_fragment, fragment);
		if (fp == fragments)
			break;
	}

	pr_info("omod7 show (s=%d)\n", (int)s);
	return s;
}


void CopyAndProcessData(char *dest, const char *src, size_t N)
{
	char sch, dch;
	size_t n;

	for (n = 0; n < N; n++) {
		sch = src[n];
		dch = tolower(sch);
		dest[n] = dch;
		ConvStat.TotalChars++;
		if (dch != sch)
			ConvStat.CharsConverted++;
	}
}

static ssize_t tolower_store(struct class *cl,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	size_t s, bls;
	struct store_fragment *fp, *fp0;
	void *slabbuf;

	if (count == 1 && *buf == '\n') {
		FreeAllFragments();
		pr_info("omod7 free all!\n");
		ConvStat.TotalCalls++;
		return count;
	}

	s = count;
	fp0 = NULL;

	while (s) {
		// alloc new buff
		bls = s;
		if (bls > BLOCK_MAX_DATA_SIZE)
			bls = BLOCK_MAX_DATA_SIZE;
		slabbuf = kmem_cache_alloc(pmycache, GFP_KERNEL);
		if (slabbuf == NULL) {
			pr_err("omod7: SLAB alloc failed");
			return 0;
		}
		fp = (struct store_fragment *) slabbuf;
		INIT_LIST_HEAD(&fp->fragment);
		INIT_LIST_HEAD(&fp->block);
		fp->datasize = bls;
		if (fp0 == NULL) {
			// first block in fragment - register it in fragments list
			if (fragments == NULL) {
				fragments = fp;
			} else {
				list_add_tail(&fp->fragment, &fragments->fragment);
				fragments = fp;
			}
			fp0 = fp;
			pr_info("omod7 add block0 for fragment (%lu bytes)", bls);
		} else {
			// put the block into blocks list
			list_add_tail(&fp->block, &fp0->block);
			pr_info("omod7 add next block (%lu bytes)", bls);
		}
		// copy bls bytes of data into block and process them
		CopyAndProcessData(fp->data, buf, bls);
		buf += bls;
		s -= bls;
	}

	pr_info("omod7 store %lu bytes\n", count);
	ConvStat.TotalCalls++;

	return count;
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

	sprintf(buf, "omod7 ToLower statistics:\n"
			"Total Calls: %u\n"
			"Total chars processed: %u\n"
			"Total chars converted to lowercase: %u\n",
			ConvStat.TotalCalls,
			ConvStat.TotalChars,
			ConvStat.CharsConverted);

	L = strlen(buf);

	pr_info("omod7 stat show (L=%d)\n", L);
	return L;
}

static struct class_attribute class_attr_stat = {
	.attr = { .name = SysFS_STAT_NAME, .mode = 0444 },
	.show = stat_show,
};


static int __init omod_init(void)
{
	int ret;

	pr_info("omod7 SysFS ToLower v3 startup...\n");

	fragments = NULL;

	pmycache = kmem_cache_create(SLAB_NAME, SLAB_BUFF_SIZE, 0 /*Alignment */, 0, NULL);
	if (pmycache == NULL) {
		pr_err("omod7: kmem_cache_create error\n");
		return -ENOMEM;
	}

	attr_class = class_create(THIS_MODULE, SysFS_DIR_NAME);
	if (attr_class == NULL) {
		pr_err("omod7: error creating sysfs class\n");
		kmem_cache_destroy(pmycache);
		return -EEXIST;
	}

	ret = class_create_file(attr_class, &class_attr_tolower);
	if (ret) {
		pr_err("omod7: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		kmem_cache_destroy(pmycache);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("omod7: error creating sysfs class stat attribute\n");
		class_remove_file(attr_class, &class_attr_tolower);
		class_destroy(attr_class);
		kmem_cache_destroy(pmycache);
		return ret;
	}

	pr_info("omod7 SysFS ToLower module loaded.\n");
	return 0;
}

static void __exit omod_exit(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_tolower);
	class_destroy(attr_class);

	FreeAllFragments();
	kmem_cache_destroy(pmycache);

	pr_info("omod7 SysFS ToLower normal shutdown.\n");
}

module_init(omod_init);
module_exit(omod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("OlegH Lesson07 module: kernel mem alloc");
MODULE_VERSION("0.1");

