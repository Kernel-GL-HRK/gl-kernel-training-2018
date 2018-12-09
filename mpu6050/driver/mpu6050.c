// SPDX-License-Identifier: GPL-2.0
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
#include <linux/interrupt.h>

#include <mpu6050-regs.h>

#define MODULE_TAG			"mpu6050"
#define CLASS_NAME			"mpu6050"

static unsigned long int mode;
module_param(mode, ulong, 0664);
MODULE_PARM_DESC(mode, "mode");

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
};
struct mpu6050_device *mpu6050_dev;


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

static irqreturn_t mpu6050_irq_handler(int irq, void *dev)
{
	struct mpu6050_device *mpu6050_dev = dev;

	mpu6050_read_all_values(mpu6050_dev);
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
	ret = i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0x01);
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

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temp);

static struct attribute *mpu6050_class_attrs[] = {
	&class_attr_accel_x.attr,
	&class_attr_accel_y.attr,
	&class_attr_accel_z.attr,
	&class_attr_gyro_x.attr,
	&class_attr_gyro_y.attr,
	&class_attr_gyro_z.attr,
	&class_attr_temp.attr,
	NULL,
};
ATTRIBUTE_GROUPS(mpu6050_class);

static struct class mpu6050_module_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = mpu6050_class_groups,
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

	mpu6050_dev->client = client;
	mpu6050_dev->irq_gpio_num = 6;
	mpu6050_dev->irq_num = gpio_to_irq(mpu6050_dev->irq_gpio_num);

    ret = devm_gpio_request_one(&client->dev, mpu6050_dev->irq_gpio_num, 
											GPIOF_IN, "MPU6050 IRQ PIN");
    if (IS_ERR_VALUE(ret)) {
        dev_err(&client->dev, "Fail to get MPU6050 IRQ PIN\n");
		goto fail1;
        }

    dev_info(&client->dev, "Got MPU6050 IRQ PIN - %d\n", mpu6050_dev->irq_gpio_num);

	ret = devm_request_threaded_irq(&client->dev, mpu6050_dev->irq_num, NULL, 
									mpu6050_irq_handler, IRQF_ONESHOT | IRQF_TRIGGER_FALLING, 
									"MPU6050 IRQ", mpu6050_dev);
	if (IS_ERR_VALUE(ret)) {
        dev_err(&client->dev, "Fail to get MPU6050 IRQ PIN\n");
		goto fail1;
        }

	dev_info(&client->dev, "Got MPU6050 IRQ - %d\n", mpu6050_dev->irq_num);

	mpu6050_init(mpu6050_dev);

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
	dev_info(&client->dev, "%s\n", __func__);

	devm_free_irq(&client->dev, mpu6050_dev->irq_num, mpu6050_dev);

	kfree(mpu6050_dev);
	class_unregister(&mpu6050_module_class);

	dev_info(&client->dev, "module unloaded\n");
	return 0;
}

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
//module_i2c_driver(mpu6050_driver);


static int mpu6050_module_init(void)
{
	int ret;

	ret = i2c_add_driver(&mpu6050_driver);

	return 0;
}

static void mpu6050_module_exit(void)
{
	i2c_del_driver(&mpu6050_driver);
}

module_init(mpu6050_module_init);
module_exit(mpu6050_module_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Alternative mpu6050 driver");
MODULE_VERSION("0.1");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
