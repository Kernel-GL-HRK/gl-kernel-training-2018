#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/list.h>

#define MSG_CACHE_NAME "msg_cache"

struct m_list {
	struct list_head ptr;
	char msg[PAGE_SIZE];
};

struct m_stat {
	ssize_t converted;
	ssize_t read_call;
	ssize_t write_call;
};

static struct kmem_cache *msg_cache_ptr;
static struct m_stat      stat_info;
static struct m_list      msg;


static void cstr(void *p)
{
	static int cnt;

	pr_info("memory_slab_list: kmem_cache_create called : %d", ++cnt);
}

static int msg_cache_create(void)
{
	pr_info("memory_slab_list: cache created.\n");
	msg_cache_ptr = kmem_cache_create(MSG_CACHE_NAME, sizeof(struct m_list),
		0, SLAB_HWCACHE_ALIGN, cstr);
	return msg_cache_ptr ? 0 : -EFAULT;
}

static void msg_cache_destroy(void)
{
	kmem_cache_destroy(msg_cache_ptr);
	pr_info("memory_slab_list: cache destroyed!\n");
}

static void msg_list_free(void)
{
	struct m_list *ptr, *tmp;

	list_for_each_entry_safe(ptr, tmp, &msg.ptr, ptr) {
		list_del(&ptr->ptr);
		pr_info("memory_slab_list: freeing => %s\n", ptr->msg);
		kmem_cache_free(msg_cache_ptr, ptr);
	}
}

static void msg_list_init(void)
{
	INIT_LIST_HEAD(&msg.ptr);
	pr_info("memory_slab_list: new message list created.\n");
}

static int to_lower(char *c)
{
	int ret_code;

	ret_code = 0;
	if ('A' <= *c && 'Z' >= *c) {
		*c = 'a' + *c - 'A';
		ret_code = 1;
	}
	return ret_code;
}

static int cvrt_message(char *message)
{
	int cnt;
	int indx;

	cnt = 0;
	indx = 0;
	while (message[indx])
		cnt += to_lower(&message[indx++]);
	return cnt;
}

static ssize_t cvrt_read(struct class *class, struct class_attribute *attr,
							char *buf)
{
	static struct m_list *node;

	pr_info("memory_slab_list: call %s", __func__);
	list_for_each_entry_reverse(node, &msg.ptr, ptr) {
		sprintf(buf + strlen(buf), node->msg);
	}
	sprintf(buf + strlen(buf), "\n");
	++stat_info.read_call;
	return strlen(buf);
}

static ssize_t cvrt_write(struct class *class,
	struct class_attribute *attr,
	const char *buf, size_t count)
{
	struct m_list *tmp;

	pr_info("memory_slab_list: call %s", __func__);
	++stat_info.write_call;
	tmp =  kmem_cache_alloc(msg_cache_ptr, GFP_KERNEL);
	strscpy(tmp->msg, buf, count);
	stat_info.converted += cvrt_message(tmp->msg);
	list_add_tail(&tmp->ptr, &msg.ptr);
	return count;
}


static ssize_t stat_info_read(struct class *class,
				struct class_attribute *attr, char *buf)
{
	struct m_list *node;

	pr_info("memory_slab_list: call %s", __func__);

	sprintf(buf, "Converter statistics:\n");
	sprintf(buf + strlen(buf), "Called: %d\nWrite: %d\nRead : %d\n",
			stat_info.read_call + stat_info.write_call,
			stat_info.write_call, stat_info.read_call);
	sprintf(buf + strlen(buf), "Converted: %d\n", stat_info.converted);
	sprintf(buf + strlen(buf), "Now in buffer: ");
	list_for_each_entry_reverse(node, &msg.ptr, ptr) {
		sprintf(buf + strlen(buf), node->msg);
	}
	sprintf(buf + strlen(buf), "\n");
	return strlen(buf);
}


struct class_attribute class_attr_cvrt = {
	.attr = { .name = "cvrt", .mode = 0666 },
	.show	= cvrt_read,
	.store	= cvrt_write
};


struct class_attribute class_attr_stat_info = {
	.attr = { .name = "stat", .mode = 0666 },
	.show	= stat_info_read
};


static struct class *attr_class;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "toLower");
	if (attr_class == NULL) {
		pr_err("memory_slab_list: error creating sysfs class\n");
		return -ENOMEM;
	}
	ret = class_create_file(attr_class, &class_attr_cvrt);
	if (ret) {
		pr_err("memory_slab_list: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}
	ret = class_create_file(attr_class, &class_attr_stat_info);
	if (ret) {
		pr_err("memory_slab_list: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}
	msg_cache_create();
	msg_list_init();
	pr_info("memory_slab_list: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	msg_list_free();
	msg_cache_destroy();
	class_remove_file(attr_class, &class_attr_cvrt);
	class_remove_file(attr_class, &class_attr_stat_info);
	class_destroy(attr_class);

	pr_info("memory_slab_list: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Memory slab list example`");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");