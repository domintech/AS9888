/******************************************************************************
 *
 * $Id: DMTCommon.h 278 2011-08-01 09:07:36Z yamada.rj $
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
#ifndef DMTD_INC_DMTCOMMON_H
#define DMTD_INC_DMTCOMMON_H

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif    					

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <crtdbg.h>
#include "Android.h"

#define DBG_LEVEL	DBG_LEVEL4
#define ENABLE_DMTDEBUG	1
#define OUTPUT_STDOUT	1

#else
#include <stdio.h>     //frpintf
#include <stdlib.h>    //atoi
#include <string.h>    //memset
#include <unistd.h>
#include <stdarg.h>    //va_list
#include <utils/Log.h> //LOGV
#include <errno.h>     //errno

#endif

/*** Constant definition ******************************************************/
#undef LOG_TAG
#define LOG_TAG "DMT_Daemon"

#define DBG_LEVEL0	0	// Critical
#define DBG_LEVEL1	1	// Notice
#define DBG_LEVEL2	2	// Information
#define DBG_LEVEL3	3	// Debug
#define DBG_LEVEL4	4	// Verbose

#ifndef DBG_LEVEL
#define DBG_LEVEL	DBG_LEVEL0
#endif

#define DATA_AREA01	0x0001
#define DATA_AREA02	0x0002
#define DATA_AREA03	0x0004
#define DATA_AREA04	0x0008
#define DATA_AREA05	0x0010
#define DATA_AREA06	0x0020
#define DATA_AREA07	0x0040
#define DATA_AREA08	0x0080
#define DATA_AREA09	0x0100
#define DATA_AREA10	0x0200
#define DATA_AREA11	0x0400
#define DATA_AREA12	0x0800
#define DATA_AREA13	0x1000
#define DATA_AREA14	0x2000
#define DATA_AREA15	0x4000
#define DATA_AREA16	0x8000


/* Debug area definition */
#define DMTDATA_DR			DATA_AREA01
#define DMTDATA_TM			DATA_AREA02
#define DMTDATA_PE			DATA_AREA03
#define DMTDATA_ME			DATA_AREA04
#define DMTDATA_MO			DATA_AREA05
#define DMTDATA_PO			DATA_AREA07
#define DMTDATA_HLPS		DATA_AREA08
#define DMTDATA_INIT		DATA_AREA09	/*<! Initial parameter */
#define DMTDATA_VEC			DATA_AREA10
#define DMTDATA_FST			DATA_AREA11
#define DMTDATA_MAGDRV		DATA_AREA12	/*<! Driver data */


#ifndef ENABLE_DMTDEBUG
#define ENABLE_DMTDEBUG		0	/* Eanble debug output when it is 1. */
#endif

#ifndef OUTPUT_STDOUT
#define OUTPUT_STDOUT		0	/* Output to stdout when it is 1. */
#endif

#define OPMODE_CONSOLE		0x01
#define OPMODE_FST			0x02

/***** Debug output ******************************************/
#if ENABLE_DMTDEBUG
#if OUTPUT_STDOUT
#define DMTDEBUG(level, format, ...) \
    (((level) <= DBG_LEVEL) \
	  ? (fprintf(stdout, (format), ##__VA_ARGS__)) \
	  : ((void)0))
#else
#define DMTDEBUG(level, format, ...) \
	LOGD_IF(((level) <= DBG_LEVEL), (format), ##__VA_ARGS__)
#endif
#else
#define DMTDEBUG(level, format, ...)
#endif

/***** Dbg Area Output ***************************************/
#if ENABLE_DMTDEBUG
#define DMTDATA(flag, format, ...)  \
	((((int)flag) & g_dbgzone) \
	  ? (fprintf(stdout, (format), ##__VA_ARGS__)) \
	  : ((void)0))
#else
#define DMTDATA(flag, format, ...)
#endif

/***** Data output *******************************************/
#if OUTPUT_STDOUT
#define DMTDUMP(format, ...) \
	fprintf(stderr, (format), ##__VA_ARGS__)
#else
#define DMTDUMP(format, ...) \
	LOGD((format), ##__VA_ARGS__)
#endif


/***** Error output *******************************************/
#define DMTERROR \
	((g_opmode == 0) \
	  ? (LOGE("%s:%d Error.", __FUNCTION__, __LINE__)) \
	  : (fprintf(stderr, "%s:%d Error.\n", __FUNCTION__, __LINE__)))

#define DMTERROR_STR(api) \
	((g_opmode == 0) \
	  ? (LOGE("%s:%d %s Error (%s).", \
	  		  __FUNCTION__, __LINE__, (api), strerror(errno))) \
	  : (fprintf(stderr, "%s:%d %s Error (%s).\n", \
	  		  __FUNCTION__, __LINE__, (api), strerror(errno))))

/*** Type declaration *********************************************************/

/*** Global variables *********************************************************/
extern int g_stopRequest;	/*!< 0:Not stop,  1:Stop */
extern int g_opmode;		/*!< 0:Daemon mode, 1:Console mode. */
extern int g_dbgzone;		/*!< Debug zone. */

/*** Prototype of function ****************************************************/

#endif //DMTD_INC_DMTCOMMON_H



