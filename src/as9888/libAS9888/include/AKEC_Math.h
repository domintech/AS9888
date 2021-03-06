/******************************************************************************
 *
 * $Id: AKEC_Math.h 219 2011-07-05 23:17:38Z yamada.rj $
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
#ifndef AKEC_INC_MATH_H
#define AKEC_INC_MATH_H

#include "AKEC_Configure.h"

/***** Constant definition ****************************************************/
#define AKEC_PI		3.141592654f
#define RAD2DEG(rad)	((rad)*180.0f/AKEC_PI)

/***** Macro definition *******************************************************/

#ifdef AKEC_PRECISION_DOUBLE
#define AKEC_SIN(x)			sin(x)
#define AKEC_COS(x)			cos(x)
#define AKEC_ASIN(x)		asin(x)
#define AKEC_ACOS(x)		acos(x)
#define AKEC_ATAN2(y, x)	atan2((y), (x))
#define AKEC_SQRT(x)		sqrt(x)
#else
#define AKEC_SIN(x)			sinf(x)
#define AKEC_COS(x)			cosf(x)
#define AKEC_ASIN(x)		asinf(x)
#define AKEC_ACOS(x)		acosf(x)
#define AKEC_ATAN2(y, x)	atan2f((y), (x))
#define AKEC_SQRT(x)		sqrtf(x)
#endif

#endif

