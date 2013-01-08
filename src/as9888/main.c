/******************************************************************************
 *
 * $Id: main.c 298 2011-08-11 12:24:23Z yamada.rj $
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
#else
#include <sched.h>
#include <pthread.h>
#include <linux/input.h>
#endif

#define ERROR_INITDEVICE		(-1)
#define ERROR_OPTPARSE			(-2)
#define ERROR_ACC_COMPENSATE	(-3)
#define ERROR_RECOVER_REGS		(-4)
#define ERROR_LOAD_PARAMETERS	(-5)
#define ERROR_GETOPEN_STAT		(-6)
#define ERROR_STARTCLONE		(-7)
#define ERROR_GETCLOSE_STAT		(-8)

/* Global variable. See DMTCommon.h file. */
int g_stopRequest = 0;
int g_opmode = 0;
int g_dbgzone = 0;
int g_mainQuit = DMT_FALSE;

/* Static variable. */
static pthread_t s_thread;  /*!< Thread handle */

/*!
 A thread function which is raised when measurement is started.
 @param[in] args A pointer to #AS9888PRMS structure.
 */
static void* thread_main(void* args)
{
	MeasureLoop((AS9888PRMS *) args);
	return ((void*)0);
}

/*!
  Signal handler.  This should be used only in DEBUG mode.
  @param[in] sig Event
 */
static void signal_handler(int sig)
{
	if (sig == SIGINT) {
		LOGE("SIGINT signal");
		g_stopRequest = 1;
		g_mainQuit = DMT_TRUE;
	}
}

/*!
 Starts new thread.
 @return If this function succeeds, the return value is 1. Otherwise,
 the return value is 0.
 @param[in] prms A pointer to #AS9888PRMS structure. This pointer is passed
 to the new thread argument.
 */
static int startClone(AS9888PRMS* prms)
{
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	g_stopRequest = 0;
	if (pthread_create(&s_thread, &attr, thread_main, prms) == 0) {
		return 1;
	} else {
		return 0;
	}
}

/*!
 Output measurement result. If this application is run as ConsoleMode,
 the measurement result is output to console. If this application is run as
 DaemonMode, the measurement result is written to device driver.
 Float value is converted to Q16 format.
 @param[in] prms pointer to #AS9888PRMS structure.
 @param[in] flag This flag shows which data contains the valid data.
 The device driver will report only the valid data to HAL layer.
 */
void Disp_ResultHook(AS9888PRMS* prms, const uint16 flag)
{
	if (g_opmode & OPMODE_CONSOLE) 
	{
		Disp_Result_DR(prms);
	} 
	else 
	{
		int rbuf[YPR_DATA_SIZE] = { 0 };
		rbuf[0] = flag;				/* Data flag */
		rbuf[1] = DISP_CONV_Q16(prms->mfv_avec.u.x);	/* Ax */
		rbuf[2] = DISP_CONV_Q16(prms->mfv_avec.u.y);	/* Ay */
		rbuf[3] = DISP_CONV_Q16(prms->mfv_avec.u.z);	/* Az */
		rbuf[4] = 3;				/* Acc status */
		rbuf[5] = DISP_CONV_Q16(prms->mfv_hvec.u.x);	/* Mx */
		rbuf[6] = DISP_CONV_Q16(prms->mfv_hvec.u.y);	/* My */
		rbuf[7] = DISP_CONV_Q16(prms->mfv_hvec.u.z);	/* Mz */
		rbuf[8] = prms->mi_doeRet;	/* Mag status */
		rbuf[9] = DISP_CONV_Q16(prms->mf_azimuth);
		rbuf[10] = DISP_CONV_Q16(prms->mf_pitch);
		rbuf[11] = DISP_CONV_Q16(prms->mf_roll);
		DMT_SetYPR(rbuf);
	}
}

/*!
 This function parse the option.
 @retval 1 Parse succeeds.
 @retval 0 Parse failed.
 @param[in] argc Argument count
 @param[in] argv Argument vector
 @param[out] layout_patno
 */
int OptParse(
	int		argc,
	char*	argv[],
	AKEC_PATNO*	layout_patno)
{
#ifdef WIN32
	// Static
	*layout_patno = PAT1;
	g_opmode = OPMODE_CONSOLE | OPMODE_FST;
	g_dbgzone = DMTDATA_INIT | DMTDATA_FST | DMTDATA_VEC | \
				DMTDATA_ME | DMTDATA_PE;
				//(DMTDATA_DR | DMTDATA_TM | DMTDATA_PE | DMTDATA_ME | DMTDATA_MO | DMTDATA_PO | DMTDATA_HLPS);
#else
	int		opt;
	char	optVal;

	*layout_patno = PAT_INVALID;

	while ((opt = getopt(argc, argv, "fsm:z:")) != -1) {
		switch(opt){
			case 'm':
				optVal = (char)(optarg[0] - '0');
				if ((PAT1 <= optVal) && (optVal <= PAT8)) {
					*layout_patno = (AKEC_PATNO)optVal;
					DMTDEBUG(DBG_LEVEL2, "%s: Layout=%d\n", __FUNCTION__, optVal);
				}
				break;
			case 'z':
				errno = 0;
				g_dbgzone = (int)strtol(optarg, (char**)NULL, 0);
				DMTDEBUG(DBG_LEVEL2, "%s: Dbg Zone=%d\n", __FUNCTION__, g_dbgzone);
				break;
			case 's':
				g_opmode |= OPMODE_CONSOLE;
				break;
			case 'f':
				g_opmode |= OPMODE_FST;
				break;
			default:
				LOGE("%s: Invalid argument", argv[0]);
				return 0;
		}
	}

	/* If layout is not specified with argument, get parameter from driver */
	if (*layout_patno == PAT_INVALID) {
		char n;
		if (DMT_GetLayout(&n) == DMT_SUCCESS) {
			if ((PAT1 <= n) && (n <= PAT8)) {
				*layout_patno = (AKEC_PATNO)n;
			}
		}
	}
	/* Error */
	if (*layout_patno == PAT_INVALID) {
		LOGE("No layout is specified.");
		return 0;
	}
#endif

	return 1;
}

int main(int argc, char **argv)
{
	AS9888PRMS	prms;
	int			retValue = 0;
	AKEC_PATNO	pat;

	/* Show the version info of this software. */
	Disp_StartMessage();

#if ENABLE_DMTDEBUG
	/* Register signal handler */
	signal(SIGINT, signal_handler);
#endif

	/* Open device driver */
	if(DMT_InitDevice() != DMT_SUCCESS) {
		retValue = ERROR_INITDEVICE;
		goto MAIN_QUIT;
	}

	/* Parse command-line options */
	/* This function calls device driver function to get layout */
	if (OptParse(argc, argv, &pat) == 0) {
		retValue = ERROR_OPTPARSE;
		goto MAIN_QUIT;
	}

	/* Initialize parameters structure. */
	InitAS9888PRMS(&prms);

	/* Put argument to PRMS */
	prms.m_layout = pat;

	/* Factory Shipment Test */
	if (g_opmode & OPMODE_FST){
		if (Acc_Compensate(&prms) != DMT_SUCCESS) {
			retValue = ERROR_ACC_COMPENSATE;
			goto MAIN_QUIT;
		}
	}

	/* Recover previous settings */
	if(RecoverRegisters(&prms) != DMT_SUCCESS) {
		retValue = ERROR_RECOVER_REGS;
		goto MAIN_QUIT;
	}

	/* Here is the Main Loop */
	if (g_opmode & OPMODE_CONSOLE) {
		/*** Console Mode *********************************************/
		while (DMT_TRUE) {
			/* Select operation */
			switch (Menu_Main()) {
			case MODE_AOffsetCal:
				Acc_Compensate(&prms);
				break;
			case MODE_Measure:
				/* Read Parameters from file. */
				if(LoadParameters(&prms) != DMT_SUCCESS) {
					retValue = ERROR_LOAD_PARAMETERS;
					goto MAIN_QUIT;
				}
				/* Reset flag */
				g_stopRequest = 0;
				/* Measurement routine */
				MeasureLoop(&prms);

				/* Write Parameters to file. */
				SaveParameters(&prms);
				SaveImageFile(&prms.m_img);
				break;

			case MODE_Quit:
				goto MAIN_QUIT;
				break;

			default:
				DMTDEBUG(DBG_LEVEL0, "Unknown operation mode.\n");
				break;
			}
		}
	} 
	else {
		/*** Daemon Mode *********************************************/
		while (g_mainQuit == DMT_FALSE) 
		{
			int st = 0;
			/* Wait until device driver is opened. */
			if (DMT_GetOpenStatus(&st) != DMT_SUCCESS) {
				retValue = ERROR_GETOPEN_STAT;
				goto MAIN_QUIT;
			}
			if (st == 0) {
				LOGI("Suspended.");
			} else {
				LOGI("Compass Opened.");
				/* Read Parameters from file. */
				if(LoadParameters(&prms) != DMT_SUCCESS) {
					retValue = ERROR_LOAD_PARAMETERS;
					goto MAIN_QUIT;
				}
				/* Reset flag */
				g_stopRequest = 0;
				/* Start measurement thread. */
				if (startClone(&prms) == 0) {
					retValue = ERROR_STARTCLONE;
					goto MAIN_QUIT;
				}

				/* Wait until device driver is closed. */
				if (DMT_GetCloseStatus(&st) != DMT_SUCCESS) {
					retValue = ERROR_GETCLOSE_STAT;
					g_mainQuit = DMT_TRUE;
				}
				/* Wait thread completion. */
				g_stopRequest = 1;
				pthread_join(s_thread, NULL);
				LOGI("Compass Closed.");

				/* Write Parameters to file. */
				SaveParameters(&prms);
				SaveImageFile(&prms.m_img);
			}
		}
	}

MAIN_QUIT:

	/* Close device driver. */
	DMT_DeinitDevice();
	/* Show the last message. */
	Disp_EndMessage(retValue);

	return retValue;
}


