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
	uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return NULL;
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
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "del lock error");
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
		pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_FREE;
		uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "add lock error");
			return NULL;
		}
		if (NULL == g_cpssMem_Manage.stuMemFHeadList.head &&
			NULL == g_cpssMem_Manage.stuMemFHeadList.tail)
		{
			g_cpssMem_Manage.stuMemFHeadList.head = pstuMemRecord;
			g_cpssMem_Manage.stuMemFHeadList.tail = pstuMemRecord;
			g_cpssMem_Manage.stuMemFHeadList.nTotalCount++;
			pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_RESE;
			g_cpssMem_Manage.nTotalCount++;
		}
		else if (NULL != g_cpssMem_Manage.stuMemFHeadList.tail)
		{
			g_cpssMem_Manage.stuMemFHeadList.tail->next = pstuMemRecord;
			pstuMemRecord->prev = g_cpssMem_Manage.stuMemFHeadList.tail;
			g_cpssMem_Manage.stuMemFHeadList.nTotalCount++;
			g_cpssMem_Manage.stuMemFHeadList.tail = pstuMemRecord;
			pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_RESE;
			g_cpssMem_Manage.nTotalCount++;
		}
		else
		{
			free(pstuMemRecord);
			pstuMemRecord = NULL;
			VOS_PrintErr(__FILE__, __LINE__, "link is error");
		}
		uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "del lock error");
		}
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
	CPSS_MEM_RECORD * pstuMemRecord = g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head;
	
	if (VOS_OK != VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return NULL;
	}
	while (NULL != pstuMemRecord)
	{
		if (pstuMemRecord->pstrVoid == pstuVoid)
		{
			break;
		}
		pstuMemRecord = pstuMemRecord->next;
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
	VOS_INT32 uRet = VOS_OK;

	if (VOS_OK != VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return uRet;
	}
	/*
	if (NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head &&
		NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail)
	{
		pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_FREE;
		goto ERR_EXIT;
	}*/
	if (NULL == pstuMemRecord->next &&
		NULL == pstuMemRecord->prev &&
		g_cpssMem_Manage.stuMemFHeadList.head == g_cpssMem_Manage.stuMemFHeadList.tail &&
		1 == g_cpssMem_Manage.stuMemFHeadList.nTotalCount)
	{
		g_cpssMem_Manage.stuMemFHeadList.head = NULL;
		g_cpssMem_Manage.stuMemFHeadList.tail = NULL;
	}
	else if (NULL == pstuMemRecord->prev && NULL != pstuMemRecord->next)
	{
		g_cpssMem_Manage.stuMemFHeadList.head = pstuMemRecord->next;
		g_cpssMem_Manage.stuMemFHeadList.head->prev = NULL;
		pstuMemRecord->next = NULL;
	}
	else if (NULL != pstuMemRecord->prev && NULL == pstuMemRecord->next)
	{
		g_cpssMem_Manage.stuMemFHeadList.tail = pstuMemRecord->prev;
		g_cpssMem_Manage.stuMemFHeadList.tail->next = NULL;
		pstuMemRecord->prev = NULL;
	}
	else if (NULL != pstuMemRecord->prev && NULL != pstuMemRecord->next)
	{
		pstuMemRecord->prev->next = pstuMemRecord->next;
		pstuMemRecord->next->prev = pstuMemRecord->prev;
	}
	else
	{
		pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_FREE;
		VOS_PrintErr(__FILE__, __LINE__, "free link is error in Key[%d]", nMemRdKey);
		goto ERR_EXIT;
	}
	g_cpssMem_Manage.stuMemFHeadList.nTotalCount--;

	if (NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head &&
		NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail)
	{
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head = pstuMemRecord;
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail = pstuMemRecord;
	}
	else if (NULL != g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head &&
			 NULL != g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail)
	{
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail->next = pstuMemRecord;
		pstuMemRecord->prev = g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail;
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail = pstuMemRecord;
	}
	else
	{
		free(pstuMemRecord);
		pstuMemRecord = NULL;
		VOS_PrintErr(__FILE__, __LINE__, "link is error in Key[%d]", nMemRdKey);
		goto ERR_EXIT;
	}
	pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_USED;
	g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].nTotalCount++;
	g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].uMemSize += pstuMemRecord->nSize;
	g_cpssMem_Manage.uMemSize += pstuMemRecord->nSize;
OK_EXIT:
	if (VOS_OK != VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "del lock error");
	}
	return uRet;
ERR_EXIT:
	uRet = VOS_ERR;
	goto OK_EXIT;
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

	if (VOS_OK != VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return uRet;
	}
	if (NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head &&
		NULL == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail)
	{
		goto ERR_EXIT;
	}
	if (NULL == pstuMemRecord->next &&
		NULL == pstuMemRecord->prev &&
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail &&
		1 == g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].nTotalCount)
	{
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head = NULL;
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail = NULL;
	}
	else if (NULL == pstuMemRecord->prev && NULL != pstuMemRecord->next)
	{
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head = pstuMemRecord->next;
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].head->prev = NULL;
		pstuMemRecord->next = NULL;
	}
	else if (NULL != pstuMemRecord->prev && NULL == pstuMemRecord->next)
	{
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail = pstuMemRecord->prev;
		g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].tail->next = NULL;
		pstuMemRecord->prev = NULL;
	}
	else if(NULL != pstuMemRecord->prev && NULL != pstuMemRecord->next)
	{
		pstuMemRecord->prev->next = pstuMemRecord->next;
		pstuMemRecord->next->prev = pstuMemRecord->prev;
	}
	else
	{
		VOS_PrintErr(__FILE__, __LINE__, "free link is error in Key[%d]", nMemRdKey);
		goto ERR_EXIT;
	}
	g_cpssMem_Manage.uMemSize -= pstuMemRecord->nSize;
	g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].uMemSize -= pstuMemRecord->nSize;
	g_cpssMem_Manage.stuMemUHeadList[nMemRdKey].nTotalCount--;
	pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_RESE;
	pstuMemRecord->nSize = 0;
	pstuMemRecord->nFileLine = 0;

	if (NULL != pstuMemRecord->pstrVoid)
	{
		free(pstuMemRecord->pstrVoid);
		pstuMemRecord->pstrVoid = NULL;
	}
	BZERO(pstuMemRecord, sizeof(CPSS_MEM_RECORD));
	pstuMemRecord->nState = CPSS_MEM_RECORD_STAT_FREE;
	if ( NULL == g_cpssMem_Manage.stuMemFHeadList.head &&
		 NULL == g_cpssMem_Manage.stuMemFHeadList.tail )
	{
		g_cpssMem_Manage.stuMemFHeadList.head = pstuMemRecord;
		g_cpssMem_Manage.stuMemFHeadList.tail = pstuMemRecord;
	}
	else if ( NULL != g_cpssMem_Manage.stuMemFHeadList.head &&
			  NULL != g_cpssMem_Manage.stuMemFHeadList.tail )
	{
		g_cpssMem_Manage.stuMemFHeadList.tail->next = pstuMemRecord;
		pstuMemRecord->prev = g_cpssMem_Manage.stuMemFHeadList.tail;
		g_cpssMem_Manage.stuMemFHeadList.tail = pstuMemRecord;
	}
	else
	{
		free(pstuMemRecord);
		pstuMemRecord = NULL;
		VOS_PrintErr(__FILE__, __LINE__, "link is error in Key[%d]", nMemRdKey);
		goto ERR_EXIT;
	}
	g_cpssMem_Manage.stuMemFHeadList.nTotalCount++;
OK_EXIT:
	if ( VOS_OK != VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex) )
	{
		VOS_PrintErr(__FILE__, __LINE__, "del lock error");
	}
	return uRet;
ERR_EXIT:
	uRet = VOS_ERR;
	goto OK_EXIT;
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
	/*
	uRet = VOS_Mutex_Init(&g_cpssMem_Manage.hMutexBuffer, "CPSS_MEM_BUFFE");
	if (VOS_OK != uRet)
	{
		//VOS_PrintErr(__FILE__,__LINE__,"cpss init mem mutex error");
		printf("cpss init mem mutex error\n");
		return uRet;
	}
	*/
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
	ulFileNameLen = strlen(strFile)-32;
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
	ulFileNameLen = strlen(strFile) - 32;
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
	pstuMemRecord = cpss_mem_find_record_info(nMemRdKey, vAdress);
	if (NULL == pstuMemRecord)
	{
		return NULL;
	}
	pstuMemRecord->nFileLine = nLine;
	ulFileNameLen = strlen(strFile) - 32;
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
		free(vAdress);
		vAdress = NULL;
		VOS_PrintInfo(__FILE__, __LINE__, "free memory find is error key:%d %x\n",
			nMemRdKey, vAdress);
		return uRet;
	}
	if (VOS_OK != cpss_mem_move_to_free(nMemRdKey, pstuMemRecord))
	{
		free(vAdress);
		vAdress = NULL;
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
	//VOS_Mutex_Destroy(&g_cpssMem_Manage.hMutexBuffer);
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
