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
	uRet = VOS_Mutex_Init(&g_cpssMem_Manage.hMutexBuffer, "CPSS_MEM_BUFFE");
	if (VOS_OK != uRet)
	{
		//VOS_PrintErr(__FILE__,__LINE__,"cpss init mem mutex error");
		printf("cpss init mem mutex error\n");
		return uRet;
	}
	g_cpssMem_Manage.nTotalCount = 0;
	g_cpssMem_Manage.nFreeCount = 0;
	g_cpssMem_Manage.uMemSize = 0;
	g_cpssMem_Manage.pstuFreeBufHead = NULL;
	g_cpssMem_Manage.pstuFreeBufTail = NULL;
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
						   VOS_CHAR * strInfo, 
						   VOS_CHAR * strFile, 
						   VOS_INT32 nLine)
{
	VOS_INT32 uRet = 0;
	VOS_VOID * uRetVoid = NULL;
	uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return NULL;
	}
	g_cpssMem_Manage.uMemSize += ulSize;
	//VOS_PrintInfo(__FILE__,__LINE__,"calloc size memory %d",ulSize);
//	printf("calloc size memory :%d:%s\n",ulSize,strInfo);
	uRetVoid = malloc(ulSize);
	//printf("==:%d:===\n",g_cpssMem_Manage.uMemSize);
	if (NULL == uRetVoid)
	{
		g_cpssMem_Manage.uMemSize -= ulSize;
		VOS_PrintInfo(__FILE__,__LINE__,"now memory is:%d, calloc size memory :%d:%s:ErrCode:%d\n",
			g_cpssMem_Manage.uMemSize, ulSize,strInfo,GetLastError());
	}
	
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"del lock error");
		return NULL;
	}
	return uRetVoid;
}

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_mem_free
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_VOID cpss_mem_free(void * vAdress, VOS_INT32 ulSize, VOS_CHAR * strFile, VOS_INT32 nLine)
{
	VOS_INT32 uRet = 0;
	uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return ;
	}
	g_cpssMem_Manage.uMemSize -= ulSize;
	memset(vAdress, 0xFF, ulSize);
	free(vAdress);
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"del lock error");
	}
	//VOS_PrintInfo(__FILE__,__LINE__,"Free size memory %d",ulSize);
	//printf("Free size memory %d\n",ulSize);
}


/*===  FUNCTION  ==============================================================
*         Name:  cpss_print_mem_data
*  Description:  打印内存空间
*  Input      :	
*  OutPut     :	
*  Return     :    
* =============================================================================*/
static VOS_VOID cpss_print_mem_data(VOS_STRING pstrStat,CPSS_MEM_BUFFER * pstuFreeBufTmp)
{
	while(NULL != pstuFreeBufTmp)
	{
		sprintf(pstrStat+VOS_Strlen(pstrStat),"%p:->next:%p Index:%d Size:%d Stat:%d\n",
			pstuFreeBufTmp,
			(NULL != pstuFreeBufTmp?pstuFreeBufTmp->next:NULL),
			pstuFreeBufTmp->nIndex,pstuFreeBufTmp->nSize,pstuFreeBufTmp->nStat);
		pstuFreeBufTmp = pstuFreeBufTmp->next;
	}
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_insert_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static VOS_UINT32 cpss_in_mem_buffer_queued(CPSS_MEM_BUFFER * pStuInput)
{
	VOS_UINT32 ulRet = VOS_ERR;
	if ((NULL != g_cpssMem_Manage.pstuFreeBufHead && NULL == g_cpssMem_Manage.pstuFreeBufTail) ||
		(NULL == g_cpssMem_Manage.pstuFreeBufHead && NULL != g_cpssMem_Manage.pstuFreeBufTail))
	{
		VOS_PrintErr(__FILE__,__LINE__,"buffer link is error");
				cpss_print_mem_info("insert Faild");
		return ulRet;
	}
	pStuInput->next = NULL;
	pStuInput->prev = NULL;
	cpss_free_mem_buffer(pStuInput);
	if (NULL == g_cpssMem_Manage.pstuFreeBufHead && 
		NULL == g_cpssMem_Manage.pstuFreeBufTail)
	{
		g_cpssMem_Manage.pstuFreeBufHead = pStuInput;
	}else
	if (NULL != g_cpssMem_Manage.pstuFreeBufHead && 
		NULL != g_cpssMem_Manage.pstuFreeBufTail)
	{
		g_cpssMem_Manage.pstuFreeBufTail->next = pStuInput;
		pStuInput->prev = g_cpssMem_Manage.pstuFreeBufTail;
	}
	g_cpssMem_Manage.pstuFreeBufTail = pStuInput;
	g_cpssMem_Manage.nFreeCount ++;
	return VOS_OK;
}

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_insert_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static VOS_UINT32 cpss_out_mem_buffer_queued(pCPSS_MEM_BUFFER * pStuInput,VOS_UINT32 ulNum)
{
	VOS_UINT32 ulRet = VOS_OK;
	VOS_UINT32 uIndex = 0;
	CPSS_MEM_BUFFER * pstuTmp = NULL;
	CPSS_MEM_BUFFER * pstuRes = NULL;

	pstuTmp = pstuRes = g_cpssMem_Manage.pstuFreeBufHead;
	if (NULL == pstuRes)
	{
		cpss_print_mem_info("Memory Head List is NULL");
		VOS_PrintErr(__FILE__,__LINE__,"Memory Head List is NULL");
		return ulRet;
	}
	pstuRes->prev = NULL;
	for (uIndex = 0; uIndex<ulNum; uIndex++)
	{
		if (NULL == pstuRes)
		{
			cpss_print_mem_info("Memory List Not enough");
			VOS_PrintErr(__FILE__,__LINE__,"Memory List Not enough");
			return ulRet;
		}
		pstuRes->nStat = CPSS_MEM_BUFFER_STAT_RESE;
		if (NULL != pstuRes)
		{
			g_cpssMem_Manage.nFreeCount --;
			pstuRes = pstuRes->next;
		}
	}
	if (NULL != pstuRes)
	{
		if (NULL != pstuRes->prev)
		{
			pstuRes->prev->next = NULL;
		}
	}
	g_cpssMem_Manage.pstuFreeBufHead = pstuRes;
	if (NULL == g_cpssMem_Manage.pstuFreeBufHead)
	{
		g_cpssMem_Manage.pstuFreeBufTail = NULL;
	}else
	{
		g_cpssMem_Manage.pstuFreeBufHead->prev =NULL;
	}
	*pStuInput = pstuTmp;
	return ulRet;
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static CPSS_MEM_BUFFER * cpss_get_free_mem_buffer(VOS_INT32 ulNum)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MEM_BUFFER * pstuRes = NULL,* pstuTmp = NULL;
	VOS_INT32 uCurrFree = 0;
	VOS_INT32 uIndex = 0;

	uCurrFree = ulNum - g_cpssMem_Manage.nFreeCount;
	if (uCurrFree > 0)
	{
		pstuRes = (CPSS_MEM_BUFFER*)VOS_Malloc(sizeof(CPSS_MEM_BUFFER)*uCurrFree, "get buffer");
		VOS_Memset(pstuRes, sizeof(CPSS_MEM_BUFFER)*uCurrFree);
		if (NULL == pstuRes)
		{
			VOS_PrintErr(__FILE__,__LINE__,"get mem buffer address is NULL");
			return NULL;
		}
		for (uIndex = 0;uIndex< uCurrFree; uIndex++)
		{
			pstuRes[uIndex].nIndex = g_cpssMem_Manage.nTotalCount+1+uIndex;
			if (VOS_OK != cpss_in_mem_buffer_queued(&pstuRes[uIndex]))
			{
				VOS_PrintErr(__FILE__,__LINE__,"insert mem buffer faild NULL");
			}
		}
		g_cpssMem_Manage.nTotalCount += uCurrFree;
	}
	if (VOS_OK != cpss_out_mem_buffer_queued(&pstuTmp,ulNum))
	{
		VOS_PrintErr(__FILE__,__LINE__,"insert mem buffer faild NULL");
	}
	return pstuTmp;
}

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_VOID cpss_free_mem_buffer(CPSS_MEM_BUFFER * pstuMemBuffer)
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL == pstuMemBuffer)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input param is NULL");
		return ;
	}
	VOS_Memset(pstuMemBuffer->strBuffer, CPSS_MSG_BUFFER_SIZE);
	pstuMemBuffer->nStat = CPSS_MEM_BUFFER_STAT_FREE;
	pstuMemBuffer->nSize = 0;
	return ;
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  得到下一个空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
CPSS_MEM_BUFFER * cpss_get_next_buffer(CPSS_MEM_BUFFER * pStuInput)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	VOS_CHAR   pstrBuffer[1024]={0};
	CPSS_MEM_BUFFER * pStuFRet = NULL, *pStuFHead = NULL;
	if (NULL == pStuInput)
	{
		return NULL;
	}
	pStuFRet = pStuInput->next;
	if (NULL == pStuFRet)
	{
		return NULL;
	}
	uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutexBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return NULL;
	}
	//cpss_print_mem_data(pstrBuffer, pStuInput);
	//VOS_PrintDebug(__FILE__, __LINE__,"\n======Print:%p Info\n%s",pStuInput, pstrBuffer);

	/**move out in input list*/
	pStuInput->next = pStuFRet->next;
	//pStuFRet->prev->next = pStuFRet->next;
	if (NULL != pStuInput->next)
	{
		pStuInput->next->prev = pStuInput;
		//pStuFRet->next->prev = pStuFRet->prev;
	}
	if (VOS_OK != cpss_in_mem_buffer_queued(pStuFRet))
	{
		VOS_PrintErr(__FILE__,__LINE__,"insert mem buffer faild NULL");
	}
	//cpss_print_mem_info("Get Next Memory Buffer");
	pStuFHead = pStuInput->next;
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutexBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"del lock error");
	}
	return pStuFHead;
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
CPSS_MEM_BUFFER * cpss_get_mem_buffer(CPSS_MEM_BUFFER * pStuInput, VOS_UINT32 nNum)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	CPSS_MEM_BUFFER * pStuFRet = NULL, *pStuFTemp = NULL;
	if (NULL == pStuInput)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get mem buffer input is null");
		return NULL;
	}
	uRet = VOS_Mutex_Lock(&g_cpssMem_Manage.hMutexBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return NULL;
	}
	/************************************************************************/
	/* move in free list to used list                                       */
	/************************************************************************/	
	pStuFRet = cpss_get_free_mem_buffer(nNum);
	if (NULL == pStuFRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get free mem buffer is error");
		goto END_PROC;
	}
	//cpss_print_mem_info("Get New Memory Buffer");
	/************************************************************************/
	/* move in used list to used list                                       */
	/************************************************************************/
	pStuFTemp = pStuInput;
	while(NULL != pStuFTemp->next)
	{
		pStuFTemp = pStuFTemp->next;
	}
	pStuFTemp->next = pStuFRet;
	pStuFRet->prev = pStuFTemp;
	/*
	pStuInput->next = pStuFHead;
	pStuFHead->prev = pStuInput;
	*/
END_PROC:
	uRet = VOS_Mutex_Unlock(&g_cpssMem_Manage.hMutexBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"del lock error");
	}
	return pStuFRet;
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
	VOS_Mutex_Destroy(&g_cpssMem_Manage.hMutexBuffer);
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
	CPSS_MEM_BUFFER * pstuFreeBufTmp = NULL;
	VOS_CHAR pstrBuffer[1020] = {0};

	sprintf(pstrBuffer,"=====%s=====\nMemory Size:%d Free Count:%d Used Count:%d\nHead:%p-->Tail:%p\n", 
		pstrStat,
		g_cpssMem_Manage.uMemSize,
		g_cpssMem_Manage.nFreeCount,
		g_cpssMem_Manage.nTotalCount,
		g_cpssMem_Manage.pstuFreeBufHead,
		g_cpssMem_Manage.pstuFreeBufTail);	
	pstuFreeBufTmp = g_cpssMem_Manage.pstuFreeBufHead;
	cpss_print_mem_data(pstrBuffer+VOS_Strlen(pstrBuffer), pstuFreeBufTmp);
	VOS_PrintInfo(__FILE__, __LINE__,"Print Buffer Info\n%s",pstrBuffer);
}
