/*
 * @file include/linux/as9888.h
 * @brief AS9888 compass Linux device driver
 * @author Domintech Technology Co., Ltd (http://www.domintech.com.tw)
 * @version 1.0
 * @date 2011/11/28
 *
 * @section LICENSE
 *
 *  Copyright 2011 Domintech Technology Co., Ltd
 *
 * 	This software is licensed under the terms of the GNU General Public
 * 	License version 2, as published by the Free Software Foundation, and
 * 	may be copied, distributed, and modified under those terms.
 *
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 *
 */
#ifndef AS9888_H
#define AS9888_H

#include <linux/ioctl.h>

#define AS9888_I2C_NAME "as9888"

#define SENSOR_DATA_SIZE	38	/* INT1ST ~ ST4 */
#define YPR_DATA_SIZE		12
#define RWBUF_SIZE			128

#define ACC_DATA_FLAG		0
#define MAG_DATA_FLAG		1
#define ORI_DATA_FLAG		2
#define DMT_NUM_SENSORS		3

#define ACC_DATA_READY		(1<<(ACC_DATA_FLAG))
#define MAG_DATA_READY		(1<<(MAG_DATA_FLAG))
#define ORI_DATA_READY		(1<<(ORI_DATA_FLAG))

#define AS9888_WIA_VALUE		0x48
#define AS9888_MEASUREMENT_TIME_US	10000

#define	AS9888_MODE_POWERDOWN	0x00
#define AS9888_MODE_SNG_MEASURE	0x01
#define AS9888_MODE_CNT_MEASURE	0x02
#define AS9888_MODE_EEPROM		0x04
#define	AS9888_MODE_SELF_TEST	0x08
#define AS9888_MODE_SOFT_RESET	0x10

#define AS9888_REG_WIA			0x00
#define AS9888_REG_INT1ST		0x01
#define AS9888_REG_INT2ST		0x02
#define AS9888_REG_ST1			0x03
#define AS9888_REG_ST3			0x25
#define AS9888_REG_INT1EN		0x5E
#define AS9888_REG_MS			0x62
#define AS9888_EEP_EREF1		0x65


#define DMTIO                   0xA1

/* IOCTLs for DMT library */
#define ECS_IOCTL_READ              _IOWR(DMTIO, 0x01, char*)
#define ECS_IOCTL_WRITE             _IOW(DMTIO, 0x02, char*)
#define ECS_IOCTL_SET_MODE          _IOW(DMTIO, 0x03, short)
#define ECS_IOCTL_GETDATA           _IOR(DMTIO, 0x04, char[SENSOR_DATA_SIZE])
#define ECS_IOCTL_SET_YPR           _IOW(DMTIO, 0x05, int[YPR_DATA_SIZE])
#define ECS_IOCTL_GET_OPEN_STATUS   _IOR(DMTIO, 0x06, int)
#define ECS_IOCTL_GET_CLOSE_STATUS  _IOR(DMTIO, 0x07, int)
#define ECS_IOCTL_GET_DELAY         _IOR(DMTIO, 0x08, long long int[DMT_NUM_SENSORS])
#define ECS_IOCTL_GET_LAYOUT        _IOR(DMTIO, 0x09, char)
#define ECS_IOCTL_RESET				_IO(DMTIO, 0x0B)
#define ECS_IOCTL_GET_TEMPERATURE	_IOR(DMTIO, 0x0C, char)

struct as9888_platform_data {
	char layout;
	int gpio_INT1;
	int gpio_INT2;
	int gpio_RSTN;
};

#endif

