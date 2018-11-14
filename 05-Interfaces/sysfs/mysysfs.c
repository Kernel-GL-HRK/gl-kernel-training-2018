
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/string.h>


static char msg[PAGE_SIZE];

static struct cvrt_stat {
	ssize_t converted;
	ssize_t read_call;
	ssize_t write_call;
} info;


static ssize_t cvrt_read(struct class *class, struct class_attribute *attr,
						 char *buf)
{
	pr_info("mysysfs: call %s", __func__);

	strlcpy(buf, msg, strlen(msg));

	++info.read_call;

	return strlen(buf);
}

static ssize_t cvrt_write(struct class *class, struct class_attribute *attr,
						  const char *buf, size_t count)
{
	int indx = 0;

	pr_info("mysysfs: call %s", __func__);
	++info.write_call;
	memcpy(msg, buf, strlen(buf));
	msg[strlen(buf)] = '\0';
	while (msg[indx]) {
		if ('A' <= msg[indx] &&  'Z' >= msg[indx]) {
			msg[indx] = 'a' + (msg[indx] - 'A');
			++info.converted;
		}
		++indx;
	}
	return count;
}


static ssize_t info_read(struct class *class, struct class_attribute *attr,
						 char *buf)
{

	pr_info("mysysfs: call %s", __func__);

	sprintf(buf, "Converter statistics:\n");
	sprintf(buf + strlen(buf), "Called: %d\nWrite: %d\nRead : %d\n",
			info.read_call + info.write_call, info.write_call,
			info.read_call);
	sprintf(buf + strlen(buf), "Converted: %d\n", info.converted);
	sprintf(buf + strlen(buf), "Now in buffer: %s", msg);

	return strlen(buf);
}


struct class_attribute class_attr_cvrt = {
	.attr = { .name = "cvrt", .mode = 0666 },
	.show	= cvrt_read,
	.store	= cvrt_write
};


struct class_attribute class_attr_info = {
	.attr = { .name = "stat", .mode = 0666 },
	.show	= info_read
};


//CLASS_ATTR_RO(class_attr_info);

static struct class *attr_class;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "toLower");
	if (attr_class == NULL) {
		pr_err("mysysfs: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_cvrt);
	if (ret) {
		pr_err("mysysfs: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}


	ret = class_create_file(attr_class, &class_attr_info);
	if (ret) {
		pr_err("mysysfs: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mysysfs: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_cvrt);
	class_remove_file(attr_class, &class_attr_info);

	class_destroy(attr_class);

	pr_info("mysysfs: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Sysfs example`");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


