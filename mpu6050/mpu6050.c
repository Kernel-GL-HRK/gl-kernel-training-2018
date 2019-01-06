#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/timer.h>
#include <linux/i2c-dev.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#include "mpu6050-regs.h"

#define FILT_ORDER 4

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values_raw[3][FILT_ORDER];
	int gyro_values_raw[3][FILT_ORDER];

	int raw_store_idx;

	int accel_values[3];
	int gyro_values[3];
	int temperature;
};

static struct gpio_desc *irq_line;

static int irq_num;

static struct mpu6050_data g_mpu6050_data;

static struct workqueue_struct *reader_wq;

static int mpu6050_read_data(void)
{
	int temp;
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	int store_pos = g_mpu6050_data.raw_store_idx;
	/* accel */
	g_mpu6050_data.accel_values_raw[0][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
	g_mpu6050_data.accel_values_raw[1][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
	g_mpu6050_data.accel_values_raw[2][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));
	/* gyro */
	g_mpu6050_data.gyro_values_raw[0][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
	g_mpu6050_data.gyro_values_raw[1][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
	g_mpu6050_data.gyro_values_raw[2][store_pos]
			= (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
	/* Temperature in degrees C =
	 * (TEMP_OUT Register Value  as a signed quantity)/340 + 36.53
	 */
	temp = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
	g_mpu6050_data.temperature = (temp + 12420 + 170) / 340;

	return 0;
}

static void mpu6050_filter_accel_gyro_data(void)
{
	int axis;
	int i;

	for (axis = 0; axis < 3; axis++) {
		int sum = 0; 
		for (i = 0; i < FILT_ORDER; i++)
			sum += g_mpu6050_data.accel_values_raw[axis][i];

		g_mpu6050_data.accel_values[axis] = sum/FILT_ORDER;
	}

	for (axis = 0; axis < 3; axis++) {
		int sum = 0; 
		for (i = 0; i < FILT_ORDER; i++)
			sum += g_mpu6050_data.gyro_values_raw[axis][i];

		g_mpu6050_data.gyro_values[axis] = sum/FILT_ORDER;
	}
}

static void mpu_irq_work(struct work_struct *work)
{
	mpu6050_read_data();

	g_mpu6050_data.raw_store_idx++;
	g_mpu6050_data.raw_store_idx = g_mpu6050_data.raw_store_idx % FILT_ORDER;

	mpu6050_filter_accel_gyro_data();
}

DECLARE_WORK(read_filter_work, mpu_irq_work);

static irqreturn_t mpu_irq(int irq, void *dev_id)
{
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (!queue_work(reader_wq, &read_filter_work)) {
		dev_err(&drv_client->dev, "Unable to queue work: already on queue\n");
	}

	return IRQ_HANDLED;
}

static int mpu6050_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	int ret;

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	/* Read who_am_i register */
	ret = i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed with error: %d\n",
			ret);
		return ret;
	}
	if (ret != MPU6050_WHO_AM_I) {
		dev_err(&drv_client->dev,
			"wrong i2c device found: expected 0x%X, found 0x%X\n",
			MPU6050_WHO_AM_I, ret);
		return -1;
	}
	dev_info(&drv_client->dev,
		"i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n",
		ret);

	irq_line = gpiod_get(&drv_client->dev, "int", GPIOD_IN);

	gpiod_direction_input(irq_line);

	if (IS_ERR(irq_line)) {
		dev_err(&drv_client->dev, "unable to retreive IRQ GPIO descriptor\n");
		return -1;
	}

	irq_num = gpiod_to_irq(irq_line);

	if (irq_num < 0) {
		dev_err(&drv_client->dev, "unable to receive IRQ for given GPIO\n");
		return -1;
	}

	if (request_irq(irq_num, mpu_irq, IRQF_TRIGGER_FALLING, "mpu6050", &drv_client)) {
		dev_err(&drv_client->dev, "request IRQ failed\n");
		return -1;
	}

	dev_info(&drv_client->dev, "Registered IRQ: %d\n", irq_num);

	g_mpu6050_data.drv_client = drv_client;

	/* Setup the device */
	/* No error handling here! */
	i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_FIFO_EN, 0);

	i2c_smbus_write_byte_data(drv_client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0);

	i2c_smbus_write_byte_data(drv_client, REG_CONFIG, CONF_DLPF_CFG_4);
	i2c_smbus_write_byte_data(drv_client, REG_GYRO_CONFIG, GCON_FCHOICE_DLPF_EN);
	i2c_smbus_write_byte_data(drv_client, REG_SAMPLERATE_DIV, 7);

	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG,
								INT_PIN_CFG_ACTIVE_LOW |
								INT_PIN_CFG_STATUS_READ_CLEAR);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, INT_EN_DATA_RDY);

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	free_irq(irq_num, &drv_client);

	gpiod_put(irq_line);

	g_mpu6050_data.drv_client = 0;

	dev_info(&drv_client->dev, "i2c driver removed\n");

	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl_mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	return strlen(buf);
}

static ssize_t temperature_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.temperature);
	return strlen(buf);
}

CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);
CLASS_ATTR_RO(temperature);

static struct class *attr_class;

static int mpu6050_init(void)
{
	int ret;

	/* Create i2c driver */
	ret = i2c_add_driver(&mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: i2c driver created\n");

	/* Create class */
	attr_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		pr_err("mpu6050: failed to create sysfs class: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: sysfs class created\n");

	/* Create accel_x */
	ret = class_create_file(attr_class, &class_attr_accel_x);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_x: %d\n", ret);
		return ret;
	}
	/* Create accel_y */
	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_y: %d\n", ret);
		return ret;
	}
	/* Create accel_z */
	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_z: %d\n", ret);
		return ret;
	}
	/* Create gyro_x */
	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_x: %d\n", ret);
		return ret;
	}
	/* Create gyro_y */
	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_y: %d\n", ret);
		return ret;
	}
	/* Create gyro_z */
	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_z: %d\n", ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(attr_class, &class_attr_temperature);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute temperature: %d\n", ret);
		return ret;
	}

	reader_wq = alloc_workqueue("MPU READER", WQ_HIGHPRI, 2);

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	if (attr_class) {
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_z);
		class_remove_file(attr_class, &class_attr_temperature);
		pr_info("mpu6050: sysfs class attributes removed\n");

		class_destroy(attr_class);
		pr_info("mpu6050: sysfs class destroyed\n");
	}

	cancel_work_sync(&read_filter_work);
	destroy_workqueue(reader_wq);

	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted\n");

	pr_info("mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
