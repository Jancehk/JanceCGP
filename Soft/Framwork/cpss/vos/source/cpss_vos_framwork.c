/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_framwork.c
 *
 *    Description:  信号量
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时34分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#include "cpss_com_pid.h"
#include "cpss_vos_framwork.h"
/* ===  FUNCTION  ==============================================================
 *         Name:  regist_cpuid_to_dbsvr
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 regist_cpuid_to_dbsvr(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR;
	CPSS_MSG		MsgSend = {0};
	VOS_UINT32		uCount = 0,uIndex = 0;
	VOS_UINT32		uBuffLen = 0;
	//CPSS_MEM_BUFFER stuBuffer;
	VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input msg is error");
		return uRet;
	}
	cps_set_msg_dst_cpuid(&MsgSend, CPSSFWCPUID, CPSSFWPID);
	cps_set_msg_src_cpuid(&MsgSend, DBSVRCPUID, DBSVRPID);

	MsgSend.Body.msghead.uType = CPSS_REQ_DBSVR_GET;
	MsgSend.Body.msghead.uSubType = CPSS_TYPE_CPUID_PID;

	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_BUFFER,&uIndex,
			strBuffer + sizeof(VOS_UINT32), (VOS_UINT32 *)&uCount);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add cpuid data error");
		return uRet;
	}
	VOS_Memcpy(strBuffer, (VOS_CHAR *)&uCount,sizeof(VOS_UINT32));
	uBuffLen = sizeof(VOS_UINT32) + sizeof(CPSS_CPUID_INFO)*uCount;
/*	VOS_PrintInfo(__FILE__,__LINE__,"Src:CPuID:%u->Pid:%d,Dst:CPuID:%u->Pid:%d",
		MsgSend.Body.msghead.stSrcProc.ulCpuID,
		MsgSend.Body.msghead.stSrcProc.ulPID,
		MsgSend.Body.msghead.stDstProc.ulCpuID,
		MsgSend.Body.msghead.stDstProc.ulPID);

	VOS_PrintInfo(__FILE__,__LINE__,"Dst:CPuID:%u->Pid:%d",
		pMsgSend->Body.msghead.stDstProc.ulCpuID,
		pMsgSend->Body.msghead.stDstProc.ulPID);*/
	uRet = framwork_send_data(&MsgSend, strBuffer, uBuffLen,VOS_SEND_SKT_TYPE_FINISH);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_dbsvr
 *  Description:  向DBsvr 告诉自己的cpuip和pid，然后请求server其他人的cpuid和pid
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 set_cpuid_from_dbsvr(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR,uIndex = 0;
	VOS_CHAR	  * pstrBuffer = NULL;

	pstrBuffer = pMsgInfo->Body.stuDataBuf.strBuffer;
	if (NULL == pstrBuffer)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set cpuid param is error");
		return uRet;
	}
	
	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_STUCPUID,&uIndex,
		pstrBuffer + sizeof(VOS_UINT32),
		(VOS_UINT32 *)pstrBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.stuDataBuf.strBuffer);
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_init_proc
 *  Description:  初始化共同服务器
 * ==========================================================================*/
static VOS_UINT32 framwork_init(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	switch(pMsgInfo->Body.msghead.uCmd)
	{
	case CPSS_CMD_SYSTEM_INIT:
		VOS_PrintInfo(__FILE__, __LINE__, "Get CPuID From DBSvr");
		uRet = regist_cpuid_to_dbsvr(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintInfo(__FILE__,__LINE__,"Get CpuID from DBsvr is Error ");
		}
		//
		break;
	case CPSS_CMD_SYSTEM_UNIT:
		uRet = VOS_OK;
		break;
	default:
		break;
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_use_proc
 *  Description:  用户和密码处理函数
 * ==========================================================================*/
static VOS_UINT32 dbsvr_use_proc(VOS_VOID *parg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)parg;
	VOS_UINT32 uSubType = pMsgInfo->Body.msghead.uSubType;
	switch(uSubType)
	{
	case CPSS_TYPE_CHECK_USE:
		uRet = cpss_result_use_info(pMsgInfo->Body.stuDataBuf.strBuffer);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "cpss result use info faild");
			uRet = VOS_OK;
		}
		break;
	default:
		break;
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_proc_init
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 dbsvr_proc_init(VOS_VOID *pVoidMsg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG		pMsgInfo = (pCPSS_MSG)pVoidMsg;
	switch(pMsgInfo->Body.msghead.uSubType)
	{
	case CPSS_TYPE_CPUID_PID:
		uRet = set_cpuid_from_dbsvr(pMsgInfo);
		if (VOS_OK == uRet)
		{
			VOS_PrintInfo(__FILE__,__LINE__,"Recv DBsvr Responce proc init OK");
		}
		else
		{
			VOS_PrintErr(__FILE__,__LINE__,"Recv DBsvr Responce proc init Faild");
		}
		break;
	default:
		VOS_PrintErr(__FILE__,__LINE__,"uCmd %X",pMsgInfo->Body.msghead.uCmd);
		uRet=VOS_ERR;
		break;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_init_proc
 *  Description:  初始化共同服务器
 * ==========================================================================*/
VOS_UINT32 framwork_init_proc(VOS_VOID *parg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)parg;
	VOS_UINT8 nCheck = 0;
	
	switch (pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_SYSTEM_INIT:
		uRet = framwork_init(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work init faild");
		}
		break;
	case CPSS_RES_DBSVR_GET:
		uRet = dbsvr_proc_init(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work check use faild");
		}
		break;
	case CPSS_RES_DBSVR_USE:
		uRet = dbsvr_use_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work check use faild");
		}
		break;
	case CPSS_TYPE_SYSTEM_TELNET:
		break;
		default:
			VOS_PrintErr(__FILE__, __LINE__, "Type:%x,SubType:%x:Cmd:%x,SubCmd:%x", 
				pMsgInfo->Body.msghead.uType,
				pMsgInfo->Body.msghead.uSubType,
				pMsgInfo->Body.msghead.uCmd,
				pMsgInfo->Body.msghead.uSubCmd);
		break;
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_timeout_proc
 *  Description:  telnet服务器消息超时处理
 * ==========================================================================*/
VOS_UINT32 framwork_timeout_proc(VOS_VOID *pargc,VOS_UINT32 pargv)
{
	VOS_UINT32 uRet = VOS_ERR;
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_framwork_init
 *  Description:  初始化framwork服务器
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_framwork_init()
{
	VOS_UINT32 uRet = VOS_ERR;
	if (VOS_OK != VOS_RegistPidInit(0,CPSS_PID_FW, CPSS_STRING_FW, 1, framwork_init_proc, framwork_timeout_proc))
	{
		TELNET_PrintErr(__FILE__, __LINE__, "Regist Framwork Server is Error");
	}
	uRet = VOS_OK;
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_send_data
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 framwork_send_data(VOS_VOID *pVoidMsg, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)pVoidMsg;
	//pMsgInfo->pClient = g_psockHandle;
	
	uRet = cpss_send_data(pVoidMsg,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}
