/* =========================================================================
 *
 *       Filename:  cpss_vk_print.c
 *
 *    Description:  打印函数接口文件
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 19时32分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ========================================================================*/

#include "cpss_vk_print.h"
#include "cpss_com_pid.h"
#include "cpss_tm_timer.h"
#include "cpss_vk_thread.h"
#include "cpss_public.h"
#include "cpss_vk_file.h"

#define VOS_Log_Malloc(ulSize)			VOS_Malloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Realloc(pstrads,ulSize)	VOS_Realloc((pstrads), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Remset(pstrads)			VOS_Remset((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Memcls(pstrads)			VOS_Memcls((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Memcat(pstrA,pstrB)		VOS_Memcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Memsize(pstrads)		VOS_Memsize((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_LOG))
#define VOS_Log_Free(pstrads)			VOS_Free((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_LOG))

#define VOS_Log_Strcat(pstrA,pstrB)		VOS_CpsStrcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_LOG))

static MANAGE_PRINT g_manageprint = {0};
THREAD VOS_UINT32	g_ProcessID = 0;
THREAD VOS_UINT32	g_ThreadID = 0;
/* ===  FUNCTION  ==========================================================
 *         Name:  cpss_vsprintf(const char *fmt, ...)
 *  Description: 格式化包囊函数 , VOS_INT32  *nFileLength
 * ========================================================================*/
static VOS_CHAR * cpss_vsprintf(pPRINT_INFO pPrintInfoTmp, const VOS_CHAR *fmt, va_list ap)
{
	VOS_CHAR *p = NULL;//, *np;
	VOS_INT32 size =PRINTT_INFO_LEN,n=0;
	if (NULL == pPrintInfoTmp->pszPrintInfo)
	{
		pPrintInfoTmp->pszPrintInfo = (VOS_CHAR*)VOS_Log_Malloc(PRINTT_INFO_LEN);
		if (NULL == pPrintInfoTmp->pszPrintInfo)
		{
			return NULL;
		}
	}
	/*
	if ((p = (VOS_CHAR *)VOS_Malloc (size, "get print str trace")) == NULL)
	if (NULL == p)	
	{
		*nFileLength = 0;
		return NULL;
	}
	*/
	p = pPrintInfoTmp->pszPrintInfo;
	while (1) 
	{
		/*  Try to print in the allocated space. */
#if (OS_TYPE == OS_TYPE_WINDOW)
		n = _vsnprintf (p, size, fmt, ap);
#elif (OS_TYPE == OS_TYPE_LINUX)
		n = vsnprintf (p, size, fmt, ap);
#endif

		/*  Else try again with more space. */
		if (n > -1 && n < size)    /*  glibc 2.1 */
		{
			pPrintInfoTmp->ulPrintSize = n;
			return p;
		}
		else           /*  glibc 2.0 */
		{
			size *= 2;  /*  twice the old size */
		}
		if ((p = (VOS_CHAR*)VOS_Log_Realloc(p, size)) == NULL)
		{
			VOS_Free(pPrintInfoTmp->pszPrintInfo, n);
			pPrintInfoTmp->pszPrintInfo = NULL;
			return NULL;
		}
		else
		{
			pPrintInfoTmp->pszPrintInfo = p;
		}
		/*  If that worked, return the string. */
		/*
		if (n > -1 && n < size)
		{
			pPrintInfoTmp->ulPrintSize = n;
			return p;
		}
		else
		{
			break;
		}
		*/
	}
return NULL;
#if 0
		/*  Else try again with more space. */
		if (n > -1)    /*  glibc 2.1 */
		{
			size = n+1; /*  precisely what is needed */
		}
		else           /*  glibc 2.0 */
		{
			size *= 2;  /*  twice the old size */
		}
		/*
		//VOS_Free(p, n);
		//if ((np = (VOS_CHAR *)VOS_Malloc (size, "get print str trace")) == NULL)
		if ((np = realloc (p, size)) == NULL) 
		{
			VOS_Free(p,n);
			*nFileLength = 0;
			return NULL;
		} 
		else 
		{
			p = np;
		}
		*/
	}
	//*nFileLength = 0;
#endif
}		/* -----  end of function cpss_vsprintf(const char *fmt, ...)  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_notdir
 *  Description:	取消目录
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_STRING cpss_notdir(VOS_STRING pszPath)
{
	VOS_STRING szPathTmp = NULL;
#if (OS_TYPE == OS_TYPE_LINUX)
	szPathTmp = strrchr(pszPath,'/');
#elif (OS_TYPE == OS_TYPE_WINDOW)
	int ch = '\\';
	szPathTmp = strrchr(pszPath,ch);
#endif
	if (NULL == szPathTmp)
	{
		return pszPath;
	}
//	VOS_Strcpy(szPathTmp,"0001");
	//return szPathTmp;
	//*(szPathTmp+strlen(szPathTmp)-2)=0;
	*(szPathTmp+strlen(szPathTmp)-2)=0;
	if (NULL != szPathTmp )
	{
		szPathTmp[13] = 0;
		return szPathTmp+7;
	}
	return NULL;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_init_print
 *  Description:	初始化线程log 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_init_print (VOS_UINT32 ulThreadID)
{
	if (0 == g_ProcessID)
	{
		g_ProcessID = GetCurrentProcessId();
	}
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_open_trace_handle
 *  Description:	得到trace目录
 *  Input      :    pszTracePath   trace path 
					pubufferLen    file  length
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_INT32 cpss_open_trace_handle(VOS_STRING buffer, VOS_STRING strbuffKey)
{
	VOS_INT32 logfd = 0;
	VOS_UINT32 nRet = -1;
	VOS_UINT32	ulReadTimes = 0,ulReadSize = 0,ulTimes = 0,ulRet=0;
	VOS_UINT32  ubufferLen = 0;
	VOS_CHAR   pszTracePath[MAX_PATH] = {0};
	VOS_CHAR   strtimebuff[CPSS_MAX_TIME] ={0};
	VOS_CHAR   strbuffer[PRINT_TRACE_LOG_MAX_BUFFER]={0};
	
	if (NULL == buffer)
	{
		printf("log file path address null");
		return nRet;
	}
	
	if (0 == *buffer)
	{
		printf("log file path null");
		return nRet;
	}

	
	ulRet = cpss_get_date_or_time(strtimebuff,1);
	if (VOS_OK != ulRet)
	{
		return nRet;
	}
	sprintf(pszTracePath,"%s\\trace",buffer);
	if (VOS_OK != chdir(pszTracePath))
	{
#if (OS_TYPE == OS_TYPE_WINDOW)
		if (VOS_OK != mkdir(pszTracePath))
#elif (OS_TYPE == OS_TYPE_LINUX)
		if (VOS_OK != mkdir(pszTracePath,TRUE))
#endif
		{
			printf("Make file :%s Error",pszTracePath);
			return nRet;
		}
	}
	sprintf(pszTracePath,"%s\\trace\\%s",buffer,strtimebuff);
	if (VOS_OK != chdir(pszTracePath))
	{
#if (OS_TYPE == OS_TYPE_WINDOW)
		if (VOS_OK != mkdir(pszTracePath))
#elif (OS_TYPE == OS_TYPE_LINUX)
		if (VOS_OK != mkdir(pszTracePath,false))
#endif
		{
			printf("Make file :%s Error",pszTracePath);
			return nRet;
		}
	}
	for (ulReadTimes =0;ulReadTimes<PRINT_TRACE_LOG_MAX_BUFFER;ulReadTimes++)
	{
		sprintf(pszTracePath,"%s\\trace\\%s\\N_J_S_%04d.%s",
			buffer, strtimebuff, ulReadTimes, strbuffKey);
		logfd = open(pszTracePath,O_RDWR);
		if (0 > logfd)
		{
			break;
		}
		close(logfd);
	}
	if (0 != ulReadTimes)
	{
		ulReadTimes --;
		sprintf(pszTracePath,"%s\\trace\\%s\\N_J_S_%04d.%s",
			buffer, strtimebuff, ulReadTimes, strbuffKey);
	}
	else
	{
		sprintf(pszTracePath,"%s\\trace\\%s\\N_J_S_%04d.%s",
			buffer, strtimebuff, 0, strbuffKey);
	}
OPEN_FILE:
	logfd = _open(pszTracePath, O_WRONLY | O_CREAT | O_APPEND, _S_IREAD | _S_IWRITE);
	if (logfd > 0)
	{
		ubufferLen = lseek(logfd, 0L, SEEK_END);
		//printf("%d--%d",ubufferLen, PRINT_TRACE_LOG_MAX_BUFFER*PRINT_TRACE_LOG_MAX_BUFFER);
		if (PRINT_TRACE_LOG_MAX_BUFFER*PRINT_TRACE_LOG_MAX_BUFFER < ubufferLen)
		{
			close(logfd);
			ulReadTimes ++;
			sprintf(pszTracePath,"%s\\trace\\%s\\N_J_S_%04d.trc",buffer,strtimebuff,ulReadTimes);
			goto OPEN_FILE;
		}
	}
	return logfd;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_write_buffer
 *  Description: 组合字符串格式
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static  VOS_VOID cpss_write_buffer (VOS_STRING strPath,VOS_STRING strKey, VOS_STRING strBuffer,VOS_UINT32 ulength)
{
	VOS_UINT32 logfd = 0;
	VOS_UINT32 uWlength = 0;
	logfd = cpss_open_trace_handle(strPath, strKey);
	if (logfd < 0)
	{
		printf("open log file (%s) error ",strPath);
		return ;
	}
	uWlength = write(logfd,strBuffer,ulength);
	if (uWlength != ulength)
	{
		printf("write log file (%s) error ",strPath);
	}
	close(logfd);
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_in_a_to_b_trace
*  Description:	将a 插入到link中
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static  VOS_UINT32 cpss_move_in_a_to_b_trace (pPRINT_INFO *ppPrintHead, pPRINT_INFO *ppPrintTail, pPRINT_INFO pPrintIn,VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pPRINT_INFO pPrintHead= NULL;
	pPRINT_INFO pPrintTail = NULL;
	if (NULL == ppPrintHead||
		NULL == ppPrintTail ||
		NULL == pPrintIn)
	{
		printf("cpss print in a to b parm is null [%s:(%d)]",__FILE__,__LINE__);
		goto END_PROC;
	}
	pPrintIn->next = NULL;
	pPrintIn->prev = NULL;
	pPrintHead = *ppPrintHead;
	pPrintTail = *ppPrintTail;
	if ((NULL != pPrintHead && NULL == pPrintHead) ||
		(NULL == pPrintHead && NULL != pPrintTail))
	{
		printf("cpss print in a to b parm is error H%p-->T:%p [%s:(%d)]",
			*ppPrintHead,*ppPrintTail,__FILE__,__LINE__);
		goto END_PROC;
	}else
	if (NULL == pPrintHead && NULL == pPrintHead)
	{
		*ppPrintHead = pPrintIn;
		*ppPrintTail = pPrintIn;
		ulRet = VOS_OK;
		goto END_PROC;
	}
	if (uType == PRINTF_TYPE_TAIL)
	{
		if (NULL == pPrintTail->next)
		{
			pPrintTail->next = pPrintIn;
			pPrintIn->prev = *ppPrintTail;
			*ppPrintTail = pPrintIn;
		}
		else
		{
			printf("cpss print in a to b tial next is error H%p-->T:%p [%s:(%d)]",
				pPrintTail,pPrintTail->next,__FILE__,__LINE__);
			goto END_PROC;
		}
	}else
	if (uType == PRINTF_TYPE_HEAD)
	{
		if (NULL == pPrintHead->prev)
		{
			pPrintHead->prev = pPrintIn;
			pPrintIn->next = *ppPrintHead;
			*ppPrintHead = pPrintIn;
		}
		else
		{
			printf("cpss print in a to b head prev is error H%p-->T:%p [%s:(%d)]",
				pPrintHead,pPrintHead->prev,__FILE__,__LINE__);
			goto END_PROC;
		}
	}
	else
	{
		printf("cpss print in a to b type is error [%s:(%d)]",__FILE__,__LINE__);
		goto END_PROC;
	}
	ulRet = VOS_OK;
END_PROC:
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_out_a_to_b_trace
*  Description:	将a 在link中的删除
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static  VOS_UINT32 cpss_move_out_a_to_b_trace (pPRINT_INFO *ppPrintHead, pPRINT_INFO *ppPrintTail, pPRINT_INFO pPrintIn,VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pPRINT_INFO pPrintHead= NULL;
	pPRINT_INFO pPrintTail = NULL;

	if (NULL == ppPrintHead ||
		NULL == ppPrintTail ||
		NULL == pPrintIn)
	{
		printf("cpss print out a to b parm is null [%s:(%d)]",__FILE__,__LINE__);
		goto END_PROC;
	}

	pPrintHead = *ppPrintHead;
	pPrintTail = *ppPrintTail;
	if ((NULL == pPrintHead || NULL == pPrintHead))
	{
		printf("cpss print out a to b parm is error H%p-->T:%p [%s:(%d)]",
			*ppPrintHead,*ppPrintTail,__FILE__,__LINE__);
		goto END_PROC;
	}
	/************************************************************************/
	/*   find in link b                                                     */
	/************************************************************************/
	if (uType == PRINTF_TYPE_HEAD)
	{
		while (NULL != pPrintHead)
		{
			if (pPrintHead == pPrintIn)
			{
				break;
			}
			pPrintHead = pPrintHead->next;
		}
	}else
	if (uType == PRINTF_TYPE_TAIL)
	{
		pPrintHead = NULL;
		while (NULL != pPrintTail)
		{
			if (pPrintTail == pPrintIn)
			{
				break;
			}
			pPrintTail = pPrintTail->prev;
		}
		pPrintHead = pPrintTail;
	}
	else
	{
		printf("cpss print in a to b type is error [%s:(%d)]",__FILE__,__LINE__);
		goto END_PROC;
	}
	if (NULL == pPrintHead)
	{
		printf("cpss print in a to b not find is %p error [%s:(%d)]",
			pPrintIn, __FILE__,__LINE__);
		goto END_PROC;
	}
	if (NULL == pPrintHead->next &&
		NULL == pPrintHead->prev)
	{
		*ppPrintHead = NULL;
		*ppPrintTail = NULL;
	}else
	if (NULL != pPrintHead->next &&
		NULL == pPrintHead->prev)
	{
		*ppPrintHead = pPrintHead->next;
		(*ppPrintHead)->prev = NULL;
	}else
	if (NULL == pPrintHead->next &&
		NULL != pPrintHead->prev)
	{
		*ppPrintTail = pPrintHead->prev;
		(*ppPrintTail)->next = NULL;
	}
	else
	{
		pPrintHead->prev->next = pPrintHead->next;
		pPrintHead->next->prev = pPrintHead->prev;
	}
	pPrintIn->next = NULL;
	pPrintIn->prev = NULL;
	ulRet = VOS_OK;
END_PROC:
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_in_a_for_b_trace
*  Description:	将a 在link中的删除
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static  VOS_UINT32 cpss_get_in_a_for_b_trace (pPRINT_INFO *ppPrintHead, pPRINT_INFO *ppPrintTail, pPRINT_INFO *ppPrintOut)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pPRINT_INFO pPrintHead= NULL;
	pPRINT_INFO pPrintTail = NULL;
	PRINT_INFO *pPrintInfoTmp = NULL;

	if (NULL == ppPrintHead ||
		NULL == ppPrintTail ||
		NULL == ppPrintOut)
	{
		printf("cpss print out a to b parm is null [%s:(%d)]",__FILE__,__LINE__);
		goto END_PROC;
	}
	pPrintHead = *ppPrintHead;
	pPrintTail = *ppPrintTail;

	pPrintInfoTmp = pPrintHead;
	while(NULL != pPrintInfoTmp)
	{
		if (PRINT_INFO_RESERVE == pPrintInfoTmp->ulState)
		{
			pPrintInfoTmp = pPrintInfoTmp->next;
			continue;
		}
		pPrintInfoTmp->ulState = PRINT_INFO_RESERVE;
		break;
	}

	if (NULL == pPrintInfoTmp)
	{
		if (g_manageprint.ulPrintCount < PRINT_LOG_COUNT)
		{
			pPrintInfoTmp = (pPRINT_INFO)VOS_Log_Malloc(sizeof(PRINT_INFO));
			if (NULL == pPrintInfoTmp)
			{
				printf("cpss print get b in a memset a is error [%s:(%d)]", __FILE__,__LINE__);
				goto END_PROC;
			}
			/*
			pPrintInfoTmp->pszPrintInfo = (VOS_CHAR *)VOS_Malloc(PRINTT_INFO_LEN ,"get print buffer");
			if (NULL == pPrintInfoTmp->pszPrintInfo)
			{
				printf("cpss print get b in a memset a->buff is error [%s:(%d)]", __FILE__,__LINE__);
				VOS_Free(pPrintInfoTmp, sizeof(PRINT_INFO));
				goto END_PROC;
			}
			BZERO(pPrintInfoTmp->pszPrintInfo , PRINTT_INFO_LEN );
			*/
			pPrintInfoTmp->ulLogID = g_manageprint.ulPrintCount + 1;
			pPrintInfoTmp->ulState = PRINT_INFO_RESERVE;
			pPrintInfoTmp->next = NULL;
			pPrintInfoTmp->prev = NULL;

			if (NULL == pPrintHead && NULL == pPrintTail)
			{
				*ppPrintHead = pPrintInfoTmp;
				*ppPrintTail = pPrintInfoTmp;
			}else
			if (NULL != pPrintHead && NULL != pPrintTail)
			{
				pPrintTail->next = pPrintInfoTmp;
				pPrintInfoTmp->prev = pPrintTail;
				*ppPrintTail = pPrintInfoTmp;
			}
			else
			{
				printf("cpss print free to used H:%p->T:%p faild[%s:(%d)]",
					pPrintHead,	pPrintTail,
					__FILE__,__LINE__);
				*ppPrintHead = pPrintInfoTmp;
				*ppPrintTail = pPrintInfoTmp;
			}
			g_manageprint.ulPrintCount++;
			g_manageprint.ulFreePrintCount++;
		}
		ulRet = VOS_OK;
	}
	*ppPrintOut = NULL;
	if (NULL != pPrintInfoTmp)
	{
		//BZERO(pPrintInfoTmp->pszPrintInfo, PRINTT_INFO_LEN);
		BZERO(pPrintInfoTmp->szPrintFile,  PRINTT_FILE_LEN);
		BZERO(pPrintInfoTmp->szPrintPid,   PRINTT_PID_LEN);
		BZERO(pPrintInfoTmp->szPrintTime,  PRINTT_TIME_LEN);
		//BZERO(pPrintInfoTmp->szPrintType,  PRINTT_TYPE_LEN);
		pPrintInfoTmp->ulPrintType = 0;
		pPrintInfoTmp->ulPrintLine = 0;
		*ppPrintOut = pPrintInfoTmp;
		ulRet = VOS_OK;
	}
END_PROC:
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_a_to_b_for_trace
*  Description:	将used的trace移动到free链表中 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static  VOS_UINT32 cpss_move_a_to_b_for_trace(pPRINT_INFO *ppPrintInfo, VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pPRINT_INFO pPrintInfo = NULL;
	int nIndex = 0;

	if (NULL == ppPrintInfo)
	{
		printf("cpss_move_a_to_b_for_trace parm is NULL");
		goto END_PROC;
	}
	pPrintInfo = *ppPrintInfo;

	ulRet = VOS_Mutex_Lock(&g_manageprint.hMutex);
	if (VOS_OK != ulRet)
	{
		printf("cpss print free to used lock faild");
		goto END_PROC;
	}
	switch (uType)
	{
	case PRITF_MOVE_USED_TO_FREE:
		/*remove from used link*/
		ulRet = cpss_move_out_a_to_b_trace(
			&g_manageprint.pUsedPrintinfoHead,
			&g_manageprint.pUsedPrintinfoTail,
			pPrintInfo,PRINTF_TYPE_HEAD);
		if (VOS_OK != ulRet)
		{
			printf("cpss move a to b out is error H;%p->T:%p[%s(%d)]\n", 
				g_manageprint.pUsedPrintinfoHead,
				g_manageprint.pUsedPrintinfoTail,
				__FILE__, __LINE__);
			goto END_PROC;
		}

		nIndex = pPrintInfo->ulLogID;
		VOS_Log_Free(pPrintInfo->pszPrintInfo);
		BZERO(pPrintInfo, sizeof(PRINT_INFO));
		pPrintInfo->ulLogID = nIndex;
		pPrintInfo->ulState = PRINT_INFO_FREE;

		/*add in free link*/
		ulRet = cpss_move_in_a_to_b_trace(
			&g_manageprint.pFreePrintinfoHead,
			&g_manageprint.pFreePrintinfoTail,
			pPrintInfo,PRINTF_TYPE_TAIL);
		if (VOS_OK != ulRet)
		{
			printf("cpss move a to b in is error H;%p->T:%p[%s(%d)]\n", 
				g_manageprint.pFreePrintinfoHead,
				g_manageprint.pFreePrintinfoTail,
				__FILE__, __LINE__);
			goto END_PROC;
		}
		g_manageprint.ulFreePrintCount ++;
		ulRet = VOS_OK;
		break;
	case PRITF_MOVE_FREE_TO_USED:
		/*remove from free link*/
		ulRet = cpss_move_out_a_to_b_trace(
			&g_manageprint.pFreePrintinfoHead,
			&g_manageprint.pFreePrintinfoTail,
			pPrintInfo,PRINTF_TYPE_HEAD);
		if (VOS_OK != ulRet)
		{
			printf("cpss move a to b out is error H;%p->T:%p[%s(%d)]\n", 
				g_manageprint.pFreePrintinfoHead,
				g_manageprint.pFreePrintinfoTail,
				__FILE__, __LINE__);
			goto END_PROC;
		}
		/*add in used link*/
		ulRet = cpss_move_in_a_to_b_trace(
			&g_manageprint.pUsedPrintinfoHead,
			&g_manageprint.pUsedPrintinfoTail,
			pPrintInfo,PRINTF_TYPE_TAIL);
		if (VOS_OK != ulRet)
		{
			printf("cpss move a to b in is error H;%p->T:%p[%s(%d)]\n", 
				g_manageprint.pUsedPrintinfoHead,
				g_manageprint.pUsedPrintinfoTail,
				__FILE__, __LINE__);
			return ulRet;
		}
		if (g_manageprint.ulFreePrintCount > 0 )
		{
			g_manageprint.ulFreePrintCount --;
		}
		ulRet = VOS_OK;
		break;
	case PRITF_MOVE_GET_FREE:
		ulRet = cpss_get_in_a_for_b_trace(
				&g_manageprint.pFreePrintinfoHead,
				&g_manageprint.pFreePrintinfoTail,ppPrintInfo);
		if (VOS_OK != ulRet)
		{
			printf("cpss move a to b in is error H;%p->T:%p[%s(%d)]\n", 
				g_manageprint.pUsedPrintinfoHead,
				g_manageprint.pUsedPrintinfoTail,
				__FILE__, __LINE__);
			return ulRet;
		}
		break;
	default:
		printf("cpss move a to b type is error [%s(%d)]\n",__FILE__, __LINE__);
		break;
	}
END_PROC:
	if (VOS_OK != ulRet)
	{
		printf("cpss_move_a_to_b_for_trace error faild");
	}
	ulRet = VOS_Mutex_Unlock(&g_manageprint.hMutex);
	if (VOS_OK != ulRet)
	{
		printf("cpss print free to used unlock faild");
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_print_trace
 *  Description:	插入trace到链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 VOS_VOID cpss_print_trace ()
 {
	PRINT_INFO		  * pPrintinfo = NULL;
	VOS_CHAR*			stuBuffer  = NULL;
	VOS_CHAR			szCurrentPath[256] = {0};
	return;
	VOS_PrintBuffer(&stuBuffer,"\n\n**************\n");
	VOS_PrintBuffer(&stuBuffer,"Print Level:%d\n",g_manageprint.g_printShowlevel);
	VOS_PrintBuffer(&stuBuffer,"Trace Count:%d\n",g_manageprint.ulPrintCount);
	VOS_PrintBuffer(&stuBuffer,"Trace Free Count:%d\n",g_manageprint.ulFreePrintCount);
	VOS_PrintBuffer(&stuBuffer,"Trace Save Count:%d\n",g_manageprint.ulSaveCount);
	pPrintinfo = g_manageprint.pFreePrintinfoHead;
	while (NULL != pPrintinfo)
	{
		VOS_PrintBuffer(&stuBuffer, "Free Trace %d:%p->next:%p\n",
			pPrintinfo->ulLogID, 
			pPrintinfo, 
			pPrintinfo->next);
		pPrintinfo = pPrintinfo->next;
	}
	pPrintinfo = g_manageprint.pUsedPrintinfoHead;
	while (NULL != pPrintinfo)
	{
		VOS_PrintBuffer(&stuBuffer,"Used Trace %d:%p->next:%p\n",
			pPrintinfo->ulLogID, 
			pPrintinfo, 
			pPrintinfo->next);
		pPrintinfo = pPrintinfo->next;
	}

	if (VOS_OK != cpss_get_current_path(szCurrentPath))
	{
		return ;
	}
	cpss_write_buffer(szCurrentPath, "msg", stuBuffer, VOS_Strlen(stuBuffer));
	VOS_PrintBufferRelease(stuBuffer);
	//VOS_PrintDebug("",CPSS_PRINTF_BUFFER,"%s",pstuBuffer->strBuffer);
 }
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_print_trace
 *  Description:	插入trace到链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 static  pPRINT_INFO cpss_get_free_print_trace ()
{
	 VOS_UINT32 ulRet = VOS_ERR;
	 PRINT_INFO *pPrintInfoTmp = NULL;
	 while(NULL == pPrintInfoTmp)
	 {
		 cpss_move_a_to_b_for_trace(&pPrintInfoTmp,PRITF_MOVE_GET_FREE);
		 if (NULL != pPrintInfoTmp)
		 {
			 break;;
		 }
		 /* 如果空闲队列都用完了，那么就等带使用队列释放后在接着处理*/
		 ulRet = VOS_Wait_Event(&g_manageprint.g_MsgEvent,80);//PRINT_SLEEP_TIME
		 if (VOS_OK != ulRet)
		 {
			 printf("cpss print free to used wait timeout[%s:(%d)]",__FILE__,__LINE__);
		 }
	 }
	 return pPrintInfoTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_print_trace
 *  Description:	插入trace到链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 static  pPRINT_INFO cpss_get_used_print_trace ()
{
	PRINT_INFO *pPrintInfoTmp = g_manageprint.pUsedPrintinfoHead;
	cpss_print_trace();
	while(NULL != pPrintInfoTmp)
	{
		return pPrintInfoTmp;
	}
	if (NULL != g_manageprint.pUsedPrintinfoTail)
	{
		VOS_PrintWarn(__FILE__,__LINE__,"File%s(%d) Print Used Tail is NULL");
	}
	return pPrintInfoTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_print_free_used_trace
 *  Description:	将free的trace移动use链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 static  VOS_UINT32 cpss_move_print_free_used_trace (pPRINT_INFO pPrintInfo)
 {
	 VOS_UINT32 ulRet = VOS_ERR;
	 ulRet = cpss_move_a_to_b_for_trace(&pPrintInfo, PRITF_MOVE_FREE_TO_USED);
	 if (VOS_OK != ulRet)
	 {
		 printf("cpss print free to used lock faild");
	 }
	 return ulRet;
 }
 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_print_used_free_trace
 *  Description:	将used的trace移动到free链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 static  VOS_UINT32 cpss_move_print_used_free_trace (pPRINT_INFO pPrintInfo)
 {
	VOS_UINT32 ulRet = VOS_ERR;

	ulRet = cpss_move_a_to_b_for_trace(&pPrintInfo, PRITF_MOVE_USED_TO_FREE);
	if (VOS_OK != ulRet)
	{
		printf("cpss print free to used lock faild");
	}

	ulRet = VOS_Set_Event(&g_manageprint.g_MsgEvent);
	if (VOS_OK != ulRet)
	{
		printf("cpss print free to used unlock faild[%s(%d)]", __FILE__, __LINE__);
	}
	return ulRet;
 }
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_insert_print_trace
 *  Description:	插入trace到链表中 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
 static  VOS_UINT32 cpss_insert_print_trace (
	 VOS_UINT32 ucProcessPid,				//PID名称
	 VOS_UINT32 ulPrintType,				//打印属性
	 VOS_STRING pszPrintFile,				//打印文件
	 VOS_UINT32 ulPrintLine,				//打印行数
	 pPRINT_INFO pPrintInfo)				//内容
 {
	VOS_CHAR   pszPrintTime[CPSS_MAX_TIME] = { 0 };
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR * pszPrintPid = NULL;
	pPRINT_INFO pPrintInfoTmp = pPrintInfo;//cpss_get_free_print_trace();
	if (0 == g_ThreadID)
	{
		g_ThreadID = GetCurrentThreadId();
	}
	pszPrintPid = cpss_get_pid_name(ucProcessPid);	//得到当前PID对应名称
	cpss_get_current_time_to_str(pszPrintTime);		//得到当前系统时间
	if (0 == *pszPrintTime)
	{
		printf("CGP Get Current Time is Error!\n");
		return ulRet;
	}
	if (NULL != pPrintInfoTmp)
	{
		VOS_Memcpy(pPrintInfoTmp->szPrintPid, pszPrintPid, strlen(pszPrintPid));
		VOS_Memcpy(pPrintInfoTmp->szPrintTime, pszPrintTime, strlen(pszPrintTime));
		VOS_Memcpy(pPrintInfoTmp->szPrintFile, pszPrintFile, strlen(pszPrintFile));
		pPrintInfoTmp->ulPrintType	= ulPrintType;
		pPrintInfoTmp->ulPrintLine  = ulPrintLine;
		pPrintInfoTmp->ulThreadID = g_ThreadID;
	}
	return cpss_move_print_free_used_trace(pPrintInfoTmp);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_printstr
 *  Description: 组合字符串格式
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static  VOS_INT32 cpss_insert_printstr (
		VOS_UINT32 ucProcessPid,			//PID名称
		VOS_UINT32 ulPrintType,				//打印属性
		VOS_UINT8  ucPrintLevel,			//打印级别
		VOS_STRING pszPrintFile,			//打印文件
		VOS_UINT32 ulPrintLine,				//打印行数
		pPRINT_INFO pPrintInfo)				//打印内容
{
	VOS_UINT32 ulRet = VOS_ERR;

	ulRet = cpss_insert_print_trace(ucProcessPid, ulPrintType, pszPrintFile, ulPrintLine, pPrintInfo);
	if (VOS_OK != ulRet)
	{
		printf("Insert Trace Error\n");
		return ulRet;
	}
	// printf("set Event ok\n");
	ulRet = VOS_Set_Event(&g_manageprint.g_LogEvent);
	if (VOS_OK != ulRet)
	{
		printf("set Event Error\n");
		return ulRet;
	}
	return ulRet;
}		/* -----  end of function cpss_get_printstr  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_key_str_value
 *  Description: 组合字符串格式
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static  VOS_CHAR * cpss_print_key_str_value (VOS_INT32 ulPrintType)
{
	VOS_CHAR * g_strPrintKey[] ={
			CPSS_PRINT_STR_ERR,
			CPSS_PRINT_STR_WARN,
			CPSS_PRINT_STR_INFO,
			CPSS_PRINT_STR_DBUG,
			CPSS_PRINT_STR_DUMP,};
	if (ulPrintType > CPSS_PRINT_DBUG || ulPrintType < CPSS_PRINT_ERR)
	{
		return g_strPrintKey[0];
	}
	return g_strPrintKey[ulPrintType-1];
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_key_str
 *  Description: 组合字符串格式
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static  VOS_VOID cpss_print_key_str (HANDLE hHandle, VOS_INT32 nBeforeClr, VOS_INT32 nAfterClr,VOS_UINT32 ulPrintType)
{

#if (OS_TYPE == OS_TYPE_WINDOW)
	VOS_UINT16 g_ulPrintType[] = {NONE_INT, RED_INT, LIGHT_RED_INT, GREEN_INT, LIGHT_GREEN_INT, BLUE_INT, LIGHT_BLUE_INT, 
		DARY_GRAY_INT, CYAN_INT, LIGHT_CYAN_INT, PURPLE_INT,LIGHT_PURPLE_INT, BROWN_INT, YELLOW_INT, LIGHT_GRAY,WHITE_INT};
#elif (OS_TYPE = OS_TYPE_LINUX)
	VOS_CHAR *g_strPrintType[] = {NONE_STR, RED_STR, LIGHT_RED_STR, GREEN_STR, LIGHT_GREEN_STR, BLUE_STR, LIGHT_BLUE_STR, 
		DARY_GRAY_STR, CYAN_STR, LIGHT_CYAN_STR, PURPLE_STR,LIGHT_PURPLE_STR, BROWN_STR, YELLOW_STR, LIGHT_GRAY,WHITE_STR};
#endif

	if ((nBeforeClr < NONE || nBeforeClr > WHITE) ||
		(nAfterClr < NONE || nAfterClr > WHITE))
	{
		printf("before:%d or after:%d is error ",nBeforeClr, nAfterClr);
		nBeforeClr = NONE;
		nAfterClr = NONE;
	}
	
#if (OS_TYPE == OS_TYPE_WINDOW)
	SetConsoleTextAttribute(hHandle, g_ulPrintType[nBeforeClr]);
	printf("[%s]", cpss_print_key_str_value(ulPrintType));
	SetConsoleTextAttribute(hHandle, g_ulPrintType[nAfterClr]);
#elif (OS_TYPE = OS_TYPE_LINUX)
	fprintf(stdout, "%s[%s]%s", g_strPrintType[nBeforeClr], cpss_print_key_str_value(ulPrintType), g_strPrintType[nAfterClr]);
#endif
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_trace_proc
 *  Description:	打印trace 回调函数 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_print_trace_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_print_trace_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32    ulRet = VOS_ERR;//g_traceo_on_off
	VOS_INT32     ulBufferLen =0;
	VOS_CHAR      buffer[10*1024] = {0};
	VOS_CHAR      szCurrentPath[MAX_PATH] = {0};
	pPRINT_INFO   pPrint_info=NULL;
	VOS_UINT32	  uExitFlg = VOS_ERR;
	VOS_STRING    pszColor = NULL;
	VOS_UINT16    att = FOREGROUND_INTENSITY|BACKGROUND_INTENSITY;//FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY |BACKGROUND_BLUE ;
	pMANAGE_PRINT g_manageprint =(pMANAGE_PRINT)lpParameter;

	if (NULL == g_manageprint)
	{
		printf("cpss_print_trace_proc manageprint is NULL");
		goto END_PROC;
	}
	g_manageprint->g_traceo_on_off = TRUE;
	
	ulRet = VOS_Wait_Event(&g_manageprint->g_LogEvent,30);//PRINT_SLEEP_TIME
	if (VOS_OK != ulRet)
	{
		VOS_PrintWarn(__FILE__, __LINE__, "wait log event is timeout");
	}
	ulRet = cpss_get_current_path(szCurrentPath);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get current path is NULL");
		goto END_PROC;
	}
	while(1)
	{
		pPrint_info = cpss_get_used_print_trace();
		if (NULL == pPrint_info)
		{
			if (VOS_OK == uExitFlg)
			{
				ulRet = VOS_OK;
				break;
			}
			if (VOS_OK == cpss_check_thread_is_exit(1))
			{
				uExitFlg = VOS_OK;
				continue;
			}
			ulRet = VOS_Wait_Event(&g_manageprint->g_LogEvent, INFINITE);//*PRINT_SLEEP_TIME
			//printf("printf wait 2\n");
			if (VOS_OK != ulRet)
			{
				VOS_PrintWarn(__FILE__, __LINE__, "wait log event is timeout");
			}
			continue;
		}
		//TRUE == g_manageprint->g_traceo_on_off
		if (0 != pPrint_info->szPrintFile[0] && 
			0 != pPrint_info->ulPrintLine)
		{
			fprintf(stdout, "\n");
			switch(pPrint_info->ulPrintType)
			{
			case CPSS_PRINT_ERR:
				cpss_print_key_str(g_manageprint->g_hConsole,RED,NONE,pPrint_info->ulPrintType);
				break;
			case CPSS_PRINT_WARN:
				cpss_print_key_str(g_manageprint->g_hConsole,YELLOW,NONE,pPrint_info->ulPrintType);
				break;
			case CPSS_PRINT_INFO:
				cpss_print_key_str(g_manageprint->g_hConsole,GREEN,NONE,pPrint_info->ulPrintType);
				break;
			case CPSS_PRINT_DBUG:
				cpss_print_key_str(g_manageprint->g_hConsole,BLUE,NONE,pPrint_info->ulPrintType);
				break;
			case CPSS_PRINT_DUMP:
				cpss_print_key_str(g_manageprint->g_hConsole,CYAN,NONE,pPrint_info->ulPrintType);
				break;
			default:
				cpss_print_key_str(g_manageprint->g_hConsole,RED,NONE,pPrint_info->ulPrintType);
				break;
			}
		}

		if (0 == pPrint_info->szPrintFile[0] || 0 == pPrint_info->ulPrintLine)
		{
			if (CPSS_PRINTF_BUFFER == pPrint_info->ulPrintLine)
			{
				sprintf(buffer,"\n%s",pPrint_info->pszPrintInfo);
			}
			else if (CPSS_NO_PRINTF == pPrint_info->ulPrintLine)
			{
				buffer[0] = 0;
			}
			else if (CPSS_PRINTF_BUFCTL == pPrint_info->ulPrintLine)
			{
				sprintf(buffer,"%s",pPrint_info->pszPrintInfo);
			}
			else
			{
				sprintf(buffer,"\n%s",pPrint_info->pszPrintInfo);
			}
		}
		else
		{
			if(CPSS_PRINT_LEVEL_1 == g_manageprint->g_printShowlevel)//所有信息
			{
				sprintf (buffer, "[%s][%s][%04X:%04X][%s_%04d] %s", 	
					pPrint_info->szPrintPid,
					pPrint_info->szPrintTime,
					g_ProcessID,
					pPrint_info->ulThreadID,
					pPrint_info->szPrintFile,
					pPrint_info->ulPrintLine,
					pPrint_info->pszPrintInfo);
			}
			else if(CPSS_PRINT_LEVEL_2 == g_manageprint->g_printShowlevel)//没有文件和行号
			{
				sprintf (buffer, "[%s][%s][%04X:%04X] %s",
					pPrint_info->szPrintPid,
					pPrint_info->szPrintTime,
					g_ProcessID,
					pPrint_info->ulThreadID,
					pPrint_info->pszPrintInfo);
			}
			else if(CPSS_PRINT_LEVEL_3 == g_manageprint->g_printShowlevel)//日期不显示
			{
				sprintf (buffer, "[%s][%04X:%04X][%s_%04d] %s",
					pPrint_info->szPrintPid,
					g_ProcessID,
					pPrint_info->ulThreadID,
					pPrint_info->szPrintFile,
					pPrint_info->ulPrintLine,
					pPrint_info->pszPrintInfo);
			}
			else if(CPSS_PRINT_LEVEL_4 == g_manageprint->g_printShowlevel)//
			{
				sprintf (buffer, "[%s][%04X] %s",
					pPrint_info->szPrintPid,
					pPrint_info->ulThreadID,
					pPrint_info->pszPrintInfo);
			}
			else if(CPSS_PRINT_LEVEL_5 == g_manageprint->g_printShowlevel)
			{
				sprintf (buffer, "%s", pPrint_info->pszPrintInfo);
			}
			else
			{
				sprintf (buffer, "%s", pPrint_info->pszPrintInfo);
			}
		}
		if (0 != buffer[0])
		{
			fprintf(stdout, "%s",buffer);
		}
		fflush(stdout);

		// write log in file
		if (0 == *pPrint_info->szPrintFile || 0 == pPrint_info->ulPrintLine)
		{
			sprintf(buffer,"%s\n",pPrint_info->pszPrintInfo);
		}
		else
		{
			sprintf(buffer,"[%s][%s][%s][%04X][%s_%04d]%s\n",cpss_print_key_str_value(pPrint_info->ulPrintType),
				pPrint_info->szPrintPid,
				pPrint_info->szPrintTime,
				g_ThreadID,
				pPrint_info->szPrintFile,
				pPrint_info->ulPrintLine,
				pPrint_info->pszPrintInfo);
		}
		ulBufferLen = VOS_Strlen(buffer);
		buffer[ulBufferLen] =0;
		//lseek(logfd,ulbufferLen,0);
		
		cpss_write_buffer(szCurrentPath, "trc", buffer, ulBufferLen);
		if (g_manageprint->ulSaveCount == 0XFFFFFFFF)
		{
			g_manageprint->ulSaveCount =0;
		}
		g_manageprint->ulSaveCount++;

		ulRet = cpss_move_print_used_free_trace(pPrint_info);
		if (VOS_OK != ulRet)
		{
			printf("Clear Space Error\n");
		}
	}
	ulRet = VOS_OK;
END_PROC:
	if (VOS_OK != ulRet)
	{
		printf("cpss_print_log Error exit\n");
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_init
 *  Description:	打印平台初始化 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_print_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	BZERO(&g_manageprint, sizeof(MANAGE_PRINT));
	g_manageprint.g_printShowlevel = CPSS_PRINT_LEVEL_1;
	g_manageprint.g_printlevel = CPSS_PRINT_DUMP;
	g_manageprint.ulSaveCount  = 0;
	g_manageprint.g_traceo_on_off = TRUE;
	g_manageprint.ulPrintCount = PRINT_LOG_COUNT/2;
	g_manageprint.g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 获取标准输出设备句柄
	
	ulRet = VOS_Mutex_Init(&g_manageprint.hMutex, "VOS_MUTEX_PRINT_TRACE");
	if (VOS_OK != ulRet)
	{
		printf("cpss init print trace mutex error\n");
		return ulRet;
	}
	
	ulRet = VOS_Init_Event(&g_manageprint.g_LogEvent, "VOS_PRINT_INFO_STRING");
	if (VOS_OK != ulRet)
	{
		printf("cpss init print log event error\n");
		return ulRet;
	}
	
	ulRet = VOS_Init_Event(&g_manageprint.g_MsgEvent, "VOS_PRINT_MSG_INFO_STRING");
	if (VOS_OK != ulRet)
	{
		printf("cpss init print log msg event error\n");
		return ulRet;
	}

	g_manageprint.ulFreePrintCount = g_manageprint.ulPrintCount;
	if (0 < g_manageprint.ulFreePrintCount)
	{		
		g_manageprint.pFreePrintinfoHead = (pPRINT_INFO)VOS_Log_Malloc(sizeof(PRINT_INFO) * g_manageprint.ulPrintCount);
		BZERO(g_manageprint.pFreePrintinfoHead, sizeof(PRINT_INFO) * g_manageprint.ulPrintCount);
		for (uIndex = 0; uIndex < g_manageprint.ulFreePrintCount; uIndex++)
		{
			g_manageprint.pFreePrintinfoHead[uIndex].ulLogID = uIndex + 1;
			g_manageprint.pFreePrintinfoHead[uIndex].ulState = PRINT_INFO_FREE;
			g_manageprint.pFreePrintinfoHead[uIndex].next = &g_manageprint.pFreePrintinfoHead[uIndex+1];
			if (uIndex + 1 < g_manageprint.ulFreePrintCount)
			{
				g_manageprint.pFreePrintinfoHead[uIndex+1].prev = &g_manageprint.pFreePrintinfoHead[uIndex];
				g_manageprint.pFreePrintinfoHead[uIndex+1].next = NULL;
			}
		}
		g_manageprint.pFreePrintinfoTail = &g_manageprint.pFreePrintinfoHead[uIndex - 1];
		g_manageprint.pFreePrintinfoTail->next = NULL;
	}
	
	ulRet = cpss_create_thread(&g_manageprint.hPrintHandle,
								0, 
								cpss_print_trace_proc, 
								(VOS_VOID*)&g_manageprint,
								&g_manageprint.dwThreadId, "Log");
	if (VOS_OK != ulRet)
	{
		printf("cpss init open log thread error\n");
		return ulRet;
	}
	return ulRet;
}		/* -----  end of function cpss_print_init  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_close
 *  Description:	关闭打印平台 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_print_close ()
{
	VOS_UINT32 ulRet = VOS_ERR;
	if (NULL != g_manageprint.g_hConsole)
	{
		CloseHandle(g_manageprint.g_hConsole);
	}
	if (NULL != g_manageprint.hPrintHandle)
	{
		CloseHandle(g_manageprint.hPrintHandle);
	}
	VOS_Mutex_Destroy(&g_manageprint.hMutex);
	VOS_Destory_Event(&g_manageprint.g_LogEvent, 0);
	VOS_Destory_Event(&g_manageprint.g_MsgEvent, 0);
}		/* -----  end of function cpss_print_init  ----- */
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_print
 *  Description:	平台打印函数
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_print(
	VOS_UINT32 	ucProcessPid,		//子模块ID
	VOS_UINT32  ulPrintType,			//日志类型(INFO  ERR  WARM)
	VOS_STRING  szFilename,			//打印日志文件名称
	VOS_UINT32  ulLine,				//打印的行数
	VOS_STRING 	szFormat,			//日志内容
	va_list argptr)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_STRING pszprintfinfo = NULL;
	pPRINT_INFO pPrintInfoTmp = NULL;
	VOS_CHAR   pszFilename[MAX_PATH] = { 0 };
	VOS_INT32  nFileLength = 0;//strlen(szFilename);

	if (CPSS_PRINT_LEVEL_CLOSE == g_manageprint.g_traceo_on_off)
	{
		return VOS_OK;
	}

	if (ulPrintType > g_manageprint.g_printlevel)
	{
		return VOS_OK;
	}

	nFileLength = VOS_Strlen(szFilename);
	pPrintInfoTmp = cpss_get_free_print_trace();
	if (NULL == pPrintInfoTmp)
	{
		printf("CGP Get free trace is NULL!\n");
		return VOS_ERR;
	}

	if (NULL == cpss_vsprintf(pPrintInfoTmp, szFormat, argptr))
	{
		printf("CGP Format trace error\n");
		return VOS_ERR;
	}

	strncpy(pszFilename, szFilename, nFileLength);
	ulRet = cpss_insert_printstr(
			ucProcessPid,
			ulPrintType,
			g_manageprint.g_printShowlevel,
			cpss_notdir(pszFilename),
			ulLine,
			pPrintInfoTmp);
	
	if(VOS_OK != ulRet)
	{
		printf("cpss insert text error\n");
	}
	return VOS_OK;
}
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_print_dump
 *  Description:	平台打印函数
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_print_dump(
		VOS_UINT32 	ucProcessPid,		//子模块ID
		VOS_UINT32  ulPrintType,			//日志类型(INFO  ERR  WARM)
		VOS_STRING  szFilename,			//打印日志文件名称
		VOS_UINT32  ulLine,				//打印的行数
		VOS_VOID * pstrVoid,
		VOS_UINT32 ulLeng)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_STRING szProcessPidName = NULL;
	pPRINT_INFO pPrintInfoTmp = NULL;
	VOS_CHAR   pszFilename[MAX_PATH] = {0};
	VOS_CHAR   szTime[CPSS_MAX_TIME] = {0};
	VOS_INT32  nFileLength = 0;//strlen(szFilename);

	if (CPSS_PRINT_LEVEL_CLOSE == g_manageprint.g_traceo_on_off)
	{
		return VOS_OK;
	}

	if (ulPrintType > g_manageprint.g_printlevel)
	{
		return VOS_OK;
	}

	nFileLength = VOS_Strlen(szFilename);
	pPrintInfoTmp = cpss_get_free_print_trace();
	if (NULL == pPrintInfoTmp)
	{
		printf("CGP Get free trace is NULL!\n");
		return VOS_ERR;
	}

	if (NULL == pPrintInfoTmp->pszPrintInfo)
	{
		pPrintInfoTmp->pszPrintInfo = (VOS_CHAR*)VOS_Log_Malloc(ulLeng);
		BZERO(pPrintInfoTmp->pszPrintInfo, ulLeng);
		if (NULL == pPrintInfoTmp->pszPrintInfo)
		{
			printf("CGP memory for trace is NULL!\n");
			return VOS_ERR;
		}
	}
	else
	{
		printf("CGP log print info is not empty!\n");
		return VOS_ERR;
	}
	VOS_Memcpy(pPrintInfoTmp->pszPrintInfo, pstrVoid, ulLeng);
	pPrintInfoTmp->ulPrintSize = ulLeng;

	strncpy(pszFilename, szFilename, nFileLength);
	ulRet = cpss_insert_printstr(
			ucProcessPid,
			ulPrintType,
			g_manageprint.g_printShowlevel,
			cpss_notdir(pszFilename),
			ulLine,
			pPrintInfoTmp);
	
	if(VOS_OK != ulRet)
	{
		printf("cpss insert text error\n");
	}
	return VOS_OK;
}

/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void VOS_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MAIN, CPSS_PRINT_INFO, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintError
 *  Description:  
 * ========================================================================*/
void VOS_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MAIN, CPSS_PRINT_ERR,
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void VOS_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MAIN, CPSS_PRINT_WARN, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintDebug
 *  Description:  
 * ========================================================================*/
void VOS_PrintDebug (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MAIN, CPSS_PRINT_DBUG, 
		szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_PrintDebug  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintBuffer
 *  Description:  
 * ========================================================================*/
VOS_UINT32 VOS_PrintBuffer (
		VOS_VOID * pstuBuffer,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR	*pstuBufTmp = NULL;
	VOS_CHAR	**pstuBufRtn = NULL;
	VOS_CHAR	*pstuFmtBuf = NULL;
	PRINT_INFO	stuPrintInfoBuffer;				//打印到缓存buffer中
	VOS_UINT32 ulOffset = 0;
	VOS_UINT32 ulCpLen = 0;

	if (pstuBuffer == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer is param is error");
		goto END_PROC;
	}
	pstuBufRtn = pstuBuffer;
	BZERO(&stuPrintInfoBuffer, sizeof(PRINT_INFO));
	
	va_start(ap, fmt);
	pstuBufTmp = cpss_vsprintf(&stuPrintInfoBuffer, fmt, ap);
	va_end(ap);
	if (NULL == pstuBufTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer get memory is error");
		goto END_PROC;
	}
	if (*pstuBufRtn == NULL)
	{
		pstuFmtBuf = (VOS_CHAR*)VOS_Log_Remset(pstuBufTmp);
	}
	else
	{
		pstuFmtBuf = (VOS_CHAR*)VOS_Log_Strcat(*pstuBufRtn, pstuBufTmp);
		VOS_Log_Free(pstuBufTmp);
	}
	if (NULL == pstuFmtBuf)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer get memory is error2");
		goto END_PROC;
	}
	*pstuBufRtn = pstuFmtBuf;
	ulRet = VOS_OK;
END_PROC:
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer is error exit");
	}
	return ulRet;
}
/* ===  FUNCTION  =========================================================
*         Name:  VOS_PrintBufferRelease
*  Description:
* ========================================================================*/
VOS_UINT32 VOS_PrintBufferRelease(VOS_VOID * pstuBuffer)
{
	VOS_UINT32 ulRet = VOS_OK;

	if (pstuBuffer == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer is param is error");
		goto END_PROC;
	}
	VOS_Log_Free(pstuBuffer);
	ulRet = VOS_OK;
END_PROC:
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "print buffer is error exit");
	}
	return ulRet;
}
/*
#define VOS_Printf(fmt, ...) {\
	va_list arg_ptr = NULL;\
	va_start(arg_ptr, fmt);\
	if(VOS_OK != cpss_print(CPSS_PID_MAIN, CPSS_PRINT_INFO, \
		__FILE__, __LINE__, fmt, arg_ptr))\
	{\
		printf("print info error!\n");\
	}\
	va_end(arg_ptr);\
}*/
/*__FUNCTION__*/
