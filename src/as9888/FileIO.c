/******************************************************************************
 *
 * $Id: FileIO.c 298 2011-08-11 12:24:23Z yamada.rj $
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
#include "FileIO.h"

/*!
 Load parameters from file which is specified with #CSPEC_SETTING_FILE.
 This function reads data from a beginning of the file line by line, and
 check parameter name sequentially. In otherword, this function depends on
 the order of eache parameter described in the file.
 @return If function fails, the return value is #DMT_FAIL. When function fails,
 the output is undefined. Therefore, parameters which are possibly overwritten
 by this function should be initialized again. If function succeeds, the
 return value is #DMT_SUCCESS.
 @param[out] prms A pointer to #AS9888PRMS structure. Loaded parameter is
 stored to the member of this structure.
 */
int16 LoadParameters(AS9888PRMS * prms)
{
	int16 ret;
	FILE *fp = NULL;

	//Open setting file for read.
	if ((fp = fopen(CSPEC_SETTING_FILE, "r")) == NULL) 
	{
		DMTERROR_STR("fopen");
		return DMT_FAIL;
	}

	ret = 1;

	/* Load data to HO */
	ret = ret && LoadFloatVec(fp, "HO", &prms->mfv_HO);
	/* Load FST_COMP */
	ret = ret && LoadFloat(fp, "t9eRt", &prms->m_cmp.t9eRt);
	ret = ret && LoadFloat(fp, "t9eLv", &prms->m_cmp.t9eLv);
	ret = ret && LoadFloatVec(fp, "kst", &prms->m_cmp.kst);
	ret = ret && LoadFloatVec(fp, "st", &prms->m_cmp.st);
	ret = ret && LoadFloatVec(fp, "kot", &prms->m_cmp.kot);
	ret = ret && LoadFloatVec(fp, "ot", &prms->m_cmp.ot);

	if (fclose(fp) != 0) 
	{
		DMTERROR_STR("fclose");
		ret = 0;
	}

	if (ret == 0) 
	{
		DMTERROR_STR(__FUNCTION__);
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Load \c float type value from file. The name of parameter is specified with
 \a lpKeyName. If the name matches the beginning of read line, the string
 after #DELIMITER will converted to \c float type value.
 @return If function fails, the return value is 0. When function fails, the
 value @ val is not overwritten. If function succeeds, the return value is 1.
 @param[in] fp Pointer to \c FILE structure.
 @param[in] lpKeyName The name of parameter.
 @param[out] val Pointer to \c float type value. Upon successful completion
 of this function, read value is copied to this variable.
 */
int16 LoadFloat(FILE *fp, const char *lpKeyName, AKFLOAT *val)
{
	char buf[64] = { '\0' };

	// ATTENTION! %ns should be modified according to the size of buf.
#ifdef AKEC_PRECISION_DOUBLE
	if (fscanf(fp, "%63s" DELIMITER "%lf", buf, val) != 2) {
#else
	if (fscanf(fp, "%63s" DELIMITER "%f", buf, val) != 2) {
#endif
		LOGE("%s: scanf error.", __FUNCTION__);
		return 0;
	}
	// Compare the read parameter name with given name.
	if (strncmp(buf, lpKeyName, sizeof(buf)) != 0) 
	{
		LOGE("%s: strncmp (%s) error.", __FUNCTION__, lpKeyName);
		return 0;
	}

	return 1;
}

/*!
 Load \c float type value from file and convert it to AKFVEC type
 structure. This function adds ".x", ".y" and ".z" to the last of parameter
 name and try to read value with combined name.
 @return If function fails, the return value is 0. When function fails, the
 output is undefined. If function succeeds, the return value is 1.
 @param[in] fp A opened \c FILE pointer.
 @param[in] lpKeyName The parameter name.
 @param[out] vec A pointer to AKFVEC structure. Upon successful completion
 of this function, read values are copied to this variable.
 */
int16 LoadFloatVec(FILE *fp, const char *lpKeyName, AKFVEC *vec)
{
	char keyName[64];
	int16 ret = 1;

	snprintf(keyName, sizeof(keyName), "%s.x", lpKeyName);
	ret = ret && LoadFloat(fp, keyName, &vec->u.x);

	snprintf(keyName, sizeof(keyName), "%s.y", lpKeyName);
	ret = ret && LoadFloat(fp, keyName, &vec->u.y);

	snprintf(keyName, sizeof(keyName), "%s.z", lpKeyName);
	ret = ret && LoadFloat(fp, keyName, &vec->u.z);

	return ret;
}

/*!
 Load AS9888IMAGE structure from file.
 */
int16 LoadImageFile(AS9888IMAGE	*image)
{
	FILE *fp = NULL;
	char buf[8];
	int16 i;
	int val;

	/* Open setting file for read. */
	if ((fp = fopen(CSPEC_IMAGE_FILE, "r")) == NULL) 
	{
		DMTERROR_STR("fopen");
		return DMT_FAIL;
	}

	for (i=0; i < AS9888_IMAGE_SIZE; i++) 
	{
		if (fscanf(fp, "%7s = %d", buf, &val) != 2) 
		{
			LOGE("%s: scanf error.", __FUNCTION__);
			goto LOAD_IMAGE_ERROR;
		}
		if (atoi(buf) != i) 
		{
			LOGE("%s: invalid index (%d, %s).", __FUNCTION__, i, buf);
			goto LOAD_IMAGE_ERROR;
		}
		if ((val < 0) || (255 < val)) 
		{
			LOGE("%s: invalid value (%d).", __FUNCTION__, val);
			goto LOAD_IMAGE_ERROR;
		}
		image->v[i] = (uint8)(val);
	}

LOAD_IMAGE_ERROR:
	/* Close setting file */
	if (fclose(fp) != 0) 
	{
		DMTERROR_STR("fclose");
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Save parameters to file which is specified with #CSPEC_SETTING_FILE. This
 function saves variables when the offsets of magnetic sensor estimated
 successfully.
 @return If function fails, the return value is #DMT_FAIL. When function fails,
 the parameter file may collapsed. Therefore, the parameters file should be
 discarded. If function succeeds, the return value is #DMT_SUCCESS.
 @param[out] prms A pointer to #AS9888PRMS structure. Member variables are
 saved to the parameter file.
 */
int16 SaveParameters(AS9888PRMS *prms)
{
	int16 ret = 1;
	FILE *fp;

	//Open setting file for write.
	if ((fp = fopen(CSPEC_SETTING_FILE, "w")) == NULL) 
	{
		DMTERROR_STR("fopen");
		return DMT_FAIL;
	}

	/* Save data to HO */
	ret = ret && SaveFloatVec(fp, "HO", &prms->mfv_HO);
	/* Save FST_COMP */
	ret = ret && SaveFloat(fp, "t9eRt", prms->m_cmp.t9eRt);
	ret = ret && SaveFloat(fp, "t9eLv", prms->m_cmp.t9eLv);
	ret = ret && SaveFloatVec(fp, "kst", &prms->m_cmp.kst);
	ret = ret && SaveFloatVec(fp, "st", &prms->m_cmp.st);
	ret = ret && SaveFloatVec(fp, "kot", &prms->m_cmp.kot);
	ret = ret && SaveFloatVec(fp, "ot", &prms->m_cmp.ot);

	if (fclose(fp) != 0) 
	{
		DMTERROR_STR("fclose");
		ret = 0;
	}

	if (ret == 0) 
	{
		DMTERROR_STR(__FUNCTION__);
		return DMT_FAIL;
	}

	return DMT_SUCCESS;
}

/*!
 Save parameters of float type to file.
 @return If function fails, the return value is 0. When function fails,
 parameter is not saved to file. If function succeeds, the return value is 1.
 @param[in] fp Pointer to \c FILE structure.
 @param[in] lpKeyName The name of paraemter.
 @param[in] val Pointer to \c AKFLOAT type value.
 */
int16 SaveFloat(FILE *fp, const char *lpKeyName, const AKFLOAT val)
{
	if (fprintf(fp, "%s" DELIMITER "%f\n", lpKeyName, val) < 0) 
	{
		LOGE("%s: printf (%s) error.", __FUNCTION__, lpKeyName);
		return 0;
	} 
	else 
	{
		return 1;
	}
}

/*!
 Save parameters of AKFVEC type structure to file. This function adds
 ".x", ".y" and ".z" to the last of parameter name and save value with
 the combined name.
 @return If function fails, the return value is 0. When function fails, not
 all parameters are saved to file, i.e. parameters file may collapsed.
 If function succeeds, the return value is 1.
 @param[in] fp Pointer to \c FILE structure.
 @param[in] lpKeyName The name of paraemter.
 @param[in] vec Pointer to \c AKFVEC type structure.
 */
int16 SaveFloatVec(FILE *fp, const char *lpKeyName, const AKFVEC *vec)
{
	int16 ret = 0;
	char keyName[64];

	ret = 1;
	snprintf(keyName, sizeof(keyName), "%s.x", lpKeyName);
	ret = ret && SaveFloat(fp, keyName, vec->u.x);

	snprintf(keyName, sizeof(keyName), "%s.y", lpKeyName);
	ret = ret && SaveFloat(fp, keyName, vec->u.y);

	snprintf(keyName, sizeof(keyName), "%s.z", lpKeyName);
	ret = ret && SaveFloat(fp, keyName, vec->u.z);

	return ret;
}

/*!
 Save AS9888IMAGE structure to file.
 */
int16 SaveImageFile(const AS9888IMAGE *image)
{
	FILE *fp = NULL;
	int16 i;

	/* Open setting file for read. */
	if ((fp = fopen(CSPEC_IMAGE_FILE, "w")) == NULL) 
	{
		DMTERROR_STR("fopen");
		return DMT_FAIL;
	}

	for (i=0; i<AS9888_IMAGE_SIZE; i++) 
	{
		if (fprintf(fp, "%d = %d\n", i, image->v[i]) < 0) {
			LOGE("%s: printf (%d) error.", __FUNCTION__, i);
			goto SAVE_IMAGE_ERROR;
		}
	}

SAVE_IMAGE_ERROR:
	/* Close setting file */
	if (fclose(fp) != 0) 
	{
		DMTERROR_STR("fclose");
		return DMT_FAIL;
	}
	return DMT_SUCCESS;
}

