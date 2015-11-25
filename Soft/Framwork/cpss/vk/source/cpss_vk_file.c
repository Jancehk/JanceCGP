/*==============================================================================
 *
 *       Filename:  cpss_vk_file.c
 *
 *    Description:  文件操作
 *
 *        Version:  1.0
 *        Created:  2011年05月28日 22时17分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ============================================================================*/
#include "cpss_common.h"
#include "cpss_vk_file.h"
#include "cpss_vk_print.h"
#include "cpss_public.h"
#if (OS_TYPE == OS_TYPE_WINDOW)
#include <direct.h>
#elif (OS_TYPE == OS_TYPE_LINUX)
#include <unistd.h>
#endif

static VOS_CHAR g_CurrentDir[MAX_PATH]={0};

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_notdir
*  Description:	取消目录
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static VOS_STRING cpss_leftdir(VOS_STRING pszPath)
{
	VOS_STRING szPathTmp = NULL;
#if (OS_TYPE == OS_TYPE_LINUX)
	szPathTmp = strrchr(pszPath,'/');
#elif (OS_TYPE == OS_TYPE_WINDOW)
	szPathTmp = strrchr(pszPath,'\\');
#endif
	if (NULL != szPathTmp)
	{
		*szPathTmp=0;
	}
	return pszPath;
}
/*===  FUNCTION  ===============================================================
*         Name:  cpss_set_current_path
*  Description:	  
*  Input      :	
*  OutPut     :	
*  Return     :    
* =============================================================================*/
VOS_UINT32 cpss_set_current_path (VOS_CHAR * pSelfPath)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR strExePath[MAX_PATH] = {0};
	VOS_Strcpy(strExePath,pSelfPath);
	cpss_leftdir(strExePath);
	if (VOS_OK != chdir(strExePath))
	{
		if (NULL == getcwd(g_CurrentDir, MAX_PATH))
		{
			return ulRet;
		}
	}
	else
	{
		strcpy(g_CurrentDir, strExePath);
	}
	return VOS_OK;
}
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_ip_address
 *  Description:	  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_get_current_path (VOS_CHAR * pSelfPath)
{
	VOS_UINT32 ulRet = VOS_ERR;
	if (NULL == pSelfPath)
	{
		return ulRet;
	}
	if (0 != g_CurrentDir)
	{
		strcpy(pSelfPath,g_CurrentDir);
		return VOS_OK;
	}
	if (NULL == getcwd(pSelfPath, MAX_PATH))
	{
		ulRet = VOS_OK;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_ip_address
 *  Description:	  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_get_ip_address (VOS_CHAR * pstrCPUID, VOS_CHAR * pstrOutput)
{
	VOS_INT32 fd = 0,nSize = 0,result = 0;
	VOS_CHAR szIpAddress[32]={0};
	VOS_CHAR   buffer[MAX_PATH] = {0};
	memset(szIpAddress, 0, sizeof(szIpAddress));
	result = cpss_get_current_path(buffer);
	if (VOS_OK != result)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Get Self Path Error%d",fd);
		return result;
	}
	//strcat(buffer,"\\local.cfg");
	VOS_Strcat(buffer, pstrCPUID);
	fd = open(buffer,O_RDONLY);//O_RDWR|O_CREAT);, S_IREAD | S_IWRITE O_BINARY O_WRONLY
	if (VOS_OK < fd)
	{
		nSize = read(fd, szIpAddress, 31);
		close(fd);
	}
	if (0 == *szIpAddress || 6 >nSize)
	{
		while(0 == result)
		{
			//printf("Please Input Local Ip:");
			VOS_PrintInfo("", CPSS_PRINTF_BUFFER, pstrOutput);
			nSize = scanf("%s",szIpAddress);
			result = get_cpuid_from_str(szIpAddress);
			if (0 == result)
			{
				continue;
			}
		fd = open(buffer,O_WRONLY|O_CREAT,755);
		write(fd,szIpAddress,strlen(szIpAddress));
		close(fd);
		}
	}
	else
	{
		result = get_cpuid_from_str(szIpAddress);
	}
	return result;
}		/* -----  end of function cpss_get_ip_address  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_file_handle
 *  Description:	  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_INT32 cpss_get_file_handle()
{
	return VOS_OK;
}