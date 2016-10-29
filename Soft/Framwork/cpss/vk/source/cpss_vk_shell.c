/* =========================================================================
*
*       Filename:  cpss_vk_shell.c
*
*    Description:  命令函数接口文件
*
*        Version:  1.0
*        Created:  2011年11月15日 00时04分58秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Jance (), shenghs2004@163.com
*        Company:  Jance网络工作室
*
* ========================================================================*/
#include "cpss_public.h"
#include "cpss_vk_thread.h"
#include "cpss_vk_shell.h"
#include "cpss_vk_socket.h"
#include "cpss_vos_mem.h"

#define VOS_Shl_Malloc(ulSize)			VOS_Malloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))
#define VOS_Shl_Realloc(pstrads,ulSize)	VOS_Realloc((pstrads), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))
#define VOS_Shl_Remset(pstrads)			VOS_Remset((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))
#define VOS_Shl_Memcat(pstrA,pstrB)		VOS_Memcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))
#define VOS_Shl_Memsize(pstrads)		VOS_Memsize((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))
#define VOS_Shl_Free(pstrads)			VOS_Free((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))

#define VOS_Shl_Strcat(pstrA,pstrB)		VOS_CpsStrcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_SHELL))

static VOS_THREAD_INFO		g_cmd_thread_info;
VOS_Event			g_cmd_Event;
static CPSS_CLIENT_INFO	ClientInfo;
/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_init
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
static VOS_STRING shell_cmd_cmp(VOS_STRING key,VOS_STRING pstrCmd)
{
	VOS_STRING pstrRet = NULL;
	VOS_UINT32 ulLeng = 0;
	VOS_UINT32 uIndex = 0;
	while(0 != *(pstrCmd+uIndex))
	{
		if (' ' == *(pstrCmd+uIndex))
		{
			break;
		}
		if ('\n' == *(pstrCmd+uIndex))
		{
			break;
		}
		if ('\r' == *(pstrCmd+uIndex))
		{
			break;
		}
		if (*(key+uIndex) == *(pstrCmd+uIndex))
		{
			uIndex++;
			continue;
		}
		break;
	}
	if ((VOS_UINT32)VOS_Strlen(key) != uIndex)
	{
		return pstrRet;
	}
	pstrRet = pstrCmd+uIndex;
	while(' ' == *pstrRet)
	{
		pstrRet ++;
	}
	return pstrRet;
}

/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_init
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
static VOS_UINT32 shell_sub_one_cmd_check(CPSS_CLIENT_INFO * pClient, VOS_STRING pstrCmd)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_STRING pstrTemp = NULL;
	pstrTemp = shell_cmd_cmp("msgtab",pstrCmd);
	if(NULL != pstrTemp)
	{
		ulRet = VOS_OK;
		cpss_get_msg_tab_info((VOS_VOID*)pClient, CPSS_MSG_SEND_TCP|CPSS_MSG_RECV_TCP);
		return ulRet;
	}
	
	pstrTemp = shell_cmd_cmp("logtab",pstrCmd);
	if(NULL != pstrTemp)
	{
		ulRet = VOS_OK;
		cpss_get_msg_tab_info((VOS_VOID*)pClient, CPSS_MSG_SEND_TCP|CPSS_MSG_RECV_TCP);
		return ulRet;
	}

	pstrTemp = shell_cmd_cmp("clitab",pstrCmd);
	if (NULL != pstrTemp)
	{
		ulRet = VOS_OK;
		cpss_get_cli_tab_info((VOS_VOID*)pClient, CPSS_MSG_RECV_TCP);
		return ulRet;
	}
	pstrTemp = shell_cmd_cmp("help",pstrCmd);
	if(NULL != pstrTemp)
	{
		VOS_PrintBuffer(&pClient->pstuBuffer, "->clitab :set info");
		VOS_PrintBuffer(&pClient->pstuBuffer, "\r\n->cpss_get :get info");
		VOS_PrintBuffer(&pClient->pstuBuffer, "\r\n->cpss_exit :exit system\n");
		pClient->nBufferLeng = VOS_Strlen(pClient->pstuBuffer);
		ulRet = VOS_OK;
		return ulRet;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_init
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
static VOS_UINT32 shell_cmd_check(CPSS_CLIENT_INFO * pClient, VOS_STRING pstrCmd)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_STRING pstrTemp = NULL;
	if (0 == *pstrCmd)
	{
		return ulRet;
	}
	pstrTemp = shell_cmd_cmp("cpss_set",pstrCmd);
	if(NULL != pstrTemp)
	{
		ulRet = VOS_OK;
		return ulRet;
	}
	pstrTemp = shell_cmd_cmp("cpss_get",pstrCmd);
	if(NULL != pstrTemp)
	{
		ulRet = shell_sub_one_cmd_check(pClient, pstrTemp);
		if (VOS_OK != ulRet)
		{
			VOS_PrintBuffer(&pClient->pstuBuffer, "\r\n->%s error input help\r\n", pstrTemp);
			pClient->nBufferLeng = VOS_Strlen(pClient->pstuBuffer);
		}
		ulRet = VOS_OK;
		return ulRet;
	}
	pstrTemp = shell_cmd_cmp("cpss_exit",pstrCmd);
	if(NULL != pstrTemp)
	{
		ulRet = VOS_OK;
		return ulRet;
	}
	pstrTemp = shell_cmd_cmp("cpss_help",pstrCmd);
	if(NULL != pstrTemp)
	{
		VOS_PrintBuffer(&pClient->pstuBuffer, "->cpss_set :set info");
		VOS_PrintBuffer(&pClient->pstuBuffer, "\r\n->cpss_get :get info");
		VOS_PrintBuffer(&pClient->pstuBuffer, "\r\n->cpss_exit :exit system\r\n");
		pClient->nBufferLeng = VOS_Strlen(pClient->pstuBuffer);
		ulRet = VOS_OK;
		return ulRet;
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_check_use_info
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_check_use_info(pCPSS_USER_INFO pstuUserInfo)
{
	VOS_UINT32	ulRet = VOS_ERR;
	CPSS_MSG	MsgInfo;
	VOS_CHAR	strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	VOS_UINT32	ulSendSize = 0;

	if (NULL == pstuUserInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input use or pstrpass is null");
		return ulRet;
	}
	BZERO(&MsgInfo,sizeof(CPSS_MSG));
	MsgInfo.Body.msghead.stSrcProc.ulCpuID = CPSSCPUID;
	MsgInfo.Body.msghead.stDstProc.ulCpuID = DBSVRCPUID;
	MsgInfo.Body.msghead.stDstProc.ulPID   = DBSVRPID;
	MsgInfo.Body.msghead.stSrcProc.ulPID = CPSSFWPID;

	MsgInfo.pClient = cpss_get_sktinfo_with_pid(MsgInfo.Body.msghead.stSrcProc.ulPID,
		CPSS_GET_SKT_LINK_SOCKET);

	//MsgInfo.Body.msghead.uType	   = CPSS_REQ_DBSVR_USE;
	//MsgInfo.Body.msghead.uSubType  = CPSS_TYPE_CHECK_USE;

	//VOS_PrintBuffer(&strDataBuf,"%p<=>%s<=>%s",pstrVoidCli,pstrUse,pstrPass);
	ulSendSize = sizeof(CPSS_USER_INFO);
	VOS_Memcpy(strBuffer,pstuUserInfo,ulSendSize);

	ulRet = cpss_send_data(&MsgInfo, strBuffer, ulSendSize,
		VOS_SEND_SKT_TYPE_UDP);

	if (VOS_OK != ulRet)
	{
		/*VOS_PrintErr(__FILE__,__LINE__,"send data is error %d:%s",
			MsgInfo.Body.strDataBuf.nSize,
			MsgInfo.Body.strDataBuf.strBuffer);
		*/
	}
	return ulRet;
}		/* -----  end of function cpss_subsystem_init  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_result_use_info
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_result_use_info(pCPSS_USER_INFO pstuUserInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pCPSS_CLIENT_INFO pClient = (pCPSS_CLIENT_INFO)pstuUserInfo->pstrVoidCli;
	if (NULL == pClient)
	{
		return ulRet;
	}
	pClient->stuUserInfo.ulResult = pstuUserInfo->ulResult;
	VOS_Set_Event(pClient->pCmdEvent);
	ulRet = VOS_OK;
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_hello_send
 *  Description:  给telnet客户端发送初始化信息
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 telnet_check_user(pCPSS_CLIENT_INFO pClient)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pClient->stuUserInfo.ulResult = VOS_ERR;
	pClient->stuUserInfo.pstrVoidCli = pClient;
	ulRet = cpss_check_use_info(&pClient->stuUserInfo);
	if (VOS_OK != ulRet)
	{
		VOS_PrintInfo(__FILE__,__LINE__,"send use :%s and Password:%s error",
			pClient->stuUserInfo.strUser,
		pClient->stuUserInfo.strUser);
	}
	VOS_Wait_Event(pClient->pCmdEvent, INFINITE);
	ulRet = VOS_ERR;
	if (VOS_OK == pClient->stuUserInfo.ulResult)
	{
		VOS_PrintBuffer(&pClient->pstuBuffer,"Username:%s online\r\n",
			pClient->stuUserInfo.strUser);
		ulRet = VOS_OK;
	}
/*
	if (0 == VOS_Strcmp(pClient->stuUserInfo.strUser, "jance") &&
		0 == VOS_Strcmp(pClient->stuUserInfo.strPass, "duxiezi"))
	{
		VOS_PrintBuffer(&pClient->pstuBuffer,"Username:%s online\r\n",
			pClient->stuUserInfo.strUser);
		ulRet = VOS_OK;
	}*/
//	TELNET_PrintErr(__FILE__,__LINE__,"user or password is error");
	return ulRet;
}

/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_init
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
VOS_UINT32 shell_cmd_init ()
{
	return VOS_OK;
}		/* -----  end of function shell_cmd_init  ----- */

/*===  FUNCTION  ===============================================================
-         Name:  print_user_info
-  Description:	输出用户信息  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
static VOS_VOID print_user_info (VOS_VOID * pVoidClient)
{
	CPSS_CLIENT_INFO * pClient = (CPSS_CLIENT_INFO*)pVoidClient;
	VOS_PrintBuffer(&pClient->pstuBuffer,"%s #",
		pClient->stuUserInfo.strUser);
}
/*===  FUNCTION  ===============================================================
-         Name:  shell_user_check
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
VOS_UINT32 shell_user_check (VOS_VOID * pVoidClient)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 uCount = 0;

	//pCPSS_MEM_BUFFER pstuBuffer = NULL;
	//pCPSS_MEM_BUFFER pstuBufTmp = NULL;
	VOS_CHAR *pstrBuff = NULL;
	VOS_CHAR cmdbuf[32] = {0};
	VOS_UINT32 ulCmdLen = 0;
	CPSS_CLIENT_INFO * pClient = (CPSS_CLIENT_INFO*)pVoidClient;

	pClient->bIsEvent = CPSS_CLIENT_CMD_DO;
	/*
	if (pClient->nCmdConut < 1)
	{
		pClient->nCmdConut = 1;
		pClient->nBufferLeng = 0;
	}
	*/
	pstrBuff = pClient->stuCmdLink.strCmdBuff;

	if (CPSS_CLIENT_OFFLINE == pClient->nLineStat)
	{
		VOS_Strcpy(pClient->stuUserInfo.strUser,pstrBuff);
		if (0 == pClient->stuUserInfo.strUser[0])
		{
			pClient->nLineStat = CPSS_CLIENT_OFFLINE;
			VOS_PrintBuffer(&pClient->pstuBuffer,"%s", "Usename:");
		}
		else
		{
			pClient->nLineStat = CPSS_CLIENT_USERNAME;
			VOS_PrintBuffer(&pClient->pstuBuffer,"%s", "Passwd:");
		}
		pClient->nBufferLeng = VOS_Strlen(pClient->pstuBuffer);
		ulRet = CPSS_CLIENT_OFFLINE;
	}
	else if (CPSS_CLIENT_USERNAME == pClient->nLineStat)
	{
		VOS_Strcpy(pClient->stuUserInfo.strPass, pstrBuff);
		
		if (VOS_OK != telnet_check_user(pClient))
		{
			pClient->nLineStat = CPSS_CLIENT_OFFLINE;
			VOS_PrintBuffer(pClient->pstuBuffer, "User and passwd not correct please check and try again!\r\nUsename:");
		}
		else
		{
			print_user_info(pClient);
			pClient->nLineStat = CPSS_CLIENT_ONLINE;
		}
		pClient->nBufferLeng = VOS_Strlen(pClient->pstuBuffer);
		ulRet = pClient->nLineStat;//pClient->nLineStat;
	}
	else if (CPSS_CLIENT_ONLINE == pClient->nLineStat)
	{
		VOS_PrintDebug(__FILE__, __LINE__, "exec [%s] CLI[%d] stat:[%d]",
		pClient->stuCmdLink.strCmdBuff,pClient->ulID,pClient->nStat);
	//	pClient->bIsEvent = CPSS_CLIENT_CMD_DOING;
	//	pClient->bIsWait = 1;
		if (VOS_OK != shell_cmd_check(pClient, pstrBuff) && 0 != *pstrBuff)
		{
			cpss_exec_cmd_proc(pClient, pstrBuff);
		}
		else
		{
			print_user_info(pClient);
			pClient->bIsEvent = CPSS_CLIENT_CMD_DOED;
		}
		ulRet = CPSS_CLIENT_ONLINE;
	}
	
	if (CPSS_CLIENT_ONLINE == pClient->nLineStat)
	{
		if (0 != pClient->stuCmdLink.nCmdLength)
		{
			VOS_PrintInfo("", CPSS_NO_PRINTF, "[%d] User:%s-->Command:%s", 
				pClient->ulID, pClient->stuUserInfo.strUser, 
				pClient->stuCmdLink.strCmdBuff);
		}
	}

	/*VOS_PrintDebug(__FILE__, __LINE__, "[%p] CMD len [%d] stat is [%d]",pClient,
		pClient->nBufferLeng,pClient->nStat);*/
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_exec_cmd_timeout_proc
 *  Description:  处理TCP 内容超时处理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_exec_cmd_timeout_proc(VOS_VOID *arg, VOS_UINT32 *len)
{
	VOS_UINT32 ulRet  = VOS_ERR;
	VOS_PrintErr(__FILE__, __LINE__, "cpss cpss_exec_cmd_timeout_proc wait event");
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_exec_cmd
 *  Description:  执行命令
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_exec_cmd (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_exec_cmd (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32			uRet = VOS_ERR;
	SHELL_CMD_MGR	 *	pShellCmd;
	CPSS_CLIENT_INFO *	pClient = NULL;//(CPSS_CLIENT_INFO *)pVoidClient;
	VOS_CHAR		 *	pstuBufTmp =NULL;
	SHELL_CMD_TIMER  *  pstuCmdTimer = NULL;
	VOS_VOID		 *  pTimer = NULL;
	FILE			 *  fp = NULL;
	VOS_CHAR			cmdbuf[CPSS_CMD_BUFFER_SIZE] = {0};
	VOS_UINT32			ulCmdLen = 0;

	pShellCmd = (SHELL_CMD_MGR*)lpParameter;
	if (NULL == pShellCmd)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss_exec_cmd input parameter is NULL");
		goto END_PROC;
	}

	pstuCmdTimer = (SHELL_CMD_TIMER*)VOS_Shl_Malloc(sizeof(SHELL_CMD_TIMER));
	if (NULL == pstuCmdTimer)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss_exec_cmd get new address");
		goto END_PROC;
	}

	pClient = (CPSS_CLIENT_INFO*)pShellCmd->pVoidClient;
	if (NULL == pClient)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss_exec_cmd input client info is NULL");
		goto FREE_END;
	}


	sprintf(cmdbuf, "%s 2>&1", pShellCmd->pstrCommand);
	
	pShellCmd->bIsEvent = CPSS_CLIENT_CMD_DOING;
	pstuCmdTimer->ThreadID = pShellCmd->ThreadID;

	uRet = VOS_Set_Event(&pShellCmd->hCmdEvent);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system wait event");
		goto FREE_END;
	}
	
	pTimer = (VOS_VOID *)cpss_set_timer(300, cpss_exec_cmd_timeout_proc, 
		pstuCmdTimer, sizeof(SHELL_CMD_MGR));

	if ((fp = _popen(cmdbuf, "rb")) == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "open system call failed(errno=%d)", GetLastError());
		goto FREE_END;
	}
	cpss_kill_timer(pTimer);
	pstuBufTmp = pClient->pstuBuffer;
#if 0
	while ((pClient->nBufferLeng = fread(pstuBufTmp->strBuffer, sizeof(VOS_CHAR), CPSS_MSG_BUFFER_SIZE - 4, fp)) == CPSS_MSG_BUFFER_SIZE - 4)
	{
		pstuBufTmp->nSize = pClient->nBufferLeng;
/*		VOS_PrintInfo(__FILE__, __LINE__, "[%d]Read Lend:%d",
			pClient->nCmdConut,
			pstuBufTmp->nSize);*/
		if (NULL == pstuBufTmp)
		{
			break;
		}
		pstuBufTmp = cpss_get_mem_buffer(pstuBufTmp, 1);
		pClient->nCmdConut ++;
	}
	pstuBufTmp->nSize = pClient->nBufferLeng;
	VOS_PrintDebug(__FILE__, __LINE__, "[%d]Read Lend:%d",
		pClient->nCmdConut,
		pstuBufTmp->nSize);

	print_user_info(pClient);
#endif
	pClient->bIsEvent = CPSS_CLIENT_CMD_DOED;
	VOS_Set_Event(pClient->pCmdEvent);

FREE_END:
	if (NULL != pstuCmdTimer)
	{
		VOS_Shl_Free(pstuCmdTimer);
		pstuCmdTimer = NULL;
	}
END_PROC:
	if (0 < fp)
	{
		uRet = _pclose(fp);
		if (uRet < 0)
		{
			VOS_PrintErr(__FILE__, __LINE__, "pclose system call failed(cmdret=%d)", uRet);
			uRet = VOS_ERR;
		}
		else
		{
			uRet = VOS_OK;
		}
	}
	return uRet;
}

/*===  FUNCTION  ===============================================================
-         Name:  cpss_exec_cmd_proc
-  Description:	  执行命令  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
VOS_UINT32 cpss_exec_cmd_proc(VOS_VOID * pVoidClient, VOS_CHAR * command)
{
	VOS_UINT32	ret    = 0;
	VOS_UINT32	bufLen = 0;
	VOS_UINT32  len    = 0;
	VOS_CHAR strMsgEvent[125] = {0};
	SHELL_CMD_MGR * g_pShellCmd;

	g_pShellCmd = (SHELL_CMD_MGR*)VOS_Shl_Malloc(sizeof(SHELL_CMD_MGR));
	if (NULL == g_pShellCmd)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system get malloc error ");
		return ret;
	}
	sprintf(strMsgEvent,"CMD_EMENT_%p",g_pShellCmd);
	
	if (VOS_OK != VOS_Init_Event(&g_pShellCmd->hCmdEvent, strMsgEvent))
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system get init event ");
		goto FREE_END;
	}

	g_pShellCmd->pstrCommand = command;
	g_pShellCmd->pVoidClient = pVoidClient;
	g_pShellCmd->bIsEvent = CPSS_CLIENT_CMD_DO;
	
	ret = cpss_create_thread(&g_pShellCmd->hExec,
					0, 
					cpss_exec_cmd, 
					(VOS_VOID*)g_pShellCmd,
					&g_pShellCmd->ThreadID,
					"");
	if (VOS_OK != ret)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system get malloc error ");
		goto FREE_END;
	}

	while(CPSS_CLIENT_CMD_DO == g_pShellCmd->bIsEvent)
	{
		ret = VOS_Wait_Event(&g_pShellCmd->hCmdEvent,1800);
		if (VOS_OK != ret)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss system wait event is error");
			goto FREE_END;
		}
	}

FREE_END:
	if (g_pShellCmd != NULL)
	{
		VOS_Destroy_Event(&g_pShellCmd->hCmdEvent,0);
		VOS_Free(g_pShellCmd,sizeof(SHELL_CMD_MGR));
		g_pShellCmd = NULL;
	}
	return ret;
}

/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_main
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
VOS_VOID shell_print_cmd (VOS_CHAR* pstuBuffer)
{
	//VOS_PrintDebug(__FILE__, __LINE__, "Print buffer [%s]", pstuBuffer);
}

/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_exec_proc
-  Description:	命令函数初始化
-  Input      :
-  OutPut     :
-  Return     :
- =============================================================================*/
#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  shell_cmd_exec_proc(VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 shell_cmd_exec_proc(VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32			uStat = 0;
	VOS_UINT32			uIsFirst = 8;
	VOS_CHAR			strMsgEvent[125] = { 0 };

	BZERO(&ClientInfo, sizeof(CPSS_CLIENT_INFO));
	ClientInfo.nLineStat = CPSS_CLIENT_OFFLINE;
	//ClientInfo.nCmdConut = 1;
	ClientInfo.pCmdEvent = VOS_Shl_Malloc(sizeof(VOS_Event));
	if (ClientInfo.pCmdEvent == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"get Event size is null");
		return VOS_ERR;
	}
	ClientInfo.nBufferLeng = 0;
TRY_EVENT:
	sprintf(strMsgEvent, "CLIENT_EMENT_SHELL_%p", &ClientInfo);

	if (VOS_OK != VOS_Init_Event(ClientInfo.pCmdEvent, strMsgEvent))
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system get init event ");
		goto TRY_EVENT;
	}
	while (1)
	{
		uStat = shell_user_check(&ClientInfo);
		if (VOS_OK == cpss_check_thread_is_exit(0))
		{
			break;
		}
		if (CPSS_CLIENT_ONLINE == uStat	&&
			CPSS_CLIENT_CMD_DOING == ClientInfo.bIsEvent)
		{
			VOS_Wait_Event(ClientInfo.pCmdEvent, INFINITE);
			if (NULL != ClientInfo.pstuBuffer)
			{
				VOS_PrintDebug(__FILE__, __LINE__, "Print Wait clint buffer address [%p]",
					ClientInfo.pstuBuffer);
				continue;
			}
		}
		shell_print_cmd(ClientInfo.pstuBuffer);
		if (NULL != ClientInfo.pstuBuffer)
		{
			if (0 >= VOS_Strlen(ClientInfo.pstuBuffer))
			{
				VOS_Wait_Event(ClientInfo.pCmdEvent, INFINITE);
				VOS_PrintDebug(__FILE__, __LINE__, "Print Wait [%p] size[%d]",
					ClientInfo.pstuBuffer, VOS_Strlen(ClientInfo.pstuBuffer));
				continue;
			}
			if (CPSS_CLIENT_OFFLINE == uStat)
			{
				if (8 == uIsFirst)
				{
					VOS_PrintInfo("", CPSS_PRINTF_BUFFER, ClientInfo.pstuBuffer);
					uIsFirst = 0;
				}
				else
				{
					VOS_PrintInfo("", CPSS_PRINTF_BUFCTL, ClientInfo.pstuBuffer);
				}
			}
			else
			{
				VOS_PrintInfo("", CPSS_PRINTF_BUFCTL, ClientInfo.pstuBuffer);
			}
		}
		ClientInfo.pstuBuffer[0] = 0;
		ClientInfo.nBufferLeng = 0;
		BZERO(ClientInfo.stuCmdLink.strCmdBuff, 1024);
		gets(ClientInfo.stuCmdLink.strCmdBuff);
		ClientInfo.stuCmdLink.nCmdLength = VOS_Strlen(ClientInfo.stuCmdLink.strCmdBuff);
	}
	return VOS_OK;
}		/* -----  end of function shell_cmd_init  ----- */

/*===  FUNCTION  ===============================================================
-         Name:  cpss_shell_cmd_main
-  Description:	命令函数初始化  
-  Input      :	
-  OutPut     :	
-  Return     :    
- =============================================================================*/
VOS_UINT32 cpss_shell_cmd_main()
{
	VOS_UINT32 ulRet = VOS_OK;

	ulRet = VOS_Init_Event(&g_cmd_Event,"G_CMD_EVENT");
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "");
		return ulRet;
	}
	ulRet = cpss_create_thread(&g_cmd_thread_info.hThread,
		0,
		shell_cmd_exec_proc,
		&g_cmd_thread_info,
		&g_cmd_thread_info.dwThreadId,
		"");
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Rejected!");
		return ulRet;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
-         Name:  cpss_shell_cmd_wait
-  Description:	等待cmdshell处理结束
-  Input      :
-  OutPut     :
-  Return     :
- =============================================================================*/
VOS_UINT32 cpss_shell_cmd_wait()
{
	VOS_UINT32 ulRet = VOS_ERR;
	ulRet = VOS_Wait_Event(&g_cmd_Event, INFINITE);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "wait cmd shell faild");
		return ulRet;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
-         Name:  cpss_shell_cmd_set
-  Description:	等待cmdshell处理结束
-  Input      :
-  OutPut     :
-  Return     :
- =============================================================================*/
VOS_UINT32 cpss_shell_cmd_set()
{
	VOS_UINT32 ulRet = VOS_ERR;
	ulRet = VOS_Set_Event(&g_cmd_Event);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "wait cmd shell faild");
		return ulRet;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
-         Name:  cpss_shell_cmd_destory
-  Description:	释放cmdshell处理结束
-  Input      :
-  OutPut     :
-  Return     :
- =============================================================================*/
VOS_UINT32 cpss_shell_cmd_destory()
{
	VOS_UINT32 ulRet = VOS_ERR;
	if (VOS_OK != VOS_Destroy_Event(&g_cmd_Event, 0))
	{
		VOS_PrintErr(__FILE__, __LINE__, "wait cmd shell faild");
		return VOS_ERR;
	}
	return ulRet;
}
/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_exit
-  Description:	退出shell
-  Input      :
-  OutPut     :
-  Return     :
- =============================================================================*/
VOS_UINT32 cpss_shell_cmd_exit()
{
	if (NULL == g_cmd_thread_info.hThread)
	{
		return VOS_OK;
	}
	cpss_close_thread(g_cmd_thread_info.hThread, &g_cmd_thread_info.dwThreadId);

	VOS_Destroy_Event(ClientInfo.pCmdEvent, 0);
	g_cmd_thread_info.hThread = NULL;
	cpss_shell_cmd_set();
	return VOS_OK;
}
