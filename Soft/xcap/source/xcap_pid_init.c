/*
 * =====================================================================================
 *
 *       Filename:  test_pid_init.c
 *
 *    Description:  测试函数
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 18时41分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#include "xcap_pid_init.h"
/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_ProcessInit
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_ProcessInit()
{
	xcap_fileds_init();
	if (VOS_OK != VOS_RegistPidInit(CPSS_SYSTEM_TYPE_XCAP, CPSS_PID_XCAP, CPSS_STRING_XCAP,1,
			pid_init_xcap_proc,xcap_pid_timeout_proc))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}

	if (VOS_OK != VOS_RegistPidInit(CPSS_SYSTEM_TYPE_XCAP, CPSS_PID_MONEY, CPSS_STRING_MOCLI,1,
		pid_init_money_proc,money_pid_timeout_proc))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}

	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
*         Name:  framwork_init_proc
*  Description:  初始化共同服务器
* ==========================================================================*/
static VOS_UINT32 xcap_system_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT8 nCheck = 0;
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg head is null");
		return uRet;
	}

	switch (cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_TYPE_SYS:
		if (CPSS_CMD_SYSTEM_INIT == cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
		{
			XCAP_PrintInfo(__FILE__, __LINE__, "XCAP System Init OK");
			uRet = VOS_OK;
		}
		else
		if (CPSS_CMD_SYSTEM_UNIT == cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
		{
			uRet = VOS_OK;
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

/* ===  FUNCTION  ==============================================================
*         Name:  xcap_system_url_proc
*  Description:  URL 解析处理入口
* ==========================================================================*/
static VOS_UINT32 xcap_system_url_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT8 nCheck = 0;
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg head is null");
		return uRet;
	}

	switch (cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case XCAP_TYPE_URL:
		uRet = xcap_request_URL(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "send url to XCAP Error ");
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

/* ===  FUNCTION  ==============================================================
 *         Name:  pid_init_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 pid_init_xcap_proc(VOS_VOID *arg)
{
	VOS_UINT32		uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)arg;
	switch (cps_get_reqtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_REQUEST_SYSTEM:
		uRet = xcap_system_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "send url to XCAP Error ");
		}
		break;
	case XCAP_REQUEST_URL:
		uRet = xcap_system_url_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			XCAP_PrintErr(__FILE__, __LINE__, "send url to XCAP Error ");
		}
		break;
	default:
		break;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_pid_timeout_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_pid_timeout_proc(VOS_VOID *argc,VOS_UINT32 argv)
{
	return VOS_OK;
}