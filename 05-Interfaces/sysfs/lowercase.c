#include <linux/init.h>      // Macros used to mark up functions  __init __exit
#include <linux/module.h>    // Core header for loading LKMs into the kernel
#include <linux/kernel.h>    // Contains types, macros, functions for the kernel
#include <linux/err.h>
#include <linux/device.h>

#include <linux/ctype.h>


static char buffer[4096];
static ssize_t count = 0;

static unsigned read_calls = 0;
static unsigned write_calls = 0;
static unsigned chars_processed = 0;

static ssize_t convert_show(struct class *class, struct class_attribute *attr, char *out_buf)
{
	memcpy(out_buf, buffer, count);
	return count;

}

static ssize_t convert_store(struct class *class, struct class_attribute *attr, const char *in_buf, size_t buf_size)
{
	int i;
	for (i = 0; i < buf_size; ++i) {
		buffer[i] = toupper(in_buf[i]);
	}
	count = buf_size;
	return count;
}

static ssize_t stats_show(struct class *class, struct class_attribute *attr, char *out_buf)
{
	char temp_buf[256];
	int char_count = sprintf(temp_buf, "Read calls:%u\nWrite calls: %u\nChars processed^ %u\n", read_calls, write_calls, chars_processed);
	++char_count;
	memcpy(out_buf, temp_buf, char_count);
	return char_count;
}



static struct class *lc_class = 0;

static CLASS_ATTR_RW(convert);
static CLASS_ATTR_RO(stats);
static struct attribute *lc_attrs[] = {
        &class_attr_convert.attr,
        &class_attr_stats.attr,
        NULL
};

static struct attribute_group lc_group_attrs = {
        .name = "lowercase",
        .attrs = lc_attrs
};

static int __init lc_init(void)
{
	int ret;

	lc_class = class_create(THIS_MODULE, "lowercase");
	if (lc_class == NULL) {
		pr_err("lowercase mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = sysfs_create_group(lc_class->dev_kobj, &lc_group_attrs);
	if (ret) {
		pr_err("lowercase: error creating sysfs attribute group\n");
		class_destroy(lc_class);
		return ret;
	}

	pr_info("lowercase: module loaded\n");
	return 0;
}

static void __exit lc_exit(void)
{
	class_destroy(lc_class);
	pr_info("lowercase: module exited\n");
}


module_init(lc_init);
module_exit(lc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksii Khmara");
MODULE_DESCRIPTION("A simple Linux driver - sysfs lowercase homework");
MODULE_VERSION("0.1");
