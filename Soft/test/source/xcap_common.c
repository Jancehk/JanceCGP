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
#include "xcap_common.h"
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_get_head
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_get_value(VOS_CHAR * Des,VOS_CHAR * Src, VOS_CHAR key)
{
	VOS_UINT32 m_pos=0;
	if (NULL == Des || NULL == Src)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get value Des Error");
		return m_pos;
	}
	while(0 != *(Src+m_pos) && key != *(Src+m_pos))
	{
		Des[m_pos] = Src[m_pos];
		//*(Des+m_pos) = *(Src+m_pos);
		m_pos++;
	}
	if (0 != m_pos)
	{
		m_pos++;
	}
	return m_pos;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_get_head(XCAP_REQUEST_HEAD *Des,VOS_CHAR * Src)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32	index=0;
	if (NULL == Des)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head Des Error");
		return uRet;
	}
	uRet = xcap_get_value(Des->Method,Src,' ');
	if (0 == uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head mothed");
		uRet = 404;
		return uRet;
	}
	index = uRet;
	uRet = xcap_get_value(Des->Request_URI,Src+index,' ');
	if (0 == uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head uir");
		uRet = 404;
		return uRet;
	}
	index += uRet;
	uRet = xcap_get_value(Des->Request_version, Src+index,' ');
	if (0 == uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head version");
		uRet = 404;
		return uRet;
	}
	index += uRet;
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_get_fields(XCAP_FIELDS * Des,VOS_CHAR * Src)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 m_pos=0;
	m_pos = xcap_get_value(Des->Key,Src,':');
	if (0 == m_pos)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head for fields key");
		return 400;//怀请求
	}
	m_pos = xcap_get_value(Des->Value,Src+m_pos,':');
	if (0 == m_pos)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"get head for fields value");
		return 400;//怀请求
	}
	uRet = VOS_OK;
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_request_analysis(VOS_CHAR * pstbufer, XCAP_REQUEST * pReq)
{
	VOS_UINT32	uRet = VOS_ERR;
	VOS_CHAR	strLine[XCAP_REQUESURI_LENGTH]={0};
	VOS_CHAR	* pstrTmp = NULL;
	VOS_UINT32	step =0;
	if (NULL == pReq)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"request analysis Req is NULL");
		return uRet;
	}
	pstrTmp = pstbufer;
	pReq->fields_num = 0;
	do 
	{
		memset(strLine,0,XCAP_REQUESURI_LENGTH);
		pstrTmp = cpss_getline(pstrTmp,strLine,FALSE);
		if (0 == *strLine)
		{
			continue;
		}
		if (0 ==  step)
		{
			uRet = xcap_get_head(&pReq->Req_head,strLine);
			if (VOS_OK != uRet)
			{
				XCAP_PrintErr(__FILE__,__LINE__,"get head mothed");
				return uRet;
			}
			step++;
		}
		else if (1 == step)
		{
			uRet = xcap_get_fields(&pReq->Req_head_fields[pReq->fields_num],strLine);
			if (VOS_OK != uRet)
			{
				XCAP_PrintErr(__FILE__,__LINE__,"get fields error code:%d",uRet);
				return uRet;
			}
			pReq->fields_num++;
		}
	} while ('\0' != *pstrTmp);
	pReq->fields_num--;
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_request_check(XCAP_REQUEST * pReq,VOS_UINT32 flg)
{
	VOS_UINT32 uRet = VOS_ERR;
	// check version
	if (0 == VOS_Strcmp(pReq->Req_head.Request_version,"HTTP/1.1"))
	{
		XCAP_PrintErr(__FILE__,__LINE__,"req version not allow");
		return 505;
	}
	if (0 == _tcscmp(pReq->Req_head.Method, "GET"))
	{
		pReq->Req_mothod = XCAP_REQ_GET;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "HEAD"))
	{
		pReq->Req_mothod = XCAP_REQ_HEAD;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "POST"))
	{
		pReq->Req_mothod = XCAP_REQ_POST;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "PUT"))
	{
		pReq->Req_mothod = XCAP_REQ_PUT;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "DELETE"))
	{
		pReq->Req_mothod = XCAP_REQ_DELETE;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "TRACE"))
	{
		pReq->Req_mothod = XCAP_REQ_TRACE;
	}
	else if (0 == _tcscmp(pReq->Req_head.Method, "CONNECT"))
	{
		pReq->Req_mothod = XCAP_REQ_CONNECT;
	}
	else
	{
		pReq->Req_mothod = XCAP_REQ_OTHER;
		XCAP_PrintErr(__FILE__,__LINE__,"req version not allow");
		return 505;
	}
	//check uri
	//check fields

	return uRet;
}
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
		if (g_StatusCode[uIndex].uCode == Code)
		{
			*pCode = &g_StatusCode[uIndex];
			return VOS_OK;
		}
	}
	return VOS_ERR;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_Fields
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 xcap_response_package(VOS_CHAR * pstbufer, XCAP_RESPONSE * pRes, VOS_STRING pstout)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 ulength = 0;
	VOS_UINT32 uIndex = 0;
	STATUS_CODE * pCode;
	uRet = xcap_response_get_code(pRes->Res_head.StatueCode,&pCode);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"response get code error");
		return uRet;
	}
	ulength = sprintf(pstbufer,"%s %s %s\n",
							pRes->Res_head.Request_version,
							pCode->uCode,
							pCode->phrase);
	while(uIndex < pRes->fields_num)
	{
		if (0 == pRes->Res_head_fields[ulength].Key)
		{
			continue;
		}
		ulength += sprintf(pstbufer+ulength,"%s:%s\r\n",
			pRes->Res_head_fields[ulength].Key,
			pRes->Res_head_fields[ulength].Value);

	}
	ulength += sprintf(pstbufer+ulength,"%s\r\n",pstout);
	return uRet;
}
VOS_UINT32 xcap_response_send(VOS_UINT32 uStatCode, VOS_STRING pstrBuff, VOS_UINT32 flg)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_CHAR strSendBuffer[4096]={0};
	XCAP_RESPONSE Res;
	uRet = xcap_response_package(strSendBuffer,&Res,pstrBuff);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"response get package is error");
		return uRet;
	}
	return uRet;
}
VOS_UINT32 xcap_request_deal(VOS_STRING pstrBuff)
{
	VOS_UINT32 uRet = VOS_ERR;
	XCAP_REQUEST stReq;
	if (NULL == pstrBuff)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"request deal is NULL ");
		return uRet;
	}
	uRet = xcap_request_analysis(pstrBuff, &stReq);
	if (VOS_OK != uRet)
	{
		xcap_response_send(uRet, NULL, 0);
		return VOS_ERR;
	}
	uRet = xcap_request_check(&stReq,0);
	if (VOS_OK != uRet)
	{
		xcap_response_send(uRet, NULL, 0);
		return VOS_ERR;
	}
	return uRet;
}

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