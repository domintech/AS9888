/******************************************************************************
 *
 * $Id: AS9888Driver.h 278 2011-08-01 09:07:36Z yamada.rj $
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
#ifndef DMTD_INC_AS9888DRIVER_H
#define DMTD_INC_AS9888DRIVER_H

#include <as9888.h>	/* Device driver */
#include <stdint.h>			/* int8_t, int16_t etc. */

/*** Constant definition ******************************************************/
#define AS9888_REG_WIA		0x00
#define AS9888_REG_WIA		0x00
#define AS9888_REG_INT1ST	0x01
#define AS9888_REG_INT2ST	0x02
#define AS9888_REG_ST1		0x03
#define AS9888_REG_HXL		0x04
#define AS9888_REG_HXH		0x05
#define AS9888_REG_HYL		0x06
#define AS9888_REG_HYH		0x07
#define AS9888_REG_HZL		0x08
#define AS9888_REG_HZH		0x09
#define AS9888_REG_TMPS		0x0A
#define AS9888_REG_EMPT		0x0B
#define AS9888_REG_A1XL		0x0C
#define AS9888_REG_A1XH		0x0D
#define AS9888_REG_A1YL		0x0E
#define AS9888_REG_A1YH		0x0F
#define AS9888_REG_A1ZL		0x10
#define AS9888_REG_A1ZH		0x11
#define AS9888_REG_A2XL		0x12
#define AS9888_REG_A2XH		0x13
#define AS9888_REG_A2YL		0x14
#define AS9888_REG_A2YH		0x15
#define AS9888_REG_A2ZL		0x16
#define AS9888_REG_A2ZH		0x17
#define AS9888_REG_A3XL		0x18
#define AS9888_REG_A3XH		0x19
#define AS9888_REG_A3YL		0x1A
#define AS9888_REG_A3YH		0x1B
#define AS9888_REG_A3ZL		0x1C
#define AS9888_REG_A3ZH		0x1D
#define AS9888_REG_A4XL		0x1E
#define AS9888_REG_A4XH		0x1F
#define AS9888_REG_A4YL		0x20
#define AS9888_REG_A4YH		0x21
#define AS9888_REG_A4ZL		0x22
#define AS9888_REG_A4ZH		0x23
#define AS9888_REG_ST2		0x24
#define AS9888_REG_ST3		0x25
#define AS9888_REG_ST4		0x26
#define AS9888_REG_PEWSL	0x27
#define AS9888_REG_PEWSH	0x28
#define AS9888_REG_MEST1	0x29
#define AS9888_REG_MEST2	0x2A
#define AS9888_REG_MEST3	0x2B
#define AS9888_REG_MEST4	0x2C
#define AS9888_REG_MEST5	0x2D
#define AS9888_REG_MEST6	0x2E
#define AS9888_REG_MEST7	0x2F
#define AS9888_REG_MEST8	0x30
#define AS9888_REG_MEST9	0x31
#define AS9888_REG_HLIST	0x32
#define AS9888_REG_HLST		0x33

#define AS9888_REG_REF1		0x34
#define AS9888_REG_REF2		0x35
#define AS9888_REG_REF3		0x36
#define AS9888_REG_OSC		0x37
#define AS9888_REG_AXGA		0x38
#define AS9888_REG_AYGA		0x39
#define AS9888_REG_AZGA		0x3A
#define AS9888_REG_AXDA		0x3B
#define AS9888_REG_AYDA		0x3C
#define AS9888_REG_AZDA		0x3D
#define AS9888_REG_TMSE1	0x3E
#define AS9888_REG_TMSE2	0x3F
#define AS9888_REG_AXOF		0x40
#define AS9888_REG_AYOF		0x41
#define AS9888_REG_AZOF		0x42
#define AS9888_REG_PESE		0x43
#define AS9888_REG_MESE1	0x44
#define AS9888_REG_MESE2	0x45
#define AS9888_REG_MESE3	0x46
#define AS9888_REG_MESE4	0x47
#define AS9888_REG_MESE5	0x48
#define AS9888_REG_MOSE1	0x49
#define AS9888_REG_MOSE2	0x4A
#define AS9888_REG_MOSE3	0x4B
#define AS9888_REG_MOSE4	0x4C
#define AS9888_REG_TPSE1	0x4D
#define AS9888_REG_TPSE2	0x4E
#define AS9888_REG_TPSE3	0x4F
#define AS9888_REG_TPSE4	0x50
#define AS9888_REG_POSE1	0x51
#define AS9888_REG_POSE2	0x52
#define AS9888_REG_HLXB1	0x53
#define AS9888_REG_HLYB1	0x54
#define AS9888_REG_HLZB1	0x55
#define AS9888_REG_HLSE1	0x56
#define AS9888_REG_HLTH1	0x57
#define AS9888_REG_HLXB2	0x58
#define AS9888_REG_HLYB2	0x59
#define AS9888_REG_HLZB2	0x5A
#define AS9888_REG_HLSE2	0x5B
#define AS9888_REG_HLTH2	0x5C
#define AS9888_REG_PSSE		0x5D
#define AS9888_REG_INT1EN	0x5E
#define AS9888_REG_INT2EN	0x5F
#define AS9888_REG_SLCT1	0x60
#define AS9888_REG_SLCT2	0x61
#define AS9888_REG_MS		0x62
#define AS9888_REG_TST1		0x63
#define AS9888_REG_TST2		0x64

#define AS9888_E2P_EREF1	0x65
#define AS9888_E2P_EREF2	0x66
#define AS9888_E2P_EREF3	0x67
#define AS9888_E2P_EOSC		0x68
#define AS9888_E2P_EHCX		0x69
#define AS9888_E2P_EHCY		0x6A
#define AS9888_E2P_EHCZ		0x6B
#define AS9888_E2P_EAXGA	0x6C
#define AS9888_E2P_EAYGA	0x6D
#define AS9888_E2P_EAZGA	0x6E
#define AS9888_E2P_EAXDT	0x6F
#define AS9888_E2P_EAYDT	0x70
#define AS9888_E2P_EAZDT	0x71
#define AS9888_E2P_EAXDA	0x72
#define AS9888_E2P_EAYDA	0x73
#define AS9888_E2P_EAZDA	0x74
#define AS9888_E2P_EAXGTD	0x75
#define AS9888_E2P_EAYGTD	0x76
#define AS9888_E2P_EAZGTD	0x77
#define AS9888_E2P_ET1		0x78
#define AS9888_E2P_ET2		0x79

#define DMT_TRUE	1		/*!< Represents true */
#define DMT_FALSE	0		/*!< Represents false */
#define DMT_SUCCESS	1		/*!< Represents success.*/
#define DMT_FAIL	0		/*!< Represents fail. */
#define DMT_ERROR	-1		/*!< Represents error. */

/*! Size of writable registers */
#define AS9888_IMAGE_SIZE	46
/*! Size of EEPROM area */
#define AS9888_E2P_SIZE		21

/*! 0:Don't Output data, 1:Output data */
#define DMT_DBG_DATA	0
/*! Typical interval in ns */
#define AS9888_MEASUREMENT_TIME_NS	((AS9888_MEASUREMENT_TIME_US) * 1000)


/*** Type declaration *********************************************************/
typedef unsigned char BYTE;
typedef int DMT_BOOL;

/*** Global variables *********************************************************/

/*** Prototype of Function  ***************************************************/

int16_t DMT_InitDevice(void);

void DMT_DeinitDevice(void);

int16_t DMT_TxData(
	const BYTE address,
	const BYTE* data,
	const uint16_t numberOfBytesToWrite);

int16_t DMT_RxData(
	const BYTE address,
	BYTE* data,
	const uint16_t numberOfBytesToRead);

int16_t DMT_GetData(BYTE data[SENSOR_DATA_SIZE]);

void DMT_SetYPR(const int buf[YPR_DATA_SIZE]);

int DMT_GetOpenStatus(int* status);

int DMT_GetCloseStatus(int* status);

int16_t DMT_SetMode(const BYTE mode);

int16_t DMT_GetDelay(int64_t delay[DMT_NUM_SENSORS]);

int16_t DMT_GetLayout(char* layout);

int16_t DMT_GetTemperature(char* temperature);

int16_t DMT_Reset(void);

int16_t DMT_GetImage(BYTE image[AS9888_IMAGE_SIZE]);

int16_t DMT_SetImage(const BYTE image[AS9888_IMAGE_SIZE]);

int16_t DMT_GetE2P(BYTE e2p[AS9888_E2P_SIZE]);

#endif //DMTD_INC_AS9888DRIVER_H

