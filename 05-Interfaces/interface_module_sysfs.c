#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/ctype.h>

static char sysfs_char[PAGE_SIZE];

static struct statisic 
{
	long char_processed;
	long char_returned;
	long char_received;
	long total_write_calls;
	long total_read_calls;
}
iface_sysfs_stat;

static ssize_t sysfs_conv_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("my_interface: sysfs_conv_show called\n");

	iface_sysfs_stat.total_read_calls++;

	sprintf(buf, "%s", sysfs_char);
	return strlen(buf);
}

static ssize_t sysfs_conv_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	pr_info("my_interface: sysfs_conv_store called with %s", buf);

	iface_sysfs_stat.total_write_calls++;

	char *iter = buf;
	char *target = sysfs_char;

	while (*iter) {
		iface_sysfs_stat.char_received++;
		if (isupper(*iter)) {
			*target = tolower(*iter);
			iface_sysfs_stat.char_processed++;
		} else {
			*target = *iter;
		}
		target++;
		iter++;
	}

	return count;
}

static ssize_t sysfs_stat_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("my_interface: sysfs_stat_show called\n");

	char loc_buf[PAGE_SIZE];

	sprintf(loc_buf, "my_interface char processed: %ld,\n", iface_sysfs_stat.char_processed);
	strcat(buf,loc_buf);
	sprintf(loc_buf, "             char returned: %ld,\n", iface_sysfs_stat.char_returned);
	strcat(buf,loc_buf);
	sprintf(loc_buf, "             char received: %ld,\n", iface_sysfs_stat.char_received);
	strcat(buf,loc_buf);
	sprintf(loc_buf, "             total write calls: %ld,\n", iface_sysfs_stat.total_write_calls);
	strcat(buf,loc_buf);
	sprintf(loc_buf, "             total read calls: %ld\n", iface_sysfs_stat.total_read_calls);
	strcat(buf,loc_buf);

	return strlen(buf);
}

CLASS_ATTR_RW(sysfs_conv);
CLASS_ATTR_RO(sysfs_stat);

static struct class *attr_class = NULL;

static int __init init_if_module(void)
{
	pr_info("Init sysfs interface module\n");

	int ret;

	attr_class = class_create(THIS_MODULE, "my_interface");
	if (attr_class == NULL) {
		pr_err("my_interface: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_sysfs_conv);
	if (ret) {
		pr_err("my_interface: error creating sysfs class converter attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_sysfs_stat);
	if (ret) {
		pr_err("my_interface: error creating sysfs class statistic attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("my_interface: module loaded\n");
	return 0;
}

static void __exit exit_if_module(void)
{
	class_remove_file(attr_class, &class_attr_sysfs_stat);
	class_remove_file(attr_class, &class_attr_sysfs_conv);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
	pr_info("Goodbye, cruel world!\n");
}

module_init(init_if_module);
module_exit(exit_if_module);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Perederii Serhii");
MODULE_DESCRIPTION("A module for sysfs string processing");
MODULE_VERSION("0.2");
