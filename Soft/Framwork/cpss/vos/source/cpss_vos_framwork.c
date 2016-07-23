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
*         Name:  framwork_send_data
*  Description:  发送telnet的数据
*  Input      :
*  OutPut     :
*  Return     :
* ==========================================================================*/
static VOS_UINT32 framwork_send_data(VOS_UINT32 ulCpuID,
	VOS_UINT32 ulPID,
	VOS_UINT32 uType,
	VOS_UINT32 uCmd,
	VOS_CHAR * pstrBuffer,
	VOS_UINT32 uBufLen)
{
	VOS_UINT32 uRet = VOS_ERR;
	CPSS_MSG		MsgSend = { 0 };

	cps_set_msg_from_cpuid(&MsgSend, CPSSFWCPUID, CPSSFWPID);
	cps_set_msg_to_cpuid(&MsgSend, ulCpuID, ulPID);

	MsgSend.Body.msghead.uType = uType;
	MsgSend.Body.msghead.uCmd = uCmd;

	uRet = cpss_send_data(&MsgSend, pstrBuffer, uBufLen,
		VOS_SEND_SKT_TYPE_FINISH | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "send udp data error");
	}
	return uRet;
}
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
	pCPSS_CPUID_HEADER pstrCpuidCount = NULL;
	VOS_UINT32		uCount = 0,uIndex = 0;
	VOS_UINT32		uBuffLen = 0;
	VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__,__LINE__,"input msg is error");
		return uRet;
	}
	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_BUFFER,	strBuffer);
	if (VOS_OK != uRet || NULL == strBuffer)
	{
		VOS_PrintErr(__FILE__,__LINE__,"add cpuid data error");
		return uRet;
	}
	pstrCpuidCount = (pCPSS_CPUID_HEADER)strBuffer;
	uBuffLen = sizeof(CPSS_CPUID_HEADER) + (sizeof(CPSS_CPUID_INFO)*pstrCpuidCount->ulCount);
	pstrCpuidCount->ulCount = htonl(pstrCpuidCount->ulCount);

	uRet = framwork_send_data(DBSVRCPUID, DBSVRPID,
		cps_set_msg_type(DBSVR_REQUEST_MGR, DBSVR_TYPE_CPUIDPID, CPSS_MSG_REG),
		0,	strBuffer, uBuffLen);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "send udp data error");
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
	pCPSS_CPUID_HEADER pstrCpuidCount = NULL;

	pstrBuffer = pMsgInfo->Body.strDataBuf;
	if (NULL == pstrBuffer)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set cpuid param is error");
		return uRet;
	}
	pstrCpuidCount = (pCPSS_CPUID_HEADER)pstrBuffer;
	pstrCpuidCount->ulCount = ntohl(pstrCpuidCount->ulCount);

	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_STUCPUID, pstrBuffer);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__,__LINE__,"set cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
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
	switch (cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_MSG_INIT:
		VOS_PrintInfo(__FILE__, __LINE__, "Get CPuID From DBSvr");
		uRet = regist_cpuid_to_dbsvr(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintInfo(__FILE__, __LINE__, "Get CpuID from DBsvr is Error ");
		}
		break;
	case CPSS_MSG_RESU:
		uRet = set_cpuid_from_dbsvr(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "Recv DBsvr Responce proc init Faild");
		}
		break;
	default:
		VOS_PrintInfo(__FILE__, __LINE__, "Recv DBsvr Responce framwork init faild");
		break;
	}
	return uRet;
}
#if 0
/* ===  FUNCTION  ==============================================================
*         Name:  dbsvr_use_proc
*  Description:  用户和密码处理函数
* ==========================================================================*/
static VOS_UINT32 dbsvr_use_proc(VOS_VOID *parg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)parg;
	VOS_UINT32 uSubType = 0;//pMsgInfo->Body.msghead.uSubType;
	switch (uSubType)
	{
	case CPSS_TYPE_CHECK_USE:
		uRet = cpss_result_use_info(pMsgInfo->Body.strDataBuf);
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
	switch(pMsgInfo->Body.msghead.uType)
	{
	case CPSS_TYPE_CPUID_PID:
		break;
	default:
		VOS_PrintErr(__FILE__,__LINE__,"uCmd %X",pMsgInfo->Body.msghead.uCmd);
		uRet=VOS_ERR;
		break;
	}
	return uRet;
}
#endif

/* ===  FUNCTION  ==============================================================
*         Name:  framwork_init_proc
*  Description:  初始化共同服务器
* ==========================================================================*/
static VOS_UINT32 framwork_sys_type_proc(pCPSS_MSG pMsgInfo)
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
	case CPSS_MSG_INIT:
		uRet = framwork_init(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work init faild");
		}
		break;
	case CPSS_MSG_UNIT:
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
*         Name:  framwork_init_proc
*  Description:  初始化共同服务器
* ==========================================================================*/
static VOS_UINT32 framwork_system_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL == pMsgInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "msg head is null");
		return uRet;
	}
	switch (cps_get_reqcontent_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_TYPE_SYS:
		uRet = framwork_init(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work init faild");
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
 *         Name:  framwork_init_proc
 *  Description:  初始化共同服务器
 * ==========================================================================*/
VOS_UINT32 framwork_init_proc(VOS_VOID *parg)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)parg;
	VOS_UINT8 nCheck = 0;
	
	switch (cps_get_reqtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_REQUEST_SYSTEM:
		uRet = framwork_system_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			VOS_PrintErr(__FILE__, __LINE__, "fram work init faild");
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

