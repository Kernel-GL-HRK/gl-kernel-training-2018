
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timer.h>

#define DEFAULT_DELAY_IN_MS 5000
#define MIN_DELAY_IN_MS 100

static unsigned long jiffies_stamp;
static unsigned long jiffies_now;
static unsigned long delta_in_ms;
static struct timespec epoch_now;
static struct timer_list my_timer;

static long delay = DEFAULT_DELAY_IN_MS;

module_param(delay, long, 0444);

static void mod_my_timer(unsigned long val_in_ms)
{
	if (!val_in_ms)
		return;
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(val_in_ms));
}

static void my_timer_callback(unsigned long data)
{
	pr_info("my_time: timer called (%s), timer delay: %lu [ms]\n",
		__func__, delta_in_ms);
	mod_my_timer(delta_in_ms);
}

static ssize_t my_show(struct class *class, struct class_attribute *attr,
						char *buf)
{
	struct tm last_read;

	pr_info("my_time: call %s", __func__);

	jiffies_now = jiffies;

	if (time_before(jiffies_stamp, jiffies_now)) {
		getnstimeofday(&epoch_now);
		time_to_tm(epoch_now.tv_sec, 0,  &last_read);
		pr_info("my_time: call %s ", __func__);
		sprintf(buf, "Now: %04ld-%02d-%02d %02d-%02d-%02d\n",
			1900 + last_read.tm_year, 1 + last_read.tm_mon, last_read.tm_mday,
			last_read.tm_hour, last_read.tm_min, last_read.tm_sec);
		sprintf(buf + strlen(buf), "Time in sec from previous read:: %lu\n",
				(jiffies_now - jiffies_stamp) / HZ);
		time_to_tm(epoch_now.tv_sec - (jiffies_now - jiffies_stamp) / HZ, 0,
			&last_read);
		sprintf(buf + strlen(buf),
			"Last read time (absolute): %04ld-%02d-%02d %02d-%02d-%02d\n",
			1900 + last_read.tm_year, 1 + last_read.tm_mon, last_read.tm_mday,
			last_read.tm_hour, last_read.tm_min, last_read.tm_sec);
		jiffies_stamp = jiffies_now;
	} else {
		sprintf(buf, "Overflow detected. Re-init now.\n");
		jiffies_stamp = jiffies_now;
	}
	return strlen(buf);
}

static ssize_t my_store(struct class *class, struct class_attribute *attr,
						const char *buf, size_t count)
{
	int ret_value;

	pr_info("my_time: call %s", __func__);
	ret_value = sscanf(buf, "%lu", &delay);

	pr_info("my_time: call %s, new delay obtained: %lu\n", __func__, delay);
	if (delay > 0 && delay < MIN_DELAY_IN_MS)
		pr_info("my_time: call %s, value out of bounds, use old value: %lu\n",
			__func__, delta_in_ms);

	if (ret_value != 1) {
		pr_info("my_time: call %s, can't read delay value!\n", __func__);
		pr_info("my_time: call %s, use old value: %ld\n",
			__func__, delta_in_ms);
	}

	if (ret_value == 1 && (delay == 0  || delay > MIN_DELAY_IN_MS)) {
		if (delay == 0)
			pr_info("my_time: call %s, switch timer off!\n", __func__);
		else
			pr_info("my_time: call %s, set new timer delay value: %ld\n",
				__func__, delay);
		delta_in_ms = delay;
		delay = 0;
	}

	mod_my_timer(delta_in_ms);
	return count;
}


static ssize_t my_info(struct class *class, struct class_attribute *attr,
						char *buf)
{

	pr_info("my_time: call %s", __func__);
	sprintf(buf, "Time delay now is: %ld [ms]\n", delta_in_ms);
	if (delta_in_ms == 0)
		sprintf(buf + strlen(buf), "Timer switch off.\n");
	return strlen(buf);
}


struct class_attribute class_attr_tm = {
	.attr = { .name = "info", .mode = 0666 },
	.show	= my_show,
};


struct class_attribute class_attr_set = {
	.attr = { .name = "set", .mode = 0666 },
	.show	= my_info,
	.store  = my_store
};


static struct class *attr_class;


static int my_init(void)
{
	int ret;

	jiffies_stamp = jiffies;

	attr_class = class_create(THIS_MODULE, "myTime");

	if (attr_class == NULL) {
		pr_err("my_time: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_tm);
	if (ret) {
		pr_err("my_time: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}


	ret = class_create_file(attr_class, &class_attr_set);
	if (ret) {
		pr_err("my_time: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	if (delay != 0 && delay < DEFAULT_DELAY_IN_MS) {
		pr_info("my_time: param value out of bounds: %ld", delay);
		delay = DEFAULT_DELAY_IN_MS;
		pr_info("my_time: set time delay to default: %ld", delay);
	}

	setup_timer(&my_timer, my_timer_callback, 0);
	delta_in_ms = delay;
	mod_my_timer(delta_in_ms);

	pr_info("my_time: loaded\n");
	return 0;
}

static void my_exit(void)
{
	del_timer(&my_timer);
	class_remove_file(attr_class, &class_attr_tm);
	class_remove_file(attr_class, &class_attr_set);

	class_destroy(attr_class);

	pr_info("my_time: removed\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Linux kernel time example`");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


