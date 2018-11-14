#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/ctype.h>

static int call_read;
static int call_write;
static int total_char;
static char str[PAGE_SIZE];

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	call_read++;

	pr_info("mymodule: rw_show: string: %s\n", str);
	sprintf(buf, "Output lowercase string:\n%s\n", str);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	int i;
	int temp;

	call_write++;

	strcpy(str, buf);
	temp = strlen(str);

	pr_info("mymodule: rw_store: string: %s\n", str);

	for (i = 0; i < temp; i++) {
		total_char++;
		str[i] = tolower(str[i]);
	}
	return count;
}

static ssize_t statistics_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("Statistics:\ncall Read %d call Write %d Total characters %d\n",
					call_read, call_write, total_char);
	sprintf(buf, "Statistics:\ncall Read %d call Write %d Total characters %d\n",
					call_read, call_write, total_char);

	return strlen(buf);
}


CLASS_ATTR_RW(rw);
CLASS_ATTR_RO(statistics);

static struct class *attr_class;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "lowercase");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

		ret = class_create_file(attr_class, &class_attr_statistics);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule: module loaded\nCreate:\n/sys/class/lowercase/rw\n"
					"/sys/class/lowercase/statistics\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_statistics);
	class_remove_file(attr_class, &class_attr_rw);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\nRemove:\n/sys/class/lowercase/rw\n"
					"/sys/class/lowercase/statistics\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Dmitriy Ganshin");
MODULE_DESCRIPTION("Simple Kernel module sysfs  (write, read, statistics");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
