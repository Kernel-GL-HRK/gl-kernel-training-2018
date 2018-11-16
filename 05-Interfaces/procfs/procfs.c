#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

#define DIR_NAME	"uppercase"
#define ENT1_NAME	"converter"
#define ENT2_NAME	"statistic"

static struct proc_dir_entry *dir;
static struct proc_dir_entry *ent1, *ent2;

static struct
{
	uint32_t nread;
	uint32_t nwrite;
	uint32_t charprocessed;
	uint32_t charconverted;
} stat;

static char buf[PAGE_SIZE];
static ssize_t len;

static char bufstat[PAGE_SIZE];
static ssize_t lenstat;

static ssize_t readconv(struct file *file,
					char __user *pbuf,
					size_t count,
					loff_t *ppos)
{
	char *path;
	ssize_t nreaded;

	path = dentry_path_raw(file->f_path.dentry, buf, sizeof(buf));
	pr_info("uppercase:\n"
			"\tread file path: %s\n"
			"\tread file full path: %s\n"
			"\tpbuf: %p\n"
			"\tcount: %zu\n",
			file->f_path.dentry->d_iname,
			path,
			pbuf,
			count
	);

	if (copy_to_user(pbuf, buf, len))
		return -EFAULT;

	nreaded = len;
	len -= len;
	if (len != 0)
		stat.nread++;

	return nreaded;
}

static ssize_t readstat(struct file *file,
					char __user *pbuf,
					size_t count,
					loff_t *ppos)
{
	char *path;

	path = dentry_path_raw(file->f_path.dentry, buf, sizeof(buf));
	pr_info("uppercase:\n"
			"\tread file path: %s\n"
			"\tread file full path: %s\n"
			"\tpbuf: %p\n"
			"\tcount: %zu\n",
			file->f_path.dentry->d_iname,
			path,
			pbuf,
			count
	);

	if (lenstat == 0) {
		lenstat = snprintf(bufstat, sizeof(bufstat),
							"read: %u\n"
							"write: %u\n"
							"char processed: %u\n"
							"char converted: %u\n",
							stat.nread,
							stat.nwrite,
							stat.charprocessed,
							stat.charconverted
		);
		lenstat++; // '\0'
	} else {
		lenstat = 0;
	}

	if (copy_to_user(pbuf, bufstat, lenstat))
		return -EFAULT;
	return lenstat;
}

static ssize_t writeconv(struct file *file,
					const char __user *pbuf,
					size_t count,
					loff_t *ppos)
{
	char *path, *s, c;

	path = dentry_path_raw(file->f_path.dentry, buf, sizeof(buf));
	pr_info("uppercase:\n"
			"\twrite file path: %s\n"
			"\twrite file full path: %s\n"
			"\tpbuf: %p\n"
			"\tcount: %zu\n",
			file->f_path.dentry->d_iname,
			path,
			pbuf,
			count
	);
	stat.nwrite++;

	if (copy_from_user(buf, pbuf, count))
		return -EFAULT;
	buf[count] = 0;

	// Convert
	s = buf;
	while (*s != 0) {
		c = toupper(*s);
		if (c != *s)
			stat.charconverted++;
		*s++ = c;
	}
	len = s - buf;
	stat.charprocessed += len;

	return count;
}

static int mymodule_init(void)
{
	static const struct file_operations ops_conv = {
		.owner = THIS_MODULE,
		.read = readconv,
		.write = writeconv
	};

	static const struct file_operations ops_stat = {
		.owner = THIS_MODULE,
		.read = readstat,
	};

	dir = proc_mkdir(DIR_NAME, NULL);
	if (dir == NULL) {
		pr_err("uppercase: error creating procfs directory\n");
		return -ENOMEM;
	}

	ent1 = proc_create(ENT1_NAME, 0666, dir, &ops_conv);
	if (ent1 == NULL) {
		pr_err("caseconv: error creating procfs entry 1\n");
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	ent2 = proc_create(ENT2_NAME, 0444, dir, &ops_stat);
	if (ent2 == NULL) {
		pr_err("uppercase: error creating procfs entry 2\n");
		remove_proc_entry(ENT1_NAME, dir);
		remove_proc_entry(DIR_NAME, NULL);
		return -ENOMEM;
	}

	pr_info("uppercase: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	proc_remove(dir);
	pr_info("uppercase: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Roman.Storozhenko <d2.718l@gmail.com>");
MODULE_DESCRIPTION("Simple string uppercase converter using procfs");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

