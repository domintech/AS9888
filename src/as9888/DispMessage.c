/******************************************************************************
 *
 * $Id: DispMessage.c 317 2011-08-23 11:07:13Z yamada.rj $
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
#include "DispMessage.h"
#include "DMTCommon.h"

/*!
 Print startup message to Android Log daemon.
 */
void Disp_StartMessage(void)
{
	LOGI("AS9888 for Android v20110823 "
		 "(Library: v%d.%d.%d.%d) started.",
		 AKEC_GetVersion_Major(),
		 AKEC_GetVersion_Minor(),
		 AKEC_GetVersion_Revision(),
		 AKEC_GetVersion_DateCode());
	/*LOGI("Debug: %s", ((ENABLE_DMTDEBUG)?("ON"):("OFF")));*/
	/*LOGI("Debug level: %d", DBG_LEVEL);*/
	/*LOGI("Output to: %s", ((OUTPUT_STDOUT)?("stdout"):("LOGD")));*/
}

/*!
 Print ending message to Android Log daemon.
 */
void Disp_EndMessage(int ret)
{
	LOGI("AS9888 for Android end (%d).", ret);
}

/*!
 Print calculated result.
 @param[in] prms A pointer to a #AS9888PRMS structure. The value of member
 variables of this structure will be printed.
 */
void Disp_Result_DR(AS9888PRMS *prms)
{
	DMTDATA(DMTDATA_DR,
		"AZIMUTH[deg]=%6.1f, PITCH[deg]=%6.1f, ROLL[deg]=%6.1f\n",
		prms->mf_azimuth, prms->mf_pitch, prms->mf_roll);
}
void Disp_Result_TM(AS9888PRMS *prms)
{
	DMTDATA(DMTDATA_TM,
		"Temperature[LSB]=%d\n",
		prms->m_img.r.TMSE1);
}
void Disp_Result_PE(AS9888PRMS *prms)
{
	uint8 PEST = (prms->mi_st3 & 0x01);
	DMTDATA(DMTDATA_PE,
		"Pedometer[Steps]=%d, Status=%s\n",
		prms->mi_PEWS, ((PEST)?("Walking"):("Stop")));
}
void Disp_Result_ME(AS9888PRMS *prms)
{
	uint8 MELV = ((prms->mi_st3 & 0x0E) >> 1);
	DMTDATA(DMTDATA_ME,
		"METS Level=%d, Excercise=%6.3f, METS:%6.3f\n",
		MELV, prms->m_METS_exp.u.ex2,
		((prms->m_METS_ex.u.ex1 - prms->mf_METS_pmets) * 3600.0f / 16.0f) + 1.);
}
void Disp_Result_MO(AS9888PRMS *prms)
{
	uint8 MOST = (prms->mi_st4 & 0x3F);
	if (MOST & 0x01)
		DMTDATA(DMTDATA_MO, "Motion = +X\n");
	if (MOST & 0x02)
		DMTDATA(DMTDATA_MO, "Motion = -X\n");
	if (MOST & 0x04)
		DMTDATA(DMTDATA_MO, "Motion = +Y\n");
	if (MOST & 0x08)
		DMTDATA(DMTDATA_MO, "Motion = -Y\n");
	if (MOST & 0x10)
		DMTDATA(DMTDATA_MO, "Motion = +Z\n");
	if (MOST & 0x20)
		DMTDATA(DMTDATA_MO, "Motion = -Z\n");
}
void Disp_Result_PO(AS9888PRMS *prms)
{
	uint8 POST = ((prms->mi_st3 & 0x70) >> 4);
	switch(POST){
	case 0:
		DMTDATA(DMTDATA_PO, "Posture = +X up\n");
		break;
	case 1:
		DMTDATA(DMTDATA_PO, "Posture = +Y up\n");
		break;
	case 2:
		DMTDATA(DMTDATA_PO, "Posture = +Z up\n");
		break;
	case 3:
		DMTDATA(DMTDATA_PO, "Posture = +X down\n");
		break;
	case 4:
		DMTDATA(DMTDATA_PO, "Posture = +Y down\n");
		break;
	case 5:
		DMTDATA(DMTDATA_PO, "Posture = +Z down\n");
		break;
	case 7:
		DMTDATA(DMTDATA_PO, "Posture = Initial Value\n");
		break;
	}

}
void Disp_Result_HLPS(AS9888PRMS *prms)
{
	int16 hlst1, hlst2;
	hlst1 = (prms->mi_HLST & 0x03);
	hlst2 = (prms->mi_HLST & 0x0C)>>2;

	if(hlst1 == 0) {
		DMTDATA(DMTDATA_HLPS, "HG/LG1: Undefined, ");
	}
	else if(hlst1 == 1) {
		DMTDATA(DMTDATA_HLPS, "HG/LG1: L state  , ");
	}
	else if(hlst1 == 2) {
		DMTDATA(DMTDATA_HLPS, "HG/LG1: H state  , ");
	}

	if(hlst2 == 0) {
		DMTDATA(DMTDATA_HLPS, "HG/LG2: Undefined\n");
	}
	else if(hlst2 == 1) {
		DMTDATA(DMTDATA_HLPS, "HG/LG2: L state\n");
	}
	else if(hlst2 == 2) {
		DMTDATA(DMTDATA_HLPS, "HG/LG2: H state\n");
	}
}

/*!
 Output main menu to stdout and wait for user input from stdin.
 @return Selected mode.
 */
MODE Menu_Main(void)
{
	char msg[20];
	memset(msg, 0, sizeof(msg));

	DMTDEBUG(DBG_LEVEL1,
	" --------------------  AS9888 Console Application -------------------- \n"
	"   1. Start measurement. \n"
	"   2. Start offset calibration for accelerometer. \n"
	"   Q. Quit application. \n"
	" --------------------------------------------------------------------- \n"
	" Please select a number.\n"
	"   ---> ");
	fgets(msg, 10, stdin);
	DMTDEBUG(DBG_LEVEL1, "\n");

	// BUG : If 2-digits number is input,
	//    only the first character is compared.
	if (!strncmp(msg, "1", 1)) {
		return MODE_Measure;
	} else if (!strncmp(msg, "2", 1)) {
		return MODE_AOffsetCal;
	} else if (strncmp(msg, "Q", 1) == 0 || strncmp(msg, "q", 1) == 0) {
		return MODE_Quit;
	} else {
		return MODE_ERROR;
	}
}

void Disp_FST_Result(FST_RESULT results[], const int16 ret)
{
	int16 i = 0;

	/* Verbose output */
	DMTDATA(DMTDATA_FST, "------------------\n");
	DMTDATA(DMTDATA_FST, " No.   Fail  Data\n");
	DMTDATA(DMTDATA_FST, "------------------\n");
	while((results[i].status != FST_ERROR) && (i < FST_NUMOF_STEPS)){
		DMTDATA(DMTDATA_FST, " %d-%2d  %-4s  %d\n",
				results[i].step, results[i].no,
				((results[i].status == FST_TEST_PASS)?("."):("F")),
				results[i].data);
		i++;
	}
	DMTDATA(DMTDATA_FST, "------------------\n");

	/* Show result */
	if(ret == FST_TEST_PASS){
		DMTDEBUG(DBG_LEVEL1, " FST passed.\n\n");
	} else {
		DMTDEBUG(DBG_LEVEL1, " FST failed.\n\n");
	}
}


