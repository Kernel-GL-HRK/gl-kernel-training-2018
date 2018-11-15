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

struct module_statistic {
	int use_cnt;
	int chars_processed;
	int chars_converted;
	int chars_alphabetic;
	int chars_numeric;
};

struct module_statistic *statistic;
static char *msg_buffer;
static int msg_size;


static void convert_to_lowercase(void)
{
	int i;
	char temp;

	for (i = 0; i < msg_size; i++) {
		temp = tolower(msg_buffer[i]);
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

static ssize_t numeric_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic->chars_numeric);

	return strlen(buf);
}

static ssize_t alphabetic_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic->chars_alphabetic);

	return strlen(buf);
}

static ssize_t used_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic->use_cnt);

	return strlen(buf);
}

static ssize_t converted_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic->chars_converted);

	return strlen(buf);
}

static ssize_t processed_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", statistic->chars_processed);

	return strlen(buf);
}

static ssize_t buffer_show(struct class *class, struct class_attribute *attr, char *buf)
{
	memcpy(buf, msg_buffer, msg_size);
	return msg_size;
}

static ssize_t buffer_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret_cnt;

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

static struct class *module_class;

static int __init mod_init(void)
{
	int ret;

	module_class = class_create(THIS_MODULE, CLASS_NAME);
	if (module_class == NULL) {
		pr_err("%s: Can't create a class in sysfs\n", MODULE_TAG);
		goto fail;
	}

	ret = class_create_file(module_class, &class_attr_buffer);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_buffer.attr.name, module_class->name);
		return ret;
	}

	ret = class_create_file(module_class, &class_attr_processed);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_processed.attr.name, module_class->name);
		return ret;
	}

	ret = class_create_file(module_class, &class_attr_converted);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_converted.attr.name, module_class->name);
		return ret;
	}

	ret = class_create_file(module_class, &class_attr_used);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_used.attr.name, module_class->name);
		return ret;
	}

	ret = class_create_file(module_class, &class_attr_alphabetic);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_alphabetic.attr.name, module_class->name);
		return ret;
	}

	ret = class_create_file(module_class, &class_attr_numeric);
	if (ret) {
		pr_err("%s: Can't create attribute %s in sysfs/%s\n",
			MODULE_TAG, class_attr_numeric.attr.name, module_class->name);
		return ret;
	}

	msg_buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (msg_buffer == NULL) {
		pr_err("%s: Memory allocation error\n", MODULE_TAG);
		goto fail;
	}

	statistic = kzalloc(sizeof(struct module_statistic), GFP_KERNEL);
	if (statistic == NULL) {
		pr_err("%s: Memory allocation error\n", MODULE_TAG);
		goto fail;
	}

	pr_info("%s: module loaded\n", MODULE_TAG);
	return 0;

fail:
	class_destroy(module_class);
	kfree(msg_buffer);
	kfree(statistic);
	return -ENOMEM;
}

static void __exit mod_exit(void)
{
	class_destroy(module_class);
	kfree(msg_buffer);
	kfree(statistic);
	pr_info("%s: module unloaded\n", MODULE_TAG);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A to-lower-case converter module for sysfs");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
