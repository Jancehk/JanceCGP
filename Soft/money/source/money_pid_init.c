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
	if (VOS_OK != VOS_RegistPidInit(CPSS_SYSTEM_TYPE_MONEY, CPSS_PID_MONEY,CPSS_STRING_MONEY,1,
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

	switch (cps_get_reqtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_REQUEST_SYSTEM:
		uRet = money_system_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			Money_PrintErr(__FILE__, __LINE__, "get all cpuid faild");
		}
		break;
	case MONEY_REQUEST_MGR:
		uRet = money_deal_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			Money_PrintErr(__FILE__, __LINE__, "money deal faild");
		}
		break;
	default:
		Money_PrintErr(__FILE__, __LINE__, "this function is not support");
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