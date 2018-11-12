#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

enum eStat {CALLS, PROCESSED, CONVERTED};

struct module_stat {
	ssize_t stat_calls;
	ssize_t stat_processed;
	ssize_t stat_converted;
} module_stat_t;

static struct module_stat stat;
static char value[PAGE_SIZE];

static void inc_stat(int type)
{
	switch (type) {
	case CALLS:
		++stat.stat_calls;
	break;
	case PROCESSED:
		++stat.stat_processed;
	break;
	case CONVERTED:
		++stat.stat_converted;
	break;
	}
}

static void reset_stat(void)
{
	stat.stat_calls = 0;
	stat.stat_processed = 0;
	stat.stat_converted = 0;
}

static char to_upper(const char lower)
{
	char upper;
	inc_stat(PROCESSED);
	if (lower >= 'a' && lower <= 'z') {
		upper = lower + 'A' - 'a';
		inc_stat(CONVERTED);
	} else {
		upper = lower;
	}
	return upper;
}

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("mymodule: rw_show: value = %s\n", value);

	sprintf(buf, "%s\n", value);
	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	int i = 0;
	int length;

	inc_stat(CALLS);
	strcpy(value, buf);
	length = strlen(value);
	
	pr_info("mymodule: rw_store: value = %s\n", value);

	for (i = 0; i < length - 1; i++)	{
		value[i] = to_upper(value[i]);
	}
	return count;
}

static ssize_t r_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "Calls:     %d\n"
		"Processed: %d\n"
		"Converted: %d \n",
		stat.stat_calls,
		stat.stat_processed,
		stat.stat_converted);
	return strlen(buf);
}

/*CLASS_ATTR_RW(rw);*/
struct class_attribute class_attr_rw = {
	.attr = { .name = "rw", .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

/*CLASS_ATTR_R(r);*/
struct class_attribute class_attr_r = {
	.attr = { .name = "r", .mode = 0666 },
	.show	= r_show
};

static struct class *attr_class = 0;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "sysfs_module");
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

	ret = class_create_file(attr_class, &class_attr_r);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	reset_stat();

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_rw);
	class_remove_file(attr_class, &class_attr_r);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_VERSION("0.1");
