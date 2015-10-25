/*
 * =====================================================================================
 *
 *       Filename:  money_common.c
 *
 *    Description:  xcap����
 *
 *        Version:  1.0
 *        Created:  2012��07��28�� 18ʱ41��32��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 * =====================================================================================
 */
#include "money_common.h"

/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void Money_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MONEY, CPSS_PRINT_INFO, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintErr
 *  Description:  
 * ========================================================================*/
void Money_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MONEY, CPSS_PRINT_ERR,
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintWarn
 *  Description:  
 * ========================================================================*/
void Money_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_MONEY, CPSS_PRINT_WARN, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  money_send_data
 *  Description:  ����telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 send_udp_data(VOS_VOID *pVoidMsg,VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen,VOS_UINT8 Type,
								 VOS_UINT8 uType, VOS_UINT8 uSubType)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)pVoidMsg;
	pMsgInfo->Body.msghead.uType = uType;
	pMsgInfo->Body.msghead.uSubType = uSubType;

	uRet = cpss_send_data(pVoidMsg, pstuBuffer, uBufLen,
		Type | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		Money_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  money_send_data
 *  Description:  ����telnet������
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 send_tcp_data(VOS_VOID *pVoidMsg,VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen,
								 VOS_UINT8 uType, VOS_UINT8 uSubType)
{
	VOS_UINT32 uRet = VOS_ERR;
	uRet = cpss_send_data(pVoidMsg,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		Money_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root
 *  Description:  ����xcap �ַ���
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 get_xcap_root(VOS_CHAR * pstrInput)
{
	VOS_UINT32 uIndex = 0;
	VOS_UINT32 uCount = sizeof(g_rootInfo)/sizeof(XCAP_ROOT_INFO);
	VOS_CHAR * pstrRoot = NULL;
	for (uIndex = 0; uIndex < uCount; uIndex ++)
	{
		//pstrRoot = VOS_Strstr(pstrInput, g_rootInfo[uIndex].RootUrlName);
		//if (NULL != pstrRoot)
		if (0 == VOS_Strcmp(pstrInput, g_rootInfo[uIndex].RootUrlName))
		{
			return g_rootInfo[uIndex].m_no;
		}
	}
	return g_rootInfo[0].m_no;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  money_proc_init
 *  Description:  ����telnet������
 * ==========================================================================*/
VOS_UINT32 money_proc_init(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)pVoidMsg;
	switch(pMsgInfo->Body.msghead.uCmd)
	{
	case CPSS_CMD_SYSTEM_INIT:
		uRet = VOS_OK;
		break;
	case CPSS_CMD_SYSTEM_UNIT:
		uRet = VOS_OK;
		break;
	default:
		Money_PrintErr(__FILE__,__LINE__,"uCmd %X",pMsgInfo->Body.msghead.uCmd);
		uRet=VOS_ERR;
		break;
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  proc_xcap_url_result
 *  Description:  ����telnet������
 * ==========================================================================*/
VOS_UINT32 proc_xcap_url_result(VOS_VOID *pVoidMsg)
{
	VOS_UINT32		uRet = VOS_ERR;
	VOS_CHAR		strUrlName[XCAP_HOST_LENGTH] = {0};
	pXCAP_SER_MGR    stuXcapSerMgr = {0};
	VOS_UINT8		uNum = -1;
	CPSS_MSG		MsgInfo = {0};
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)pVoidMsg;
	
	if (NULL == pMsgInfo)
	{
		Money_PrintErr(__FILE__,__LINE__,"input msg info is NULL");
		goto END_PROC;
	}
	stuXcapSerMgr =(pXCAP_SER_MGR)pMsgInfo->Body.stuDataBuf.strBuffer;
	if (0x0A != stuXcapSerMgr->uStat)
	{
		Money_PrintErr(__FILE__,__LINE__,"Recv buffer is Error");
		goto END_PROC;
	}
	Money_PrintInfo(__FILE__,__LINE__,"GET URL %p:%s",
		stuXcapSerMgr->Req_Mgr, 
		&stuXcapSerMgr->URL);
	VOS_Memcpy(&MsgInfo.Body.stuDataBuf.strBuffer, stuXcapSerMgr, sizeof(XCAP_SER_MGR));
	MsgInfo.Body.stuDataBuf.nSize = sizeof(XCAP_SER_MGR);
	uNum = get_xcap_root(&stuXcapSerMgr->URL);
	if (uNum >=  0 && uNum < sizeof(g_rootInfo)/sizeof(XCAP_ROOT_INFO))
	{
		if (NULL == g_rootInfo[uNum].dealFun)
		{
			Money_PrintErr(__FILE__,__LINE__,"dealFun fun is NULL num:%d",uNum);
			uRet = VOS_OK;
			goto END_PROC;
		}
		uRet = g_rootInfo[uNum].dealFun(&stuXcapSerMgr->URL, &MsgInfo);
		if (uRet != VOS_OK)
		{
			Money_PrintErr(__FILE__,__LINE__,"dealFun error,input num:%d, ret:%d",uNum, uRet);
		}
	}
	else
	{
		Money_PrintErr(__FILE__,__LINE__,"get_xcap_root error %d", uNum);
	}
	if (0 != MsgInfo.Body.stuDataBuf.nSize)
	{
		Money_PrintInfo(__FILE__,__LINE__,"RES :\n%s", MsgInfo.Body.stuDataBuf.strBuffer+sizeof(XCAP_SER_MGR));
		
		VOS_Memcpy(&MsgInfo.Body.msghead.stSrcProc,
			&pMsgInfo->Body.msghead.stDstProc,	sizeof(CPSS_COM_PID));
		VOS_Memcpy(&MsgInfo.Body.msghead.stDstProc,
			&pMsgInfo->Body.msghead.stSrcProc,	sizeof(CPSS_COM_PID));

		MsgInfo.Body.msghead.ulRecvMsgID = pMsgInfo->Body.msghead.ulMsgID;
		
		Money_PrintInfo(__FILE__,__LINE__,"Recv MsgID:%d\n", pMsgInfo->Body.msghead.ulMsgID);
		
		uRet = send_udp_data(&MsgInfo, "", 0,VOS_SEND_SKT_TYPE_FINISH,
			CPSS_RES_XCAP_GET,CPSS_TYPE_GET_SUBURL);
		if (VOS_OK != uRet)
		{
			Money_PrintErr(__FILE__,__LINE__,"send udp data error");
		}
		return uRet;

	}
	uRet = VOS_OK;
END_PROC:
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root_body
 *  Description:  �õ�root��body
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 get_xcap_html_head(pCPSS_MEM_BUFFER pstuBuff,VOS_CHAR * pstrInput, VOS_UINT32 nType)
{
	VOS_UINT32 uRet = VOS_OK;
	switch(nType)
	{
	case 0:
		VOS_PrintBuffer(pstuBuff,"%s%s%s",
			"<html>\r\n<head>\r\n\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n\t<title>",
			pstrInput,
			"</title>\r\n</head>\r\n<body>");
		break;
	case 1:
		VOS_PrintBuffer(pstuBuff,"%s%",
		"\r\n</body></html>");
		break;
	default:
		uRet = VOS_ERR;
		break;	
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root_body
 *  Description:  �õ�root��body
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 get_xcap_root_body(VOS_CHAR * pstrInput,VOS_VOID * pMsgVoid)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_CHAR strPut[1024] = {0};
	FILE * filefd = 0;
	pCPSS_MEM_BUFFER pstuBuff = NULL;
	CPSS_MSG * pMsgInfo = (CPSS_MSG *)pMsgVoid;

	if (NULL == pMsgInfo)
	{
		Money_PrintErr(__FILE__,__LINE__,"get_xcap_root_body input msg info is NULL");
		goto END_PROC;
	}
	

	//pMsgInfo.Body.msghead.ulMsgLength = 0;
	pstuBuff = &pMsgInfo->Body.stuDataBuf;
	
	if (NULL == pstuBuff)
	{
		Money_PrintErr(__FILE__,__LINE__,"get_xcap_root_body input msg buffer is NULL");
		goto END_PROC;
	}
	//VOS_PrintBuffer(pstuBuff,"%X%X",0XEF,0XEF);
	//pstuBuff->strBuffer[0] = (VOS_CHAR)0XEF;
	//pstuBuff->strBuffer[1] = (VOS_CHAR)0XBB;
	//pstuBuff->strBuffer[2] = (VOS_CHAR)0XBF;
	//pstuBuff->nSize = 3;

	//pstrRes[0] = (VOS_CHAR)0XEF;
	//pstrRes[1] = (VOS_CHAR)0XBB;
	//pxCap_Response->pstrBodyBuffer[pxCap_Response->nBodyConut][2] = (VOS_CHAR)0XBF;
	get_xcap_html_head(pstuBuff,"Insert title here",0);
	VOS_PrintBuffer(pstuBuff,"%s",
		"<form action=\"user.html\" name=\"form1\">");//
//		"\r\n<img src=\"src/bd_header.jpg\">");
	VOS_PrintBuffer(pstuBuff,"%s%s%s%s",
		"\r\n<table align=\"center\">",
		"\r\n\t<tr>",
		"\r\n\t\t<td>User:</td>",
		"\r\n\t\t<td><input type=\"text\" name=\"userName\"></td>");

	VOS_PrintBuffer(pstuBuff,"%s%s%s%s",
		"\r\n\t</tr>",
		"\r\n\t<tr>" ,
		"\r\n\t\t<td>Passwd:</td>" ,
		"\r\n\t\t<td><input type=\"password\" name=\"userPwd\"></td>");

	VOS_PrintBuffer(pstuBuff,"%s%s%s%s",
		"\r\n\t</tr>" ,
		"\r\n\t<tr>" ,
		"\r\n\t\t<td><input type=\"submit\" value=\"Submit\"></td>" ,
		"\r\n\t\t<td><input type=\"reset\" name=\"Refush\"></td>");

	VOS_PrintBuffer(pstuBuff,"%s%s",
		"\r\n\t</tr>" ,
		"\r\n</table>" );

	return get_xcap_html_head(pstuBuff,"",1);
END_PROC:
	return uRet;
}