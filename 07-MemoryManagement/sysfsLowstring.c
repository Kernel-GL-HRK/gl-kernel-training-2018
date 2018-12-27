#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mempool.h>

#define POOL_MINIMUM_NUM	2

struct msgNode{
	struct list_head	list;
	char 			msg[PAGE_SIZE];
};
static struct kmem_cache	*cachep;
static mempool_t 		*pool;
static struct msgNode 		msgList;


struct usageStatistic{
	int convertedCharacters;
	int numcallShow;
};

static struct usageStatistic stats;



int msg_release_items(struct msgNode *const msgList)
{
	

	struct list_head 	*ptr 	= NULL;
	struct list_head 	*next 	= NULL;
	struct msgNode 		*entry 	= NULL;

	if (NULL == msgList)
		return EINVAL;

	list_for_each_safe(ptr, next, &msgList->list){
		entry = list_entry(ptr, struct msgNode, list);
		list_del(&entry->list);
		mempool_free(entry, pool);
	}
	return 0;
}

static ssize_t converter_show(struct class *class, struct class_attribute *attr, char *buf)
{
	struct msgNode 		*entry 	= NULL;
	int isEmpty = list_empty(&msgList.list);
	
	if (isEmpty == 0) {
		entry = list_last_entry(&msgList.list, struct msgNode, list);
		strcpy(buf, entry->msg);
		list_del(&entry->list);
		mempool_free(entry, pool);
	}

	++stats.numcallShow;
	return strlen(buf);
}

int strToLower(const char *const input, char *const output)
{
	int index = 0;
	char character;
	int numCharactersConverted = 0;
	
	if ((NULL == input) || (NULL == output))
		return -EINVAL;
	
	//Convert input string to lower
	while (input[index]) {
		character = input[index];
		output[index] = (char)tolower(character);
		++index;
		++numCharactersConverted;
	}

	//Copy end of line character
	output[index] = input[index];
	return numCharactersConverted;
}

static ssize_t converter_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{

	struct msgNode *newMsgNode = (struct msgNode *)mempool_alloc(pool, GFP_KERNEL);	
							
	if (NULL == newMsgNode) {
		pr_err("mymodule: error msg reserve item\n");
	} else {

		int ret = strToLower(buf, newMsgNode->msg);
		if (ret >= 0) {
			stats.convertedCharacters += ret;
			list_add_tail(&newMsgNode->list, &msgList.list);
		}
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
		class_remove_file(attr_class, &class_attr_converter);
		class_destroy(attr_class);
		return ret;
	}

	INIT_LIST_HEAD(&msgList.list);

	cachep = kmem_cache_create("msg_cachep", sizeof(struct msgNode),
						0,
						SLAB_HWCACHE_ALIGN,
						NULL);

	if (NULL == cachep) {
		pr_err("Error creating cache \n");
		class_remove_file(attr_class, &class_attr_stat);
		class_remove_file(attr_class, &class_attr_converter);
		class_destroy(attr_class);
		return -ENOMEM;
	}

	pool = mempool_create(POOL_MINIMUM_NUM,  mempool_alloc_slab,
	    mempool_free_slab, cachep);

	if (NULL == pool){
		pr_err("Error creating mem pool \n");
		kmem_cache_destroy(cachep);
		class_remove_file(attr_class, &class_attr_stat);
		class_remove_file(attr_class, &class_attr_converter);
		class_destroy(attr_class);
		return -ENOMEM;
	}


	pr_info("mymodule: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	
	msg_release_items(&msgList);
	mempool_destroy(pool);
	kmem_cache_destroy(cachep);

	class_remove_file(attr_class, &class_attr_converter);
	class_remove_file(attr_class, &class_attr_stat);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("MemoryManagement.Part 3");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
