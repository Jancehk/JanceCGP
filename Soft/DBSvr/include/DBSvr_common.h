/*
 * =====================================================================================
 *
 *       Filename:  test_pid_init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 18时42分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
*/
//#pragma comment(lib,"cpss.lib")
#ifndef _DBSvr_COMMON_H
#define _DBSvr_COMMON_H
#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_msg.h"

	
#ifndef CPSS_TCP_DBSVR
#define CPSS_TCP_DBSVR      5001
#endif
#define CPSS_PID_DBSvr	VOS_SOCKET_IN | VOS_UDP_PID |CPSS_TCP_DBSVR

/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void DBSvr_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  DBSvr_PrintErr
 *  Description:  
 * ========================================================================*/
void DBSvr_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void DBSvr_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 * ==========================================================================*/
VOS_VOID* VOS_GetProcAdr(VOS_CHAR * pstrFun);
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 * ==========================================================================*/
VOS_UINT32 dbsvr_check_file();
/* ===  FUNCTION  ==============================================================
 *         Name:  openrecord
 *  Description:  打开记录集
 * ==========================================================================*/
VOS_UINT32 open_record(VOS_CHAR * pstrCmd, VOS_UINT32 * padoRecord);
/* ===  FUNCTION  ==============================================================
 *         Name:  get_record_count
 *  Description:  打开记录集
 * ==========================================================================*/
VOS_UINT32 get_record_count(VOS_UINT32 padoRecord);
/* ===  FUNCTION  ==============================================================
 *         Name:  openrecord
 *  Description:  打开记录集
 * ==========================================================================*/
VOS_UINT32 free_record(VOS_UINT32 padoRecord);
/* ===  FUNCTION  ==============================================================
 *         Name:  get_count_in_db
 *  Description:  得到记录集个数
 * ==========================================================================*/
VOS_UINT32 get_count_in_db(VOS_CHAR * pstrCmd);
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_send_data
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 dbsvr_send_data(VOS_VOID *pVoidMsg, 
						   VOS_VOID * pstuBuffer, 
						   VOS_UINT32 uBufLen,
						   VOS_UINT32 uType);


/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_get_info_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 dbsvr_init_proc(pCPSS_MSG pMsgInfo);
/* ===  FUNCTION  ==============================================================
*         Name:  DBSVR_deal_proc
*  Description:  初始化共同服务器
* ==========================================================================*/
VOS_UINT32 DBSVR_deal_proc(pCPSS_MSG pMsgInfo);
#ifdef _cplusplus
}
#endif
#endif
