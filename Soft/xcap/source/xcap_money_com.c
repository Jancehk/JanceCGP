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
//static VOS_VOID* g_psockHandle = 0;
/* ===  FUNCTION  ==============================================================
 *         Name:  money_send_data
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 money_send_data(VOS_VOID *pVoidMsg, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)pVoidMsg;
	//pMsgInfo->pClient = g_psockHandle;
	
	uRet = cpss_send_data(pVoidMsg,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  set_cpuid_from_dbsvr
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 set_cpuid_from_dbsvr(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR,uIndex = 0;
	VOS_CHAR	  * pstrBuffer = NULL;

	pstrBuffer = pMsgInfo->Body.strDataBuf;
	if (NULL == pstrBuffer)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"set cpuid param is error");
		return uRet;
	}

	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_STUCPUID, &uIndex,
		pstrBuffer + sizeof(VOS_UINT32),(VOS_UINT32 *)pstrBuffer);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"set cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_proc_init
 *  Description:  发送telnet的数据
 * ==========================================================================*/
static VOS_UINT32 xcap_get_proc(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)pVoidMsg;
	switch (cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case XCAP_REQUEST_MONCLI:
		uRet = xcap_responce_proc(pMsgInfo);
		if (VOS_OK == uRet)
		{
			XCAP_PrintInfo(__FILE__,__LINE__,"xcap responce proc OK");
		}
		else
		{
			XCAP_PrintErr(__FILE__,__LINE__,"xcap responce proc Faild");
		}	
		break;
	default:
		XCAP_PrintErr(__FILE__,__LINE__,"uCmd %X",pMsgInfo->Body.msghead.uCmd);
		uRet=VOS_ERR;
		break;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  pid_init_dbcli_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 pid_init_money_proc(VOS_VOID *arg)
{
	VOS_UINT32		uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)arg;

	switch (cps_get_reqtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case XCAP_REQUEST_MONCLI:
		uRet = xcap_get_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "xcap recv faild");
		}
		break;
	default:
		VOS_PrintErr(__FILE__, __LINE__, "Type:%08x,Cmd:%08x",
			pMsgInfo->Body.msghead.uType,
			pMsgInfo->Body.msghead.uCmd);
		break;
	}
	return uRet;
}
VOS_UINT32 money_pid_timeout_proc(VOS_VOID *argc,VOS_UINT32 argv)
{
	return VOS_OK;
}
