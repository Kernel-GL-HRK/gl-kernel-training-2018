// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/timekeeping.h>

#define MODULE_TAG			"Timer"
#define CLASS_NAME			"Timer"

static unsigned long int interval;
module_param(interval, ulong, 0444);
MODULE_PARM_DESC(interval, "An interval in ms for periodic output in dmesg");
static struct timer_list print_timer;

static unsigned long last_read;
static struct timeval last_abs_time;

static void update_print_timer(unsigned long time_ms)
{
	int ret;

	if (time_ms) {
		ret = mod_timer(&print_timer, jiffies + msecs_to_jiffies(interval));
		if (!ret)
			pr_debug("%s: Timer was started\n", MODULE_TAG);
		else
			pr_debug("%s: Timer was updated\n", MODULE_TAG);
	} else {
		ret = del_timer(&print_timer);
		if (!ret)
			pr_debug("%s: Inactive timer was stopped\n", MODULE_TAG);
		else
			pr_debug("%s: Active timer was stopped\n", MODULE_TAG);
	}
}

static void print_timer_fn(unsigned long data)
{
	struct timespec  uptime;

	get_monotonic_boottime(&uptime);
	pr_info("%s: System Uptime: %d sec\n", MODULE_TAG, uptime.tv_sec);

	update_print_timer(interval);
}

static ssize_t last_read_show(struct class *class, struct class_attribute *attr, char *buf)
{
	int cnt;
	unsigned long delta;

	delta = jiffies - last_read;
	last_read = jiffies;
	delta = jiffies_to_msecs(delta)/1000;

	cnt = scnprintf(buf, PAGE_SIZE, "%ld\n", delta);

	return cnt;
}

static ssize_t last_absolute_show(struct class *class, struct class_attribute *attr, char *buf)
{
	struct tm local_time;
	int cnt;

	time64_to_tm(last_abs_time.tv_sec, sys_tz.tz_minuteswest * 60, &local_time);

	cnt = scnprintf(buf, PAGE_SIZE, "%d/%02d/%02d %02d:%02d:%02d\n",
		local_time.tm_year + 1900, local_time.tm_mon, local_time.tm_mday,
		local_time.tm_hour, local_time.tm_min, local_time.tm_sec);

	do_gettimeofday(&last_abs_time);

	return cnt;
}

static ssize_t print_interval_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%ld\n", interval);
}

static ssize_t print_interval_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	unsigned long temp;
	int ret;

	ret = kstrtoul(buf, 0, &temp);
	if (ret < 0)
		return ret;
	interval = temp;

	update_print_timer(interval);

	return count;
}


CLASS_ATTR_RO(last_read);
CLASS_ATTR_RO(last_absolute);
CLASS_ATTR_RW(print_interval);

static struct attribute *timer_class_attrs[] = {
	&class_attr_last_read.attr,
	&class_attr_last_absolute.attr,
	&class_attr_print_interval.attr,
	NULL,
};
ATTRIBUTE_GROUPS(timer_class);

static struct class module_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = timer_class_groups,
};

static int __init mod_init(void)
{
	int ret;

	ret = class_register(&module_class);
	if (ret < 0) {
		pr_err("%s: Can't register a class in sysfs\n", MODULE_TAG);
		return ret;
	}

	setup_timer(&print_timer, print_timer_fn, 0);
	update_print_timer(interval);

	//Set a zero point for last_read
	last_read = jiffies;

	//Set a zero point for last_abs_time
	do_gettimeofday(&last_abs_time);

	pr_info("%s: module loaded\n", MODULE_TAG);
	return 0;
}

static void __exit mod_exit(void)
{
	update_print_timer(0);
	class_unregister(&module_class);

	pr_info("%s: module unloaded\n", MODULE_TAG);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A timer module for sysfs");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
