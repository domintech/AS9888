/******************************************************************************
 *
 * $Id: TestLimit.h 298 2011-08-11 12:24:23Z yamada.rj $
 *
 * -- Copyright Notice --
 *
 * Copyright (c) 2004 Asahi Kasei Microdevices Corporation, Japan
 * All Rights Reserved.
 *
 * This software program is proprietary program of Asahi Kasei Microdevices 
 * Corporation("AKM") licensed to authorized Licensee under Software License 
 * Agreement (SLA) executed between the Licensee and AKM.
 * 
 * Use of the software by unauthorized third party, or use of the software 
 * beyond the scope of the SLA is strictly prohibited.
 *
 * -- End Asahi Kasei Microdevices Copyright Notice --
 *
 ******************************************************************************/
#ifndef INC_TESTLIMIT_H
#define INC_TESTLIMIT_H

/* Limit of factory shipment test */
/***** Step 1 *****/
#define TL1_NO_RESET				(2)

#define TL1_NO_RST_WIA				(4)
#define TL1_LO_RST_WIA				(0x48)
#define TL1_HI_RST_WIA				(0x48)

#define TL1_NO_SET_E2P				(5)

#define TL1_NO_RST_EREF1			(7)
#define TL1_LO_RST_EREF1			(0)
#define TL1_HI_RST_EREF1			(0xFF)

#define TL1_NO_RST_EREF2			(8)
#define TL1_LO_RST_EREF2			(0)
#define TL1_HI_RST_EREF2			(0xFF)

#define TL1_NO_RST_EREF3			(9)
#define TL1_LO_RST_EREF3			(0)
#define TL1_HI_RST_EREF3			(0xFF)

#define TL1_NO_RST_EOSC				(10)
#define TL1_LO_RST_EOSC				(0)
#define TL1_HI_RST_EOSC				(0xFF)

#define TL1_NO_RST_EHCX				(11)
#define TL1_LO_RST_EHCX				(0)
#define TL1_HI_RST_EHCX				(0xFF)

#define TL1_NO_RST_EHCY				(12)
#define TL1_LO_RST_EHCY				(0)
#define TL1_HI_RST_EHCY				(0xFF)

#define TL1_NO_RST_EHCZ				(13)
#define TL1_LO_RST_EHCZ				(0)
#define TL1_HI_RST_EHCZ				(0xFF)

#define TL1_NO_RST_EAXGA			(14)
#define TL1_LO_RST_EAXGA			(0)
#define TL1_HI_RST_EAXGA			(0xFF)

#define TL1_NO_RST_EAYGA			(15)
#define TL1_LO_RST_EAYGA			(0)
#define TL1_HI_RST_EAYGA			(0xFF)

#define TL1_NO_RST_EAZGA			(16)
#define TL1_LO_RST_EAZGA			(0)
#define TL1_HI_RST_EAZGA			(0xFF)

#define TL1_NO_RST_EAXDT			(17)
#define TL1_LO_RST_EAXDT			(0)
#define TL1_HI_RST_EAXDT			(0xFF)

#define TL1_NO_RST_EAYDT			(18)
#define TL1_LO_RST_EAYDT			(0)
#define TL1_HI_RST_EAYDT			(0xFF)

#define TL1_NO_RST_EAZDT			(19)
#define TL1_LO_RST_EAZDT			(0)
#define TL1_HI_RST_EAZDT			(0xFF)

#define TL1_NO_RST_EAXDA			(20)
#define TL1_LO_RST_EAXDA			(0)
#define TL1_HI_RST_EAXDA			(0xFF)

#define TL1_NO_RST_EAYDA			(21)
#define TL1_LO_RST_EAYDA			(0)
#define TL1_HI_RST_EAYDA			(0xFF)

#define TL1_NO_RST_EAZDA			(22)
#define TL1_LO_RST_EAZDA			(0)
#define TL1_HI_RST_EAZDA			(0xFF)

#define TL1_NO_RST_EAXGTD			(23)
#define TL1_LO_RST_EAXGTD			(0)
#define TL1_HI_RST_EAXGTD			(0xFF)

#define TL1_NO_RST_EAYGTD			(24)
#define TL1_LO_RST_EAYGTD			(0)
#define TL1_HI_RST_EAYGTD			(0xFF)

#define TL1_NO_RST_EAZGTD			(25)
#define TL1_LO_RST_EAZGTD			(0)
#define TL1_HI_RST_EAZGTD			(0xFF)

#define TL1_NO_RST_ET1				(26)
#define TL1_LO_RST_ET1				(0)
#define TL1_HI_RST_ET1				(0xFF)

#define TL1_NO_RST_ET2				(27)
#define TL1_LO_RST_ET2				(0)
#define TL1_HI_RST_ET2				(0xFF)

#define TL1_NO_SET_PDN1				(28)
#define TL1_NO_SET_REF1				(30)
#define TL1_NO_SET_AXGA				(34)
#define TL1_NO_SET_AXDA				(37)


/***** Step 2 *****/
#define TL2_NO_SET_INT1EN			(1)
#define TL2_NO_SET_SLCT2			(2)
#define TL2_NO_SET_SNG				(3)
#define TL2_NO_WAIT_SNG				(4)

#define TL2_NO_SNG_INT1ST			(5)
#define TL2_LO_SNG_INT1ST			(1)
#define TL2_HI_SNG_INT1ST			(1)

#define TL2_NO_SNG_ST1				(6)
#define TL2_LO_SNG_ST1				(1)
#define TL2_HI_SNG_ST1				(1)

#define TL2_NO_SNG_HX				(7)
#define TL2_LO_SNG_HX				(-4095)
#define TL2_HI_SNG_HX				(4094)

#define TL2_NO_SNG_HY				(9)
#define TL2_LO_SNG_HY				(-4095)
#define TL2_HI_SNG_HY				(4094)

#define TL2_NO_SNG_HZ				(11)
#define TL2_LO_SNG_HZ				(-4095)
#define TL2_HI_SNG_HZ				(4094)

#define TL2_NO_SNG_ST2				(13)
#define TL2_LO_SNG_ST2				(0)
#define TL2_HI_SNG_ST2				(0)

#define TL2_NO_SET_SELF				(14)
#define TL2_NO_WAIT_SELF			(15)

#define TL2_NO_SELF_ST1				(16)
#define TL2_LO_SELF_ST1				(1)
#define TL2_HI_SELF_ST1				(1)

#define TL2_NO_SELF_HX				(17)
#define TL2_LO_SELF_HX				(-100)
#define TL2_HI_SELF_HX				(100)

#define TL2_NO_SELF_HY				(19)
#define TL2_LO_SELF_HY				(-100)
#define TL2_HI_SELF_HY				(100)

#define TL2_NO_SELF_HZ				(21)
#define TL2_LO_SELF_HZ				(-1000)
#define TL2_HI_SELF_HZ				(-200)

#define TL2_NO_SELF_ST2				(23)
#define TL2_LO_SELF_ST2				(2)
#define TL2_HI_SELF_ST2				(2)


/***** Step 3 *****/
#define TL3_NO_SET_SLCT2			(1)
#define TL3_NO_SET_SNG				(2)
#define TL3_NO_WAIT_SNG				(3)

#define TL3_NO_ST1			(4)
#define TL3_LO_ST1			(1)
#define TL3_HI_ST1			(1)

#define TL3_NO_TMPS			(5)
#define TL3_LO_TMPS			(0x28)
#define TL3_HI_TMPS			(0xE0)

#define TL3_NO_EMPT			(6)
#define TL3_LO_EMPT			(0)
#define TL3_HI_EMPT			(0)

#define TL3_NO_AX			(7)
#define TL3_LO_AX			(-2047)
#define TL3_HI_AX			(2046)

#define TL3_NO_AY			(9)
#define TL3_LO_AY			(-2047)
#define TL3_HI_AY			(2046)

#define TL3_NO_AZ			(11)
#define TL3_LO_AZ			(-2047)
#define TL3_HI_AZ			(2046)

#define TL3_NO_ST2			(13)
#define TL3_LO_ST2			(2)
#define TL3_HI_ST2			(2)

#define TL3_NO_AX_MAX		(14)
#define TL3_LO_AX_MAX		(-2047)
#define TL3_HI_AX_MAX		(2046)

#define TL3_NO_AY_MAX		(15)
#define TL3_LO_AY_MAX		(-2047)
#define TL3_HI_AY_MAX		(2046)

#define TL3_NO_AZ_MAX		(16)
#define TL3_LO_AZ_MAX		(-2047)
#define TL3_HI_AZ_MAX		(2046)

#define TL3_NO_AX_MIN		(17)
#define TL3_LO_AX_MIN		(-2047)
#define TL3_HI_AX_MIN		(2046)

#define TL3_NO_AY_MIN		(18)
#define TL3_LO_AY_MIN		(-2047)
#define TL3_HI_AY_MIN		(2046)

#define TL3_NO_AZ_MIN		(19)
#define TL3_LO_AZ_MIN		(-2047)
#define TL3_HI_AZ_MIN		(2046)

#define TL3_NO_AX_VAR		(20)
#define TL3_LO_AX_VAR		(-2047)
#define TL3_HI_AX_VAR		(2046)

#define TL3_NO_AY_VAR		(21)
#define TL3_LO_AY_VAR		(-2047)
#define TL3_HI_AY_VAR		(2046)

#define TL3_NO_AZ_VAR		(22)
#define TL3_LO_AZ_VAR		(-2047)
#define TL3_HI_AZ_VAR		(2046)

#define TL3_NO_SET_AXDA		(24)
#define TL3_NO_SET_AYDA		(25)
#define TL3_NO_SET_AZDA		(26)

#define TL3_NO_OPT_SET_SNG	(27)
#define TL3_NO_OPT_WAIT_SNG	(28)

#define TL3_NO_OPT_ST1		(29)
#define TL3_LO_OPT_ST1		(1)
#define TL3_HI_OPT_ST1		(1)

#define TL3_NO_OPT_TMPS		(30)
#define TL3_LO_OPT_TMPS		(0x28)
#define TL3_HI_OPT_TMPS		(0xE0)

#define TL3_NO_OPT_EMPT		(31)
#define TL3_LO_OPT_EMPT		(0)
#define TL3_HI_OPT_EMPT		(0)

#define TL3_NO_OPT_AX		(32)
#define TL3_LO_OPT_AX		(-2047)
#define TL3_HI_OPT_AX		(2046)

#define TL3_NO_OPT_AY		(34)
#define TL3_LO_OPT_AY		(-2047)
#define TL3_HI_OPT_AY		(2046)

#define TL3_NO_OPT_AZ		(36)
#define TL3_LO_OPT_AZ		(-2047)
#define TL3_HI_OPT_AZ		(2046)

#define TL3_NO_OPT_ST2		(38)
#define TL3_LO_OPT_ST2		(2)
#define TL3_HI_OPT_ST2		(2)

#define TL3_NO_OPT_AX_MAX	(39)
#define TL3_LO_OPT_AX_MAX	(-2047)
#define TL3_HI_OPT_AX_MAX	(2046)

#define TL3_NO_OPT_AY_MAX	(40)
#define TL3_LO_OPT_AY_MAX	(-2047)
#define TL3_HI_OPT_AY_MAX	(2046)

#define TL3_NO_OPT_AZ_MAX	(41)
#define TL3_LO_OPT_AZ_MAX	(-2047)
#define TL3_HI_OPT_AZ_MAX	(2046)

#define TL3_NO_OPT_AX_MIN	(42)
#define TL3_LO_OPT_AX_MIN	(-2047)
#define TL3_HI_OPT_AX_MIN	(2046)

#define TL3_NO_OPT_AY_MIN	(43)
#define TL3_LO_OPT_AY_MIN	(-2047)
#define TL3_HI_OPT_AY_MIN	(2046)

#define TL3_NO_OPT_AZ_MIN	(44)
#define TL3_LO_OPT_AZ_MIN	(-2047)
#define TL3_HI_OPT_AZ_MIN	(2046)

#define TL3_NO_OPT_AX_VAR	(45)
#define TL3_LO_OPT_AX_VAR	(-2047)
#define TL3_HI_OPT_AX_VAR	(2046)

#define TL3_NO_OPT_AY_VAR	(46)
#define TL3_LO_OPT_AY_VAR	(-2047)
#define TL3_HI_OPT_AY_VAR	(2046)

#define TL3_NO_OPT_AZ_VAR	(47)
#define TL3_LO_OPT_AZ_VAR	(-2047)
#define TL3_HI_OPT_AZ_VAR	(2046)

#endif
