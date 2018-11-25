
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include "linux/slab.h"


struct cvrt_stat {
	ssize_t converted;
	ssize_t read_call;
	ssize_t write_call;
};

struct msg_buffer {
	char *data;
	int size;
};

static struct msg_buffer msg = { .data = NULL, .size = 0 };
static struct cvrt_stat info;

static ssize_t cvrt_read(struct class *class, struct class_attribute *attr,
						 char *buf)
{
	pr_info("memory_resize: call %s", __func__);
	if (msg.data)
		sprintf(buf, "%s", msg.data);
	else
		*buf = '\0';
	++info.read_call;
	return strlen(buf);
}

static int convert(char *str)
{
	int converted;

	converted = 0;
	while (*str) {
		if ('A' <= *str &&  'Z' >= *str) {
			*str = 'a' + (*str - 'A');
			++converted;
		}
		++str;
	}
	return converted;
}

static int get_power_2(int size)
{
	int power;

	power = 0;
	while (size) {
		size /= 2;
		++power;
	}
	return (1 << power);
}

static ssize_t cvrt_write(struct class *class, struct class_attribute *attr,
						  const char *buf, size_t count)
{
	char *tmp;
	int buf_len, msg_len;

	pr_info("memory_resize: call %s", __func__);
	++info.write_call;

	buf_len = strlen(buf);

	if (!msg.size) {
		msg.size = get_power_2(buf_len);
		pr_info("memory_resize: block allocate (1), size %d", msg.size);
		msg.data = kmalloc(msg.size, GFP_KERNEL);
		*msg.data = '\0';
	}

	msg_len = strlen(msg.data);

	if (msg_len + buf_len > msg.size) {
		tmp = kmalloc(msg.size, GFP_KERNEL);
		memcpy(tmp, msg.data, msg_len);
		*(tmp + msg_len) = '\0';

		kfree(msg.data);

		msg.size = get_power_2(msg.size);
		pr_info("memory_resize: block allocate(2), size %d", msg.size);
		msg.data = kmalloc(msg.size, GFP_KERNEL);
		memcpy(msg.data, tmp, msg_len);
		*(msg.data + msg_len) = '\0';

		msg_len = strlen(msg.data);

		kfree(tmp);
	}

	memcpy(msg.data + msg_len, buf, buf_len);
	*(msg.data + msg_len + buf_len) = '\0';
	info.converted += convert(msg.data);

	return count;
}



static ssize_t info_read(struct class *class, struct class_attribute *attr,
						 char *buf)
{

	pr_info("memory_resize call %s", __func__);
	sprintf(buf, "Converter statistics:\n");
	sprintf(buf + strlen(buf), "Called: %d\nWrite: %d\nRead : %d\n",
			info.read_call + info.write_call, info.write_call,
			info.read_call);
	sprintf(buf + strlen(buf), "Converted: %d\n", info.converted);
	sprintf(buf + strlen(buf), "Now in buffer: %s", msg.data);

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


static struct class *attr_class;


static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "toLower");
	if (attr_class == NULL) {
		pr_err("memory_resize: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_cvrt);
	if (ret) {
		pr_err("memory_resize: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}


	ret = class_create_file(attr_class, &class_attr_info);
	if (ret) {
		pr_err("memory_resize: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("memory_resize module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	kfree(msg.data);
	class_remove_file(attr_class, &class_attr_cvrt);
	class_remove_file(attr_class, &class_attr_info);

	class_destroy(attr_class);

	pr_info("memory_resize module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Memory resize example`");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


