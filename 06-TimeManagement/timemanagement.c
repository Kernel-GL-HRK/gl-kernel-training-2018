#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/kernel.h>



struct _time {
	struct tm 	prevReadAbsoluteTime;
	unsigned long 	prevReadInSec;
};

static struct timer_list periodicTimer;
static unsigned long setPeriodInSec;

static struct _time getTimePrevRead(void)
{
	static unsigned short 	isFirstRead = 1;
	static unsigned long 	prevJiff;
	static struct timeval  	prevTv;
	static struct _time 	result ;
	struct timeval 		currentTv;

	do_gettimeofday(&currentTv);
	if (isFirstRead) {
		isFirstRead = 0;
	} else {
		result.prevReadInSec = (jiffies - prevJiff) / HZ;
		time_to_tm(prevTv.tv_sec, 0, &result.prevReadAbsoluteTime);
	}
	prevTv 		= currentTv;
	prevJiff 	= jiffies;
	return result;
}

static void slotPeriodicMsg(struct timer_list *t)
{
	if (t == NULL) {
		pr_warn("Timer for periodic message is not init\n");
		return;
	}
	pr_info("this is  message with period %ld sec\n", setPeriodInSec);
	mod_timer(t, jiffies + HZ * setPeriodInSec);
}

static ssize_t periodicallyPrintSetting_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (!setPeriodInSec)
		sprintf(buf, "Periodic timer is disable\n");
	else
		sprintf(buf, "Message is printed with  period %ld sec\n", setPeriodInSec);
	return strlen(buf);
}



static ssize_t periodicallyPrintSetting_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{

	long inputPeriodInSec;
	static const unsigned long MaxPeriod = 100;
	int res = sscanf(buf, "%ld", &inputPeriodInSec);
	if (res != 1) {
		pr_warn("Input is not correct\n");
		return count;
	}

	if (inputPeriodInSec < 0 || inputPeriodInSec > MaxPeriod) {
		pr_warn("Input value is not valid. Set a positive value between 0 and %ld\n", MaxPeriod);
	} else {
		del_timer(&periodicTimer);
		timer_setup(&periodicTimer, slotPeriodicMsg, 0);
		if (inputPeriodInSec == 0) {
			pr_info("Period msg is stopped \n");
		} else {
			mod_timer(&periodicTimer, jiffies + HZ * inputPeriodInSec);
			setPeriodInSec = inputPeriodInSec;
			pr_info("Message period is set to %ld sec\n", setPeriodInSec);
		}
	}
	return count;
}



static ssize_t stat_show(struct class *class, struct class_attribute *attr, char *buf)
{
	static const char *months[] = {"January", "February", "March", "April",
					"May", "June", "July", "August", "September",
					"October", "November", "December"};
	static const int baseYear = 1900;

	struct _time prevReadTime = getTimePrevRead();
	sprintf(buf, "Seconds passed since previous read %ld\n"
		"Previous reading was: %d %s %ld %d:%d:%d\n",	prevReadTime.prevReadInSec,
								prevReadTime.prevReadAbsoluteTime.tm_mday,
								months[prevReadTime.prevReadAbsoluteTime.tm_mon],
								prevReadTime.prevReadAbsoluteTime.tm_year + baseYear,
								prevReadTime.prevReadAbsoluteTime.tm_hour,
								prevReadTime.prevReadAbsoluteTime.tm_min,
								prevReadTime.prevReadAbsoluteTime.tm_sec);
	return strlen(buf);
}

struct class_attribute class_attr_periodicallyPrintSetting = {
	.attr = { .name = "periodicallyPrintSetting", .mode = 0666 },
	.show	= periodicallyPrintSetting_show,
	.store	= periodicallyPrintSetting_store
};


CLASS_ATTR_RO(stat);

static struct class *attr_class ;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "TimeManagement");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_periodicallyPrintSetting);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}
	pr_info("module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	del_timer(&periodicTimer);
	class_remove_file(attr_class, &class_attr_periodicallyPrintSetting);
	class_remove_file(attr_class, &class_attr_stat);
	class_destroy(attr_class);

	pr_info("module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("Time management example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
