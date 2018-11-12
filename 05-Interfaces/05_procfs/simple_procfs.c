#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


static char msg[PAGE_SIZE];
static ssize_t msg_size;
static char msg_stat[PAGE_SIZE];
static ssize_t msg_stat_size;
static int converted_cnt;
static int processed_cnt;
static int read_cnt;
static int write_cnt;
static char stat_update_flag;

static ssize_t mywrite(struct file *file, const char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t not_copied;

	pr_info("simple_procfs: %s: count=%d\n", __func__, count);

	not_copied = copy_from_user(msg, pbuf, count);

	msg_size = count - not_copied;
	processed_cnt += msg_size;
	pr_info("simple_procfs: %s: msg_size=%d\n", __func__, msg_size);
	write_cnt++;
	stat_update_flag = 1;
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
		for (i = 0; i < num; i++) {
			if (pbuf[i] <= (int)'z' && pbuf[i] >= (int)'a') {
				pbuf[i] = (int)pbuf[i] - (int)'a' + (int)'A';
				converted_cnt++;
			}
		}
	}
	msg_size = 0; /* Indicate EOF on next read */
	pr_info("simple_procfs: %s: return=%d\n", __func__, num);
	read_cnt++;
	return num;
}

static ssize_t myread_stat(struct file *file, char __user *pbuf,
size_t count, loff_t *ppos)
{
	ssize_t num = 0, not_copied;

	pr_info("simple_procfs_stat: %s: count=%d\n", __func__, count);
	if (!stat_update_flag)
		goto exit_point;
	msg_stat_size = snprintf(msg_stat, PAGE_SIZE - 1,
	"Converted count = %d\n", converted_cnt);
	msg_stat_size += snprintf(msg_stat + msg_stat_size,
	PAGE_SIZE - msg_stat_size - 1, "Processed count = %d\n", processed_cnt);
	msg_stat_size += snprintf(msg_stat + msg_stat_size,
	PAGE_SIZE - msg_stat_size - 1, "Read count = %d\n", read_cnt);
	msg_stat_size += snprintf(msg_stat + msg_stat_size,
	PAGE_SIZE - msg_stat_size - 1, "write count = %d\n\n", write_cnt);
	num = min_t(ssize_t, msg_stat_size, count);
	if (num) {
		not_copied = copy_to_user(pbuf, msg_stat, num);
		num -= not_copied;
	}

exit_point:
	stat_update_flag = 0; // indicate EOF for next build
	pr_info("simple_procfs_stat: %s: return=%d\n", __func__, num);
	return num;
}


static const struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static const struct file_operations myops_stat = {
	.owner = THIS_MODULE,
	.read = myread_stat,
};

static struct proc_dir_entry *ent;
static struct proc_dir_entry *ent_stat;


static int simple_procfs_init(void)
{
	converted_cnt = 0;
	processed_cnt = 0;
	read_cnt = 0;
	write_cnt = 0;
	stat_update_flag = 0;
	ent = proc_create("simple_procfs", 0644, NULL, &myops);
	if (ent == NULL) {
		pr_err("simple_procfs: error creating procfs entry\n");
		return -ENOMEM;
	}

	ent_stat = proc_create("simple_procfs_stat", 0444, NULL, &myops_stat);
	if (ent_stat == NULL) {
		pr_err("simple_procfs_stat: error creating procfs entry\n");
		return -ENOMEM;
	}

	pr_info("simple_procfs: module loaded\n");
	return 0;
}

static void simple_procfs_exit(void)
{
	proc_remove(ent);
	proc_remove(ent_stat);

	pr_info("simple_procfs: module exited\n");
}

module_init(simple_procfs_init);
module_exit(simple_procfs_exit);

MODULE_AUTHOR("Oleksandr.Siruk <alexsirook@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
