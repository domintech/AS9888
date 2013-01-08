/******************************************************************************
 *
 * $Id: Measure.h 278 2011-08-01 09:07:36Z yamada.rj $
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
#ifndef DMTD_INC_MEASURE_H
#define DMTD_INC_MEASURE_H

// Include files for AS9888 library.
#include "DMTCompass.h"

/*** Constant definition ******************************************************/
#define AS9888_INTEN_DRI		0x01
#define AS9888_INTEN_TMI		0x02
#define AS9888_INTEN_PEI		0x04
#define AS9888_INTEN_MEI		0x08
#define AS9888_INTEN_MOI		0x10
#define AS9888_INTEN_POI		0x40
#define AS9888_INTEN_HLPSI		0x80

#define AS9888_SLCT1_PEEN		0x04
#define AS9888_SLCT1_MEEN		0x08
#define AS9888_SLCT1_MOEN		0x10
#define AS9888_SLCT1_POEN		0x40
#define AS9888_SLCT1_HLEN		0x80
#define AS9888_SLCT2_PSEN		0x01
#define AS9888_SLCT2_TMEN		0x02
#define AS9888_SLCT2_HEEN		0x04
#define AS9888_SLCT2_LFEN		0x08

/*** Type declaration *********************************************************/

/*** Global variables *********************************************************/
extern int	g_Int1IsHigh;

/*** Prototype of function ****************************************************/
void InitAS9888PRMS(AS9888PRMS*	prms);

void SetDefaultPRMS(AS9888PRMS*	prms);

int16 RecoverRegisters(AS9888PRMS* prms);

int16 Acc_Compensate(AS9888PRMS* prms);

int16 InitAS9888_Measure(AS9888PRMS* prms);

void MeasureLoop(AS9888PRMS* prms);

int16 EventProcess_DR( const uint8 bData[], AS9888PRMS* prms);

int16 EventProcess_TM( const uint8 temperature, AS9888PRMS*	prms);

int16 EventProcess_ME(
	const uint8	mets[],
	DMT_BOOL elapsedTimer,
	AS9888PRMS*	prms
);

#endif

