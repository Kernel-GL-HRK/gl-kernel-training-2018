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

	pr_info("simple_procfs: %s: count=%d\n", __func__, count);

	not_copied = copy_from_user(msg, pbuf, count);

	msg_size = count - not_copied;

	pr_info("simple_procfs: %s: msg_size=%d\n", __func__, msg_size);
	return msg_size;
}

static ssize_t myread(struct file *file, char __user *pbuf,
size_t count, loff_t *ppos)
{
	int i = 0;
	ssize_t num, not_copied;

	pr_info("simple_procfs: %s: count=%d\n", __func__, count);

	num = min_t(ssize_t, msg_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		num -= not_copied;
	}

	for (i = 0; i < num; i++) {
		if (pbuf[i] <= (int)'z' && pbuf[i] >= (int)'a')
			pbuf[i] = (int)pbuf[i] - (int)'a' + (int)'A';
	}

	msg_size = 0; /* Indicate EOF on next read */

	pr_info("simple_procfs: %s: return=%d\n", __func__, num);
	return num;
}


static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static struct proc_dir_entry *ent;


static int simple_procfs_init(void)
{
	ent = proc_create("simple_procfs", 0644, NULL, &myops);
	if (ent == NULL) {
		pr_err("simple_procfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("simple_procfs: module loaded\n");
	return 0;
}

static void simple_procfs_exit(void)
{
	proc_remove(ent);

	pr_info("simple_procfs: module exited\n");
}

module_init(simple_procfs_init);
module_exit(simple_procfs_exit);

MODULE_AUTHOR("Oleksandr.Siruk <alexsirook@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
