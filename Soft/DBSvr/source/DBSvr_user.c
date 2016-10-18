/*
 * =====================================================================================
 *
 *       Filename:  DBSvr_user.c
 *
 *    Description:  用户检查函数
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

#include "DBSvr_user.h"

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_user_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 dbsvr_check_user_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		ulRet = VOS_ERR;
	VOS_VOID		*padoRecord = NULL;
	pCPSS_USER_INFO pstuUserInfo = NULL;
	VOS_CHAR		strCmd[1024]={0};

	pstuUserInfo = (pCPSS_USER_INFO)pMsgInfo->Body.strDataBuf;
	if (sizeof(CPSS_USER_INFO) < pMsgInfo->Body.msghead.ulMsgLength)
	{
		DBSvr_PrintErr(__FILE__, __LINE__, "recv data size is not correct[%d=%d]", 
			pMsgInfo->Body.msghead.ulMsgLength, sizeof(CPSS_USER_INFO));
		ulRet = VOS_OK;
	}
	sprintf(strCmd,"select t.power from ManageUse t where t.usename = \"%s\" and t.passwd=\"%s\"",
		pstuUserInfo->strUser,pstuUserInfo->strPass);
	ulRet = open_record(strCmd, &padoRecord);
	if (VOS_OK != ulRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"check user and password system error");
		ulRet = VOS_OK;
	}
	pstuUserInfo->ulResult = VOS_ERR;
	if (1 == get_record_count(padoRecord))
	{
		//DBSvr_PrintErr(__FILE__,__LINE__,"user or password is Error");
		pstuUserInfo->ulResult = VOS_OK;
	}
	free_record(padoRecord);
	

	ulRet = send_resp_data(pMsgInfo, pstuUserInfo, sizeof(CPSS_USER_INFO));
	if (VOS_OK != ulRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_get_info_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_user_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	switch (cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_MSG_CHK:
		uRet = dbsvr_check_user_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintInfo(__FILE__,__LINE__,"check user and password Faild");
		}
		break;
	default:
		DBSvr_PrintErr(__FILE__,__LINE__,"get dbsvr info sub type is not correct:%d",pMsgInfo->Body.msghead.uType);
		break;
	}
	if (uRet != VOS_OK)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"dbsvr user info error return");
	}
	return VOS_OK;
}
