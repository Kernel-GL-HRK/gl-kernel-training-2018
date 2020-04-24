// SPDX-License-Identifier: GPL-3.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>

#define MAXLEN PAGE_SIZE

static char strValue[MAXLEN];
static struct obj_statisic
{
	int total_calls;
	int total_characters;
	int characters_not_converted;
	int characters_converted;
}
statisic;

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	statisic.total_calls++;
	pr_info("mymodule_sysfs: rw_show: value = %s\n", strValue);
	sprintf(buf, "%s\n", strValue);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	int i;
	size_t cpy_count;

	statisic.total_calls++;
	cpy_count = (count < MAXLEN) ? count : MAXLEN;
	strncpy(strValue, buf, cpy_count);
	pr_info("mymodule_sysfs: rw_store: called with %s\n", strValue);
	strValue[cpy_count] = '\0';
	i=1;
	while (strValue[i]) {
		strValue[i] = tolower(strValue[i]);
		statisic.total_characters++;
		if ((strValue[i] >= 'a' && strValue[i] <= 'z') || \
			(strValue[i] >= 'A' && strValue[i] <= 'Z'))
				statisic.characters_converted++;
		i++;
	}
	statisic.characters_not_converted =
			statisic.total_characters - statisic.characters_converted;
	pr_info("mymodule_sysfs: rw_store: value = %s\n", strValue);

	return count;
}

static ssize_t statistic_show(struct class *class, struct class_attribute *attr, char *buf)
{
	static char temp_buf[MAXLEN];
	statisic.total_calls++;
	pr_info("mymodule_sysfs: statistic show");
	sprintf(temp_buf, 
	"		.total_calls = %d\n"
	"		.total_characters = %d\n"
	"		.characters_converted = %d\n"
	"		.characters_not_converted = %d\n",
	statisic.total_calls, statisic.total_characters, 
	statisic.characters_converted,	statisic.characters_not_converted);
	strcat(buf, temp_buf);
	return strlen(buf);
}

// CLASS_ATTR_RW(rw);
// CLASS_ATTR_RO(statistic);

struct class_attribute class_attr_rw = {
	.attr = {.name = "rw", .mode = 0666},
	.show = rw_show,
	.store = rw_store
};

struct class_attribute class_attr_statistic = {
	.attr = {.name = "statistic", .mode = 0444},
	.show = statistic_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "mymodule_sysfs");
	if (attr_class == NULL)	{
		pr_err("mymodule_sysfs: error creating sysfs class\n");
		return -ENOMEM;
	}
	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("mymodule_sysfs: error creating class_attr_rw class attribute\n");
		class_destroy(attr_class);
		return ret;
	}
	ret = class_create_file(attr_class, &class_attr_statistic);
	if (ret) {
		pr_err("mymodule_sysfs: error creating class_attr_statistic class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule_sysfs: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_rw);
	class_remove_file(attr_class, &class_attr_statistic);
	class_destroy(attr_class);

	pr_info("mymodule_sysfs: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Eugene.Ovdiyuk <ovdiyuk@gmail.com>");
MODULE_DESCRIPTION("sysfs simple Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
