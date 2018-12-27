#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mempool.h>

struct msgNode {
	struct list_head	list;
	char			*msg;
};
static struct kmem_cache	*cachepNode;
static struct kmem_cache	*cachepCharacters;
static mempool_t		*poolCharacters;

static struct msgNode		msgList;


struct usageStatistic {
	int convertedCharacters;
	int numcallShow;
};

static struct usageStatistic stats;


int msg_release_items(struct msgNode *const msgList)
{
	struct list_head	*ptr	= NULL;
	struct list_head	*next	= NULL;
	struct msgNode		*entry	= NULL;

	if (NULL == msgList)
		return -EINVAL;

	list_for_each_safe(ptr, next, &msgList->list) {
		entry = list_entry(ptr, struct msgNode, list);
		list_del(&entry->list);
		mempool_free(entry->msg, poolCharacters);
		kmem_cache_free(cachepNode, entry);
	}
	return 0;
}

static ssize_t converter_show(struct class *class,
				struct class_attribute *attr,
				char *buf)
{
	struct msgNode		*entry = NULL;
	int isEmpty = list_empty(&msgList.list);
	if (isEmpty == 0) {
		entry = list_last_entry(&msgList.list, struct msgNode, list);
		strcpy(buf, entry->msg);
		list_del(&entry->list);
		mempool_free(entry->msg, poolCharacters);
		kmem_cache_free(cachepNode, entry);
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


static unsigned long alignedNearestPow2(unsigned int x)
{
	unsigned long alignedVal = 1;

	if (x < 2)
		return 2;

	while (alignedVal < x)
		alignedVal *= 2;

	return alignedVal;
}

static char *msg_reserve_items(size_t size)
{
	char *newCharMem = NULL;

	if (size > poolCharacters->min_nr) {
		size_t newSize = alignedNearestPow2(size);
		int ret = mempool_resize(poolCharacters, newSize);
		if (ret)
			return newCharMem;
	}
	newCharMem = (char *)mempool_alloc(poolCharacters, GFP_KERNEL);
	return newCharMem;
}


static ssize_t converter_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{

	struct msgNode *newMsgNode = (struct msgNode *)kmem_cache_alloc(cachepNode, GFP_KERNEL);
	int ret = 0;
	if (NULL == newMsgNode) {
		pr_err("mymodule: error msg reserve node item\n");
	} else {

		newMsgNode->msg = msg_reserve_items(count);
		if (NULL == newMsgNode->msg) {
			pr_err("mymodule: error msg reserve char item\n");
			return count;
		}

		ret = strToLower(buf, newMsgNode->msg);
		if (ret >= 0) {
			stats.convertedCharacters += ret;
			list_add_tail(&newMsgNode->list, &msgList.list);
		}
	}
	return count;
}


static ssize_t stat_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	sprintf(buf, "Lowercase converter statistics:\n"
	"Characters was converted: %d\n"
	"Show was called: %d\n",
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
	int ret = 0;

	attr_class = class_create(THIS_MODULE, "strlowConverter");
	if (NULL == attr_class) {
		pr_err("mymodule: error creating sysfs class\n");
		ret = -ENOMEM;
		return ret;
	}

	ret = class_create_file(attr_class, &class_attr_converter);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		goto fail_create_file_converter;
	}


	ret = class_create_file(attr_class, &class_attr_stat);
	if (ret) {
		pr_err("mymodule: error creating sysfs class attribute\n");
		goto fail_create_file_stats;
	}

	INIT_LIST_HEAD(&msgList.list);

	cachepNode = kmem_cache_create("msg_cachepNode", sizeof(struct msgNode),
						0,
						SLAB_HWCACHE_ALIGN,
						NULL);

	if (NULL == cachepNode) {
		pr_err("Error creating cache\n");
		ret =  -ENOMEM;
		goto fail_create_cachepNode;
	}


	cachepCharacters = kmem_cache_create("char_cachep", sizeof(char),
						0,
						SLAB_HWCACHE_ALIGN,
						NULL);

	if (NULL == cachepCharacters) {
		pr_err("Error creating cache for char\n");
		ret = -ENOMEM;
		goto fail_create_cachepCharacters;
	}


	poolCharacters = mempool_create(0, mempool_alloc_slab,
					mempool_free_slab, cachepNode);

	if (NULL == poolCharacters) {
		pr_err("Error creating mem pool\n");
		ret = -ENOMEM;
		goto fail_create_poolCharacters;
	}

	pr_info("mymodule: module loaded\n");
	return 0;

fail_create_poolCharacters:
	kmem_cache_destroy(cachepCharacters);

fail_create_cachepCharacters:
	kmem_cache_destroy(cachepNode);

fail_create_cachepNode:
	class_remove_file(attr_class, &class_attr_stat);

fail_create_file_stats:
	class_remove_file(attr_class, &class_attr_converter);

fail_create_file_converter:
	class_destroy(attr_class);

	return ret;
}

static void mymodule_exit(void)
{
	msg_release_items(&msgList);
	mempool_destroy(poolCharacters);
	kmem_cache_destroy(cachepCharacters);
	kmem_cache_destroy(cachepNode);

	class_remove_file(attr_class, &class_attr_converter);
	class_remove_file(attr_class, &class_attr_stat);
	class_destroy(attr_class);

	pr_info("mymodule: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Nikishyn <rnikishyn@yahoo.com>");
MODULE_DESCRIPTION("Memory Management: Part 4");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
