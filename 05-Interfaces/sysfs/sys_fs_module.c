#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/string.h>

#define MODULE_NAME "my_sysfs"


struct {
	char 	buf[PAGE_SIZE];
	ssize_t buf_size;
	ssize_t total_processed_bytes;
	ssize_t total_altered_bytes;
} module_state;
 
static void convert_buff(void)
{
	ssize_t pos = 0;
	for(; pos < module_state.buf_size; ++pos)
	{
		char cs = module_state.buf[pos];
		if(cs >='A' && cs <='Z')
		{
			++module_state.total_altered_bytes;
			module_state.buf[pos]  -= 'A'-'a' ;
		}
		++module_state.total_processed_bytes;
	}
}

static ssize_t rw_show(struct class *class, struct class_attribute *attr, char *buf)
{
	memcpy(buf, module_state.buf, module_state.buf_size);
	return module_state.buf_size;
}

static ssize_t rw_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
    module_state.buf_size = min_t(ssize_t,sizeof(module_state.buf),count);

	memcpy(module_state.buf, buf, module_state.buf_size);
	convert_buff();
	return module_state.buf_size;
}

static ssize_t stat_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return snprintf(buf,PAGE_SIZE,
		MODULE_NAME": usage\n"\
		"\t bytes processed %d\n"\
		"\t bytes converted %d\n", module_state.total_processed_bytes, module_state.total_altered_bytes);
}


// CLASS_ATTR_RW(rw);
struct class_attribute class_attr_rw = {
	.attr = { .name = "convert", .mode = 0666 },
	.show	= rw_show,
	.store	= rw_store
};

CLASS_ATTR_RO(stat);
// struct class_attribute class_attr_stat = {
// 	.attr = { .name = "stat", .mode = 0444 },
// 	.show	= stat
// };

static struct class *attr_class = 0;


static int mymodule_init(void)
{
	int ret;
	
	memset(&module_state,0, sizeof(module_state));

	attr_class = class_create(THIS_MODULE, MODULE_NAME);
	if (attr_class == NULL) {
		pr_err("mymodule: error creating sysfs class\n");
		return -ENOMEM;
	}

	ret = class_create_file(attr_class, &class_attr_rw);
	if (ret) {
		pr_err("mymodule: error creating sysfs class  'convert' attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("mymodule: error creating sysfs class 'stat' attribute\n");
		class_remove_file(attr_class, &class_attr_rw);
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_stat);
	class_remove_file(attr_class, &class_attr_rw);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Kostiantyn E Felix");
MODULE_DESCRIPTION("Simple Kernel sys_fs module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
