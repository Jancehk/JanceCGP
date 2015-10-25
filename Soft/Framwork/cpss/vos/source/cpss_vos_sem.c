/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.c
 *
 *    Description:  �ź���
 *
 *        Version:  1.0
 *        Created:  2011��06��09�� 23ʱ34��22��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 * =====================================================================================
 */
#include "cpss_vos_sem.h"
#include "cpss_vk_socket.h"
static CPSS_MSG_SEM_MANAGE *g_cpsMsgSem_Manage;

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_move_a_to_b
 *  Description:  a��Ϣ��b�������Ƿ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG * cpss_msg_find_a_in_b(CPSS_MSG *pMsgList,CPSS_MSG *pMsgInfo)
{
	while(NULL != pMsgList)
	{
		if (pMsgList == pMsgInfo)
		{
			return pMsgInfo;
		}
		pMsgList = pMsgList->next;
	}
	return NULL;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_add_a_to_b
 *  Description:  a��Ϣ���ӵ���b������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID cpss_msg_memset(CPSS_MSG *pMsgInfo)
{
	VOS_Memset(&pMsgInfo->Body,sizeof(CPSS_COM_DATA));
//	pMsgInfo->ulBufLength = CPSS_MSG_BUFFER_SIZE;
	pMsgInfo->nStat = CPSS_MSG_STAT_FREE;
	pMsgInfo->pClient = NULL;
/*	
	VOS_PrintInfo(__FILE__, __LINE__, "memset kill timer is:%p:%p ",pMsgInfo,pMsgInfo->pTimer);
*/
	if (NULL != pMsgInfo->pTimer)
	{
/*		cpss_kill_timer(pMsgInfo->pTimer);*/
		pMsgInfo->pTimer = NULL;
	}
	VOS_Memset(&pMsgInfo->Body,sizeof(CPSS_COM_DATA));
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_add_a_to_b
 *  Description:  a��Ϣ���ӵ���b������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG * cpss_msg_add_a_to_b(CPSS_MSG **ppMsgListH, CPSS_MSG **ppMsgListT, CPSS_MSG *pMsgInfo)
{
	if (NULL == ppMsgListH || NULL == ppMsgListT)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Add msg list tail is NULL");
		return NULL;
	}
	
	if ((NULL == *ppMsgListH && NULL != *ppMsgListT) || NULL != *ppMsgListH && NULL == *ppMsgListT)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Add msg list value is NULL");
		return NULL;
	}else
	if (NULL == *ppMsgListH && NULL == *ppMsgListT)
	{
		*ppMsgListT = pMsgInfo;
		*ppMsgListH = pMsgInfo;
		pMsgInfo->next = NULL;
		pMsgInfo->prev = NULL;
	}else
	{
		/* *ppMsgListT->next = pMsgInfo;*/
		pMsgInfo->prev = *ppMsgListT;
		pMsgInfo->prev->next = pMsgInfo;
		*ppMsgListT = pMsgInfo;
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_print_msgtab_debug
*  Description: 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static VOS_VOID cpss_print_msginfo_debug(VOS_VOID * pstuBuffer,CPSS_MSG *pstuMsg)
{
	VOS_CHAR *pstrStat[] = {"Free","Resv","Useg","Used"};
	while(NULL != pstuMsg)
	{
		VOS_PrintBuffer(pstuBuffer, "msg tab link [%p]->[%p][ID:%d Stat:%d:%s Clit:%p Len:%d]\n", 
			pstuMsg,
			pstuMsg->next,
			pstuMsg->Body.msghead.ulMsgID,
			pstuMsg->nStat,
			pstrStat[pstuMsg->nStat-CPSS_MSG_STAT_FREE],
			pstuMsg->pClient,
			pstuMsg->ulMsgLength);
		pstuMsg = pstuMsg->next;
	}
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_print_msgtab_debug
*  Description: 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static VOS_VOID cpss_print_msgtab_debug(VOS_UINT32 ulType)
{
	CPSS_MSG *pstuMsg = NULL;
	CPSS_MEM_BUFFER		stuBuffer  = {0};
	pCPSS_MEM_BUFFER  	pstuBuffer = NULL;
	
	switch(ulType)
	{
	case MOVE_TYPE_USE_MSG_TCP_RECV_FREE_TO_USED:
		VOS_PrintBuffer(&stuBuffer,"******[TCP Recv Free To Used]******");
		break;
	case MOVE_TYPE_USE_MSG_UDP_RECV_FREE_TO_USED:
		VOS_PrintBuffer(&stuBuffer,"******[UDP Recv Free To Used]******");
		break;
	case MOVE_TYPE_USE_MSG_TCP_SEND_FREE_TO_USED:
		VOS_PrintBuffer(&stuBuffer,"******[TCP Send Free To Used]******");
		break;
	case MOVE_TYPE_USE_MSG_UDP_SEND_FREE_TO_USED:
		VOS_PrintBuffer(&stuBuffer,"******[UDP Send Free To Used]******");
		break;
	case MOVE_TYPE_USE_MSG_TCP_RECV_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[TCP Recv Used To Free]******");
		break;
	case MOVE_TYPE_USE_MSG_UDP_RECV_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[UDP Recv Used To Free]******");
		break;
	case MOVE_TYPE_USE_MSG_TCP_SEND_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[TCP Send Used To Free]******");
		break;
	case MOVE_TYPE_USE_MSG_UDP_SEND_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[UDP Send Used To Free]******");
		break;
	case MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[TCP ID Recv Used To Free]******");
		break;
	case MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[UDP ID Recv Used To Free]******");
		break;
	case MOVE_TYPE_USE_MSG_RESE_TO_FREE:
		VOS_PrintBuffer(&stuBuffer,"******[Rese To Free]******");
		break;
	default:
		VOS_PrintBuffer(&stuBuffer,"******[TYPE is Error]******");
		break;
	}
	
	VOS_PrintBuffer(&stuBuffer,"\nMsg Free Table list\nMsg Total [%d]\nFree cont %d head[%p]-tail[%p]\n", 
		g_cpsMsgSem_Manage->msgtab.nMsgTotalCount,
		g_cpsMsgSem_Manage->msgtab.nFreeCount,
		g_cpsMsgSem_Manage->msgtab.pFreeHead,
		g_cpsMsgSem_Manage->msgtab.pFreeTial);
	cpss_print_msginfo_debug(&stuBuffer, g_cpsMsgSem_Manage->msgtab.pFreeHead);
		
	VOS_PrintBuffer(&stuBuffer,"Msg Used Table list \nT-S:%p C:%04d head[%p]-tail[%p]\n",
		&g_cpsMsgSem_Manage->msgtab.StyleSend.TypeTCP,
		g_cpsMsgSem_Manage->msgtab.StyleSend.TcpCount,
		g_cpsMsgSem_Manage->msgtab.StyleSend.TypeTCP.pUseHead,
		g_cpsMsgSem_Manage->msgtab.StyleSend.TypeTCP.pUseTial);
	cpss_print_msginfo_debug(&stuBuffer, g_cpsMsgSem_Manage->msgtab.StyleSend.TypeTCP.pUseHead);

	VOS_PrintBuffer(&stuBuffer, "T-R:%p C:%04d head[%p]-tail[%p]\n", 
		&g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeTCP,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.TcpCount,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeTCP.pUseHead,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeTCP.pUseTial);
	cpss_print_msginfo_debug(&stuBuffer, g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeTCP.pUseHead);

/***/
	
	VOS_PrintBuffer(&stuBuffer,"U-S:%p C:%04d head[%p]-tail[%p]\n",
		&g_cpsMsgSem_Manage->msgtab.StyleSend.TypeUDP,
		g_cpsMsgSem_Manage->msgtab.StyleSend.UdpCount,
		g_cpsMsgSem_Manage->msgtab.StyleSend.TypeUDP.pUseHead,
		g_cpsMsgSem_Manage->msgtab.StyleSend.TypeUDP.pUseTial);
	cpss_print_msginfo_debug(&stuBuffer, g_cpsMsgSem_Manage->msgtab.StyleSend.TypeUDP.pUseHead);
	
	VOS_PrintBuffer(&stuBuffer, "U-R:%p C:%04d head[%p]-tail[%p]\n", 
		&g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeUDP,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.UdpCount,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeUDP.pUseHead,
		g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeUDP.pUseTial);
	cpss_print_msginfo_debug(&stuBuffer, g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeUDP.pUseHead);
/**/




	VOS_PrintInfo("", CPSS_PRINTF_BUFFER, "%s", stuBuffer.strBuffer);
	pstuBuffer = stuBuffer.next;
	while (NULL != pstuBuffer)
	{
		VOS_PrintInfo("", CPSS_PRINTF_BUFCTL, "%s",stuBuffer.strBuffer);
		pstuBuffer = (pCPSS_MEM_BUFFER)cpss_get_next_buffer(stuBuffer);
	}
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_del_a_to_b
 *  Description:  a��Ϣ��b������ɾ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG * cpss_msg_del_a_to_b(CPSS_MSG **ppMsgListH,
									  CPSS_MSG **ppMsgListT,
									  CPSS_MSG *pMsgInfo)
{
	CPSS_MSG * pMsgList;
	if (NULL == ppMsgListT || NULL == ppMsgListH)
	{
		VOS_PrintErr(__FILE__,__LINE__,"msg head is NULL");
		return NULL;
	}
	if (NULL == *ppMsgListT || NULL == *ppMsgListH)
	{
		VOS_PrintErr(__FILE__,__LINE__,"msg head value is NULL");
		return NULL;
	}
	pMsgList = *ppMsgListH;
	while(NULL != pMsgList)
	{
		/*find in tab list*/
		if (pMsgList != pMsgInfo)
		{
			pMsgList = pMsgList->next;
			continue;
		}
		/* in head one */
		if (NULL == pMsgList->prev &&
			NULL != pMsgList->next)   //��һ��
		{
			pMsgList->next->prev = NULL;
			*ppMsgListH = pMsgInfo->next;
		}else  /* in last one*/ 
		if (NULL != pMsgList->prev &&
			NULL == pMsgList->next)  //���һ��
		{
			pMsgList->prev->next = NULL;
			*ppMsgListT = pMsgList->prev;
		}else /* in middle one*/
		if (NULL != pMsgList->prev &&
			NULL != pMsgList->next)
		{
			pMsgList->next->prev = pMsgList->prev;
			pMsgList->prev->next = pMsgList->next;
		}else /*just only one*/ 
		if (NULL == pMsgList->prev &&
			NULL == pMsgList->next)
		{
			*ppMsgListH = NULL;
			*ppMsgListT = NULL;
		}
		break;
	}
	if (pMsgInfo != NULL)
	{
		pMsgInfo->next = NULL;
		pMsgInfo->prev = NULL;
	}
	else
	{
		VOS_PrintErr(__FILE__,__LINE__,"msginfo is NULL");
	}
	return pMsgInfo;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_move_a_to_b
 *  Description:  ��tcp��send��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG * cpss_get_msg_use_id_in_tab(CPSS_MSG_TAB *pMsgTab, VOS_UINT32 ulMsgID)
{
	CPSS_MSG * tmpMsg = NULL;
	
	if (NULL == pMsgTab)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input table is NULL");
		return tmpMsg;
	}
	tmpMsg = pMsgTab->pUseHead;
	while(NULL != tmpMsg)
	{
		if (tmpMsg->Body.msghead.ulMsgID = ulMsgID)
		{
			break;
		}
	}
	return tmpMsg;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_msg_move_a_to_b
 *  Description:  ��tcp��send��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_move_msg_a_to_b(CPSS_MSG_TAB *pMsgTab, CPSS_MSG * pMsgInfo, VOS_UINT8 Type)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG * a = NULL;
	CPSS_MSG * b = NULL;
	switch (Type)
	{
	case MOVE_USED_TO_FREE:
		// find 
		a = cpss_msg_find_a_in_b(pMsgTab->pUseHead, pMsgInfo);
		if (NULL == a)
		{
			a = cpss_msg_find_a_in_b(g_cpsMsgSem_Manage->msgtab.pFreeHead, pMsgInfo);
			if (NULL != a)
			{
				uRet = VOS_OK;
			}
			else
			{
				VOS_PrintErr(__FILE__, __LINE__, "UF no find F:%p U:%p in %p",
					g_cpsMsgSem_Manage->msgtab.pFreeHead,pMsgInfo,pMsgTab);
			}
			goto END_PROC;
		}
		// del used list
		cpss_msg_memset(a);
		b = cpss_msg_del_a_to_b(&pMsgTab->pUseHead, &pMsgTab->pUseTial, a);
		if (NULL == b)
		{
			VOS_PrintErr(__FILE__, __LINE__, "delete msg is error :%p",a);
		}

		// add free list
		b = cpss_msg_add_a_to_b(&g_cpsMsgSem_Manage->msgtab.pFreeHead, &g_cpsMsgSem_Manage->msgtab.pFreeTial, a);
		if (NULL == b)
		{
			VOS_PrintErr(__FILE__, __LINE__, "add msg is error :%p",a);
		}
		a->nStat = CPSS_MSG_STAT_FREE;
		uRet = VOS_OK;
		break;
	case MOVE_FREE_TO_USED:
		// find 
		a = cpss_msg_find_a_in_b(g_cpsMsgSem_Manage->msgtab.pFreeHead, pMsgInfo);
		if (NULL == a)
		{
			a = cpss_msg_find_a_in_b(pMsgTab->pUseHead, pMsgInfo);
			if (NULL != a)
			{
				uRet = VOS_OK;
			}
			else
			{
				VOS_PrintErr(__FILE__, __LINE__, "FU no find F:%p U:%p in %p",
					g_cpsMsgSem_Manage->msgtab.pFreeHead,pMsgInfo,pMsgTab);
			}
			goto END_PROC;
		}
		// del used list
		b = cpss_msg_del_a_to_b(&g_cpsMsgSem_Manage->msgtab.pFreeHead,
			&g_cpsMsgSem_Manage->msgtab.pFreeTial, a);
		if (NULL == b)
		{
			VOS_PrintErr(__FILE__, __LINE__, "delete msg is error :%p",a);
		}
		// add free list
		b = cpss_msg_add_a_to_b(&pMsgTab->pUseHead, &pMsgTab->pUseTial, a);
		if (NULL == b)
		{
			VOS_PrintErr(__FILE__, __LINE__, "add msg is error :%p",a);
		}
		a->nStat = CPSS_MSG_STAT_USEING;
		uRet = VOS_OK;
		break;
	default:
		VOS_PrintErr(__FILE__, __LINE__, "input type is error :%d",Type);
		uRet = VOS_ERR;
		break;
	}
END_PROC:
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "move is error :%d:%p",Type,pMsgInfo);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_msg_use_msg
 *  Description:  ����Ϣ���͸����ķ�ʽ�ƶ���Ϣ
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_move_msg_use_msg(CPSS_MSG *pMsgInfo, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG_STYLE *pMsgtab = &g_cpsMsgSem_Manage->msgtab;
	CPSS_MSG_TAB *  pTypeTable = NULL;
	VOS_UINT32	 *  pUsedCount = NULL;
	VOS_UINT8		nType = 0;

	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input msg info is error");
		return uRet;
	}
	if (NULL == pMsgtab)
	{
		VOS_PrintErr(__FILE__,__LINE__,"move msg info system error");
		return uRet;
	}

	// add a lock
	uRet = VOS_Mutex_Lock(&g_cpsMsgSem_Manage->hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return uRet;
	}
	uRet = VOS_ERR;

	if (MOVE_TYPE_USE_MSG_RESE_TO_FREE == uType)
	{
		cpss_msg_memset(pMsgInfo);
		goto END_PROC;
	}
	switch(uType)
	{
	case MOVE_TYPE_USE_MSG_TCP_RECV_FREE_TO_USED:
		pTypeTable = &pMsgtab->StyleRecv.TypeTCP;
		nType = MOVE_FREE_TO_USED;
		pUsedCount  = &pMsgtab->StyleRecv.TcpCount;
		break;
	case MOVE_TYPE_USE_MSG_UDP_RECV_FREE_TO_USED:
		pTypeTable = &pMsgtab->StyleRecv.TypeUDP;
		nType = MOVE_FREE_TO_USED;
		pUsedCount  = &pMsgtab->StyleRecv.UdpCount;
		break;
	case MOVE_TYPE_USE_MSG_TCP_SEND_FREE_TO_USED:
		pTypeTable = &pMsgtab->StyleSend.TypeTCP;
		nType = MOVE_FREE_TO_USED;
		pUsedCount  = &pMsgtab->StyleSend.TcpCount;
		break;
	case MOVE_TYPE_USE_MSG_UDP_SEND_FREE_TO_USED:
		pTypeTable = &pMsgtab->StyleSend.TypeUDP;
		nType = MOVE_FREE_TO_USED;
		pUsedCount  = &pMsgtab->StyleSend.UdpCount;
		break;
	case MOVE_TYPE_USE_MSG_TCP_RECV_USED_TO_FREE:
		pTypeTable = &pMsgtab->StyleRecv.TypeTCP;
		nType = MOVE_USED_TO_FREE;
		pUsedCount  = &pMsgtab->StyleRecv.TcpCount;
		break;
	case MOVE_TYPE_USE_MSG_UDP_RECV_USED_TO_FREE:
		pTypeTable = &pMsgtab->StyleRecv.TypeUDP;
		nType = MOVE_USED_TO_FREE;
		pUsedCount  = &pMsgtab->StyleRecv.UdpCount;
		break;
	case MOVE_TYPE_USE_MSG_TCP_SEND_USED_TO_FREE:
		pTypeTable = &pMsgtab->StyleSend.TypeTCP;
		nType = MOVE_USED_TO_FREE;
		pUsedCount  = &pMsgtab->StyleSend.TcpCount;
		break;
	case MOVE_TYPE_USE_MSG_UDP_SEND_USED_TO_FREE:
		pTypeTable = &pMsgtab->StyleSend.TypeUDP;
		nType = MOVE_USED_TO_FREE;
		pUsedCount  = &pMsgtab->StyleSend.UdpCount;
		break;
	default:
		VOS_PrintErr(__FILE__,__LINE__,"type is error:%d Msg:%p ",uType,pMsgtab);
		break;
	}
	if (NULL == pTypeTable || NULL == pUsedCount)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Msg:%p table or cout is NULL %p:%p ",
			pMsgtab, pTypeTable,pUsedCount);
	}
	uRet = cpss_move_msg_a_to_b(pTypeTable,pMsgInfo,nType);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"move error Table:%p Msg:%p Count:%p:%d Type:%d:%s",
			pTypeTable,pMsgInfo,pUsedCount,*pUsedCount,nType,
			nType==MOVE_USED_TO_FREE?"U-F":"F-U");
		goto ERROR_EXIT;
	}
	if (MOVE_USED_TO_FREE == nType)
	{
		*pUsedCount = *pUsedCount-1;
		pMsgtab->nFreeCount++;
	}else
	if (MOVE_FREE_TO_USED == nType)
	{
		pMsgtab->nFreeCount--;
		*pUsedCount = *pUsedCount+1;
	}
	else
	{
		VOS_PrintErr(__FILE__,__LINE__,"type error Table:%p Msg:%p Count:%p:%d Type:%d:%s",
			pTypeTable,pMsgInfo,pUsedCount,*pUsedCount,nType,
			nType==MOVE_USED_TO_FREE?"U-F":"F-U");
	}
END_PROC:
	uRet = VOS_OK;
//	cpss_print_msgtab_debug(uType);
	// del a lock
ERROR_EXIT:
	if (uRet != VOS_OK)
	{
		VOS_PrintErr(__FILE__,__LINE__,"remove faild");
	}
	if (VOS_OK != VOS_Mutex_Unlock(&g_cpsMsgSem_Manage->hMutex))
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_msg_use_msg
 *  Description:  ����Ϣ���͸����ķ�ʽ�ƶ���Ϣ
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG	* cpss_get_msg_use_id(VOS_UINT32 uMsgID, VOS_UINT32 uType,VOS_UINT8 *puTypeNext)
{
	CPSS_MSG_STYLE *pMsgtab = &g_cpsMsgSem_Manage->msgtab;
	CPSS_MSG_TAB   *pTypeTbale = NULL;
	CPSS_MSG	   *pMsgInfo = NULL;
	
	if (0 == uMsgID)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input msg info is error");
		return NULL;
	}
	if (NULL == pMsgtab)
	{
		VOS_PrintErr(__FILE__,__LINE__,"move msg info system error");
		return NULL;
	}
	switch(uType)
	{
	case MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE:
		pTypeTbale = &pMsgtab->StyleRecv.TypeTCP;
		*puTypeNext = MOVE_TYPE_USE_MSG_TCP_RECV_USED_TO_FREE;
		break;
	case MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE:
		pTypeTbale = &pMsgtab->StyleRecv.TypeUDP;
		*puTypeNext = MOVE_TYPE_USE_MSG_UDP_RECV_USED_TO_FREE;
		break;
	case MOVE_TYPE_USE_MID_TCP_SEND_USED_TO_FREE:
		pTypeTbale = &pMsgtab->StyleSend.TypeTCP;
		*puTypeNext = MOVE_TYPE_USE_MSG_TCP_SEND_USED_TO_FREE;
		break;
	case MOVE_TYPE_USE_MID_UDP_SEND_USED_TO_FREE:
		pTypeTbale = &pMsgtab->StyleSend.TypeUDP;
		*puTypeNext = MOVE_TYPE_USE_MSG_UDP_SEND_USED_TO_FREE;
		break;
	default:
		VOS_PrintErr(__FILE__,__LINE__,"type is error:%d",uType);
		break;
	}
	if (0 == *puTypeNext)
	{
		VOS_PrintErr(__FILE__,__LINE__,"type table is NULL");
		goto EXITPROC;
	}
	pMsgInfo = cpss_get_msg_use_id_in_tab(pTypeTbale, uMsgID);
	if (NULL == pMsgInfo)
	{
		cpss_print_msgtab_debug(*puTypeNext);
	}
EXITPROC:
	return pMsgInfo;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_msg_use_msg
 *  Description:  ����Ϣ���͸����ķ�ʽ�ƶ���Ϣ
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_move_msg_use_msgid(VOS_UINT32 uMsgID, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG	   *pMsgInfo = NULL;
	VOS_UINT8      uTypeNext = 0;
	pMsgInfo = cpss_get_msg_use_id(uMsgID,uType,&uTypeNext);
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"not find in %d:%d",uMsgID,uType);
		goto EXITPROC;
	}
	uRet = cpss_move_msg_use_msg(pMsgInfo,uTypeNext);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"move a to b error");
		goto EXITPROC;
	}
EXITPROC:
	// del a lock
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"msg_use_msgid error");
	}
	return uRet;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_init_msg_sem
 *  Description:  ƽ̨��ʼ����Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_init_msg_sem()
{
	VOS_UINT32 uRet = VOS_ERR;
	g_cpsMsgSem_Manage = (CPSS_MSG_SEM_MANAGE *)cpss_get_msgtab();
	if (NULL == g_cpsMsgSem_Manage)
	{
		VOS_PrintErr(__FILE__,__LINE__,"cpss init msg manage error");
		return uRet;
	}
	uRet = VOS_Mutex_Init(&g_cpsMsgSem_Manage->hMutex, "VOS_MUTEX_MSG_SEM");
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"cpss init msg mutex error");
		return uRet;
	}
	g_cpsMsgSem_Manage->msgtab.nMsgTotalCount = 0;
	g_cpsMsgSem_Manage->msgtab.nFreeCount = 0;
	g_cpsMsgSem_Manage->msgtab.nIDCount = 1;
	VOS_Memset(&g_cpsMsgSem_Manage->msgtab, sizeof(CPSS_MSG_STYLE));
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_get_msg
 *  Description:  ������Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static CPSS_MSG * cps_get_msg()
{
	VOS_CHAR * pstrBuff = NULL;
	CPSS_MSG * msgTmp = (CPSS_MSG *)VOS_Malloc(sizeof(CPSS_MSG),"get msg info");
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get msg address error");
		return msgTmp;
	}
	VOS_Memset(msgTmp, sizeof(CPSS_MSG));
	
	g_cpsMsgSem_Manage->msgtab.nMsgTotalCount++;
	g_cpsMsgSem_Manage->msgtab.nFreeCount++;
	/*
	pstrBuff = (VOS_CHAR *)VOS_Malloc(sizeof(VOS_CHAR)*CPSS_MSG_BUFFER_SIZE);
	if (NULL == pstrBuff)
	{
		VOS_Free(msgTmp, sizeof(CPSS_MSG));
		VOS_PrintErr(__FILE__,__LINE__,"get msg address error");
		return msgTmp;
	}*/
//	msgTmp->ulBufLength = CPSS_MSG_BUFFER_SIZE;
	//msgTmp->Head.pUcBuf = pstrBuff;
	return msgTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_msg_info
 *  Description:  ������Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
CPSS_MSG * cpss_get_msg_info()
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG	 * pMsgTmp = NULL;
	CPSS_MSG     * stuMsg = NULL;
	// add a lock
	uRet = VOS_Mutex_Lock(&g_cpsMsgSem_Manage->hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
		return pMsgTmp;
	}
	if (NULL == g_cpsMsgSem_Manage->msgtab.pFreeHead)
	{
		pMsgTmp = cps_get_msg();
		if (NULL == pMsgTmp)
		{
			VOS_PrintErr(__FILE__,__LINE__,"get msg address error");
			pMsgTmp = NULL;
			goto ERROR_EXIT;
		}
		g_cpsMsgSem_Manage->msgtab.pFreeHead = pMsgTmp;
		g_cpsMsgSem_Manage->msgtab.pFreeTial = pMsgTmp;
	}
	else
	{
		pMsgTmp = g_cpsMsgSem_Manage->msgtab.pFreeHead;

		while(NULL != pMsgTmp)
		{
			if (CPSS_MSG_STAT_FREE == pMsgTmp->nStat)
			{
				pMsgTmp->nStat = CPSS_MSG_STAT_RESERVE;
				break;
			}
			pMsgTmp= pMsgTmp->next;
		}
		if (NULL != pMsgTmp)
		{
			goto END_PROC;
		}
		pMsgTmp = cps_get_msg();
		if (NULL == pMsgTmp)
		{
			VOS_PrintErr(__FILE__,__LINE__,"get msg address error");
			pMsgTmp = NULL;
			goto ERROR_EXIT;
		}
		if (NULL == g_cpsMsgSem_Manage->msgtab.pFreeTial)
		{
			VOS_Free(pMsgTmp, sizeof(CPSS_MSG));
			VOS_PrintErr(__FILE__,__LINE__,"get msg address error");
			pMsgTmp = NULL;
			goto ERROR_EXIT;
		}		
		g_cpsMsgSem_Manage->msgtab.pFreeTial->next = pMsgTmp;
		pMsgTmp->prev = g_cpsMsgSem_Manage->msgtab.pFreeTial;
		g_cpsMsgSem_Manage->msgtab.pFreeTial = pMsgTmp;
	}
	
END_PROC:
	if (g_cpsMsgSem_Manage->msgtab.nIDCount >= 0X7fffffff)
	{
		VOS_PrintInfo(__FILE__, __LINE__, "Msg Count is most big:%08x,and set zero",
			g_cpsMsgSem_Manage->msgtab.nIDCount);
		g_cpsMsgSem_Manage->msgtab.nIDCount = 0;
	}

ERROR_EXIT:
	if (NULL != pMsgTmp)
	{
		VOS_Memset(&pMsgTmp->Body, sizeof(CPSS_COM_DATA));
		g_cpsMsgSem_Manage->msgtab.nIDCount++;
		pMsgTmp->Body.msghead.ulMsgID = g_cpsMsgSem_Manage->msgtab.nIDCount;
		pMsgTmp->ulMsgLength = 0;
		pMsgTmp->nStat = CPSS_MSG_STAT_RESERVE;
		pMsgTmp->nType = CPSS_MSG_TYPE_FREE;
	}
	else
	{
		VOS_PrintErr(__FILE__,__LINE__,"get msg is error null");
	}
	// del a lock
	uRet = VOS_Mutex_Unlock(&g_cpsMsgSem_Manage->hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add lock error");
	}
	return pMsgTmp;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_send_free_to_used
 *  Description:  ��tcp��send��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_send_free_to_used(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_TCP_SEND_FREE_TO_USED);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move T_S Queued F-U",pMsgInfo);
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_recv_free_to_used
 *  Description:  ��tcp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_free_to_used(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_TCP_RECV_FREE_TO_USED);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move T_R Queued F-U",pMsgInfo);
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_tcp_send_used_to_free
*  Description:  ��tcp��send��Ϣ��ʹ�õ��б��ƶ�������
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_move_tcp_send_used_to_free(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_TCP_SEND_USED_TO_FREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move T_S Queued U-F",pMsgInfo);
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_recv_used_to_free
 *  Description:  ��tcp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_used_to_free(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_CLIENT_INFO pClientInfo = NULL;
	if (NULL != pMsgInfo->pClient)
	{
		pClientInfo = (pCPSS_CLIENT_INFO)pMsgInfo->pClient;
	}
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_TCP_RECV_USED_TO_FREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move T_R Queued U-F",pMsgInfo);
	}
	return uRet;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_send_free_to_used(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_UDP_SEND_FREE_TO_USED);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move U_S Queued F-U",pMsgInfo);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_udp_send_free_to_used
*  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_move_udp_send_used_to_free(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_UDP_SEND_USED_TO_FREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move U_S Queued U-F",pMsgInfo);
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_used_to_free(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_UDP_RECV_USED_TO_FREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move U_R Queued U-F",pMsgInfo);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_udp_send_free_to_used
*  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_free_to_used(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_UDP_RECV_FREE_TO_USED);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move U_R Queued F-U",pMsgInfo);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_reservefree_to_free
*  Description:  ��reserve��free��Ϣ�ӿ��е��б��ƶ���ʹ����
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_move_reserve_to_free(CPSS_MSG *pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msg(pMsgInfo, MOVE_TYPE_USE_MSG_RESE_TO_FREE);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%p]move Queued R-F",pMsgInfo);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_delete_recv_msg_use_msgid
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ������Ƴ������ж���
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID, VOS_UINT8 nType)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msgid(ulMsgID, MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE);
	if (VOS_OK != uRet && 0 == nType)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%d]move ID T_R Queued F-U",ulMsgID);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_delete_recv_msg_use_msgid
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ������Ƴ������ж���
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID, VOS_UINT8 nType)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_msg_use_msgid(ulMsgID, MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE);
	if (VOS_OK != uRet && 0 == nType)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%d]move ID D_R Queued F-U",ulMsgID);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_delete_recv_msg_use_msgid
*  Description:  ������ϢID��tcp ��Ϣ���ܶ������Ƴ������ж���
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
VOS_UINT32 cpss_move_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_move_tcp_recv_used_to_free_use_msgid(ulMsgID,1);
	if (VOS_OK == uRet)
	{
		return uRet;
	}
	uRet = cpss_move_udp_recv_used_to_free_use_msgid(ulMsgID,1);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "[%d]move ID D_R Queued F-U-T", ulMsgID);
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_recv_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"not find in T_R:%d",ulMsgID);
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_recv_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"not find in U_R:%d",ulMsgID);
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_send_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"not find in T_S:%d",ulMsgID);
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_send_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"not find in U_S:%d",ulMsgID);
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	VOS_PrintErr(__FILE__,__LINE__,"not find in U:%d",ulMsgID);
END_PROC:
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	VOS_PrintErr(__FILE__,__LINE__,"not find in T:%d",ulMsgID);
END_PROC:
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_send_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_SEND_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	VOS_PrintErr(__FILE__,__LINE__,"not find in S:%d",ulMsgID);
END_PROC:
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_recv_msg_for_id(VOS_UINT32 ulMsgID)
{
	VOS_UINT8 uTypeNext = 0;
	CPSS_MSG * pMsgInfo = NULL;
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	pMsgInfo = cpss_get_msg_use_id(ulMsgID,MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE,&uTypeNext);
	if (NULL != pMsgInfo)
	{
		goto END_PROC;
	}
	VOS_PrintErr(__FILE__,__LINE__,"not find in R:%d",ulMsgID);
END_PROC:
	return pMsgInfo;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
CPSS_MSG * cpss_get_used_msg(VOS_UINT32 ulPID, VOS_UINT32 uType)
{
	pCPSS_MSG_TAB pMsgTypeTab = NULL;
	CPSS_MSG * pMsgInfo = NULL;
	VOS_UINT32 uRet = VOS_ERR;

	switch(uType)
	{
	case GET_TCP_RECV_USED:
		pMsgTypeTab = &g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeTCP;
		break;
	case GET_UDP_RECV_USED:
		pMsgTypeTab = &g_cpsMsgSem_Manage->msgtab.StyleRecv.TypeUDP;
		break;
	default:
		break;
	}
	if (NULL == pMsgTypeTab)
	{
		goto END_PROC;
	}

	// add a lock
	uRet = VOS_Mutex_Lock(&g_cpsMsgSem_Manage->hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
		return NULL;
	}
	uRet = VOS_ERR;
	pMsgInfo = pMsgTypeTab->pUseHead;
	
	while (NULL != pMsgInfo)
	{
		if (CPSS_MSG_STAT_USEING == pMsgInfo->nStat)
		{
			if (pMsgInfo->Body.msghead.stDstProc.ulPID == ulPID)
			{
				break;
			}
		}
		pMsgInfo = pMsgInfo->next;
	}
	VOS_PrintDebug(__FILE__,__LINE__,"Get %s:Msg:%p Pid:%d",
		GET_UDP_RECV_USED == uType?"UDP":"TCP",
		pMsgInfo,
		ulPID);
END_PROC:
	if (VOS_OK != VOS_Mutex_Unlock(&g_cpsMsgSem_Manage->hMutex))
	{
		VOS_PrintErr(__FILE__, __LINE__, "add lock error");
	}
	return pMsgInfo;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
VOS_UINT32 cps_set_msg_src_cpuid(CPSS_MSG * msgTmp, VOS_UINT32 ulCpuID, VOS_UINT32 ulPID)
{
	VOS_UINT32 ulRtn = VOS_ERR;
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__,__LINE__,"msg is NULL");
		return ulRtn;
	}
	if (ulCpuID == 0 ||
		ulPID == 0)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CPuID[%d] PID[%d] is NULL",ulCpuID,ulPID);
		return ulRtn;
	}
	msgTmp->Body.msghead.stSrcProc.ulCpuID = ulCpuID;
	msgTmp->Body.msghead.stSrcProc.ulPID = ulPID;
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
VOS_UINT32 cps_set_msg_dst_cpuid(CPSS_MSG * msgTmp, VOS_UINT32 ulCpuID, VOS_UINT32 ulPID)
{
	VOS_UINT32 ulRtn = VOS_ERR;
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg is NULL");
		return ulRtn;
	}
	if (ulCpuID == 0 ||
		ulPID == 0)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CPuID[%d] PID[%d] is NULL", ulCpuID, ulPID);
		return ulRtn;
	}
	msgTmp->Body.msghead.stDstProc.ulCpuID = ulCpuID;
	msgTmp->Body.msghead.stDstProc.ulPID = ulPID;
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
VOS_UINT32 cps_set_msg_data(CPSS_MSG * msgTmp, VOS_VOID* m_Value, VOS_UINT8 uType)
{
	VOS_UINT32 ulRtn = VOS_ERR;
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg is NULL");
		return ulRtn;
	}
	if (uType < SET_MSG_DATA_TYPE ||
		uType > SET_MSG_DATA_NUM)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Set Type [%d] not correct", uType);
		return ulRtn;
	}
	if (NULL == m_Value)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Set Value is NULL");
		return ulRtn;
	}
	switch (uType)
	{
	case SET_MSG_DATA_TYPE:
		ulRtn = VOS_OK;
		break;
	case SET_MSG_DATA_CMD:
		ulRtn = VOS_OK;
		break;
	default:
		break;
	}
	return ulRtn;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cps_uninit_msg_sem
*  Description:  ƽ̨ж����Ϣ����
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_VOID cps_uninit_msg_sem()
{
	VOS_Mutex_Destroy(&g_cpsMsgSem_Manage->hMutex);
}

/* ===  FUNCTION  ==============================================================
*         Name:  cps_show_msg_info
*  Description:  ��ӡ��Ϣ��Ϣ
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_VOID cps_show_msg_info(CPSS_MSG * msgTmp)
{
	VOS_CHAR strbuff[1024]={0};
	VOS_UINT32 uLength = 0;
	CPSS_COM_HEAD *msghead;
	CPSS_COM_DATA *msgdata;

	if (NULL == msgTmp)
	{
		return;
	}
	msgdata = &msgTmp->Body;
	msghead = &msgdata->msghead;

	uLength = sprintf(strbuff,"%08X\nSCPuID:%d_PID:%d_MsgID:%d, DCPuID:%d_PID:%d_MsgID:%d",
		msgTmp,
		msghead->stDstProc.ulCpuID,
		msghead->stDstProc.ulPID,
		msgdata->msghead.ulMsgID,
		msghead->stSrcProc.ulCpuID,
		msghead->stSrcProc.ulPID,
		msgdata->msghead.ulRecvMsgID);

	uLength = sprintf(strbuff+uLength,"\nStat:%d Client:%08X prev:%08X next:%08X\n%s",
		msgTmp->nStat,
		msgTmp->pClient,
		msgTmp->prev,
		msgTmp->next,
		msgTmp->Body.stuDataBuf.strBuffer);
	VOS_PrintInfo("",CPSS_PRINTF_BUFFER,strbuff);
}