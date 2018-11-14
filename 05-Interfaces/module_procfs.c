#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>


#define DIR_NAME	"my_procfs_dir"
#define ENT1_NAME	"uppercase"
#define ENT2_NAME	"statistics"

static char msg[PAGE_SIZE];
static ssize_t msg_size;
static char stat_msg[PAGE_SIZE];
static size_t stat_msg_size;
static int call_read;
static int call_write;
static int total_char;

static ssize_t mywrite(struct file *file, const char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t not_copied;
	int i;

	call_write++;

	pr_info("mymodule: mywrite: count=%d\n", count);

	not_copied = copy_from_user(msg, pbuf, count);

	msg_size = count - not_copied;

	for (i = 0; i < msg_size; i++) {
		total_char++;
		msg[i] = toupper(msg[i]);
	}

	pr_info("mymodule: mywrite: msg_size=%d\n", msg_size);
	return msg_size;
}

static ssize_t myread(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	ssize_t num, not_copied;

	call_read++;

	pr_info("mymodule: myread: count=%d\n", count);

	num = min_t(ssize_t, msg_size, count);

	if (num) {
		not_copied = copy_to_user(pbuf, msg, num);
		num -= not_copied;
	}

	msg_size = 0; /* Indicate EOF on next read */

	pr_info("mymodule: myread: return=%d\n", num);
	return num;
}

static ssize_t stat_read(struct file *file, char __user *pbuf, size_t count, loff_t *ppos)
{
	pr_info("Call Statistics:\ncalled Read %d - called Write"
		" %d Total characters %d\n", call_read, call_write, total_char);

	stat_msg_size = sprintf(stat_msg, "Call Statistics:\n called Read %d"
	" - called Write %d Total characters %d\n", call_read, call_write, total_char);

	if (count < stat_msg_size)
		return -EINVAL;
	if (*ppos != 0)
		return 0;
	if (copy_to_user(pbuf, stat_msg, stat_msg_size))
		return -EINVAL;

	*ppos = stat_msg_size;

	pr_info("mymodule: myread: return=%d\n", stat_msg_size);
	return stat_msg_size;
}


static const struct file_operations rw_ops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static const struct file_operations r_ops = {
	.owner = THIS_MODULE,
	.read = stat_read,
};


static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;


static int mymodule_init(void)
{
	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("mymodule: error creating: proc/my_procfs_dir\n");
		return -ENOMEM;
	}

	ent1 = proc_create(ENT1_NAME, 0666, dir, &rw_ops);
	if (ent1 == NULL) {
		pr_err("mymodule: error creating: proc/my_procfs_dir/uppercase\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	ent2 = proc_create(ENT2_NAME, 0444, dir, &r_ops);
	if (ent2 == NULL) {
		pr_err("mymodule: error creating: /proc/my_procfs_dir/statistics\n");
		remove_proc_entry(ENT1_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("mymodule: module loaded\nCreate:\n"
		"/proc/my_procfs_dir/uppercase\n /proc/my_procfs_dir/statistics\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(dir);

	pr_info("mymodule: module exited\nRemove:\n"
		"/proc/my_procfs_dir/uppercase\n/proc/my_procfs_dir/statistics\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Dmitriy Ganshin");
MODULE_DESCRIPTION("Simple Kernel module example procfs: write, read, statistics");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
