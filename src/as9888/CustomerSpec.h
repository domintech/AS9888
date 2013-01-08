/******************************************************************************
 *
 * $Id: CustomerSpec.h 313 2011-08-17 08:09:53Z yamada.rj $
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
#ifndef INC_CUSTOMERSPEC_H
#define INC_CUSTOMERSPEC_H

/*******************************************************************************
 User defines parameters.
 ******************************************************************************/
// Parameters for Average
//	The number of magnetic/acceleration data to be averaged.
#define CSPEC_HNAVE_D	4
#define CSPEC_ANAVE_D	16
#define CSPEC_HNAVE_V	2
#define CSPEC_ANAVE_V	8
#define CSPEC_LAYOUT	PAT1
#define CSPEC_FST_AVE	16
#define CSPEC_FST_MODE	FST_MODE_CONTINUE

#ifdef WIN32
#define CSPEC_SETTING_FILE	"dmtd.txt"
#define CSPEC_IMAGE_FILE "as9888.img"
#else
#define CSPEC_SETTING_FILE	"/data/misc/dmtd.txt"
#define CSPEC_IMAGE_FILE "/data/misc/as9888.img"
#endif

#define CSPEC_AXGA_PE16	0x00
#define CSPEC_AYGA_PE32	0x00
#define CSPEC_AZGA_PE64	0x00
#define CSPEC_PESE		0x03
#define CSPEC_MESE1		0x0D
#define CSPEC_MESE2		0x18
#define CSPEC_MESE3		0x1E
#define CSPEC_MESE4		0x25
#define CSPEC_MESE5		0x01
#define CSPEC_MOSE1		0x10
#define CSPEC_MOSE2		0x2E
#define CSPEC_MOSE3		0x66
#define CSPEC_MOSE4		0x04
#define CSPEC_TPSE1		0x40
#define CSPEC_TPSE2		0x10
#define CSPEC_TPSE3		0x1C
#define CSPEC_TPSE4		0x21
#define CSPEC_POSE1		0x12
#define CSPEC_POSE2		0x05
#define CSPEC_HLXB1		0x00
#define CSPEC_HLYB1		0x00
#define CSPEC_HLZB1		0x20
#define CSPEC_HLSE1		0x23
#define CSPEC_HLTH1		0x17
#define CSPEC_HLXB2		0x00
#define CSPEC_HLYB2		0x00
#define CSPEC_HLZB2		0xE0
#define CSPEC_HLSE2		0x23
#define CSPEC_HLTH2		0x17
#define CSPEC_PSSE		0x5D
#define CSPEC_INT1EN	0xDF
#define CSPEC_INT2EN	0x00
#define CSPEC_SLCT1		0xDC	/* PEEN=1, MEEN=1, MOEN=1, TPEN=0, POEN=1, HLEN=1 */
#define CSPEC_SLCT2		0x06	/* PSEN=0, TMEN=1, HEEN=1, LFEN=0 */

#endif

