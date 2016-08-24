/*
 * =====================================================================================
 *
 *       Filename:  xcap_common.c
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

#include "xcap_common.h"

#define VOS_XCAP_Malloc(ulSize)					VOS_Malloc((ulSize), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Calloc(ulSize)					VOS_Calloc((ulSize), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Realloc(pstrads,ulSize)		VOS_Realloc((pstrads), (ulSize), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Remset(pstrads)				VOS_Remset((pstrads), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_MemcatEx(pstrA,pstrB,ulSize)	VOS_MemcatEx((pstrA), (pstrB), (ulSize), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_MemcatEx2(pstrA,pstrB,ulSize)	VOS_MemcatEx2((pstrA), (pstrB), (ulSize), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_MemSize(pstrads)				VOS_Memsize((pstrads), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Memcls(pstrads)				VOS_Memcls((pstrads), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Memcat(pstrA,pstrB)			VOS_Memcat((pstrA), (pstrB), (XCAP_MEM_HEAD_KEY_URL_XCAP))
#define VOS_XCAP_Free(pstrads)					VOS_Free((pstrads), XCAP_MEM_HEAD_KEY_URL_XCAP)
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
 *         Name:  xcap_get_response_status_code
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_get_response_status_code(VOS_UINT16 Code, STATUS_CODE ** pCode)
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
 *  Description:  �õ���ǰϵͳʱ����ļ�ʱ��,��ת��Ϊhttpͷ��ʱ���ʽ
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
 *  Description:  ��ʼ�� ͷ��ڵ�
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
 *  Description:  ����xcap �ַ���
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
 *  Description:  ����xcap �ַ���
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
 *  Description:  ����xcap �ַ���
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
	XCAP_PrintInfo(__FILE__, __LINE__, " Method:%s URL:%s Ver:%s",pMsgInfo->Req_head.Method,
		pMsgInfo->Req_head.Request_URI,
		pMsgInfo->Req_head.Request_version);
	
	for (uIndex = 0; uIndex <pMsgInfo->fields_num; uIndex++)
	{
		XCAP_PrintInfo("", CPSS_PRINTF_BUFFER,"  %s%s", pMsgInfo->Req_head_fields[uIndex].Key, pMsgInfo->Req_head_fields[uIndex].Value);
	}
	if (NULL != pMsgInfo->pstrReqBody)
	{
		XCAP_PrintInfo("", CPSS_PRINTF_BUFFER, "  Request Body:%s", pMsgInfo->pstrReqBody);
	}
}

/* ===  FUNCTION  ==============================================================
*         Name:  xcap_analyzing_info
*  Description:  ����xcap �ַ���
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_VOID xcap_print_response(pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32 uIndex = 0;

	if (NULL == pxCap_Response)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap printf requet pMsg is NULL");
		return;
	}
	XCAP_PrintInfo(__FILE__, __LINE__, " Ver:%s Code:%d", pxCap_Response->Res_head.Request_version,
		pxCap_Response->Res_head.StatueCode);

	for (uIndex = 0; uIndex <pxCap_Response->fields_num; uIndex++)
	{
		XCAP_PrintInfo("", CPSS_PRINTF_BUFFER, "  %s%s", 
			pxCap_Response->Res_head_fields[uIndex].Key, 
			pxCap_Response->Res_head_fields[uIndex].Value);
	}
}

/* ===  FUNCTION  ==============================================================
*         Name:  xcap_document_select
*  Description:  ��xcap ����������url��Ϣ
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_server_select(XCAP_MSG_MANAGE *pXcap_Msg_Mgr)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32		ulBodysize = 0;
	VOS_CHAR *		pstrFileBuffer = NULL;
	VOS_CHAR *		pstrFileBufferTmp = NULL;
	VOS_CHAR *		pstrTemp = NULL;
	VOS_CHAR *		pstrname = NULL;
	if (NULL == pXcap_Msg_Mgr)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "document select parameter error");
		return uRet;
	}
	if (0 == pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[0])
	{
		XCAP_PrintErr(__FILE__, __LINE__, "document select path is error");
		return uRet;
	}
	if (0 == pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[1])
	{
		VOS_Strcpy(&pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[1], "memory\\index.html");
	}

	pstrname = strrchr(pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI, '?');
	if (NULL != pstrname)
	{
		VOS_Strcpy(pXcap_Msg_Mgr->xCap_Respone_Info.strContentType, pstrname);
	}
	pstrFileBufferTmp = cpss_get_file_data(pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI);
	if (NULL == pstrFileBufferTmp)
	{
		goto ERR_EXIT;
	}
	pstrTemp = xcap_find_fields(&pXcap_Msg_Mgr->xCap_Request_Info, XCAP_REQ_FIELDE_ACCEPT);
	if (NULL != pstrTemp && NULL != VOS_Strstr(pstrTemp, "text/html"))
	{
		pstrFileBuffer = VOS_XCAP_MemcatEx(pstrFileBuffer, XCAP_DOC_BODY, VOS_Strlen(XCAP_DOC_BODY));
		if (NULL == pstrFileBuffer)
		{
			goto ERR_EXIT;
		}
		ulBodysize = VOS_XCAP_MemSize(pstrFileBufferTmp);
		pstrFileBuffer = VOS_XCAP_MemcatEx(pstrFileBuffer, pstrFileBufferTmp, ulBodysize);
		if (NULL == pstrFileBuffer)
		{
			goto ERR_EXIT;
		}
		ulBodysize += VOS_Strlen(XCAP_DOC_BODY);
		VOS_XCAP_Free(pstrFileBufferTmp);
		pstrFileBufferTmp = NULL;
	}
	else
	{
		pstrFileBuffer = (VOS_CHAR*)VOS_XCAP_Remset(pstrFileBufferTmp);
		ulBodysize = VOS_XCAP_MemSize(pstrFileBuffer);
	}
	goto OK_EXIT;
ERR_EXIT:
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", XCAP_DOC_BODY, XCAP_HTML_BEGIN, XCAP_HEAD_BEGIN);
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", "title name", XCAP_HEAD_END, XCAP_BODY_BEGIN);
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", "<center><h1>404 Not Found</h1></center><hr><center>Powered by Jance</center>", XCAP_BODY_END, XCAP_HTML_END);
	ulBodysize = VOS_Strlen(pstrFileBuffer);
OK_EXIT:
	pXcap_Msg_Mgr->xCap_Respone_Info.ulBodysize = ulBodysize;
	pXcap_Msg_Mgr->xCap_Respone_Info.pstrBody = pstrFileBuffer;
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  xcap_document_select
*  Description:  ��xcap ����������url��Ϣ
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_document_select(XCAP_MSG_MANAGE *pXcap_Msg_Mgr)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32		ulBodysize = 0;
	VOS_CHAR *		pstrFileBuffer = NULL;
	VOS_CHAR *		pstrFileBufferTmp = NULL;
	VOS_CHAR *		pstrTemp = NULL;
	VOS_CHAR *		pstrname = NULL;
	if (NULL == pXcap_Msg_Mgr)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "document select parameter error");
		return uRet;
	}
	if (0 == pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[0])
	{
		XCAP_PrintErr(__FILE__, __LINE__, "document select path is error");
		return uRet;
	}
	if (0 == pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[1])
	{
		VOS_Strcpy(&pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI[1], "index.html");
	}
	pstrname = strrchr(pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI, '.');
	if (NULL != pstrname)
	{
		VOS_Strcpy(pXcap_Msg_Mgr->xCap_Respone_Info.strContentType, pstrname);
	}
	pstrFileBufferTmp = cpss_get_file_data(pXcap_Msg_Mgr->xCap_Request_Info.Req_head.Request_URI);
	if (NULL == pstrFileBufferTmp)
	{
		goto ERR_EXIT;
	}
	pstrTemp = xcap_find_fields(&pXcap_Msg_Mgr->xCap_Request_Info, XCAP_REQ_FIELDE_ACCEPT);
	if (NULL != pstrTemp && NULL != VOS_Strstr(pstrTemp,"text/html"))
	{
		pstrFileBuffer = VOS_XCAP_MemcatEx(pstrFileBuffer, XCAP_DOC_BODY, VOS_Strlen(XCAP_DOC_BODY));
		if (NULL == pstrFileBuffer)
		{
			goto ERR_EXIT;
		}
		ulBodysize = VOS_XCAP_MemSize(pstrFileBufferTmp);
		pstrFileBuffer = VOS_XCAP_MemcatEx(pstrFileBuffer, pstrFileBufferTmp, ulBodysize);
		if (NULL == pstrFileBuffer)
		{
			goto ERR_EXIT;
		}
		ulBodysize += VOS_Strlen(XCAP_DOC_BODY);
		VOS_XCAP_Free(pstrFileBufferTmp);
		pstrFileBufferTmp = NULL;
	}
	else
	{
		pstrFileBuffer = (VOS_CHAR*)VOS_XCAP_Remset(pstrFileBufferTmp);
		ulBodysize = VOS_XCAP_MemSize(pstrFileBuffer);
	}
	goto OK_EXIT;
ERR_EXIT:
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", XCAP_DOC_BODY, XCAP_HTML_BEGIN, XCAP_HEAD_BEGIN);
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", "title name", XCAP_HEAD_END, XCAP_BODY_BEGIN);
	VOS_PrintBuffer(&pstrFileBuffer, "%s%s%s", "<center><h1>404 Not Found</h1></center><hr><center>Powered by Jance</center>", XCAP_BODY_END, XCAP_HTML_END);
	ulBodysize = VOS_Strlen(pstrFileBuffer);
OK_EXIT:
	pXcap_Msg_Mgr->xCap_Respone_Info.ulBodysize = ulBodysize;
	pXcap_Msg_Mgr->xCap_Respone_Info.pstrBody = pstrFileBuffer;
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  xcap_root
*  Description:  xcap ·����
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_root(pXCAP_MSG_MANAGE pMsgMgr)
{
	VOS_UINT32		uRet = VOS_ERR;
	pXCAP_REQUEST	pMsgInfo = NULL;
	pXCAP_RESPONSE	pRespone_Info = NULL;

	if (NULL == pMsgMgr)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap msg mgr null");
		return uRet;
	}
	pMsgInfo = &pMsgMgr->xCap_Request_Info;
	pRespone_Info = &pMsgMgr->xCap_Respone_Info;
	switch (pMsgInfo->Req_mothod)
	{
	case XCAP_REQ_GET:
		uRet = xcap_server_select(pMsgMgr);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "xcap get documet data failed");
			return uRet;
		}
		if (NULL == pRespone_Info->pstrBody)
		{
			pRespone_Info->Res_head.StatueCode = XCAP_RES_CODE_404;
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
	return uRet;
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
		XCAP_PrintErr(__FILE__,__LINE__,"req method not allow");
		uRet = VOS_ERR;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  ����xcap �ַ���
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_analyzing_buffer(pXCAP_REQUEST pReqInfo, VOS_CHAR * pstrInput, VOS_UINT32 ulBodySize)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_UINT32	ulReqBodySize = 0;
	VOS_CHAR	strLastchar = 0;
	VOS_CHAR	uFieldsNum = 0;
	VOS_CHAR *	pstrTemp = NULL;
	VOS_CHAR *	pstrTemp1 = NULL;
	VOS_CHAR *	pstrInfo = NULL;
	VOS_CHAR	uLength = 0;
	VOS_CHAR	pbuf[CPSS_MSG_BUFFER_SIZE] = {0};//, VOS_STRING pline, VOS_INT32 skip;


	pstrInfo = pstrInput;
	strLastchar = pstrInput[ulBodySize - 1];
	pstrInput[ulBodySize-1] = 0;
	while(0 != pstrInfo[0])
	{
		BZERO(pbuf, CPSS_MSG_BUFFER_SIZE);
		pstrInfo = cpss_getline(pstrInfo, pbuf, VOS_FALSE);
		cpss_trim_right(pbuf, CPSS_SEP_BUFF);
		if (0 ==pbuf[0])
		{
			if (ulBodySize == (VOS_UINT32)(pstrInfo+1 - pstrInput))
			{
				break;
			}
			if (ulBodySize > (VOS_UINT32)(pstrInfo + 1 - pstrInput))
			{
				continue;
			}
			XCAP_PrintInfo(__FILE__, __LINE__, "get request body size[%d] is not correct %d: %d:%d",
				ulReqBodySize, pstrInfo - pstrInput, ulBodySize);
			return uRet;
		}
		uFieldsNum = xcap_analyzing_fields(pReqInfo, pbuf);
		if (XCAP_FIELDS_HOST == uFieldsNum)
		{
			uLength = VOS_Strlen(g_fields_req[uFieldsNum].m_fields.Key);
			pstrTemp = VOS_Strstr(pbuf + uLength, ":");
			if (NULL != pstrTemp)
			{
				VOS_Strncpy(pReqInfo->strHost, pbuf + uLength, pstrTemp - pbuf - uLength);
				pReqInfo->uPort = atoi(pstrTemp + 1);
			}
			continue;
		}
		if (VOS_OK == uFieldsNum)
		{
			continue;
		}
		
		
		pstrTemp = VOS_Strstr(pbuf, "/");
		pstrTemp1 = VOS_Strstr(pbuf, " HTTP");
		if (NULL != pstrTemp && NULL != pstrTemp1)
		{
			VOS_Strncpy(pReqInfo->Req_head.Method, pbuf, pstrTemp - pbuf);
			cpss_trim(pReqInfo->Req_head.Method, " ");
			VOS_Strncpy(pReqInfo->Req_head.Request_URI, pstrTemp, pstrTemp1 - pstrTemp);
			VOS_Strcpy(pReqInfo->Req_head.Request_version, pstrTemp1);
			continue;
		}
		if (0 == VOS_Strcmp(pReqInfo->Req_head.Method, "POST"))
		{
			if (0 != (ulBodySize - (pstrInfo + 1 - pstrInput)))
			{
				XCAP_PrintInfo(__FILE__, __LINE__, "get request body size[%d] is not correct %d: %d",
					ulBodySize, pstrInfo - pstrInput);
				return uRet;
			}
			pstrTemp = xcap_find_fields(pReqInfo, XCAP_REQ_FIELDE_CONTENT_LENGTH);
			if (NULL == pstrTemp)
			{
				return uRet;
			}
			ulReqBodySize = atoi(pstrTemp);
			if (ulReqBodySize<0 || ulReqBodySize > (VOS_Strlen(pbuf)+1))
			{
				XCAP_PrintInfo(__FILE__, __LINE__, "get request body size[%d] is not correct %d: %d",
					ulReqBodySize, pstrInfo - pstrInput);
				return uRet;
			}
			pReqInfo->pstrReqBody = cpss_trim_left(pstrInfo - ulReqBodySize, CPSS_SEP_BUFF);
			if (NULL == pReqInfo->pstrReqBody)
			{
				XCAP_PrintInfo(__FILE__, __LINE__, "get request body failed");
				return uRet;
			}
			continue;
		}
		XCAP_PrintInfo(__FILE__, __LINE__, "get fields %s is failed", pstrInfo);
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
 *  Description:  ����xcap �ַ���
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
*  Description:  ����xcap �ַ���
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_CHAR* xcap_get_content_type(const VOS_CHAR *name){
	char *dot, *buf;
	if (NULL == name)
	{
		return "text/html";
	}
	dot = strrchr(name, '.');

	/* Text */
	if (strcmp(dot, ".txt") == 0){
		buf = "text/plain";
	}
	else if (strcmp(dot, ".css") == 0){
		buf = "text/css";
	}
	else if (strcmp(dot, ".js") == 0){
		buf = "text/javascript";
	}
	else if (strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0){
		buf = "text/xml";
	}
	else if (strcmp(dot, ".xhtm") == 0 || strcmp(dot, ".xhtml") == 0 || strcmp(dot, ".xht") == 0){
		buf = "application/xhtml+xml";
	}
	else if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".shtml") == 0 || strcmp(dot, ".hts") == 0){
		buf = "text/html";

		/* Images */
	}
	else if (strcmp(dot, ".gif") == 0){
		buf = "image/gif";
	}
	else if (strcmp(dot, ".png") == 0){
		buf = "image/png";
	}
	else if (strcmp(dot, ".bmp") == 0){
		buf = "application/x-MS-bmp";
	}
	else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".jpe") == 0 || strcmp(dot, ".jpz") == 0){
		buf = "image/jpeg";
		/* Audio & Video */
	}
	else if (strcmp(dot, ".wav") == 0){
		buf = "audio/wav";
	}
	else if (strcmp(dot, ".wma") == 0){
		buf = "audio/x-ms-wma";
	}
	else if (strcmp(dot, ".wmv") == 0){
		buf = "audio/x-ms-wmv";
	}
	else if (strcmp(dot, ".au") == 0 || strcmp(dot, ".snd") == 0){
		buf = "audio/basic";
	}
	else if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0){
		buf = "audio/midi";
	}
	else if (strcmp(dot, ".mp3") == 0 || strcmp(dot, ".mp2") == 0){
		buf = "audio/x-mpeg";
	}
	else if (strcmp(dot, ".rm") == 0 || strcmp(dot, ".rmvb") == 0 || strcmp(dot, ".rmm") == 0){
		buf = "audio/x-pn-realaudio";
	}
	else if (strcmp(dot, ".avi") == 0){
		buf = "video/x-msvideo";
	}
	else if (strcmp(dot, ".3gp") == 0){
		buf = "video/3gpp";
	}
	else if (strcmp(dot, ".mov") == 0){
		buf = "video/quicktime";
	}
	else if (strcmp(dot, ".wmx") == 0){
		buf = "video/x-ms-wmx";
	}
	else if (strcmp(dot, ".asf") == 0 || strcmp(dot, ".asx") == 0){
		buf = "video/x-ms-asf";
	}
	else if (strcmp(dot, ".mp4") == 0 || strcmp(dot, ".mpg4") == 0){
		buf = "video/mp4";
	}
	else if (strcmp(dot, ".mpe") == 0 || strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpg") == 0 || strcmp(dot, ".mpga") == 0){
		buf = "video/mpeg";

		/* Documents */
	}
	else if (strcmp(dot, ".pdf") == 0){
		buf = "application/pdf";
	}
	else if (strcmp(dot, ".rtf") == 0){
		buf = "application/rtf";
	}
	else if (strcmp(dot, ".doc") == 0 || strcmp(dot, ".dot") == 0){
		buf = "application/msword";
	}
	else if (strcmp(dot, ".xls") == 0 || strcmp(dot, ".xla") == 0){
		buf = "application/msexcel";
	}
	else if (strcmp(dot, ".hlp") == 0 || strcmp(dot, ".chm") == 0){
		buf = "application/mshelp";
	}
	else if (strcmp(dot, ".swf") == 0 || strcmp(dot, ".swfl") == 0 || strcmp(dot, ".cab") == 0){
		buf = "application/x-shockwave-flash";
	}
	else if (strcmp(dot, ".ppt") == 0 || strcmp(dot, ".ppz") == 0 || strcmp(dot, ".pps") == 0 || strcmp(dot, ".pot") == 0){
		buf = "application/mspowerpoint";
		/* Binary & Packages */
	}
	else if (strcmp(dot, ".zip") == 0){
		buf = "application/zip";
	}
	else if (strcmp(dot, ".rar") == 0){
		buf = "application/x-rar-compressed";
	}
	else if (strcmp(dot, ".gz") == 0){
		buf = "application/x-gzip";
	}
	else if (strcmp(dot, ".jar") == 0){
		buf = "application/java-archive";
	}
	else if (strcmp(dot, ".tgz") == 0 || strcmp(dot, ".tar") == 0){
		buf = "application/x-tar";
	}
	else {
		buf = "application/octet-stream";
	}
	return buf;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_head
 *  Description:  ����xcap �ַ���
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_head(pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_CHAR	strTimebuffer[INTERNET_RFC1123_BUFSIZE] = {0};
	VOS_Strcpy(pxCap_Response->Res_head.Request_version, "HTTP/1.1");
	if (0 == pxCap_Response->Res_head.StatueCode)
	{
		pxCap_Response->Res_head.StatueCode = XCAP_RES_CODE_203;
	}
	xcap_set_fildes(XCAP_RES_FIELDE_SERVER, pxCap_Response, "Jance Xcap Server");
	xcap_set_fildes(XCAP_RES_FIELDE_CONTENT_TYPE, pxCap_Response, 
		xcap_get_content_type(pxCap_Response->strContentType));//"text/html";charset=ISO-8859-1
	xcap_set_fildes(XCAP_RES_FIELDE_CONTENT_LANGUAGE, pxCap_Response, "zh-CN");
	xcap_set_fildes(XCAP_RES_FIELDE_CONNECTIN, pxCap_Response, "keep-alive");
	xcap_set_fildes(XCAP_RES_FIELDE_DATE,      pxCap_Response, xcap_get_http_date(strTimebuffer, NULL));

	uRet = VOS_OK;
	return uRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  xcap_set_body_size
*  Description:  ����body��size
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_set_body_size(pXCAP_RESPONSE pxCap_Response, VOS_UINT32 ulBodySize)
{
	VOS_UINT32		ulRet = VOS_ERR;
	VOS_CHAR		strSize[64] = { 0 };
	if (NULL == pxCap_Response)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "get body size input is NULL");
		return ulRet;
	}
	if (0 > ulBodySize)
	{
		ulBodySize = 0;
	}
	sprintf(strSize, "%d", ulBodySize);
	xcap_set_fildes(XCAP_RES_FIELDE_CONTENT_LENGTH, pxCap_Response, strSize);
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
*         Name:  xcap_set_responce_head
*  Description:  ����body��head����
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_set_responce_head(pXCAP_RESPONSE pxCap_Response, VOS_CHAR** pstrDataBuffer)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_UINT32  uIndex = 0;
	if (NULL == pstrDataBuffer ||
		NULL == pxCap_Response)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "input is NULL");
		return uRet;
	}
	while (uIndex <= pxCap_Response->fields_num)
	{
		if (pxCap_Response->Res_head_fields[uIndex].Key[0] == 0 ||
			pxCap_Response->Res_head_fields[uIndex].Value[0] == 0)
		{
			uIndex++;
			continue;
		}
		VOS_PrintBuffer(pstrDataBuffer, "%s%s\r\n",
			pxCap_Response->Res_head_fields[uIndex].Key,
			pxCap_Response->Res_head_fields[uIndex].Value);
		uIndex++;
	}
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_xcap_body
 *  Description:  ����xcap �ַ���
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_set_responce_body(pCPSS_MSG pMsgInfo, pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32		uRet = VOS_ERR;
	VOS_UINT32		ulBodysize = 0;
	VOS_CHAR	*	pstrDataBuffer = NULL;
	VOS_CHAR		stTemp =0;
	CPSS_MSG		stuSendMsgInfo = { 0 };


	if (NULL == pxCap_Response)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"input is NULL");
		return uRet;
	}
	uRet = xcap_get_response_status_code(pxCap_Response->Res_head.StatueCode, 
		&pxCap_Response->pCode);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is failed");
		return uRet;
	}
	if (NULL == pxCap_Response->pCode)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is error");
		return uRet;
	}
	uRet = xcap_set_body_size(pxCap_Response, pxCap_Response->ulBodysize);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap_set_body_size is error");
		return uRet;
	}

	VOS_PrintBuffer(&pstrDataBuffer,"%s %d %s\r\n",
		pxCap_Response->Res_head.Request_version,
		pxCap_Response->pCode->uCode, 
		pxCap_Response->pCode->phrase);
	uRet = xcap_set_responce_head(pxCap_Response, &pstrDataBuffer);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get responce code is failed");
		return uRet;
	}
	xcap_print_response(pxCap_Response);
	VOS_PrintBuffer(&pstrDataBuffer, "\r\n");
	pstrDataBuffer = VOS_XCAP_MemcatEx2(pstrDataBuffer, pxCap_Response->pstrBody, pxCap_Response->ulBodysize);
	if (NULL == pstrDataBuffer)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "get send body is failed");
		return uRet;
	}
	ulBodysize = VOS_XCAP_MemSize(pstrDataBuffer);
	uRet = cpss_copy_msg(pMsgInfo, &stuSendMsgInfo);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "get send msg failed");
		return uRet;
	}
	uRet = cpss_send_data(&stuSendMsgInfo, pstrDataBuffer, ulBodysize,
						VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "send data failed");
	}
	VOS_PrintBufferRelease(pstrDataBuffer);
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  set_xcap_head
*  Description:  ����xcap �ַ���
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 xcap_responce_data(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR;
	CPSS_MSG		MsgInfo;

	BZERO(&MsgInfo, sizeof(CPSS_MSG));

	MsgInfo.Body.msghead.stSrcProc.ulCpuID = cpss_get_cpuid_pid(
		CPSS_SYSTEM_TYPE_XCAP, CPSS_SUBSYS_TYPE_MOCLI, CPSS_GET_TYPE_CPUID);
	if (0 == MsgInfo.Body.msghead.stSrcProc.ulCpuID)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "src cpuid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stSrcProc.ulPID = cpss_get_cpuid_pid(
		CPSS_SYSTEM_TYPE_XCAP, CPSS_SUBSYS_TYPE_MOCLI, CPSS_GET_TYPE_PID);
	if (0 == MsgInfo.Body.msghead.stSrcProc.ulPID)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "src pid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stDstProc.ulCpuID = cpss_get_cpuid_pid(
		CPSS_SYSTEM_TYPE_MONEY, CPSS_SUBSYS_TYPE_MONEY, CPSS_GET_TYPE_CPUID);
	if (0 == MsgInfo.Body.msghead.stDstProc.ulCpuID)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "dst cpuid is error");
		return uRet;
	}
	MsgInfo.Body.msghead.stDstProc.ulPID = cpss_get_cpuid_pid(
		CPSS_SYSTEM_TYPE_MONEY, CPSS_SUBSYS_TYPE_MONEY, CPSS_GET_TYPE_PID);
	if (0 == MsgInfo.Body.msghead.stDstProc.ulPID)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "dst pid is error");
		return uRet;
	}

	MsgInfo.Body.msghead.ulParentMsgID = pMsgInfo->ulMsgID;
	//MsgInfo.Body.msghead.uType = uType;

	//uRet = money_send_data(&MsgInfo, pstuBuffer, uBufLen, VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "send udp data error");
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
	pXCAP_MSG_MANAGE pXcap_Msg_Mgr = (pXCAP_MSG_MANAGE)pMsgInfo->pXcapMgr;
	if (NULL == pXcap_Msg_Mgr)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"xcap request is msg mgr is NULL");
		return ulRet;
	}
	pxCap_Respone_Info = &pXcap_Msg_Mgr->xCap_Respone_Info;
	if (NULL == pxCap_Respone_Info->pstrBody)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap body is error");
		return ulRet;
	}
	ulRet = xcap_set_head(pxCap_Respone_Info);
	if (VOS_OK != ulRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap_set_head is error");
		return ulRet;
	}
	ulRet = xcap_set_responce_body(pMsgInfo, pxCap_Respone_Info);
	if (VOS_OK != ulRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap_set_body_size is error");
		return ulRet;
	}
	if (NULL != pXcap_Msg_Mgr)
	{
		VOS_XCAP_Free(pXcap_Msg_Mgr);
		pMsgInfo->pXcapMgr = NULL;
	}
	return VOS_OK;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_user_body
 *  Description:  ����xcap �ַ���
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
 *         Name:  send_xcap_URL
 *  Description:  ��xcap ����������url��Ϣ
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_request_URL(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_UINT32		uBuffLen = 0;
	//VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	//pXCAP_SER_MGR    pstuXcapSerMgr = NULL;
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
	pMsgInfo->pXcapMgr = pXcap_Msg_Mgr;

	uRet = xcap_analyzing_buffer(&pXcap_Msg_Mgr->xCap_Request_Info, 
		pMsgInfo->Body.strDataBuf,
		pMsgInfo->Body.msghead.ulMsgLength);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "analyzing xcap buffer error");
		goto ERR_PROC;
	}
	uRet = xcap_request_check(&pXcap_Msg_Mgr->xCap_Request_Info);
	if (VOS_OK != uRet )
	{
		XCAP_PrintErr(__FILE__, __LINE__, "check xcap buffer error");
		goto ERR_PROC;
	}
	pXcap_Msg_Mgr->xCap_Request_Info.ulMsgID = pMsgInfo->ulMsgID;
	//pXcap_Msg_Mgr->xCap_Respone_Info.ulMsgID = pMsgInfo->ulMsgID;
	
	xcap_print_request(&pXcap_Msg_Mgr->xCap_Request_Info);
	uRet = xcap_root(pXcap_Msg_Mgr);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "check xcap buffer error");
		goto ERR_PROC;
	}
	uRet = xcap_responce_proc(pMsgInfo);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "xcap responce error");
		goto ERR_PROC;
	}
	return VOS_OK;
ERR_PROC:
	if (NULL != pXcap_Msg_Mgr)
	{
		VOS_XCAP_Free(pXcap_Msg_Mgr);
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
 *  Description:  ����telnet������
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
 *  Description:  ����xcap �ַ���
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 get_xcap_file_no_find(pXCAP_RESPONSE pxCap_Response)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_PrintBuffer(&pxCap_Response->pstrBody, "%s"
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

// ����socket ��ʼ���� �������� ���տͻ������� ����httpЭ�� �����ļ����ݸ��ͻ���

#define HTTP_PORT "2010"
#define MAX_CONNECTION 10
#define DOCUMENT_ROOT "www"
#define LOG_PATH "log/access.log"

void parser(char *s,char res[][255],char host[][255]);
static char *strtoupper( char *s );
static long filesize(const char *filename);
static void mime_content_type( const char *name, char *ret );

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
            
             /** Ŀ¼�б� **/
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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "socket.h"
#include "http.h"

#define MAX_RECV_SIZE    1440//Ӳ���������Ľ����ֽ���

char g_host[URL_LEN];
char g_ip[URL_LEN + 1];//ip/����
char g_port[5 + 1];
char g_buf_send[4 * 1024];//���������ݴ���
char g_buf_recv[10 * 1024];//���������ݴ���

BreakPoint_ST g_break_point;
/*

   ����:�ж϶ϵ���Ч��,����У��url�Ƿ�һ��
   ����:
   ����:
   >0---------��Ч,�������ļ���С
   -1----------��Ч
 */

int Get_Breakpoint_Available(BreakPoint_ST *breakpoint, char *url, char *file_crc)
{
	//�ж϶ϵ��Ƿ���Ч,���������ļ�У����
	if ((memcmp(breakpoint->url, url, strlen(url)) == 0) && (breakpoint->recv_size == MAX_RECV_SIZE))
		return breakpoint->download_size;
	else
	{
		return -1;
	}
}

/*

   48.����:�ж�Ҫ�����ļ��Ƿ���ڶϵ�

   49.����:

   50.filename---Ҫ���ص��ļ���

   51.file_crc----���������������ļ���У����

   52.����:

   53.0---------�޶ϵ�

   54.>0--------�жϵ�,�������ļ���С

   55.*/

   56.int Get_Breakpoint(char *url, char *filename, char *file_crc)

   57.{

	58.    char filename_bp[64];

	59.    int fd = -1;

	60.    int ret;

	61.    BreakPoint_ST break_point;

	62.

		63.    //�ϵ��ļ��� filename+bp

		64.    sprintf(filename_bp, "%s.bp", filename);

	65.

		66.    //����Ƿ����filename�ϵ��ļ�

		67.    fd = open(filename_bp, O_RDONLY, S_IRUSR | S_IWUSR);

	68.    if (fd == -1)

		69.    {

		70.        #ifdef DEBUG_HTTP

			71.        printf("no exsit %s\n", filename_bp);

		72.        #endif

			73.        return 0;

		74.    }

	75.

		76.    //���ڶϵ�

		77.    ret = read(fd, &break_point, sizeof(break_point));

	78.    if (ret != sizeof(break_point))

		79.    {

		80.        perror("ERR:Get_Breakpoint read");

		81.        exit(-1);

		82.    }

	83.

		84.    close(fd);

	85.

		86.    //�ж϶ϵ��Ƿ���Ч

		87.    ret = Get_Breakpoint_Available(&break_point, url, file_crc);

	88.    if (ret > 0)

		89.        return ret;

	90.    else

		91.    {

		92.

			93.        printf("%s not available\n", filename_bp);

		94.        remove(filename);

		95.        remove(filename_bp);

		96.        return 0;

		97.

			98.    }

	99.}

100.

101./*

	102.����:����ϵ���Ϣ,�ļ���filename.bp

	103.����:

	104.filename---Ҫ���ص��ļ���

	105.file_crc----���������������ļ���У����

	106.����:

	107.0---------�ɹ�

	108.>0--------�жϵ�,�������ļ���С

	109.*/

	110.int Save_Breakpoint(char *url, char *filename, int download_size, char *file_crc)

	111.{

	112.    int fd;

	113.    BreakPoint_ST breakpoint;

	114.    char filename_bp[128];//�ϵ���Ϣ�ļ���������·��

	115.

		116.    sprintf(filename_bp, "%s.bp", filename);

	117.    /* ����Ŀ���ļ� */

		118.    if ((fd = open(filename_bp, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1)

		119.    {

		120.        fprintf(stderr, "Open %s Error��%s\n", filename_bp, strerror(errno));

		121.        exit(1);

		122.    }

	123.    memset(&breakpoint, 0x0, sizeof(breakpoint));

	124.    strcpy(breakpoint.url, url);

	125.    //strcpy(breakpoint.crc,file_crc);

		126.    strcpy(breakpoint.filename, filename);

	127.    breakpoint.download_size = download_size;

	128.    breakpoint.recv_size = MAX_RECV_SIZE;

	129.

		130.    //xu tioa zheng wei fen ci xie ru

		131.    if (write(fd, &breakpoint, sizeof(breakpoint)) != sizeof(breakpoint))

		132.    {

		133.        perror("ERR:Save_Breakpoint");

		134.        exit(1);

		135.    }

	136.

		137.    close(fd);

	138.

		139.    return 0;

	140.

		141.

		142.

		143.}

144.

145./*

	146.����:�����ļ�,׷��д

	147.����:

	148.����:

	149.0---------�ɹ�

	150.*/

	151.

	152.int Save_File(char *filebuf, int filelength, char *filename)

	153.{

	154.    int fd;

	155.    /* ����Ŀ���ļ�׷��д */

		156.    if ((fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) == -1)

		157.    {

		158.        fprintf(stderr, "Open %s Error��%s\n", filename, strerror(errno));

		159.        exit(1);

		160.    }

	161.    //xu tioa zheng wei fen ci xie ru

		162.    if (write(fd, filebuf, filelength) != filelength)

		163.    {

		164.        perror("ERR:Save_File");

		165.        exit(1);

		166.    }

	167.

		168.    close(fd);

	169.

		170.    return 0;

	171.

		172.

		173.}

174.

175.

176.int HTTP_GetResponseCode(void)

177.{

	178.

		179.

		180.}

181.

182. /*

	 183.����:��ȡhttp���ص�Э��ʵ�����峤��

	 184.����:

	 185.revbuf--------���յ��ķ���ֵ

	 186.����ֵ:

	 187.>=0---------����(ʵ������)�ĳ���

	 188.-1-----------���ݷ��ش���

	 189.*/

	 190.int HTTP_GetRecvLength(char *revbuf)

	 191.{

	192.    char *p1 = NULL;

	193.    int HTTP_Body = 0;//�����峤��

	194.    int HTTP_Head = 0;//HTTP Э��ͷ����

	195.

		196.

		197.    HTTP_Body = HTTP_GetContentLength(revbuf);

	198.    if (HTTP_Body == -1)

		199.        return -1;

	200.

		201.    p1 = strstr(revbuf, "\r\n\r\n");

	202.    if (p1 == NULL)

		203.        return -1;

	204.    else

		205.    {

		206.        HTTP_Head = p1 - revbuf + 4;// 4��\r\n\r\n�ĳ���

		207.        return HTTP_Body + HTTP_Head;

		208.    }

	209.

		210.

		211.}

212.

213.

214./*

	215.����:��ȡhttp���ص�Content-Length����

	216.����:

	217.revbuf--------���յ�������

	218.����ֵ:

	219.>=0---------Content-Length����

	220.-1-----------���ݷ��ش���

	221.*/

	222.int HTTP_GetContentLength(char *revbuf)

	223.{

	224.    char *p1 = NULL, *p2 = NULL;

	225.    int HTTP_Body = 0;//�����峤��

	226.

		227.    p1 = strstr(revbuf, "Content-Length");

	228.    if (p1 == NULL)

		229.        return -1;

	230.    else

		231.    {

		232.        p2 = p1 + strlen("Content-Length") + 2;

		233.        HTTP_Body = atoi(p2);

		234.        return HTTP_Body;

		235.    }

	236.

		237.}

238.

239. /*

	 240. ����:

	 241. ����:

	 242. sockfd--------���յ��ķ���ֵ

	 243. ����ֵ:

	 244. >0---------���յ�����

	 245. -1----------ʧ��

	 246. =0---------����˶Ͽ�����

	 247. ע:�ڲ����ջ���10k

	 248. */

	 249.

	 250.int HTTP_Recv(int sockfd, char *buf_recv)

	 251.{

	252.    int ret;

	253.    int recvlen = 0;

	254.    int downloadlen = 0;

	255.    //int contentlen=0;

		256.    char buf_recv_tmp[10 * 1024 + 1];

	257.

		258.    memset(buf_recv_tmp, 0x0, sizeof(buf_recv_tmp));

	259.    while (1)

		260.    {

		261.        ret = Recv(sockfd, buf_recv_tmp + recvlen, sizeof(buf_recv_tmp) - 1, 0);

		262.

			263.        if (ret <= 0)//����ʧ��

			264.        {

			265.            perror("ERR:recv fail");

			266.            return ret;

			267.        }

		268.

			269.

			270.        if (recvlen == 0)

			271.        {

			272.            #ifdef DEBUG_HTTP_RECV

				273.            printf("recv len = %d\n", ret);

			274.             printf("recv = %s\n", buf_recv_tmp);

			275.            #endif

				276.            //��ȡ��Ҫ���س���;

				277.            downloadlen = HTTP_GetRecvLength(buf_recv_tmp);

			278.

				279.

				280.            #ifdef DEBUG_HTTP_RECV

				281.            printf("downloadlen = %d\n", downloadlen);

			282.            #endif

				283.        }

		284.

			285.        recvlen += ret;

		286.        #ifdef DEBUG_HTTP_RECV

			287.        printf("total recvlen = %d\n", recvlen);

		288.        #endif

			289.

			290.        if (downloadlen == recvlen)//�������

			291.            break;

		292.

			293.

			294.    }

	295.    memcpy(buf_recv, buf_recv_tmp, downloadlen);

	296.    return recvlen;

	297.

		298.}

299.

300./*

	301.����:��ȡ����url�е��ļ���,���һ��/����ַ�

	302.����:

	303.����ֵ:

	304.0-----------�ɹ�

	305.-1----------ʧ��

	306.ע:�ڲ����ջ���10k

	307.*/

	308.

	309.int HTTP_GetFileName(char *url, char *filename)

	310.{

	311.    //��ȡurl�����һ��/�������

		312.    int len;

	313.    int i;

	314.

		315.    len = strlen(url);

	316.    for (i = len - 1; i>0; i--)

		317.    {

		318.        if (url[i] == '/')

			319.            break;

		320.    }

	321.    if (i == 0)//���ص�ַ����

		322.    {

		323.        printf("url not contain '/'\n");

		324.        return -1;

		325.    }

	326.    else

		327.    {

		328.

			329.        strcpy(filename, url + i + 1);

		330.        #ifdef DEBUG_HTTP

			331.        printf("filename=%s\n", filename);

		332.        #endif

			333.        return 0;

		334.    }

	335.}

336.

337./*

	338.����:��ȡ����url�е�·��,��һ��/����ַ�

	339.����:

	340.����ֵ:

	341.0-----------�ɹ�

	342.-1----------ʧ��

	343.ע:url ex "http://host:port/path"

	344.*/

	345.int HTTP_GetPath(char *url, char *path)

	346.{

	347.    char *p;

	348.

		349.    p = strstr(url, "http://");

	350.    if (p == NULL)

		351.    {

		352.        p = strchr(url, '/');

		353.        if (p == NULL)

			354.            return -1;

		355.        else

			356.        {

			357.            strcpy(path, p);

			358.            return 0;

			359.        }

		360.    }

	361.    else

		362.    {

		363.        p = strchr(url + strlen("http://"), '/');

		364.        if (p == NULL)

			365.            return -1;

		366.        else

			367.        {

			368.            strcpy(path, p);

			369.            return 0;

			370.        }

		371.    }

	372.

		373.}

374./*

	375.����:��ȡ����url�е�ip��port,ip֧������,�˿�Ĭ��Ϊ80

	376.����:

	377.����ֵ:

	378.1-----------����ʽ

	379.2-----------ip portʽ

	380.-1----------ʧ��

	381.ע:url ex "http://host:port/path"

	382.*/

	383.

	384.int HTTP_Get_IP_PORT(char *url, char *ip, char *port)

	385.{

	386.    char *p = NULL;

	387.    int offset = 0;

	388.    char DOMAIN_NAME[128];

	389.

		390.    p = strstr(url, "http://");

	391.    if (p == NULL)

		392.    {

		393.        offset = 0;

		394.    }

	395.    else

		396.    {

		397.        offset = strlen("http://");

		398.    }

	399.

		400.    p = strchr(url + offset, '/');

	401.    if (p == NULL)

		402.    {

		403.        printf("url:%s format error\n", url);

		404.        return -1;

		405.

			406.    }

	407.    else

		408.    {

		409.

			410.        memset(DOMAIN_NAME, 0x0, sizeof(DOMAIN_NAME));

		411.        memcpy(DOMAIN_NAME, url + offset, (p - url - offset));

		412.        p = strchr(DOMAIN_NAME, ':');

		413.        if (p == NULL)

			414.        {

			415.            strcpy(ip, DOMAIN_NAME);

			416.            strcpy(port, "80");

			417.            //printf("ip %p,port %p\n",ip,port);

				418.

				419.            #ifdef DEBUG_HTTP

				420.            printf("ip=%s,port=%s\n", ip, port);//debug info

			421.            #endif

				422.            return 1;

			423.

				424.        }

		425.        else

			426.        {

			427.            *p = '\0';

			428.

				429.            strcpy(ip, DOMAIN_NAME);

			430.            strcpy(port, p + 1);

			431.

				432.            #ifdef DEBUG_HTTP

				433.            printf("ip=%s,port=%s\n", ip, port);//debug info

			434.            #endif

				435.            return 2;

			436.

				437.        }

		438.

			439.

			440.        return 0;

		441.    }

	442.

		443.}

444.void Package_Url_Get_File(char *path, char *range)

445.{

	446.    char buf[64];

	447.    memset(g_buf_send, 0x0, sizeof(g_buf_send));

	448.    sprintf(g_buf_send, "GET %s", path);

	449.

		450.

		451.    //HTTP/1.1\r\n ǰ����Ҫһ���ո�

		452.    strcat(g_buf_send, " HTTP/1.1\r\n");

	453.    strcat(g_buf_send, "Host: ");

	454.    strcat(g_buf_send, g_host);

	455.    //strcat(g_buf_send, ":");

		456.    //strcat(g_buf_send, PORT);

		457.

		458.    sprintf(buf, "\r\nRange: bytes=%s", range);

	459.    strcat(g_buf_send, buf);

	460.    strcat(g_buf_send, "\r\nKeep-Alive: 200");

	461.    strcat(g_buf_send, "\r\nConnection: Keep-Alive\r\n\r\n");

	462.

		463.

		464.}

465.

466.int Package_Url_Get_FileSize(char *url)

467.{

	468.

		469.    memset(g_buf_send, 0x0, sizeof(g_buf_send));

	470.    sprintf(g_buf_send, "HEAD %s", url);

	471.

		472.        //HTTP/1.1\r\n ǰ����Ҫһ���ո�

		473.    strcat(g_buf_send, " HTTP/1.1\r\n");

	474.    strcat(g_buf_send, "Host: ");

	475.    strcat(g_buf_send, g_host);

	476.    //strcat(g_buf_send, ":");

		477.    //strcat(g_buf_send, PORT);

		478.    strcat(g_buf_send, "\r\nConnection: Keep-Alive\r\n\r\n");

	479.

		480.    return 0;

	481.}

482.

483.

484.int HTTP_GetFileSize(int sockfd, char *path)

485.{

	486.    int ret = -1;

	487.    char buf_recv_tmp[10 * 1024 + 1];

	488.

		489.    Package_Url_Get_FileSize(path);

	490.#ifdef DEBUG_HTTP

		491.    printf("send = %s \n", g_buf_send);

	492.#endif

		493.

		494.    Send(sockfd, g_buf_send, strlen(g_buf_send), 0);

	495.

		496.    memset(buf_recv_tmp, 0x0, sizeof(buf_recv_tmp));

	497.    ret = Recv(sockfd, buf_recv_tmp, sizeof(buf_recv_tmp) - 1, 0);

	498.#ifdef DEBUG_HTTP

		499.    printf("recv len = %d\n", ret);

	500.    printf("recv = %s\n", buf_recv_tmp);

	501.#endif

		502.    if (ret <= 0)

		503.    {

		504.        perror("ERR:recv fail GetFileSize()");

		505.        return -1;

		506.

			507.    }

	508.    ret = HTTP_GetContentLength(buf_recv_tmp);

	509.    if (ret <= 0)

		510.        return -1;

	511.    else

		512.        return ret;

	513.

		514.

		515.}

516.

517.

518.

519.

520./*

	521.����:�ֶ������ļ�

	522.����:

	523.����ֵ:

	524.>0----------�������ļ���С(�������ϴ�����)

	525.-1----------ʧ��

	526.*/

	527.int HTTP_GetFile(int sockfd, char *path, int filelength, int download_size, char *filebuf)

	528.{

	529.    int count;

	530.    char range[32];

	531.    int i;

	532.    int j = 0;//�ɹ����ش���

	533.    int ret = -1;

	534.    char *p = NULL;

	535.    int download_index;//���ؿ�ʼ����

	536.

		537.    count = (filelength%MAX_RECV_SIZE) ? (filelength / MAX_RECV_SIZE + 1) : (filelength / MAX_RECV_SIZE);

	538.

		539.    download_index = download_size / MAX_RECV_SIZE;

	540.

		541.    for (i = download_index; i<count; i++)

		542.    {

		543.        //if(i == 20)//���Զϵ�

			544.            //break;

			545.

			546.

			547.        if ((i == (count - 1)) && (filelength%MAX_RECV_SIZE))

			548.            sprintf(range, "%d-%d", i*MAX_RECV_SIZE, filelength - 1);

		549.        else

			550.            sprintf(range, "%d-%d", i*MAX_RECV_SIZE, (i + 1)*MAX_RECV_SIZE - 1);

		551.

			552.

			553.        Package_Url_Get_File(path, range);

		554.        #ifdef DEBUG_HTTP

			555.         printf("send = %s \n", g_buf_send);

		556.        #endif

			557.         Send(sockfd, g_buf_send, strlen(g_buf_send), 0);

		558.

			559.        /*���Ϊ��ȡhttp ����Э��ͷ��Э�����ܳ�,Ȼ�󶨳�����*/

			560.        memset(g_buf_recv, 0x0, sizeof(g_buf_recv));

		561.        ret = HTTP_Recv(sockfd, g_buf_recv);

		562.        if (ret < 0)

			563.            break;

		564.        if (ret == 0)//����˶Ͽ�����

			565.        {

			566.            sockfd = Socket_Connect(g_ip, g_port);

			567.             i--;

			568.            continue;

			569.        }

		570.        /*��ȡЭ��������,������filebuf��*/

			571.        p = strstr(g_buf_recv, "\r\n\r\n");

		572.        if (p == NULL)//jia ru duan dian baocun

			573.        {

			574.            printf("ERR:g_buf_recv not contain end flag\n");

			575.            break;

			576.        }

		577.         else

			578.         {

			579.             memcpy(filebuf + j*MAX_RECV_SIZE, p + 4, MAX_RECV_SIZE);

			580.             j++;

			581.

				582.         }

		583.    }

	584.    if (i == count)

		585.        return (filelength - download_size);

	586.    else

		587.        return (i*MAX_RECV_SIZE - download_size);

	588.}

589.

590./*

	591.����:HTTP�����ļ�

	592.����:

	593.����ֵ:

	594.0----------�������

	595.-1---------ʧ��

	596.-2---------�����������

	597.ע:�����ļ���bin����Ŀ¼

	598.*/

	599.int HTTP_DownloadFile(char *url, char *save_path)

	600.{

	601.    int ret;

	602.    int sockfd;

	603.    int filesize;

	604.    int download_size;

	605.    char filename[FILENAME_LEN + 1];

	606.    char filename_bp[FILENAME_LEN + 3 + 1];

	607.    char *filebuf;

	608.    char save_file_path[FILENAME_LEN + 1];//���������ļ���·��+�ļ���

	609.

		610.    char path[PATH_LEN + 1];//url�е�path

	611.

		612.    //��ȡip��port��url(url �ݲ�ʵ��,��Ҫgethostbyname linux)

		613.    ret = HTTP_Get_IP_PORT(url, g_ip, g_port);

	614.    if (ret == -1)

		615.        return -1;

	616.    else

		617.    {

		618.        sprintf(g_host, "%s:%s", g_ip, g_port);

		619.    }

	620.    //��ȡ�����ļ���

		621.    ret = HTTP_GetFileName(url, filename);

	622.    if (ret == -1)

		623.        return -1;

	624.

		625.    ret = HTTP_GetPath(url, path);

	626.    if (ret == -1)

		627.        return -1;

	628.    //sleep(3);//debug info

		629.    //��������

		630.    sockfd = Socket_Connect(g_ip, g_port);

	631.

		632.    //��ȡ�����ļ��ܴ�С

		633.    filesize = HTTP_GetFileSize(sockfd, path);

	634.    if (filesize == -1)

		635.        return -1;

	636.    //#ifdef DEBUG_HTTP

		637.    printf("http need download size %d\n", filesize);

	638.    //#endif

		639.    //malloc����洢�ļ��ռ�

		640.    filebuf = (char *)malloc(filesize);

	641.    if (filebuf == NULL)

		642.    {

		643.        perror("malloc filebuf fail");

		644.        return -1;

		645.    }

	646.    else

		647.        memset(filebuf, 0x0, filesize);

	648.

		649.    download_size = Get_Breakpoint(url, filename, NULL);

	650.    #ifdef DEBUG_HTTP

		651.    printf("breakpoint download_size=%d\n", download_size);//debug info

	652.    sleep(3);//debug info

	653.    #endif

		654.    //�ֶ������ļ�

		655.    ret = HTTP_GetFile(sockfd, path, filesize, download_size, filebuf);

	656.    Close(sockfd);

	657.    if (ret < 0)

		658.    {

		659.        free(filebuf);

		660.        return -1;

		661.    }

	662.    else

		663.    {

		664.

			665.        sprintf(save_file_path, "%s%s", save_path, filename);

		666.

			667.        #ifdef DEBUG_HTTP

			668.        printf("save_path=%s\n", save_path);

		669.        printf("filename=%s\n", filename);

		670.        printf("save_file_path=%s\n", save_file_path);

		671.        printf("download_size = %d\n", ret);

		672.        #endif

			673.        Save_File(filebuf, ret, save_file_path);

		674.        free(filebuf);

		675.        if ((ret + download_size) == filesize)//ȫ���������

			676.        {

			677.            sprintf(filename_bp, "%s.bp", filename);

			678.            remove(filename_bp);

			679.

				680.     printf("download success\n");

			681.            return 0;

			682.        }

		683.        else//�����������

			684.        {

			685.            printf("part download success\n");

			686.            //����ϵ���Ϣ

				687.            Save_Breakpoint(url, save_file_path, ret + download_size, NULL);

			688.            return -2;

			689.        }

		690.    }

	691.

		692.}


#endif