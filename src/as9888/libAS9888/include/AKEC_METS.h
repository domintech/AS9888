/******************************************************************************
 *
 * $Id: AKEC_METS.h 318 2011-08-24 00:01:09Z kitaura.mc $
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
#ifndef AKEC_INC_METS_H
#define AKEC_INC_METS_H

#include "AKEC_Device.h"

/***** Constant definition ****************************************************/
#define AKEC_ME_LVNUM	4		/*!< METS Lv num */

/***** Type declaration *******************************************************/
typedef union _AKEC_EXTH{
	struct {
		AKFLOAT	ex1;
		AKFLOAT	ex2;
		AKFLOAT	ex3;
		AKFLOAT	ex4;
	}u;
	AKFLOAT	v[4];
} AKEC_EXTH;

/***** Prototype of function **************************************************/
AKLIB_C_API_START
void AKEC_InitMETSCal(
			int16		pMEST[],	/*!< [out] */
			int32		n[],		/*!< [out] */
			int32		p[],		/*!< [out] */
			AKEC_EXTH	*ex,		/*!< [out] */
			AKEC_EXTH	*exp		/*!< [out] */
);

int16 AKEC_METSCal(	
  const		int16		MEST[],		/*!< [in] */
			int16		pMEST[],	/*!< [in/out] */
			int32		n[],		/*!< [in/out] */
			int32		p[],		/*!< [in/out] */
			AKEC_EXTH	*ex,		/*!< [out] */
			AKEC_EXTH	*exp		/*!< [out] */
);

AKLIB_C_API_END
#endif

