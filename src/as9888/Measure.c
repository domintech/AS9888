/******************************************************************************
 *
 * $Id: Measure.c 298 2011-08-11 12:24:23Z yamada.rj $
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
#include "DMTCommon.h"
#include "DMTCompass.h"
#include "DispMessage.h"
#include "FileIO.h"
#include "Measure.h"

#ifdef WIN32
#include "AS9888_LinuxDriver.h"
#endif

/*! Convert float to register */
//#define AO2REG(fAO)		(uint8)(((int16)((fAO)>0)/16)

void AO2REG(
	const	AKFLOAT aoFloat[3],
			uint8	aoReg[3])
{
	int16 i;
	int16 rnd;

	for (i=0; i<3; i++) {
		if (aoFloat[i] > 0) {
			rnd = 8;
		} else {
			rnd = -8;
		}
		aoReg[i] = (uint8)((aoFloat[i] + rnd)/16);
	}
}

/*! Default register image */
static const AS9888IMAGE s_DefImage = {{
		0x00,			/* REF1*/
		0x00,			/* REF2*/
		0x00,			/* REF3*/
		0x00,			/* OSC*/
		CSPEC_AXGA_PE16,/* AXGA*/
		CSPEC_AYGA_PE32,/* AYGA*/
		CSPEC_AZGA_PE64,/* AZGA*/
		0x00,			/* AXDA*/
		0x00,			/* AYDA*/
		0x00,			/* AZDA*/
		0x00,			/* TMSE1*/
		0x00,			/* TMSE2*/
		0x00,			/* AXOF*/
		0x00,			/* AYOF*/
		0x00,			/* AZOF*/
		CSPEC_PESE,		/* PESE*/
		CSPEC_MESE1,	/* MESE1*/
		CSPEC_MESE2,	/* MESE2*/
		CSPEC_MESE3,	/* MESE3*/
		CSPEC_MESE4,	/* MESE4*/
		CSPEC_MESE5,	/* MESE5*/
		CSPEC_MOSE1,	/* MOSE1*/
		CSPEC_MOSE2,	/* MOSE2*/
		CSPEC_MOSE3,	/* MOSE3*/
		CSPEC_MOSE4,	/* MOSE4*/
		0x00,			/* RESERVE1*/
		0x00,			/* RESERVE2*/
		0x00,			/* RESERVE3*/
		0x00,			/* RESERVE4*/
		CSPEC_POSE1,	/* POSE1*/
		CSPEC_POSE2,	/* POSE2*/
		CSPEC_HLXB1,	/* HLXB1*/
		CSPEC_HLYB1,	/* HLYB1*/
		CSPEC_HLZB1,	/* HLZB1*/
		CSPEC_HLSE1,	/* HLSE1*/
		CSPEC_HLTH1,	/* HLTH1*/
		CSPEC_HLXB2,	/* HLXB2*/
		CSPEC_HLYB2,	/* HLYB2*/
		CSPEC_HLZB2,	/* HLZB2*/
		CSPEC_HLSE2,	/* HLSE2*/
		CSPEC_HLTH2,	/* HLTH2*/
		CSPEC_PSSE,		/* PSSE*/
		CSPEC_INT1EN,	/* INT1EN*/
		CSPEC_INT2EN,	/* INT2EN*/
		CSPEC_SLCT1,	/* SLCT1*/
		CSPEC_SLCT2		/* SLCT2*/
}};

/*!
 Initialize #AS9888PRMS structure. At first, 0 is set to all parameters.
 After that, some parameters, which should not be 0, are set to specific
 value. Some of initial values can be customized by editing the file
 \c "CustomerSpec.h".
 @param[out] prms A pointer to #AS9888PRMS structure.
 */
void InitAS9888PRMS(AS9888PRMS* prms)
{
	/* Set 0 to the AS9888 structure. */
	memset(prms, 0, sizeof(AS9888PRMS));

	/* Sensitivity */
	prms->mfv_hs.u.x = AKEC_HSENSE_DEFAULT;
	prms->mfv_hs.u.y = AKEC_HSENSE_DEFAULT;
	prms->mfv_hs.u.z = AKEC_HSENSE_DEFAULT;
	prms->mfv_as.u.x = AKEC_ASENSE_DEFAULT;
	prms->mfv_as.u.y = AKEC_ASENSE_DEFAULT;
	prms->mfv_as.u.z = AKEC_ASENSE_DEFAULT;

	/* Initialize variables that initial value is not 0. */
	prms->mi_hnaveD = CSPEC_HNAVE_D;
	prms->mi_anaveD = CSPEC_ANAVE_D;
	prms->mi_hnaveV = CSPEC_HNAVE_V;
	prms->mi_anaveV = CSPEC_ANAVE_V;
	prms->m_layout = CSPEC_LAYOUT;
}

/*!
 */
int16 RecoverRegisters(AS9888PRMS*	prms)
{
	/* Load image from file */
	if(LoadImageFile(&prms->m_img) != DMT_SUCCESS) 
	{
		return DMT_FAIL;
	}

	/* Read EEPROM value from device */
	if(DMT_GetE2P(prms->m_e2p.v) != DMT_SUCCESS) 
	{
		return DMT_FAIL;
	}
	/* Set Image to device */
	if(DMT_SetImage(prms->m_img.v) != DMT_SUCCESS) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Set initial values to registers of AS9888. Then initialize algorithm
 parameters.
 @return If parameters are read successfully, the return value is
 #DMT_SUCCESS. Otherwise the return value is #DMT_FAIL. No
 error code is reserved to show which operation has failed.
 @param[in,out] prms A pointer to a #AS9888PRMS structure.
 */
int16 InitAS9888_Measure(AS9888PRMS* prms)
{
	char	temperature;
	AKFLOAT tF;

	/* Initialize for Decomp */
	AKEC_InitDecompAS9888(AKEC_HDATA_SIZE, prms->mfv_hdata, AKEC_ADATA_SIZE, prms->mfv_adata);

	/* Initialize for DOE */
	AKEC_InitDOE(&prms->m_doev);
	/* Initialize DOE success flag */
	prms->mi_doeRet = 0;

	/* Initialize for METS */
	AKEC_InitMETSCal(
			prms->miv_pMEST,
			prms->miv_METS_n,
			prms->miv_METS_p,
			&(prms->m_METS_ex),
			&(prms->m_METS_exp));

	/* Get current temperature */
	if (DMT_GetTemperature(&temperature) != DMT_SUCCESS) 
	{
		DMTERROR;
		return DMT_FAIL;
	}
	DMTDATA(DMTDATA_INIT, "Temperature = 0x%02X\n", temperature);
	prms->m_img.r.TMSE1 = temperature;
	tF = 35 + (120 - temperature)/1.6f;

	/* Calculate offset and sensitivity */
	AKEC_GetAccelerometerParam(&prms->m_cmp, tF, &prms->mfv_as, &prms->mfv_ao);

	/* AXOF, AYOF, AZOF register */
	AO2REG(prms->mfv_ao.v, &prms->m_img.r.AXOF);

	/* To work posture function */
	prms->m_img.r.POSE1 = 0x00;
	prms->m_img.r.POSE2 = 0x0F;

	/* Set Image to registers */
	if(DMT_SetImage(prms->m_img.v) != DMT_SUCCESS) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	DMTDATA(DMTDATA_INIT, "AGA= 0x%02X,0x%02X,0x%02X\n",
			prms->m_img.r.AXGA,
			prms->m_img.r.AYGA,
			prms->m_img.r.AZGA);
	DMTDATA(DMTDATA_INIT, "ADA= 0x%02X,0x%02X,0x%02X\n",
			prms->m_img.r.AXDA,
			prms->m_img.r.AYDA,
			prms->m_img.r.AZDA);
	DMTDATA(DMTDATA_INIT, "AOF= 0x%02X,0x%02X,0x%02X\n",
			prms->m_img.r.AXOF,
			prms->m_img.r.AYOF,
			prms->m_img.r.AZOF);
	DMTDATA(DMTDATA_INIT, "REF= 0x%02X,0x%02X,0x%02X,0x%02X\n",
			prms->m_img.r.REF1,
			prms->m_img.r.REF2,
			prms->m_img.r.REF3,
			prms->m_img.r.OSC);

	return DMT_SUCCESS;
}

/*!
  Offset calibration for accelerometer.
 @param[in,out] prms A pointer to a #AS9888PRMS structure.
 */
int16 Acc_Compensate(
	AS9888PRMS*	prms
)
{
	DEV_HANDLE	handle;
	FST_RESULT	results[FST_NUMOF_STEPS];
	BYTE i2c[10];
	int16 pf;

	/* Set handler */
	handle.txData = DMT_TxData;
	handle.rxData = DMT_RxData;
	handle.setMode = DMT_SetMode;
	handle.getData = DMT_GetData;
	handle.reset = DMT_Reset;

	/* Function Test */
	pf = AKEC_Test_And_Compensate(
			&handle,
		    FST_MODE_CONTINUE,
			CSPEC_FST_AVE,
			(CSPEC_SLCT2 & 0x08),
			results,
		   	&prms->m_cmp
		);
	Disp_FST_Result(results, pf);

	/* Save parameters to file */
	if (SaveParameters(prms) != DMT_SUCCESS) {
		return DMT_FAIL;
	}

	/* Get current setting (REF1 : AZDA) from device */
	if(DMT_RxData(AS9888_REG_REF1, i2c, 10) != DMT_SUCCESS) {
		DMTERROR;
		return DMT_FAIL;
	}
	/* copy the read value to current image, then save it to file. */
	prms->m_img = s_DefImage;
	prms->m_img.r.REF1 = i2c[0];
	prms->m_img.r.REF2 = i2c[1];
	prms->m_img.r.REF3 = i2c[2];
	prms->m_img.r.OSC  = i2c[3];
	prms->m_img.r.AXGA = ((prms->m_img.r.AXGA&0x80)|(i2c[4]&0x7F));
	prms->m_img.r.AYGA = ((prms->m_img.r.AYGA&0x80)|(i2c[5]&0x7F));
	prms->m_img.r.AZGA = ((prms->m_img.r.AZGA&0x80)|(i2c[6]&0x7F));
	prms->m_img.r.AXDA = i2c[7];
	prms->m_img.r.AYDA = i2c[8];
	prms->m_img.r.AZDA = i2c[9];
	if (SaveImageFile(&prms->m_img) != DMT_SUCCESS) {
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 This is the main routine of measurement.
 @param[in,out] prms A pointer to a #AS9888PRMS structure.
 */
void MeasureLoop(AS9888PRMS* prms)
{
	BYTE    i2cData[SENSOR_DATA_SIZE]; /* INT1ST ~ ST4 */
	int16   ret;
	struct	timespec currTime = {0, 0};
	struct	timespec lastTime = {0, 0};
	DMT_BOOL	elapsedTimer = DMT_FALSE;
	DMT_BOOL	isFirstMeasure = DMT_TRUE;

	/* Initialize library functions and device */
	if (InitAS9888_Measure(prms) != DMT_SUCCESS) {
		goto MEASURE_END;
	}

	/* Beginning time */
	if (clock_gettime(CLOCK_MONOTONIC, &lastTime) < 0) {
		DMTERROR;
		goto MEASURE_END;
	}

	/* Set to measurement msmode  */
	if(DMT_SetMode(AS9888_MODE_CNT_MEASURE) != DMT_SUCCESS) {
		goto MEASURE_END;
	}

	//int i = 0;
	while (g_stopRequest != DMT_TRUE) {
		if (DMT_GetData(i2cData) == DMT_SUCCESS) {

			/* Get current time */
			if (clock_gettime(CLOCK_MONOTONIC, &currTime) < 0) {
				DMTERROR;
				goto MEASURE_END;
			}
			/* Calcurate duration */
			if ((currTime.tv_sec - lastTime.tv_sec) > 16) {
				elapsedTimer = DMT_TRUE;
				/* Copy the last time */
				lastTime = currTime;
			} else {
				elapsedTimer = DMT_FALSE;
			}

			/* To make posture function work */
			if(isFirstMeasure) {
				// Set POSE1, POSE2 register
				i2cData[0] = prms->m_img.r.POSE1 = CSPEC_POSE1;
				i2cData[1] = prms->m_img.r.POSE2 = CSPEC_POSE2;
				if(DMT_TxData(AS9888_REG_POSE1, i2cData, 2) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				isFirstMeasure = DMT_FALSE;
			}

			/* Check DR1 */
			if(i2cData[0] & 0x01) {
				ret = EventProcess_DR(&i2cData[AS9888_REG_ST1 - AS9888_REG_INT1ST], prms);
				Disp_ResultHook(prms, 0xFF);
			}
			/* Check TM1 */
			if(i2cData[0] & 0x02) {
				ret = EventProcess_TM(i2cData[AS9888_REG_TMPS - AS9888_REG_INT1ST], prms);

				/* Set TMSE1 */
				if(DMT_TxData(AS9888_REG_TMSE1, &prms->m_img.r.TMSE1, 1) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				/* Set AOF */
				if(DMT_TxData(AS9888_REG_AXOF, &prms->m_img.r.AXOF, 3) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				Disp_Result_TM(prms);
			}
			/* Check PE1 */
			if(i2cData[0] & 0x04) {
				BYTE pews[2];
				if(DMT_RxData(AS9888_REG_PEWSL, pews, 2) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				prms->mi_st3 = i2cData[AS9888_REG_ST3 - AS9888_REG_INT1ST];
				prms->mi_PEWS = ((((uint16)pews[1])<<8) + ((uint16)pews[0]));
				Disp_Result_PE(prms);
			}
			/* Check ME1 */
			if((i2cData[0] & 0x08) || elapsedTimer){
				BYTE mets[9];
				if(DMT_RxData(AS9888_REG_MEST1, mets, 9) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				prms->mi_st3 = i2cData[AS9888_REG_ST3 - AS9888_REG_INT1ST];
				ret = EventProcess_ME(mets, elapsedTimer, prms);
				Disp_Result_ME(prms);
			}
			/* Check MO1 */
			if(i2cData[0] & 0x10) {
				prms->mi_st4 = i2cData[AS9888_REG_ST4 - AS9888_REG_INT1ST];
				Disp_Result_MO(prms);
			}
			/* Check PO1 */
			if(i2cData[0] & 0x40) {
				prms->mi_st3 = i2cData[AS9888_REG_ST3 - AS9888_REG_INT1ST];
				Disp_Result_PO(prms);
			}
			/* Check HLPS1 */
			if(i2cData[0] & 0x80) {
				BYTE hlst[2];
				if(DMT_RxData(AS9888_REG_HLIST, hlst, 2) != DMT_SUCCESS) {
					DMTERROR;
					goto MEASURE_END;
				}
				prms->mi_HLIST = hlst[0];
				prms->mi_HLST = hlst[1];
				Disp_Result_HLPS(prms);
			}
		} else {
			DMTERROR;
			goto MEASURE_END;
		}
#ifdef WIN32
		AKD_MAGIC();
		if (_kbhit()) {
			_getch();
			goto MEASURE_END;
		}
#endif
	}

MEASURE_END:
	/* Set to PowerDown mode */
	if (DMT_SetMode(AS9888_MODE_POWERDOWN) != DMT_SUCCESS) 
	{
		DMTERROR;
		return;
	}
}


int16 EventProcess_DR(const uint8	bData[], AS9888PRMS* prms)
{
	int16 akret;

	/* Decomposition */
	akret = AKEC_DecompAS9888(
		bData,
		AKEC_CONT,
		&prms->m_e2p.r.EHCX,
		&prms->mf_temperature,
		AKEC_HDATA_SIZE,
		prms->mfv_hdata,
		AKEC_ADATA_SIZE,
		prms->mfv_adata,
		&prms->mi_st1,
		&prms->mi_st2
	);

	if(akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	/* DOE for magnetometer */
	akret = AKEC_DOE(&prms->m_doev, prms->mfv_hdata, &prms->mfv_HO);

	if (akret == 1) 
	{
		prms->mi_doeRet = 3;
	}

	/* Calculate offset and sensitivity for accelerometer */
	AKEC_GetAccelerometerParam(&prms->m_cmp, prms->mf_temperature, &prms->mfv_as, &prms->mfv_ao);

	/* VbNorm */
	akret = AKEC_VbNorm(
		AKEC_HDATA_SIZE,
		prms->mfv_hdata,
		1,
		&prms->mfv_HO,
		&prms->mfv_hs,
		AKEC_HSENSE_TARGET,
		AKEC_HDATA_SIZE,
		prms->mfv_hvbuf
	);
	if(akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	akret = AKEC_VbNorm(
		AKEC_ADATA_SIZE,
		prms->mfv_adata,
		1,
		&prms->mfv_ao,
		&prms->mfv_as,
		AKEC_ASENSE_TARGET,
		AKEC_ADATA_SIZE,
		prms->mfv_avbuf
	);
	if(akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	/* Azimuth calculation */
	akret = AKEC_Direction(
		AKEC_HDATA_SIZE,
		prms->mfv_hvbuf,
		prms->mi_hnaveD,
		AKEC_ADATA_SIZE,
		prms->mfv_avbuf,
		prms->mi_anaveD,
		prms->m_layout,
		&prms->mf_azimuth,
		&prms->mf_pitch,
		&prms->mf_roll
	);
	if(akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	/* Vector average */
	akret = AKEC_VbAve(
		AKEC_HDATA_SIZE,
		prms->mfv_hvbuf,
		prms->mi_hnaveV,
		&prms->mfv_hvec);
	if (akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}
	akret = AKEC_VbAve(
		AKEC_ADATA_SIZE,
		prms->mfv_avbuf,
		prms->mi_anaveV,
		&prms->mfv_avec);
	if (akret == AKEC_ERROR) 
	{
		DMTERROR;
		return DMT_FAIL;
	}

	DMTDATA(DMTDATA_VEC, "hvec=%7.2f, %7.2f, %7.2f avec=%7.2f, %7.2f, %7.2f\n",
		prms->mfv_hvec.u.x, prms->mfv_hvec.u.y, prms->mfv_hvec.u.z,
		prms->mfv_avec.u.x, prms->mfv_avec.u.y, prms->mfv_avec.u.z);
	/*DMTDATA(DBG_LEVEL0, "ho=%f,%f,%f\n",
		prms->mfv_HO.u.x, prms->mfv_HO.u.y, prms->mfv_HO.u.z); */

	return DMT_SUCCESS;
}

/*!
 */
int16 EventProcess_TM( const uint8 temperature, AS9888PRMS*	prms)
{
	AKFLOAT tF;

	prms->m_img.r.TMSE1 = temperature;
	tF = 35 + (120 - temperature)/1.6f;

	/* Calculate offset and sensitivity */
	AKEC_GetAccelerometerParam(&prms->m_cmp, tF, &prms->mfv_as, &prms->mfv_ao);

	/* AXOF, AYOF, AZOF register */
	AO2REG(prms->mfv_ao.v, &prms->m_img.r.AXOF);

	return DMT_SUCCESS;
}

/*!
 */
int16 EventProcess_ME(const uint8	mets[], DMT_BOOL elapsedTimer, AS9888PRMS*	prms)
{
	int16 i;
	int16 MEST[9];

	/* copy to int16 */
	for (i=0; i<9; i++) 
	{
		MEST[i] = mets[i];
	}

	if (elapsedTimer) 
	{
		prms->mf_METS_pmets = prms->m_METS_ex.u.ex1;
	}

	if(AKEC_METSCal(
		MEST,
		prms->miv_pMEST,
		prms->miv_METS_n,
		prms->miv_METS_p,
		&(prms->m_METS_ex),
		&(prms->m_METS_exp)
		) != 1) 
	{
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}


