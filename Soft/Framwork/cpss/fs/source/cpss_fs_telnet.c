/*
 * =====================================================================================
 *
 *       Filename:  cpss_fs_telnet.c
 *
 *    Description:  TELNET ������
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
#include "cpss_fs_telnet.h"
#include "cpss_vk_socket.h"


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_telnet_init
 *  Description:  ��ʼ��telnet������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_telnet_init()
{
	VOS_UINT32 ulRet = VOS_ERR;
	if (VOS_OK != VOS_RegistPidInit(0,CPSS_PID_TELNET, CPSS_STRING_TELNET, 1,telnet_init_proc, telnet_timeout_proc))
	{
		TELNET_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}
//	TELNET_PrintInfo(__FILE__, __LINE__, "Regist Telnet Server OK");

	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_send_data
 *  Description:  ����telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_send_data(pCPSS_MSG pMsgInfo, VOS_VOID * strBuffer, VOS_UINT32 uBufLen,VOS_UINT32 uType)
{
	VOS_UINT32 ulRet = VOS_ERR;// strBuffer  uBufLen
	ulRet = cpss_send_data(pMsgInfo, strBuffer, uBufLen, uType|VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != ulRet)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "send data is NULL");
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_add_cmd
 *  Description:  ����telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID telnet_add_cmd(CPSS_CLIENT_INFO * pClient, VOS_VOID * strBuffer, VOS_UINT32 uBufLen)
{
	if (uBufLen <= 0)
	{
		return ;
	}
	if (pClient->stuCmdLink.nCmdLength > CPSS_MSG_BUFFER_SIZE -10)
	{
		return;
	}
	VOS_Strncpy(pClient->stuCmdLink.strCmdBuff+pClient->stuCmdLink.nCmdLength, strBuffer, uBufLen);
	pClient->stuCmdLink.nCmdLength += uBufLen;
}
static VOS_VOID telnet_del_cmd(CPSS_CLIENT_INFO * pClient, VOS_UINT32 uBufLen)
{
	if (pClient->stuCmdLink.nCmdLength <= 0xFFFFFFFF)
	{
		BZERO(pClient->stuCmdLink.strCmdBuff, pClient->stuCmdLink.nCmdLength);
		pClient->stuCmdLink.nCmdLength = 0;
		return ;
	}
	if (pClient->stuCmdLink.nCmdLength <= 0)
	{
		return ;
	}
	if (pClient->stuCmdLink.nCmdLength <= uBufLen)
	{
		BZERO(pClient->stuCmdLink.strCmdBuff, pClient->stuCmdLink.nCmdLength);
		pClient->stuCmdLink.nCmdLength = 0;
	}
	else
	{
		BZERO(pClient->stuCmdLink.strCmdBuff+(pClient->stuCmdLink.nCmdLength-uBufLen), uBufLen);
		pClient->stuCmdLink.nCmdLength -= uBufLen;
	}
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_hello_send
 *  Description:  ��telnet�ͻ��˷��ͳ�ʼ����Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID telnet_init_send(pCPSS_MSG pMsgInfo)
{
	VOS_CHAR strSendBuff[CPSS_MSG_BUFFER_SIZE] = {0};
	TELNET_ACI stuACI;
	switch(pMsgInfo->Body.msghead.uCmd)
	{
	case CPSS_CMD_SYSTEM_INIT:
		VOS_PrintInfo(__FILE__,__LINE__,"TELNET System Init OK");
		break;
	case CPSS_CMD_SYSTEM_UNIT:
		break;
	case CPSS_CMD_SYSTEM_TELNET:
		stuACI.mACI  = 0xFF;
		stuACI.mACI1 = 0xFD;
		stuACI.mACI2 = 0x25;
		VOS_Memcpy(strSendBuff,&stuACI, sizeof(TELNET_ACI));
		telnet_send_data(pMsgInfo, strSendBuff, 3, VOS_SEND_SKT_TYPE_FINISH);
		break;
	}
/*
	strSendBuff[3] = 0xFF;
	strSendBuff[4] = 0xFB;
	strSendBuff[5] = 0x01;

	strSendBuff[6] = 0xFF;
	strSendBuff[7] = 0xFB;
	strSendBuff[8] = 0x03;
	
	strSendBuff[9] = 0xFF;
	strSendBuff[10] = 0xFD;
	strSendBuff[11] = 0x27;
	
	strSendBuff[12] = 0xFF;
	strSendBuff[13] = 0xFD;
	strSendBuff[14] = 0x1F;
	
	strSendBuff[15] = 0xFF;
	strSendBuff[16] = 0xFD;
	strSendBuff[17] = 0x00;
	
	strSendBuff[18] = 0xFF;
	strSendBuff[19] = 0xFB;
	strSendBuff[20] = 0x00;
*/

//	sprintf(strSendBuff,"Jance CGP Server Runing Ver:%s\r\nUsename:", cpss_getver());
//	telnet_send_data(pMsgInfo, strSendBuff, strlen(strSendBuff));
//	TELNET_PrintInfo(__FILE__, __LINE__, "Printf info ");
	return;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_hello_send
 *  Description:  ��telnet�ͻ��˷��ͳ�ʼ����Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID telnet_hello_send(pCPSS_MSG pMsgInfo)
{
	VOS_CHAR strSendBuff[CPSS_MSG_BUFFER_SIZE] = {0};

	sprintf(strSendBuff,"Jance CGP Server Runing Ver:%s\r\nUsename:", cpss_getver());
	telnet_send_data(pMsgInfo, strSendBuff, strlen(strSendBuff), VOS_SEND_SKT_TYPE_FINISH);
//	TELNET_PrintInfo(__FILE__, __LINE__, "Printf info hello send");
	return;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_hello_send
 *  Description:  ��telnet�ͻ��˷��ͳ�ʼ����Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID telnet_ACI_CMD_proc(pCPSS_MSG pMsgInfo)
{
	VOS_CHAR strSendBuff[CPSS_MSG_BUFFER_SIZE] = {0};
	VOS_CHAR strMsgEvent[125] = {0};
	pCPSS_CLIENT_INFO pClientInfo =NULL;
	
	pClientInfo = pMsgInfo->pClient;
	if (NULL == pClientInfo)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "telnet Client info is NULL");
		return;
	}
TRY_EVENT:
	sprintf(strMsgEvent,"CLIENT_EMENT_%p",pClientInfo);
	if (0 != pClientInfo->hCmdEvent.strmutex[0])
	{
		return;
	}
	if (VOS_OK != VOS_Init_Event(&pClientInfo->hCmdEvent, strMsgEvent))
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss system get init event ");
		goto TRY_EVENT;
	}

	pMsgInfo->Body.msghead.uCmd = pMsgInfo->Body.stuDataBuf[0];
	if (CPSS_TELNET_WILL == pMsgInfo->Body.msghead.uCmd)
	{
		telnet_hello_send(pMsgInfo);
	}
/*	strSendBuff[0] = 0xFF;
	strSendBuff[1] = 0xFD;
	strSendBuff[2] = 0x25;
	telnet_send_data(pMsgInfo, strSendBuff, 3);
*/
//	TELNET_PrintInfo(__FILE__, __LINE__, "Printf info ");
	return;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_CTL_CMD_proc
 *  Description:  ��telnet�ͻ��˷��ͳ�ʼ����Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 telnet_CTL_CMD_proc(pCPSS_MSG pMsgInfo)
{
	VOS_CHAR strSendBuff[CPSS_MSG_BUFFER_SIZE] = {0};
	VOS_CHAR strMsgEvent[125] = {0};
	pCPSS_CLIENT_INFO pClientInfo =NULL;
	return VOS_OK;
	pClientInfo = pMsgInfo->pClient;
	if (NULL == pClientInfo)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "telnet Client info is NULL");
		return VOS_ERR;
	}

	//pMsgInfo->Body.msghead.uCmd = pMsgInfo->Body.stuDataBuf.strBuffer[1];
	pMsgInfo->Body.msghead.uCmd = (VOS_UINT32)pMsgInfo->Body.stuDataBuf;
	if (CPSS_TELNET_WILL == pMsgInfo->Body.msghead.uCmd)
	{
		telnet_hello_send(pMsgInfo);
	}
	return VOS_OK;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_hello_send
 *  Description:  ��telnet�ͻ��˷��ͳ�ʼ����Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID telnet_sytem_proc(pCPSS_MSG pMsgInfo)
{
	CPSS_CLIENT_INFO  * pClient = NULL;
	VOS_CHAR		  * pstuBuffer = NULL;
	VOS_CHAR		  * pBuf = NULL;
	VOS_CHAR			strSendBuff[CPSS_MSG_BUFFER_SIZE] = {0};
	VOS_UINT32			nCmdLeng = 0;
	VOS_UINT32			nIndex = 0;
	VOS_UINT32			nSendLen = 0;
	if (NULL == pMsgInfo)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "telnet system msg is NULL");
		return;
	}
	pClient = pMsgInfo->pClient;
	if (NULL == pClient)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "telnet system Client info is NULL");
		return;
	}
	
	VOS_Remset(pClient->pstuBuffer, pMsgInfo->Body.stuDataBuf);
	pBuf = pClient->pstuBuffer;
	if (NULL == pBuf || 0 == *pBuf)
	{
		TELNET_PrintErr(__FILE__, __LINE__, "telnet recv buffer is NULL");
		return;
	}
	//VOS_PrintInfo(__FILE__,__LINE__,"recv buff[%x:%x:%x:%x:%x]",pBuf[0],pBuf[1],pBuf[2],pBuf[3],pBuf[4]);
	nCmdLeng = VOS_Strlen(pBuf);
	if (0X08 == *pBuf)
	{
		strSendBuff[0] = 0x08;
		strSendBuff[1] = 0x20;
		strSendBuff[2] = 0x08;
		if (0 < nCmdLeng)
		{
			if(VOS_OK == telnet_send_data(pMsgInfo, strSendBuff, VOS_Strlen(strSendBuff), VOS_SEND_SKT_TYPE_FINISH))
			{
				telnet_del_cmd(pClient,1);
			}
		}
		return ;
	}else
	if ((13 == *pBuf && 10 == *(pBuf+1)) ||
		(13 == pBuf[nCmdLeng -2] && 10 == pBuf[nCmdLeng -1]))
	{
		pBuf[nCmdLeng -2] = 0;
		pBuf[nCmdLeng -1] = 0;
		telnet_add_cmd(pClient, pBuf, nCmdLeng - 2);
		shell_user_check(pClient);
	}
	else
	{
		if (CPSS_CLIENT_USERNAME == pClient->nLineStat)
		{
			if(VOS_OK == telnet_send_data(pMsgInfo, "*", VOS_Strlen("*"),VOS_SEND_SKT_TYPE_FINISH))
			{
				telnet_add_cmd(pClient,pBuf, nCmdLeng);
			}
			return;
		}
		if(VOS_OK == telnet_send_data(pMsgInfo, pBuf, VOS_Strlen(pBuf),VOS_SEND_SKT_TYPE_FINISH))
		{
			telnet_add_cmd(pClient,pBuf, nCmdLeng);
		}
		return;
	}
	telnet_send_data(pMsgInfo, "\r\n", VOS_Strlen("\r\n"),VOS_SEND_SKT_TYPE_INSERT);
	nSendLen = pClient->nBufferLeng;
	pstuBuffer = pClient->pstuBuffer;

	while(NULL != pstuBuffer || pClient->nCmdConut > 0)
	{
		VOS_PrintDebug(__FILE__, __LINE__, "New Buffer[%p]",pstuBuffer);
		if (CPSS_CLIENT_CMD_DOING == pClient->bIsEvent)
		{
			VOS_Wait_Event(&pClient->hCmdEvent, 300);
			if (NULL != pstuBuffer && VOS_Memsize(pstuBuffer, 0) <= 0)
			{
				VOS_PrintDebug(__FILE__, __LINE__, "Print Cent[%p] Size:%d",pstuBuffer,
					VOS_Memsize(pstuBuffer, 0));
				continue;
			}
		}
		if (NULL != pstuBuffer)
		{
			if (0 >= VOS_Memsize(pstuBuffer, 0))//1 == ClientInfo.bIsWait && 
			{
				VOS_Wait_Event(&pClient->hCmdEvent, 300);
				VOS_PrintDebug(__FILE__, __LINE__, "Wait Leng[%p]->next[%p]",pstuBuffer, pClient->pstuBuffer);
				continue;
			}
			VOS_PrintDebug(__FILE__, __LINE__, "Send Data[%p] size[%d]", pstuBuffer, VOS_Memsize(pstuBuffer, 0));
			telnet_send_data(pMsgInfo, pstuBuffer, VOS_Memsize(pstuBuffer, 0), VOS_SEND_SKT_TYPE_FINISH);
			pClient->nCmdConut--;
			//VOS_PrintInfo(__FILE__, __LINE__, "[%d]Print Leng:%d",pClient->nCmdConut,pstuBuffer->nSize);
		}
		/*
		if (CPSS_CLIENT_CMD_DOED == pClient->bIsEvent)
		{
			if (NULL == pstuBuffer)
			{
				break;
			}
			if (NULL == pstuBuffer->next)
			{
				pstuBuffer = (CPSS_MEM_BUFFER *)cpss_get_next_buffer(&pClient->pstuBuffer);
				continue;
			}
		}
		if (pstuBuffer != &pClient->pstuBuffer)
		{
			pstuBuffer = (CPSS_MEM_BUFFER *)cpss_get_next_buffer(&pClient->pstuBuffer);
		}
		else
		{
			pstuBuffer = pClient->pstuBuffer.next;
		}
		*/
	}
	telnet_del_cmd(pClient,-1);
	return;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_init_proc
 *  Description:  ��ʼ��telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_init_proc(VOS_VOID *parg)
{
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)parg;
	VOS_UINT8 nCheck = 0;
	switch (pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_SYSTEM_INIT:
		telnet_init_send(pMsgInfo);
		break;
	case CPSS_CMD_SYSTEM_UNIT:
		break;
	case CPSS_TYPE_SYSTEM_TELNET:
		nCheck = pMsgInfo->Body.stuDataBuf[0];
		switch (nCheck)
		{
		case CPSS_TELNET_IAC:
			telnet_ACI_CMD_proc(pMsgInfo);
			return VOS_OK;
		case CPSS_TELNET_CTL:
			return telnet_CTL_CMD_proc(pMsgInfo);
		}
		if (0 ==  nCheck)
		{
			TELNET_PrintInfo(__FILE__, __LINE__, "=========[Client:%p,RecvLeng:%d,Msg:%p Next:%pPrev:%p]==\n",
				pMsgInfo->pClient, pMsgInfo->Body.msghead.ulMsgLength,
				pMsgInfo,pMsgInfo->next,pMsgInfo->prev);
		}
		else
		{
			telnet_sytem_proc(pMsgInfo);
		}
		break;
	default:
		TELNET_PrintErr(__FILE__, __LINE__, "Type:%08x,Cmd:%08x", 
			pMsgInfo->Body.msghead.uType,
			pMsgInfo->Body.msghead.uCmd);
		break;
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_init_proc
 *  Description:  ��ʼ��telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_timeout_proc(VOS_VOID *pargc,VOS_UINT32 pargv)
{
	return VOS_OK;
}
