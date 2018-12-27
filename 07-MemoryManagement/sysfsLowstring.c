#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/slab.h>


struct slabMsgType{
	struct 	kmem_cache 	*cachep;
	char 			*string;
};

static struct slabMsgType slabAllocMsg;


struct usageStatistic{
	int convertedCharacters;
	int numcallShow;
};

static struct usageStatistic stats;



void msg_release_items(struct slabMsgType *const msg)
{
	if (NULL == msg)
		return;

	if (NULL != msg->string)
		kmem_cache_free(msg->cachep, msg->string);

	if (NULL != msg->cachep)
		kmem_cache_destroy(msg->cachep);
}

static signed int msg_reserve_items(size_t size, struct slabMsgType *const msg)
{
	
	if (NULL == msg)
		return EINVAL;
	
	msg_release_items(msg);

	msg->cachep = kmem_cache_create("msg_cachep",
					size,
					0,
					SLAB_HWCACHE_ALIGN,
					NULL);

	if (NULL == msg->cachep) {
		pr_err("Error creating cache \n");
		return -ENOMEM;
	}

	msg->string = (char *)kmem_cache_alloc(msg->cachep,GFP_KERNEL);
	
	if (NULL == msg->string) {
		pr_err("Error allocating from cache \n");
		kmem_cache_destroy(msg->cachep);
		return -ENOMEM;
	}

	return 0;
}

static ssize_t converter_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (slabAllocMsg.string) {
		strcpy(buf, slabAllocMsg.string);
		slabAllocMsg.string[0] = '\0';	
	}

	++stats.numcallShow;
	return strlen(buf);
}

static ssize_t converter_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	int index = 0;
	char character;

	int ret = msg_reserve_items(count, &slabAllocMsg); 

	if (!ret) {
		//Convert input string to lower
		while (buf[index]) {
			character = buf[index];
			slabAllocMsg.string[index] = (char)tolower(character);
			++index;
			++stats.convertedCharacters;
		}

		//Copy end of line character
		slabAllocMsg.string[index] = buf[index];
	}
	else { 
		pr_err("mymodule: error msg reserve item\n");
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
	if (NULL == attr_class) {
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
	
	msg_release_items(&slabAllocMsg);

	class_remove_file(attr_class, &class_attr_converter);
	class_remove_file(attr_class, &class_attr_stat);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("Dynamically allocate memory(use SLAB allo).Part1");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
