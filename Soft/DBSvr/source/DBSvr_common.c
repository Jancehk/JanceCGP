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
//#include <sys\stat.h>
#include "DBSvr_common.h"
#include "DBSvr_Com_Import.h"
#include "SYS\STAT.H"
//static VOS_VOID* g_psockHandle = 0;
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_get_head
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/

/* ===  FUNCTION  =========================================================
 *         Name:  DBSvr_PrintInfo
 *  Description:  
 * ========================================================================*/
void DBSvr_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_DBSvr, CPSS_PRINT_INFO, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  DBSvr_PrintErr
 *  Description:  
 * ========================================================================*/
void DBSvr_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_DBSvr, CPSS_PRINT_ERR,
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  DBSvr_PrintWarn
 *  Description:  
 * ========================================================================*/

void DBSvr_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_DBSvr, CPSS_PRINT_WARN, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_send_data
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_send_data(VOS_VOID *pVoidMsg, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen,VOS_UINT32 uType)
{
	VOS_UINT32 uRet = VOS_ERR;
	pCPSS_MSG pMsgInfo = (pCPSS_MSG)pVoidMsg;
	//pMsgInfo->pClient = g_psockHandle;
	uRet = cpss_send_data(pVoidMsg,
		pstuBuffer,uBufLen,
		uType | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"send udp data error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_get_file_path
 *  Description:  得到dbsvr的文件路径
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 dbsvr_get_file_path(VOS_CHAR * pstr_path, VOS_CHAR * pstr_file, VOS_CHAR * pstr_name)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_CHAR access_path[256] = {0};

	if (VOS_OK != cpss_get_current_path(access_path))
	{
		return uRet;
	}
	sprintf(pstr_path, "%s\\%s", access_path,pstr_file);
	if (VOS_OK != chdir(pstr_path))
	{
#if (OS_TYPE == OS_TYPE_WINDOW)
		if (VOS_OK != mkdir(pstr_path))
#elif (OS_TYPE == OS_TYPE_LINUX)
		if (VOS_OK != mkdir(pstr_path,TRUE))
#endif
		{
			printf("Make file :%s Error",pstr_path);
			return uRet;
		}
	}
	sprintf(pstr_path, "%s\\%s\\%s",access_path,pstr_file,pstr_name);
	return VOS_TRUE;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 VOS_FreeDllInfo()
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL != pCloseAccMDB)
	{
		uRet = pCloseAccMDB(g_adoConnect);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"close db is failed");
		}
		//DBSvr_PrintInfo(__FILE__,__LINE__,"close accdb handle:%d",g_adoConnect);
	}
	if (NULL == g_hDLL)
	{
		FreeLibrary(g_hDLL);//卸载MyDll.dll文件；
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_VOID* VOS_GetProcAdr(VOS_CHAR * pstrFun)
{
	VOS_CHAR   str_dbsvr_path[256]={0};
	VOS_VOID * pstrFunaddr = NULL;
	while (NULL == g_hDLL)
	{
		if (VOS_OK != dbsvr_get_file_path(str_dbsvr_path,"Dll","CJanceDBdll.dll"))
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"get file path is error");
			return NULL;
		}
		g_hDLL = LoadLibrary(str_dbsvr_path);
	}
	pstrFunaddr = GetProcAddress(g_hDLL, pstrFun);
	if (NULL == pstrFunaddr)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get %s failed",pstrFun);
	}
	return pstrFunaddr;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_open_mdb
 *  Description:  连接数据库
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_open_mdb()
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_CHAR   str_dbsvr_path[256]={0};

	if (NULL == pOpenAccMDB)
	{
		pOpenAccMDB = (OpenAccMDBT)VOS_GetProcAdr("OpenAccMDB");
		if (NULL == pOpenAccMDB)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"get CreateAccMDB failed");
			return VOS_ERR;
		}
	}
	if (NULL == pCloseAccMDB)
	{
		pCloseAccMDB = (CloseAccMDBT)VOS_GetProcAdr("CloseAccMDB");
		if (NULL == pCloseAccMDB)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"get CloseAccMDB failed");
			return VOS_ERR;
		}
	}
	if (0 != g_adoConnect)
	{
		return VOS_OK;
	}
	
	if (VOS_OK != dbsvr_get_file_path(str_dbsvr_path,"DBSvr","DBsvr.Dat"))
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get file path is error");
		return uRet;
	}
	uRet = pOpenAccMDB(str_dbsvr_path,&g_adoConnect);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"create mdb faild%d",uRet);
	}
	//DBSvr_PrintInfo(__FILE__,__LINE__,"open accdb handle:%d",g_adoConnect);
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_check_file()
{
	VOS_UINT32		uRet = VOS_ERR;
	VOS_CHAR   str_dbsvr_path[256]={0};
	VOS_UINT32 uIndex = 0,uIndex2 = 0;
	struct stat	    nstat;
	CreateAccMDBT pCreateAccMDB = NULL;
	if (VOS_OK != dbsvr_get_file_path(str_dbsvr_path,"DBSvr","DBsvr.Dat"))
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get file path is error");
		return uRet;
	}
	uIndex2 =0;
TRY_NEXT:
	if (0 == stat(str_dbsvr_path,&nstat))
	{
		uRet = dbsvr_open_mdb();
		if (VOS_OK != uRet)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"open db failed");
		}
		return VOS_OK;
	}
	if (uIndex2>5)
	{
		return uRet;
	}
	uIndex = 0;
	while (NULL == pCreateAccMDB && uIndex <5)
	{
		pCreateAccMDB = (CreateAccMDBT)VOS_GetProcAdr("CreateAccMDB");
		uIndex++;
	}
	if (NULL == pCreateAccMDB)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get CreateAccMDB failed%d");
		return VOS_ERR;
	}
	uRet = pCreateAccMDB(str_dbsvr_path);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"create mdb faild%d",uRet);
	}
	uIndex2++;
	goto TRY_NEXT;
	//DBSvr_PrintErr(__FILE__,__LINE__,"access file path is error");
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  openrecord
 *  Description:  打开记录集
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 open_record(VOS_CHAR * pstrCmd, VOS_UINT32 * padoRecord)
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL == pExecRecd)
	{
		pExecRecd = (ExecRecdT)VOS_GetProcAdr("ExecRecd");
		if (NULL == pExecRecd)
		{
			return VOS_ERR;
		}
	}
	uRet = dbsvr_open_mdb();
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"open db failed");
		return VOS_ERR;
	}
	uRet = pExecRecd(g_adoConnect,padoRecord,pstrCmd);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"Open Record is failed,Error Code:%d:%s",uRet,pstrCmd);
	}
	//DBSvr_PrintInfo(__FILE__,__LINE__,"open Recd handle:%d",*padoRecord);
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_record_count
 *  Description:  打开记录集
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 get_record_count(VOS_UINT32 padoRecord)
{
	if (NULL == pGetRecordCount)
	{
		pGetRecordCount = (GetRecordCountT)VOS_GetProcAdr("GetRecordCount");
		if (NULL == pGetRecordCount)
		{
			return VOS_ERR;
		}
	}
	return pGetRecordCount(padoRecord);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  openrecord
 *  Description:  打开记录集
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 free_record(VOS_UINT32 padoRecord)
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL == pFreeRecd)
	{
		pFreeRecd = (FreeRecdT)VOS_GetProcAdr("FreeRecd");
		if (NULL == pFreeRecd)
		{
			return VOS_ERR;
		}
	}
	uRet = pFreeRecd(padoRecord);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"Free Record is failed,Error Code:%d",uRet);
	}
	//DBSvr_PrintInfo(__FILE__,__LINE__,"close Recd handle:%d",padoRecord);
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  exec_cmd
 *  Description:  执行sql
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 exec_sql(VOS_CHAR * pstrCmd)
{
	VOS_UINT32 uRet = VOS_ERR;
	if (NULL == pExecCommand)
	{
		pExecCommand = (ExecCommandT)VOS_GetProcAdr("ExecCommand");
		if (NULL == pExecCommand)
		{
			return VOS_ERR;
		}
	}
	uRet = dbsvr_open_mdb();
	uRet = pExecCommand(g_adoConnect,pstrCmd);
	if (VOS_OK != uRet )
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"ExecCommand failed:%d:%s",uRet,pstrCmd);
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  get_count_in_db
 *  Description:  得到记录集个数
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 get_count_in_db(VOS_CHAR * pstrCmd)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 uCount = 0;
	VOS_UINT32 padoRecord = 0;
	uRet = open_record(pstrCmd,&padoRecord);
	if (VOS_OK != uRet )
	{
		return VOS_ERR;
	}
	uCount = get_record_count(padoRecord);
	free_record(padoRecord);
	return uCount;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  check_cpuid_pid_in_db
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 check_cpuid_pid_in_db(pCPSS_CPUID_INFO pstuCPuID)
{
	VOS_UINT32 uRet = VOS_ERR;
	VOS_UINT32 uCount = 0;
	sprintf(g_strCommand,"select * from CPUIDPID where SubSys=%d and m_Index=%d",
		pstuCPuID->ulSubSys,
		pstuCPuID->ulIndex);
	return get_count_in_db(g_strCommand);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  insert_cpuid_pid_to_db
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 update_cpuid_pid_to_db(pCPSS_CPUID_INFO pstuCPuID)
{
	sprintf(g_strCommand,"update CPUIDPID t set t.ProcessCPuID=\"%u\",t.Pid=%d where t.SubSys =%d and t.m_Index=%d",
		pstuCPuID->ulProcessCPuID,
		pstuCPuID->ulPid,
		pstuCPuID->ulSubSys,
		pstuCPuID->ulIndex);
	return exec_sql(g_strCommand);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  insert_cpuid_pid_to_db
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 delete_cpuid_pid_to_db(pCPSS_CPUID_INFO pstuCPuID)
{
	sprintf(g_strCommand,"delete * from CPUIDPID t where t.SubSys =%d and t.m_Index=%d",
		pstuCPuID->ulSubSys,
		pstuCPuID->ulIndex);
	return exec_sql(g_strCommand);
}

/* ===  FUNCTION  ==============================================================
*         Name:  insert_cpuid_pid_to_db
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 insert_cpuid_pid_to_db(pCPSS_CPUID_INFO pstuCPuID)
{
	sprintf(g_strCommand,"INSERT INTO CPUIDPID(SubSys,m_Index,ProcessCPuID,Pid)  VALUES (%d,%d,\"%u\",%d)",
		pstuCPuID->ulSubSys,
		pstuCPuID->ulIndex,
		pstuCPuID->ulProcessCPuID,
		pstuCPuID->ulPid);
	return exec_sql(g_strCommand);
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_update_cpuid_pid
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 dbsvr_update_cpuid_pid(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR;
	VOS_UINT32		uCount = 0,uIndex = 0,Tmp;
	CPSS_CPUID_INFO stuCPuID;
	
	uRet = dbsvr_check_file();
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"check DB faild");
	}
	VOS_Memcpy(&uCount, pMsgInfo->Body.strDataBuf, sizeof(VOS_UINT32));
	while(uIndex < uCount)
	{
		VOS_Memcpy(&stuCPuID,
			pMsgInfo->Body.strDataBuf+sizeof(VOS_UINT32)+uIndex*sizeof(CPSS_CPUID_INFO),
			sizeof(CPSS_CPUID_INFO));

		uRet = dbsvr_open_mdb();
		if (VOS_OK != uRet)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"open db failed");
			return uRet;
		}
		uIndex ++;
		Tmp = check_cpuid_pid_in_db(&stuCPuID);
		if (1 == Tmp)
		{
			uRet = update_cpuid_pid_to_db(&stuCPuID);
			if (VOS_OK != uRet)
			{
				DBSvr_PrintErr(__FILE__,__LINE__,"update db failed");
				return VOS_ERR;
			}
			continue;
		}
		else if (1 < Tmp)
		{
			uRet = delete_cpuid_pid_to_db(&stuCPuID);
			if (VOS_OK != uRet)
			{
				DBSvr_PrintErr(__FILE__,__LINE__,"delete db failed");
				return VOS_ERR;
			}
		}
		uRet = insert_cpuid_pid_to_db(&stuCPuID);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"insert db failed");
			return VOS_ERR;
		}
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_responce_cpuid_pid
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 dbsvr_responce_cpuid_pid(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32		uRet = VOS_ERR;
	CPSS_MSG		MsgInfo = {0};
	VOS_UINT32		uIndex = 0;
	VOS_UINT32		uCount = 0;
	VOS_UINT32		uBuffLen = 0;
	VOS_CHAR		strBuffer[CPSS_MSG_BUFFER_SIZE]={0};

	uRet = dbsvr_update_cpuid_pid(pMsgInfo);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"update cpuid is error :%d", 
			uRet);
	}

	BZERO(&MsgInfo, sizeof(CPSS_MSG));
	
	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_STUCPUID,&uIndex,
		pMsgInfo->Body.strDataBuf + sizeof(VOS_UINT32),
		(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"set cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
	}

	uRet = cpss_get_cpuid_pid_to_buffer(CPSS_SET_TO_BUFFER,&uIndex,
		strBuffer + sizeof(VOS_UINT32), &uCount);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get cpuid to stu is error :%d",
			(VOS_UINT32 *)&pMsgInfo->Body.strDataBuf);
	}


	VOS_Memcpy(&MsgInfo.Body.msghead, 
		&pMsgInfo->Body.msghead,sizeof(CPSS_COM_HEAD));

	MsgInfo.Body.msghead.uType = cps_set_msg_type( CPSS_REQUEST_SYSTEM, CPSS_TYPE_SYS, CPSS_MSG_RESU);
	//MsgInfo.Body.msghead.uSubType = CPSS_TYPE_CPUID_PID;

	VOS_Memcpy(strBuffer, (VOS_CHAR *)&uCount,sizeof(VOS_UINT32));
	uBuffLen = sizeof(VOS_UINT32) + sizeof(CPSS_CPUID_INFO)*uCount;

	uRet = cpss_send_data(&MsgInfo,
		strBuffer, uBuffLen,
		VOS_SEND_SKT_TYPE_FINISH | VOS_SEND_SKT_TYPE_UDP);
	if (VOS_OK != uRet)
	{
		DBSvr_PrintErr(__FILE__, __LINE__, "send udp data error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_get_info_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 dbsvr_get_info_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	switch (cps_get_msgtype_from_msg(pMsgInfo->Body.msghead.uType))
	{
	case CPSS_MSG_REG:
		uRet = dbsvr_responce_cpuid_pid(pMsgInfo);
		break;
	default:
		DBSvr_PrintErr(__FILE__,__LINE__,"get dbsvr info sub type is not correct:%d",pMsgInfo->Body.msghead.uType);
		break;
	}
	if (uRet != VOS_OK)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get dbsvr info sub fun error");
	}
	return uRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_get_info_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_init_proc(pCPSS_MSG pMsgInfo)
{
	VOS_UINT32 uRet = VOS_ERR;
	switch(pMsgInfo->Body.msghead.uCmd)
	{
	case CPSS_CMD_SYSTEM_INIT:
		uRet = VOS_OK;
		DBSvr_PrintInfo(__FILE__,__LINE__,"DBSvr init OK");
		break;
	case CPSS_CMD_SYSTEM_UNIT:
		uRet = VOS_FreeDllInfo();
		if (VOS_OK != uRet)
		{
			DBSvr_PrintErr(__FILE__,__LINE__,"free dll info is faild");
		}
		uRet = VOS_OK;
		break;
	default:
		DBSvr_PrintErr(__FILE__,__LINE__,"get dbsvr info sub type is not correct:%d",pMsgInfo->Body.msghead.uType);
		break;
	}
	if (uRet != VOS_OK)
	{
		DBSvr_PrintErr(__FILE__,__LINE__,"get dbsvr info sub fun error");
	}
	return uRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  DBSVR_deal_proc
*  Description:  初始化共同服务器
*  Description: 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 DBSVR_deal_proc(pCPSS_MSG pMsgInfo)
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
	case DBSVR_TYPE_CPUIDPID:
		uRet = dbsvr_get_info_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintInfo(__FILE__, __LINE__, "get info to DBSvr Faild");
		}
		break;
	case DBSVR_TYPE_USER:
		uRet = dbsvr_user_proc(pMsgInfo);
		if (VOS_OK != uRet)
		{
			DBSvr_PrintInfo(__FILE__, __LINE__, "get check user Faild");
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