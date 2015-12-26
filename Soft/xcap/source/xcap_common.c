/*
 * =====================================================================================
 *
 *       Filename:  xcap_common.c
 *
 *    Description:  xcap函数
 *
 *        Version:  1.0
 *        Created:  2012年07月28日 18时41分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
*/
//#include <sys\stat.h>
#include "xcap_common.h"

#define VOS_XCAP_Malloc(ulSize)				VOS_Malloc((ulSize), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_Realloc(pstrads,ulSize)	VOS_Realloc((pstrads), (ulSize), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_Remset(pstrads)			VOS_Remset((pstrads), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_MemSize(pstrads)			VOS_Memsize((pstrads), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_Memcls(pstrads)			VOS_Memcls((pstrads), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_Memcat(pstrA,pstrB)		VOS_Memcat((pstrA), (pstrB), (XCAP_MEM_HEAD_KEY_URL_COMM))
#define VOS_XCAP_Free(pstrads)				VOS_Free((pstrads), XCAP_MEM_HEAD_KEY_URL_COMM)
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void XCAP_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_XCAP, CPSS_PRINT_INFO, 
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
void XCAP_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_XCAP, CPSS_PRINT_ERR,
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
void XCAP_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_XCAP, CPSS_PRINT_WARN, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_response_get_code(VOS_UINT16 Code, STATUS_CODE ** pCode)
{
	VOS_UINT32 uIndex = 0;
	VOS_UINT32 uCount = sizeof(g_StatusCode)/sizeof(STATUS_CODE);
	for (uIndex = 0; uIndex < uCount; uIndex++)
	{
		if (g_StatusCode[uIndex].uNum == Code)
		{
			*pCode = &g_StatusCode[uIndex];
			return VOS_OK;
		}
	}
	return VOS_ERR;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_get_http_date
 *  Description:  得到当前系统时间或文件时间,并转换为http头域时间格式
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_CHAR * xcap_get_http_date(VOS_STRING pstrTime, LPFILETIME pft)
{
	SYSTEMTIME st;
	if (NULL == pstrTime)
	{
		XCAP_PrintErr(__FILE__, __LINE__,"get xcap http date is NULL");
		return "";
	}
	if (NULL != pft)
	{
		FileTimeToSystemTime( pft, &st );
	}
	else
	{
		GetSystemTime( &st );
	}
	InternetTimeFromSystemTime( &st, INTERNET_RFC1123_FORMAT,
								pstrTime, INTERNET_RFC1123_BUFSIZE );
	return pstrTime;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_fileds_init
 *  Description:  初始化 头域节点
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_VOID xcap_fileds_init()
{
	VOS_UINT32 uIndex    = 0,uCount = sizeof(g_fields_req)/sizeof(XCAP_FIELDS_TEMP);
	for (uIndex = 0; uIndex < uCount; uIndex ++)
	{
		g_fields_req[uIndex].m_fieldsLen = VOS_Strlen(g_fields_req[uIndex].m_fields.Key);
	}
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_analyzing_fields(pXCAP_REQUEST pMsgInfo, VOS_CHAR * pstrInput)
{
	VOS_UINT32 uRet      = VOS_ERR;
	VOS_CHAR * pstrTemp  = NULL;
	//VOS_UINT32 uIndexTmp = 0;
	VOS_UINT32 uIndex    = 0,uCount = sizeof(g_fields_req)/sizeof(XCAP_FIELDS_TEMP);

	if (0 == pstrInput[0])
	{
		return uRet;
	}
	for (uIndex = 0; uIndex < uCount; uIndex ++)
	{
		pstrTemp = VOS_Strstr(pstrInput, g_fields_req[uIndex].m_fields.Key);
		if (NULL != pstrTemp)
		{
			break;
		}
	}
	if (uIndex == uCount)
	{
		return uRet;
	}
	VOS_Strcpy(pMsgInfo->Req_head_fields[pMsgInfo->fields_num].Key,
		g_fields_req[uIndex].m_fields.Key);
	VOS_Strcpy(pMsgInfo->Req_head_fields[pMsgInfo->fields_num].Value,
		pstrTemp+g_fields_req[uIndex].m_fieldsLen);
	pMsgInfo->fields_num ++;
	uRet = VOS_OK;
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_CHAR * xcap_find_fields(pXCAP_REQUEST pMsgInfo, VOS_UINT32 m_Value)
{
	VOS_UINT32 uIndex = 0;
	if (NULL == pMsgInfo)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap find requet pMsg is NULL");
		return NULL;
	}
	for (uIndex = 0; uIndex < pMsgInfo->fields_num; uIndex ++)
	{
		if (0 == VOS_Strcmp(g_fields_req[m_Value].m_fields.Key, pMsgInfo->Req_head_fields[uIndex].Key))
		{
			return pMsgInfo->Req_head_fields[uIndex].Value;
		}
	}
	return NULL;
	
}
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID xcap_print_request(pXCAP_REQUEST pMsgInfo)
{
	VOS_CHAR strBuffer[CPSS_MSG_BUFFER_SIZE] = {0};
	VOS_UINT32 uIndex = 0;
	
	if (NULL == pMsgInfo)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap printf requet pMsg is NULL");
		return ;
	}
	XCAP_PrintInfo(__FILE__, __LINE__, "<-- Method:%s URL:%s Ver:%s",pMsgInfo->Req_head.Method,
		pMsgInfo->Req_head.Request_URI,
		pMsgInfo->Req_head.Request_version);
	/*
	for (uIndex = 0; uIndex <pMsgInfo->fields_num; uIndex++)
	{
		XCAP_PrintInfo("", CPSS_PRINTF_BUFFER,"<-- %s%s", pMsgInfo->Req_head_fields[uIndex].Key, pMsgInfo->Req_head_fields[uIndex].Value);
	}
	*/
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_request_check(XCAP_REQUEST * pReq)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 uIndex = 0;
	VOS_UINT32 uCount = sizeof(g_xcap_method)/sizeof(XCAP_METHOD);
	// check version
	if (0 == VOS_Strcmp(pReq->Req_head.Request_version,"HTTP/1.1"))
	{
		XCAP_PrintErr(__FILE__,__LINE__,"req version not allow");
		return 505;
	}
	
	for (uIndex = 0; uIndex < uCount; uIndex ++)
	{
		if (0 == _tcscmp(pReq->Req_head.Method, g_xcap_method[uIndex].strReq_Method))
		{
			pReq->Req_mothod = g_xcap_method[uIndex].nReq_Method;
			break;
		}
	}
	uRet = VOS_OK;
	if (uIndex == uCount)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"req version not allow");
		uRet = VOS_ERR;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_analyzing_buffer(pXCAP_REQUEST pMsgInfo, VOS_CHAR * pstrInput)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_CHAR   uFieldsNum = 0;
	VOS_CHAR * pstrTemp = NULL;
	VOS_CHAR * pstrTemp1 = NULL;
	VOS_CHAR * pstrInfo = NULL;
	VOS_CHAR   uLength = 0;
	VOS_CHAR   pbuf[CPSS_MSG_BUFFER_SIZE] = {0};//, VOS_STRING pline, VOS_INT32 skip;


	pstrInfo = pstrInput;
	while(0 != pstrInfo[0])
	{
		BZERO(pbuf, CPSS_MSG_BUFFER_SIZE);
		pstrInfo = cpss_getline(pstrInfo, pbuf, VOS_FALSE);
		cpss_trim_right(pbuf, CPSS_SEP_BUFF);
		if (0 ==pbuf[0])
		{
			continue;
		}
		uFieldsNum = xcap_analyzing_fields(pMsgInfo, pbuf);
		if (VOS_ERR != uFieldsNum)
		{
			if (XCAP_FIELDS_HOST == uFieldsNum)
			{
				uLength = VOS_Strlen(g_fields_req[uFieldsNum].m_fields.Key);
				pstrTemp = VOS_Strstr(pbuf+uLength, ":");
				if (NULL != pstrTemp)
				{
					VOS_Strncpy(pMsgInfo->strHost, pbuf+uLength, pstrTemp -pbuf-uLength);
					pMsgInfo->uPort = atoi(pstrTemp+1);
				}
			}
			continue;
		}
		
		
		pstrTemp = VOS_Strstr(pbuf, "/");
		pstrTemp1 = VOS_Strstr(pbuf, " HTTP");
		if (NULL != pstrTemp && NULL != pstrTemp1)
		{
			VOS_Strncpy(pMsgInfo->Req_head.Method, pbuf, pstrTemp - pbuf);
			cpss_trim(pMsgInfo->Req_head.Method," ");
			VOS_Strncpy(pMsgInfo->Req_head.Request_URI, pstrTemp , pstrTemp1 - pstrTemp);
			VOS_Strcpy(pMsgInfo->Req_head.Request_version, pstrTemp1);
		}
		else
		{
			XCAP_PrintErr(__FILE__, __LINE__, "buf no find / or http:%s",pbuf);
		}
	}
/*	pstrTemp = xcap_find_fields(pMsgInfo, XCAP_REQ_FIELDE_REFERER);
	if (NULL != pstrTemp)
	{
		XCAP_PrintInfo(__FILE__, __LINE__, "get fields %s: %s",
			g_fields_req[XCAP_REQ_FIELDE_REFERER].m_fields.Key, pstrTemp);
	}*/
	uRet = VOS_OK;
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_fildes
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID xcap_set_fildes(VOS_UINT32 ufildesNum, pXCAP_RESPONSE pxCap_Response, VOS_CHAR * pstBuffer)
{
	pXCAP_FIELDS pstuFildes = NULL;
	VOS_UINT32 uIndex = 0;

	if (NULL == pxCap_Response)
	{
		return;
	}
	// check in response head fields
	for (uIndex = 0; uIndex < pxCap_Response->fields_num;uIndex ++)
	{
		if (0 == VOS_Strcmp(g_fields_res[ufildesNum].m_fields.Key, pxCap_Response->Res_head_fields[uIndex].Key))
		{
			VOS_Strcpy(pxCap_Response->Res_head_fields[uIndex].Value, pstBuffer);
			return ;
		}
	}
	if (ufildesNum < 0 || ufildesNum > sizeof(g_fields_res)/sizeof(XCAP_FIELDS_TEMP))
	{
		return;
	}
	VOS_Strncpy(pxCap_Response->Res_head_fields[pxCap_Response->fields_num].Key, g_fields_res[ufildesNum].m_fields.Key,XCAP_KEY_LENGTH);
	VOS_Strncpy(pxCap_Response->Res_head_fields[pxCap_Response->fields_num].Value, pstBuffer,XCAP_VALUE_LENGTH);
	pxCap_Response->fields_num ++;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_head
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_head(pXCAP_RESPONSE pxCap_Response, VOS_UINT16 StatueCode)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_CHAR	strTimebuffer[INTERNET_RFC1123_BUFSIZE] = {0};
	VOS_Strcpy(pxCap_Response->Res_head.Request_version, "HTTP/1.1");
	pxCap_Response->Res_head.StatueCode = StatueCode;
	xcap_set_fildes(XCAP_RES_FIELDE_SERVER,	   pxCap_Response, "Jance Xcap Server");
//	set_xcap_fildes(XCAP_RES_FIELDE_CONNECTIN, pxCap_Response, "keep-alive");
//	xcap_set_fildes(XCAP_RES_FIELDE_DATE,      pxCap_Response, xcap_get_http_date(strTimebuffer, NULL));
	uRet = VOS_OK;
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_body
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_body_size(pXCAP_RESPONSE pxCap_Response, pCPSS_MSG pMsgInfo, VOS_UINT32 ulOffset)
{
	VOS_UINT32		ulRet = VOS_ERR;
	VOS_CHAR		strSize[64]={0};
	VOS_CHAR		* pstrDataBuffer = pMsgInfo->Body.strDataBuf;
	VOS_UINT32		ulBodySize = 0;
	if (NULL == pxCap_Response ||
		NULL == pMsgInfo)
	{
		XCAP_PrintErr(__FILE__, __LINE__,"get body size input is NULL");
		return ulRet;
	}
	//VOS_PrintBufferBin(pstrDataBuffer,"",0,&ulBodySize);
	if (0 == ulBodySize)
	{
		XCAP_PrintErr(__FILE__, __LINE__,"get body is empty");
	}
	ulBodySize = ulBodySize - ulOffset;//Content-Length:
	sprintf(strSize, "%d", ulBodySize);
	xcap_set_fildes(XCAP_RES_FIELDE_CONTENT_LENGTH,pxCap_Response, strSize);
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_head
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_responce_head(VOS_CHAR* pstrDataBuffer, pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_UINT32  uIndex = 0;
	if (NULL == pstrDataBuffer ||
		NULL == pxCap_Response)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"input is NULL");
		return uRet;
	}
	while(uIndex <= pxCap_Response->fields_num)
	{
		if (pxCap_Response->Res_head_fields[uIndex].Key[0] == 0 ||
			pxCap_Response->Res_head_fields[uIndex].Value[0] == 0)
		{
			uIndex++;
			continue;
		}
		VOS_PrintBuffer(pstrDataBuffer,"%s%s\r\n",
			pxCap_Response->Res_head_fields[uIndex].Key,
			pxCap_Response->Res_head_fields[uIndex].Value);
		uIndex++;
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_body
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_body(pXCAP_RESPONSE pxCap_Response, pCPSS_MSG pMsgInfo)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_CHAR	* pstrDataBuffer = NULL;
	VOS_CHAR	stTemp =0;
	VOS_UINT32  uRecvBufferLen = 0;
	VOS_UINT32  uOffsetSize = 0;
	pCPSS_MSG   pRecvMsgInfo = NULL;
	STATUS_CODE *pCode = NULL;

	if (NULL == pxCap_Response ||
		NULL == pMsgInfo)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"input is NULL");
		return uRet;
	}
	if (pxCap_Response->ulMsgID == 0)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"xcap client is null");
		return uRet;
	}
	pstrDataBuffer = pMsgInfo->Body.strDataBuf;
	if (VOS_OK != xcap_response_get_code(pxCap_Response->Res_head.StatueCode, &pCode))
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is failed");
		return uRet;
	}
	if (NULL == pCode)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is error");
		return uRet;
	}

	VOS_PrintBuffer(pstrDataBuffer,"%s %d %s\r\n",pxCap_Response->Res_head.Request_version,
 		pCode->uCode,pCode->phrase);
	
	if (VOS_OK != xcap_set_responce_head(pstrDataBuffer, pxCap_Response))
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is failed");
		return uRet;
	}
	
	pstrDataBuffer = pMsgInfo->Body.strDataBuf;
	while(NULL != pstrDataBuffer)
	{
		//xcap_send_info_msgid(pxCap_Response->ulMsgID, pstrDataBuffer->strBuffer, pstrDataBuffer->nSize,VOS_SEND_SKT_TYPE_INSERT);
		/*
		if (pstrDataBuffer != &pMsgInfo->Body.strDataBuf)
		{
			pstrDataBuffer = (CPSS_MEM_BUFFER *)cpss_get_next_buffer(&pMsgInfo->Body.strDataBuf);
		}
		else
		{
			pstrDataBuffer = pMsgInfo->Body.strDataBuf.next;
		}
		*/
	}	
	stTemp = (VOS_CHAR)0XEF;
	xcap_send_info_msgid(pxCap_Response->ulMsgID, (VOS_CHAR*)&stTemp,1,VOS_SEND_SKT_TYPE_INSERT);
	stTemp = (VOS_CHAR)0XBB;
	xcap_send_info_msgid(pxCap_Response->ulMsgID, (VOS_CHAR*)&stTemp,1,VOS_SEND_SKT_TYPE_INSERT);
	stTemp = (VOS_CHAR)0XBF;
	xcap_send_info_msgid(pxCap_Response->ulMsgID, (VOS_CHAR*)&stTemp,1,VOS_SEND_SKT_TYPE_INSERT);
	xcap_send_info_msgid(pxCap_Response->ulMsgID, 
		XCAP_HEAD_BODY,VOS_Strlen(XCAP_HEAD_BODY),VOS_SEND_SKT_TYPE_INSERT);

	
	uRecvBufferLen = sizeof(XCAP_SER_MGR);
	pstrDataBuffer = pMsgInfo->Body.strDataBuf;
	while(NULL != pstrDataBuffer)
	{
		/*if (pstrDataBuffer->nSize >= CPSS_MSG_BUFFER_USED || NULL == pstrDataBuffer->next)
		{
			xcap_send_info_msgid(pxCap_Response->ulMsgID, pstrDataBuffer->strBuffer+uRecvBufferLen, 
				pstrDataBuffer->nSize-uRecvBufferLen,VOS_SEND_SKT_TYPE_FINISH);
			uRet = VOS_OK;
			break;
		}
		
		xcap_send_info_msgid(pxCap_Response->ulMsgID, pstrDataBuffer->strBuffer+uRecvBufferLen, 
			pstrDataBuffer->nSize-uRecvBufferLen,VOS_SEND_SKT_TYPE_INSERT);
		uRecvBufferLen = 0;
		
		if (pstrDataBuffer != &pMsgInfo->Body.strDataBuf)
		{
			pstrDataBuffer = (CPSS_MEM_BUFFER *)cpss_get_next_buffer(&pMsgInfo->Body.strDataBuf);
		}
		else
		{
			pstrDataBuffer = pMsgInfo->Body.strDataBuf.next;
		}
		*/
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_responce_proc
 *  Description:  xcap responce 
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_responce_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	pXCAP_RESPONSE pxCap_Respone_Info = NULL;
	pXCAP_MSG_MANAGE pXcap_Msg_Mgr = NULL;
	pXCAP_SER_MGR  pXcap_Svr_Mgr = (pXCAP_SER_MGR)pMsgInfo->Body.strDataBuf;
	
	if (NULL == pXcap_Svr_Mgr)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"xcap request is msg mgr is NULL");
		return ulRet;
	}
	pXcap_Msg_Mgr = pXcap_Svr_Mgr->Req_Mgr;
	if (NULL == pXcap_Msg_Mgr)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"xcap request is msg mgr is NULL");
		return ulRet;
	}
	//if (0 == pMsgInfo->Body.strDataBuf.strBuffer[sizeof(XCAP_SER_MGR)])
	{
		XCAP_PrintErr(__FILE__,__LINE__,"xcap request is msg mgr is NULL");
		return ulRet;
	}
	
	pxCap_Respone_Info = &pXcap_Msg_Mgr->xCap_Respone_Info;
	xcap_set_head(pxCap_Respone_Info, XCAP_RES_CODE_200);
	xcap_set_body_size(pxCap_Respone_Info, pMsgInfo, sizeof(XCAP_SER_MGR));
	xcap_set_body(pxCap_Respone_Info,pMsgInfo);
	//VOS_Free(pXcap_Msg_Mgr, sizeof(pXCAP_MSG_MANAGE));
	return VOS_OK;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_head
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_get_body_proc(pCPSS_MSG pMsgInfo, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen,
								 VOS_UINT8 uType, VOS_UINT8 uSubType)
{
	VOS_UINT32		uRet = VOS_ERR;
	CPSS_MSG		MsgInfo;

	BZERO(&MsgInfo, sizeof(CPSS_MSG));
	
	MsgInfo.Body.msghead.stSrcProc.ulCpuID = cpss_get_cpuid_pid(
		CPSS_CONNECT_SUB_XCAP, CPSS_CONNECT_MOCLI, CPSS_GET_TYPE_CPUID);
	if (0 == MsgInfo.Body.msghead.stSrcProc.ulCpuID)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"src cpuid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stSrcProc.ulPID   = cpss_get_cpuid_pid(
		CPSS_CONNECT_SUB_XCAP, CPSS_CONNECT_MOCLI, CPSS_GET_TYPE_PID);
	if (0 == MsgInfo.Body.msghead.stSrcProc.ulPID)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"src pid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stDstProc.ulCpuID = cpss_get_cpuid_pid(
		CPSS_CONNECT_SUB_MONEY, CPSS_CONNECT_MONEY, CPSS_GET_TYPE_CPUID);
	if (0 == MsgInfo.Body.msghead.stDstProc.ulCpuID)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"dst cpuid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stDstProc.ulPID   = cpss_get_cpuid_pid(
		CPSS_CONNECT_SUB_MONEY, CPSS_CONNECT_MONEY, CPSS_GET_TYPE_PID);
	if (0 == MsgInfo.Body.msghead.stDstProc.ulPID)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"dst pid is error");
		return uRet;
	}

	MsgInfo.Body.msghead.ulParentMsgID = pMsgInfo->ulMsgID;
	MsgInfo.Body.msghead.uType = uType;

	uRet = money_send_data(&MsgInfo,pstuBuffer,uBufLen,VOS_SEND_SKT_TYPE_FINISH);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_user_body
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 get_xcap_user_body(pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32 uRet = VOS_ERR;
//	set_xcap_head();
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_analyzing_root(pXCAP_SER_MGR pSerMgr, pXCAP_REQUEST pMsgInfo)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR	strTimebuffer[INTERNET_RFC1123_BUFSIZE] = {0};
	VOS_CHAR * pstrTemp1 = NULL;

	if (NULL == pSerMgr)
	{
		XCAP_PrintErr(__FILE__, __LINE__,"analyzing root input null");
		return ulRet;
	}
	BZERO(pSerMgr,sizeof(XCAP_SER_MGR));
	pstrTemp1 = VOS_Strstr(pMsgInfo->Req_head.Request_URI,"/");
	if (pMsgInfo->Req_head.Request_URI == pstrTemp1)
	{
		VOS_Strcpy(pSerMgr->RootName,"money");
	}
	*pstrTemp1 = 0;
	VOS_Strcpy(pSerMgr->RootName,pMsgInfo->Req_head.Request_URI);
	*pstrTemp1 = '/';
	VOS_Strcpy(&pSerMgr->URL,pMsgInfo->Req_head.Request_URI);
	ulRet = VOS_OK;
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  send_xcap_URL
 *  Description:  给xcap 服务器发送url信息
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_request_URL(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32		uBuffLen = 0;
	VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	pXCAP_SER_MGR    pstuXcapSerMgr = NULL;
	pXCAP_MSG_MANAGE  pXcap_Msg_Mgr=NULL;
	
	if (CPSS_MSG_REQ != cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "get address is error");
		return uRet;
	}

	pXcap_Msg_Mgr = (pXCAP_MSG_MANAGE)VOS_XCAP_Malloc(sizeof(XCAP_MSG_MANAGE));
	if (NULL == pXcap_Msg_Mgr)
	{
		XCAP_PrintErr(__FILE__, __LINE__,"get address is error");
		return uRet;
	}
	BZERO(pXcap_Msg_Mgr, sizeof(XCAP_MSG_MANAGE));

	uRet = xcap_analyzing_buffer(&pXcap_Msg_Mgr->xCap_Request_Info, 
		pMsgInfo->Body.strDataBuf);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "analyzing xcap buffer error");
		goto ERR_PROC;
	}
	if(VOS_OK != xcap_request_check(&pXcap_Msg_Mgr->xCap_Request_Info))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "check xcap buffer error");
		goto ERR_PROC;
	}
	pstuXcapSerMgr = (pXCAP_SER_MGR)strBuffer;
	if (VOS_OK != xcap_analyzing_root(pstuXcapSerMgr,&pXcap_Msg_Mgr->xCap_Request_Info))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "analyzing xcap root error ");
		goto ERR_PROC;
	}
	pstuXcapSerMgr->Req_Mgr = pXcap_Msg_Mgr;
	pstuXcapSerMgr->uStat = 0x0A;
	pXcap_Msg_Mgr->xCap_Request_Info.ulMsgID = pMsgInfo->ulMsgID;
	pXcap_Msg_Mgr->xCap_Respone_Info.ulMsgID = pMsgInfo->ulMsgID;
	
	xcap_print_request(&pXcap_Msg_Mgr->xCap_Request_Info);
	switch (pXcap_Msg_Mgr->xCap_Request_Info.Req_mothod)
	{
	case XCAP_REQ_GET:
		//VOS_Memcpy(strBuffer,(VOS_CHAR*)&stuXcapSerMgr,sizeof(XCAP_SER_MGR));
		uBuffLen = sizeof(XCAP_SER_MGR)+VOS_Strlen(pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI);
		//uRet = xcap_get_body_proc(pMsgInfo, strBuffer, uBuffLen, CPSS_REQ_XCAP_GET, CPSS_TYPE_GET_SUBURL);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "send xcap data failed");
		}
		break;
	case XCAP_REQ_PUT:
		break;
	case XCAP_REQ_OTHER:
		break;
	case XCAP_REQ_POST:
		break;
	case XCAP_REQ_HEAD:
		break;
	case XCAP_REQ_DELETE:
		break;
	case XCAP_REQ_TRACE:
		break;
	case XCAP_REQ_CONNECT:
		break;
	default:
		break;
	}
//END_PROC:
	//pMsgInfo->pRecvBuffer[0] = pXcap_Msg_Mgr;
	return VOS_OK;
ERR_PROC:
	if (NULL != pXcap_Msg_Mgr)
	{
		VOS_Free(pXcap_Msg_Mgr, sizeof(XCAP_MSG_MANAGE));
	}
	return VOS_ERR;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  money_send_data
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_send_info(VOS_VOID *pVoidMsg, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	
	uRet = cpss_send_data(pVoidMsg,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_send_info_msgid
 *  Description:  发送telnet的数据
 *  IBnput      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_send_info_msgid(VOS_UINT32 uMsgID, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	
	uRet = cpss_send_data_msgid(uMsgID,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}










































/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 get_xcap_file_no_find(pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_PrintBuffer(&pxCap_Response->pstuBuffer,"%s"
		"<html>\r\n<head>\r\n\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n\t<title>Insert title here</title>\r\n</head>\r\n" \
		"<body>" \
		"\r\n<table align=\"center\">" \
		"\r\n\t<tr>" \
		"\r\n\t\t<td>not find 404</td>" \
		"\r\n\t</tr>" \
		"\r\n</table>" \
		"\r\n</body></html>");
	pxCap_Response->Res_head.StatueCode = XCAP_RES_CODE_404;
	return VOS_OK;
}

















#if 0 




#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <error.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// 建立socket 开始侦听 接收连接 接收客户端数据 解析http协议 发送文件数据给客户端

#define HTTP_PORT "2010"
#define MAX_CONNECTION 10
#define DOCUMENT_ROOT "www"
#define LOG_PATH "log/access.log"

void parser(char *s,char res[][255],char host[][255]);
static char *strtoupper( char *s );
static long filesize(const char *filename);
static int file_exists(const char *filename);
static void mime_content_type( const char *name, char *ret );
static int WriteLog( const char *message );
static int is_dir(const char *filename);

static unsigned short g_is_log        = 1;
static int g_log_fd                    = 0;

int main(void)
{
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_in sin;
    struct stat file_stat;
    pid_t pid;
    char client_ip[100];
    char buf[20000];
    char buf_all[2000];
    char buf1[2000];
    char p[3][255];
    char h[3][255];
    char tmp[2000];
    char cwd[1024];
    char filename[2000];
    char filepath[2000];
    int fd,size;
    int currentConn = 0;
    
    DIR * dir;
    struct dirent * ptr;
    


    chdir("../");
    
    if ( (pid = fork()) < 0 )
    {
        perror("fork");
        exit(1);
    }
    else if ( pid != 0)
    {
        exit(1);
    }
    
    if((server_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(HTTP_PORT));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    
    if(listen(server_sock,MAX_CONNECTION) < 0)
    {
        perror("listen");
        exit(1);
    }
    
    printf("fasthttp successful created ...\n");
    
    while(1)
    {
        unsigned int clientlen = sizeof(client_addr);
        if((client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&clientlen)) < 0)
        {
            perror("accept");
            exit(1);
        }
        
        if((pid = fork()) == 0)
        {

            if(read(client_sock,buf,20000) < 0)
            {
                perror("read data from client");
                exit(1);
            }
            
            parser(buf,p,h);
            
            if(strcmp(strtoupper(p[0]),"GET") != 0
                && strcmp(strtoupper(p[0]),"POST") != 0
                && strcmp(strtoupper(p[0]),"HEAD") != 0)
            {
                memset(&buf,0,sizeof(buf));
                
                sprintf(buf, "HTTP/1.1 501 Not Implemented\r\nServer: %s\r\nContent-Type: text/html\r\nContent-Length:    1489\r\nAccept-Ranges: bytes\r\nConnection: close\r\n\r\n", "Apache");
                write(client_sock,buf,strlen(buf));
                
                memset(&buf,0,sizeof(buf));
                sprintf(buf,"<h2>%s Method Not Implemented</h2>","501");
                write(client_sock,buf,strlen(buf));
                close(client_sock);
                exit(0);
            }
             
            if(strcmp(p[1],"/") == 0)
            {
                memset(&tmp,0,sizeof(tmp));
                sprintf(tmp,"%s","index.html");
                strcat(p[1],tmp);
            }
            
            WriteLog(p[1]);
            getcwd(filepath, sizeof(filepath));
            strcat(filepath,"/");
            strcat(filepath,DOCUMENT_ROOT);
             strcat(filepath,p[1]);

            if(!file_exists(filepath))
            {
                memset(&buf,0,sizeof(buf));
                sprintf(buf, "HTTP/1.1 404 Not Found\r\nServer: %s\r\nContent-Type: text/html\r\nContent-Length:    257271\r\nConnection: close\r\n\r\n", "Apache");
                write(client_sock,buf,strlen(buf));
                
                memset(&buf,0,sizeof(buf));
                sprintf(buf,"<html><head><title>404 Not Found</title></head><body bgcolor=\"white\"><center><h1>404 Not Found</h1></center><hr><center>Powered by %s</center></body></html>","fasthttp");
                write(client_sock,buf,strlen(buf));
                close(client_sock);
                
                memset(&buf,0,sizeof(buf));
                sprintf(buf,"404 Not Found\t%s\n",filepath);
                WriteLog(buf);
                
                exit(0);
            }
            
            if(access(filepath,R_OK) < 0)
            {
                memset(&buf,0,sizeof(buf));
                sprintf(buf, "HTTP/1.1 403 Forbidden\r\nServer: %s\r\nContent-Type: text/html\r\nContent-Length:    25727\r\nConnection: close\r\n\r\n", "Apache");
                write(client_sock,buf,strlen(buf));
                close(client_sock);
                exit(0);
            }
            
             /** 目录列表 **/
             if(is_dir(filepath))
             {
                memset(&tmp,0,sizeof(tmp));
                sprintf(tmp,"<html><head><title>Index of %s</title></head><body><h1>Index of %s</h1><ul><li><a href=\"/\"> Parent Directory</a></li>",filepath,filepath);
                strcat(buf,tmp);

                 if((dir = opendir(filepath)) != NULL)
                 {
                     while((ptr = readdir(dir)) != NULL)
                     {
                         
                         if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
                         {
                             continue;    
                         }
                         
                         memset(&buf,0,sizeof(buf));
                         sprintf(buf,"%s/%s",filepath,ptr->d_name);
                        
                        if(is_dir(buf))
                        {
                            memset(&buf,0,sizeof(buf));
                            sprintf(buf,"<li><a href=\"%s/\"> %s/</a></li>",ptr->d_name,ptr->d_name);
                        }
                        else
                        {
                            memset(&buf,0,sizeof(buf));
                            sprintf(buf,"<li><a href=\"%s\"> %s</a></li>",ptr->d_name,ptr->d_name);    
                        }
                        strcat(tmp,buf);
                     }
                 }
                 closedir(dir);

                 memset(&buf,0,sizeof(buf));
                sprintf(buf,"%s","</ul>");
                strcat(tmp,buf);
                
                memset(&buf,0,sizeof(buf));
                sprintf(buf, "HTTP/1.1 200 OK\r\nServer: fasthttp\r\nContent-Type: text/html;charset=utf-8\r\nContent-Length:    %d\r\nConnection: close\r\n\r\n", strlen(tmp));
                write(client_sock,buf,strlen(buf));
            
                write(client_sock,tmp,strlen(tmp));
                close(client_sock);
             }
             
            memset(&tmp,0,sizeof(tmp));
            mime_content_type(filepath,tmp);
            
            memset(&buf,0,sizeof(buf));
            sprintf(buf, "HTTP/1.1 200 OK\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length:    25727\r\nConnection: close\r\n\r\n", "Apache",tmp);
            write(client_sock,buf,strlen(buf));
            
            
            
            memset(&buf,0,sizeof(buf));
            fd = open(filepath,O_RDONLY);
            read(fd,buf,filesize(filepath));
            close(fd);
            
            write(client_sock,buf,filesize(filepath));
            close(client_sock);
            
            memset(&buf,0,sizeof(buf));
            sprintf(buf,"200 OK\t%s\t%d\n",filepath,filesize(filepath));
            WriteLog(buf);
            
            exit(0);
        }
        else
        {
            wait(NULL);
        }
        
        close(client_sock);
    }
    
    
}

void parser(char *s,char res[][255],char host[][255])
{
    int i,j = 0;
    int n;
    char hosts[255];

    for (i = 0;s[i] != '\r';i++)        /* obtain the first line in http protocol head */
        ;
    s[i] = '\0';
    n=i++;
    
    for (i = 0,j = 0;i < 3;i++,j++)        /* divide the protocol head in blank */
    {
        strcpy(res[j],strsep(&s," "));
    }
    
    for(i=n;s[i] != '\r';i++)
    {
        strcat(hosts,s[i]);
    }
    
    for (i = 0,j = 0;i < 3;i++,j++)        /* divide the protocol head in blank */
    {
        strcpy(host[j],strsep(&hosts,":"));
    }
    
}

/**
 * strtoupper - string to upper
 *
 */
static char *strtoupper( char *s )
{
    int i, len = sizeof(s);
    for( i = 0; i < len; i++ )
    {
        s[i] = ( s[i] >= 'a' && s[i] <= 'z' ? s[i] + 'A' - 'a' : s[i] );
    }
    
    return(s);
}

/**
 *  filesize - get file size
 */
static long filesize(const char *filename)
{
    struct stat buf;
    if (!stat(filename, &buf))
    {
        return buf.st_size;
    }
    return 0;
}

/**
 * file_exists - check file is exist
 */
static int file_exists(const char *filename)
{
    struct stat buf;
    
    if (stat(filename, &buf) < 0)
    {
        if (errno == ENOENT)
        {
            return 0;
        }
    }
    return 1;
}

/**
 * Get MIME type header
 *
 */
static void mime_content_type( const char *name, char *ret ){
    char *dot, *buf;

    dot = strrchr(name, '.');

    /* Text */
    if ( strcmp(dot, ".txt") == 0 ){
        buf = "text/plain";
    } else if ( strcmp( dot, ".css" ) == 0 ){
        buf = "text/css";
    } else if ( strcmp( dot, ".js" ) == 0 ){
        buf = "text/javascript";
    } else if ( strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0 ){
        buf = "text/xml";
    } else if ( strcmp(dot, ".xhtm") == 0 || strcmp(dot, ".xhtml") == 0 || strcmp(dot, ".xht") == 0 ){
        buf = "application/xhtml+xml";
    } else if ( strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".shtml") == 0 || strcmp(dot, ".hts") == 0 ){
        buf = "text/html";

    /* Images */
    } else if ( strcmp( dot, ".gif" ) == 0 ){
        buf = "image/gif";
    } else if ( strcmp( dot, ".png" ) == 0 ){
        buf = "image/png";
    } else if ( strcmp( dot, ".bmp" ) == 0 ){
        buf = "application/x-MS-bmp";
    } else if ( strcmp( dot, ".jpg" ) == 0 || strcmp( dot, ".jpeg" ) == 0 || strcmp( dot, ".jpe" ) == 0 || strcmp( dot, ".jpz" ) == 0 ){
        buf = "image/jpeg";

    /* Audio & Video */
    } else if ( strcmp( dot, ".wav" ) == 0 ){
        buf = "audio/wav";
    } else if ( strcmp( dot, ".wma" ) == 0 ){
        buf = "audio/x-ms-wma";
    } else if ( strcmp( dot, ".wmv" ) == 0 ){
        buf = "audio/x-ms-wmv";
    } else if ( strcmp( dot, ".au" ) == 0 || strcmp( dot, ".snd" ) == 0 ){
        buf = "audio/basic";
    } else if ( strcmp( dot, ".midi" ) == 0 || strcmp( dot, ".mid" ) == 0 ){
        buf = "audio/midi";
    } else if ( strcmp( dot, ".mp3" ) == 0 || strcmp( dot, ".mp2" ) == 0 ){
        buf = "audio/x-mpeg";
    } else if ( strcmp( dot, ".rm" ) == 0  || strcmp( dot, ".rmvb" ) == 0 || strcmp( dot, ".rmm" ) == 0 ){
        buf = "audio/x-pn-realaudio";
    } else if ( strcmp( dot, ".avi" ) == 0 ){
        buf = "video/x-msvideo";
    } else if ( strcmp( dot, ".3gp" ) == 0 ){
        buf = "video/3gpp";
    } else if ( strcmp( dot, ".mov" ) == 0 ){
        buf = "video/quicktime";
    } else if ( strcmp( dot, ".wmx" ) == 0 ){
        buf = "video/x-ms-wmx";
    } else if ( strcmp( dot, ".asf" ) == 0  || strcmp( dot, ".asx" ) == 0 ){
        buf = "video/x-ms-asf";
    } else if ( strcmp( dot, ".mp4" ) == 0 || strcmp( dot, ".mpg4" ) == 0 ){
        buf = "video/mp4";
    } else if ( strcmp( dot, ".mpe" ) == 0  || strcmp( dot, ".mpeg" ) == 0 || strcmp( dot, ".mpg" ) == 0 || strcmp( dot, ".mpga" ) == 0 ){
        buf = "video/mpeg";

    /* Documents */
    } else if ( strcmp( dot, ".pdf" ) == 0 ){
        buf = "application/pdf";
    } else if ( strcmp( dot, ".rtf" ) == 0 ){
        buf = "application/rtf";
    } else if ( strcmp( dot, ".doc" ) == 0  || strcmp( dot, ".dot" ) == 0 ){
        buf = "application/msword";
    } else if ( strcmp( dot, ".xls" ) == 0  || strcmp( dot, ".xla" ) == 0 ){
        buf = "application/msexcel";
    } else if ( strcmp( dot, ".hlp" ) == 0  || strcmp( dot, ".chm" ) == 0 ){
        buf = "application/mshelp";
    } else if ( strcmp( dot, ".swf" ) == 0  || strcmp( dot, ".swfl" ) == 0 || strcmp( dot, ".cab" ) == 0 ){
        buf = "application/x-shockwave-flash";
    } else if ( strcmp( dot, ".ppt" ) == 0  || strcmp( dot, ".ppz" ) == 0 || strcmp( dot, ".pps" ) == 0 || strcmp( dot, ".pot" ) == 0 ){
        buf = "application/mspowerpoint";

    /* Binary & Packages */
    } else if ( strcmp( dot, ".zip" ) == 0 ){
        buf = "application/zip";
    } else if ( strcmp( dot, ".rar" ) == 0 ){
        buf = "application/x-rar-compressed";
    } else if ( strcmp( dot, ".gz" ) == 0 ){
        buf = "application/x-gzip";
    } else if ( strcmp( dot, ".jar" ) == 0 ){
        buf = "application/java-archive";
    } else if ( strcmp( dot, ".tgz" ) == 0  || strcmp( dot, ".tar" ) == 0 ){
        buf = "application/x-tar";
    } else {
        buf = "application/octet-stream";
    }
    strcpy(ret, buf);
}

/**
 * Log message
 *
 */
static int WriteLog( const char *message )
{
    if ( !g_is_log )
    {
        fprintf(stderr, "%s", message);
        return 0;
    }
    if ( g_log_fd == 0 )
    {
        char g_log_path[2000];
        getcwd(g_log_path, sizeof(g_log_path));
        strcat(g_log_path,"/");
        strcat(g_log_path,LOG_PATH);
        
        if ( (g_log_fd = open(g_log_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1 )
        {
            perror("open log file error");
            return -1;
        }
    }
    
    if (write(g_log_fd, message, strlen(message)) == -1)
    {
        perror("write log error");
        return -1;
    }

    return 0;
}

/**
 * is_dir - check file is directory
 *
 */
static int is_dir(const char *filename){
    struct stat buf;
    if ( stat(filename, &buf) < 0 ){
        return -1;
    }
    if (S_ISDIR(buf.st_mode)){
        return 1;
    }
    return 0;
}
#endif