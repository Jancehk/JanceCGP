/*==============================================================================
 *
 *       Filename:  cpss_vk_socket.c
 *
 *    Description:  socket ����
 *
 *        Version:  1.0
 *        Created:  2011��05��23�� 22ʱ13��07��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 * ============================================================================*/
#include "cpss_vk_socket.h"
#include "cpss_common.h"
#include "cpss_com_pid.h"
#include "cpss_public.h"
#include "cpss_vk_print.h"

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_socket_delete
*  Description:  ��socket��ָ�����������Ƴ�
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 cpss_socket_delete(
	CPSS_SOCKET_LINK **pSocketHead,
	CPSS_SOCKET_LINK **pSocketTail,
	CPSS_SOCKET_LINK *pSocketInfo)
{
	VOS_UINT32 uRtn = VOS_ERR;
	if (NULL == pSocketInfo)
	{
		return uRtn;
	}
	if (VOS_OK != VOS_Mutex_Lock(&g_handleiocpmanage.hSocketMutex))
	{
		return uRtn;
	}
	if (NULL == pSocketHead || NULL == pSocketTail)
	{
		return uRtn;
	}
	if (NULL == pSocketInfo->next && NULL == pSocketInfo->prev)
	{
		*pSocketHead = NULL;
		*pSocketTail = NULL;
	}
	else if (NULL != pSocketInfo->next && NULL == pSocketInfo->prev)
	{
		*pSocketHead = pSocketInfo->next;
	}
	else if (NULL == pSocketInfo->next && NULL != pSocketInfo->prev)
	{
		*pSocketTail = pSocketInfo->prev;
	}
	else
	{
		pSocketInfo->prev->next = pSocketInfo->next;
		pSocketInfo->next->prev = pSocketInfo->prev;
	}
	pSocketInfo->next = NULL;
	pSocketInfo->prev = NULL;
	return VOS_Mutex_Unlock(&g_handleiocpmanage.hSocketMutex);
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_socket_insert
*  Description:  ��socket��ָ���������ϲ���
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 cpss_socket_insert(
	CPSS_SOCKET_LINK **pSocketHead,
	CPSS_SOCKET_LINK **pSocketTail,
	CPSS_SOCKET_LINK *pSocketInfo)
{
	VOS_UINT32 uRtn = VOS_ERR;
	if (NULL == pSocketInfo)
	{
		return uRtn;
	}
	if (NULL == pSocketHead || NULL == pSocketTail)
	{
		return uRtn;
	}
	if (VOS_OK != VOS_Mutex_Lock(&g_handleiocpmanage.hSocketMutex))
	{
		return uRtn;
	}
	if (NULL == *pSocketHead && NULL == *pSocketTail)
	{
		*pSocketHead = pSocketInfo;
	}
	else
	{
		pSocketInfo->prev = *pSocketTail;
		(*pSocketTail)->next = pSocketInfo;
	}
	*pSocketTail = pSocketInfo;
	return VOS_Mutex_Unlock(&g_handleiocpmanage.hSocketMutex);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_close_socket
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID cpss_close_socket(CPSS_SOCKET_LINK * pSocketLinkTInfo)
{
	if (NULL == pSocketLinkTInfo)
	{
		return ;
	}

	if (SOCKET_ERROR != pSocketLinkTInfo->nlSocketfd)
	{
#if (OS_TYPE == OS_TYPE_WINDOW)
		closesocket(pSocketLinkTInfo->nlSocketfd);
#elif (OS_TYPE == OS_TYPE_LINUX)
		Close(pSocketLinkTInfo->nlSocketfd);
#endif
	}
	pSocketLinkTInfo->nlSocketfd = SOCKET_ERROR;
	pSocketLinkTInfo->uIStat = CPSS_SKT_STAT_CLOSE;
	if (VOS_OK != cpss_socket_delete(
		&g_handleiocpmanage.pUsedSocketHead,
		&g_handleiocpmanage.pUsedSocketTail,
		pSocketLinkTInfo))
	{
		VOS_PrintErr(__FILE__,__LINE__,"socket delete error");
		return;
	}
	if (VOS_OK != cpss_socket_insert(
		&g_handleiocpmanage.pFreeSocketHead,
		&g_handleiocpmanage.pFreeSocketTail,
		pSocketLinkTInfo))
	{
		VOS_PrintErr(__FILE__, __LINE__, "socket insert error");
	}

}		/* -----  end of function cpss_iocp_close  ----- */

/************************************************************************/
/*					Server Common Function                              */
/************************************************************************/

/* ===  FUNCTION  =================================================================
 *         Name:  cpss_show_cli_info
 *  Description:  ��ӡ�ͻ�������
 * ===============================================================================*/
static VOS_VOID cpss_show_cli_info(pCPSS_CLIENT_INFO CliTmp)
{
	VOS_CHAR strbuff[1024]={0};
	VOS_UINT32 uLength = 0;
	if (NULL == CliTmp)
	{
		return;
	}
	
	uLength = sprintf(strbuff,"%08X\nClient FD:%d ID:%d Type:%d Stat:%d IP:%s:%d__User:%s:time:%d\nprev:%08X__next:%08X",
		CliTmp,
		CliTmp->pAccptSocket->nlSocketfd,
		CliTmp->ulID,
		CliTmp->nType,
		CliTmp->nStat,
		CliTmp->strIPaddress,
		CliTmp->pAccptSocket->uPort,
		CliTmp->stuUserInfo.strUser,
		CliTmp->ulCientHaveTime,
		CliTmp->prev,
		CliTmp->next);
	VOS_PrintInfo("",CPSS_PRINTF_BUFFER,strbuff);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_client_info
 *  Description:  �ڿ����б���ȡ�ÿͻ���Ϣ,��������һ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static pCPSS_CLIENT_INFO cpss_get_free_client_info(CPSS_IOCP_MANAGE *piocpmanage)
{
	CPSS_CLIENT_INFO *pClientInfoTmp = piocpmanage->pFreeTCPClientHead;

	VOS_Mutex_Lock(&piocpmanage->hClientMutex);
	while(NULL != pClientInfoTmp)
	{
		if (VOS_SOCKET_STAT_RESERVE == pClientInfoTmp->nStat)// free �Ƿ�ԤԼ
		{
			pClientInfoTmp = pClientInfoTmp->next;
			continue;
		}
		pClientInfoTmp->nStat = VOS_SOCKET_STAT_RESERVE;

		VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
		return pClientInfoTmp;
	}
	/*
	if (NULL != piocpmanage->pFreeClientTial)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Free Tail is NULL");
		VOS_Mutex_Unlock(&piocpmanage->hMutex);
		return pClientInfoTmp;
	}
	*/
	pClientInfoTmp = (pCPSS_CLIENT_INFO)VOS_Malloc(sizeof(CPSS_CLIENT_INFO), "get client info");
	if (NULL == pClientInfoTmp)
	{
		VOS_PrintErr(__FILE__,__LINE__,"socket calloc client info NULL");
		VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
		return pClientInfoTmp;
	}
	
	memset(pClientInfoTmp,0,sizeof(CPSS_CLIENT_INFO));
	if (NULL == piocpmanage->pFreeTCPClientTial)
	{
		piocpmanage->pFreeTCPClientHead = pClientInfoTmp;
		piocpmanage->pFreeTCPClientTial = pClientInfoTmp;
	}
	else
	{
		piocpmanage->pFreeTCPClientTial->next = pClientInfoTmp;
		pClientInfoTmp->prev = piocpmanage->pFreeTCPClientTial;
		piocpmanage->pFreeTCPClientTial = pClientInfoTmp;
	}
	
	
	piocpmanage->ulClientCount++;
	if (piocpmanage->ulClientCount > 0XFFFFFFFF)
	{
		piocpmanage->ulClientCount = 1;
	}
	if (pClientInfoTmp->nCmdConut < 1)
	{
		pClientInfoTmp->nCmdConut = 1;
		//pClientInfoTmp->pstrBuffer[pClientInfoTmp->nCmdConut -1 ] = (VOS_CHAR *)VOS_Malloc(CPSS_MSG_BUFFER_SIZE, "shell use Get buffer");
		//VOS_Malloc(CPSS_MSG_BUFFER_SIZE, &pClientInfoTmp->pstrBuffer[pClientInfoTmp->nCmdConut]);
		pClientInfoTmp->nBufferLeng = 0;
	}
	
	pClientInfoTmp->ulID = piocpmanage->ulClientCount;
	pClientInfoTmp->nStat = VOS_SOCKET_STAT_RESERVE;
	VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
	return pClientInfoTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_update_skt_pid
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_update_skt_pid(pCPSS_SOCKET_LINK pSocketLink,VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 ulPidIndex = 0;
	VOS_INT32  uLen = 0;
	SOCKADDR_IN seraddr;

	if (NULL == pSocketLink->pstuPid)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set socket pid is error");
		return ulRet;
	}
	uLen=sizeof(seraddr); 
	getsockname(pSocketLink->nlSocketfd,(SOCKADDR *)&seraddr,&uLen);
	
	//char *ip=inet_ntoa(sockAddr.sin_addr);
	pSocketLink->uPort= ntohs(seraddr.sin_port);
	*(pSocketLink->pstuPid->pPid) = VOS_SOCKET_IN | uType | pSocketLink->uPort;
	
	ulPidIndex = cpss_get_id_for_pid_str(pSocketLink->pstuPid->szPidName);
	ulRet = cpss_update_cpuid_pid(g_ulSubSystem, ulPidIndex,
		CPSS_STAT_ENABLE, *(pSocketLink->pstuPid->pPid), CPSS_GET_TYPE_PID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set %s cpuid is error",
			pSocketLink->pstuPid->szPidName);
	}
	/*
	VOS_PrintInfo(__FILE__, __LINE__, "Update PID:[%08d:%s] Port:%d",
		*pSocketLink->pstuPid->pPid,
		pSocketLink->pstuPid->szPidName,
		pSocketLink->uPort);
	*/
	ulRet = VOS_OK;
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_wait_socket
 *  Description:  �ȴ�Socket�����е�����֪ͨ
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_INT32 cpss_wait_socket(VOS_UINT32 uType,
								  pCPSS_SKT_FD_SET pstuSktfd)
{
	VOS_INT32	uSelectCount = 0;
	struct timeval stime;
	CPSS_SOCKET_LINK * hSocketLink = g_handleiocpmanage.pUsedSocketHead;
	
	if (NULL == pstuSktfd)
	{
		VOS_PrintErr(__FILE__, __LINE__,"cpss wait socket is param is error\n");
		return 0;
	}
	if (NULL == hSocketLink)
	{
		VOS_Sleep(1);
		return 0;
	}
	
	stime.tv_sec  = 1;
	stime.tv_usec = 800;
	pstuSktfd->nCurrent = 0;
	pstuSktfd->nMaxfd = 0;
	FD_ZERO(&pstuSktfd->fdread);
	FD_ZERO(&pstuSktfd->fdwrite);
	FD_ZERO(&pstuSktfd->fderror);
	
	while (NULL != hSocketLink)
	{
		if (hSocketLink->nlSocketType == uType &&
			hSocketLink->nlSocketfd != 0 &&
			CPSS_SKT_STAT_OPENED == hSocketLink->uIStat)
		{
			FD_SET(hSocketLink->nlSocketfd, &pstuSktfd->fdread);
			FD_SET(hSocketLink->nlSocketfd, &pstuSktfd->fderror);
			if (pstuSktfd->nMaxfd < hSocketLink->nlSocketfd)
			{
				pstuSktfd->nMaxfd = hSocketLink->nlSocketfd;
			}
		}
		hSocketLink = hSocketLink->next;
	}
	if (0 == pstuSktfd->nMaxfd)
	{
		VOS_Sleep(1);
		return 0;
	}
	
	uSelectCount =select(pstuSktfd->nMaxfd+1, 
		&pstuSktfd->fdread, NULL, 
		&pstuSktfd->fderror, &stime);
	
	if (0 > uSelectCount)
	{
		VOS_PrintErr(__FILE__,__LINE__,"select count(%d) socket error",uSelectCount);
		return 0;
	}
	return uSelectCount;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_socket_from_fdset
*  Description:  �������еõ�һ��socket
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static CPSS_SOCKET_LINK * cpss_get_socket_from_fdset(
	VOS_UINT32 uType,
	pCPSS_SKT_FD_SET pstuSktfd)
{
	CPSS_SOCKET_LINK * hSocketLink = g_handleiocpmanage.pUsedSocketHead;
	VOS_INT32 ulIndex = 0;
	if (NULL == hSocketLink)
	{
		return NULL;
	}
	while (NULL != hSocketLink)
	{
		if (ulIndex < pstuSktfd->nCurrent)
		{
			ulIndex++;
			hSocketLink = hSocketLink->next;
			continue;
		}
		pstuSktfd->nCurrent;
		if (hSocketLink->nlSocketType == uType &&
			hSocketLink->nlSocketfd != 0 &&
			CPSS_SKT_STAT_OPENED == hSocketLink->uIStat)
		{
			if (FD_ISSET(hSocketLink->nlSocketfd, &pstuSktfd->fdread) != 0)
			{
				return hSocketLink;
			}
			if (FD_ISSET(hSocketLink->nlSocketfd, &pstuSktfd->fderror) != 0)
			{
				return hSocketLink;
			}
		}
		hSocketLink = hSocketLink->next;
	}
	return NULL;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_socket_from_client
*  Description:  ͨ��client��Ϣ��socketʹ�ñ����ҵ���Ӧ��socket
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static CPSS_SOCKET_LINK * cpss_get_socket_from_client(CPSS_CLIENT_INFO * pClientInfo)
{
	CPSS_SOCKET_LINK * pSocketLink = NULL;
	pSocketLink = g_handleiocpmanage.pUsedSocketHead;
	while (NULL != pSocketLink)
	{
		if (pSocketLink->pstuClientInfo == pClientInfo)
		{
			break;
		}
		pSocketLink = pSocketLink->next;
	}
	return pSocketLink;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_memset_client_info
*  Description:  ��ʼ��client�ṹ��
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_VOID cpss_memset_client_info(CPSS_CLIENT_INFO *pClientInfo)
{
	CPSS_SOCKET_LINK *pstuClientSocket = NULL;
	pCPSS_MSG pMsg = NULL;

	pstuClientSocket = cpss_get_socket_from_client(pClientInfo);
	if (NULL != pstuClientSocket)
	{
		cpss_close_socket(pstuClientSocket);
	}
	VOS_Memset(&pClientInfo->clientaddr, sizeof(SOCKADDR_IN));
	pClientInfo->dwTThreadId = 0;

	VOS_Memset(pClientInfo->strIPaddress, CPSS_CLIENT_IPADS_LENGTH);
	VOS_Memset(pClientInfo->stuUserInfo.strUser, CPSS_CLIENT_NAME_LENGTH);
	VOS_Memset(pClientInfo->stuUserInfo.strPass, CPSS_CLIENT_PASS_LENGTH);
	pClientInfo->ulCientHaveTime = 0;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_used_to_free
 *  Description:  ��ʹ�õ��б��ƶ���������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_move_used_to_free(
	CPSS_IOCP_MANAGE *piocpmanage,
	CPSS_CLIENT_INFO *pClientInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_CLIENT_INFO *pUClientHead = piocpmanage->pUsedTCPClientHead;
	CPSS_CLIENT_INFO *pFClientTial = piocpmanage->pFreeTCPClientTial;
	VOS_Mutex_Lock(&piocpmanage->hClientMutex);
	while(NULL != pUClientHead)
	{
		if (pUClientHead != pClientInfo)
		{
			pUClientHead = pUClientHead->next;
			continue;
		}
		cpss_memset_client_info(pClientInfo);

		if (NULL == pUClientHead->next &&
			NULL == pUClientHead->prev)// remove user head
		{
			piocpmanage->pUsedTCPClientHead = NULL;
			piocpmanage->pUsedTCPClientTial = NULL;
		}
		else if (NULL != pUClientHead->next &&
				 NULL == pUClientHead->prev)// ��ͷ��
		{
			piocpmanage->pUsedTCPClientHead = pUClientHead->next;
			piocpmanage->pUsedTCPClientHead->prev = NULL;
		}
		else if (NULL == pUClientHead->next &&
			     NULL != pUClientHead->prev)// ��β��
		{
			piocpmanage->pUsedTCPClientTial = pUClientHead->prev;
			piocpmanage->pUsedTCPClientTial->next = NULL;
		}
		else if (NULL != pUClientHead->next &&
			     NULL != pUClientHead->prev)// ��β��
		{
			pUClientHead->next->prev = pUClientHead->prev;
			pUClientHead->prev->next = pUClientHead->next;
		}
		else
		{
			VOS_PrintErr(__FILE__,__LINE__,"check Use Client is Error for %d08X",
				pUClientHead);
			break;
		}
		
		if (NULL == pFClientTial)// add free tail
		{
			piocpmanage->pUsedTCPClientHead = pClientInfo;
			piocpmanage->pUsedTCPClientTial = pClientInfo;
		}
		else
		{
			pFClientTial->next = pClientInfo;
			pClientInfo->prev = pFClientTial;
			pClientInfo->next = NULL;
			piocpmanage->pFreeTCPClientTial = pClientInfo;
		}
		pClientInfo->nStat = VOS_SOCKET_STAT_FREEING;
		break;
	}
	
	piocpmanage->ulClientOnline --;
	VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
	ulRet = VOS_OK;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_free_to_used
 *  Description:  �����е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_client_move_free_to_used(CPSS_IOCP_MANAGE *piocpmanage,CPSS_CLIENT_INFO *pClientInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_CLIENT_INFO *pFClientHead = piocpmanage->pFreeTCPClientHead;
	CPSS_CLIENT_INFO *pUClientTial = piocpmanage->pUsedTCPClientTial;
	
	VOS_Mutex_Lock(&piocpmanage->hClientMutex);
	while(NULL != pFClientHead)
	{
		//find client in free head
		if (pFClientHead != pClientInfo)
		{
			pFClientHead = pFClientHead->next;
			continue;
		}

		if (NULL == pFClientHead->next &&
			NULL == pFClientHead->prev)// remove free head
		{
			// just only one free client
			piocpmanage->pFreeTCPClientHead = NULL;
			piocpmanage->pFreeTCPClientTial = NULL;
		}
		else if (NULL == pFClientHead->next &&
			     NULL != pFClientHead->prev)
		{
			// client is last one
			piocpmanage->pFreeTCPClientTial = pFClientHead->prev;
			piocpmanage->pFreeTCPClientTial->next = NULL;
		}
		else if (NULL != pFClientHead->next &&
			     NULL == pFClientHead->prev)
		{
			// client is first one
			piocpmanage->pFreeTCPClientHead = pFClientHead->next;
			piocpmanage->pFreeTCPClientTial->prev = NULL;
		}
		else if (NULL != pFClientHead->next &&
				 NULL != pFClientHead->prev)
		{
			//client is middle one
			pFClientHead->next->prev = pFClientHead->prev;
			pFClientHead->prev->next = pFClientHead->next;
		}
		else
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss move free to used error ");
			break;
		}
		pFClientHead->next = NULL;
		pFClientHead->prev = NULL;
		
		if (NULL == pUClientTial)// add use tail
		{
			piocpmanage->pUsedTCPClientHead = pClientInfo;
			piocpmanage->pUsedTCPClientTial = pClientInfo;
		}
		else
		{
			pUClientTial->next = pClientInfo;
			pClientInfo->prev = pUClientTial;
			piocpmanage->pUsedTCPClientTial = pClientInfo;
		}
		pClientInfo->nStat = CPSS_CLIENT_ONLINE;
		break;
	}

	piocpmanage->ulClientOnline ++;
	VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
	ulRet = VOS_OK;
	return ulRet;
}

/* ===  FUNCTION  =================================================================
*         Name:  cpss_get_socket_from_pid
*  Description:  ��PID�л�ȡһ��socket
* ===============================================================================*/
static CPSS_SOCKET_LINK * cpss_get_socket_from_pid(VOS_CHAR * szPidName, VOS_UINT32 ulPid)
{
	static VOS_UINT32 uIndex = 1;
	VOS_UINT32 ulType = ulPid & VOS_SOCKET_STAT;
	VOS_UINT32 uPort = ulPid & VOS_SOCKET_PORT;
	VOS_UINT32 nRtn = VOS_ERR;
	CPSS_SOCKET_LINK * hSocketProcTmp = NULL, *hSocketProcTmp1 = NULL;

	if (NULL == g_handleiocpmanage.pFreeSocketHead)
	{
		hSocketProcTmp = (CPSS_SOCKET_LINK*)VOS_Malloc(sizeof(CPSS_SOCKET_LINK),
			"Socket Proc Address");
		if (NULL == hSocketProcTmp)
		{
			VOS_PrintErr(__FILE__, __LINE__, "SKTInit calloc faild");
			return NULL;
		}
		BZERO(hSocketProcTmp, sizeof(CPSS_SOCKET_LINK));
		hSocketProcTmp->uIStat = CPSS_SKT_STAT_CREATE;
		hSocketProcTmp->uID = uIndex;
		uIndex++;
	}
	else
	{
		hSocketProcTmp = g_handleiocpmanage.pFreeSocketHead;
		hSocketProcTmp->uIStat = CPSS_SKT_STAT_CLOSE;
		nRtn = cpss_socket_delete(
			&g_handleiocpmanage.pFreeSocketHead,
			&g_handleiocpmanage.pFreeSocketTail,
			hSocketProcTmp);
		if (VOS_OK != nRtn)
		{
			VOS_PrintErr(__FILE__, __LINE__, "delete free socket error");
		}
	}
	hSocketProcTmp->nlSocketfd = 0;
	hSocketProcTmp->nlSocketType = ulType >> 16;
	hSocketProcTmp->uPort = uPort;
	nRtn = cpss_socket_insert(
		&g_handleiocpmanage.pUsedSocketHead,
		&g_handleiocpmanage.pUsedSocketTail,
		hSocketProcTmp);
	if (VOS_OK != nRtn)
	{
		VOS_PrintErr(__FILE__, __LINE__, "insert used socket error");
	}
	return hSocketProcTmp;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_recv_msg
 *  Description: ��TCP���ƶ���socket�Ͻ��յ�����
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static VOS_UINT32 cpss_tcp_recv_msg(pCPSS_SOCKET_LINK pSocketInfo)
{
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			ulMsgLength = 0;
	CPSS_SOCKET_LINK  *	pSocketLink = NULL;
	CPSS_CLIENT_INFO  *	pClientInfo = NULL;
	CPSS_MSG		  * msgTmp = NULL;
	pCPSS_MEM_BUFFER	pstuBuf = NULL;

	if (NULL == pSocketInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpg socket info is NULL");
		return ulRet;
	}
	pClientInfo = pSocketInfo->pstuClientInfo;
	if (NULL == pClientInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpg client info is NULL");
		return ulRet;
	}
	msgTmp = (CPSS_MSG*)cpss_get_msg_info();
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg size address is NULL");
		return ulRet;
	}
	msgTmp->pClient = pClientInfo;
	msgTmp->nType = CPSS_MSG_TYPE_RECVING;

	if (CPSS_SOCKET_PACKAGE_OFF == (pClientInfo->uClientStat & CPSS_SOCKET_PACKAGE_OFF))
	{
		msgTmp->Body.msghead.stDstProc.ulCpuID = CPSSCPUID;
		msgTmp->Body.msghead.stDstProc.ulPID = *pSocketLink->pstuPid->pPid;
		msgTmp->Body.msghead.stSrcProc.ulCpuID = get_cpuid_from_str(pClientInfo->strIPaddress);
		msgTmp->Body.msghead.stSrcProc.ulPID = VOS_SOCKET_IN | VOS_TCP_PID | pSocketInfo->uPort;

		pstuBuf = &(msgTmp->Body.stuDataBuf);
		ulMsgLength = recv(pSocketInfo->nlSocketfd,
			pstuBuf->strBuffer + pstuBuf->nSize,
			CPSS_MSG_BUFFER_SIZE- pstuBuf->nSize,0);

		if (0 == ulMsgLength || 0xffffffff == ulMsgLength)
		{
			cpss_move_reserve_to_free(msgTmp);
			return VOS_CLIENT_DISCONNECT;
		}

		pstuBuf->nSize += ulMsgLength;
		msgTmp->ulMsgLength = ulMsgLength;
		msgTmp->Body.msghead.ulMsgLength = ulMsgLength;
		if (pstuBuf->nSize > CPSS_MSG_BUFFER_SIZE)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get msg size big");
			return ulRet;
		}
		ulRet = VOS_OK;
	}
	else if (CPSS_SOCKET_PACKAGE_ON == (pClientInfo->uClientStat & CPSS_SOCKET_PACKAGE_ON))
	{
		ulMsgLength = recv(pSocketInfo->nlSocketfd,
			(VOS_CHAR*)&msgTmp->Body.msghead + msgTmp->ulMsgLength,
			sizeof(CPSS_COM_HEAD) - msgTmp->ulMsgLength,0);
		if (0 == ulMsgLength || 0xffffffff == ulMsgLength)
		{
			cpss_move_reserve_to_free(msgTmp);
			return VOS_CLIENT_DISCONNECT;
		}

		pstuBuf = &(msgTmp->Body.stuDataBuf);
		ulMsgLength = recv(pSocketInfo->nlSocketfd,
			pstuBuf->strBuffer + pstuBuf->nSize,
			CPSS_MSG_BUFFER_SIZE- pstuBuf->nSize,0);

		if (CPSSCPUID != msgTmp->Body.msghead.stDstProc.ulCpuID ||
			*pSocketLink->pstuPid->pPid != msgTmp->Body.msghead.stDstProc.ulPID)
		{
			cpss_move_reserve_to_free(msgTmp);
			VOS_PrintErr(__FILE__, __LINE__, "Dst error:CPUID:%08d:%08d PID:%08d:%08d",
				CPSSCPUID, msgTmp->Body.msghead.stDstProc.ulCpuID,
				*pSocketLink->pstuPid->pPid, msgTmp->Body.msghead.stDstProc.ulPID);
			return ulRet;
		}
		//pSocketLink = pClientInfo->pSocketInfo;
		//msgTmp->Body.msghead.stSrcProc.ulCpuID = get_cpuid_from_str(pClientInfo->strIPaddress);
		//msgTmp->Body.msghead.stSrcProc.ulPID = VOS_SOCKET_IN | VOS_TCP_PID | pSocketLink->uPort;
		msgTmp->ulMsgLength = ulMsgLength;
		if (msgTmp->ulMsgLength > CPSS_MSG_BUFFER_SIZE)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get msg size big");
			return ulRet;
		}
		ulRet = VOS_OK;
	}
	//pSocketInfo->pstuRecvMsg = msgTmp;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_check_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static CPSS_MSG * cpss_check_send_msg(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_MSG  *pMsg = NULL;
	CPSS_MSG  *pRecvMsg = NULL;

	pMsg = (CPSS_MSG *)pVoidMsg;
	if (NULL == pMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Msg info is NULL");
		goto EXITPROC;
	}
		
	if (0 == pMsg->Body.msghead.stDstProc.ulCpuID ||
		0 == pMsg->Body.msghead.stDstProc.ulPID)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send destination site is NULL,CpuID:%d,Pid:%d",
			pMsg->Body.msghead.stDstProc.ulCpuID,
			pMsg->Body.msghead.stDstProc.ulPID);
		goto EXITPROC;
	}
	if (0 == pMsg->Body.msghead.stSrcProc.ulCpuID ||
		0 == pMsg->Body.msghead.stSrcProc.ulPID)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send Source site is NULL,CpuID:%d,Pid:%d",
			pMsg->Body.msghead.stSrcProc.ulCpuID,
			pMsg->Body.msghead.stSrcProc.ulPID);
		goto EXITPROC;
	}

	if (0 != pMsg->Body.msghead.ulRecvMsgID)
	{
		pRecvMsg = cpss_get_recv_msg_for_id(pMsg->Body.msghead.ulRecvMsgID);
		if (NULL == pRecvMsg)
		{
			goto EXITPROC;
		}
		VOS_PrintMsg("Send RecvID msg", pMsg);
	}
	if (0 != pMsg->Body.msghead.ulParentMsgID)
	{
		pRecvMsg = cpss_get_recv_msg_for_id(pMsg->Body.msghead.ulParentMsgID);
		if (NULL == pRecvMsg)
		{
			goto EXITPROC;
		}
	}
	ulRet = VOS_OK;
EXITPROC:
	if (VOS_ERR == ulRet)
	{
		pMsg = NULL;
	}
	return pMsg;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_check_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static CPSS_MSG * cpss_get_send_msg(CPSS_MSG * pMsg)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_MSG * pSendMsg = NULL;

	if (NULL != pMsg->pTimer)
	{
		cpss_kill_timer(pMsg->pTimer);
	}
	pSendMsg = cpss_get_msg_info();
	if (NULL == pSendMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get Msg info is NULL");
		return pSendMsg;
	}
	
	VOS_Memcpy(&pSendMsg->Body.msghead.stSrcProc,
		&pMsg->Body.msghead.stDstProc,sizeof(CPSS_COM_PID));
	VOS_Memcpy(&pSendMsg->Body.msghead.stDstProc,
		&pMsg->Body.msghead.stSrcProc,sizeof(CPSS_COM_PID));
	pSendMsg->pClient = pMsg->pClient;
	//pSendMsg->nStat = pMsg->nStat;
	
	VOS_PrintDebug(__FILE__,__LINE__,"Src:CpuID:%d,Pid:%d Des:CpuID:%d,Pid:%d",
		pMsg->Body.msghead.stSrcProc.ulCpuID,
		pMsg->Body.msghead.stSrcProc.ulPID,
		pMsg->Body.msghead.stDstProc.ulCpuID,
		pMsg->Body.msghead.stDstProc.ulPID);
	
	pSendMsg->Body.msghead.uType    = pMsg->Body.msghead.uType;
	pSendMsg->Body.msghead.uSubType = pMsg->Body.msghead.uSubType;
	pSendMsg->Body.msghead.uCmd     = pMsg->Body.msghead.uCmd;
	pSendMsg->Body.msghead.uSubCmd  = pMsg->Body.msghead.uSubCmd;
	
	pSendMsg->Body.msghead.ulRecvMsgID = pMsg->Body.msghead.ulMsgID;
	
	VOS_Memcpy(&(pSendMsg->Body.stuDataBuf), &(pMsg->Body.stuDataBuf), sizeof(CPSS_MEM_BUFFER));
	pSendMsg->Body.msghead.ulMsgLength = pMsg->Body.msghead.ulMsgLength;
	
	VOS_Memset(&pMsg->Body.stuDataBuf,sizeof(CPSS_MEM_BUFFER));

	return pSendMsg;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_udp_recv_msg
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_udp_recv_msg(pCPSS_SOCKET_LINK hSocketLink,  VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 ulSocRet = VOS_ERR;
	VOS_UINT32 ulMsgLength = 0;
	CPSS_MSG  *msgTmp = NULL;
	VOS_UINT32 ulMsgIndex = 0;
	VOS_UINT32 ulBodySize = 0;
	VOS_UINT32 ulTotalSize = 0;
	VOS_INT32  stusktaddrsz = 0;
	pCPSS_MEM_BUFFER   pstuBuff = NULL;
	SOCKADDR   stusktaddr;

	if (NULL == hSocketLink)
	{
		VOS_PrintErr(__FILE__, __LINE__, "recv UDP info is NULL");
		return ulRet;
	}
	
	msgTmp = (CPSS_MSG*)cpss_get_msg_info();
	if (NULL == msgTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg size address is NULL");
		return ulRet;
	}
	VOS_PrintMsg("Udp Recv New msg", msgTmp);
	// �������ݰ�ͷ
	stusktaddrsz = sizeof(stusktaddr);

	ulBodySize = CPSS_MSG_HEAD_SIZE;//CPSS_MSG_HEAD_SIZE+CPSS_MSG_BUFFER_SIZE;
	ulMsgIndex = 0;
	msgTmp->nType = CPSS_MSG_TYPE_RECVING;
	while (ulMsgIndex != ulBodySize)
	{
		ulMsgLength = recvfrom(
			hSocketLink->nlSocketfd, 
			(VOS_CHAR*)&(msgTmp->Body.msghead),
			ulBodySize-ulMsgIndex,
			0,
			&stusktaddr,
			&stusktaddrsz);
		
		VOS_PrintDebug(__FILE__,__LINE__,"recvfrom head total size is %d:%d",
		ulMsgLength,ulBodySize);
		if (0xFFFFFFFF == ulMsgLength)
		{
			VOS_PrintWarn(__FILE__,__LINE__,"break link %d:ErrorCode:%d",ulMsgLength,WSAGetLastError());
			goto END_PROC;
		}
		if (0 == ulMsgLength)
		{
			VOS_PrintErr(__FILE__,__LINE__,"recvfrom head %d:ere%d",ulMsgLength,WSAGetLastError());
			continue;
		}
		ulMsgIndex +=ulMsgLength;
	}
	ulMsgIndex = 0;
	pstuBuff = &msgTmp->Body.stuDataBuf;

	VOS_PrintDebug(__FILE__,__LINE__,"recvfrom data total size is %d",
		msgTmp->Body.msghead.ulMsgLength);

	while (ulTotalSize < msgTmp->Body.msghead.ulMsgLength)
	{
		if ((msgTmp->Body.msghead.ulMsgLength - ulTotalSize) > CPSS_MSG_BUFFER_USED+(sizeof(VOS_UINT32)*4))
		{
			ulBodySize = CPSS_MSG_BUFFER_USED + CPSS_MSG_BUF_HEAD_SIZE;
		}
		else
		{
			ulBodySize = msgTmp->Body.msghead.ulMsgLength - ulTotalSize;
		}
		ulMsgIndex = 0;
		while (ulMsgIndex != ulBodySize)
		{
			ulMsgLength = recvfrom(
				hSocketLink->nlSocketfd, 
				(VOS_CHAR*)(pstuBuff),
				ulBodySize-ulMsgIndex,
				0,
				&stusktaddr,
				&stusktaddrsz);

			VOS_PrintDebug(__FILE__,__LINE__,"recvfrom data total size is %d:%d",
				ulMsgLength,ulBodySize);
			
			if (0xffffffff == ulMsgLength)
			{
				VOS_PrintWarn(__FILE__,__LINE__,"recvfrom data size is %d:ere%d",ulMsgLength,WSAGetLastError());
				goto END_PROC;
			}
			if (0 == ulMsgLength)
			{
				VOS_PrintErr(__FILE__,__LINE__,"recvfrom recv %d:ere%d",ulMsgLength,WSAGetLastError());
				continue;
			}
			ulBodySize = CPSS_MSG_BUF_HEAD_SIZE	+ pstuBuff->nSize;
			ulMsgIndex +=ulMsgLength;
		}
		ulTotalSize +=ulMsgIndex;

		if (pstuBuff->nSize >= CPSS_MSG_BUFFER_USED &&
			msgTmp->Body.msghead.ulMsgLength - ulBodySize >0)
		{
			pstuBuff = (CPSS_MEM_BUFFER*)cpss_get_mem_buffer(pstuBuff,1);
		}
	}
	if (ulTotalSize == msgTmp->Body.msghead.ulMsgLength)
	{
		msgTmp->pClient = hSocketLink;
		ulSocRet = VOS_OK;
	}/*VOS_PrintErr(__FILE__, __LINE__,"udp recv msg :%d",ulSocRet);*/
END_PROC:
	if (ulSocRet == VOS_OK)
	{
		msgTmp->pClient = &hSocketLink->nlSocketfd;
/*		
	VOS_PrintInfo(__FILE__,__LINE__,"Src:CPuID:%u->Pid:%d,Dst:CPuID:%u->Pid:%d",
		msgTmp->Body.msghead.stSrcProc.ulCpuID,
		msgTmp->Body.msghead.stSrcProc.ulPID,
		msgTmp->Body.msghead.stDstProc.ulCpuID,
		msgTmp->Body.msghead.stDstProc.ulPID);*/
		ulRet = cpss_move_udp_recv_free_to_used(msgTmp);
	}
	else
	{
		ulRet = cpss_move_reserve_to_free(msgTmp);
	}
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "move udp to used error ");
	}
	return ulSocRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_socket_tcp_open
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_check_msg_complete(pCPSS_CLIENT_INFO pClientInfo, pCPSS_SOCKET_LINK pSocketInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pCPSS_MSG pstuMsg = NULL;
	if (NULL == pClientInfo || NULL == pSocketInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "check msg param is NULL");
		return ulRet;
	}
	
	//pstuMsg = pSocketInfo->pstuRecvMsg;
	if (CPSS_SOCKET_TELNET == pClientInfo->uClientStat)
	{
		pstuMsg->Body.msghead.uType = CPSS_TYPE_SYSTEM_TELNET;
		ulRet = cpss_move_tcp_recv_free_to_used(pstuMsg);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "move tcp to used error ");
			return ulRet;
		}
		ulRet = VOS_OK;
	}
	else
	{
		ulRet = cpss_move_tcp_recv_free_to_used(pstuMsg);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "move tcp to used error ");
			return ulRet;
		}
	}
	//pSocketInfo->pstuRecvMsg = NULL;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_timer_used_to_free_proc
 *  Description:  ����TCP ���ݳ�ʱ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_tcp_timer_used_to_free_proc(VOS_VOID *arg, VOS_UINT32 *len)
{
	VOS_UINT32 ulRet  = VOS_ERR;
	if (VOS_OK != cpss_move_tcp_recv_used_to_free(arg))
	{
		VOS_PrintErr(__FILE__, __LINE__, "timeout remove used to free :%p:%d",arg,len);
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_client_proc
 *  Description:  Services����tcp�Ŀͻ��˹��������ݰ����ŵ���Ϣ��������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_tcp_recv_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_tcp_recv_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_INT32 nRet = SOCKET_ERROR;
	CPSS_IOCP_MANAGE *piocpmanage=(CPSS_IOCP_MANAGE*)lpParameter;
	pCPSS_SOCKET_LINK pSocketInfo = NULL;
	CPSS_CLIENT_INFO *pClientInfo = NULL;
	CPSS_SKT_FD_SET stuSktfd;
	VOS_INT32 uCount= 0;
	VOS_INT32 uSelectCount = 0;
	struct timeval stime;
	stime.tv_sec  = 10;
	stime.tv_usec = 500;
	
	memset(&stuSktfd, 0, sizeof(stuSktfd));
	cpss_thread_success("T Recv");

	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			ulRet = VOS_OK;
			break;
		}
		if (stuSktfd.nCurrent >= uSelectCount)
		{
			uSelectCount = cpss_wait_socket(VOS_SOCKET_TCP, &stuSktfd);
			if (1 > uSelectCount)
			{
				continue;
			}
		}
		pSocketInfo = cpss_get_socket_from_fdset(VOS_SOCKET_TCP, &stuSktfd);
		if (NULL == pSocketInfo)
		{
			continue;
		}
		pClientInfo = pSocketInfo->pstuClientInfo;
		if (NULL == pClientInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpg client info is NULL");
			return ulRet;
		}
		nRet = cpss_tcp_recv_msg(pSocketInfo);
		if (nRet == VOS_ERR)
		{
			VOS_PrintErr(__FILE__, __LINE__, "recv socket msg error");
			continue;
		}
		if (VOS_CLIENT_DISCONNECT == nRet)
		{
			VOS_PrintInfo(__FILE__, __LINE__, "Client:%d %s:%d disconnect",
				pClientInfo->ulID,
				pClientInfo->strIPaddress,
				pSocketInfo->uPort);
			cpss_move_used_to_free(piocpmanage, pClientInfo);
			continue;
		}

		ulRet = cpss_check_msg_complete(pClientInfo, pSocketInfo);
		if (VOS_OK != ulRet)
		{
			//cpss_move_reserve_to_free(pSocketInfo->pstuRecvMsg);
			continue;
		}

		VOS_PrintDebug(__FILE__, __LINE__, "TC recv set event %s", pSocketInfo->pstuPid->pMsgEvent.strmutex);
		ulRet = VOS_Set_Event(&pSocketInfo->pstuPid->pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
				*pSocketInfo->pstuPid->pPid);
		//	cpss_move_reserve_to_free(pSocketInfo->pstuRecvMsg);
		}
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_send_proc
 *  Description:  Services����tcp�����ݰ� ����Ϣ��������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_tcp_send_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_tcp_send_proc (VOS_VOID * lpPareter)
#endif
{
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			nSendSize = 0;
	VOS_UINT32			nSendLength = 0;
	CPSS_CLIENT_INFO  * pClient = NULL;
	CPSS_SOCKET_LINK  * pSocket = NULL;
	CPSS_MSG		  * pstuMsg = NULL;
	CPSS_MSG		  * pstuMsgParent = NULL;
	CPSS_MEM_BUFFER	  * pstuBuf = NULL;

	cpss_thread_success("T Send");
	
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			ulRet = VOS_OK;
			break;
		}
		pstuMsg = g_handleiocpmanage.stMsgTL.msgtab.StyleSend.TypeTCP.pUseHead;
		if (NULL == pstuMsg)
		{
			ulRet = VOS_Wait_Event(&g_handleiocpmanage.pTSendMsgEvent, 500);//INFINITE
			if (VOS_OK != ulRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "tcp send Wait Event error");
			}
			continue;
		}
		pClient = (pCPSS_CLIENT_INFO)pstuMsg->pClient;
		if (NULL == pClient)
		{
			VOS_PrintErr(__FILE__, __LINE__, "send socket is disonnect ");
			cpss_move_tcp_send_used_to_free(pstuMsg);
			cpss_move_used_to_free(&g_handleiocpmanage, pClient);
			continue;
		}
		pSocket = cpss_get_socket_from_client(pClient);
		if (NULL == pSocket)
		{
			VOS_PrintErr(__FILE__, __LINE__, "send socket is disonnect ");
			cpss_move_tcp_send_used_to_free(pstuMsg);
			cpss_move_used_to_free(&g_handleiocpmanage, pClient);
			continue;
		}
		if (CPSS_MSG_STAT_USEING != pstuMsg->nStat)/*VOS_CLIENT_DISCONNECT*/
		{
			VOS_PrintErr(__FILE__,__LINE__,"send stat is disonnect ");
			cpss_move_tcp_send_used_to_free(pstuMsg);
			cpss_move_used_to_free(&g_handleiocpmanage, pClient);
			continue;
		}
		nSendLength = 0;
		pstuBuf = &(pstuMsg->Body.stuDataBuf);
		while(NULL != pstuBuf)
		{
			nSendSize = send(pSocket->nlSocketfd, (VOS_CHAR*)pstuBuf->strBuffer, pstuBuf->nSize, 0);
			if (0xFFFFFFFF == nSendSize)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send date leng:%d errorCode=%d",
					nSendSize, GetLastError());
				break;
			}
			if (nSendSize != pstuBuf->nSize)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send date leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
					nSendSize,
					GetLastError(),
					pstuMsg->Body.msghead.stDstProc.ulCpuID,
					pstuMsg->Body.msghead.stDstProc.ulPID);
			}
			nSendLength += pstuBuf->nSize + CPSS_MSG_BUF_HEAD_SIZE;
			
			if (pstuBuf != &pstuMsg->Body.stuDataBuf)
			{
				pstuBuf = (pCPSS_MEM_BUFFER)cpss_get_next_buffer(&(pstuMsg->Body.stuDataBuf));
			}
			else
			{
				pstuBuf = pstuMsg->Body.stuDataBuf.next;
			}
		}
		if (nSendLength != pstuMsg->Body.msghead.ulMsgLength)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Tcp Send:%d Msg:%d",
				nSendLength, pstuMsg->Body.msghead.ulMsgLength);
		}
		VOS_PrintDebug(__FILE__, __LINE__, "tcp send msg [%p]ID:%d recv msg ID:%d",
			pstuMsg,
			pstuMsg->Body.msghead.ulMsgID,
			pstuMsg->Body.msghead.ulRecvMsgID);

		if (0 != pstuMsg->Body.msghead.ulRecvMsgID)
		{
			cpss_move_recv_used_to_free_use_msgid(pstuMsg->Body.msghead.ulRecvMsgID);
			pClient->msg = NULL;
			pstuMsg->Body.msghead.ulRecvMsgID = 0;
		}
		cpss_move_tcp_send_used_to_free(pstuMsg);
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_tcp_send_msg (VOS_VOID *pVoidMsg)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_MSG  *pMsg = NULL;
	CPSS_MSG  *pSendMsg = NULL;
	CPSS_MSG  *pRecvMsg = NULL;
	CPSS_CLIENT_INFO *pClient = NULL;

	pMsg = cpss_check_send_msg(pVoidMsg);
	if (NULL == pMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"check send msg is NULL");
		return ulRet;
	}
	pSendMsg->pClient = pMsg->pClient;
	pClient = (pCPSS_CLIENT_INFO)pMsg->pClient;
	if (NULL == pClient)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send Client info is NULL");
		return ulRet;
	}
	pSendMsg = cpss_get_send_msg(pMsg);
	if (NULL == pSendMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get send msg is NULL");
		return ulRet;
	}
	cpss_move_tcp_send_free_to_used(pSendMsg);

	ulRet = VOS_Set_Event(&g_handleiocpmanage.pTSendMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set Event faild,CpuID:%d,Pid:%d",
			pMsg->Body.msghead.stSrcProc.ulCpuID,
			pMsg->Body.msghead.stSrcProc.ulPID);
		cpss_move_reserve_to_free(pMsg);
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_udp_send_msg (VOS_VOID *pVoidMsg)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_MSG  *pMsg = NULL;
	CPSS_MSG  *pSendMsg = NULL;

	pMsg = cpss_check_send_msg(pVoidMsg);
	if (NULL == pMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"check send msg is NULL");
		return ulRet;
	}
	
	pSendMsg = cpss_get_send_msg(pMsg);
	if (NULL == pSendMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get send msg is NULL");
		return ulRet;
	}
	cpss_move_udp_send_free_to_used(pSendMsg);

	ulRet = VOS_Set_Event(&g_handleiocpmanage.pUSendMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set Event faild,CpuID:%d,Pid:%d",
			pMsg->Body.msghead.stSrcProc.ulCpuID,
			pMsg->Body.msghead.stSrcProc.ulPID);
		cpss_move_reserve_to_free(pMsg);
		return ulRet;
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_send_data
 *  Description:  ����telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_copy_msg(VOS_VOID *pVoidMsgRecv, VOS_VOID * pVoidMsgSend)
{
	CPSS_MSG	*  pMsgRecv = (CPSS_MSG	*)pVoidMsgRecv;
	CPSS_MSG	*  pMsgSend = (CPSS_MSG	*)pVoidMsgSend;
	if (NULL == pMsgRecv ||
		NULL == pMsgSend)
	{
		return VOS_ERR;
	}
	VOS_Memcpy(&pMsgRecv->Body.msghead.stSrcProc, 
		&pMsgSend->Body.msghead.stSrcProc,sizeof(CPSS_COM_PID));
	VOS_Memcpy(&pMsgRecv->Body.msghead.stDstProc, 
		&pMsgSend->Body.msghead.stDstProc,sizeof(CPSS_COM_PID));
	pMsgRecv->Body.msghead.ulMsgID = pMsgSend->Body.msghead.ulMsgID;
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_send_data (VOS_VOID *pVoidMsg, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 ulMsgLength =0;
	CPSS_MSG		 *  pMsgInfo = NULL;

	if (NULL == pVoidMsg)
	{
		VOS_PrintErr(__FILE__, __LINE__, "send data param is NULL");
		return ulRet;
	}

	pMsgInfo = (pCPSS_MSG)pVoidMsg;
	if (NULL != strBuffer && 0 < uBufLen)
	{
		if (VOS_OK != VOS_PrintBufferBin(&pMsgInfo->Body.stuDataBuf, strBuffer, uBufLen,
			&ulMsgLength))
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss print buffer bin is error");
			return ulRet;
		}
		if (ulMsgLength != pMsgInfo->Body.msghead.ulMsgLength)
		{
			VOS_PrintDebug(__FILE__, __LINE__, "send len:%d != buff len:%d",
				pMsgInfo->Body.msghead.ulMsgLength, ulMsgLength);
			pMsgInfo->Body.msghead.ulMsgLength = ulMsgLength;
		}
	}
	if (VOS_SEND_SKT_TYPE_INSERT == (uType & VOS_SEND_SKT_TYPE_INSERT))
	{
		return VOS_OK;
	}
	
	pMsgInfo->Body.msghead.ulMsgLength += CPSS_MSG_BUF_HEAD_SIZE;
	pMsgInfo->nType = CPSS_MSG_TYPE_SENDING;
	if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
	{
		ulRet = cpss_tcp_send_msg(pMsgInfo);
		if (VOS_ERR == ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss send tcp data error");
		}
	}else
	if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
	{
		ulRet = cpss_udp_send_msg(pMsgInfo);
		if (VOS_ERR == ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss send udp data error");
		}
	}
	VOS_Memset(&pMsgInfo->Body.stuDataBuf, sizeof(CPSS_MEM_BUFFER));
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_send_data_msgid (VOS_UINT32 ulMsgID, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	pCPSS_MSG pMsgInfo = NULL;
	if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
	{
		pMsgInfo = cpss_get_tcp_recv_msg_for_id(ulMsgID);
	}else
	if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
	{
		pMsgInfo = cpss_get_udp_recv_msg_for_id(ulMsgID);
	}		
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "not find msg use msg id:%d",ulMsgID);
		return VOS_ERR;
	}
	return cpss_send_data(pMsgInfo,strBuffer,uBufLen,uType);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_accept_proc
 *  Description:  Services����UDP���ݵ��̺߳���
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_udp_recv_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_udp_recv_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32			ulRet = VOS_ERR;
	CPSS_SOCKET_LINK  * pSocketLink = NULL;
	VOS_UINT32			ulMsgIndex = 0;
	VOS_UINT32			ulMsgLength = 0;
	VOS_INT32			stusktaddrsz = 0;
	VOS_UINT8			dwType = 0;
	VOS_INT32			uSelectCount = 0;
	VOS_INT32			uCount= 0;
	CPSS_SKT_FD_SET		stuSktfd;

	memset(&stuSktfd, 0, sizeof(stuSktfd));
	cpss_thread_success("U Recv");
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			ulRet = VOS_OK;
			break;
		}
		if (stuSktfd.nCurrent >= uSelectCount)
		{
			uSelectCount = cpss_wait_socket(VOS_SOCKET_UDP, &stuSktfd);
			if (1 > uSelectCount)
			{
				continue;
			}
		}
		pSocketLink = cpss_get_socket_from_fdset(VOS_SOCKET_UDP, &stuSktfd);
		if (NULL == pSocketLink)
		{
			continue;
		}

		ulRet = cpss_udp_recv_msg(pSocketLink, CPSS_SOCKET_PACKAGE_OFF);
		if (ulRet == VOS_ERR)
		{
			VOS_PrintErr(__FILE__,__LINE__,"recv socket msg error");
			continue;
		}

		//			VOS_PrintInfo(__FILE__,__LINE__,"UDP recv set event %s", hSocketLink->szPidName);
		ulRet = VOS_Set_Event(&pSocketLink->pstuPid->pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
				*pSocketLink->pstuPid->pPid);
		}
	}
	return ulRet;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_udp_send_proc
 *  Description:  Services����tcp�����ݰ� ����Ϣ��������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_udp_send_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_udp_send_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32			ulRet = VOS_ERR;
	CPSS_MSG		  * pstuMsg = NULL;
	pCPSS_MEM_BUFFER	pstuBuf = NULL;
	CPSS_SOCKET_LINK  * pSocketInfo = NULL;
	VOS_UINT32			nSendSize = 0;
	VOS_UINT32			nSendLength = 0;
	VOS_UINT32			nSendBuffLen = 0;
	SOCKADDR_IN			stusktaddr;
	VOS_UINT32			bcastAddr = 0;
	VOS_UINT16			bPort = 0;

	if (0 == g_handleiocpmanage.pUSendMsgEvent.strmutex[0])
	{
		VOS_PrintErr(__FILE__, __LINE__, "udp send  Event is NULL");
		return VOS_ERR;
	}
	cpss_thread_success("U Send");
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			ulRet = VOS_OK;
			break;
		}
		pstuMsg = g_handleiocpmanage.stMsgTL.msgtab.StyleSend.TypeUDP.pUseHead;
		if (NULL == pstuMsg)
		{
			ulRet = VOS_Wait_Event(&g_handleiocpmanage.pUSendMsgEvent,200);
			if (VOS_OK != ulRet)
			{
				VOS_PrintErr(__FILE__,__LINE__,"udp send Wait Event error");
			}
			continue;
		}
		pSocketInfo = cpss_get_info_for_pid(pstuMsg->Body.msghead.stSrcProc.ulPID, CPSS_GET_SKT_LINK_SOCKET);
		//uSocketfd = pstuMsg->pClient;
		if (NULL == pSocketInfo)
		{
			VOS_PrintErr(__FILE__,__LINE__,"PID:%d UDP send msg socket is NULL",
				pstuMsg->Body.msghead.stSrcProc.ulPID);
			cpss_move_udp_send_used_to_free(pstuMsg);
			continue;
		}

		bcastAddr = pstuMsg->Body.msghead.stDstProc.ulCpuID;
		bPort = pstuMsg->Body.msghead.stDstProc.ulPID & VOS_SOCKET_PORT;

		stusktaddr.sin_family=AF_INET;
		stusktaddr.sin_addr.s_addr = get_cpuid_from_ip(bcastAddr) ;
		stusktaddr.sin_port=htons(bPort);

		VOS_PrintDebug(__FILE__,__LINE__,"Des:CpuID:%d,Pid:%d P:%d Src:CpuID:%d,Pid:%d P:%d",
			pstuMsg->Body.msghead.stDstProc.ulCpuID,
			pstuMsg->Body.msghead.stDstProc.ulPID,
			bPort,
			pstuMsg->Body.msghead.stSrcProc.ulCpuID,
			pstuMsg->Body.msghead.stSrcProc.ulPID,
			pstuMsg->Body.msghead.stSrcProc.ulPID & VOS_SOCKET_PORT);

		pstuBuf = &(pstuMsg->Body.stuDataBuf);

		nSendSize = sendto(pSocketInfo->nlSocketfd,
			(VOS_CHAR*)&pstuMsg->Body.msghead, sizeof(CPSS_COM_HEAD),0,
				(struct sockaddr *)&stusktaddr,sizeof(stusktaddr));
		if (0xFFFFFFFF == nSendSize)
		{
			VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d",
				nSendSize, GetLastError());
		}
		if (nSendSize != sizeof(CPSS_COM_HEAD))
		{
			VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
				nSendSize,
				GetLastError(),
				pstuMsg->Body.msghead.stDstProc.ulCpuID,
				pstuMsg->Body.msghead.stDstProc.ulPID);
		}
		
		VOS_PrintDebug(__FILE__,__LINE__,"send head data %d:%d:%d",
			nSendSize,sizeof(CPSS_COM_HEAD),
			pstuMsg->Body.msghead.ulMsgLength);
		
		while(NULL != pstuBuf)
		{
			nSendLength = CPSS_MSG_BUF_HEAD_SIZE + pstuBuf->nSize;
			nSendSize = sendto(pSocketInfo->nlSocketfd,
				(VOS_CHAR*)pstuBuf, nSendLength, 0,
				(struct sockaddr *)&stusktaddr, sizeof(stusktaddr));
			if (0xFFFFFFFF == nSendSize)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d",
					nSendSize, GetLastError());
			}
			VOS_PrintDebug(__FILE__,__LINE__,"send body data %d:%d",nSendSize,nSendLength);
			if (nSendSize != nSendLength)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send date leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
					nSendSize,
					GetLastError(),
					pstuMsg->Body.msghead.stDstProc.ulCpuID,
					pstuMsg->Body.msghead.stDstProc.ulPID);
			}
			nSendBuffLen += nSendLength;

			//pstuBuf = (pCPSS_MEM_BUFFER)cpss_get_next_buffer(&(pstuMsg->Body.stuDataBuf));

			if (pstuBuf != &(pstuMsg->Body.stuDataBuf))
			{
				pstuBuf = (pCPSS_MEM_BUFFER)cpss_get_next_buffer(&(pstuMsg->Body.stuDataBuf));
			}
			else
			{
				pstuBuf = pstuBuf->next;
			}
		}
		if (nSendLength != pstuMsg->Body.msghead.ulMsgLength)
		{
			VOS_PrintErr(__FILE__,__LINE__,"send date send:%d:Msg%d",
				nSendLength, pstuMsg->Body.msghead.ulMsgLength);
		}
		VOS_PrintDebug(__FILE__, __LINE__, "udp send msg ID:%d recv msg ID:%d",
			pstuMsg->Body.msghead.ulMsgID,
			pstuMsg->Body.msghead.ulRecvMsgID);
		
		VOS_PrintMsg("udp Send Proc 2 msg", pstuMsg);

		if (0 != pstuMsg->Body.msghead.ulRecvMsgID)
		{
			cpss_move_recv_used_to_free_use_msgid(pstuMsg->Body.msghead.ulRecvMsgID);
		}
		cpss_move_udp_send_used_to_free(pstuMsg);
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_accept_proc
 *  Description:  Services����tcp���̺߳���
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_tcp_distribute_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_tcp_distribute_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG *pMsgInfo = NULL;
	CPSS_MEM_BUFFER		stuBuffer = { 0 };
	CPSS_CLIENT_INFO * pClient = NULL;
	CPSS_SOCKET_LINK * pSocket = NULL;
	CPSS_PID_TABLE * pstuPidInfo = (CPSS_PID_TABLE*)lpParameter;
	if (NULL == pstuPidInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss distribute input Error ");
		return uRet;
	}
	if (NULL == pstuPidInfo->ppid_init_proc)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss tcp init proc NULL ");
		return uRet;
	}
	if (NULL == pstuPidInfo->ppid_timeout_proc)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss tcp init timeout proc NULL ");
		return uRet;
	}
	VOS_PrintBuffer(&stuBuffer, "T [%s] Distribute", pstuPidInfo->szPidName);
	cpss_thread_success(stuBuffer.strBuffer);

	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			uRet = VOS_OK;
			break;
		}
		pMsgInfo = cpss_get_used_msg(*pstuPidInfo->pPid, GET_TCP_RECV_USED);
		if (NULL == pMsgInfo)
		{
			if (0 == pstuPidInfo->pMsgEvent.strmutex[0])
			{
				VOS_Sleep(1);
				continue;
			}
			uRet = VOS_Wait_Event(&pstuPidInfo->pMsgEvent, INFINITE);
			if (VOS_OK != uRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "PID %08X Wait Event error",
					*pstuPidInfo->pPid);
			}
			continue;
		}
		if (NULL == pMsgInfo->pClient)
		{
			VOS_PrintErr(__FILE__,__LINE__,"PID Msg %p", pMsgInfo);
			cpss_move_tcp_recv_used_to_free(pMsgInfo);
			continue;
		}
		pClient = pMsgInfo->pClient;
		if (NULL == pClient)
		{
			VOS_PrintErr(__FILE__,__LINE__,"Client is NULL");
			continue;
		}
		pSocket = cpss_get_socket_from_client(pClient);
		if (NULL == pSocket)
		{
			VOS_PrintErr(__FILE__,__LINE__,"PID Msg Client %p pSocket %p",
				pClient,pSocket);
			continue;
		}
		
		if (CPSS_SOCKET_XCAP == pClient->uClientStat)
		{
			pMsgInfo->Body.msghead.uType = CPSS_TYPE_SYSTEM_HTTP;
		}
/*
		pMsgInfo->pTimer = (VOS_VOID *)cpss_set_timer(300, cpss_tcp_timer_used_to_free_proc, 
			pMsgInfo, sizeof(CPSS_MSG));
*/
		pMsgInfo->nStat = CPSS_MSG_STAT_USED;
		pMsgInfo->nType = CPSS_MSG_TYPE_RECVED;
		if (VOS_OK != pstuPidInfo->ppid_init_proc((VOS_VOID *)pMsgInfo))
		{
			VOS_PrintInfo(__FILE__,__LINE__,"Msg:%p Socket:%p Type:%x Stat:%x exec is faild",
				pMsgInfo,pSocket,pMsgInfo->nType,pMsgInfo->nStat);
		}
		if (CPSS_MSG_TYPE_SENDING != pMsgInfo->nType)
		{
			cpss_move_tcp_recv_used_to_free(pMsgInfo);
		}
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_socket_tcp_open
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static VOS_UINT32 cpss_tcp_accept_client (CPSS_SOCKET_LINK * pstuSocket)
{
	VOS_INT32	nlClientSocketfd = SOCKET_ERROR;
	CPSS_SOCKET_LINK *	pAccptSocket = NULL;
	CPSS_SOCKET_LINK *	pRecvSocket;
	CPSS_CLIENT_INFO *	pClientInfo =NULL;
	VOS_UINT32			ulRet = VOS_ERR;
	pCPSS_MSG		    pstuMsgInfo = NULL;
	VOS_INT32		    nSocketLength= 0;

	if (NULL == pstuSocket)
	{
		VOS_PrintErr(__FILE__, __LINE__, "accpet fun input is NULL");
		return ulRet;
	}
	pAccptSocket = pstuSocket;
	pClientInfo = cpss_get_free_client_info(&g_handleiocpmanage);
	if (NULL == pClientInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get free client info is NULL");
		goto ERR_PROC;
	}
	
	pClientInfo->pAccptSocket = pAccptSocket;
	pRecvSocket = cpss_get_socket_from_pid(pAccptSocket->pstuPid->szPidName,
		*pAccptSocket->pstuPid->pPid);
	nSocketLength= sizeof(SOCKADDR_IN);
	nlClientSocketfd = accept(pAccptSocket->nlSocketfd,
		(SOCKADDR *)&pClientInfo->clientaddr,
		&nSocketLength);
	if (SOCKET_ERROR == nlClientSocketfd)
	{
		VOS_PrintErr(__FILE__,__LINE__,"accept client is NULL");
		goto ERR_PROC;
	}
	if (NULL == pRecvSocket)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Client Socket Handle is NULL");
		goto ERR_PROC;
	}
	pRecvSocket->nlSocketType = VOS_SOCKET_TCP;
	pRecvSocket->pstuPid = pAccptSocket->pstuPid;
	pRecvSocket->uPort = htons(pClientInfo->clientaddr.sin_port);
	pRecvSocket->pstuClientInfo = pClientInfo;
	VOS_Strcpy(pClientInfo->strIPaddress,inet_ntoa(pClientInfo->clientaddr.sin_addr));
	pClientInfo->pAccptSocket = pAccptSocket;
	
	if (0 == VOS_Strcmp(pAccptSocket->pstuPid->szPidName, "TELNET"))
	{
		pClientInfo->nLineStat = CPSS_CLIENT_OFFLINE;
		pClientInfo->uClientStat = CPSS_SOCKET_TELNET;
	}else
		if (0 == VOS_Strcmp(pAccptSocket->pstuPid->szPidName, "XCAP"))
	{
		pClientInfo->nLineStat = CPSS_CLIENT_OFFLINE;
		pClientInfo->uClientStat = CPSS_SOCKET_XCAP;
	}
	else
	{
		pClientInfo->uClientStat = CPSS_SOCKET_PACKAGE_ON;
	}
	
	ulRet = cpss_client_move_free_to_used(&g_handleiocpmanage, pClientInfo);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"move free to use is error");
		goto ERR_PROC;
	}
	
	if (CPSS_SOCKET_PACKAGE_ON != pClientInfo->uClientStat)
	{
		pstuMsgInfo = (CPSS_MSG*)cpss_get_msg_info();
		if (NULL == pstuMsgInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get new msg info is NULL");
			goto ERR_PROC;
		}
		pstuMsgInfo->Body.msghead.stDstProc.ulCpuID = CPSSCPUID;
		pstuMsgInfo->Body.msghead.stDstProc.ulPID   = *pAccptSocket->pstuPid->pPid;
		pstuMsgInfo->Body.msghead.stSrcProc.ulCpuID = get_cpuid_from_str(pClientInfo->strIPaddress);
		pstuMsgInfo->Body.msghead.stSrcProc.ulPID   = VOS_SOCKET_IN | VOS_TCP_PID |pRecvSocket->uPort;
		pstuMsgInfo->pClient = pClientInfo;
		pstuMsgInfo->Body.msghead.uType = CPSS_TYPE_SYSTEM_INIT;
		if (CPSS_SOCKET_TELNET == pClientInfo->uClientStat)
		{
			pstuMsgInfo->Body.msghead.uCmd = CPSS_CMD_SYSTEM_TELNET;//CPSS_TYPE_SYSTEM_TELNET;
		}
		ulRet = cpss_move_tcp_recv_free_to_used(pstuMsgInfo);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__,__LINE__,"move tcp recv to used failed :%p",pstuMsgInfo);
		}
		VOS_Set_Event(&pAccptSocket->pstuPid->pMsgEvent);
	}
	VOS_PrintDebug(__FILE__,__LINE__,"client:%d info:%s:%d",
		pClientInfo->ulID, pClientInfo->strIPaddress, pRecvSocket->uPort);
END_PROC:
	if (VOS_ERR == ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "accept new client is error");
	}
	return ulRet;
ERR_PROC:
	if (NULL != pstuMsgInfo)
	{
		ulRet = cpss_move_reserve_to_free(pstuMsgInfo);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__,__LINE__,"move reserve to free failed :%p",pstuMsgInfo);
		}
	}
	if (NULL != pClientInfo)
	{
		cpss_memset_client_info(pClientInfo);
	}
	ulRet = VOS_ERR;
	goto END_PROC;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_accept_client_proc
 *  Description:  Services����tcp�ͻ��˵��̺߳���
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_tcp_accept_client_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_tcp_accept_client_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_SOCKET_LINK * pSocketLink = NULL;
	VOS_INT32		   uSelectCount = 0;
	VOS_UINT32		   uCount  = 0;
	VOS_UINT8		   dwType = 0;
	CPSS_SKT_FD_SET    stuSktfd = {0};
	
	
	if (NULL == lpParameter)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Parameter is NULL");
		return ulRet;
	}

	cpss_thread_success("T Accept");
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			ulRet = VOS_OK;
			break;
		}
		if (stuSktfd.nCurrent >= uSelectCount)
		{
			uSelectCount = cpss_wait_socket(VOS_SOCKET_ACC, &stuSktfd);
			if (1 > uSelectCount)
			{
				continue;
			}
		}
		pSocketLink = cpss_get_socket_from_fdset(VOS_SOCKET_ACC, &stuSktfd);
		if (NULL == pSocketLink)
		{
			continue;
		}

		if (VOS_OK != cpss_tcp_accept_client(pSocketLink))
		{
			VOS_PrintErr(__FILE__, __LINE__, "accept failed %p:%s:%d", pSocketLink,
				pSocketLink->pstuPid->szPidName, pSocketLink->uPort);
		}
	}
	return ulRet;
}		/* -----  end of function cpss_tcp_accept_client  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_udp_accept_proc
 *  Description:  ����udp���̺߳���
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_udp_distribute_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_udp_distribute_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG *pMsgInfo = NULL;
	CPSS_PID_TABLE * pstuPidInfo = (CPSS_PID_TABLE*)lpParameter;
	CPSS_SOCKET_LINK * pSocket = NULL;
	CPSS_CLIENT_INFO * pClient = NULL;
	CPSS_MEM_BUFFER		stuBuffer = { 0 };

	if (NULL == pstuPidInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss distribute input Error ");
		return uRet;
	}
	if (NULL == pstuPidInfo->ppid_init_proc)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss tcp init proc NULL ");
		return uRet;
	}
	if (NULL == pstuPidInfo->ppid_timeout_proc)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss tcp init proc NULL ");
		return uRet;
	}
	if (0 == pstuPidInfo->pMsgEvent.strmutex[0])
	{
		VOS_PrintErr(__FILE__, __LINE__, "mutex is NULL ");
		return uRet;
	}

	VOS_PrintBuffer(&stuBuffer, "U [%s] Distribute", pstuPidInfo->szPidName);
	cpss_thread_success(stuBuffer.strBuffer);

	((CPSS_SOCKET_LINK*)pstuPidInfo->pSocketInfo)->uIStat = CPSS_SKT_STAT_OPENED;
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			uRet = VOS_OK;
			break;
		}
		pMsgInfo = cpss_get_used_msg(*pstuPidInfo->pPid, GET_UDP_RECV_USED);
		if (NULL == pMsgInfo)
		{
			uRet = VOS_Wait_Event(&pstuPidInfo->pMsgEvent, INFINITE);
			if (VOS_OK != uRet)
			{
				VOS_PrintErr(__FILE__,__LINE__,"PID %08X Wait Event error",
					*pstuPidInfo->pPid);
			}
			continue;
		}
		/*
		pMsgInfo->pTimer = (VOS_VOID *)cpss_set_timer(300, cpss_tcp_timer_used_to_free_proc, 
			pMsgInfo, sizeof(CPSS_MSG));
		*/	
		pMsgInfo->nStat = CPSS_MSG_STAT_USED;
		pMsgInfo->nType = CPSS_MSG_TYPE_RECVED;
		VOS_PrintDebug(__FILE__, __LINE__, "PID:%d pSocket:%p pMsg:%p", *pstuPidInfo->pPid,
			pstuPidInfo->pSocketInfo, pMsgInfo);

		VOS_PrintMsg("udp distribute msg", pMsgInfo);
		
		VOS_PrintDebug(__FILE__, __LINE__, "UDP dealwith %s", pstuPidInfo->szPidName);

		if (VOS_OK != pstuPidInfo->ppid_init_proc((VOS_VOID *)pMsgInfo))
		{
			VOS_PrintInfo(__FILE__,__LINE__,"Msg:%p Socket:%p Type:%x Stat:%x exec is faild",
				pMsgInfo,pSocket,pMsgInfo->nType,pMsgInfo->nStat);
		}
		
		VOS_PrintMsg("udp distribute output msg", pMsgInfo);

		if (CPSS_MSG_TYPE_SENDING != pMsgInfo->nType)
		{
			cpss_move_udp_recv_used_to_free(pMsgInfo);
		}	
	}
	return uRet;
}		/* -----  end of function cpss_udp_accept_client  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_socket_tcp_open
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_socket_tcp_open (CPSS_SOCKET_LINK * pSocketLink)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_INT32  ulSoRet = VOS_ERR;
	SOCKADDR_IN seraddr;
	VOS_CHAR    strMsgEvent[32]={0};

	if (NULL == pSocketLink)
	{
		VOS_PrintErr(__FILE__,__LINE__,"create tcp server in NULL");
		return ulRet;
	}
	pSocketLink->nlSocketfd = socket(AF_INET,SOCK_STREAM,0);
	if(CPSS_SOCKET_ERROR == pSocketLink->nlSocketfd)
	{
		VOS_PrintErr(__FILE__,__LINE__,"CGP Tcp Socket Error");
		return ulRet;
	}
	pSocketLink->uIStat = CPSS_SKT_STAT_OPENING;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(pSocketLink->uPort);
	seraddr.sin_addr.s_addr = get_cpuid_from_ip(CPSSCPUID);

	ulSoRet = bind(pSocketLink->nlSocketfd,(SOCKADDR*)&seraddr,sizeof(SOCKADDR_IN));
	if (VOS_OK != ulSoRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"IP Error Or Port Used!");
		return ulRet;
	}

	ulSoRet = listen(pSocketLink->nlSocketfd,CPSS_TCP_LINK_MAX);
	if (VOS_OK != ulSoRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Rejected! Error Code:%d",GetLastError());
		return ulRet;
	}
	if (0 == pSocketLink->uPort)
	{
		ulRet = cpss_update_skt_pid(pSocketLink,VOS_TCP_PID);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "update %s cpuid is error",
				pSocketLink->pstuPid->szPidName);
			return ulRet;
		}
	}
	VOS_Memset(&pSocketLink->pstuPid->pMsgEvent, sizeof(VOS_Event));

	//��ʼ��Event
	sprintf(strMsgEvent, "TCP[%s]", pSocketLink->pstuPid->szPidName);
	ulRet = VOS_Init_Event(&pSocketLink->pstuPid->pMsgEvent, strMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"PID %s Init Event error",
			*pSocketLink->pstuPid->pPid);
		return ulRet;
	}
	pSocketLink->nlSocketType = VOS_SOCKET_ACC;
	pSocketLink->uIStat = CPSS_SKT_STAT_OPENED;

	return VOS_OK;
}		/* -----  end of function cpss_socket_tcp_open  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_socket_udp_open
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_socket_udp_open (CPSS_SOCKET_LINK * pSocketLink)
{
	VOS_UINT32	ulRet = VOS_ERR;
	VOS_INT32	ulSoRet = VOS_ERR;
	SOCKADDR_IN seraddr;
	VOS_INT32   optval = 0;
	VOS_CHAR    strMsgEvent[32]={0};

	pSocketLink->uIStat = CPSS_SKT_STAT_OPENING;
	pSocketLink->nlSocketfd= socket(AF_INET,SOCK_DGRAM,0);
	if(CPSS_SOCKET_ERROR == pSocketLink->nlSocketfd)
	{
		VOS_PrintErr(__FILE__,__LINE__,"CGP UDP Socket Error");
		return ulRet;
	}
	
	pSocketLink->uIStat = CPSS_SKT_STAT_OPEN;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(pSocketLink->uPort);
	seraddr.sin_addr.s_addr = get_cpuid_from_ip(CPSSCPUID) ;
	ulSoRet = bind(pSocketLink->nlSocketfd,(SOCKADDR*)&seraddr,sizeof(SOCKADDR_IN));
	if (VOS_OK != ulSoRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%s] IP Error Or Port Used!",
			pSocketLink->pstuPid->szPidName);
		closesocket(pSocketLink->nlSocketfd);
		pSocketLink->nlSocketfd = 0;
		return ulRet;
	}
	if (0 == pSocketLink->uPort)
	{
		ulRet = cpss_update_skt_pid(pSocketLink,VOS_UDP_PID);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "update %s cpuid is error",
				pSocketLink->pstuPid->szPidName);
			return ulRet;
		}
	}
	/*���÷���ѡ��Ϊ�٣���ֹ�����͵����ݷ��������ؽӿ�*/
	optval = 0;
	if (setsockopt(pSocketLink->nlSocketfd, IPPROTO_IP, IP_MULTICAST_LOOP,
		(VOS_CHAR *)&optval, sizeof(optval)) == SOCKET_ERROR) //�������ʧ��
	{
		VOS_PrintErr(__FILE__, __LINE__, "setsockopt(IP_MULTICAST_LOOP) failed: %d\n",
			WSAGetLastError());
		closesocket(pSocketLink->nlSocketfd);
		return ulRet;
	}
	optval = 1;
	if (setsockopt(pSocketLink->nlSocketfd, SOL_SOCKET, SO_REUSEADDR, 
		(VOS_CHAR *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		VOS_PrintErr(__FILE__, __LINE__, "setsockopt(SO_REUSEADDR) failed: %d\n",
			WSAGetLastError());
		closesocket(pSocketLink->nlSocketfd);
		return ulRet;
	}
	
	//��ʼ��Event
	sprintf(strMsgEvent, "UDP[%s]", pSocketLink->pstuPid->szPidName);
	
	ulRet = VOS_Init_Event(&pSocketLink->pstuPid->pMsgEvent, strMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%s] PID %s Init Event error",
			pSocketLink->pstuPid->szPidName,
			*pSocketLink->pstuPid->pPid);
		closesocket(pSocketLink->nlSocketfd);
		return ulRet;
	}
	
	pSocketLink->nlSocketType = VOS_SOCKET_UDP;	
	pSocketLink->uIStat = CPSS_SKT_STAT_OPENING;
	
	return VOS_OK;
}		/* -----  end of function cpss_socket_udp_open  ----- */

/* ===  FUNCTION  =================================================================
*         Name:  cpss_socket_open_pid
*  Description:  ע������ pid TCP���ϵͳ
* ===============================================================================*/
static VOS_UINT32 cpss_socket_open_pid(VOS_UINT32 dwType, CPSS_PID_TABLE * pstuPidList)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	VOS_INT32	ulSoRet = VOS_ERR;

	if (NULL == pstuPidList)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Pid Handle is NULL");
		return ulRet;
	}
	for (uIndex = 0; uIndex < pstuPidList->ulPidCount; uIndex++)
	{
		if (VOS_SOCKET_TCP == dwType)
		{
			ulSoRet = cpss_create_thread(&pstuPidList->hPidDist,
				0,
				cpss_tcp_distribute_proc,
				(VOS_VOID*)pstuPidList,
				&pstuPidList->dwPidThreadId,
				"Distribute proc");
		}
		else if (VOS_SOCKET_UDP == dwType)
		{
			ulSoRet = cpss_create_thread(&pstuPidList->hPidDist,
				0,
				&cpss_udp_distribute_proc,
				(VOS_VOID*)pstuPidList,
				&pstuPidList->dwPidThreadId,
				"U distribute proc");
			if (VOS_OK != ulSoRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "Rejected!");
				return ulRet;
			}
		}
		if (VOS_OK != ulSoRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Rejected!");
			return ulRet;
		}
	}
	ulRet = VOS_OK;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_iocp_init
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_iocp_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_SOCKET_LINK * hSocketLinkTemp = NULL;
	CPSS_PID_TABLE	*	pstuPidList  = NULL;
	VOS_UINT32 dwTcpThreadID = 0,dwUdpThreadID = 0;
	VOS_UINT32 dwType = 0;
	VOS_UINT32 ulProcessPid = 0;
	VOS_CHAR   strMutexKey[36] = {0};

	if (NULL == g_handleManagePid.pstuCPuIDList || NULL == g_handleManagePid.pstuPidList)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Reg Socket Init Error");
		return ulRet;
	}

	VOS_PrintInfo(__FILE__, __LINE__, "System CPuID:[%u]",
		CPSSCPUID);
	VOS_PrintInfo(__FILE__, __LINE__, "DBSvr  CPuID:[%u]",
		DBSVRCPUID);

	pstuPidList = g_handleManagePid.pstuPidList;
	while (NULL != pstuPidList)
	{
		ulProcessPid = 0;
		hSocketLinkTemp = cpss_get_socket_from_pid(pstuPidList->szPidName,
												   *pstuPidList->pPid);
		if (NULL == hSocketLinkTemp)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get socket handle error");
			return ulRet;
		}
		dwType = hSocketLinkTemp->nlSocketType & VOS_SOCKET_TYPE>>16;
		hSocketLinkTemp->pstuPid = pstuPidList;
		switch(dwType)
		{
		case VOS_SOCKET_TCP:
			if (VOS_OK == cpss_socket_tcp_open(hSocketLinkTemp))
			{
				VOS_PrintInfo(__FILE__,__LINE__,"T PID:[%08d:%s] Port:%d",
					*pstuPidList->pPid,
					pstuPidList->szPidName,
					hSocketLinkTemp->uPort);
			}
			else
			{
				VOS_PrintErr(__FILE__,__LINE__,"CGP T Init Error PID:[%s] Port:%d", 
					pstuPidList->szPidName,
					hSocketLinkTemp->uPort);
				pstuPidList = pstuPidList->next;
				continue;
			}
			break;
		case VOS_SOCKET_UDP:
			if (VOS_OK == cpss_socket_udp_open(hSocketLinkTemp))
			
			{
				VOS_PrintInfo(__FILE__,__LINE__,"U PID:[%08d:%s] Port:%d",
					*pstuPidList->pPid,
					pstuPidList->szPidName,
					hSocketLinkTemp->uPort);
			}
			else
			{
				VOS_PrintErr(__FILE__, __LINE__, "CGP U Init Error PID:[%s] Port:%d",
					pstuPidList->szPidName,
					hSocketLinkTemp->uPort);
				pstuPidList = pstuPidList->next;
				continue;
			}
			break;
		default:
			VOS_PrintErr(__FILE__, __LINE__, 
				"CGP Socket Init Type Error(%d)",hSocketLinkTemp->nlSocketType);
			continue;
		}
		pstuPidList->pSocketInfo = hSocketLinkTemp;
		if (VOS_OK != cpss_socket_open_pid(dwType,pstuPidList))
		{
			VOS_PrintErr(__FILE__, __LINE__, "CGP UDP Init Error");
		}
		pstuPidList = pstuPidList->next;
	}

	if (NULL == g_handleiocpmanage.ulTcpAccept)
	{
		ulRet = cpss_create_thread(&g_handleiocpmanage.ulTcpAccept,
			0,
			cpss_tcp_accept_client_proc,
			(VOS_VOID*)&g_handleiocpmanage,
			&g_handleiocpmanage.dwTThreadId,
			"T accept proc");
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for accept Rejected!");
			return ulRet;
		}
	}
	else
	{
		VOS_PrintWarn(__FILE__, __LINE__, "Accept system is already exist");
	}
	if (NULL == g_handleiocpmanage.hTcpRecv)
	{
		ulRet = cpss_create_thread(&g_handleiocpmanage.hTcpRecv,
			0,
			cpss_tcp_recv_proc,
			(VOS_VOID*)&g_handleiocpmanage,
			&dwTcpThreadID,
			"T recv proc");
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for client error");
			return ulRet;
		}
	}
	else
	{
		VOS_PrintWarn(__FILE__, __LINE__, "TCP Recv system is already exist");
	}
	if (NULL == g_handleiocpmanage.hTcpSend)
	{
		sprintf(strMutexKey, "TSENDMSG%d", time(NULL));
		ulRet = VOS_Init_Event(&g_handleiocpmanage.pTSendMsgEvent, strMutexKey);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "init Tcp Send Event");
			return ulRet;
		}
		ulRet = cpss_create_thread(&g_handleiocpmanage.hTcpSend,
			0,
			cpss_tcp_send_proc,
			(VOS_VOID*)&g_handleiocpmanage,
			&dwTcpThreadID,
			"T send proc");
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for client error");
			return ulRet;
		}
	}
	else
	{
		VOS_PrintWarn(__FILE__, __LINE__, "TCP Send system is already exist");
	}
	if (NULL == g_handleiocpmanage.hUdpRecv)
	{
		ulRet = cpss_create_thread(&g_handleiocpmanage.hUdpRecv,
			0,
			cpss_udp_recv_proc,
			(VOS_VOID*)&g_handleiocpmanage,
			&dwTcpThreadID,
			"U recv proc");
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for client error");
			return ulRet;
		}
	}
	else
	{
		VOS_PrintWarn(__FILE__, __LINE__, "UDP Recv system is already exist");
	}
	if (NULL == g_handleiocpmanage.hUdpSend)
	{
		sprintf(strMutexKey, "U_SEND");
		ulRet = VOS_Init_Event(&g_handleiocpmanage.pUSendMsgEvent, strMutexKey);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "init Udp Send Event");
			return ulRet;
		}
		ulRet = cpss_create_thread(&g_handleiocpmanage.hUdpSend,
			0,
			cpss_udp_send_proc,
			(VOS_VOID*)&g_handleiocpmanage,
			&dwTcpThreadID,
			"U send proc");
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for client error");
			return ulRet;
		}
	}
	ulRet = VOS_OK;
	return ulRet;
}		/* -----  end of function cpss_socket_init  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_subsystem_init
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_subsystem_init (VOS_UINT8 uType, VOS_UINT8 uCmd)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_SOCKET_LINK * hSocketLink = NULL;
	CPSS_MSG * pMsgInfo = NULL;
	
	if (NULL == g_handleiocpmanage.pUsedSocketHead)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Reg Socket Init Error");
		return ulRet;
	}

	//VOS_Memset(&MsgInfo, sizeof(CPSS_MSG));
	
	hSocketLink = g_handleiocpmanage.pUsedSocketHead;
	while (NULL != hSocketLink)
	{
		if (CPSS_CMD_SYSTEM_UNIT == uCmd)
		{
			ulRet = VOS_Set_Event(&hSocketLink->pstuPid->pMsgEvent);
			if (VOS_OK != ulRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
					*hSocketLink->pstuPid->pPid);
			}
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (VOS_SOCKET_ACC != hSocketLink->nlSocketType &&
			VOS_SOCKET_UDP != hSocketLink->nlSocketType)
		{
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (NULL == hSocketLink->pstuPid)
		{
			VOS_PrintErr(__FILE__,__LINE__,"Socket:%p PID is NULL",hSocketLink);
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (CPSS_CONNECT_SUB_DBSVR == g_ulSubSystem && CPSSFWPID == *hSocketLink->pstuPid->pPid)
		{
			VOS_PrintInfo(__FILE__, __LINE__, "This PID[%d] is DBSvr", 
				*hSocketLink->pstuPid->pPid);
			hSocketLink = hSocketLink->next;
			continue;
		}
		pMsgInfo = cpss_get_msg_info();
		if (NULL == pMsgInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Socket:%p PID[%d] get msg faild", 
				hSocketLink,
				*hSocketLink->pstuPid->pPid);
			hSocketLink = hSocketLink->next;
			continue;
		}
		cps_set_msg_dst_cpuid(pMsgInfo, CPSSCPUID, *hSocketLink->pstuPid->pPid);
		cps_set_msg_src_cpuid(pMsgInfo, CPSSFWCPUID, CPSSFWPID);
		pMsgInfo->Body.msghead.uType = uType;
		pMsgInfo->Body.msghead.uCmd = uCmd;

		ulRet = cpss_move_udp_recv_free_to_used(pMsgInfo);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "move tcp to used error ");
			hSocketLink = hSocketLink->next;
			continue;
		}
		ulRet = VOS_Set_Event(&hSocketLink->pstuPid->pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
				*hSocketLink->pstuPid->pPid);
			hSocketLink = hSocketLink->next;
			continue;
		}
		hSocketLink = hSocketLink->next;
	}

	ulRet = VOS_OK;
	return ulRet;
}		/* -----  end of function cpss_subsystem_init  ----- */


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_msgtab
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
CPSS_MSG_SEM_MANAGE * cpss_get_msgtab ()
{
	return &g_handleiocpmanage.stMsgTL;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_socket_init
 *  Description:  ƽ̨�׽ӿڳ�ʼ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_socket_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
	int ret;
	int nSend = 0;
	WSADATA wsaData;
#elif (OS_TYPE == OS_TYPE_LINUX)
#endif
	//memset(&g_handleiocpmanage, 0, sizeof(CPSS_IOCP_MANAGE));
	//	g_handleiocpmanage.nlSocketTcpfd = 0;
	//	g_handleiocpmanage.nlSocketUdpfd = 0;
	g_handleiocpmanage.dwTThreadId = 0;
	g_handleiocpmanage.dwUThreadId = 0;
	g_handleiocpmanage.pUsedSocketHead = NULL;
	g_handleiocpmanage.pUsedSocketTail = NULL;
	g_handleiocpmanage.pFreeSocketHead = NULL;
	g_handleiocpmanage.pFreeSocketTail = NULL;
	g_handleiocpmanage.hTcpRecv = NULL;
	g_handleiocpmanage.hTcpSend = NULL;
	g_handleiocpmanage.hUdpRecv = NULL;
	g_handleiocpmanage.hUdpSend = NULL;
	g_handleiocpmanage.pFreeTCPClientHead = NULL;
	g_handleiocpmanage.pFreeTCPClientTial = NULL;
	g_handleiocpmanage.pUsedTCPClientHead = NULL;
	g_handleiocpmanage.pUsedTCPClientTial = NULL;
	g_handleiocpmanage.ulClientCount  = 0;
	g_handleiocpmanage.ulClientOnline = 0;
	g_handleiocpmanage.ulTcpAccept	  = 0;
	
	g_handleiocpmanage.usExitSystem  = FALSE;

	VOS_Mutex_Init(&g_handleiocpmanage.hClientMutex, "VOS_MUTEX_CLIENT");
	VOS_Mutex_Init(&g_handleiocpmanage.hSocketMutex, "VOS_MUTEX_T_U_SOCKET");
	ulRet = VOS_OK;
#if (OS_TYPE == OS_TYPE_WINDOW)
	ret=WSAStartup(MAKEWORD(1,1),&wsaData);
	if(ret)
	{
		VOS_PrintErr(__FILE__,__LINE__,"WSAStartup init socket error");
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		WSACleanup( );
		return ulRet; 
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
#endif
	return ulRet;
}		/* -----  end of function cpss_socket_init  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_iocp_close
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_iocp_close ()
{
	CPSS_SOCKET_LINK * pSocketLinkTmp = NULL;
	while(NULL != pSocketLinkTmp)
	{
		cpss_close_socket(pSocketLinkTmp);
	}
	VOS_Mutex_Destroy(&g_handleiocpmanage.hClientMutex);
	VOS_Mutex_Destroy(&g_handleiocpmanage. hSocketMutex);
#if (OS_TYPE == OS_TYPE_WINDOW)
	WSACleanup( );
#elif (OS_TYPE == OS_TYPE_LINUX)
#endif
}		/* -----  end of function cpss_iocp_close  ----- */
/* ===  FUNCTION  =================================================================
 *         Name:  cpss_get_msg_tab_info
 *  Description:  ��ӡ��Ϣ����
 * ===============================================================================*/
VOS_VOID cpss_get_msg_tab_info(VOS_VOID * pVoidClient,VOS_UINT8 bStat)
{
	CPSS_MSG_STYLE * pMsgtab = &g_handleiocpmanage.stMsgTL.msgtab;
	CPSS_CLIENT_INFO * pClient = (CPSS_CLIENT_INFO *)pVoidClient;
	CPSS_MSG *pMsg = NULL;
	VOS_UINT32 leng = 0;
	if (NULL == pMsgtab)
	{
		return;
	}
	pMsg = pMsgtab->pFreeHead;
	VOS_PrintInfo(__FILE__,__LINE__,"msg free info count:%d\nHead:%08X Tial:%08X",
		pMsgtab->nFreeCount,
		pMsgtab->pFreeHead,
		pMsgtab->pFreeTial);
	while(NULL != pMsg)
	{
		cps_show_msg_info(pMsg);
		pMsg = pMsg->next;
	}
	if ((bStat & CPSS_MSG_SEND_TCP) == CPSS_MSG_SEND_TCP)
	{
		pMsg = pMsgtab->StyleSend.TypeTCP.pUseHead;
		VOS_PrintInfo(__FILE__,__LINE__,"msg send tcp info count:%d\nHead:%08X Tial:%08X",
			pMsgtab->StyleSend.TcpCount,
			pMsgtab->StyleSend.TypeTCP.pUseHead,
			pMsgtab->StyleSend.TypeTCP.pUseTial);
		while(NULL != pMsg)
		{
			cps_show_msg_info(pMsg);
			pMsg = pMsg->next;
		}
	}
	if ((bStat & CPSS_MSG_SEND_UDP) == CPSS_MSG_SEND_UDP)
	{
		pMsg = pMsgtab->StyleSend.TypeUDP.pUseHead;
		VOS_PrintInfo(__FILE__,__LINE__,"msg send udp info count:%d\nHead:%08X Tial:%08X",
			pMsgtab->StyleSend.UdpCount,
			pMsgtab->StyleSend.TypeUDP.pUseHead,
			pMsgtab->StyleSend.TypeUDP.pUseTial);
		while(NULL != pMsg)
		{
			cps_show_msg_info(pMsg);
			pMsg = pMsg->next;
		}
	}
	if ((bStat & CPSS_MSG_RECV_TCP) == CPSS_MSG_RECV_TCP)
	{
		pMsg = pMsgtab->StyleRecv.TypeTCP.pUseHead;
		VOS_PrintInfo(__FILE__,__LINE__,"msg recv tcp info count:%d\nHead:%08X Tial:%08X",
			pMsgtab->StyleRecv.TcpCount,
			pMsgtab->StyleRecv.TypeTCP.pUseHead,
			pMsgtab->StyleRecv.TypeTCP.pUseTial);
		while(NULL != pMsg)
		{
			cps_show_msg_info(pMsg);
			pMsg = pMsg->next;
		}
	}
	if ((bStat & CPSS_MSG_RECV_UDP) == CPSS_MSG_RECV_UDP)
	{
		pMsg = pMsgtab->StyleRecv.TypeUDP.pUseHead;
		VOS_PrintInfo(__FILE__,__LINE__,"msg recv udp info count:%d\nHead:08X Tial:08X",
			pMsgtab->StyleRecv.UdpCount,
			pMsgtab->StyleRecv.TypeUDP.pUseHead,
			pMsgtab->StyleRecv.TypeUDP.pUseTial);
		while(NULL != pMsg)
		{
			cps_show_msg_info(pMsg);
			pMsg = pMsg->next;
		}
	}
}

/* ===  FUNCTION  =================================================================
 *         Name:  cpss_get_msg_tab_info
 *  Description:  ��ӡ�ͻ��˶���
 * ===============================================================================*/
VOS_VOID cpss_get_cli_tab_info(VOS_VOID * pClient, VOS_UINT8 bStat)
{
	pCPSS_CLIENT_INFO pClientTmp = g_handleiocpmanage.pFreeTCPClientHead;

	VOS_PrintInfo(__FILE__, __LINE__, "Client[%d]\nFree Client Info:\nhead:%08Xtial:%08X",
		g_handleiocpmanage.ulClientCount,
		g_handleiocpmanage.pFreeTCPClientHead,
		g_handleiocpmanage.pFreeTCPClientTial);

	while(NULL != pClientTmp)
	{
		cpss_show_cli_info(pClientTmp);
		pClientTmp = pClientTmp->next;
	}

	pClientTmp = g_handleiocpmanage.pUsedTCPClientHead;

	VOS_PrintInfo(__FILE__, __LINE__, "Client[%d]\nUsed Client Info:\nhead:%08Xtial:%08X",
		g_handleiocpmanage.ulClientOnline,
		g_handleiocpmanage.pUsedTCPClientHead,
		g_handleiocpmanage.pUsedTCPClientTial);

	while(NULL != pClientTmp)
	{
		cpss_show_cli_info(pClientTmp);
		pClientTmp = pClientTmp->next;
	}
}