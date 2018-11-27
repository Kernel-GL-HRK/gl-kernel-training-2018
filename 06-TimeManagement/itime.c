#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

static struct timer_list timer_hendler;
static u64 timerperiod_ms = 1000;

static void timer_cb(struct timer_list *l)
{
	static u32 ncb;

	pr_info("itime: %s: %u\n", __func__, ncb++);
	if (timerperiod_ms != 0)
		mod_timer(&timer_hendler, jiffies + msecs_to_jiffies(timerperiod_ms));
}

static void timer_setperiod(u64 ms)
{
	if (ms == 0) {
		try_to_del_timer_sync(&timer_hendler);
	} else {
		if (timerperiod_ms == 0) {
			mod_timer(&timer_hendler, jiffies + msecs_to_jiffies(ms));
		}
	}
	timerperiod_ms = ms;
}

static void timer_init(void)
{
	timer_setup(&timer_hendler, timer_cb, 0);
	mod_timer(&timer_hendler, jiffies + msecs_to_jiffies(timerperiod_ms));
}

static void timer_deinit(void)
{
	del_timer(&timer_hendler);
}

/**
 * return: jiffies diff
 */
static u64 getdiff(void)
{
	static u64 jiffiescach;
	u64 jiffiesdiff;

	jiffies = get_jiffies_64();
	jiffiesdiff = jiffies - jiffiescach;
	jiffiescach = jiffies;
	return jiffiesdiff;
}

/**
 * sysfs callbacks
 */
static ssize_t show_rtime(struct class *class, struct class_attribute *attr,
						char *pbuf)
{
	size_t strlen;

	pr_info("itime: %s\n", __func__);
	strlen = sprintf(pbuf, "%08llu\n", getdiff() / HZ);

	return strlen + 1;
}

static ssize_t show_atime(struct class *class, struct class_attribute *attr,
						char *pbuf)
{
	size_t strlen;
	struct timeval tv;
	struct tm tm;

	pr_info("itime: %s\n", __func__);
	jiffies_to_timeval(getdiff(), &tv);
	time_to_tm(tv.tv_sec, 0, &tm);
	strlen = sprintf(pbuf, "%02i:%02i:%02i\n", tm.tm_hour, tm.tm_min,
					tm.tm_sec);

	return strlen + 1;
}

static ssize_t store_adjtime(struct class *class,
							struct class_attribute *attr,
							const char *pbuf, size_t count)
{
	int n, ms;

	n = sscanf(pbuf, "%u", &ms);
	if (n < 1) {
		pr_info("itime: %s: error time\n", __func__);
		return count;
	}

	if (ms != 0 && ms < 100)
		ms = 100;
	if (ms > 10000)
		ms = 10000;

	pr_info("itime: %s: set period: %u ms\n", __func__, ms);
	timer_setperiod(ms);

	return count;
}

/**
 * sysfs attribute
 */
static struct class_attribute rtime = {
	.attr = { .name = "relative", .mode = 0444 },
	.show	= show_rtime
};

static struct class_attribute atime = {
	.attr = { .name = "absolute", .mode = 0444 },
	.show	= show_atime
};

static struct class_attribute adjtime = {
	.attr = { .name = "adjtime", .mode = 0222 },
	.store	= store_adjtime
};

static struct class *attr_class;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "itime");
	if (attr_class == NULL) {
		pr_err("itime: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &adjtime);
	if (ret) {
		pr_err("itime: error creating sysfs class attribute adjtime\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &atime);
	if (ret) {
		pr_err("itime: error creating sysfs class attribute atime\n");
		class_remove_file(attr_class, &adjtime);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &rtime);
	if (ret) {
		pr_err("itime: error creating sysfs class attribute rtime\n");
		class_remove_file(attr_class, &atime);
		class_remove_file(attr_class, &adjtime);
		class_destroy(attr_class);
		return ret;
	}

	timer_init();

	pr_info("itime: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	timer_deinit();
	class_remove_file(attr_class, &rtime);
	class_remove_file(attr_class, &atime);
	class_remove_file(attr_class, &adjtime);
	class_destroy(attr_class);

	pr_info("itime: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Storozhenko <d2.718l@gmail.com>");
MODULE_DESCRIPTION("Time Management");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
