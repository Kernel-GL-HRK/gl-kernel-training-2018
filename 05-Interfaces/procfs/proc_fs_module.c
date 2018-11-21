#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define MODULE_NAME "my_procfs"


struct {
	char 	buf[PAGE_SIZE];
	ssize_t buf_size;
	struct 	proc_dir_entry* folder;
	struct 	proc_dir_entry* convert_entry;
	ssize_t total_processed_bytes;
	ssize_t total_altered_bytes;
} module_state;
 
static void convert_buff(void)
{
	ssize_t pos = 0;
	for(; pos < module_state.buf_size; ++pos)
	{
		char cs = module_state.buf[pos];
		if(cs >='a' && cs <='z')
		{
			++module_state.total_altered_bytes;
			module_state.buf[pos]  += 'A'-'a' ;
		}
		++module_state.total_processed_bytes;
	}
}

static ssize_t convert_write(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t not_copied;

	size_t safe_size = min_t(ssize_t,sizeof(module_state.buf),count);

	not_copied = copy_from_user(module_state.buf, pbuf, safe_size);
	module_state.buf_size = safe_size - not_copied;
	convert_buff();

	return module_state.buf_size;
}

static ssize_t convert_read(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied = 0;

	num = min_t(ssize_t, module_state.buf_size, count);
	
	if (num) {
		not_copied = copy_to_user(pbuf, module_state.buf, num);
		num -= not_copied;
	}
	
	if(not_copied) {//some data remains in buffer, so adjast this data
		ssize_t copied_bytes = module_state.buf_size-not_copied;
		ssize_t pos = 0;
		for(; pos + copied_bytes < module_state.buf_size; ++pos)
			module_state.buf[pos] = module_state.buf[pos+copied_bytes];
	}
	

	module_state.buf_size = not_copied; /* Indicate EOF on next read */
	return num;
}

	static struct file_operations entry_ops; 

static int mymodule_init(void)
{
	memset(&module_state, 0, sizeof(module_state));

	module_state.folder=proc_mkdir(MODULE_NAME,NULL);
	if(!module_state.folder){
		pr_err(MODULE_NAME": error creating procfs entry\n");
		return -ENOMEM;
	}

	memset(&entry_ops,0,sizeof(struct file_operations));
	
	entry_ops.owner = THIS_MODULE;
	entry_ops.read = convert_read;
	entry_ops.write = convert_write;
	

	module_state.convert_entry = proc_create("conv_UPPERCASE", 0666, module_state.folder, &entry_ops);
	if (!module_state.convert_entry) {
		proc_remove(module_state.folder);
		pr_err(MODULE_NAME": error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info(MODULE_NAME": module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(module_state.folder);
	proc_remove(module_state.convert_entry);
	pr_info(MODULE_NAME": module unloaded\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Konstantin E Felix <constaineem@ukr.net>");
MODULE_DESCRIPTION("procfs trainig module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
