#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/slab.h>

static const int cache_size = PAGE_SIZE;
static void *msg_buf;
static struct kmem_cache *km_ptr;
static const char *km_name = "my_cache";
static char *msg;

static struct cvrt_stat {
	ssize_t converted;
	ssize_t read_call;
	ssize_t write_call;
} info;

static void cstr(void *p)
{
	static int cnt;

	pr_info("memory_slab: %s function call: %d", __func__, ++cnt);
}

static int my_cache_create(void)
{
	pr_info("memory_slab: cache create!\n");
	km_ptr = kmem_cache_create(km_name, cache_size, 0,
				SLAB_HWCACHE_ALIGN, cstr);
	return km_ptr ? 0 : -EFAULT;
}

static int my_cache_destroy(void)
{
	kmem_cache_destroy(km_ptr);
	pr_info("memory_slab: cache destroy!\n");
}

static void mem_allocate(void)
{
	msg_buf = kmem_cache_alloc(km_ptr, GFP_KERNEL);
	msg = (char *)msg_buf;
	pr_info("memory_slab: memory allocate.\n");
}

static void mem_free(void)
{
	if (msg_buf)
		kmem_cache_free(km_ptr, msg_buf);
	pr_info("memory_slab: memory free.\n");
}

static int to_lower(char *message)
{
	int cnt;
	int indx;

	cnt = 0;
	indx = 0;
	while (message[indx]) {
		if ('A' <= message[indx] &&  'Z' >= message[indx]) {
			message[indx] = 'a' + (message[indx] - 'A');
			++cnt;
		}
		++indx;
	}
	return cnt;
}

static ssize_t cvrt_read(struct class *class, struct class_attribute *attr,
							char *buf)
{
	pr_info("memory_slab: call %s", __func__);
	strspy(buf, msg, strlen(msg));
	++info.read_call;
	return strlen(buf);
}

static ssize_t cvrt_write(struct class *class,
	struct class_attribute *attr,
	const char *buf, size_t count)
{
	int cnt = 0;

	pr_info("memory_slab: call %s", __func__);
	++info.write_call;
	mem_free();
	mem_allocate();
	strscpy(msg, buf, count);
	cnt = to_lower(msg);
	info.converted += cnt;
	return count;
}


static ssize_t info_read(struct class *class, struct class_attribute *attr,
							char *buf)
{

	pr_info("memory_slab: call %s", __func__);

	sprintf(buf, "Converter statistics:\n");
	sprintf(buf + strlen(buf), "Called: %d\nWrite: %d\nRead : %d\n",
			info.read_call + info.write_call, info.write_call,
			info.read_call);
	sprintf(buf + strlen(buf), "Converted: %d\n", info.converted);
	sprintf(buf + strlen(buf), "Now in buffer: %s\n", msg);

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
		pr_err("memory_slab: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_cvrt);
	if (ret) {
		pr_err("memory_slab: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}


	ret = class_create_file(attr_class, &class_attr_info);
	if (ret) {
		pr_err("memory_slab: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	my_cache_create();
	pr_info("memory_slab: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	mem_free();
	my_cache_destroy();
	class_remove_file(attr_class, &class_attr_cvrt);
	class_remove_file(attr_class, &class_attr_info);
	class_destroy(attr_class);

	pr_info("memory_slab: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Memory slab allocator example`");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");


