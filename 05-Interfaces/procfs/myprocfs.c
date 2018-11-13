#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


static char msg[PAGE_SIZE];
static ssize_t msg_size;

static ssize_t mywrite(struct file *file, const char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	ssize_t indx;


	pr_info("myprocfs: %s: count=%d\n", __func__, count);
	not_copied = copy_from_user(msg, pbuf, count);
	pr_info("myprocfs: %s: not_copied=%d\n", __func__, not_copied);
	msg_size = count - not_copied;
	pr_info("myprocfs: %s: msg_size=%d\n", __func__, msg_size);
	for (indx = 0; indx < msg_size; ++indx)
		if ('a' <= msg[indx] &&  'z' >= msg[indx])
			msg[indx] = 'A' + (msg[indx] - 'a');
	return msg_size;
}

static ssize_t myread(struct file *file, char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("myprocfs: %s: count=%d\n", __func__, count);
	num = min_t(ssize_t, msg_size, count);
	pr_info("myprocfs: %s: msg_size=%d\n", __func__, msg_size);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		pr_info("myprocfs: %s: not_copied=%d\n", __func__, not_copied);
		num -= not_copied;
	}
	msg_size = 0; /* Indicate EOF on next read */
	pr_info("myprocfs: %s: return=%d\n", __func__, num);
	return num;
}


static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static struct proc_dir_entry *ent;


static int mymodule_init(void)
{
	ent = proc_create("myprocfs", 0644, NULL, &myops);
	if (ent == NULL) {
		pr_err("myprocfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("myprocfs: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(ent);

	pr_info("myprocfs: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module converter to upper case");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");