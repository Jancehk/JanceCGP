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

#define VOS_File_Malloc(ulSize)					VOS_Malloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Calloc(ulSize)					VOS_Calloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Realloc(pstrads,ulSize)		VOS_Realloc((pstrads), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Remset(pstrads)				VOS_Remset((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_MemcatEx(pstrA,pstrB,ulSize)	VOS_MemcatEx((pstrA), (pstrB), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_MemcatEx2(pstrA,pstrB,ulSize)	VOS_MemcatEx2((pstrA), (pstrB), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_MemSize(pstrads)				VOS_Memsize((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Memcls(pstrads)				VOS_Memcls((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Memcat(pstrA,pstrB)			VOS_Memcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_FILE))
#define VOS_File_Free(pstrads)					VOS_Free((pstrads), CPSS_MEM_HEAD_KEY_CPSS_FILE)
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
	VOS_CHAR strExePath[MAX_PATH] = { 0 };
	if (NULL == getcwd(g_CurrentDir, MAX_PATH))
	{
		return ulRet;
	}
	return VOS_OK;
	VOS_Strcpy(strExePath,pSelfPath);
	cpss_leftdir(strExePath);
	if (VOS_OK != chdir(strExePath))
	{
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
/*===  FUNCTION  ===============================================================
*         Name:  cpss_get_file_data
*  Description:  得到文件的内容
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_CHAR* cpss_get_file_data(VOS_CHAR * pstrPath)
{
	VOS_INT32 fd = 0, nSize = 0, result = 0;
	VOS_CHAR		strFileDataTemp[10240] = { 0 };
	VOS_CHAR		buffer[MAX_PATH] = { 0 };
	VOS_CHAR		* pstrFilePath = NULL;
	VOS_CHAR		* pstrFilePath2 = NULL;
	VOS_CHAR *		pstrFileData = NULL;
	VOS_CHAR *		pstrFileDataTmp = NULL;

	result = cpss_get_current_path(buffer);
	if (VOS_OK != result || NULL == pstrPath)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Get Self Path Error%d", fd);
		return NULL;
	}
	//strcat(buffer,"\\local.cfg");
	VOS_Strcat(buffer, "\\Web");
	pstrFilePath = buffer + VOS_Strlen(buffer);
	pstrFilePath2 = pstrPath;
	while (0 != *pstrFilePath2)
	{
		if ('/' == *pstrFilePath2)
		{
			*pstrFilePath = '\\';
		}
		else
		{
			*pstrFilePath = *pstrFilePath2;
		}
		pstrFilePath2++;
		pstrFilePath++;
	}
	fd = open(buffer, O_RDONLY | O_BINARY);//O_RDWR|O_CREAT);, S_IREAD | S_IWRITE  O_WRONLY
	if (VOS_OK > fd)
	{
		VOS_PrintErr(__FILE__, __LINE__, "not found file[%s] Error%d", buffer, fd);
		return NULL;
	}
	VOS_Memset(strFileDataTemp, 0, sizeof(strFileDataTemp));
	while ((nSize = read(fd, strFileDataTemp, sizeof(strFileDataTemp))) && (nSize > 0))
	{
		pstrFileDataTmp = VOS_File_MemcatEx(pstrFileData, strFileDataTemp, nSize);
		if (NULL == pstrFileDataTmp)
		{
			VOS_File_Free(pstrFileData);
			pstrFileData = NULL;
			break;
		}
		pstrFileData = pstrFileDataTmp;
	}
	close(fd);
	return pstrFileData;
}