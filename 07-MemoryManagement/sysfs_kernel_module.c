#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/mempool.h>


struct call_data {
	char *str;
	ssize_t total;
	ssize_t updated;
	struct list_head list;
};

LIST_HEAD(calls_list);
struct kmem_cache *slab;
mempool_t *pool;
static bool is_data_obtained;

static bool to_upper(char *lower);

struct call_data *call_data_create(const char *buf)
{
	int i = 0;
	int length = 0;
	struct call_data *ret = NULL;

	ret = mempool_alloc(pool, GFP_KERNEL);

	length = strlen(buf);
	ret->str = vmalloc(length + 1);
	strcpy(ret->str, buf);
	ret->total = length;
	ret->updated = 0;

	for (i = 0; i < length; i++)	{
		if ('\n' == ret->str[i])
			ret->str[i] = '\0';
		else if (to_upper(&ret->str[i]))
			++ret->updated;
	}

	return ret;
}

void call_data_destroy(struct call_data *data)
{
	vfree(data->str);
	mempool_free(data, pool);
}

void call_data_dump(struct call_data *data)
{
	pr_info("mymodule: rw_store: value = %s\n", data->str);
	pr_info("mymodule: rw_store: total = %d\n", data->total);
	pr_info("mymodule: rw_store: updated = %d\n", data->updated);

}

static bool to_upper(char *lower)
{
	if (*lower >= 'a' && *lower <= 'z') {
		*lower = *lower + 'A' - 'a';
		return true;
	}
	return false;
}

static void fill_in_buf(char *buf, struct call_data *data)
{
	sprintf(buf, "mymodule: rw_store: value = %s\n"
				"mymodule: rw_store: total = %d\n"
				"mymodule: rw_store: updated = %d\n",
				data->str, data->total, data->updated);
}

static ssize_t rw_show(struct class *class, struct class_attribute *attr,
					char *buf)
{
	struct call_data *data = NULL;
	struct list_head *position = NULL;

	if (is_data_obtained) {
		data = list_first_entry(&calls_list, struct call_data, list);
		fill_in_buf(buf, data);
		is_data_obtained = false;
	} else {
		list_for_each(position, &calls_list) {
			data = list_entry(position, struct call_data, list);
			fill_in_buf(buf + strlen(buf), data);
		}
	}

	return strlen(buf);
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr,
						const char *buf, size_t count)
{
	struct call_data *data = NULL;

	data = call_data_create(buf);
	INIT_LIST_HEAD(&data->list);
	list_add(&data->list, &calls_list);
	call_data_dump(data);
	is_data_obtained = true;

	return count;
}

static ssize_t r_show(struct class *class, struct class_attribute *attr,
						char *buf)
{
	struct call_data *data = NULL;
	struct list_head *position = NULL;
	ssize_t total = 0;
	ssize_t updated = 0;
	ssize_t calls = 0;

	list_for_each(position, &calls_list) {
			data = list_entry(position, struct call_data, list);
			calls++;
			total += data->total;
			updated += data->updated;
		}

	sprintf(buf, "Calls:     %d\n"
			"Processed: %d\n"
			"Converted: %d\n",
			calls, total, updated);
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

static struct class *attr_class;

static int mymodule_init(void)
{
	int ret;

	is_data_obtained = false;
	slab = kmem_cache_create("call_data",
		sizeof(struct call_data), 0, 0, NULL);
	pool = mempool_create(256, mempool_alloc_slab, mempool_free_slab, slab);

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

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	struct list_head *position, *n;

	class_remove_file(attr_class, &class_attr_rw);
	class_remove_file(attr_class, &class_attr_r);
	class_destroy(attr_class);

	list_for_each_safe(position, n, &calls_list) {
		struct call_data *data = list_entry(position,
								struct call_data, list);
		list_del(position);
		pr_info("Destroy:\n");
		call_data_dump(data);
		call_data_destroy(data);
	}
	pr_info("List empty: %d\n", list_empty(&calls_list));

	mempool_destroy(pool);
	kmem_cache_destroy(slab);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_VERSION("0.1");
