#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

#define MODULE_NAME "simple_sysfs"
static ssize_t rw_store(struct class *, struct class_attribute *,
const char *, size_t);
static ssize_t rw_show(struct class *, struct class_attribute *, char *);

static char msg[PAGE_SIZE];
static int converted_cnt;
static int processed_cnt;
static int read_cnt;
static int write_cnt;

//CLASS_ATTR_RW(rw);
struct class_attribute class_attr_rw = {
	.attr = { .name = "rw", .mode = 0666 },
	.show = rw_show,
	.store = rw_store
};

struct class_attribute class_attr_converted = {
	.attr = { .name = "converted_cnt", .mode = 0444 },
	.show = rw_show
};

struct class_attribute class_attr_processed = {
	.attr = { .name = "processed_cnt", .mode = 0444 },
	.show = rw_show
};

struct class_attribute class_attr_read = {
	.attr = { .name = "read_cnt", .mode = 0444 },
	.show = rw_show
};

struct class_attribute class_attr_write = {
	.attr = { .name = "write_cnt", .mode = 0444 },
	.show = rw_show
};


static struct class *attr_class; // = 0;


static int mymodule_init(void)
{
	int ret;

	converted_cnt = 0;
	processed_cnt = 0;
	read_cnt = 0;
	write_cnt = 0;

	attr_class = class_create(THIS_MODULE, MODULE_NAME);
	if (attr_class == NULL) {
		pr_err("%s: error creating sysfs class\n", MODULE_NAME);
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
		, MODULE_NAME);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_converted);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
		, MODULE_NAME);
		class_remove_file(attr_class, &class_attr_rw);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_processed);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
		, MODULE_NAME);
		class_remove_file(attr_class, &class_attr_rw);
		class_remove_file(attr_class, &class_attr_converted);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_read);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
		, MODULE_NAME);
		class_remove_file(attr_class, &class_attr_rw);
		class_remove_file(attr_class, &class_attr_converted);
		class_remove_file(attr_class, &class_attr_processed);
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_write);
	if (ret) {
		pr_err("%s: error creating sysfs class attribute\n"
		, MODULE_NAME);
		class_remove_file(attr_class, &class_attr_rw);
		class_remove_file(attr_class, &class_attr_converted);
		class_remove_file(attr_class, &class_attr_processed);
		class_remove_file(attr_class, &class_attr_read);
		class_destroy(attr_class);
		return ret;
	}
	pr_info("%s: module loaded\n", MODULE_NAME);
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_rw);
	class_remove_file(attr_class, &class_attr_converted);
	class_remove_file(attr_class, &class_attr_processed);
	class_remove_file(attr_class, &class_attr_read);
	class_remove_file(attr_class, &class_attr_write);

	class_destroy(attr_class);

	pr_info("%s: module exited\n", MODULE_NAME);
}

static ssize_t rw_show(struct class *class,
struct class_attribute *attr, char *buf)
{

	if (attr == &class_attr_rw) {
		pr_info("%s: %s: value = %s", MODULE_NAME, __func__, msg);
		sprintf(buf, "%s", msg);
		read_cnt++;
	} else if (attr == &class_attr_converted) {
		pr_info("%s: %s: converted = %d\n", MODULE_NAME, __func__,
		converted_cnt);
		sprintf(buf, "%d\n", converted_cnt);
	} else if (attr == &class_attr_processed) {
		pr_info("%s: %s: processed = %d\n", MODULE_NAME,
		__func__, processed_cnt);
		sprintf(buf, "%d\n", processed_cnt);
	} else if (attr == &class_attr_read) {
		pr_info("%s: %s: read = %d\n", MODULE_NAME, __func__, read_cnt);
		sprintf(buf, "%d\n", read_cnt);
	} else if (attr == &class_attr_write) {
		pr_info("%s: %s: write = %d\n", MODULE_NAME,
		__func__, write_cnt);
		sprintf(buf, "%d\n", write_cnt);
	}

	return strlen(buf);
}

static ssize_t rw_store(struct class *class,
struct class_attribute *attr, const char *buf, size_t count)
{
	int i = 0;

	sprintf(msg, "%s", buf);

	write_cnt++;
	if (count > 4095) {
		pr_info("%s: %s: message too long. count = %d",
		MODULE_NAME, __func__, count);
	return count;
	}
	processed_cnt += count;
	pr_info("%s: %s: value = %s", MODULE_NAME, __func__, msg);
	for (i = 0; i < count; i++) {
		if (msg[i] <= (int)'Z' && msg[i] >= (int)'A') {
			msg[i] = (int)msg[i] - (int)'A' + (int)'a';
			converted_cnt++;
		}
	}
	return count;
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Oleksandr.Siruk <alexsirook@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
