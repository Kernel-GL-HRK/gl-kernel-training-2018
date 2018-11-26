#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/time.h>


unsigned long int delta;
unsigned long int start_module;
static unsigned long int delay;

static struct timeval abs_time;
static struct timer_list timer;


static void timer_update(void)
{
	if (delay)
		mod_timer(&timer, jiffies + msecs_to_jiffies(delay * HZ));
	else
		del_timer(&timer);
}

//message how much time has passed since the start of the module
static void timer_callback(unsigned long data)
{
	unsigned long int tmp = jiffies;
	unsigned long int ret = tmp - start_module;

	pr_info("uptime module %ld s.\n", ret/HZ);

	timer_update();
}

static ssize_t previous_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	unsigned long int  tmp =  jiffies;

	if (delta == 0) {
		pr_info("first reading: previous read = 0 s\n");
		delta = jiffies;
	return 0;
	}

	delta = jiffies_to_msecs(tmp - delta)/HZ;

	pr_info("previous read = %ld s.\n", delta);

	sprintf(buf, "previous read = %ld s.\n", delta);

	delta = jiffies;
	return strlen(buf);
}

static ssize_t previous_ads_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	struct tm local_time;

	time_to_tm(abs_time.tv_sec, sys_tz.tz_minuteswest, &local_time);

	pr_info("previous read: %ld %d %d %d:%d:%d\n",
	local_time.tm_year + 1900, local_time.tm_mon, local_time.tm_mday,
	local_time.tm_hour, local_time.tm_min, local_time.tm_sec);

	sprintf(buf, "previous read: %ld %d %d %d:%d:%d\n",
	local_time.tm_year + 1900, local_time.tm_mon, local_time.tm_mday,
	local_time.tm_hour, local_time.tm_min, local_time.tm_sec);

	do_gettimeofday(&abs_time);
	return strlen(buf);
}

static ssize_t spam_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	pr_info("message every %ld seconds\n", delay);

	sprintf(buf, "message every %ld seconds\n", delay);

	return strlen(buf);
}

static ssize_t spam_store(struct class *class,
		struct class_attribute *attr, const char *buf, size_t count)
{
	int tmp;

	pr_info("delay setting:\n");

	tmp = sscanf(buf, "%ld\n", &delay);

	if (tmp) {
		pr_info("delay: %ld\n", delay);
		timer_update();
	} else
		pr_info("ERROR\n");

	return count;
}


CLASS_ATTR_RO(previous);
CLASS_ATTR_RO(previous_ads);
CLASS_ATTR_RW(spam);

static struct class *attr_class;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "time_module");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_previous);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_previous_ads);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_spam);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	start_module = jiffies;

	do_gettimeofday(&abs_time);

	setup_timer(&timer, timer_callback, 0);


	pr_info("mymodule: module loaded\nCreate:\n/sys/class/time_module/\n");

	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_previous);
	class_remove_file(attr_class, &class_attr_previous_ads);
	class_destroy(attr_class);

	del_timer(&timer);

	pr_info("mymodule: module exited\nRemove:\n/sys/class/time_module/\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Dmitriy Ganshin");
MODULE_DESCRIPTION("homework 6: Time Management");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
