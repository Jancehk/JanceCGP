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
	VOS_UINT32		uBuffLen = 0;
	VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	VOS_CHAR		strCmd[1024]={0};
	CPSS_MSG		MsgInfo = {0};

	pstuUserInfo = (pCPSS_USER_INFO)pMsgInfo->Body.stuDataBuf;
	sprintf(strCmd,"select t.power from ManageUse t where t.usename = \"%s\" and t.passwd=\"%s\"",
		pstuUserInfo->strUser,pstuUserInfo->strPass);
	ulRet = open_record(strCmd, &padoRecord);
	if (VOS_OK != ulRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"check user and password system error");
		ulRet = VOS_OK;
	}
	pstuUserInfo->ulResult = VOS_OK;
	if (1 != get_record_count(padoRecord))
	{
		//DBSvr_PrintErr(__FILE__,__LINE__,"user or password is Error");
		pstuUserInfo->ulResult = VOS_ERR;
	}
	free_record(padoRecord);
	
	BZERO(&MsgInfo, sizeof(CPSS_MSG));
	
	VOS_Memcpy(&MsgInfo.Body.msghead.stDstProc, 
		&pMsgInfo->Body.msghead.stSrcProc,sizeof(CPSS_COM_PID));
	VOS_Memcpy(&MsgInfo.Body.msghead.stSrcProc, 
		&pMsgInfo->Body.msghead.stDstProc,sizeof(CPSS_COM_PID));
	MsgInfo.Body.msghead.uType	   = CPSS_RES_DBSVR_USE;
	//MsgInfo.Body.msghead.uSubType  = CPSS_TYPE_CHECK_USE;

	VOS_Memcpy(strBuffer, (VOS_CHAR *)pstuUserInfo,sizeof(CPSS_USER_INFO));
	uBuffLen = sizeof(CPSS_USER_INFO);

	ulRet = dbsvr_send_data(&MsgInfo, strBuffer, uBuffLen, VOS_SEND_SKT_TYPE_FINISH);
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
	switch(pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_CHECK_USE:
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
