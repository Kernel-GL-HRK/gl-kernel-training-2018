#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/timer.h>


static uint32_t last_read_time;

static int flood_interval;
module_param(flood_interval, int, 0000);
MODULE_PARM_DESC(flood_interval, "Flood interval in seconds");


struct timeval get_uptime(void)
{
	struct timeval ret;
	const uint32_t curr_time = jiffies;

	pr_info("last: %u, curr: %u\n", last_read_time, curr_time);
	jiffies_to_timeval(curr_time - last_read_time, &ret);
	last_read_time = curr_time;
	return ret;
}

struct tm get_uptime_abs(void)
{
	struct tm ret;
	struct timeval curr = get_uptime();

	time_to_tm(curr.tv_sec, 0, &ret);
	return ret;
}

void init_startup_time(void)
{
	last_read_time = jiffies;
}


struct timer_list flood_timer;

void reset_flood_timer(unsigned long data)
{
	if (data > 0)
		mod_timer(&flood_timer, jiffies + msecs_to_jiffies(data));
}

void timer_cb(unsigned long data)
{
	pr_info("Flood\n");
	reset_flood_timer(data);
}

void init_flood_timer(void)
{
	setup_timer(&flood_timer, timer_cb, flood_interval * 1000);
	reset_flood_timer(flood_interval * 1000);
}

void stop_flood_timer(void)
{
	del_timer(&flood_timer);
}

static ssize_t time_show(struct class *class, struct class_attribute *attr,
				char *buf)
{
	const struct timeval time = get_uptime();

	sprintf(buf, "Seconds: %lu\nMilisec: %lu\n", time.tv_sec, time.tv_usec);
	return strlen(buf);
}

static ssize_t time_abs_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	const struct tm time_abs = get_uptime_abs();

	sprintf(buf, "%d/%d/%ld %d:%d:%d\n",
			time_abs.tm_mday, time_abs.tm_mon, time_abs.tm_year,
			time_abs.tm_hour, time_abs.tm_min, time_abs.tm_sec);
	return strlen(buf);
}

struct class_attribute class_attr_time = {
	.attr = { .name = "time", .mode = 0666 },
	.show	= time_show
};

struct class_attribute class_attr_abs_time = {
	.attr = { .name = "time_abs", .mode = 0666 },
	.show	= time_abs_show
};

static ssize_t flood_interval_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", flood_interval);
	return strlen(buf);
}

static ssize_t flood_interval_store(struct class *class, struct class_attribute *attr,
				const char *buf, size_t count)
{
	sscanf(buf, "%d", &flood_interval);
	stop_flood_timer();
	init_flood_timer();
	return count;
}

struct class_attribute class_attr_flood_interval = {
       .attr = { .name = "flood_interval", .mode = 0666 },
       .show   = flood_interval_show,
       .store  = flood_interval_store
};

static struct class *attr_class;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "time_management");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_time);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_abs_time);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_flood_interval);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}


	init_startup_time();
	init_flood_timer();

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	stop_flood_timer();

	class_remove_file(attr_class, &class_attr_time);
	class_remove_file(attr_class, &class_attr_abs_time);
	class_remove_file(attr_class, &class_attr_flood_interval);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_VERSION("0.1");
