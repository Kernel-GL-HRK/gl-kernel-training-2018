// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#define MODULE_TAG			"LowerCase"

#define CLASS_NAME			"case_converter"

static unsigned long int mode;
module_param(mode, ulong, 0664);
MODULE_PARM_DESC(mode, "Memory allocation mode");

struct module_statistic {
	int use_cnt;
	int chars_processed;
	int chars_converted;
	int chars_alphabetic;
	int chars_numeric;
};
struct module_statistic statistic;

static char *msg_buffer;
static int msg_size;

#define CACHE_SIZE PAGE_SIZE
/*Memmory allocation types*/
#define SLAB_MODE 0
static struct kmem_cache *slab_mem;


static void convert_to_lowercase(void)
{
	int i;
	char temp;

	for (i = 0; i < msg_size; i++) {
		temp = tolower(msg_buffer[i]);
		if (temp != msg_buffer[i]) {
			statistic.chars_converted++;
			msg_buffer[i] = temp;
		}
		if (isalpha(temp))
			statistic.chars_alphabetic++;
		else if (isdigit(temp))
			statistic.chars_numeric++;
	}
	statistic.chars_processed += msg_size;
	statistic.use_cnt++;
}

static ssize_t numeric_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic.chars_numeric);

	return strlen(buf);
}

static ssize_t alphabetic_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic.chars_alphabetic);

	return strlen(buf);
}

static ssize_t used_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic.use_cnt);

	return strlen(buf);
}

static ssize_t converted_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic.chars_converted);

	return strlen(buf);
}

static ssize_t processed_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic.chars_processed);

	return strlen(buf);
}

static ssize_t buffer_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (msg_buffer == NULL)
		return 0;

	switch (mode) {
		case SLAB_MODE:
			memcpy(buf, msg_buffer, msg_size);
			kmem_cache_free(slab_mem, msg_buffer);
			msg_buffer = NULL;
			break;
	
		default:
			break;
	}	

	return msg_size;
}

static ssize_t buffer_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret_cnt;

	switch (mode) {
		case SLAB_MODE:
			if (msg_buffer == NULL) {
				msg_buffer = kmem_cache_alloc(slab_mem, GFP_KERNEL);
			}
			break;
	
		default:
			break;
	}

	if (count < PAGE_SIZE) {
		memcpy(msg_buffer, buf, count);
		msg_size = count;
		ret_cnt = count;
	} else {
		memcpy(msg_buffer, buf, PAGE_SIZE);
		msg_size = PAGE_SIZE;
		ret_cnt = count - PAGE_SIZE;
	}

	convert_to_lowercase();

	return ret_cnt;
}

CLASS_ATTR_RW(buffer);
CLASS_ATTR_RO(processed);
CLASS_ATTR_RO(converted);
CLASS_ATTR_RO(used);
CLASS_ATTR_RO(alphabetic);
CLASS_ATTR_RO(numeric);

static struct attribute *case_converter_class_attrs[] = {
	&class_attr_buffer.attr,
	&class_attr_processed.attr,
	&class_attr_converted.attr,
	&class_attr_used.attr,
	&class_attr_alphabetic.attr,
	&class_attr_numeric.attr,
	NULL,
};
ATTRIBUTE_GROUPS(case_converter_class);

static struct class module_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = case_converter_class_groups,
};

static int __init mod_init(void)
{
	int ret;

	ret = class_register(&module_class);
	if (ret < 0) {
		pr_err("%s: Can't register a class in sysfs\n", MODULE_TAG);
		return ret;
	}

	switch (mode) {
		case SLAB_MODE:
			slab_mem = kmem_cache_create(MODULE_TAG, CACHE_SIZE, 0, SLAB_HWCACHE_ALIGN | SLAB_POISON, NULL);
			if (slab_mem == NULL)
				goto fail;
			break;
		default:
			break;
	}

	pr_info("%s: module loaded\n", MODULE_TAG);
	return 0;

fail:
	class_unregister(&module_class);
	return -ENOMEM;
}

static void __exit mod_exit(void)
{
	class_unregister(&module_class);

	switch (mode) {
		case SLAB_MODE:
			if(msg_buffer != NULL) {
				kmem_cache_free(slab_mem, msg_buffer);
			}
			break;
	
		default:
			break;
	}

	if (slab_mem) kmem_cache_destroy(slab_mem);
	pr_info("%s: module unloaded\n", MODULE_TAG);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A to-lower-case converter module for sysfs");
MODULE_VERSION("2.0");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
