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
#include <linux/list.h>
#include <linux/mempool.h>

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

struct buffers_list_node {
	char *buffer;
	ssize_t len;
	struct list_head list;
};
static LIST_HEAD(buffers_list);
static struct buffers_list_node *last_readed;

static char *msg_buffer;
static int msg_size;

#define POOL_HOT_RESERV  3
#define CACHE_SIZE PAGE_SIZE
/*Memmory allocation types*/
#define SLAB_MODE 0
#define SLAB_MODE_WITH_PRESERVE 1 
#define POOL_MODE 2
static struct kmem_cache *slab_mem;
static mempool_t *pool_mem;

static void convert_to_lowercase(char *buf, int len)
{
	int i;
	char temp;

	for (i = 0; i < len; i++) {
		temp = tolower(buf[i]);
		if (temp != buf[i]) {
			statistic.chars_converted++;
			buf[i] = temp;
		}
		if (isalpha(temp))
			statistic.chars_alphabetic++;
		else if (isdigit(temp))
			statistic.chars_numeric++;
	}
	statistic.chars_processed += len;
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
	struct buffers_list_node *node;
	ssize_t ret_size;

	switch (mode) {
		case SLAB_MODE:
			if (msg_buffer == NULL)
				return 0;
			memcpy(buf, msg_buffer, msg_size);
			ret_size  = msg_size;
			kmem_cache_free(slab_mem, msg_buffer);
			msg_buffer = NULL;
			break;
		case SLAB_MODE_WITH_PRESERVE:
		case POOL_MODE:
			if (list_empty(&buffers_list)) {
				return 0;
			}
			node = list_entry(&last_readed->list, struct buffers_list_node, list);
			if(&node->list == &buffers_list) {
				return 0;
			}
			memcpy(buf, node->buffer, node->len);
			ret_size  = node->len;
			last_readed = list_prev_entry(node, list);
			break;
	
		default:
			break;
	}	

	return ret_size;
}

static ssize_t buffer_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret_cnt;
	struct buffers_list_node *blnode;
	char *pmsg;
	ssize_t *psize;

	switch (mode) {
		case SLAB_MODE:
			if (msg_buffer == NULL) {
				msg_buffer = kmem_cache_alloc(slab_mem, GFP_KERNEL);
				pmsg = msg_buffer;
				psize = &msg_size;
			}
			break;
		case SLAB_MODE_WITH_PRESERVE:
		case POOL_MODE:
			blnode = kzalloc(sizeof(struct buffers_list_node), GFP_KERNEL);
			if(mode == SLAB_MODE_WITH_PRESERVE)
				blnode->buffer = kmem_cache_alloc(slab_mem, GFP_KERNEL);
			else
				blnode->buffer = mempool_alloc(pool_mem, GFP_KERNEL);
			list_add_tail(&blnode->list, &buffers_list);
			pmsg = blnode->buffer;
			psize = &blnode->len;
			last_readed = blnode;
			break;
		default:
			break;
	}


	if (count < PAGE_SIZE) {
		memcpy(pmsg, buf, count);
		*psize = count;
		ret_cnt = count;
	} else {
		memcpy(pmsg, buf, PAGE_SIZE);
		*psize = PAGE_SIZE;
		ret_cnt = count - PAGE_SIZE;
	}

	convert_to_lowercase(pmsg, *psize);
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
		case SLAB_MODE_WITH_PRESERVE:
			slab_mem = kmem_cache_create(MODULE_TAG, CACHE_SIZE, 0, SLAB_HWCACHE_ALIGN | SLAB_POISON, NULL);
			if (slab_mem == NULL)
				goto fail;
			break;
		case POOL_MODE:
			slab_mem = kmem_cache_create(MODULE_TAG, CACHE_SIZE, 0, SLAB_HWCACHE_ALIGN | SLAB_POISON, NULL);
			if (slab_mem == NULL)
				goto fail;
			pool_mem = mempool_create_slab_pool(POOL_HOT_RESERV, slab_mem);
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
	struct buffers_list_node *node;

	class_unregister(&module_class);

	switch (mode) {
		case SLAB_MODE:
			if(msg_buffer != NULL) {
				kmem_cache_free(slab_mem, msg_buffer);
			}
			break;
		case SLAB_MODE_WITH_PRESERVE:
		case POOL_MODE:
			if (!list_empty(&buffers_list)) {
				list_for_each_entry_reverse(node, &buffers_list, list) {
					if (node->buffer) {
						if (mode == SLAB_MODE_WITH_PRESERVE)
							kmem_cache_free(slab_mem, node->buffer);
						else
							mempool_free(node->buffer, pool_mem);
					}
					kfree(node);
				}
			}
			if (mode == POOL_MODE)
				mempool_destroy(pool_mem);
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
