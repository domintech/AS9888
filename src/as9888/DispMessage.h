/******************************************************************************
 *
 * $Id: DispMessage.h 298 2011-08-11 12:24:23Z yamada.rj $
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
#ifndef DMTD_INC_DISPMESG_H
#define DMTD_INC_DISPMESG_H

/* Include file for AS9888 library. */
#include "DMTCompass.h"

/*** Constant definition ******************************************************/
#define DISP_CONV_Q16(val)		((int)((val)*65536))

/*** Type declaration *********************************************************/

/*! These defined types represents the current mode. */
typedef enum _MODE {
	MODE_ERROR,			/*!< Error */
	MODE_AOffsetCal,	/*!< Calibrate offset for accelerometer */
	MODE_Measure,		/*!< Measurement */
	MODE_Quit			/*!< Quit */
} MODE;

/*** Prototype of function ****************************************************/
/*
	Disp_   : Display messages.
	Menu_   : Display menu (two or more selection) and wait for user input.
	Prmpt_  : Display prompt and wait for user to input a message.
 */

void Disp_StartMessage(void);

void Disp_EndMessage(int ret);

void Disp_Result_DR(AS9888PRMS *prms);
void Disp_Result_TM(AS9888PRMS *prms);
void Disp_Result_PE(AS9888PRMS *prms);
void Disp_Result_ME(AS9888PRMS *prms);
void Disp_Result_MO(AS9888PRMS *prms);
void Disp_Result_PO(AS9888PRMS *prms);
void Disp_Result_HLPS(AS9888PRMS *prms);


// Defined in main.c
void Disp_ResultHook(AS9888PRMS *prms, const uint16 flag);

MODE Menu_Main(void);

void Disp_FST_Result(FST_RESULT results[], const int16 ret);

#endif

