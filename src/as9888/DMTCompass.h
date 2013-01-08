/******************************************************************************
 *
 * $Id: DMTCompass.h 298 2011-08-11 12:24:23Z yamada.rj $
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
#ifndef DMTD_INC_DMTCOMPASS_H
#define DMTD_INC_DMTCOMPASS_H

#include "DMTCommon.h"
#include "CustomerSpec.h"

#ifdef WIN32
#include "AS9888_LinuxDriver.h"
#else
#include "AS9888Driver.h"
#endif

/****************************************/
/* Include files for AS9888  library. */
/****************************************/
#include "AKEC_AS9888.h"
#include "AKEC_Configure.h"
#include "AKEC_DOE.h"
#include "AKEC_Device.h"
#include "AKEC_Direction.h"
#include "AKEC_METS.h"
#include "AKEC_VNorm.h"
#include "AKEC_Version.h"

#include "libFST_AS9888.h"


/*** Constant definition ******************************************************/

/*** Type declaration *********************************************************/
typedef union _AS9888IMAGE {
	struct {
		uint8	REF1;
		uint8	REF2;
		uint8	REF3;
		uint8	OSC;
		uint8	AXGA;
		uint8	AYGA;
		uint8	AZGA;
		uint8	AXDA;
		uint8	AYDA;
		uint8	AZDA;
		uint8	TMSE1;
		uint8	TMSE2;
		uint8	AXOF;
		uint8	AYOF;
		uint8	AZOF;
		uint8	PESE;
		uint8	MESE1;
		uint8	MESE2;
		uint8	MESE3;
		uint8	MESE4;
		uint8	MESE5;
		uint8	MOSE1;
		uint8	MOSE2;
		uint8	MOSE3;
		uint8	MOSE4;
		uint8	RESERVE1;
		uint8	RESERVE2;
		uint8	RESERVE3;
		uint8	RESERVE4;
		uint8	POSE1;
		uint8	POSE2;
		uint8	HLXB1;
		uint8	HLYB1;
		uint8	HLZB1;
		uint8	HLSE1;
		uint8	HLTH1;
		uint8	HLXB2;
		uint8	HLYB2;
		uint8	HLZB2;
		uint8	HLSE2;
		uint8	HLTH2;
		uint8	PSSE;
		uint8	INT1EN;
		uint8	INT2EN;
		uint8	SLCT1;
		uint8	SLCT2;
	}r;
	uint8 v[AS9888_IMAGE_SIZE];
} AS9888IMAGE;

typedef union _AS9888EEPROM {
	struct {
		uint8	EREF1;
		uint8	EREF2;
		uint8	EREF3;
		uint8	EOSC;
		uint8	EHCX;
		uint8	EHCY;
		uint8	EHCZ;
		uint8	EAXGA;
		uint8	EAYGA;
		uint8	EAZGA;
		uint8	EAXDT;
		uint8	EAYDT;
		uint8	EAZDT;
		uint8	EAXDA;
		uint8	EAYDA;
		uint8	EAZDA;
		uint8	EAXGTD;
		uint8	EAYGTD;
		uint8	EAZGTD;
		uint8	ET1;
		uint8	ET2;
	}r;
	uint8 v[AS9888_E2P_SIZE];
} AS9888EEPROM;

/*! A parameter structure. */
typedef struct _AS9888PRMS{

	/* Variables for Decomp8977. */
	AS9888_MSMODE	m_mode;
	AKFLOAT			mf_temperature;	/* Celsius degree */
	AKFVEC			mfv_hdata[AKEC_HDATA_SIZE];
	AKFVEC			mfv_adata[AKEC_ADATA_SIZE];
	uint8			mi_st1;
	uint8			mi_st2;
	uint8			mi_st3;
	uint8			mi_st4;

	/* Variables for HDOE. */
	AKEC_DOE_VAR	m_doev;
	int16			mi_doeRet;

	/* Variables for VNorm. */
	AKFVEC			mfv_hvbuf[AKEC_HDATA_SIZE];
	AKFVEC			mfv_avbuf[AKEC_HDATA_SIZE];
	AKFVEC			mfv_HO;
	AKFVEC			mfv_ao;
	AKFVEC			mfv_hs;
	AKFVEC			mfv_as;

	/* Variables for Direction. */
	int16			mi_hnaveD;
	int16			mi_anaveD;
	AKEC_PATNO		m_layout;
	AKFLOAT			mf_azimuth;
	AKFLOAT			mf_pitch;
	AKFLOAT			mf_roll;

	/* Variables for vector output */
	AKFVEC			mfv_hvec;
	AKFVEC			mfv_avec;
	int16			mi_hnaveV;
	int16			mi_anaveV;

	/* Variables for AS9888 Registers and EEPROM */
	AS9888IMAGE		m_img;
	AS9888EEPROM	m_e2p;
	FST_COMP		m_cmp;

	/* ACC Functions */
	uint16			mi_PEWS;
	uint8			mi_HLIST;
	uint8			mi_HLST;

	/* METSCal */
	int16			miv_pMEST[9];
	int32			miv_METS_n[AKEC_ME_LVNUM];
	int32			miv_METS_p[AKEC_ME_LVNUM];
	AKEC_EXTH		m_METS_ex;
	AKEC_EXTH		m_METS_exp;
	double			mf_METS_pmets;

} AS9888PRMS;

#endif

