/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.c
 *
 *    Description:  内存管理模块
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时34分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#include "cpss_vos_mem.h"
static CPSS_MSG_MEM_MANAGE g_cpssMem_Manage;
typedef enum _MEM_RECORD_MGR_TYPE_M_
{
	MEM_RECORD_MGR_INSERTF = 0x01,
	MEM_RECORD_MGR_INSERTU,
	MEM_RECORD_MGR_REMOVEF,
	MEM_RECORD_MGR_REMOVEU,
	MEM_RECORD_MGR_GETFREE,
	MEM_RECORD_MGR_CHECK,
};
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_print_record_info
*  Description:  打印空的内存管理记录器
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOID cpss_mem_print_record_info(VOS_CHAR* pstrKey, VOS_VOID* pstuVoid)
{
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	VOS_UINT32 nIndex = 0,nTimes = 0;
	FILE * hFile = NULL;
	hFile = fopen("D:\\aa.txt","a+");
	pstuMemRecord = g_cpssMem_Manage.stuMemFHeadList.head;
	fprintf(hFile, "find %s --> %p\r\n", pstrKey,pstuVoid);
	fprintf(hFile, "ToTal:%d Free Count:%d\r\n", g_cpssMem_Manage.nTotalCount, g_cpssMem_Manage.stuMemFHeadList.nTotalCount);
	while (NULL != pstuMemRecord)
	{
		fprintf(hFile, "%04d S:%02d,S:%p P:%p N:%p%s[%d]\r\n", ++nTimes, pstuMemRecord->nState, pstuMemRecord, pstuMemRecord->prev, pstuMemRecord->next, pstuMemRecord->strFileName, pstuMemRecord->nFileLine);
		pstuMemRecord = pstuMemRecord->next;
	}
	for (nIndex = 0; nIndex < CPSS_MEM_HEAD_KEY_CPSS_TOTAL; nIndex++)
	{
		pstuMemRecord = g_cpssMem_Manage.stuMemUHeadList[nIndex].head;
		fprintf(hFile, "%d Count:%d\r\n", nIndex, g_cpssMem_Manage.stuMemUHeadList[nIndex].nTotalCount);
		nTimes = 0;
		while (NULL != pstuMemRecord)
		{
			fprintf(hFile, "%04d S:%02d,S:%p P:%p N:%p :%p:%s %s[%d]\r\n", ++nTimes, pstuMemRecord->nState, pstuMemRecord, pstuMemRecord->prev, pstuMemRecord->next, 
				pstuMemRecord->pstrVoid, pstuMemRecord->pstrVoid, pstuMemRecord->strFileName, pstuMemRecord->nFileLine);
			pstuMemRecord = pstuMemRecord->next;
		}
	}
	fclose(hFile);

}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_record_manager
*  Description:  管理内存管理记录器
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 cpss_mem_record_manager(VOS_UINT32 nMemRdKey, CPSS_MEM_RECORD **recode, VOS_UINT32 nType)
{
	VOS_INT32 uRet = VOS_ERR;
	CPSS_MEM_RECORD ** head = NULL;
	CPSS_MEM_RECORD ** tail = NULL;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;

	if (NULL == recode)
	{
		VOS_PrintErr(__FILE__, __LINE__, "mem record manager param is error");
		return uRet;
	}
	if (VOS_OK != VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return uRet;
	}
	switch (nType)
	{
	case MEM_RECORD_MGR_INSERTF:
	case MEM_RECORD_MGR_INSERTU:
		pstuMemRecord = *recode;
		if (NULL == pstuMemRecord || NULL != pstuMemRecord->next || NULL != pstuMemRecord->prev)
		{
			goto EXIT_ERR;
		}
		if (nType == MEM_RECORD_MGR_INSERTF)
		{
			head = &g_cpssMem_Manage.stuMemFHeadList.head;
			tail = &g_cpssMem_Manage.stuMemFHeadList.tail;
		}
		if (nType == MEM_RECORD_MGR_INSERTU)
		{
			if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
			{
				goto EXIT_ERR;
			}
			head = &g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head;
			tail = &g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail;
		}
		if (NULL == *head &&
			NULL == *tail)
		{
			*head = pstuMemRecord;
			*tail = pstuMemRecord;
		}
		else if (NULL != *head &&
				 NULL != *tail)
		{
			((CPSS_MEM_RECORD*)*tail)->next = pstuMemRecord;
			pstuMemRecord->prev = (CPSS_MEM_RECORD*)*tail;
			(CPSS_MEM_RECORD*)*tail = pstuMemRecord;
		}
		else
		{
			goto EXIT_ERR;
		}
		if (nType == MEM_RECORD_MGR_INSERTF)
		{
			if (CPSS_MEM_HEAD_KEY_CPSS_TOTAL < nMemRdKey)
			{
				g_cpssMem_Manage.nTotalCount++;
			}
			g_cpssMem_Manage.stuMemFHeadList.nTotalCount++;
			pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_FREE;
			//cpss_mem_print_record_info("free insert", pstuMemRecord);
		}else
		if (nType == MEM_RECORD_MGR_INSERTU)
		{
			g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].nTotalCount++;
			g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].uMemSize += pstuMemRecord->nSize;
			g_cpssMem_Manage.uMemSize += pstuMemRecord->nSize;
			pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_USED;
			//cpss_mem_print_record_info("used insert", pstuMemRecord);
		}
		break;
	case MEM_RECORD_MGR_REMOVEF:
	case MEM_RECORD_MGR_REMOVEU:
		pstuMemRecord = *recode;
		if (NULL == pstuMemRecord )
		{
			goto EXIT_ERR;
		}
		if (nType == MEM_RECORD_MGR_REMOVEF)
		{
			head = &g_cpssMem_Manage.stuMemFHeadList.head;
			tail = &g_cpssMem_Manage.stuMemFHeadList.tail;
		}
		if (nType == MEM_RECORD_MGR_REMOVEU)
		{
			if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
			{
				goto EXIT_ERR;
			}
			head = &g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head;
			tail = &g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail;
		}
		if (*head == *tail)
		{
			if (pstuMemRecord != *head)
			{
				goto EXIT_ERR;
			}
			(CPSS_MEM_RECORD*)*head = NULL;
			(CPSS_MEM_RECORD*)*tail = NULL;
		}
		else if (NULL == pstuMemRecord->prev && NULL != pstuMemRecord->next)
		{
			if (pstuMemRecord != *head)
			{
				goto EXIT_ERR;
			}
			(CPSS_MEM_RECORD*)*head = pstuMemRecord->next;
			((CPSS_MEM_RECORD*)*head)->prev = NULL;
			pstuMemRecord->next = NULL;
		}
		else if (NULL != pstuMemRecord->prev && NULL == pstuMemRecord->next)
		{
			if (pstuMemRecord != *tail)
			{
				goto EXIT_ERR;
			}
			(CPSS_MEM_RECORD*)*tail = pstuMemRecord->prev;
			((CPSS_MEM_RECORD*)*tail)->next = NULL;
			pstuMemRecord->prev = NULL;
		}
		else if (NULL != pstuMemRecord->prev && NULL != pstuMemRecord->next)
		{
			pstuMemRecord->prev->next = pstuMemRecord->next;
			pstuMemRecord->next->prev = pstuMemRecord->prev;
		}
		else
		{
			goto EXIT_ERR;
		}
		if (nType == MEM_RECORD_MGR_REMOVEF)
		{
			g_cpssMem_Manage.stuMemFHeadList.nTotalCount--;
			//cpss_mem_print_record_info("free remove", pstuMemRecord);
		}
		else if (nType == MEM_RECORD_MGR_REMOVEU)
		{
			g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].nTotalCount--;
			g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].uMemSize -= pstuMemRecord->nSize;
			g_cpssMem_Manage.uMemSize -= pstuMemRecord->nSize;
			//cpss_mem_print_record_info("used reemove", pstuMemRecord);
			if (NULL != pstuMemRecord->pstrVoid)
			{
				free(pstuMemRecord->pstrVoid);
			}
			BZERO(pstuMemRecord, sizeof(CPSS_MEM_RECORD));
		}
		pstuMemRecord->next = NULL;
		pstuMemRecord->prev = NULL;
		pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_RESE;
		break;
	case MEM_RECORD_MGR_GETFREE:
		if (NULL != *recode)
		{
			goto EXIT_ERR;
		}
		pstuMemRecord = g_cpssMem_Manage.stuMemFHeadList.head;
		while (NULL != pstuMemRecord)
		{
			if (CPSS_MEM_RECORD_STAT_FREE == pstuMemRecord->nState)
			{
				pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_RESE;
				break;
			}
			pstuMemRecord = pstuMemRecord->next;
		}
		*recode = pstuMemRecord;
		break;
	default:
		VOS_PrintErr(__FILE__, __LINE__, "mem record manager type is error[%d]",nType);
		goto EXIT_ERR;
		break;
	}
	uRet = VOS_OK;
EXIT_OK:
	if (VOS_OK != VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "del lock error");
		return uRet;
	}
	return uRet;
EXIT_ERR:
	goto EXIT_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_get_record_info
*  Description:  得到空的内存管理记录器
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static CPSS_MEM_RECORD *cpss_mem_get_record_info()
{
	VOS_INT32 uRet = 0;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
FREE_RECODE_CHECK:
	pstuMemRecord = NULL;
	uRet = cpss_mem_record_manager(0, &pstuMemRecord, MEM_RECORD_MGR_GETFREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
		return NULL;
	}
	if (NULL == pstuMemRecord)
	{
		pstuMemRecord = (pCPSS_MEM_RECORD)malloc(sizeof(CPSS_MEM_RECORD));
		if (NULL == pstuMemRecord)
		{
			VOS_PrintErr(__FILE__, __LINE__, "malloc error");
			return NULL;
		}
		BZERO(pstuMemRecord, sizeof(CPSS_MEM_RECORD));
		uRet = cpss_mem_record_manager(CPSS_MEM_HEAD_KEY_CPSS_TOTAL+3, &pstuMemRecord, MEM_RECORD_MGR_INSERTF);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
			return NULL;
		}
		goto FREE_RECODE_CHECK;
	}
	return pstuMemRecord;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_get_record_info
*  Description:  得到空的内存管理记录器
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static CPSS_MEM_RECORD *cpss_mem_find_record_info(VOS_UINT32 nMemRdKey, VOS_VOID* pstuVoid)
{
	VOS_UINT32 uRet = VOS_ERR;
	int nIndex = 0;
	CPSS_MEM_RECORD * pstuMemRecord = g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head;
	
	if (VOS_OK != VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return NULL;
	}
	while (NULL != pstuMemRecord)
	{
		nIndex++;
		if (pstuMemRecord->pstrVoid == pstuVoid)
		{
			break;
		}
		pstuMemRecord = pstuMemRecord->next;
	}
	if (NULL == pstuMemRecord)
	{
		cpss_mem_print_record_info("check list", pstuVoid);
		for (nIndex = 0; nIndex < CPSS_MEM_HEAD_KEY_CPSS_TOTAL; nIndex++)
		{
			if (nIndex == nMemRdKey)
			{
				continue;
			}
			pstuMemRecord = g_cpssMem_Manage.stuMemUHeadList[nIndex].head;
			while (NULL != pstuMemRecord)
			{
				if (pstuMemRecord->pstrVoid == pstuVoid)
				{
					break;
				}
				pstuMemRecord = pstuMemRecord->next;
			}
			if (NULL != pstuMemRecord)
			{
				break;
			}
		}
	}
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "del lock error");
	}

	return pstuMemRecord;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_move_to_used
*  Description:  将内存管理记录器移动到使用中
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_INT32 cpss_mem_move_to_used(VOS_UINT32 nMemRdKey, CPSS_MEM_RECORD * pstuMemRecord)
{
	VOS_INT32 uRet = VOS_ERR;

	uRet = cpss_mem_record_manager(nMemRdKey, &pstuMemRecord, MEM_RECORD_MGR_REMOVEF);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
		return uRet;
	}
	uRet = cpss_mem_record_manager(nMemRdKey, &pstuMemRecord, MEM_RECORD_MGR_INSERTU);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
		return uRet;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_mem_move_to_used
*  Description:  将内存管理记录器移动到使用中
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_INT32 cpss_mem_move_to_free(VOS_UINT32 nMemRdKey, CPSS_MEM_RECORD * pstuMemRecord)
{
	VOS_INT32 uRet = VOS_OK;

	uRet = cpss_mem_record_manager(nMemRdKey, &pstuMemRecord, MEM_RECORD_MGR_REMOVEU);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
		return uRet;
	}
	uRet = cpss_mem_record_manager(nMemRdKey, &pstuMemRecord, MEM_RECORD_MGR_INSERTF);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free recode is error");
		return uRet;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_init_mem
 *  Description:  平台初始化内存管理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_init_mem()
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = VOS_Mutex_Init(&g_cpssMem_Manage.hMutex, "CPSS_INIT_MEM");
	if (VOS_OK != uRet)
	{
		//VOS_PrintErr(__FILE__,__LINE__,"cpss init mem mutex error");
		printf("cpss init mem mutex error\n");
		return uRet;
	}
	g_cpssMem_Manage.uMemSize = 0;
	g_cpssMem_Manage.nTotalCount = 0;
	BZERO(&g_cpssMem_Manage.stuMemFHeadList, sizeof(CPSS_MEM_RECORD_HEAD)*CPSS_MEM_HEAD_KEY_CPSS_TOTAL);
	return uRet;
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_mem_malloc
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_VOID * cpss_mem_malloc(VOS_INT32 ulSize,
						   VOS_UINT32 nMemRdKey, 
						   VOS_CHAR * strFile, 
						   VOS_INT32 nLine)
{
	VOS_INT32 uRet = 0;
	VOS_INT32 ulFileNameLen = 0;
	VOS_VOID * uRetVoid = NULL;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;

	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return NULL;
	}
	//VOS_PrintInfo(__FILE__,__LINE__,"calloc size memory %d",ulSize);
//	printf("calloc size memory :%d:%s\n",ulSize,strInfo);
	uRetVoid = malloc(ulSize);
	//printf("==:%d:===\n",g_cpssMem_Manage.uMemSize);
	if (NULL == uRetVoid)
	{
		VOS_PrintInfo(__FILE__,__LINE__,"now memory is:%d, calloc size memory :%d:%d:ErrCode:%d\n",
			g_cpssMem_Manage.uMemSize, ulSize, nMemRdKey, GetLastError());
		return NULL;
	}
	pstuMemRecord = cpss_mem_get_record_info();
	if (NULL == pstuMemRecord)
	{
		cpss_mem_move_to_free(nMemRdKey, pstuMemRecord);
		return NULL;
	}
	pstuMemRecord->nSize = ulSize;
	pstuMemRecord->pstrVoid = uRetVoid;
	pstuMemRecord->nFileLine = nLine;
	ulFileNameLen = strlen(strFile)-30;
	if (ulFileNameLen <= 0)
	{
		ulFileNameLen = 0;
	}
	strcpy(pstuMemRecord->strFileName, strFile + ulFileNameLen);
	if (VOS_OK != cpss_mem_move_to_used(nMemRdKey, pstuMemRecord))
	{
		if (VOS_OK != cpss_mem_move_to_free(nMemRdKey, pstuMemRecord))
		{
			free(uRetVoid);
		}
		uRetVoid = NULL;
	}
	return uRetVoid;
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_reset
*  Description: 空间从新申请  
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_VOID * cpss_mem_realloc(VOS_UINT32 nMemRdKey, void * vAdress, VOS_INT32 ulSize, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_INT32 ulFileNameLen = 0;
	VOS_CHAR * pstrBuffer = NULL;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return NULL;
	}
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	pstrBuffer = realloc(vAdress, ulSize);
	if (NULL == pstrBuffer)
	{
		//cpss_mem_move_to_free(nMemRdKey, pstuMemRecord);
		return NULL;
	}
	pstuMemRecord->pstrVoid = pstrBuffer;
	pstuMemRecord->nFileLine = nLine;
	ulFileNameLen = strlen(strFile) - 30;
	if (ulFileNameLen <= 0)
	{
		ulFileNameLen = 0;
	}
	strcpy(pstuMemRecord->strFileName, strFile + ulFileNameLen);
	return pstrBuffer;
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_reset
*  Description:  设置内存管理信息
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_VOID * cpss_mem_reset(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_INT32 ulFileNameLen = 0;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return NULL;
	}
	//cpss_mem_print_record_info(vAdress);
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	if (NULL == pstuMemRecord)
	{
		return NULL;
	}
	pstuMemRecord->nFileLine = nLine;
	ulFileNameLen = strlen(strFile) - 30;
	if (ulFileNameLen <= 0)
	{
		ulFileNameLen = 0;
	}
	strcpy(pstuMemRecord->strFileName, strFile + ulFileNameLen);
	return vAdress;
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_cat
*  Description:  设置内存管理信息
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_VOID * cpss_mem_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_UINT32			nMemsize = 0;
	CPSS_MEM_RECORD		*pstuMemRecordA = NULL;
	CPSS_MEM_RECORD		*pstuMemRecordB = NULL;
	VOS_CHAR			*pstrTmp = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return NULL;
	}
	pstuMemRecordA = cpss_mem_find_record_info(nMemRdKey, vAdressA);
	pstuMemRecordB = cpss_mem_find_record_info(nMemRdKey, vAdressB);
	if (NULL == pstuMemRecordB)
	{
		return NULL;
	}
	if (NULL == pstuMemRecordA)
	{
		pstrTmp = cpss_mem_malloc(nMemRdKey, pstuMemRecordB->nSize, strFile, nLine);
	}
	else
	{
		nMemsize = pstuMemRecordA->nSize;
		pstrTmp = cpss_mem_realloc(nMemRdKey, vAdressA, nMemsize + pstuMemRecordB->nSize, __FILE__, __LINE__);
	}
	if (NULL == pstrTmp)
	{
		return NULL;
	}
	memcpy(pstrTmp + nMemsize, vAdressB, pstuMemRecordB->nSize);
	return pstrTmp;
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_getsize
*  Description:  得到memory的大小
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_UINT32 cpss_mem_getsize(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return 0;
	}
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	if (NULL == pstuMemRecord)
	{
		return 0;
	}
	return pstuMemRecord->nSize;
}

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_mem_free
 *  Description:  释放内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_mem_free(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	if (NULL == vAdress)
	{
		VOS_PrintWarn(strFile, nLine, "free address is NULL",
			nMemRdKey, vAdress);
		return uRet;
	}
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		free(vAdress);
		vAdress = NULL;
		VOS_PrintInfo(__FILE__, __LINE__, "free memory key is error key:%d %x\n",
			nMemRdKey, vAdress);
		return uRet;
	}
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	if (NULL == pstuMemRecord)
	{
		VOS_PrintErr(__FILE__, __LINE__, "free memory find is error key:%d %p\n",
			nMemRdKey, vAdress);
		return uRet;
	}
	if (VOS_OK != cpss_mem_move_to_free(nMemRdKey, pstuMemRecord))
	{
		VOS_PrintInfo(__FILE__, __LINE__, "free memory find is error key:%d %x\n",
			nMemRdKey, vAdress);
		return uRet;
	}
	return VOS_OK;
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_getsize
*  Description:  得到memory的大小
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
VOS_UINT32 cpss_mem_cls(VOS_UINT32 nMemRdKey, void * vAdress, VOS_UINT32 nMemSize, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	CPSS_MEM_RECORD * pstuMemRecord = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return 0;
	}
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	if (NULL == pstuMemRecord)
	{
		VOS_PrintErr(strFile, nLine, "memory key [%d] address %p", nMemRdKey, vAdress);
		return 0;
	}
	if (nMemSize > pstuMemRecord->nSize)
	{
		VOS_PrintErr(strFile, nLine, "memory key [%d] address %p size not correct %d:%d", nMemRdKey, vAdress, nMemSize , pstuMemRecord->nSize);
	}
	BZERO(vAdress, pstuMemRecord->nSize);
	return pstuMemRecord->nSize;
}
/*===  FUNCTION  ==============================================================
*         Name:  cpss_str_cat
*  Description:  字符串链接
*  Input      :
*  OutPut     :
*  Return     : 拷贝后的字符串长度
* =============================================================================*/
VOS_CHAR* cpss_str_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_UINT32			nMemsize = 0;
	VOS_UINT32			ulSizeA = 0;
	VOS_UINT32			ulSizeB = 0;
	CPSS_MEM_RECORD		*pstuMemRecordA = NULL;
	CPSS_MEM_RECORD		*pstuMemRecordB = NULL;
	VOS_CHAR			*pstrTmp = NULL;
	if (nMemRdKey >= CPSS_MEM_HEAD_KEY_CPSS_TOTAL)
	{
		return NULL;
	}
	pstuMemRecordA = cpss_mem_find_record_info(nMemRdKey, vAdressA);
	if (NULL == pstuMemRecordA)
	{
		return NULL;
	}
	pstuMemRecordB = cpss_mem_find_record_info(nMemRdKey, vAdressB);
	if (NULL == pstuMemRecordB)
	{
		return NULL;
	}
	nMemsize = pstuMemRecordA->nSize;
	ulSizeA = VOS_Strlen(vAdressA);
	ulSizeB = VOS_Strlen(vAdressB);
	pstrTmp = vAdressA;
	if (nMemsize < ulSizeA + ulSizeB)
	{
		pstrTmp = cpss_mem_realloc(nMemRdKey, vAdressA, pstuMemRecordB->nSize, __FILE__, __LINE__);
		if (NULL == pstrTmp)
		{
			return NULL;
		}
	}
	VOS_Strcat(pstrTmp, vAdressB);
	return pstrTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cps_uninit_mem
 *  Description:  平台卸载内存管理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_VOID cpss_uninit_mem()
{
	VOS_Mutex_Destroy(&g_cpssMem_Manage.hMutex);
}

/*===  FUNCTION  ==============================================================
*         Name:  cpss_get_print_mem_info
*  Description:  打印内存空间
*  Input      :	
*  OutPut     :	
*  Return     :    
* =============================================================================*/
VOS_VOID cpss_print_mem_info(VOS_STRING pstrStat)
{
	VOS_CHAR *pstrBuffer = NULL;

	VOS_PrintInfo(pstrBuffer, "=====%s=====\nMemory Size:%d Free Count:%d\n",
		pstrStat,
		g_cpssMem_Manage.uMemSize,
		g_cpssMem_Manage.nTotalCount);
	//cpss_print_mem_data(pstrBuffer+VOS_Strlen(pstrBuffer), pstuFreeBufTmp);
	VOS_PrintInfo(__FILE__, __LINE__,"Print Buffer Info\n%s",pstrBuffer);
}
