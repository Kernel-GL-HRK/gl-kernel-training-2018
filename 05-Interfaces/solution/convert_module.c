#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/ctype.h>

#define UP_CONV_MAXLEN 128

static char uppercase_conv[UP_CONV_MAXLEN + 1];
static ssize_t uppercase_conv_size;

#define LO_CONV_MAXLEN 128

static char lowercase_conv[LO_CONV_MAXLEN + 1];


static ssize_t lowercase_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	strcpy(buf, lowercase_conv);
	return strlen(buf);
}

static ssize_t lowercase_store(struct class *class,
				struct class_attribute *attr, const char *buf,
				size_t count)
{
	size_t cpy_count = (count < LO_CONV_MAXLEN) ? count : LO_CONV_MAXLEN;

	strncpy(lowercase_conv, buf, cpy_count);

	/*
	 * explicitly make string null-terminated in case
	 * if cpy_count is equal to LO_CONV_MAXLEN
	 */
	lowercase_conv[cpy_count] = '\0';

	int i;

	for (i = 0; i < cpy_count; i++)
		lowercase_conv[i] = tolower(lowercase_conv[i]);

	return count;
}

CLASS_ATTR_RW(lowercase);

static ssize_t up_conv_write(struct file *file, const char __user *pbuf,
				size_t count, loff_t *ppos)
{

	if (uppercase_conv_size == UP_CONV_MAXLEN)
		return -ENOMEM;

	size_t copy_count = min(count, (size_t)UP_CONV_MAXLEN - *ppos);

	if (copy_from_user(uppercase_conv + *ppos, pbuf, copy_count))
		return -EFAULT;

	uppercase_conv_size += copy_count;

	pr_info("uppercase converter: %d bytes written\n", copy_count);

	int i;

	for (i = *ppos; i < (*ppos + copy_count); i++)
		uppercase_conv[i] = toupper(uppercase_conv[i]);

	*ppos += copy_count;

	return copy_count;
}

static ssize_t up_conv_read(struct file *file, char __user *pbuf,
				size_t count, loff_t *ppos)
{
	size_t bytes_left = uppercase_conv_size - *ppos;

	size_t read_amount = min(bytes_left, count);

	/*do not copy anything , if read amount is 0*/
	if (read_amount
		&& copy_to_user(pbuf, uppercase_conv + *ppos, read_amount))
		return -EFAULT;

	*ppos += read_amount;

	pr_info("uppercase converter: %d bytes read\n", read_amount);
	return read_amount;
}

const static struct file_operations uppercase_ops = {
	.owner = THIS_MODULE,
	.read = up_conv_read,
	.write = up_conv_write,
};

static struct proc_dir_entry *ent;

static int conv_file_created;
static struct class *lowercase_conv_class;

static void converter_exit(void)
{

	if (conv_file_created)
		class_remove_file(lowercase_conv_class, &class_attr_lowercase);

	if (lowercase_conv_class)
		class_destroy(lowercase_conv_class);

	if (ent)
		proc_remove(ent);

	pr_info("module removed from kernel\n");
}

static int converter_init(void)
{
	ent = proc_create("to_uppercase", 0666, NULL, &uppercase_ops);
	if (ent == NULL) {
		pr_err("uppercase converter: error creating procfs entry\n");
		converter_exit();
		return -ENOMEM;
	}

	lowercase_conv_class = class_create(THIS_MODULE, "to_lowercase");
	if (lowercase_conv_class == NULL) {
		pr_err("lowercase converter: error creating sysfs class\n");
		converter_exit();
		return -ENOMEM;
	}


	int creation_err;

	creation_err = class_create_file(lowercase_conv_class,
					&class_attr_lowercase);
	if (creation_err) {
		pr_err("lowercase converter: error creating sysfs class attribute\n");
		converter_exit();
		return creation_err;
	}

	conv_file_created = 1;

	pr_info("module loaded\n");
	return 0;
}

module_init(converter_init);
module_exit(converter_exit);

MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("String converter kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
