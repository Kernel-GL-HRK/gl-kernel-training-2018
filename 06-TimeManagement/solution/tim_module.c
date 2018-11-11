#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/device.h>
#include <linux/kernel.h>


static unsigned long log_period_ms = 5000;


static ssize_t log_period_store(struct class *class,
		struct class_attribute *attr, const char *buf, size_t count)
{
	unsigned long temp_log_per = 0;

	if (kstrtoul(buf, 10, &temp_log_per)) {
		pr_err("tmod: Error when parsing log period\n");
		return -EINVAL;
	}

	if (!temp_log_per)
		return -EINVAL;

	log_period_ms = temp_log_per;

	return count;
}

static ssize_t log_period_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%lu\n", log_period_ms);

	return strlen(buf);
}


static CLASS_ATTR_RW(log_period);

static ssize_t prev_read_info_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	sprintf(buf, "Not implemented\n");
	return strlen(buf);
}

static CLASS_ATTR_RO(prev_read_info);

static struct class *time_demo_class;

static int tm_init(void)
{
	int ret;

	time_demo_class = class_create(THIS_MODULE, "time_demo");
	if (time_demo_class == NULL) {
		pr_err("tmod: Error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(time_demo_class, &class_attr_log_period);
	if (ret) {
		pr_err("tmod: Error creating log period attribute\n");
		class_destroy(time_demo_class);
		return ret;
	}

	ret = class_create_file(time_demo_class, &class_attr_prev_read_info);
	if (ret) {
		pr_err("tmod: Error creating read info attribute\n");
		class_remove_file(time_demo_class, &class_attr_log_period);
		class_destroy(time_demo_class);
		return ret;
	}

	return 0;
}

static void tm_exit(void)
{
	class_remove_file(time_demo_class, &class_attr_log_period);
	class_remove_file(time_demo_class, &class_attr_prev_read_info);
	class_destroy(time_demo_class);
}

module_init(tm_init);
module_exit(tm_exit);

MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("Time management API demo");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
