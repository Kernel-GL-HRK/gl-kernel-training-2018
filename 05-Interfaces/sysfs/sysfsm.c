#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>

static char buf[PAGE_SIZE];
static ssize_t len;

static char bufstat[PAGE_SIZE];
static ssize_t lenstat;

static struct
{
	uint32_t nread;
	uint32_t nwrite;
	uint32_t charprocessed;
	uint32_t charconverted;
} stat;

static ssize_t show_converter(struct class *class, struct class_attribute *attr, char *pbuf)
{
	size_t nreaded;

	pr_info("lowercase: %s\n", __func__);

	nreaded = len;
	memcpy(pbuf, buf, len);
	len = len - len;
	return nreaded;
}

static ssize_t show_statistic(struct class *class, struct class_attribute *attr, char *pbuf)
{
	pr_info("lowercase: %s\n", __func__);

	lenstat = snprintf(bufstat, sizeof(bufstat),
						"read: %u\n"
						"write: %u\n"
						"char processed: %u\n"
						"char converted: %u\n",
						stat.nread,
						stat.nwrite,
						stat.charprocessed,
						stat.charconverted
	);
	lenstat++; // '\0'
	memcpy(pbuf, bufstat, lenstat);
	return lenstat;
}

static ssize_t store_converter(struct class *class, struct class_attribute *attr, const char *pbuf, size_t count)
{
	char *s, c;

	pr_info("lowercase: %s\n", __func__);

	// Copy and convert
	s = buf;
	while (*pbuf != 0) {
		c = tolower(*pbuf);
		if (c != *pbuf)
			stat.charconverted++;
		*s++ = c;
		pbuf++;
	}
	len = s - buf;
	stat.charprocessed += len;

	return count;
}

struct class_attribute class_attr_converter = {
	.attr = { .name = "converter", .mode = 0666 },
	.show	= show_converter,
	.store	= store_converter
};

struct class_attribute class_attr_statistic = {
	.attr = { .name = "statistic", .mode = 0444 },
	.show	= show_statistic,
};

static struct class *attr_class;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "lowercase");
	if (attr_class == NULL) {
		pr_err("lowercase: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_converter);
	if (ret) {
		pr_err("lowercase: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_statistic);
	if (ret) {
		class_remove_file(attr_class, &class_attr_converter);
		pr_err("lowercase: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("lowercase: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_statistic);
	class_remove_file(attr_class, &class_attr_converter);
	class_destroy(attr_class);

	pr_info("lowercase: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Storozhenko <d2.718l@gmail.com>");
MODULE_DESCRIPTION("Simple string lowercase converter using sysfs");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

