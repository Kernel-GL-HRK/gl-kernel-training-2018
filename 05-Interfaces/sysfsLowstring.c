#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>


static char msg[PAGE_SIZE];
struct usageStatistic{
	int convertedCharacters;
	int numcallShow;
};

static struct usageStatistic stats;

static ssize_t converter_show(struct class *class, struct class_attribute *attr, char *buf)
{
	strcpy(buf, msg);
	msg[0] = '\0';
	++stats.numcallShow;
	return strlen(buf);
}

static ssize_t converter_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	int index = 0;
	char character;
	while (buf[index]) {
		character = buf[index];
		msg[index] = (char)tolower(character);
		++index;
		++stats.convertedCharacters;
	}
	return count;
}


static ssize_t stat_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "Lowercase converter statistics:\n"
	"Characters was converted: %d \n"
	"Show was called: %d \n",
	stats.convertedCharacters, stats.numcallShow);
	return strlen(buf);
}


struct class_attribute class_attr_converter = {
	.attr = { .name = "converter", .mode = 0666 },
	.show	= converter_show,
	.store	= converter_store
};


CLASS_ATTR_RO(stat);

static struct class *attr_class ;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "strlowConverter");
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_converter);
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

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_converter);
	class_remove_file(attr_class, &class_attr_stat);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
