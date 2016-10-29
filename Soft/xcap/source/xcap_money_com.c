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

	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_STUCPUID, pstrBuffer );
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__,__LINE__,"set cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  money_url_responce
*  Description:  记账URL处理主函数
* ==========================================================================*/
static VOS_UINT32 money_url_responce(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32		ulCpuID, ulPID;
	CPSS_MSG		stuMsgSend = { 0 };
	pCPSS_MSG		pRecvMsgInfo = NULL, pMsgInfo = (pCPSS_MSG)pVoidMsg;

	if (CPSS_MSG_RES != cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "get address is error");
		return uRet;
	}
	uRet = cpss_copy_msg(&stuMsgSend, pMsgInfo);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "copy send msg failed");
		return uRet;
	}
	pRecvMsgInfo = (pCPSS_MSG)cpss_get_recv_msg_for_id(pMsgInfo->Body.msghead.ulRecvMsgID);
	if (NULL == pRecvMsgInfo)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "find recv msg failed");
		return uRet;
	}
	uRet = xcap_change_req_mode_proc(pRecvMsgInfo, XCAP_REQ_GET);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "change xcap req mode faild");
		return uRet;
	}
	uRet = cpss_copy_msg(&stuMsgSend, pRecvMsgInfo);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "copy send msg failed");
		return uRet;
	}
	ulCpuID = cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_XCAP, CPSS_SUBSYS_TYPE_XCAP, CPSS_GET_TYPE_CPUID);

	ulPID = cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_XCAP, CPSS_SUBSYS_TYPE_XCAP, CPSS_GET_TYPE_PID);

	stuMsgSend.Body.msghead.uType = cps_set_msg_type(XCAP_REQUEST_URL, XCAP_TYPE_DATA, CPSS_MSG_REQ);

	cps_set_msg_to_cpuid(&stuMsgSend, ulCpuID, ulPID);
	XCAP_PrintInfo(__FILE__, __LINE__, "Msg[%d]R_ID[%d] send to Tcp pid[%d]",
		stuMsgSend.ulMsgID,
		stuMsgSend.Body.msghead.ulRecvMsgID,
		stuMsgSend.Body.msghead.stDstProc.ulPID);
	uRet = cpss_send_data(&stuMsgSend, NULL, 0, VOS_SEND_SAVE_TO_RECV | VOS_SEND_SKT_TYPE_TCP);
	if (VOS_OK != uRet)
	{
		XCAP_PrintErr(__FILE__, __LINE__, "send udp data error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  money_responce_proc
*  Description:  记账应答出处理接口函数
* ==========================================================================*/
static VOS_UINT32 money_responce_proc(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)pVoidMsg;
	switch (cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case XCAP_TYPE_URL:
		uRet = money_url_responce(pMsgInfo);
		if (VOS_OK == uRet)
		{
			XCAP_PrintInfo(__FILE__, __LINE__, "xcap responce proc OK");
		}
		else
		{
			XCAP_PrintErr(__FILE__, __LINE__, "xcap responce proc Faild");
		}
		break;
	default:
		XCAP_PrintErr(__FILE__, __LINE__, "uCmd %X", pMsgInfo->Body.msghead.uCmd);
		uRet = VOS_ERR;
		break;
	}
	return uRet;
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
	case CPSS_REQUEST_SYSTEM:
		if (CPSS_TYPE_SYS != cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
		{
			XCAP_PrintErr(__FILE__, __LINE__, "MONEY System request Faild");
			return uRet;
		}
		if (CPSS_MSG_INIT == cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
		{
			XCAP_PrintInfo(__FILE__, __LINE__, "MONEY System Init OK");
			uRet = VOS_OK;
		}
		else if (CPSS_MSG_UNIT == cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
		{
			uRet = VOS_OK;
		}
		break;
	case MONEY_REQUEST_MGR:
		uRet = money_responce_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "xcap recv faild");
		}
		break;
	case XCAP_REQUEST_MONCLI:
		uRet = xcap_get_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "xcap recv faild");
		}
		break;
	default:
		XCAP_PrintErr(__FILE__, __LINE__, "Type:%08x,Cmd:%08x",
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
