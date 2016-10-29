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
#ifndef _XCAP_COMMON_H
#define _XCAP_COMMON_H

#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_msg.h"
	
#ifndef CPSS_TCP_MONEY
#define CPSS_TCP_MONEY      8888
#endif
#define XCAP_HOST_LENGTH			128
#define CPSS_PID_MONEY	VOS_SOCKET_IN | VOS_UDP_PID |CPSS_TCP_MONEY

#define M_DBSVRCPUID	cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_DBSVR, CPSS_GET_TYPE_CPUID)
#define M_DBSVRPID		cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_DBSVR, CPSS_GET_TYPE_PID)

#define M_CPUID			cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_MONEY, CPSS_SUBSYS_TYPE_MONEY, CPSS_GET_TYPE_CPUID)
#define M_PID			cpss_get_cpuid_pid(CPSS_SYSTEM_TYPE_MONEY, CPSS_SUBSYS_TYPE_MONEY, CPSS_GET_TYPE_PID)

typedef struct _XCAP_ROOT_INFO_T{
	VOS_CHAR  RootUrlName[XCAP_HOST_LENGTH];
	VOS_UINT32(*dealFun) (VOS_VOID * pMsgVoid, VOS_CHAR * pstrInput);
}XCAP_ROOT_INFO,*pXCAP_ROOT_INFO;

typedef struct _XCAP_URL_MANAGE_T
{
	VOS_UINT32		ulUrlLen;
	VOS_UINT8		ulParamCount;
	VOS_UINT8		rfu[11];
}XCAP_URL_MANAGE, *pXCAP_URL_MANAGE;
/* ===  FUNCTION  =========================================================
 *         Name:  Money_PrintInfo
 *  Description:  
 * ========================================================================*/
void Money_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  Money_PrintErr
 *  Description:  
 * ========================================================================*/
void Money_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  Money_PrintWarn
 *  Description:  
 * ========================================================================*/
void Money_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);

/* ===  FUNCTION  ==============================================================
 *         Name:  money_system_proc
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 money_system_proc(VOS_VOID *pVoidMsg);

/* ===  FUNCTION  ==============================================================
 *         Name:  money_deal_proc
 *  Description:  记账系统主要处理逻辑
 * ==========================================================================*/
VOS_UINT32 money_deal_proc(VOS_VOID *pVoidMsg);

/* ===  FUNCTION  ==============================================================
*         Name:  money_dbsvr_deal_proc
*  Description:  记账管理系统数据库应答处理
* ==========================================================================*/
VOS_UINT32 money_dbsvr_deal_proc(VOS_VOID *pVoidMsg);

/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root_body
 *  Description:  得到root的body
 * ==========================================================================*/
VOS_UINT32 get_xcap_root_body(VOS_CHAR * pstrInput, VOS_VOID * pMsgVoid);
/*
static XCAP_ROOT_INFO g_rootInfo[] = {
	{0, 0, "/", get_xcap_root_body},
	{1, 0, "/index.html", get_xcap_root_body},
	{2, 0, "/index.asp", get_xcap_root_body},
	{3, 0, "/index.php", get_xcap_root_body},
	{4, 0, "/user.html", get_xcap_root_body},
	{5, 0, "/src", get_xcap_root_body},
};*/

#ifdef _cplusplus
}
#endif
#endif
