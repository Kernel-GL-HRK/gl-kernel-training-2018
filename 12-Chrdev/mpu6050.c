// SPDX-License-Identifier: GPL-2.0
#define DEBUG
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#include <mpu6050-regs.h>

#define MODULE_TAG			"mpu6050"
#define CLASS_NAME			"mpu6050"
#define DEVICE_NAME			"accel"

#define ROUND_BUFFER_SIZE 128
struct round_buffer {
	s16 buffer[ROUND_BUFFER_SIZE];
	ssize_t head;
	ssize_t tail;
};

struct mpu6050_device {
	struct i2c_client *client;
	s16 accel_raw_x;
	s16 accel_raw_y;
	s16 accel_raw_z;
	s16 gyro_raw_x;
	s16 gyro_raw_y;
	s16 gyro_raw_z;
	s16 temp_raw;
	u8 is_new_data;
	int irq_gpio_num;
	int irq_num;

	struct round_buffer rbuf_x;
	struct round_buffer rbuf_y;
	struct round_buffer rbuf_z;

	ssize_t irq_handled;
	ssize_t irq_per_sec;
	struct timer_list print_timer;

	struct work_struct work;
};
struct mpu6050_device *mpu6050_dev;

static int major;
static int minor;
static struct device *pdev;

static int is_open;
static const char *read_message = "Please, use IOCTL instead\n";

static void round_buffer_init(struct round_buffer *buffer)
{
	buffer->head = 0;
	buffer->tail = 0;
	memset(buffer->buffer, 0, ROUND_BUFFER_SIZE);
}

static void round_buffer_push(struct round_buffer *buffer, s16 value)
{
	buffer->buffer[buffer->head] = value;

	buffer->head++;
	if (buffer->head == ROUND_BUFFER_SIZE)
		buffer->head = 0;	

	if(buffer->head == buffer->tail)
		buffer->tail++;
	if (buffer->tail == ROUND_BUFFER_SIZE)
		buffer->tail = 0;
}

static s16 round_buffer_get_sma(struct round_buffer *buffer)
{
	ssize_t sum = 0;
	ssize_t i;

	for(i = 0; i < ROUND_BUFFER_SIZE; i++) {
		sum += buffer->buffer[i];
	}
	sum = sum / ROUND_BUFFER_SIZE;

	return sum;
}

static void print_timer_fn(struct timer_list *timer)
{
	struct mpu6050_device *mpu6050 = container_of(timer, struct mpu6050_device, print_timer);
	static ssize_t last_irq_handled;

	mpu6050->irq_per_sec = mpu6050->irq_handled - last_irq_handled;
	last_irq_handled = mpu6050->irq_handled;

	mod_timer(&mpu6050->print_timer, jiffies + msecs_to_jiffies(1000));
}

static int mpu6050_read_all_values(struct mpu6050_device *mpu6050_device)
{
	int ret;
	struct i2c_client *client = mpu6050_device->client;
	u8 buffer[14];

	ret = i2c_smbus_read_i2c_block_data(client, REG_ACCEL_XOUT_H, sizeof(buffer), &buffer[0]);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	mpu6050_device->accel_raw_x = (s16)((s16)buffer[0]<<8 | buffer[1]);
	mpu6050_device->accel_raw_y = (s16)((s16)buffer[2]<<8 | buffer[3]);
	mpu6050_device->accel_raw_z = (s16)((s16)buffer[4]<<8 | buffer[5]);
	mpu6050_device->temp_raw 	= (s16)((s16)buffer[6]<<8 | buffer[7]);
	mpu6050_device->gyro_raw_x 	= (s16)((s16)buffer[8]<<8 | buffer[9]);
	mpu6050_device->gyro_raw_y 	= (s16)((s16)buffer[10]<<8 | buffer[11]);
	mpu6050_device->gyro_raw_z 	= (s16)((s16)buffer[12]<<8 | buffer[13]);
	mpu6050_device->is_new_data = 1;
	return 0;
}

static void mpu6050_work_function(struct work_struct *wrk)
{
	struct mpu6050_device *mpu6050_dev = container_of(wrk, struct mpu6050_device, work);

	mpu6050_read_all_values(mpu6050_dev);

	round_buffer_push(&mpu6050_dev->rbuf_x, mpu6050_dev->accel_raw_x);
	round_buffer_push(&mpu6050_dev->rbuf_y, mpu6050_dev->accel_raw_y);
	round_buffer_push(&mpu6050_dev->rbuf_z, mpu6050_dev->accel_raw_z);
}

static irqreturn_t mpu6050_irq_handler(int irq, void *dev)
{
	struct mpu6050_device *mpu6050 = dev;

	mpu6050->irq_handled++;
	
	schedule_work(&mpu6050->work);

	return IRQ_HANDLED;
}


static int mpu6050_init(struct mpu6050_device *mpu6050_device)
{
	int ret;
	int n = 0;
	struct i2c_client *client = mpu6050_device->client;

	//Reset device
	ret = i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0x80);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	//Wait device after reset
	while(ret != 0x40) {
		ret = i2c_smbus_read_byte_data(client, REG_PWR_MGMT_1);
		if (IS_ERR_VALUE(ret)) {
			dev_err(&client->dev, "i2c_smbus_read_byte_data() failed with error: %d\n", ret);
			return ret;
		}
		//Limit to 10 attempts
		if(n > 10) {
			return -ETIME;
		}
		n++;
	}

	//Disable Sleep mode
	ret = i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0x00);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	//Set DLPF = mode 0
	ret = i2c_smbus_write_byte_data(client, REG_CONFIG, 0x00);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	//Set outup sample rate to 8Khz/(1+79)=100Hz
	ret = i2c_smbus_write_byte_data(client, REG_SMPRT_DIV, 79);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	//Configure IRQ pin to Active Low, push-pull, Hold until any read
	ret = i2c_smbus_write_byte_data(client, REG_INT_PIN_CFG, 0xB0);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	//Configure interrupt
	ret = i2c_smbus_write_byte_data(client, REG_INT_ENABLE, 0x01);
	if (IS_ERR_VALUE(ret)) {
		return ret;
	}

	return 0;
}

static ssize_t accel_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->accel_raw_x);
}

static ssize_t accel_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->accel_raw_y);
}

static ssize_t accel_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->accel_raw_z);
}

static ssize_t gyro_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->gyro_raw_x);
}

static ssize_t gyro_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->gyro_raw_y);
}

static ssize_t gyro_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->gyro_raw_z);
}

static ssize_t temp_show(struct class *class, struct class_attribute *attr, char *buf)
{
	int tempH, tempL;

	tempL = mpu6050_dev->temp_raw * 2941 + 36530000;
	tempH = (int)tempL/1000000;
	tempL = (tempL - (tempH * 1000000))/10000;

	return sprintf(buf, "%d.%02d\n", tempH, tempL);
}

static ssize_t irq_handled_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->irq_handled);
}

static ssize_t irq_speed_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", mpu6050_dev->irq_per_sec);
}

static ssize_t sma_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", round_buffer_get_sma(&mpu6050_dev->rbuf_x));
}

static ssize_t sma_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", round_buffer_get_sma(&mpu6050_dev->rbuf_y));
}

static ssize_t sma_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", round_buffer_get_sma(&mpu6050_dev->rbuf_z));
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temp);
CLASS_ATTR_RO(irq_handled);
CLASS_ATTR_RO(irq_speed);
CLASS_ATTR_RO(sma_x);
CLASS_ATTR_RO(sma_y);
CLASS_ATTR_RO(sma_z);

static struct attribute *mpu6050_class_attrs[] = {
	&class_attr_accel_x.attr,
	&class_attr_accel_y.attr,
	&class_attr_accel_z.attr,
	&class_attr_gyro_x.attr,
	&class_attr_gyro_y.attr,
	&class_attr_gyro_z.attr,
	&class_attr_temp.attr,
	&class_attr_irq_handled.attr,
	&class_attr_irq_speed.attr,
	&class_attr_sma_x.attr,
	&class_attr_sma_y.attr,
	&class_attr_sma_z.attr,
	NULL,
};
ATTRIBUTE_GROUPS(mpu6050_class);

static struct class mpu6050_module_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = mpu6050_class_groups,
};


static int mpu6050_open(struct inode *inodep, struct file *filep)
{
	if(is_open) {
		pr_debug("%s: Device is already opened");
		return -EBUSY;
	}
	is_open = 1;

	return 0;
}

static int mpu6050_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	return 0;
}

static ssize_t mpu6050_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;
	static int flag = 0;

	ret = copy_to_user(buffer, read_message, strlen(read_message));
	if (ret) {
		return -EFAULT;
	}

	//just for cat
	if(flag) {
		flag = 0;
		return strlen(read_message);
	} else {
		flag = 1;
		return 0;
	}
}

static ssize_t mpu6050_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	return -EFAULT;
}

static long int mpu6050_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	return 0;
}


static struct file_operations fops =
{
	.open = mpu6050_open,
	.release = mpu6050_release,
	.read = mpu6050_read,
	.write = mpu6050_write,
	.unlocked_ioctl = mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	dev_info(&client->dev, "%s\n", __func__);

	ret = i2c_smbus_read_byte_data(client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev, "i2c_smbus_read_byte_data() failed with error: %d\n", ret);
		return ret;
	}

	//Check whether the device is MPU6050 chip 
	if (ret != 0x68) {
		dev_err(&client->dev, "Device detected, but it is not a MPU6050 chip\n");
		return -ENODEV;
	}

	mpu6050_dev = kzalloc(sizeof(struct mpu6050_device), GFP_KERNEL);
	if(mpu6050_dev == NULL) {
		dev_err(&client->dev, "Can't allocate device\n");
		return -ENOMEM;
	}

	ret = class_register(&mpu6050_module_class);
	if (ret < 0) {
		dev_err(&client->dev, "Can't register a class in sysfs\n");
		ret = -ENOMEM;
		goto fail2;
	}

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0) {
		dev_err(&client->dev, "Can't register char device\n");
		ret = -ENOMEM;
		goto fail2;
	}

	pdev = device_create(&mpu6050_module_class, &client->dev, MKDEV(major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(pdev)) {
		unregister_chrdev(major, DEVICE_NAME);
		dev_err(&client->dev, "Can't create device\n");
		ret = -ENOMEM;
		goto fail2;
	}

	i2c_set_clientdata(client, mpu6050_dev);
	mpu6050_dev->client = client;

	mpu6050_dev->irq_num = of_irq_get_byname(client->dev.of_node, "MPU6050_INT");
	if( !(mpu6050_dev->irq_num > 0)) {
		dev_err(&client->dev, "Fail to get MPU6050 IRQ\n");
		goto fail1;
	}

	round_buffer_init(&mpu6050_dev->rbuf_x);
	round_buffer_init(&mpu6050_dev->rbuf_y);
	round_buffer_init(&mpu6050_dev->rbuf_z);

	INIT_WORK(&mpu6050_dev->work, mpu6050_work_function);

	ret = request_irq(mpu6050_dev->irq_num, mpu6050_irq_handler, IRQF_TRIGGER_FALLING, "MPU6050_INT", mpu6050_dev);
	if (IS_ERR_VALUE(ret)) {
        dev_err(&client->dev, "Fail to request MPU6050 IRQ\n");
		goto fail1;
        }
	dev_info(&client->dev, "Requested MPU6050 IRQ - %d\n", mpu6050_dev->irq_num);

	mpu6050_init(mpu6050_dev);

	timer_setup(&mpu6050_dev->print_timer, print_timer_fn, 0);
	mod_timer(&mpu6050_dev->print_timer, jiffies + msecs_to_jiffies(1000));

	dev_info(&client->dev, "module loaded\n");
	return 0;

fail1:
	class_unregister(&mpu6050_module_class);
fail2:
	kfree(mpu6050_dev);
	return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
	struct mpu6050_device *mpu6050 = i2c_get_clientdata(client);
	dev_info(&client->dev, "%s\n", __func__);
	
	del_timer(&mpu6050->print_timer);

	synchronize_irq(mpu6050->irq_num);
	free_irq(mpu6050->irq_num, mpu6050);

	flush_work(&mpu6050->work);
	
	kfree(mpu6050);

	device_destroy(&mpu6050_module_class, MKDEV(major, 0));
	class_unregister(&mpu6050_module_class);
	unregister_chrdev(major, DEVICE_NAME);

	dev_info(&client->dev, "module unloaded\n");
	return 0;
}

static const struct of_device_id mpu6050_of_match[] = {
	{ .compatible = "mpu6050"},
	{ }
};
MODULE_DEVICE_TABLE(of, mpu6050_of_match);

static const struct i2c_device_id mpu6050_id[] =
    {
    	{ "mpu6050", 0 },
    	{ }
    };
MODULE_DEVICE_TABLE(i2c, mpu6050_id);

static struct i2c_driver mpu6050_driver = {
	.driver.name	= "mpu6050",
	.driver.owner = THIS_MODULE,
	.probe		= mpu6050_probe,
	.remove		= mpu6050_remove,
	.id_table	= mpu6050_id,
};
module_i2c_driver(mpu6050_driver);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Alternative mpu6050 driver");
MODULE_VERSION("0.1");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
