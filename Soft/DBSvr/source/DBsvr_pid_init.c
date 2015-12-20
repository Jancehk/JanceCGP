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
#include "DBSvr_user.h"
#include "DBSvr_pid_init.h"

VOS_UINT32 pid_init_proc(VOS_VOID *arg)
{
	VOS_UINT32		uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)arg;

	switch (cps_get_reqtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_REQUEST_SYSTEM:
		uRet = dbsvr_init_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintInfo(__FILE__, __LINE__, "DBSvr init Faild");
		}
		break;
	case DBSVR_REQUEST_MGR:
		uRet = DBSVR_deal_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "DBSvr deal proc faild");
		}
		break;
	default:
		VOS_PrintErr(__FILE__, __LINE__, "Type:%08x,Cmd:%08x",
			pMsgInfo->Body.msghead.uType,
			pMsgInfo->Body.msghead.uCmd);
		break;
	}
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_pid_timeout_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_pid_timeout_proc(VOS_VOID *argc,VOS_UINT32 argv)
{
	return VOS_OK;
}
VOS_UINT32 VOS_ProcessInit()
{
	if (VOS_OK != VOS_RegistPidInit(CPSS_CONNECT_SUB_DBSVR, CPSS_PID_DBSvr, "DBSVR", 1, pid_init_proc, dbsvr_pid_timeout_proc))
	{
		DBSvr_PrintErr(__FILE__, __LINE__, "Regist Telnet Server is Error");
		return VOS_ERR;
	}

	return VOS_OK;
}
