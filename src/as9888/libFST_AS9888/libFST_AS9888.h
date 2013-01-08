/******************************************************************************
 *
 * $Id: libFST_AS9888.h 288 2011-08-04 10:12:00Z yamada.rj $
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
#ifndef LIBFST_INC_AS9888_H
#define LIBFST_INC_AS9888_H

#include "AKEC_Device.h"			/* int8_t, int16_t etc. */

#define FST_TEST_FAIL		 0
#define FST_TEST_PASS		 1
#define FST_ERROR			-1

#define FST_NUMOF_STEPS		(39+23+47)

#define FST_MODE_CONTINUE		0
#define FST_MODE_STOP_ON_FAIL	1

#define FST_DMT_TRT		25	/* normal temperature of test */
#define FST_DMT_THT		60	/* high temeperature of test */

/***** Type declaration *******************************************************/
typedef int16(*DEV_TX_DATA)(const uint8, const uint8 *, const uint16);
typedef int16(*DEV_RX_DATA)(const uint8, uint8 *, const uint16);
typedef int16(*DEV_SET_MODE)(const uint8);
typedef int16(*DEV_GET_DATA)(uint8*);
typedef int16(*DEV_RESET)(void);

typedef struct _DEV_HANDLE {
	DEV_TX_DATA		txData;
	DEV_RX_DATA		rxData;
	DEV_SET_MODE	setMode;
	DEV_GET_DATA	getData;
	DEV_RESET		reset;
} DEV_HANDLE;

typedef struct _FST_RESULT {
	int8	status;	/* FST_TEST_FAIL, FST_TEST_PASS, FST_ERROR */
	uint8	step;
	uint8	no;
	int16	data;
} FST_RESULT;

typedef struct _FST_COMP{
	AKFLOAT		t9eRt;
	AKFLOAT		t9eLv;
	AKFVEC		kst;	/*!< Kste */
	AKFVEC		st;		/*!< Sensitivity when tested (t9eRt) */
	AKFVEC		kot;	/*!< Temperature gradient coefficient */
	AKFVEC		ot;		/*!< Offset when calibrated (t9eLv) */
} FST_COMP;

/***** Prototype of function **************************************************/
AKLIB_C_API_START
void AKEC_GetAccelerometerParam(
	const	FST_COMP*	cmp,		/* Compensation parameters */
	const	AKFLOAT	curTemperature, /* Current temperature */
			AKFVEC* as,				/* Current sensitivity */
			AKFVEC*	ao				/* Current offset */
);

/*!
  @return FST_TEST_FAIL, FST_TEST_PASS, FST_ERROR
 */
int16 AKEC_Test_And_Compensate(
	const	DEV_HANDLE*	handle,
	const	int16		mode,
	const	int16		nave,
	const	uint8		filter,
			FST_RESULT	result[FST_NUMOF_STEPS],
			FST_COMP*	cmp
);

AKLIB_C_API_END

#endif

