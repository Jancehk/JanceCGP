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
#include "money_pid_init.h"
VOS_UINT32 VOS_ProcessInit()
{
	if (VOS_OK != VOS_RegistPidInit(CPSS_CONNECT_SUB_MONEY, CPSS_PID_MONEY,CPSS_STRING_MONEY,1,
			pid_init_proc, money_pid_timeout_proc))
	{
		VOS_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}
	/*
	if (VOS_OK != VOS_RegistPidInit(CPSS_PID_TEST,"XDMS_Core",pid_init_proc))
	{
		VOS_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}*/
	return VOS_OK;
}

VOS_UINT32 pid_init_proc(VOS_VOID *arg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)arg;
	
	switch(pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_SYSTEM_INIT:
		uRet = money_proc_init(pMsgInfo);
		if (VOS_OK != uRet)
		{
			Money_PrintErr(__FILE__,__LINE__,"get all cpuid faild");
		}
		break;
	case CPSS_REQ_XCAP_GET:
		uRet = proc_xcap_url_result(pMsgInfo);
		if (VOS_OK != uRet)
		{
			Money_PrintErr(__FILE__,__LINE__,"xcap url proc faild");
		}
		break;
	default:
		Money_PrintErr(__FILE__,__LINE__,"uType %X",pMsgInfo->Body.msghead.uType);
		break;
	}
	return uRet;
}


/* ===  FUNCTION  ==============================================================
 *         Name:  money_pid_timeout_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 money_pid_timeout_proc(VOS_VOID *argc,VOS_UINT32 argv)
{
	return VOS_OK;
}