/* drivers/misc/as9888.c - as9888 driver
 *
 * @version 1.1
 * @date 2011/12/14
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#define DEBUG
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/as9888.h>

#define AS9888_DEBUG_IF	1
#define AS9888_DEBUG_DATA	1

#if AS9888_DEBUG_DATA
#define DMT_DATA(dev, ...) \
	dev_dbg((dev), ##__VA_ARGS__)
#else
#define DMT_DATA(dev, format, ...)
#endif

/* Register address to index */
#define BDIDX(addr)	((addr)-1)

/* Wait timeout in millisecond */
#define AS9888_DRDY_TIMEOUT	200

struct as9888_data {
	struct i2c_client	*i2c;
	struct input_dev	*input;
	struct device		*class_dev;
	struct class		*compass;
	/*struct delayed_work	work;*/

	wait_queue_head_t	drdy_wq;
	wait_queue_head_t	open_wq;

	struct mutex sensor_mutex;
	int8_t	sense_data[SENSOR_DATA_SIZE];

	struct mutex	val_mutex;
	uint32_t	enable_flag;
	int64_t		delay[DMT_NUM_SENSORS];

	atomic_t	active;
	atomic_t	ms_mode;
	atomic_t	drdy;
	atomic_t	suspend;
	atomic_t	addr;

	char layout;
	int	irq;
	int	rstn;
};

static struct as9888_data *s_akm;



/***** I2C I/O function ***********************************************/
static int as9888_i2c_rxdata(
	struct i2c_client *i2c,
	unsigned char *rxData,
	int length)
{
	struct i2c_msg msgs[] = {
	{
		.addr = i2c->addr,
		.flags = 0,
		.len = 1,
		.buf = rxData,
	},
	{
		.addr = i2c->addr,
		.flags = I2C_M_RD,
		.len = length,
		.buf = rxData,
	}, };
#if AS9888_DEBUG_DATA
	unsigned char addr = rxData[0];
#endif
	if (i2c_transfer(i2c->adapter, msgs, 2) < 0) {
		dev_err(&i2c->dev, "%s: transfer failed.", __func__);
		return -EIO;
	}

	DMT_DATA(&i2c->dev, "RxData: len=%02x, addr=%02x, data=%02x",
		length, addr, rxData[0]);
	return 0;
}

static int as9888_i2c_txdata(
	struct i2c_client *i2c,
	unsigned char *txData,
	int length)
{
	struct i2c_msg msg[] = {
	{
		.addr = i2c->addr,
		.flags = 0,
		.len = length,
		.buf = txData,
	}, };

	if (i2c_transfer(i2c->adapter, msg, 1) < 0) {
		dev_err(&i2c->dev, "%s: transfer failed.", __func__);
		return -EIO;
	}

	DMT_DATA(&i2c->dev, "TxData: len=%02x, addr=%02x data=%02x",
		length, txData[0], txData[1]);
	return 0;
}

static int as9888_i2c_check_device(
	struct i2c_client *client)
{
	unsigned char buffer[2];
	int err;

	/* Set measure mode */
	buffer[0] = AS9888_REG_WIA;
	err = as9888_i2c_rxdata(client, buffer, 1);
	if (err < 0) {
		dev_err(&client->dev,
			"%s: Can not read WIA.", __func__);
		return err;
	}

	/* Check read data */
	if (buffer[0] != AS9888_WIA_VALUE) {
		dev_err(&client->dev,
			"%s: The device is not AS9888.", __func__);
		return -ENXIO;
	}

	return err;
}

/***** akm miscdevice functions *************************************/
static int AKECS_Open(struct inode *inode, struct file *file);

static int AKECS_Release(struct inode *inode, struct file *file);

static long AKECS_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations AKECS_fops = {
	.owner = THIS_MODULE,
	.open = AKECS_Open,
	.release = AKECS_Release,
	.unlocked_ioctl = AKECS_ioctl,
};

static struct miscdevice as9888_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "as9888_dev",
	.fops = &AKECS_fops,
};

static int AKECS_Set_MS(
	struct as9888_data *akm,
	unsigned char mode)
{
	unsigned char buffer[2];
	int err;

	/* Busy check */
	if (atomic_cmpxchg(&akm->ms_mode, 0, mode) != 0) {
		dev_err(&akm->i2c->dev, "%s: device is busy.", __func__);
		return -EBUSY;
	}


	/* Set measure mode */
	buffer[0] = AS9888_REG_MS;
	buffer[1] = mode;
	err = as9888_i2c_txdata(akm->i2c, buffer, 2);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s: Can not set MS register.", __func__);
		atomic_set(&akm->ms_mode, 0);
	} else {
		dev_dbg(&akm->i2c->dev, "Mode is set to (%d).", mode);
	}

	return err;
}

static int AKECS_Set_PowerDown(	struct as9888_data *akm)
{
	unsigned char buffer[2];
	int err;

	/* Set MS register */
	buffer[0] = AS9888_REG_MS;
	buffer[1] = AS9888_MODE_POWERDOWN;
	err = as9888_i2c_txdata(akm->i2c, buffer, 2);
	if (err < 0) 
	{
		dev_err(&akm->i2c->dev,
			"%s: Can not set to measurement mode.", __func__);
	} 
	else {
		dev_dbg(&akm->i2c->dev, "Powerdown mode is set.");
	}

	/* Set to initial status. */
	atomic_set(&akm->ms_mode, 0);

	return err;
}

static int AKECS_Reset( struct as9888_data *akm, int hard)
{
	unsigned char buffer[2];
	int err = 0;

	if (hard != 0) 
	{
		gpio_set_value(akm->rstn, 0);
		udelay(5);
		gpio_set_value(akm->rstn, 1);
	} 
	else {
		/* Set measure mode */
		buffer[0] = AS9888_REG_MS;
		buffer[1] = AS9888_MODE_SOFT_RESET;
		err = as9888_i2c_txdata(akm->i2c, buffer, 2);
		if (err < 0) 
		{
			dev_err(&akm->i2c->dev,
				"%s: Can not set SRST bit.", __func__);
		} 
		else {
			dev_dbg(&akm->i2c->dev, "Soft reset is done.");
		}
	}

	/* Set to initial status. */
	atomic_set(&akm->ms_mode, 0);
	atomic_set(&akm->drdy, 0);
	mutex_lock(&akm->sensor_mutex);
	akm->sense_data[0] = 0;
	akm->sense_data[1] = 0;
	mutex_unlock(&akm->sensor_mutex);

	/* Device will be accessible 100 us after */
	udelay(100);

	return err;
}

static int AKECS_SetMode( struct as9888_data *akm, unsigned char mode)
{
	int err;
	unsigned char masked = mode & 0x0F;

	switch (masked) {
	case AS9888_MODE_POWERDOWN:
		err = AKECS_Set_PowerDown(akm);
		break;
	case AS9888_MODE_SNG_MEASURE:
	case AS9888_MODE_CNT_MEASURE:
	case AS9888_MODE_EEPROM:
	case AS9888_MODE_SELF_TEST:
		err = AKECS_Set_MS(akm, masked);
		break;
	default:
		dev_err(&akm->i2c->dev,
			"%s: Unknown mode(%d).", __func__, mode);
		return -EINVAL;
	}

	/* Set flag */
	atomic_set(&akm->drdy, 0);
	mutex_lock(&akm->sensor_mutex);
	akm->sense_data[0] = 0;
	akm->sense_data[1] = 0;
	mutex_unlock(&akm->sensor_mutex);

	/* wait at least 100us after changing mode */
	udelay(100);

	return err;
}

/* This function will block a process until the latest measurement
 * data is available.
 */
static int AKECS_GetData(
	struct as9888_data *akm,
	char *rbuf,
	int size)
{
	int err;
	err = wait_event_interruptible_timeout(
			akm->drdy_wq,
			atomic_read(&akm->drdy),
			AS9888_DRDY_TIMEOUT);

	if (err < 0) {
		dev_dbg(&akm->i2c->dev,
			"%s: wait_event failed (%d).", __func__, err);
		return err;
	}
	if (!atomic_read(&akm->drdy)) {
		dev_dbg(&akm->i2c->dev,
			"%s: DRDY is not set.", __func__);
		return -EBUSY;
	}

	mutex_lock(&akm->sensor_mutex);
	memcpy(rbuf, akm->sense_data, size);
	atomic_set(&akm->drdy, 0);
	/* Reset INT flags */
	akm->sense_data[0] = 0;
	akm->sense_data[1] = 0;
	mutex_unlock(&akm->sensor_mutex);

	return err;
}

/* This function gets temperature
 */
static int AKECS_GetTemperature(
	struct as9888_data *akm,
	char *temperature)
{
	unsigned char buffer[10];
	unsigned char cur_set[5];
	int err;

	/* Busy check */
	if (atomic_cmpxchg(&akm->ms_mode, 0, AS9888_MODE_SNG_MEASURE) != 0) {
		dev_err(&akm->i2c->dev, "%s: device is busy.", __func__);
		return -EBUSY;
	}

	/* Reserve current setting */
	cur_set[0] = cur_set[1] = AS9888_REG_INT1EN;
	err = as9888_i2c_rxdata(akm->i2c, &cur_set[1], 4);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s: Can not get current settings.", __func__);
		goto GET_TEMPERATURE_FAIL;
	}

	dev_dbg(&akm->input->dev, "Current settings[Hex]: %02X,%02X,%02X,%02X",
		cur_set[1], cur_set[2], cur_set[3], cur_set[4]);

	/* Set new setting */
	buffer[0] = AS9888_REG_INT1EN;
	buffer[1] = 0x01;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = 0x04;
	err = as9888_i2c_txdata(akm->i2c, buffer, 5);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s: Can not set new settings.", __func__);
		goto GET_TEMPERATURE_FAIL;
	}

	/* Set flag */
	atomic_set(&akm->drdy, 0);

	/* Set measure mode */
	buffer[0] = AS9888_REG_MS;
	buffer[1] = AS9888_MODE_SNG_MEASURE;
	err = as9888_i2c_txdata(akm->i2c, buffer, 2);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s: Can not set MS register.", __func__);
		goto GET_TEMPERATURE_FAIL;
	}

	/* wait drdy */
	err = AKECS_GetData(akm, buffer, 10);
	if (err < 0) {
		goto GET_TEMPERATURE_FAIL;
	}

	/* Restore settings */
	err = as9888_i2c_txdata(akm->i2c, cur_set, 5);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s: Can not restore settings.", __func__);
		goto GET_TEMPERATURE_FAIL;
	}

	/* copy temperature value */
	*temperature = buffer[9];

	return err;

GET_TEMPERATURE_FAIL:
	atomic_set(&akm->ms_mode, 0);
	return err;
}

static void AKECS_SetYPR(
	struct as9888_data *akm,
	int *rbuf)
{
	uint32_t ready;
	DMT_DATA(&akm->i2c->dev, "AS9888 %s: flag =0x%X", __func__,
		rbuf[0]);
	DMT_DATA(&akm->input->dev, "  Acceleration[LSB]: %6d,%6d,%6d stat=%d",
		rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
	DMT_DATA(&akm->input->dev, "  Geomagnetism[LSB]: %6d,%6d,%6d stat=%d",
		rbuf[5], rbuf[6], rbuf[7], rbuf[8]);
	DMT_DATA(&akm->input->dev, "  Orientation[YPR] : %6d,%6d,%6d",
		rbuf[9], rbuf[10], rbuf[11]);

	/* No events are reported */
	if (!rbuf[0]) {
		dev_err(&akm->i2c->dev, "Don't waste a time.");
		return;
	}

	mutex_lock(&akm->val_mutex);
	ready = (akm->enable_flag & (uint32_t)rbuf[0]);
	mutex_unlock(&akm->val_mutex);

	/* Report acceleration sensor information */
	if (ready & ACC_DATA_READY) {
		input_report_abs(akm->input, ABS_X, rbuf[1]);
		input_report_abs(akm->input, ABS_Y, rbuf[2]);
		input_report_abs(akm->input, ABS_Z, rbuf[3]);
		input_report_abs(akm->input, ABS_THROTTLE, rbuf[4]);
	}
	/* Report magnetic vector information */
	if (ready & MAG_DATA_READY) {
		input_report_abs(akm->input, ABS_RX, rbuf[5]);
		input_report_abs(akm->input, ABS_RY, rbuf[6]);
		input_report_abs(akm->input, ABS_RZ, rbuf[7]);
		input_report_abs(akm->input, ABS_RUDDER, rbuf[8]);
	}
	/* Report orientation sensor information */
	if (ready & ORI_DATA_READY) {
		input_report_abs(akm->input, ABS_HAT0X, rbuf[9]);
		input_report_abs(akm->input, ABS_HAT0Y, rbuf[10]);
		input_report_abs(akm->input, ABS_HAT1X, rbuf[11]);
		input_report_abs(akm->input, ABS_HAT1Y, rbuf[4]);
	}

	input_sync(akm->input);
}

static int AKECS_GetOpenStatus(
	struct as9888_data *akm)
{
	return wait_event_interruptible(
			akm->open_wq, (atomic_read(&akm->active) != 0));
}

static int AKECS_GetCloseStatus(
	struct as9888_data *akm)
{
	return wait_event_interruptible(
			akm->open_wq, (atomic_read(&akm->active) <= 0));
}

static int AKECS_Open(struct inode *inode, struct file *file)
{
	file->private_data = s_akm;
	return nonseekable_open(inode, file);
}

static int AKECS_Release(struct inode *inode, struct file *file)
{
	return 0;
}

static long
AKECS_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct as9888_data *akm = file->private_data;

	/* NOTE: In this function the size of "char" should be 1-byte. */
	char i2c_buf[RWBUF_SIZE];		/* for READ/WRITE */
	int8_t sensor_buf[SENSOR_DATA_SIZE];/* for GETDATA */
	int32_t ypr_buf[YPR_DATA_SIZE];	/* for SET_YPR */
	int64_t delay[DMT_NUM_SENSORS];	/* for GET_DELAY */
	char mode;			/* for SET_MODE*/
	char layout;		/* for GET_LAYOUT */
	char temperature;	/* for GET_TEMPERATURE */
	int status;			/* for OPEN/CLOSE_STATUS */
	int ret = -1;		/* Return value. */

	switch (cmd) {
	case ECS_IOCTL_READ:
	case ECS_IOCTL_WRITE:
		if (argp == NULL) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&i2c_buf, argp, sizeof(i2c_buf))) {
			dev_err(&akm->i2c->dev, "copy_from_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_SET_MODE:
		if (argp == NULL) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&mode, argp, sizeof(mode))) {
			dev_err(&akm->i2c->dev, "copy_from_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_SET_YPR:
		if (argp == NULL) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&ypr_buf, argp, sizeof(ypr_buf))) {
			dev_err(&akm->i2c->dev, "copy_from_user failed.");
			return -EFAULT;
		}
	case ECS_IOCTL_GETDATA:
	case ECS_IOCTL_GET_OPEN_STATUS:
	case ECS_IOCTL_GET_CLOSE_STATUS:
	case ECS_IOCTL_GET_DELAY:
	case ECS_IOCTL_GET_LAYOUT:
	case ECS_IOCTL_GET_TEMPERATURE:
		/* Just check buffer pointer */
		if (argp == NULL) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		break;
		break;
	default:
		break;
	}

	switch (cmd) {
	case ECS_IOCTL_READ:
		DMT_DATA(&akm->i2c->dev, "IOCTL_READ called.");
		if ((i2c_buf[0] < 1) || (i2c_buf[0] > (RWBUF_SIZE-1))) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		ret = as9888_i2c_rxdata(akm->i2c, &i2c_buf[1], i2c_buf[0]);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_WRITE:
		DMT_DATA(&akm->i2c->dev, "IOCTL_WRITE called.");
		if ((i2c_buf[0] < 2) || (i2c_buf[0] > (RWBUF_SIZE-1))) {
			dev_err(&akm->i2c->dev, "invalid argument.");
			return -EINVAL;
		}
		ret = as9888_i2c_txdata(akm->i2c, &i2c_buf[1], i2c_buf[0]);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_SET_MODE:
		DMT_DATA(&akm->i2c->dev, "IOCTL_SET_MODE called.");
		ret = AKECS_SetMode(akm, mode);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_GETDATA:
		DMT_DATA(&akm->i2c->dev, "IOCTL_GETDATA called.");
		ret = AKECS_GetData(akm, sensor_buf, SENSOR_DATA_SIZE);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_SET_YPR:
		DMT_DATA(&akm->i2c->dev, "IOCTL_SET_YPR called.");
		AKECS_SetYPR(akm, ypr_buf);
		break;
	case ECS_IOCTL_GET_OPEN_STATUS:
		DMT_DATA(&akm->i2c->dev, "IOCTL_GET_OPEN_STATUS called.");
		ret = AKECS_GetOpenStatus(akm);
		if (ret < 0) {
			dev_err(&akm->i2c->dev,
				"Get Open returns error (%d).", ret);
		}
		break;
	case ECS_IOCTL_GET_CLOSE_STATUS:
		DMT_DATA(&akm->i2c->dev, "IOCTL_GET_CLOSE_STATUS called.");
		ret = AKECS_GetCloseStatus(akm);
		if (ret < 0) {
			dev_err(&akm->i2c->dev,
				"Get Close returns error (%d).", ret);
		}
		break;
	case ECS_IOCTL_GET_DELAY:
		DMT_DATA(&akm->i2c->dev, "IOCTL_GET_DELAY called.");
		mutex_lock(&akm->val_mutex);
		delay[0] = akm->delay[0];
		delay[1] = akm->delay[1];
		delay[2] = akm->delay[2];
		mutex_unlock(&akm->val_mutex);
		break;
	case ECS_IOCTL_GET_LAYOUT:
		DMT_DATA(&akm->i2c->dev, "IOCTL_GET_LAYOUT called.");
		layout = akm->layout;
		break;
	case ECS_IOCTL_RESET:
		ret = AKECS_Reset(akm, akm->rstn);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_GET_TEMPERATURE:
		ret = AKECS_GetTemperature(akm, &temperature);
		if (ret < 0)
			return ret;
		break;
	default:
		return -ENOTTY;
	}

	switch (cmd) {
	case ECS_IOCTL_READ:
		if (copy_to_user(argp, &i2c_buf, i2c_buf[0]+1)) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GETDATA:
		if (copy_to_user(argp, &sensor_buf, sizeof(sensor_buf))) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_OPEN_STATUS:
	case ECS_IOCTL_GET_CLOSE_STATUS:
		status = atomic_read(&akm->active);
		if (copy_to_user(argp, &status, sizeof(status))) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_DELAY:
		if (copy_to_user(argp, &delay, sizeof(delay))) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_LAYOUT:
		if (copy_to_user(argp, &layout, sizeof(layout))) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_TEMPERATURE:
		if (copy_to_user(argp, &temperature, sizeof(temperature))) {
			dev_err(&akm->i2c->dev, "copy_to_user failed.");
			return -EFAULT;
		}
		break;
	default:
		break;
	}

	return 0;
}

/***** akm sysfs functions ******************************************/
static int create_device_attributes(
	struct device *dev,
	struct device_attribute *attrs)
{
	int i;
	int err = 0;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i) {
		err = device_create_file(dev, &attrs[i]);
		if (0 != err)
			break;
	}

	if (0 != err) {
		for (; i >= 0 ; --i)
			device_remove_file(dev, &attrs[i]);
	}

	return err;
}

static void remove_device_attributes(
	struct device *dev,
	struct device_attribute *attrs)
{
	int i;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i)
		device_remove_file(dev, &attrs[i]);
}

#if 0
static int create_device_binary_attributes(
	struct kobject *kobj,
	struct bin_attribute *attrs)
{
	int i;
	int err = 0;

	err = 0;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i) {
		err = sysfs_create_bin_file(kobj, &attrs[i]);
		if (0 != err)
			break;
	}

	if (0 != err) {
		for (; i >= 0 ; --i)
			sysfs_remove_bin_file(kobj, &attrs[i]);
	}

	return err;
}

static void remove_device_binary_attributes(
	struct kobject *kobj,
	struct bin_attribute *attrs)
{
	int i;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i)
		sysfs_remove_bin_file(kobj, &attrs[i]);
}
#endif

static bool get_value_as_int(char const *buf, size_t size, int *value)
{
	long tmp;

	if (size == 0)
		return false;

	/* maybe text format value */
	if ((buf[0] == '0') && (size > 1)) {
		if ((buf[1] == 'x') || (buf[1] == 'X')) {
			/* hexadecimal format */
			if (0 != strict_strtol(buf, 16, &tmp))
				return false;
		} else {
			/* octal format */
			if (0 != strict_strtol(buf, 8, &tmp))
				return false;
		}
	} else {
		/* decimal format */
		if (0 != strict_strtol(buf, 10, &tmp))
			return false;
	}

	if (tmp > INT_MAX)
		return false;

	*value = tmp;

	return true;
}

static bool get_value_as_int64(char const *buf, size_t size, long long *value)
{
	long long tmp;

	if (size == 0)
		return false;

	/* maybe text format value */
	if ((buf[0] == '0') && (size > 1)) {
		if ((buf[1] == 'x') || (buf[1] == 'X')) {
			/* hexadecimal format */
			if (0 != strict_strtoll(buf, 16, &tmp))
				return false;
		} else {
			/* octal format */
			if (0 != strict_strtoll(buf, 8, &tmp))
				return false;
		}
	} else {
		/* decimal format */
		if (0 != strict_strtoll(buf, 10, &tmp))
			return false;
	}

	if (tmp > LLONG_MAX)
		return false;

	*value = tmp;

	return true;
}

/*********************************************************************
 *
 * SysFS attribute functions
 *
 * directory : /sys/class/compass/as9888/
 * files :
 *  - enable_acc [rw] [t] : enable flag for accelerometer
 *  - enable_mag [rw] [t] : enable flag for magnetometer
 *  - enable_ori [rw] [t] : enable flag for orientation
 *  - delay_acc  [rw] [t] : delay in nanosecond for accelerometer
 *  - delay_mag  [rw] [t] : delay in nanosecond for magnetometer
 *  - delay_ori  [rw] [t] : delay in nanosecond for orientation
 *
 * debug :
 *  - reset      [w]  [t] : Reset
 *  - mode       [w]  [t] : AS9888's mode
 *  - bdata      [r]  [t] : raw data
 *  - e2p        [r]  [t] : EEPROM data
 *  - reg        [rw] [t] : Read register
 *
 * [rw]= read/write
 * [r] = read only
 * [w] = write only
 * [b] = binary format
 * [t] = text format
 */

/***** sysfs enable *************************************************/
static void as9888_sysfs_update_active_status(
	struct as9888_data *akm)
{
	uint32_t en;
	mutex_lock(&akm->val_mutex);
	en = akm->enable_flag;
	mutex_unlock(&akm->val_mutex);

	if (en == 0) {
		if (atomic_cmpxchg(&akm->active, 1, 0) == 1) {
			wake_up(&akm->open_wq);
			dev_dbg(akm->class_dev, "Deactivated");
		}
	} else {
		if (atomic_cmpxchg(&akm->active, 0, 1) == 0) {
			wake_up(&akm->open_wq);
			dev_dbg(akm->class_dev, "Activated");
		}
	}
	dev_dbg(&akm->i2c->dev,
		"Status updated: enable=0x%X, active=%d",
		en, atomic_read(&akm->active));
}

static ssize_t as9888_sysfs_enable_show(
	struct as9888_data *akm, char *buf, int pos)
{
	int flag;

	mutex_lock(&akm->val_mutex);
	flag = ((akm->enable_flag >> pos) & 1);
	mutex_unlock(&akm->val_mutex);

	return sprintf(buf, "%d\n", flag);
}

static ssize_t as9888_sysfs_enable_store(
	struct as9888_data *akm, char const *buf, size_t count, int pos)
{
	int en = 0;

	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (false == get_value_as_int(buf, count, &en))
		return -EINVAL;

	en = en ? 1 : 0;

	mutex_lock(&akm->val_mutex);
	akm->enable_flag &= ~(1<<pos);
	akm->enable_flag |= ((uint32_t)(en))<<pos;
	mutex_unlock(&akm->val_mutex);

	as9888_sysfs_update_active_status(akm);

	return count;
}

/***** Acceleration ***/
static ssize_t as9888_enable_acc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_enable_show(
		dev_get_drvdata(dev), buf, ACC_DATA_FLAG);
}
static ssize_t as9888_enable_acc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, ACC_DATA_FLAG);
}

/***** Magnetic field ***/
static ssize_t as9888_enable_mag_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_enable_show(
		dev_get_drvdata(dev), buf, MAG_DATA_FLAG);
}
static ssize_t as9888_enable_mag_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, MAG_DATA_FLAG);
}

/***** Orientation ***/
static ssize_t as9888_enable_ori_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_enable_show(
		dev_get_drvdata(dev), buf, ORI_DATA_FLAG);
}
static ssize_t as9888_enable_ori_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, ORI_DATA_FLAG);
}

/***** sysfs delay **************************************************/
static ssize_t as9888_sysfs_delay_show(
	struct as9888_data *akm, char *buf, int pos)
{
	int64_t val;

	mutex_lock(&akm->val_mutex);
	val = akm->delay[pos];
	mutex_unlock(&akm->val_mutex);

	return sprintf(buf, "%lld\n", val);
}

static ssize_t as9888_sysfs_delay_store(
	struct as9888_data *akm, char const *buf, size_t count, int pos)
{
	long long val = 0;

	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (false == get_value_as_int64(buf, count, &val))
		return -EINVAL;

	mutex_lock(&akm->val_mutex);
	akm->delay[pos] = val;
	mutex_unlock(&akm->val_mutex);

	return count;
}

/***** Accelerometer ***/
static ssize_t as9888_delay_acc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_delay_show(
		dev_get_drvdata(dev), buf, ACC_DATA_FLAG);
}
static ssize_t as9888_delay_acc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, ACC_DATA_FLAG);
}

/***** Magnetic field ***/
static ssize_t as9888_delay_mag_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_delay_show(
		dev_get_drvdata(dev), buf, MAG_DATA_FLAG);
}
static ssize_t as9888_delay_mag_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, MAG_DATA_FLAG);
}

/***** Orientation ***/
static ssize_t as9888_delay_ori_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return as9888_sysfs_delay_show(
		dev_get_drvdata(dev), buf, ORI_DATA_FLAG);
}
static ssize_t as9888_delay_ori_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return as9888_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, ORI_DATA_FLAG);
}


/***** accel (binary) ***/
#if 0
static ssize_t as9888_bin_accel_write(
	struct kobject *kobj,
	struct bin_attribute *attr,
			char *buf,
			loff_t pos,
			size_t size)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct as9888_data *akm = dev_get_drvdata(dev);
	int16_t *accel_data;

	if (size == 0)
		return 0;

	accel_data = (int16_t*)buf;

	mutex_lock(&akm->accel_mutex);
	akm->accel_data[0] = accel_data[0];
	akm->accel_data[1] = accel_data[1];
	akm->accel_data[2] = accel_data[2];
	mutex_unlock(&akm->accel_mutex);

	DMT_DATA(&akm->i2c->dev, "accel:%d,%d,%d\n",
		accel_data[0], accel_data[1], accel_data[2]);

	return size;
}
#endif


#if AS9888_DEBUG_IF
static ssize_t as9888_reset(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	int hard = 0;

	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (false == get_value_as_int(buf, count, &hard))
		return -EINVAL;

	if (AKECS_Reset(akm, hard) < 0)
		return -EINVAL;

	return 1;
}

static ssize_t as9888_mode_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	int mode = 0;

	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (false == get_value_as_int(buf, count, &mode))
		return -EINVAL;

	if (AKECS_SetMode(akm, mode) < 0)
		return -EINVAL;

	return 1;
}

static ssize_t as9888_bdata_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	char rbuf[SENSOR_DATA_SIZE];

	mutex_lock(&akm->sensor_mutex);
	memcpy(&rbuf, akm->sense_data, sizeof(rbuf));
	mutex_unlock(&akm->sensor_mutex);

	return sprintf(buf,
		"INT1ST=0x%02X, ST1=0x%02X, ST2=0x%02X, ST3=0x%02X, ST4=0x%02X\n"
		"  H1=0x%02X%02X,0x%02X%02X,0x%02X%02X\n"
		"  A1=0x%02X%02X,0x%02X%02X,0x%02X%02X\n",
		rbuf[0],rbuf[2],rbuf[35],rbuf[36],rbuf[37],
		rbuf[4],rbuf[3],rbuf[6],rbuf[5],rbuf[8],rbuf[7],
		rbuf[12],rbuf[11],rbuf[14],rbuf[13],rbuf[16],rbuf[15]);
}

static ssize_t as9888_e2p_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	int err;
	unsigned char e2p[19];
	int i, len;
	char *pbuf;

	err = AKECS_SetMode(akm, AS9888_MODE_EEPROM);
	if (err < 0)
		return err;

	e2p[0] = AS9888_EEP_EREF1;
	err = as9888_i2c_rxdata(akm->i2c, e2p, sizeof(e2p));
	if (err < 0)
		return err;

	err = AKECS_SetMode(akm, AS9888_MODE_POWERDOWN);
	if (err < 0)
		return err;

	pbuf = buf;
	for (i = 0; i < 19; i++) {
		len = snprintf(pbuf, 6, "0x%02X,", e2p[i]);
		pbuf += len;
	}

	return (pbuf - buf);
}

static ssize_t as9888_reg_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	int err;
	unsigned char i2c[1];

	i2c[0] = (unsigned char)atomic_read(&akm->addr);
	err = as9888_i2c_rxdata(akm->i2c, i2c, 1);
	if (err < 0)
		return err;

	return sprintf(buf, "0x%02X\n", i2c[0]);
}

static ssize_t as9888_reg_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	struct as9888_data *akm = dev_get_drvdata(dev);
	int addr = 0;

	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (false == get_value_as_int(buf, count, &addr))
		return -EINVAL;

	if (addr < 0 || 128 < addr)
		return -EINVAL;

	atomic_set(&akm->addr, addr);

	return 1;
}

#endif

static struct device_attribute as9888_attributes[] = {
	__ATTR(enable_acc, 0660, as9888_enable_acc_show, as9888_enable_acc_store),
	__ATTR(enable_mag, 0660, as9888_enable_mag_show, as9888_enable_mag_store),
	__ATTR(enable_ori, 0660, as9888_enable_ori_show, as9888_enable_ori_store),
	__ATTR(delay_acc,  0660, as9888_delay_acc_show,  as9888_delay_acc_store),
	__ATTR(delay_mag,  0660, as9888_delay_mag_show,  as9888_delay_mag_store),
	__ATTR(delay_ori,  0660, as9888_delay_ori_show,  as9888_delay_ori_store),
#if AS9888_DEBUG_IF
	__ATTR(reset, 0220, NULL, as9888_reset),
	__ATTR(mode,  0220, NULL, as9888_mode_store),
	__ATTR(bdata, 0440, as9888_bdata_show, NULL),
	__ATTR(e2p,   0440, as9888_e2p_show, NULL),
	__ATTR(reg,   0660, as9888_reg_show, as9888_reg_store),
#endif
	__ATTR_NULL,
};

#if 0
#define __BIN_ATTR(name_, mode_, size_, private_, read_, write_) \
	{ \
		.attr    = { .name = __stringify(name_), .mode = mode_ }, \
		.size    = size_, \
		.private = private_, \
		.read    = read_, \
		.write   = write_, \
	}

#define __BIN_ATTR_NULL \
	{ \
		.attr   = { .name = NULL }, \
	}

static struct bin_attribute as9888_bin_attributes[] = {
	__BIN_ATTR(accel, 0220, 6, NULL,
				NULL, as9888_bin_accel_write),
	__BIN_ATTR_NULL
};
#endif

static char const *const compass_class_name = "compass";
static char const *const as9888_device_name = "as9888";
static char const *const device_link_name = "i2c";
static dev_t const as9888_device_dev_t = MKDEV(MISC_MAJOR, 240);

static int create_sysfs_interfaces(struct as9888_data *akm)
{
	int err;

	if (NULL == akm)
		return -EINVAL;

	err = 0;

	akm->compass = class_create(THIS_MODULE, compass_class_name);
	if (IS_ERR(akm->compass)) {
		err = PTR_ERR(akm->compass);
		goto exit_class_create_failed;
	}

	akm->class_dev = device_create(
						akm->compass,
						NULL,
						as9888_device_dev_t,
						akm,
						as9888_device_name);
	if (IS_ERR(akm->class_dev)) {
		err = PTR_ERR(akm->class_dev);
		goto exit_class_device_create_failed;
	}

	err = sysfs_create_link(
			&akm->class_dev->kobj,
			&akm->i2c->dev.kobj,
			device_link_name);
	if (0 > err)
		goto exit_sysfs_create_link_failed;

	err = create_device_attributes(
			akm->class_dev,
			as9888_attributes);
	if (0 > err)
		goto exit_device_attributes_create_failed;
#if 0
	err = create_device_binary_attributes(
			&akm->class_dev->kobj,
			as9888_bin_attributes);
	if (0 > err)
		goto exit_device_binary_attributes_create_failed;
#endif

	return err;

#if 0
exit_device_binary_attributes_create_failed:
	remove_device_attributes(akm->class_dev, as9888_attributes);
#endif
exit_device_attributes_create_failed:
	sysfs_remove_link(&akm->class_dev->kobj, device_link_name);
exit_sysfs_create_link_failed:
	device_destroy(akm->compass, as9888_device_dev_t);
exit_class_device_create_failed:
	akm->class_dev = NULL;
	class_destroy(akm->compass);
exit_class_create_failed:
	akm->compass = NULL;
	return err;
}

static void remove_sysfs_interfaces(struct as9888_data *akm)
{
	if (NULL == akm)
		return;

	if (NULL != akm->class_dev) {
#if 0
		remove_device_binary_attributes(
			&akm->class_dev->kobj,
			as9888_bin_attributes);
#endif
		remove_device_attributes(
			akm->class_dev,
			as9888_attributes);
		sysfs_remove_link(
			&akm->class_dev->kobj,
			device_link_name);
		akm->class_dev = NULL;
	}
	if (NULL != akm->compass) {
		device_destroy(
			akm->compass,
			as9888_device_dev_t);
		class_destroy(akm->compass);
		akm->compass = NULL;
	}
}


/***** akm input device functions ***********************************/
static int as9888_input_init(struct input_dev **input)
{
	int err = 0;

	/* Declare input device */
	*input = input_allocate_device();
	if (!*input)
		return -ENOMEM;

	/* Setup input device */
	set_bit(EV_ABS, (*input)->evbit);
	/* Accelerometer [-78.5, 78.5]m/s2 in Q16*/
	input_set_abs_params(*input, ABS_X, -5144576, 5144576, 0, 0);
	input_set_abs_params(*input, ABS_Y, -5144576, 5144576, 0, 0);
	input_set_abs_params(*input, ABS_Z, -5144576, 5144576, 0, 0);
	input_set_abs_params(*input, ABS_THROTTLE, 0, 3, 0, 0);
	/* Magnetometer [-1229, 1229]uT in Q16 */
	input_set_abs_params(*input, ABS_RX, -80543744, 80543744, 0, 0);
	input_set_abs_params(*input, ABS_RY, -80543744, 80543744, 0, 0);
	input_set_abs_params(*input, ABS_RZ, -80543744, 80543744, 0, 0);
	input_set_abs_params(*input, ABS_RUDDER, 0, 3, 0, 0);
	/* Orientation [yaw:0,360 pitch:-180,180 roll:-90,90]deg
	   in Q16 */
	input_set_abs_params(*input, ABS_HAT0X, 0, 23592960, 0, 0);
	input_set_abs_params(*input, ABS_HAT0Y, -11796480, 11796480, 0, 0);
	input_set_abs_params(*input, ABS_HAT1X, -5898240, 5898240, 0, 0);
	input_set_abs_params(*input, ABS_HAT1Y, 0, 3, 0, 0);

	/* Set name */
	(*input)->name = "compass";

	/* Register */
	err = input_register_device(*input);
	if (err) {
		input_free_device(*input);
		return err;
	}

	return err;
}

/***** akm functions ************************************************/

static irqreturn_t as9888_irq(int irq, void *handle)
{
	struct as9888_data *akm = handle;
	char buffer[SENSOR_DATA_SIZE];
	int err;

	memset(buffer, 0, sizeof(buffer));
	buffer[BDIDX(AS9888_REG_INT1ST)] = AS9888_REG_INT1ST;
	err = as9888_i2c_rxdata(akm->i2c, buffer, 2);
	if (err < 0) {
		dev_err(&akm->i2c->dev, "%s failed.", __func__);
		goto work_func_end;
	}
	/* Check INTST value */
	if (buffer[BDIDX(AS9888_REG_INT1ST)] == 0x00) {
		dev_err(&akm->i2c->dev, "%s ST is not set.", __func__);
		goto work_func_end;
	}
	/* Check ST1 register */
	if ((buffer[BDIDX(AS9888_REG_INT1ST)] & 0x01) ||
			(buffer[BDIDX(AS9888_REG_INT1ST)] & 0x02)) {
		/* DR/TM is set, read measurement data */
		buffer[BDIDX(AS9888_REG_ST1)] = AS9888_REG_ST1;
		err = as9888_i2c_rxdata(akm->i2c,
				&buffer[BDIDX(AS9888_REG_ST1)], (SENSOR_DATA_SIZE - 2));
		if (err < 0) 
		{
			dev_err(&akm->i2c->dev, "%s failed.", __func__);
			goto work_func_end;
		}
		mutex_lock(&akm->sensor_mutex);
		memcpy(&akm->sense_data[BDIDX(AS9888_REG_ST1)],
				&buffer[BDIDX(AS9888_REG_ST1)], (SENSOR_DATA_SIZE - 2));
	} 
	else {
		/* Other event, read only ST3 and ST4 */
		buffer[BDIDX(AS9888_REG_ST3)] = AS9888_REG_ST3;
		err = as9888_i2c_rxdata(akm->i2c,
				&buffer[BDIDX(AS9888_REG_ST3)], 2);
		if (err < 0) 
		{
			dev_err(&akm->i2c->dev, "%s failed.", __func__);
			goto work_func_end;
		}
		mutex_lock(&akm->sensor_mutex);
		memcpy(&akm->sense_data[BDIDX(AS9888_REG_ST3)],	&buffer[BDIDX(AS9888_REG_ST3)], 2);
	}
	akm->sense_data[BDIDX(AS9888_REG_INT2ST)] |= buffer[BDIDX(AS9888_REG_INT2ST)];
	akm->sense_data[BDIDX(AS9888_REG_INT1ST)] |= buffer[BDIDX(AS9888_REG_INT1ST)];
	mutex_unlock(&akm->sensor_mutex);

	atomic_set(&akm->drdy, 1);
	if (atomic_read(&akm->ms_mode) != AS9888_MODE_CNT_MEASURE) 
	{
		atomic_set(&akm->ms_mode, 0);
	}
	wake_up(&akm->drdy_wq);

work_func_end:
	return IRQ_HANDLED;
}

#if 0
static void as9888_delayed_work(struct work_struct *work)
{
	struct as9888_data *akm = container_of(work, struct as9888_data, work.work);

	as9888_irq(akm->irq, akm);
}
#endif

static int as9888_suspend(struct device *dev)
{
        return 0;
}

static int as9888_resume(struct device *dev)
{
        return 0;
}

int as9888_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct as9888_platform_data *pdata;
	int err = 0;
	int i;

	dev_dbg(&client->dev, "start probing.");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "%s: check_functionality failed.", __func__);
		err = -ENODEV;
		goto exit0;
	}

	/* Allocate memory for driver data */
	s_akm = kzalloc(sizeof(struct as9888_data), GFP_KERNEL);
	if (!s_akm) {
		dev_err(&client->dev, "%s: memory allocation failed.", __func__);
		err = -ENOMEM;
		goto exit1;
	}

	/***** Set layout information *****/
	pdata = client->dev.platform_data;
	if (pdata) 
	{
		/* Platform data is available. copy its value to local. */
		s_akm->layout = pdata->layout;
		s_akm->rstn = pdata->gpio_RSTN;
	} 
	else {
		/* Platform data is not available.
		   Layout information should be set by each application. */
		dev_dbg(&client->dev, "%s: No platform data.", __func__);
		s_akm->layout = 0;
		s_akm->rstn = 0;
	}

	/***** I2C initialization *****/
	s_akm->i2c = client;
	/* check connection */
	err = as9888_i2c_check_device(client);
	if (err < 0)
		goto exit2;
	/* set client data */
	i2c_set_clientdata(client, s_akm);


	/***** input *****/
	err = as9888_input_init(&s_akm->input);
	if (err) 
	{
		dev_err(&client->dev, "%s: input_dev register failed", __func__);
		goto exit3;
	}
	input_set_drvdata(s_akm->input, s_akm);


	/**** initialize variables in as9888_data *****/
	init_waitqueue_head(&s_akm->drdy_wq);
	init_waitqueue_head(&s_akm->open_wq);

	mutex_init(&s_akm->sensor_mutex);
	mutex_init(&s_akm->val_mutex);

	atomic_set(&s_akm->active, 0);
	atomic_set(&s_akm->ms_mode, 0);
	atomic_set(&s_akm->drdy, 0);
	atomic_set(&s_akm->suspend, 0);
	atomic_set(&s_akm->addr, 0);

	s_akm->enable_flag = 0;
	for (i=0; i<DMT_NUM_SENSORS; i++)
		s_akm->delay[i] = -1;


	/***** IRQ setup *****/
	s_akm->irq = client->irq;

	if (s_akm->irq == 0) 
	{
		dev_dbg(&client->dev, "%s: IRQ is not set.", __func__);
		/* Use timer to notify measurement end */
		/* INIT_DELAYED_WORK(&s_akm->work, as9888_delayed_work); */
		goto exit4;
	} 
	else {
		err = request_threaded_irq(	s_akm->irq,	NULL, as9888_irq, IRQF_TRIGGER_HIGH|IRQF_ONESHOT, dev_name(&client->dev), s_akm);
		if (err < 0) 
		{
			dev_err(&client->dev,
				"%s: request irq failed.", __func__);
			goto exit4;
		}
	}

	/***** misc *****/
	err = misc_register(&as9888_dev);
	if (err) 
	{
		dev_err(&client->dev, "%s: as9888_dev register failed", __func__);
		goto exit5;
	}

	/***** sysfs *****/
    err = create_sysfs_interfaces(s_akm);
    if (0 > err) 
	{
        dev_err(&client->dev, "%s: create sysfs failed.", __func__);
        goto exit6;
    }

	dev_dbg(&client->dev, "successfully probed.");
	return 0;

exit6:
	misc_deregister(&as9888_dev);
exit5:
	if (s_akm->irq)
		free_irq(s_akm->irq, s_akm);
exit4:
	input_unregister_device(s_akm->input);
exit3:
exit2:
	kfree(s_akm);
exit1:
exit0:
	return err;
}

static int as9888_remove(struct i2c_client *client)
{
	struct as9888_data *akm = i2c_get_clientdata(client);

	remove_sysfs_interfaces(akm);
	if (misc_deregister(&as9888_dev) < 0)
		dev_dbg(&client->dev, "misc deregister failed.");
	if (akm->irq)
		free_irq(akm->irq, akm);
	input_unregister_device(akm->input);
	kfree(akm);
	dev_dbg(&client->dev, "successfully removed.");
	return 0;
}

static const struct i2c_device_id as9888_id[] = {
	{AS9888_I2C_NAME, 0 },
	{ }
};

static const struct dev_pm_ops as9888_pm_ops = {
        .suspend    = as9888_suspend,
        .resume     = as9888_resume,
};

static struct i2c_driver as9888_driver = {
        .probe          = as9888_probe,
        .remove  = as9888_remove,
        .id_table       = as9888_id,
        .driver = {
                .name = AS9888_I2C_NAME,
                .pm             = &as9888_pm_ops,
        },
};

static int __init as9888_init(void)
{
	printk(KERN_INFO "AS9888 compass driver: initialize.");
	return i2c_add_driver(&as9888_driver);
}

static void __exit as9888_exit(void)
{
	printk(KERN_INFO "AS9888 compass driver: release.");
	i2c_del_driver(&as9888_driver);
}

module_init(as9888_init);
module_exit(as9888_exit);

MODULE_AUTHOR("DMT_RD");
MODULE_DESCRIPTION("AS9888 compass driver");
MODULE_LICENSE("GPL");

