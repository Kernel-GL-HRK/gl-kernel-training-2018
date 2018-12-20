#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/timer.h>

#define MODULE_NAME "timeMan"

static unsigned long prev_seconds_val;
static struct timeval tv_local;
static int print_period_val;
static struct timer_list abs_timer;
static void abs_timer_update(void);

static ssize_t prev_seconds_show(struct class *class,
struct class_attribute *attr, char *buf)
{
	int cnt = 0;
	long diff = 0;
	//get jiffies val in local var
	unsigned long jiff_tmp = jiffies;

	diff = jiffies_to_msecs(jiff_tmp - prev_seconds_val);
	prev_seconds_val = jiff_tmp;
	cnt = sprintf(buf, "%dS passed since last read\n", (int)diff/1000);
	return cnt;
}

static ssize_t prev_abs_show(struct class *class,
struct class_attribute *attr, char *buf)
{
	int cnt = 0;
	struct tm tm_local;

	//convert timeval to tm
	time_to_tm(tv_local.tv_sec, sys_tz.tz_minuteswest, &tm_local);
	cnt = sprintf(buf + cnt, "%ld/%d/%d %d:%d:%d\n", tm_local.tm_year +
	1900, tm_local.tm_mon + 1, tm_local.tm_mday, tm_local.tm_hour,
	tm_local.tm_min, tm_local.tm_sec);
	// update tv_local with current time for next read
	do_gettimeofday(&tv_local);
	return cnt;
}

static ssize_t print_period_show(struct class *class,
struct class_attribute *attr, char *buf)
{
	int cnt = 0;

	cnt = sprintf(buf, "current period is %d\n", print_period_val);
	return cnt;
}

static ssize_t print_period_store(struct class *class,
struct class_attribute *attr, const char *buf, size_t count)
{
	int cnt = sscanf(buf, "%d\n", &print_period_val);

	if (cnt) {
		pr_info("Print interval set to %d\n", print_period_val);
		abs_timer_update();
	} else {
		pr_info("Print interval parse error\n");
	}
	return count;
}

CLASS_ATTR_RW(print_period);
CLASS_ATTR_RO(prev_seconds);
CLASS_ATTR_RO(prev_abs);

static struct attribute *timeMan_class_attrs[] = {
	&class_attr_print_period.attr,
	&class_attr_prev_seconds.attr,
	&class_attr_prev_abs.attr,
	NULL,
};

ATTRIBUTE_GROUPS(timeMan_class); // will be timeMan_class_groups

static struct class attr_class = {
	.name = MODULE_NAME,
	.owner = THIS_MODULE,
	.class_groups = timeMan_class_groups,
};

static void abs_timer_callback(unsigned long data)
{
	struct tm tm_timer;
	struct timeval tv_timer;

	do_gettimeofday(&tv_timer);
	//convert timeval to tm
	time_to_tm(tv_timer.tv_sec, sys_tz.tz_minuteswest, &tm_timer);
	pr_info("%ld/%d/%d %d:%d:%d\n", tm_timer.tm_year + 1900,
	tm_timer.tm_mon + 1, tm_timer.tm_mday, tm_timer.tm_hour,
	tm_timer.tm_min, tm_timer.tm_sec);
	abs_timer_update();
}

static void abs_timer_update(void)
{
	if (print_period_val)
		mod_timer(&abs_timer, jiffies
		+ msecs_to_jiffies(print_period_val * 1000));
	else
		del_timer(&abs_timer);
}

static int mymodule_init(void)
{
	int ret;

	ret = class_register(&attr_class);
	if (ret < 0) {
		pr_err("%s: error registering sysfs class\n", MODULE_NAME);
		return -ENOMEM;
	}

	//set first seconds read timestamp to module init time
	prev_seconds_val = jiffies;
	// set first abs read time to module init time
	do_gettimeofday(&tv_local);
	//default print period val is 5 seconds.
	print_period_val = 5;

	setup_timer(&abs_timer, abs_timer_callback,	0);
	mod_timer(&abs_timer, jiffies
	+ msecs_to_jiffies(print_period_val * 1000));

	pr_info("%s: module registered\n", MODULE_NAME);
	return 0;
}

static void mymodule_exit(void)
{
	class_unregister(&attr_class);
	del_timer(&abs_timer);
	pr_info("%s: module exited\n", MODULE_NAME);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Oleksandr.Siruk <alexsirook@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
