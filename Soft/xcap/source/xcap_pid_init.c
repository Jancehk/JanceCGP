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
	if (VOS_OK != VOS_RegistPidInit(CPSS_CONNECT_SUB_XCAP, CPSS_PID_XCAP, CPSS_STRING_XCAP,1,
			pid_init_xcap_proc,xcap_pid_timeout_proc))
	{
		XCAP_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}

	if (VOS_OK != VOS_RegistPidInit(CPSS_CONNECT_SUB_XCAP, CPSS_PID_MONEY, CPSS_STRING_MOCLI,1,
		pid_init_money_proc,money_pid_timeout_proc))
	{
		VOS_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}

	return VOS_OK;
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
	switch (pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_SYSTEM_INIT: 
		if (CPSS_CMD_SYSTEM_INIT == pMsgInfo->Body.msghead.uCmd)
		{
			XCAP_PrintInfo(__FILE__,__LINE__,"XCAP System Init OK");
			uRet = VOS_OK;
		}else
		if (CPSS_CMD_SYSTEM_UNIT == pMsgInfo->Body.msghead.uCmd)
		{
			uRet = VOS_OK;
		}
		break;
	case CPSS_TYPE_SYSTEM_HTTP:
		if (VOS_OK != xcap_request_URL(pMsgInfo))
		{
			XCAP_PrintErr(__FILE__,__LINE__,"send url to XCAP Error ");
		}
		else
		{
			uRet = VOS_OK;
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