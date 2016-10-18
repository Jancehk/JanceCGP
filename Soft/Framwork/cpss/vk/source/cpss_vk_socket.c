/*==============================================================================
 *
 *       Filename:  cpss_vk_socket.c
 *
 *    Description:  socket 函数
 *
 *        Version:  1.0
 *        Created:  2011年05月23日 22时13分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ============================================================================*/
#include "cpss_vk_socket.h"
#include "cpss_common.h"
#include "cpss_com_pid.h"
#include "cpss_public.h"
#include "cpss_vk_print.h"


#define VOS_Skt_Malloc(ulSize)					VOS_Malloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Calloc(ulSize)					VOS_Calloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Realloc(pstrads,ulSize)			VOS_Realloc((pstrads), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Remset(pstrads)					VOS_Remset((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_MemSize(pstrads)				VOS_Memsize((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Memcls(pstrads)					VOS_Memcls((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Memcat(pstrA,pstrB)				VOS_Memcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_MemcatEx(pstrA,pstrB,ulSize)	VOS_MemcatEx((pstrA), (pstrB), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_MemcatEx2(pstrA,pstrB,ulSize)	VOS_MemcatEx2((pstrA), (pstrB), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SKT))
#define VOS_Skt_Free(pstrads)					VOS_Free((pstrads), CPSS_MEM_HEAD_KEY_CPSS_SKT)
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_socket_delete
*  Description:  将socket从指定的链表上移出
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
*  Description:  将socket从指定的链表上插入
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
	if (VOS_SOCKET_TCP != pSocketLinkTInfo->nlSocketType)
	{
		VOS_Destroy_Event(&pSocketLinkTInfo->pstuPid->pMsgEvent, 0);
	}
}		/* -----  end of function cpss_iocp_close  ----- */

/************************************************************************/
/*					Server Common Function                              */
/************************************************************************/

/* ===  FUNCTION  =================================================================
 *         Name:  cpss_show_cli_info
 *  Description:  打印客户端内容
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
		((pCPSS_SOCKET_LINK)CliTmp->pAccptSocket)->nlSocketfd,
		CliTmp->ulID,
		CliTmp->nType,
		CliTmp->nStat,
		CliTmp->strIPaddress,
		((pCPSS_SOCKET_LINK)CliTmp->pAccptSocket)->uPort,
		CliTmp->stuUserInfo.strUser,
		CliTmp->ulCientHaveTime,
		CliTmp->prev,
		CliTmp->next);
	VOS_PrintInfo("",CPSS_PRINTF_BUFFER,strbuff);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_client_info
 *  Description:  在空闲列别中取得客户信息,或者申请一份
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
		if (VOS_SOCKET_STAT_RESERVE == pClientInfoTmp->nStat)// free 是否被预约
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
	pClientInfoTmp = (pCPSS_CLIENT_INFO)VOS_Skt_Malloc(sizeof(CPSS_CLIENT_INFO));
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
	pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid = VOS_SOCKET_IN | uType | pSocketLink->uPort;
	
	ulPidIndex = cpss_get_id_for_pid_str(pSocketLink->pstuPid->szPidName);
	ulRet = cpss_update_cpuid_pid(cpss_get_systemid(), ulPidIndex,
		CPSS_STAT_ENABLE, pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid, CPSS_GET_TYPE_PID);
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
 *  Description:  等待Socket队列中的事象通知
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
	
	stime.tv_sec  = 5;
	stime.tv_usec = 800;
	//pstuSktfd->nCurrent = 0;
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
			//FD_SET(hSocketLink->nlSocketfd, &pstuSktfd->fderror);
			if (pstuSktfd->nMaxfd < hSocketLink->nlSocketfd)
			{
				pstuSktfd->nMaxfd = hSocketLink->nlSocketfd;
			}
		}
		hSocketLink = hSocketLink->next;
	}
	/*
	
	VOS_PrintDebug(__FILE__, __LINE__, "1 begin select [%d]:%d", uType,
		pstuSktfd->nMaxfd);*/
	if (0 == pstuSktfd->nMaxfd)
	{
		VOS_Sleep(10);
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
*  Description:  从事象中得到一个socket
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
		/*if (ulIndex < pstuSktfd->nCurrent)
		{
			hSocketLink = hSocketLink->next;
			continue;
		}*/
		if (hSocketLink->nlSocketType == uType &&
			hSocketLink->nlSocketfd != 0 &&
			CPSS_SKT_STAT_OPENED == hSocketLink->uIStat)
		{
			if (FD_ISSET(hSocketLink->nlSocketfd, &pstuSktfd->fdread) != 0)
			{
				//pstuSktfd->nCurrent++;
				return hSocketLink;
			}
			if (FD_ISSET(hSocketLink->nlSocketfd, &pstuSktfd->fderror) != 0)
			{
				//pstuSktfd->nCurrent++;
				return hSocketLink;
			}
		}
		hSocketLink = hSocketLink->next;
	}
	return NULL;
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_socket_from_client
*  Description:  通过client信息在socket使用表中找到对应的socket
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
*  Description:  初始化client结构体
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_VOID cpss_memset_client_info(CPSS_CLIENT_INFO *pClientInfo)
{
	CPSS_SOCKET_LINK *pstuClientSocket = NULL;
	pCPSS_MSG pMsg = NULL;
	/*
	pstuClientSocket = cpss_get_socket_from_client(pClientInfo);
	if (NULL != pstuClientSocket)
	{
		cpss_close_socket(pstuClientSocket);
	}*/
	BZERO(&pClientInfo->clientaddr, sizeof(SOCKADDR_IN));
	pClientInfo->dwTThreadId = 0;

	BZERO(pClientInfo->strIPaddress, CPSS_CLIENT_IPADS_LENGTH);
	BZERO(pClientInfo->stuUserInfo.strUser, CPSS_CLIENT_NAME_LENGTH);
	BZERO(pClientInfo->stuUserInfo.strPass, CPSS_CLIENT_PASS_LENGTH);
	pClientInfo->ulCientHaveTime = 0;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_used_to_free
 *  Description:  将使用的列表移动到空闲中
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
	if (NULL == pClientInfo)
	{
		return ulRet;
	}
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
				 NULL == pUClientHead->prev)// 最头上
		{
			piocpmanage->pUsedTCPClientHead = pUClientHead->next;
			piocpmanage->pUsedTCPClientHead->prev = NULL;
		}
		else if (NULL == pUClientHead->next &&
			     NULL != pUClientHead->prev)// 最尾部
		{
			piocpmanage->pUsedTCPClientTial = pUClientHead->prev;
			piocpmanage->pUsedTCPClientTial->next = NULL;
		}
		else if (NULL != pUClientHead->next &&
			     NULL != pUClientHead->prev)// 最尾部
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
		piocpmanage->ulClientOnline--;
		break;
	}
	
	VOS_Mutex_Unlock(&piocpmanage->hClientMutex);
	ulRet = VOS_OK;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_free_to_used
 *  Description:  将空闲的列表移动到使用中
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

/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_msg_mem_data
*  Description:  申请消息的空间
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 cps_get_msg_mem_data(CPSS_MSG * msgTmp)
{
	VOS_UINT32 ulRtn = VOS_ERR;
	VOS_CHAR * pstrTmp = NULL;

	if (NULL == msgTmp)
	{
		return ulRtn;
	}
	if (0 != VOS_Memcmp(msgTmp->Body.msghead.strSegName, CPSS_COMM_SEG_NAME,
		VOS_Strlen(CPSS_COMM_SEG_NAME)))
	{
		VOS_PrintErr(__FILE__, __LINE__, "Recv Head Seg Name Error");
		return ulRtn;
	}
	if (2 * 1024 * 1024 < msgTmp->Body.msghead.ulMsgLength)
	{
		VOS_PrintWarn(__FILE__, __LINE__, "recv data size is large %d:ere", msgTmp->Body.msghead.ulMsgLength);
		return ulRtn;
	}
	if (NULL == msgTmp->Body.strDataBuf)
	{
		msgTmp->Body.strDataBuf = (VOS_CHAR*)VOS_Skt_Calloc(msgTmp->Body.msghead.ulMsgLength);
	}
	else
	{
		pstrTmp = (VOS_CHAR*)VOS_Skt_Realloc(msgTmp->Body.strDataBuf, msgTmp->Body.msghead.ulMsgLength);
		if (NULL == pstrTmp)
		{
			VOS_Skt_Free(msgTmp->Body.strDataBuf);
		}
		msgTmp->Body.strDataBuf = pstrTmp;
	}
	if (NULL == msgTmp->Body.strDataBuf)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg data size is error");
		return ulRtn;
	}
	return VOS_OK;
}

/* ===  FUNCTION  =================================================================
*         Name:  cpss_get_socket_from_pid
*  Description:  从PID中获取一个socket
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
		hSocketProcTmp = (CPSS_SOCKET_LINK*)VOS_Skt_Malloc(sizeof(CPSS_SOCKET_LINK));
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
 *  Description: 在TCP中制定的socket上接收到数据
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static VOS_UINT32 cpss_tcp_recv_msg(pCPSS_SOCKET_LINK pSocketIN)
{
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			ulMsgLength = 0;
	VOS_UINT32			ulBodySize = 0;
	VOS_UINT32			ulMsgIndex = 0;
	CPSS_SOCKET_LINK  *	pSocketLink = NULL;
	CPSS_CLIENT_INFO  *	pClientInfo = NULL;
	CPSS_MSG		  * msgTmp = NULL;
	//pCPSS_MEM_BUFFER	pstuBuf = NULL;

	if (NULL == pSocketIN)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpg socket info is NULL");
		return ulRet;
	}
	pSocketLink = pSocketIN;
	pClientInfo = pSocketLink->pstuClientInfo;
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
	if (msgTmp->nRState != CPSS_MSG_RS_STAT_FREE || CPSS_MSG_SELF_STAT_RESERVE != msgTmp->nSelfStat)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg status is error[%d:%d]", msgTmp->nSelfStat, msgTmp->nRState);
		return ulRet;
	}
	msgTmp->nRState = CPSS_MSG_RS_STAT_RECVING;

	if (CPSS_SOCKET_PACKAGE_OFF == (pClientInfo->uClientStat & CPSS_SOCKET_PACKAGE_OFF))
	{
		VOS_Memcpy(msgTmp->Body.msghead.strSegName, CPSS_COMM_SEG_NAME, sizeof(CPSS_COMM_SEG_NAME));

		cps_set_msg_dst_cpuid(msgTmp, CPSSCPUID, pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
		cps_set_msg_src_cpuid(msgTmp, pSocketLink->stuSrcCPuID.ulCPuID,
			pSocketLink->stuSrcCPuID.ulPid);

		msgTmp->Body.msghead.ulMsgLength = CPSS_MSG_BUFFER_SIZE;
		if (CPSS_SOCKET_XCAP == pClientInfo->uClientStat)
		{
			msgTmp->Body.msghead.uType = cps_set_msg_type(XCAP_REQUEST_URL, XCAP_TYPE_URL, CPSS_MSG_REQ);;
		}

		if (VOS_OK != cps_get_msg_mem_data(msgTmp))
		{
			cpss_move_reserve_to_free(msgTmp);
			return VOS_CLIENT_DISCONNECT;
		}
		ulMsgLength = recv(pSocketLink->nlSocketfd,
			msgTmp->Body.strDataBuf,
			CPSS_MSG_BUFFER_SIZE,0);

		if (0 == ulMsgLength)
		{
			cpss_move_reserve_to_free(msgTmp);
			return VOS_CLIENT_DISCONNECT;
		}
		msgTmp->Body.msghead.ulMsgLength = ulMsgLength;
		if (VOS_OK != cps_get_msg_mem_data(msgTmp))
		{
			cpss_move_reserve_to_free(msgTmp);
			return VOS_CLIENT_DISCONNECT;
		}
		msgTmp->ulMsgLength = CPSS_MSG_HEAD_SIZE + ulMsgLength;
		VOS_PrintDebug(__FILE__, __LINE__, "<--RecvP ALL[%d]Head[%d]Data[%d]ID[%d] for T:%s",
			msgTmp->ulMsgLength, CPSS_MSG_HEAD_SIZE, ulMsgLength,
			msgTmp->ulMsgID,
			pSocketLink->pstuPid->szPidName);
		ulRet = VOS_OK;
	}
	else if (CPSS_SOCKET_PACKAGE_ON == (pClientInfo->uClientStat & CPSS_SOCKET_PACKAGE_ON))
	{
		ulBodySize = CPSS_MSG_HEAD_SIZE;//CPSS_MSG_HEAD_SIZE+CPSS_MSG_BUFFER_SIZE;
		ulMsgIndex = 0;
		while (ulMsgIndex != ulBodySize)
		{
			ulMsgLength = recv(pSocketLink->nlSocketfd,
				(VOS_CHAR*)&msgTmp->Body.msghead + ulMsgIndex,
				ulBodySize - ulMsgIndex, 0);

			VOS_PrintDebug(__FILE__, __LINE__, "recvfrom head total size is %d:%d",
				ulMsgLength, ulBodySize);

			if (0 == ulMsgLength)
			{
				VOS_PrintWarn(__FILE__, __LINE__, "break link %d:ErrorCode:%d", ulMsgLength, WSAGetLastError());
				goto END_DIS_PROC;
			}
			ulMsgIndex += ulMsgLength;
		}

		if (CPSSCPUID != msgTmp->Body.msghead.stDstProc.ulCpuID ||
			pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid != msgTmp->Body.msghead.stDstProc.ulPID)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Dst error:CPUID:%08d:%08d PID:%08d:%08d",
				CPSSCPUID, msgTmp->Body.msghead.stDstProc.ulCpuID,
				pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid, msgTmp->Body.msghead.stDstProc.ulPID);
			goto END_DIS_PROC;
		}
		if (VOS_OK != cps_get_msg_mem_data(msgTmp))
		{
			VOS_PrintErr(__FILE__, __LINE__, "get msg mem data is err");
			goto END_DIS_PROC;
		}

		ulMsgIndex = 0;
		ulBodySize = msgTmp->Body.msghead.ulMsgLength;

		VOS_PrintDebug(__FILE__, __LINE__, "recvfrom data total size is %d",
			ulBodySize);

		while (ulMsgIndex != ulBodySize && ulBodySize >0)
		{
			ulMsgLength = recv(pSocketLink->nlSocketfd,
				msgTmp->Body.strDataBuf + ulMsgIndex,
				ulBodySize - ulMsgIndex, 0);
			
			VOS_PrintDebug(__FILE__, __LINE__, "recv data total size is %d:%d",
				ulMsgLength, ulBodySize);

			if (0 == ulMsgLength)
			{
				VOS_PrintWarn(__FILE__, __LINE__, "break link %d:ErrorCode:%d", ulMsgLength, WSAGetLastError());
				goto END_DIS_PROC;
			}
			ulMsgIndex += ulMsgLength;
		}
		msgTmp->ulMsgLength = ulMsgIndex;
	}
	msgTmp->nRState = CPSS_MSG_RS_STAT_RECVED;
	ulRet = cpss_move_tcp_recv_free_to_used(msgTmp);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "move tcp to used error ");
		goto END_DIS_PROC;
	}
END_OK_PROC:
	return ulRet;
//END_ERR_PROC:
	cpss_move_reserve_to_free(msgTmp);
	ulRet = VOS_ERR;
	goto END_OK_PROC;
END_DIS_PROC:
	cpss_move_reserve_to_free(msgTmp);
	ulRet = VOS_CLIENT_DISCONNECT;
	goto END_OK_PROC;
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
 *         Name:  cpss_get_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static CPSS_MSG * cpss_get_send_msg(CPSS_MSG * pMsg)
{
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_MSG * pSendMsg = NULL;
	CPSS_MSG * pRecvMsg = NULL;

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
	VOS_Memcpy(pSendMsg->Body.msghead.strSegName, CPSS_COMM_SEG_NAME, sizeof(CPSS_COMM_SEG_NAME));

	VOS_Memcpy(&pSendMsg->Body.msghead.stSrcProc,
		&pMsg->Body.msghead.stDstProc,sizeof(CPSS_COM_PID));
	VOS_Memcpy(&pSendMsg->Body.msghead.stDstProc,
		&pMsg->Body.msghead.stSrcProc,sizeof(CPSS_COM_PID));
	pSendMsg->pClient = pMsg->pClient;
	/*
	VOS_PrintDebug(__FILE__,__LINE__,"Src:CpuID:%d,Pid:%d Des:CpuID:%d,Pid:%d",
		pMsg->Body.msghead.stSrcProc.ulCpuID,
		pMsg->Body.msghead.stSrcProc.ulPID,
		pMsg->Body.msghead.stDstProc.ulCpuID,
		pMsg->Body.msghead.stDstProc.ulPID);
	*/
	pSendMsg->Body.msghead.uType    = pMsg->Body.msghead.uType;
	pSendMsg->Body.msghead.uCmd     = pMsg->Body.msghead.uCmd;

	/* 设置接受和发送消息的ID */
	if (0 != pMsg->ulMsgID)
	{
		pRecvMsg = cpss_get_recv_msg_for_id(pMsg->ulMsgID);
		if (NULL == pRecvMsg)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get recv Msg for id:%d", pMsg->ulMsgID);
			return NULL;
		}
		/* 给发送出去的消息中记录该消息是否有接受消息ID */
		pSendMsg->Body.msghead.ulRecvMsgID = pMsg->ulMsgID;
		if (0 != pMsg->Body.msghead.ulParentMsgID)
		{
			pSendMsg->Body.msghead.ulParentMsgID = pMsg->Body.msghead.ulParentMsgID;
			pRecvMsg->nRState = CPSS_MSG_RS_STAT_SENDING;
		}
		else
		{
			pRecvMsg->nRState = CPSS_MSG_RS_STAT_SENDED;
		}
	}
	if (pMsg->Body.msghead.ulMsgLength > 0)
	{
		pSendMsg->Body.strDataBuf = (VOS_CHAR*)VOS_Skt_Calloc(pMsg->Body.msghead.ulMsgLength);
		if (NULL == pSendMsg->Body.strDataBuf)
		{
			VOS_PrintErr(__FILE__, __LINE__, "get send Msg address is Error %d", pMsg->ulMsgID);
			return NULL;
		}
		VOS_Memcpy(pSendMsg->Body.strDataBuf, pMsg->Body.strDataBuf, pMsg->Body.msghead.ulMsgLength);
	}
	pSendMsg->Body.msghead.ulMsgLength = pMsg->Body.msghead.ulMsgLength;
	pSendMsg->ulMsgLength = pMsg->ulMsgLength;
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
	VOS_INT32  stusktaddrsz = 0;
	//pCPSS_MEM_BUFFER   pstuBuff = NULL;
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
	// 接受数据包头
	stusktaddrsz = sizeof(stusktaddr);

	if (msgTmp->nRState != CPSS_MSG_RS_STAT_FREE || CPSS_MSG_SELF_STAT_RESERVE != msgTmp->nSelfStat)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg status is error[%d:%d]", msgTmp->nSelfStat, msgTmp->nRState);
		return ulRet;
	}
	msgTmp->nRState = CPSS_MSG_RS_STAT_RECVING;

	ulBodySize = CPSS_MSG_HEAD_SIZE;//CPSS_MSG_HEAD_SIZE+CPSS_MSG_BUFFER_SIZE;
	ulMsgIndex = 0;
	while (ulMsgIndex != ulBodySize)
	{
		ulMsgLength = recvfrom(
			hSocketLink->nlSocketfd, 
			(VOS_CHAR*)&(msgTmp->Body.msghead) + ulMsgIndex,
			ulBodySize-ulMsgIndex,
			0,
			&stusktaddr,
			&stusktaddrsz);
		if (0 == ulMsgLength || 0xFFFFFFFF == ulMsgLength)
		{
			if (10054 == WSAGetLastError())
			{
				VOS_PrintErr(__FILE__, __LINE__, "Specifies the target does not exist![%d]",
					hSocketLink->nlSocketfd);
			}
			else
			{
				VOS_PrintErr(__FILE__, __LINE__, "RecvF1[H]Prepare[%d]Already[%d]Real[%d]:E[%d]",
					ulBodySize, ulMsgIndex, ulMsgLength, WSAGetLastError());
			}
			ulRet = VOS_OK;
			goto END_PROC;
		}
		if (ulMsgIndex != ulBodySize && 0 != ulMsgIndex)
		{
			VOS_PrintDebug(__FILE__, __LINE__, "RecvF[H]Prepare[%d]Already[%d]Real[%d]",
				ulBodySize, ulMsgIndex, ulMsgLength);
		}
		ulMsgIndex += ulMsgLength;
	}
	msgTmp->ulMsgLength = ulMsgIndex;
	/*
	VOS_PrintDebug(__FILE__, __LINE__, "RecvF Head[%d]Data[%d]",
		msgTmp->ulMsgLength, msgTmp->Body.msghead.ulMsgLength);
	*/
	if (VOS_OK != cps_get_msg_mem_data(msgTmp))
	{
		VOS_PrintErr(__FILE__, __LINE__, "get msg mem data is err");
		goto END_PROC;
	}

	ulMsgIndex = 0;
	ulBodySize = msgTmp->Body.msghead.ulMsgLength;
	
	while (ulMsgIndex != ulBodySize)
	{
		ulMsgLength = recvfrom(
			hSocketLink->nlSocketfd,
			msgTmp->Body.strDataBuf + ulMsgIndex,
			ulBodySize - ulMsgIndex,
			0,
			&stusktaddr,
			&stusktaddrsz);
		
		if (0 == ulMsgLength)
		{
			VOS_PrintErr(__FILE__, __LINE__, "RecvF recv %d:ere%d", ulMsgLength, WSAGetLastError());
			continue;
		}
		if (ulMsgIndex != ulBodySize && 0 != ulMsgIndex)
		{
			VOS_PrintDebug(__FILE__, __LINE__, "RecvF[D]Prepare[%d]Already[%d]Real[%d]",
				ulBodySize, ulMsgIndex, ulMsgLength);
		}
		ulMsgIndex += ulMsgLength;
	}

	msgTmp->ulMsgLength += ulMsgIndex;
	VOS_PrintDebug(__FILE__, __LINE__, "<--RecvF ALL[%d]Head[%d]Data[%d]ID[%d] for %s",
		msgTmp->ulMsgLength, CPSS_MSG_HEAD_SIZE, ulBodySize,
		msgTmp->ulMsgID,
		hSocketLink->pstuPid->szPidName);
	//msgTmp->pClient = hSocketLink;
	msgTmp->nRState = CPSS_MSG_RS_STAT_RECVED;
	ulSocRet = VOS_OK;

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
 *         Name:  cpss_tcp_timer_used_to_free_proc
 *  Description:  处理TCP 内容超时处理
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
 *  Description:  Services接受tcp的客户端过来的数据包并放到消息队列里面
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
		uSelectCount = cpss_wait_socket(VOS_SOCKET_TCP, &stuSktfd);
		if (1 > uSelectCount)
		{
			continue;
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
			cpss_close_socket(pSocketInfo);
			continue;
		}

		//VOS_PrintDebug(__FILE__, __LINE__, "TC recv set event %s", pSocketInfo->pstuPid->pMsgEvent.strmutex);
		ulRet = VOS_Set_Event(&pSocketInfo->pstuPid->pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
				pSocketInfo->pstuPid->pCPuID->stuCPuid.ulPid);
			cpss_move_used_to_free(piocpmanage, pClientInfo);
			cpss_close_socket(pSocketInfo);
		//	cpss_move_reserve_to_free(pSocketInfo->pstuRecvMsg);
		}
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_send_proc
 *  Description:  Services发送tcp的数据包 从消息队列里面
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
	VOS_UINT32			nSendHeadLength = 0;
	VOS_UINT32			nSendBodyLength = 0;
	CPSS_CLIENT_INFO  * pClient = NULL;
	CPSS_SOCKET_LINK  * pSocketInfo = NULL;
	//CPSS_SOCKET_LINK  * pSocket = NULL;
	CPSS_MSG		  * pstuMsg = NULL;
	CPSS_MSG		  * pstuMsgParent = NULL;
	
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
			ulRet = VOS_Wait_Event(&g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TSEND].pMsgEvent, INFINITE);//INFINITE
			if (VOS_OK != ulRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "tcp send Wait Event error");
			}
			continue;
		}
		if (CPSS_MSG_SELF_STAT_USEING != pstuMsg->nSelfStat)/*VOS_CLIENT_DISCONNECT*/
		{
			VOS_PrintErr(__FILE__, __LINE__, "msg stat[%d] is disonnect ", pstuMsg->nSelfStat);
			goto ERR_CONTINUE;
		}
		pSocketInfo = cpss_get_info_for_pid(pstuMsg->Body.msghead.stDstProc.ulPID, CPSS_GET_SKT_LINK_SOCKET);
		//uSocketfd = pstuMsg->pClient;
		if (NULL == pSocketInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%d UDP send msg socket is NULL",
				pstuMsg->Body.msghead.stSrcProc.ulPID);
			cpss_move_udp_send_used_to_free(pstuMsg);
			goto ERR_CONTINUE;
		}
		pClient = (pCPSS_CLIENT_INFO)pSocketInfo->pstuClientInfo;
		if (NULL == pClient)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%d pid socket[%p] client info is null",
				pstuMsg->Body.msghead.stSrcProc.ulPID, pSocketInfo);
			goto ERR_CONTINUE;
		}
		nSendHeadLength = 0;
		nSendBodyLength = 0;

		if (CPSS_SOCKET_PACKAGE_ON == (pClient->uClientStat & CPSS_SOCKET_PACKAGE_ON))
		{
			nSendSize = send(pSocketInfo->nlSocketfd, (VOS_CHAR*)&pstuMsg->Body.msghead, CPSS_MSG_HEAD_SIZE, 0);
			if (0 == nSendSize)
			{
				VOS_PrintErr(__FILE__, __LINE__, "send date leng:%d errorCode=%d",
					nSendSize, GetLastError());
				goto ERR_CONTINUE;
			}
			if (nSendSize != CPSS_MSG_HEAD_SIZE)
			{
				VOS_PrintErr(__FILE__, __LINE__, "send date leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
					nSendSize,
					GetLastError(),
					pstuMsg->Body.msghead.stDstProc.ulCpuID,
					pstuMsg->Body.msghead.stDstProc.ulPID);
				goto ERR_CONTINUE;
			}
			nSendHeadLength = CPSS_MSG_HEAD_SIZE;
		}
		nSendSize = send(pSocketInfo->nlSocketfd, (VOS_CHAR*)pstuMsg->Body.strDataBuf, pstuMsg->Body.msghead.ulMsgLength, 0);
		if (0 == nSendSize)
		{
			VOS_PrintErr(__FILE__, __LINE__, "send date leng:%d errorCode=%d",
				nSendSize, GetLastError());
			goto ERR_CONTINUE;
		}
		if (nSendSize != pstuMsg->Body.msghead.ulMsgLength)
		{
			VOS_PrintErr(__FILE__, __LINE__, "send date leng:%d[%d] errorCode=%d,CpuID:%08u,Pid:%d",
				nSendSize,
				pstuMsg->Body.msghead.ulMsgLength,
				GetLastError(),
				pstuMsg->Body.msghead.stDstProc.ulCpuID,
				pstuMsg->Body.msghead.stDstProc.ulPID);
			goto ERR_CONTINUE;
		}
		nSendBodyLength = nSendSize;
		
		VOS_PrintDebug(__FILE__, __LINE__, "-->Send ALL[%d]Data[%d]S_ID[3]R_ID[0]",
			nSendBodyLength + nSendHeadLength,
			nSendBodyLength,
			pstuMsg->ulMsgID,
			pstuMsg->Body.msghead.ulRecvMsgID);
		goto CLR_CONTINUE;
ERR_CONTINUE:
		cpss_move_used_to_free(&g_handleiocpmanage, pClient);
CLR_CONTINUE:
		if (0 != pstuMsg->Body.msghead.ulRecvMsgID)
		{
			cpss_move_recv_used_to_free_use_msgid(pstuMsg->Body.msghead.ulRecvMsgID);
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
static VOS_UINT32 cpss_tcp_send_msg (VOS_VOID *pVoidMsg)
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
	/*
	pClient = (pCPSS_CLIENT_INFO)pMsg->pClient;
	if (NULL == pClient)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send Client info is NULL");
		return ulRet;
	}
	*/
	pSendMsg = cpss_get_send_msg(pMsg);
	if (NULL == pSendMsg)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get send msg is NULL");
		return ulRet;
	}
	pSendMsg->pClient = pMsg->pClient;
	cpss_move_tcp_send_free_to_used(pSendMsg);

	ulRet = VOS_Set_Event(&g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TSEND].pMsgEvent);
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
 *         Name:  cpss_udp_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
static VOS_UINT32 cpss_udp_send_msg (VOS_VOID *pVoidMsg)
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

	ulRet = VOS_Set_Event(&g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].pMsgEvent);
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
*         Name:  cpss_save_msg_to_recv
*  Description:  保持msg到recv消息队列
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 cpss_save_msg_to_recv(VOS_VOID *pVoidMsg, VOS_UINT32 uType)
{
	VOS_UINT32			ulRet = VOS_ERR;
	pCPSS_MSG			pMsg = NULL;
	CPSS_PID_TABLE *	pstuPid = NULL;				// Pid Handle
	if (0 == (uType & 0x000000ff))
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg type is incorrect");
		return ulRet;
	}
	if (NULL == ((pCPSS_MSG)pVoidMsg)->pPid)
	{
		VOS_PrintErr(__FILE__, __LINE__, "send msg pid info is null");
		return ulRet;
	}
	pstuPid = (PCPSS_PID_TABLE)((pCPSS_MSG)pVoidMsg)->pPid;
	pMsg = cpss_check_send_msg(pVoidMsg);
	if (NULL == pMsg)
	{
		VOS_PrintErr(__FILE__, __LINE__, "check send msg is NULL");
		return ulRet;
	}
	pMsg = cpss_get_send_msg(pVoidMsg);
	if (NULL == pMsg)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get send msg is NULL");
		return ulRet;
	}
	pMsg->nRState = CPSS_MSG_RS_STAT_RECVED;
	if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
	{
		ulRet = cpss_move_tcp_recv_free_to_used(pMsg);
	}
	else if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
	{
		ulRet = cpss_move_udp_recv_free_to_used(pMsg);
	}
	else
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg type is incorrect");
		ulRet = VOS_ERR;
	}
	if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
	{
		VOS_PrintDebug(__FILE__, __LINE__, "<--RecvA ALL[%d]Head[%d]Data[%d]ID[%d] for T:%s",
			pMsg->ulMsgLength, CPSS_MSG_HEAD_SIZE, pMsg->ulMsgLength - CPSS_MSG_HEAD_SIZE,
			pMsg->ulMsgID,
			pstuPid->szPidName);
	}
	else if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
	{
		VOS_PrintDebug(__FILE__, __LINE__, "<--RecvA ALL[%d]Head[%d]Data[%d]ID[%d] for U:%s",
			pMsg->ulMsgLength, CPSS_MSG_HEAD_SIZE, pMsg->ulMsgLength - CPSS_MSG_HEAD_SIZE,
			pMsg->ulMsgID,
			pstuPid->szPidName);
	}
	if (VOS_OK != ulRet)
	{
		cpss_move_reserve_to_free(pMsg);
		VOS_PrintErr(__FILE__, __LINE__, "move tcp recv to used failed :%p", pMsg);
		return ulRet;
	}
	ulRet = VOS_Set_Event(&pstuPid->pMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set Event faild,CpuID:%d,Pid:%d",
			pMsg->Body.msghead.stSrcProc.ulCpuID,
			pMsg->Body.msghead.stSrcProc.ulPID);
		cpss_move_reserve_to_free(pMsg);
		return ulRet;
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_copy_msg
 *  Description:  发送telnet的数据
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
	VOS_Memcpy(&pMsgSend->Body.msghead.stSrcProc,
		&pMsgRecv->Body.msghead.stSrcProc, sizeof(CPSS_COM_PID));
	VOS_Memcpy(&pMsgSend->Body.msghead.stDstProc,
		&pMsgRecv->Body.msghead.stDstProc, sizeof(CPSS_COM_PID));
	//pMsgRecv->Body.msghead.ulMsgID = pMsgSend->Body.msghead.ulMsgID;
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
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			ulSubSysID = 0;
	VOS_CHAR *			strSendBuff = NULL;
	pCPSS_CPUID_TABLE	pstuCPuIDTemp = NULL;
	CPSS_MSG *			pMsgInfo = NULL;
	CPSS_COM_PID		stuPidTmp;

	if (NULL == pVoidMsg)
	{
		VOS_PrintErr(__FILE__, __LINE__, "send data param is NULL");
		return ulRet;
	}

	pMsgInfo = (pCPSS_MSG)pVoidMsg;
	pMsgInfo->ulMsgLength = CPSS_MSG_HEAD_SIZE;
	if (NULL != strBuffer && 0 < uBufLen)
	{
		strSendBuff = VOS_Skt_MemcatEx(pMsgInfo->Body.strDataBuf, strBuffer, uBufLen);
		if (NULL == strSendBuff)
		{
			VOS_PrintErr(__FILE__, __LINE__, "send data get memory is error:%d",GetLastError());
			return ulRet;
		}
		pMsgInfo->Body.strDataBuf = strSendBuff;
		pMsgInfo->Body.msghead.ulMsgLength = uBufLen;
		pMsgInfo->ulMsgLength +=  uBufLen;
		/*VOS_PrintDebug(__FILE__, __LINE__, "send len:%d buff len:%d",
			pMsgInfo->Body.msghead.ulMsgLength + CPSS_MSG_HEAD_SIZE, uBufLen);*/
	}
	if (VOS_SEND_SKT_TYPE_INSERT == (uType & VOS_SEND_SKT_TYPE_INSERT))
	{
		return VOS_OK;
	}
	if (VOS_SEND_SAVE_TO_RECV == (uType & VOS_SEND_SAVE_TO_RECV))
	{
		ulRet = cpss_save_msg_to_recv(pMsgInfo, uType);
		if (VOS_ERR == ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss send tcp data error[%d]", ulRet);
		}
		ulRet = VOS_OK;
		goto EXIT_OK;
	}
	if (VOS_SEND_RECV_RESPONSE == (uType & VOS_SEND_RECV_RESPONSE))
	{
		memcpy(&stuPidTmp, &pMsgInfo->Body.msghead.stDstProc, sizeof(stuPidTmp));
		memcpy(&pMsgInfo->Body.msghead.stDstProc, &pMsgInfo->Body.msghead.stSrcProc, sizeof(stuPidTmp));
		memcpy(&pMsgInfo->Body.msghead.stSrcProc, &stuPidTmp, sizeof(stuPidTmp));
		pMsgInfo->Body.msghead.uType++;
	}
	if (VOS_SEND_ALL_OF_PID == (uType & VOS_SEND_ALL_OF_PID))
	{
		pstuCPuIDTemp = cpss_get_cpuid_header();
		ulSubSysID = pMsgInfo->Body.msghead.stSrcProc.ulPID;
		while (NULL != pstuCPuIDTemp)
		{
			if (pstuCPuIDTemp->stuCPuID_Info.ulSubsysID != ulSubSysID)
			{
				pstuCPuIDTemp = pstuCPuIDTemp->next;
				continue;
			}
			if (pstuCPuIDTemp->stuCPuID_Info.ulSystemID ==
				cpss_get_systemid())
			{
				pstuCPuIDTemp = pstuCPuIDTemp->next;
				continue;
			}
			cps_set_msg_to_cpuid(pMsgInfo, 
				pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulCPuID,
				pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulPid);
			if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
			{
				ulRet = cpss_tcp_send_msg(pMsgInfo);
				if (VOS_ERR == ulRet)
				{
					VOS_PrintErr(__FILE__, __LINE__, "cpss send tcp data error[%d]", ulRet);
				}
			}
			else
			if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
			{
				ulRet = cpss_udp_send_msg(pMsgInfo);
				if (VOS_ERR == ulRet)
				{
					VOS_PrintErr(__FILE__, __LINE__, "cpss send udp data error[%d]", ulRet);
				}
			}

			pstuCPuIDTemp = pstuCPuIDTemp->next;
		}
		ulRet = VOS_OK;
		goto EXIT_OK;
	}
	
	if (VOS_SEND_SKT_TYPE_TCP == (uType & VOS_SEND_SKT_TYPE_TCP))
	{
		ulRet = cpss_tcp_send_msg(pMsgInfo);
		if (VOS_ERR == ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss send tcp data error");
		}
	}else if (VOS_SEND_SKT_TYPE_UDP == (uType & VOS_SEND_SKT_TYPE_UDP))
	{
		ulRet = cpss_udp_send_msg(pMsgInfo);
		if (VOS_ERR == ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss send udp data error");
		}
	}
EXIT_OK:
	VOS_Skt_Free(pMsgInfo->Body.strDataBuf);
	pMsgInfo->Body.strDataBuf = NULL;
	//BZERO(&pMsgInfo->Body.strDataBuf, sizeof(CPSS_MEM_BUFFER));
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
 *  Description:  Services接受UDP数据的线程函数
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
		uSelectCount = cpss_wait_socket(VOS_SOCKET_UDP, &stuSktfd);
		if (1 > uSelectCount)
		{
			continue;
		}
		pSocketLink = cpss_get_socket_from_fdset(VOS_SOCKET_UDP, &stuSktfd);
		if (NULL == pSocketLink)
		{
			continue;
		}
		ulRet = cpss_udp_recv_msg(pSocketLink, CPSS_SOCKET_PACKAGE_OFF);
		if (ulRet == VOS_ERR)
		{
			//VOS_PrintErr(__FILE__, __LINE__, "recv socket msg [%d]error", pSocketLink->nlSocketfd);
			continue;
		}

		//			VOS_PrintInfo(__FILE__,__LINE__,"UDP recv set event %s", hSocketLink->szPidName);
		ulRet = VOS_Set_Event(&pSocketLink->pstuPid->pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
				pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
		}
	}
	return ulRet;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_udp_send_proc
 *  Description:  Services发送tcp的数据包 从消息队列里面
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
	CPSS_SOCKET_LINK  * pSocketInfo = NULL;
	VOS_UINT32			nSendSize = 0;
	VOS_UINT32			nSendLength = 0;
	VOS_UINT32			nSendBuffLen = 0;
	SOCKADDR_IN			stusktaddr;
	VOS_UINT32			bcastAddr = 0;
	VOS_UINT16			bPort = 0;

	if (0 == g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].pMsgEvent.strmutex[0])
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
			ulRet = VOS_Wait_Event(&g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].pMsgEvent, INFINITE);
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
			goto SEND_OVER;
		}

		bcastAddr = pstuMsg->Body.msghead.stDstProc.ulCpuID;
		bPort = pstuMsg->Body.msghead.stDstProc.ulPID & VOS_SOCKET_PORT;

		BZERO(&stusktaddr, sizeof(SOCKADDR_IN));
		stusktaddr.sin_family=AF_INET;
		stusktaddr.sin_addr.s_addr = get_cpuid_from_ip(bcastAddr) ;
		stusktaddr.sin_port=htons(bPort);

		VOS_PrintDebug(__FILE__,__LINE__,"DC:%ud:%d[%d] SC:%ud:%d[%d]",
			pstuMsg->Body.msghead.stDstProc.ulCpuID,
			pstuMsg->Body.msghead.stDstProc.ulPID,
			bPort,
			pstuMsg->Body.msghead.stSrcProc.ulCpuID,
			pstuMsg->Body.msghead.stSrcProc.ulPID,
			pstuMsg->Body.msghead.stSrcProc.ulPID & VOS_SOCKET_PORT);


		nSendSize = sendto(pSocketInfo->nlSocketfd,
			(VOS_CHAR*)&pstuMsg->Body.msghead, CPSS_MSG_HEAD_SIZE, 0,
				(struct sockaddr *)&stusktaddr,sizeof(stusktaddr));
		if (0xFFFFFFFF == nSendSize)
		{
			VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d",
				nSendSize, GetLastError());
			goto SEND_OVER;
		}
		if (nSendSize != CPSS_MSG_HEAD_SIZE)
		{
			VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
				nSendSize,
				GetLastError(),
				pstuMsg->Body.msghead.stDstProc.ulCpuID,
				pstuMsg->Body.msghead.stDstProc.ulPID);
			goto SEND_OVER;
		}
		/*
		VOS_PrintDebug(__FILE__, __LINE__, "SendT Head[%d]Data[%d][%p]",
			nSendSize, pstuMsg->Body.msghead.ulMsgLength, pstuMsg->Body.strDataBuf);
		*/
		nSendBuffLen = 0;
		nSendLength = pstuMsg->ulMsgLength - CPSS_MSG_HEAD_SIZE;// + pstuBuf->nSize;
		while (NULL != pstuMsg->Body.strDataBuf && nSendBuffLen<nSendLength)
		{
			nSendLength -= nSendBuffLen;
			nSendSize = sendto(pSocketInfo->nlSocketfd,
				(VOS_CHAR*)pstuMsg->Body.strDataBuf, nSendLength, 0,
				(struct sockaddr *)&stusktaddr, sizeof(stusktaddr));
			if (0xFFFFFFFF == nSendSize || 0 == nSendSize)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send head leng:%d errorCode=%d",
					nSendSize, GetLastError());
				goto SEND_OVER;
			}
			//VOS_PrintDebug(__FILE__,__LINE__,"send body data %d:%d",nSendSize,nSendLength);
			if (nSendSize != nSendLength)
			{
				VOS_PrintErr(__FILE__,__LINE__,"send date leng:%d errorCode=%d,CpuID:%08X,Pid:%d",
					nSendSize,
					GetLastError(),
					pstuMsg->Body.msghead.stDstProc.ulCpuID,
					pstuMsg->Body.msghead.stDstProc.ulPID);
				goto SEND_OVER;
			}
			nSendBuffLen += nSendSize;

		}
		if (nSendLength != pstuMsg->Body.msghead.ulMsgLength)
		{
			VOS_PrintErr(__FILE__,__LINE__,"send date send:%d:Msg%d",
				nSendLength, pstuMsg->Body.msghead.ulMsgLength);
			goto SEND_OVER;
		}
		VOS_PrintDebug(__FILE__, __LINE__, "-->SendT ALL[%d]Head[%d]Data[%d]S_ID[%d]R_ID[%d]",
			pstuMsg->ulMsgLength, CPSS_MSG_HEAD_SIZE,
			pstuMsg->Body.msghead.ulMsgLength, pstuMsg->ulMsgID,
			pstuMsg->Body.msghead.ulRecvMsgID);


	SEND_OVER:
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
 *  Description:  Services接受tcp的线程函数
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
	VOS_CHAR		* stuBuffer = NULL;
	CPSS_SOCKET_LINK * pSocket = NULL;
	CPSS_PID_THREAD_INFO * pstuPidThrInfo = (CPSS_PID_THREAD_INFO*)lpParameter;
	CPSS_PID_TABLE * pstuPidInfo = NULL;

	if (NULL == pstuPidThrInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss distribute thread input Error ");
		return uRet;
	}
	pstuPidInfo = (CPSS_PID_TABLE*)pstuPidThrInfo->pPidTbl;
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
	if (pstuPidInfo->ulPidCount > 1)
	{
		VOS_PrintBuffer(&stuBuffer, "T [%s:%d] Distribute", pstuPidInfo->szPidName, pstuPidThrInfo->nPID_ID);
	}
	else
	{
		VOS_PrintBuffer(&stuBuffer, "T [%s] Distribute", pstuPidInfo->szPidName);
	}
	cpss_thread_success(stuBuffer);
	VOS_PrintBufferRelease(stuBuffer);

	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			uRet = VOS_OK;
			break;
		}
		pMsgInfo = cpss_get_used_msg(pstuPidInfo->pCPuID->stuCPuid.ulPid, GET_TCP_RECV_USED);
		if (NULL == pMsgInfo)
		{
			if (0 == pstuPidInfo->pMsgEvent.strmutex[0])
			{
				VOS_Sleep(10);
				continue;
			}
			uRet = VOS_Wait_Event(&pstuPidInfo->pMsgEvent, INFINITE);
			if (VOS_OK != uRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "PID %08X Wait Event error",
					pstuPidInfo->pCPuID->stuCPuid.ulPid);
			}
			continue;
		}
		if (NULL == pMsgInfo->pClient)
		{
			VOS_PrintErr(__FILE__,__LINE__,"PID Msg %p", pMsgInfo);
			cpss_move_tcp_recv_used_to_free(pMsgInfo);
			continue;
		}
		pSocket = cpss_get_info_for_pid(pMsgInfo->Body.msghead.stDstProc.ulPID, CPSS_GET_SKT_LINK_SOCKET);
		if (NULL == pSocket || pSocket->pstuPid != pstuPidInfo)
		{
			VOS_PrintErr(__FILE__,__LINE__,"PID[%s] Msg  pSocket %p is error ",
				pstuPidInfo->szPidName, pSocket);
			cpss_move_tcp_recv_used_to_free(pMsgInfo);
			continue;
		}
/*
		pMsgInfo->pTimer = (VOS_VOID *)cpss_set_timer(300, cpss_tcp_timer_used_to_free_proc, 
			pMsgInfo, sizeof(CPSS_MSG));
*/
		pMsgInfo->nRState = CPSS_MSG_RS_STAT_DOING;
		if (VOS_OK != pstuPidInfo->ppid_init_proc((VOS_VOID *)pMsgInfo))
		{
			VOS_PrintErr(__FILE__,__LINE__,"Msg:%p Socket:%p Stat:%x exec is faild",
				pMsgInfo, pSocket, pMsgInfo->nRState);
		}
		if (CPSS_MSG_RS_STAT_SENDED == pMsgInfo->nRState ||
			CPSS_MSG_RS_STAT_DOING == pMsgInfo->nRState)
		{
			cpss_move_recv_used_to_free(pMsgInfo);
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
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			ulPort = 0;
	VOS_INT32			nlClientSocketfd = SOCKET_ERROR;
	CPSS_SOCKET_LINK *	pAccptSocket = NULL;
	CPSS_SOCKET_LINK *	pRecvSocket;
	CPSS_CLIENT_INFO *	pClientInfo =NULL;
	CPSS_MSG		    stuMsgInfo = { 0 };
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
		pAccptSocket->pstuPid->pCPuID->stuCPuid.ulPid);
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
	ulPort = htons(pClientInfo->clientaddr.sin_port);
	pRecvSocket->nlSocketType = VOS_SOCKET_TCP;
	pRecvSocket->pstuPid = pAccptSocket->pstuPid;
	pRecvSocket->uPort = ulPort;
	pRecvSocket->nlSocketfd = nlClientSocketfd;
	pRecvSocket->pstuClientInfo = pClientInfo;
	VOS_Strcpy(pClientInfo->strIPaddress,inet_ntoa(pClientInfo->clientaddr.sin_addr));
	pClientInfo->pClientSocket = pRecvSocket;
	pRecvSocket->stuSrcCPuID.ulCPuID = get_cpuid_from_str(pClientInfo->strIPaddress);
	pRecvSocket->stuSrcCPuID.ulPid = VOS_SOCKET_OUT | VOS_TCP_PID | ulPort;
	
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
		cps_set_msg_src_cpuid(&stuMsgInfo, CPSSCPUID, pRecvSocket->pstuPid->pCPuID->stuCPuid.ulPid);
		cps_set_msg_dst_cpuid(&stuMsgInfo, pRecvSocket->stuSrcCPuID.ulCPuID,
			pRecvSocket->stuSrcCPuID.ulPid);
		stuMsgInfo.pClient = pClientInfo;
		stuMsgInfo.pPid = pRecvSocket->pstuPid;
		stuMsgInfo.Body.msghead.uType = cps_set_msg_type(CPSS_REQUEST_SYSTEM, XCAP_TYPE_NEW_CLIENT, CPSS_MSG_INIT);
		if (CPSS_SOCKET_TELNET == pClientInfo->uClientStat)
		{
			stuMsgInfo.Body.msghead.uCmd = CPSS_CMD_SYSTEM_TELNET;//CPSS_TYPE_SYSTEM_TELNET;
		}
		ulRet = cpss_send_data(&stuMsgInfo, NULL, 0, 
			VOS_SEND_SAVE_TO_RECV | VOS_SEND_SKT_TYPE_TCP);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Self recv tcp data error in accept client");
			goto ERR_PROC;
		}
	}
	VOS_PrintDebug(__FILE__,__LINE__,"Client:%d info:%s:%d was connection",
		pClientInfo->ulID, pClientInfo->strIPaddress, pRecvSocket->uPort);
	pRecvSocket->uIStat = CPSS_SKT_STAT_OPENED;
END_PROC:
	if (VOS_ERR == ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "accept new client is error");
	}
	return ulRet;
ERR_PROC:
	if (NULL != pClientInfo)
	{
		cpss_memset_client_info(pClientInfo);
	}
	ulRet = VOS_ERR;
	goto END_PROC;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_tcp_accept_client_proc
 *  Description:  Services接受tcp客户端的线程函数
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
		uSelectCount = cpss_wait_socket(VOS_SOCKET_ACC, &stuSktfd);
		if (1 > uSelectCount)
		{
			continue;
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
 *  Description:  接受udp的线程函数
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
	CPSS_SOCKET_LINK * pSocket = NULL;
	CPSS_CLIENT_INFO * pClient = NULL;
	VOS_CHAR		 * stuBuffer = NULL;
	CPSS_PID_THREAD_INFO * pstuPidThrInfo = (CPSS_PID_THREAD_INFO*)lpParameter;
	CPSS_PID_TABLE * pstuPidInfo = NULL;

	if (NULL == pstuPidThrInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss distribute thread input Error ");
		return uRet;
	}
	pstuPidInfo = (CPSS_PID_TABLE*)pstuPidThrInfo->pPidTbl;

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
	if (pstuPidInfo->ulPidCount > 1)
	{
		VOS_PrintBuffer(&stuBuffer, "U [%s:%d] Distribute", pstuPidInfo->szPidName, pstuPidThrInfo->nPID_ID);
	}
	else
	{
		VOS_PrintBuffer(&stuBuffer, "U [%s] Distribute", pstuPidInfo->szPidName);
	}
	cpss_thread_success(stuBuffer);
	VOS_PrintBufferRelease(stuBuffer);

	((CPSS_SOCKET_LINK*)pstuPidInfo->pSocketInfo)->uIStat = CPSS_SKT_STAT_OPENED;
	while(FALSE == g_handleiocpmanage.usExitSystem)
	{
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			uRet = VOS_OK;
			break;
		}
		pMsgInfo = cpss_get_used_msg(pstuPidInfo->pCPuID->stuCPuid.ulPid, GET_UDP_RECV_USED);
		if (NULL == pMsgInfo)
		{
			uRet = VOS_Wait_Event(&pstuPidInfo->pMsgEvent, INFINITE);
			if (VOS_OK != uRet)
			{
				VOS_PrintErr(__FILE__,__LINE__,"PID %08X Wait Event error",
					pstuPidInfo->pCPuID->stuCPuid.ulPid);
			}
			continue;
		}
		pSocket = cpss_get_info_for_pid(pMsgInfo->Body.msghead.stDstProc.ulPID, CPSS_GET_SKT_LINK_SOCKET);
		if (NULL == pSocket || pSocket->pstuPid != pstuPidInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "PID %08X[%08X] get socket is error",
				pstuPidInfo->pCPuID->stuCPuid.ulPid,
				pMsgInfo->Body.msghead.stDstProc.ulPID);
			goto CONTINUE_GO;
		}
		/*
		pMsgInfo->pTimer = (VOS_VOID *)cpss_set_timer(300, cpss_tcp_timer_used_to_free_proc, 
			pMsgInfo, sizeof(CPSS_MSG));
		*/	
		pMsgInfo->nRState = CPSS_MSG_RS_STAT_DOING;

		VOS_PrintDebug(__FILE__, __LINE__, "PID:%d pSocket:%p pMsg:%p", pstuPidInfo->pCPuID->stuCPuid.ulPid,
			pstuPidInfo->pSocketInfo, pMsgInfo);

		VOS_PrintMsg("udp distribute msg", pMsgInfo);
		
		VOS_PrintDebug(__FILE__, __LINE__, "UDP dealwith %s", pstuPidInfo->szPidName);

		if (VOS_OK != pstuPidInfo->ppid_init_proc((VOS_VOID *)pMsgInfo))
		{
			VOS_PrintErr(__FILE__,__LINE__,"Msg:%p Socket:%p Stat:%x exec is faild",
				pMsgInfo, pSocket, pMsgInfo->nRState);
		}
		//VOS_PrintMsg("udp distribute output msg", pMsgInfo);
	CONTINUE_GO:
		if (CPSS_MSG_RS_STAT_SENDED == pMsgInfo->nRState || 
			CPSS_MSG_RS_STAT_DOING == pMsgInfo->nRState )
		{
			cpss_move_recv_used_to_free(pMsgInfo);
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
	BZERO(&pSocketLink->pstuPid->pMsgEvent, sizeof(VOS_Event));

	//初始化Event
	sprintf(strMsgEvent, "TCP[%s]", pSocketLink->pstuPid->szPidName);
	ulRet = VOS_Init_Event(&pSocketLink->pstuPid->pMsgEvent, strMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"PID %s Init Event error",
			pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
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
	seraddr.sin_addr.s_addr = get_cpuid_from_ip(pSocketLink->pstuPid->pCPuID->stuCPuid.ulCPuID);//CPSSCPUID pCPuID
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

	/*设置返还选项为假，禁止将发送的数据返还给本地接口*/

	optval = 0;
	if (setsockopt(pSocketLink->nlSocketfd, IPPROTO_IP, IP_MULTICAST_LOOP,
		(VOS_CHAR *)&optval, sizeof(optval)) == SOCKET_ERROR) //如果设置失败
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
	//初始化Event
	sprintf(strMsgEvent, "UDP[%s]", pSocketLink->pstuPid->szPidName);
	
	ulRet = VOS_Init_Event(&pSocketLink->pstuPid->pMsgEvent, strMsgEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"[%s] PID %s Init Event error",
			pSocketLink->pstuPid->szPidName,
			pSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
		closesocket(pSocketLink->nlSocketfd);
		return ulRet;
	}
	
	pSocketLink->nlSocketType = VOS_SOCKET_UDP;	
	pSocketLink->uIStat = CPSS_SKT_STAT_OPENING;
	
	return VOS_OK;
}		/* -----  end of function cpss_socket_udp_open  ----- */

/* ===  FUNCTION  =================================================================
*         Name:  cpss_socket_open_pid
*  Description:  注册启动 pid TCP类的系统
* ===============================================================================*/
static VOS_UINT32 cpss_socket_open_pid(VOS_UINT32 dwType, CPSS_PID_TABLE * pstuPidList)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	VOS_INT32	ulSoRet = VOS_ERR;
	CPSS_PID_THREAD_INFO * pPidListInfoTmp;
	VOS_CHAR	strBuffer[64] = { 0 };
	VOS_CHAR	strBufferKey[64] = { 0 };

	if (NULL == pstuPidList)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Pid Handle is NULL");
		return ulRet;
	}
	for (uIndex = 0; uIndex < pstuPidList->ulPidCount; uIndex++)
	{
		pPidListInfoTmp = pstuPidList->pPidListInfo + uIndex;
		pPidListInfoTmp->pPidTbl = pstuPidList;
		pPidListInfoTmp->nPID_ID = uIndex;
		if (pstuPidList->ulPidCount>1)
		{
			sprintf(strBufferKey, "[%s:%d] Distribute proc", pstuPidList->szPidName, uIndex);
		}
		else
		{
			sprintf(strBufferKey, "[%s] Distribute proc", pstuPidList->szPidName);
		}
		if (VOS_SOCKET_TCP == dwType)
		{
			sprintf(strBuffer, "T%s", strBufferKey);
			ulSoRet = cpss_create_thread(&pPidListInfoTmp->hThread.hThread,
				0,
				cpss_tcp_distribute_proc,
				(VOS_VOID*)pPidListInfoTmp,
				&pPidListInfoTmp->hThread.dwThreadId,
				strBuffer);
		}
		else if (VOS_SOCKET_UDP == dwType)
		{
			sprintf(strBuffer, "U%s", strBufferKey);
			ulSoRet = cpss_create_thread(&pPidListInfoTmp->hThread.hThread,
				0,
				&cpss_udp_distribute_proc,
				(VOS_VOID*)pPidListInfoTmp,
				&pPidListInfoTmp->hThread.dwThreadId,
				strBuffer);
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
	int nIndex = 0;
	VOS_UINT32 dwType = 0;
	VOS_UINT32 ulProcessPid = 0;
	VOS_CHAR   strMutexKey[36] = {0};

	if (VOS_OK != cpss_pid_check())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Reg Socket Init Error");
		return ulRet;
	}

	VOS_PrintInfo(__FILE__, __LINE__, "System CPuID:[%u]",
		CPSSCPUID);
	VOS_PrintInfo(__FILE__, __LINE__, "DBSvr  CPuID:[%u]",
		DBSVRCPUID);
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_ACCEPT].FunProc = cpss_tcp_accept_client_proc;
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_ACCEPT].hThread.hThread = NULL;
	VOS_Strcpy(g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_ACCEPT].strThreadName, "Accept");

	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TSEND].FunProc = cpss_tcp_send_proc;
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TSEND].hThread.hThread = NULL;
	VOS_Strcpy(g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TSEND].strThreadName, "T send");

	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TRECV].FunProc = cpss_tcp_recv_proc;
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TRECV].hThread.hThread = NULL;
	VOS_Strcpy(g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_TRECV].strThreadName, "T recv");

	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].FunProc = cpss_udp_send_proc;
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].hThread.hThread = NULL;
	VOS_Strcpy(g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_USEND].strThreadName, "U send");

	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_URECV].FunProc = cpss_udp_recv_proc;
	g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_URECV].hThread.hThread = NULL;
	VOS_Strcpy(g_handleiocpmanage.hIOThread[CPSS_IOCP_THREAD_URECV].strThreadName, "U recv");

	for (int nIndex = 0; nIndex < CPSS_IOCP_THREAD_COUNT; nIndex++)
	{
		if (NULL != g_handleiocpmanage.hIOThread[nIndex].hThread.hThread)
		{
			VOS_PrintWarn(__FILE__, __LINE__, "%s system is already exist",
				g_handleiocpmanage.hIOThread[nIndex].strThreadName);
			continue;
		}
		sprintf(strMutexKey, "%sMSG%d", g_handleiocpmanage.hIOThread[nIndex].strThreadName, time(NULL));
		ulRet = VOS_Init_Event(&g_handleiocpmanage.hIOThread[nIndex].pMsgEvent, strMutexKey);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "init Tcp Send Event");
			return ulRet;
		}
		sprintf(strMutexKey, "%s proc", g_handleiocpmanage.hIOThread[nIndex].strThreadName);
		ulRet = cpss_create_thread(&g_handleiocpmanage.hIOThread[nIndex].hThread.hThread,
			0,
			g_handleiocpmanage.hIOThread[nIndex].FunProc,
			(VOS_VOID*)&g_handleiocpmanage,
			&g_handleiocpmanage.hIOThread[nIndex].hThread.dwThreadId,
			strMutexKey);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "create thread for %s Rejected!",
				g_handleiocpmanage.hIOThread[nIndex].strThreadName);
			return ulRet;
		}
	}
	pstuPidList = cpss_get_pid_header();
	while (NULL != pstuPidList)
	{
		ulProcessPid = 0;
		if (0 != (VOS_SOCKET_IN & pstuPidList->pCPuID->stuCPuid.ulPid))
		{
			hSocketLinkTemp = cpss_get_socket_from_pid(pstuPidList->szPidName,
				pstuPidList->pCPuID->stuCPuid.ulPid);
			if (NULL == hSocketLinkTemp)
			{
				VOS_PrintErr(__FILE__, __LINE__, "get socket handle error");
				return ulRet;
			}
			dwType = hSocketLinkTemp->nlSocketType & VOS_SOCKET_TYPE >> 16;
			hSocketLinkTemp->pstuPid = pstuPidList;
			switch (dwType)
			{
			case VOS_SOCKET_TCP:
				if (VOS_OK == cpss_socket_tcp_open(hSocketLinkTemp))
				{
					VOS_PrintInfo(__FILE__, __LINE__, "T PID:[%08d:%s] Port:%d",
						pstuPidList->pCPuID->stuCPuid.ulPid,
						pstuPidList->szPidName,
						hSocketLinkTemp->uPort);
				}
				else
				{
					VOS_PrintErr(__FILE__, __LINE__, "CGP T Init Error PID:[%s] Port:%d",
						pstuPidList->szPidName,
						hSocketLinkTemp->uPort);
					pstuPidList = pstuPidList->next;
					continue;
				}
				break;
			case VOS_SOCKET_UDP:
				if (VOS_OK == cpss_socket_udp_open(hSocketLinkTemp))

				{
					VOS_PrintInfo(__FILE__, __LINE__, "U PID:[%08d:%s] Port:%d",
						pstuPidList->pCPuID->stuCPuid.ulPid,
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
					"CGP Socket Init Type Error(%d)", hSocketLinkTemp->nlSocketType);
				pstuPidList = pstuPidList->next;
				continue;
			}
		}
		pstuPidList->pSocketInfo = hSocketLinkTemp;
		if (VOS_OK != cpss_socket_open_pid(dwType,pstuPidList))
		{
			VOS_PrintErr(__FILE__, __LINE__, "CGP UDP Init Error");
		}
		pstuPidList = pstuPidList->next;
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
VOS_UINT32 cpss_subsystem_init (VOS_UINT32 uType, VOS_UINT32 uCmd)
{
	VOS_UINT32 nIndex = 0;
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_SOCKET_LINK * hSocketLink = NULL;
	CPSS_MSG * pMsgInfo = NULL;
	
	if (NULL == g_handleiocpmanage.pUsedSocketHead)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Reg Socket Init Error");
		return ulRet;
	}
	//BZERO(&MsgInfo, sizeof(CPSS_MSG));
	for (nIndex = 0; nIndex < CPSS_IOCP_THREAD_COUNT; nIndex++)
	{
		ulRet = VOS_Set_Event(&g_handleiocpmanage.hIOThread[nIndex].pMsgEvent);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "init Tcp Send Event");
		}
	}
	
	hSocketLink = g_handleiocpmanage.pUsedSocketHead;
	while (NULL != hSocketLink)
	{
		if (CPSS_CMD_SYSTEM_UNIT == uCmd)
		{
			ulRet = VOS_Set_Event(&hSocketLink->pstuPid->pMsgEvent);
			if (VOS_OK != ulRet)
			{
				VOS_PrintErr(__FILE__, __LINE__, "PID:%08X Recv Set Event error",
					hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
			}
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (NULL == hSocketLink->pstuPid)
		{
			VOS_PrintErr(__FILE__,__LINE__,"Socket:%p PID is NULL",hSocketLink);
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (VOS_SOCKET_ACC == hSocketLink->nlSocketType)
		{
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (CPSS_SYSTEM_TYPE_DBSVR == cpss_get_systemid() && CPSSFWPID == hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid)
		{
			VOS_PrintInfo(__FILE__, __LINE__, "This PID[%d] is DBSvr", 
				hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
			hSocketLink = hSocketLink->next;
			continue;
		}
		pMsgInfo = cpss_get_msg_info();
		if (NULL == pMsgInfo)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Socket:%p PID[%d] get msg faild", 
				hSocketLink,
				hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
			hSocketLink = hSocketLink->next;
			continue;
		}
		cps_set_msg_dst_cpuid(pMsgInfo, CPSSCPUID, hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
		cps_set_msg_src_cpuid(pMsgInfo, CPSSFWCPUID, CPSSFWPID);
		pMsgInfo->Body.msghead.uType = uType;
		pMsgInfo->Body.msghead.uCmd = uCmd;
		pMsgInfo->nRState = CPSS_MSG_RS_STAT_RECVED;

		switch (hSocketLink->nlSocketType)
		{
		case VOS_SOCKET_TCP:
			ulRet = cpss_move_tcp_recv_free_to_used(pMsgInfo);
			break;
		case VOS_SOCKET_UDP:
			ulRet = cpss_move_udp_recv_free_to_used(pMsgInfo);
			break;
		default:
			VOS_PrintErr(__FILE__, __LINE__, "socket type is error");
			hSocketLink = hSocketLink->next;
			continue;
		}
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
				hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid);
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
 *  Description:  平台套接口初始化
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
	g_handleiocpmanage.pUsedSocketHead = NULL;
	g_handleiocpmanage.pUsedSocketTail = NULL;
	g_handleiocpmanage.pFreeSocketHead = NULL;
	g_handleiocpmanage.pFreeSocketTail = NULL;
	g_handleiocpmanage.pFreeTCPClientHead = NULL;
	g_handleiocpmanage.pFreeTCPClientTial = NULL;
	g_handleiocpmanage.pUsedTCPClientHead = NULL;
	g_handleiocpmanage.pUsedTCPClientTial = NULL;
	g_handleiocpmanage.ulClientCount  = 0;
	g_handleiocpmanage.ulClientOnline = 0;
	
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
	VOS_UINT32 ulRet = VOS_ERR;
	CPSS_SOCKET_LINK * pSocketLinkTmp = NULL;
	PCPSS_PID_TABLE		pstuPidList = NULL;

	while(NULL != pSocketLinkTmp)
	{
		cpss_close_socket(pSocketLinkTmp);
	}
	for (int nIndex = 0; nIndex < CPSS_IOCP_THREAD_COUNT; nIndex++)
	{
		ulRet = VOS_Destroy_Event(&g_handleiocpmanage.hIOThread[nIndex].pMsgEvent, 0);
		if (VOS_OK != ulRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "init Tcp Send Event");
		}
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
 *  Description:  打印消息队列
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
 *  Description:  打印客户端队列
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
