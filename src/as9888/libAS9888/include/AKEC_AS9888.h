/******************************************************************************
 *
 * $Id: AKEC_AS9888.h 219 2011-07-05 23:17:38Z yamada.rj $
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
#ifndef AKEC_INC_AS9888_H
#define AKEC_INC_AS9888_H

#include "AKEC_Device.h"

/***** Constant definition ****************************************************/
#define AKEC_BDATA_SIZE			34

#define AKEC_HDATA_SIZE			32
#define AKEC_ADATA_SIZE			32

#define AKEC_HSENSE_DEFAULT		1
#define AKEC_HSENSE_TARGET		0.3f
#define AKEC_ASENSE_DEFAULT		256
#define AKEC_ASENSE_TARGET		9.80665f

#define AKEC_ST1_ERROR(st1)	(((st1)&0x06) != 0x00)
#define AKEC_ST2_ERROR(st2)	(((st2)&0x0D) != 0x00)

/***** Type declaration *******************************************************/
typedef enum _AS9888_MSMODE {
	AKEC_SNG	= 0,
	AKEC_CONT	= 1
} AS9888_MSMODE;

/***** Prototype of function **************************************************/
AKLIB_C_API_START
void AKEC_InitDecompAS9888(
	const	int16	nhdata,
			AKFVEC	hdata[AKEC_HDATA_SIZE],
	const	int16	nadata,
			AKFVEC	adata[AKEC_ADATA_SIZE]
);

int16 AKEC_DecompAS9888(
	const	uint8		bdata[AKEC_BDATA_SIZE],
	const	AS9888_MSMODE	msmode,
	const	uint8		EHC[3],
			AKFLOAT*	temperature,
	const	int16		nhdata,
			AKFVEC		hdata[AKEC_HDATA_SIZE],
	const	int16		nadata,
			AKFVEC		adata[AKEC_ADATA_SIZE],
			uint8*		st1,
			uint8*		st2
);
AKLIB_C_API_END

#endif

