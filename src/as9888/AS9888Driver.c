/******************************************************************************
 *
 * $Id: AS9888Driver.c 287 2011-08-04 05:46:05Z yamada.rj $
 *
 * -- Copyright Notice --
 *
 * Copyright (c) 2004 Asahi Kasei Microdevices Corporation (AKM), Japan All
 * Rights Reserved.
 *
 * 1.Copyright
 * 1.1 All rights in and to this program are reserved to AKM.
 * 1.2 You acknowledge that derivative work of this program contains
 * copyrighted information of AKM.
 *
 * 2.Liability to third parties
 * AKM shall not be liable for any obligations to you, your customer or any
 * third party in connection with or in relation to this program or derivative
 * work of this program, including but not limited to, providing maintenance,
 * technical support, or update to your customer or any third party for this
 * program or derivative work of this program.
 *
 * 3.No Warranty
 * 3.1 To the maximum extent permitted by applicable law, this program is
 * provided to you "as is" and with all faults, and AKM hereby disclaims all
 * warranties and conditions, either express, implied or statutory, including
 * but not limited to, any (if any) implied warranties or conditions of
 * merchantability, of fitness for a particular purpose, of accuracy or
 * completeness of responses, of results, of up-to-dateness all with regard to
 * this program, and the provision of or failure to provide support services.
 * Also, there is no warranty or condition of title, quiet enjoyment, quiet
 * possession, correspondence to description or non-infringement, with regard
 * to this program.
 * 3.2 AKM shall not be liable for any losses or damages incurred by you which
 * are related to the development of derivative work of this program and/or
 * sale of Company Products.  The entire risk as to the quality of or arising
 * out of use or performance of this program and/or derivative work of this
 * program, if any, shall be assumed by you.
 *
 * 4.Limitation of liability
 * To the maximum extent permitted by applicable law, in no event shall AKM be
 * liable for any direct, special, incidental, indirect, or consequential
 * damages whatsoever (including, but not limited to, damages for loss of
 * profits, for business interruption, for personal injury, for loss of privacy,
 * and for any other pecuniary or other loss whatsoever) arising out of or in
 * any way related to the use of or inability to use of this program and/or
 * derivative work of this program, even if the provision of or failure to
 * provide support services.
 *
 * -- End Asahi Kasei Microdevices Copyright Notice --
 *
 ******************************************************************************/
#include <fcntl.h>
#include "DMTCommon.h"		// DBGPRINT()
#include "AS9888Driver.h"

#define SENSOR_NAME      "/dev/as9888_dev"

static int s_fdDev = -1;

/*!
 Open device driver.
 This function opens both device drivers of magnetic sensor and acceleration
 sensor. Additionally, some initial hardware settings are done, such as
 measurement range, built-in filter function and etc.
 @return If this function succeeds, the return value is #DMT_SUCCESS.
 Otherwise the return value is #DMT_FAIL.
 */
int16_t DMT_InitDevice(void)
{
	if (s_fdDev < 0) {
		// Open magnetic sensor's device driver.
		if ((s_fdDev = open(SENSOR_NAME, O_RDWR)) < 0) {
			DMTERROR_STR("open");
			return DMT_FAIL;
		}
	}

	return DMT_SUCCESS;
}

/*!
 Close device driver.
 This function closes both device drivers of magnetic sensor and acceleration
 sensor.
 */
void DMT_DeinitDevice(void)
{
	if (s_fdDev >= 0) {
		close(s_fdDev);
		s_fdDev = -1;
	}
}

/*!
 Writes data to a register of the AS9888.  When more than one byte of data is
 specified, the data is written in contiguous locations starting at an address
 specified in \a address.
 @return If this function succeeds, the return value is #DMT_SUCCESS. Otherwise
 the return value is #DMT_FAIL.
 @param[in] address Specify the address of a register in which data is to be
 written.
 @param[in] data Specify data to write or a pointer to a data array containing
 the data.  When specifying more than one byte of data, specify the starting
 address of the array.
 @param[in] numberOfBytesToWrite Specify the number of bytes that make up the
 data to write.  When a pointer to an array is specified in data, this argument
 equals the number of elements of the array.
 */
int16_t DMT_TxData(
	const	BYTE	address,
	const	BYTE	*data,
	const	uint16_t	numberOfBytesToWrite)
{
	int i;
	char buf[RWBUF_SIZE];

	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}
	if (numberOfBytesToWrite > (RWBUF_SIZE-2)) {
		LOGE("%s: Tx size is too large.", __FUNCTION__);
		return DMT_FAIL;
	}

	buf[0] = numberOfBytesToWrite + 1;
	buf[1] = address;

	for (i = 0; i < numberOfBytesToWrite; i++) {
		buf[i + 2] = data[i];
	}
	if (ioctl(s_fdDev, ECS_IOCTL_WRITE, buf) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	} else {

#if ENABLE_DMTDEBUG
		DMTDATA(DMTDATA_MAGDRV, "addr(HEX)=%02x data(HEX)=", address);
		for (i = 0; i < numberOfBytesToWrite; i++) {
			DMTDATA(DMTDATA_MAGDRV, " %02x", data[i]);
		}
		DMTDATA(DMTDATA_MAGDRV, "\n");
#endif
		return DMT_SUCCESS;
	}
}

/*!
 Acquires data from a register or the EEPROM of the AS9888.
 @return If this function succeeds, the return value is #DMT_SUCCESS. Otherwise
 the return value is #DMT_FAIL.
 @param[in] address Specify the address of a register from which data is to be
 read.
 @param[out] data Specify a pointer to a data array which the read data are
 stored.
 @param[in] numberOfBytesToRead Specify the number of bytes that make up the
 data to read.  When a pointer to an array is specified in data, this argument
 equals the number of elements of the array.
 */
int16_t DMT_RxData(
	const	BYTE	address,
			BYTE	*data,
	const	uint16_t	numberOfBytesToRead)
{
	int i;
	char buf[RWBUF_SIZE];

	memset(data, 0, numberOfBytesToRead);

	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}
	if (numberOfBytesToRead > (RWBUF_SIZE-1)) {
		LOGE("%s: Rx size is too large.", __FUNCTION__);
		return DMT_FAIL;
	}

	buf[0] = numberOfBytesToRead;
	buf[1] = address;

	if (ioctl(s_fdDev, ECS_IOCTL_READ, buf) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	} else {
		for (i = 0; i < numberOfBytesToRead; i++) {
			data[i] = buf[i + 1];
		}
#if ENABLE_DMTDEBUG
		DMTDATA(DMTDATA_MAGDRV, "addr(HEX)=%02x len=%d data(HEX)=",
				address, numberOfBytesToRead);
		for (i = 0; i < numberOfBytesToRead; i++) {
			DMTDATA(DMTDATA_MAGDRV, " %02x", data[i]);
		}
		DMTDATA(DMTDATA_MAGDRV, "\n");
#endif
		return DMT_SUCCESS;
	}
}

/*!
 Acquire magnetic data from AS9888. If measurement is not done, this function
 waits until measurement completion.
 @return If this function succeeds, the return value is #DMT_SUCCESS. Otherwise
 the return value is #DMT_FAIL.
 @param[out] data A magnetic data array. The size should be larger than #SENSOR_DATA_SIZE.
 */
int16_t DMT_GetData(BYTE data[SENSOR_DATA_SIZE])
{
	memset(data, 0, SENSOR_DATA_SIZE);

	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}

	if (ioctl(s_fdDev, ECS_IOCTL_GETDATA, data) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}

	DMTDATA(DMTDATA_MAGDRV,
		"INT1ST=%02x, ST1=%02x, ST2=%02x, ST3=%02x, ST4=%02x\n",
		data[0], data[1], data[35], data[36], data[37]);

	return DMT_SUCCESS;
}

/*!
 Set calculated data to device driver.
 @param[in] buf
 */
void DMT_SetYPR(const int buf[YPR_DATA_SIZE])
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
	}
	if (ioctl(s_fdDev, ECS_IOCTL_SET_YPR, buf) < 0) {
		DMTERROR_STR("ioctl");
	}
}

/*!
 */
int DMT_GetOpenStatus(int* status)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}
	if (ioctl(s_fdDev, ECS_IOCTL_GET_OPEN_STATUS, status) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}
	return DMT_SUCCESS;
}

/*!
 */
int DMT_GetCloseStatus(int* status)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}
	if (ioctl(s_fdDev, ECS_IOCTL_GET_CLOSE_STATUS, status) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}
	return DMT_SUCCESS;
}

/*!
 Set AS9888 to the specific mode.
 @return If this function succeeds, the return value is #DMT_SUCCESS. Otherwise
 the return value is #DMT_FAIL.
 @param[in] mode This value should be one of the AK8975_Mode which is defined in
 akm8975.h file.
 */
int16_t DMT_SetMode(const BYTE mode)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}

	if (ioctl(s_fdDev, ECS_IOCTL_SET_MODE, &mode) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Acquire delay
 @return If this function succeeds, the return value is #DMT_SUCCESS. Otherwise
 the return value is #DMT_FAIL.
 @param[out] delay A delay in microsecond.
 */
int16_t DMT_GetDelay(int64_t delay[DMT_NUM_SENSORS])
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.\n", __FUNCTION__);
		return DMT_FAIL;
	}
	if (ioctl(s_fdDev, ECS_IOCTL_GET_DELAY, delay) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}
	return DMT_SUCCESS;
}

/*!
 Get layout information from device driver, i.e. platform data.
 */
int16_t DMT_GetLayout(char* layout)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}

	if (ioctl(s_fdDev, ECS_IOCTL_GET_LAYOUT, layout) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Get layout information from device driver, i.e. platform data.
 */
int16_t DMT_GetTemperature(char* temperature)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}

	if (ioctl(s_fdDev, ECS_IOCTL_GET_TEMPERATURE, temperature) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
  Reset device
 */
int16_t DMT_Reset(void)
{
	if (s_fdDev < 0) {
		LOGE("%s: Device file is not opened.", __FUNCTION__);
		return DMT_FAIL;
	}

	if (ioctl(s_fdDev, ECS_IOCTL_RESET) < 0) {
		DMTERROR_STR("ioctl");
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
  Get register value
 */
int16_t DMT_GetImage(BYTE image[AS9888_IMAGE_SIZE])
{
	return DMT_RxData(AS9888_REG_REF1, image, AS9888_IMAGE_SIZE);
}

/*!
  Set register value
 */
int16_t DMT_SetImage(const BYTE image[AS9888_IMAGE_SIZE])
{
	return DMT_TxData(AS9888_REG_REF1, image, AS9888_IMAGE_SIZE);
}

/*!
  Get EEPROM value
 */
int16_t DMT_GetE2P(BYTE e2p[AS9888_E2P_SIZE])
{
	/* Set to EEPROM read access mode */
	if (DMT_SetMode(AS9888_MODE_EEPROM) != DMT_SUCCESS) {
		DMTERROR;
		return DMT_FAIL;
	}

	/* Read values. EREF1-ET2 */
	if (DMT_RxData(AS9888_EEP_EREF1, e2p, AS9888_E2P_SIZE) != DMT_SUCCESS) {
		DMTERROR;
		return DMT_FAIL;
	}

	/* Set to PowerDown mode  */
	if (DMT_SetMode(AS9888_MODE_POWERDOWN) != DMT_SUCCESS) {
		DMTERROR;
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

