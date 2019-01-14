#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


static char msg[PAGE_SIZE];
static ssize_t msg_size;

static struct cvrt_stat {
	ssize_t copied;
	ssize_t not_copied;
	ssize_t converted;
	ssize_t read_call;
	ssize_t write_call;
} stat_store;

static ssize_t mywrite(struct file *file, const char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	ssize_t indx;


	pr_info("myprocfs: %s: count=%d\n", __func__, count);
	not_copied = copy_from_user(msg, pbuf, count);
	++stat_store.write_call;
	stat_store.not_copied += not_copied;
		pr_info("myprocfs: %s: not_copied=%d\n", __func__, not_copied);
	msg_size = count - not_copied;
	stat_store.copied += msg_size;
	pr_info("myprocfs: %s: msg_size=%d\n", __func__, msg_size);
	for (indx = 0; indx < msg_size; ++indx)
		if ('a' <= msg[indx] &&  'z' >= msg[indx]) {
			msg[indx] = 'A' + (msg[indx] - 'a');
			++stat_store.converted;
		}

	return msg_size;
}

static ssize_t myread(struct file *file, char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	pr_info("myprocfs: %s: count=%d\n", __func__, count);
	num = min_t(ssize_t, msg_size, count);
	pr_info("myprocfs: %s: msg_size=%d\n", __func__, msg_size);
	++stat_store.read_call;
	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		pr_info("myprocfs: %s: not_copied=%d\n", __func__, not_copied);
		stat_store.not_copied += not_copied;
		num -= not_copied;
	}
	msg_size = 0; /* Indicate EOF on next read */
	stat_store.copied += num;
	pr_info("myprocfs: %s: return=%d\n", __func__, num);
	return num;
}

static ssize_t mystat(struct file *file, char __user *pbuf,
size_t count, loff_t *ppos)
{

	char buf[512];
	ssize_t len;

	sprintf(buf, "Converter statistics:\n");
	sprintf(buf + strlen(buf), "Called: %d\nWrite: %d\nRead : %d\n",
		stat_store.read_call + stat_store.write_call, stat_store.write_call,
		stat_store.read_call);
	sprintf(buf + strlen(buf), "Copied: %d\nNot copied: %d\nConverted: %d\n",
		stat_store.copied, stat_store.not_copied, stat_store.converted);

	len = strlen(buf);

	if (*ppos > 0 || count < len) {
		pr_info("myprocfs: %s: successful copy data to user.", __func__);
		return 0;
	}

	if (copy_to_user(pbuf, buf, len)) {
		pr_info("myprocfs: %s: error copy data to user.", __func__);
		return -EFAULT;
	}

	*ppos = len;

	return len;
}

static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static const struct file_operations mystatops = {
	.owner = THIS_MODULE,
	.read = mystat,
};

static struct proc_dir_entry *root_ent;
static struct proc_dir_entry *cvrt_ent;
static struct proc_dir_entry *stat_ent;



static int mymodule_init(void)
{
	root_ent = proc_mkdir("myprocfs", NULL);
	if (root_ent == NULL) {
		pr_err("myprocfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	cvrt_ent = proc_create("convertor", 0644, root_ent, &myops);
	if (cvrt_ent == NULL) {
		pr_err("myprocfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	stat_ent = proc_create("stat", 0444, root_ent, &mystatops);
	if (stat_ent == NULL) {
		pr_err("myprocfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("myprocfs: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(root_ent);

	pr_info("myprocfs: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Volodymyr Savchenko <savchenko.volod@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module converter to upper case");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");