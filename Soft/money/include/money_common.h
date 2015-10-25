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

#ifndef  CPSS_UDP_DBCLI
#define  CPSS_UDP_DBCLI		8090
#endif	
#ifndef CPSS_TCP_MONEY
#define CPSS_TCP_MONEY      8888
#endif
#define XCAP_HOST_LENGTH			32
#define CPSS_PID_MONEY	VOS_SOCKET_IN | VOS_UDP_PID |CPSS_TCP_MONEY
#define CPSS_PID_DBCLI	VOS_SOCKET_IN | VOS_UDP_PID |CPSS_TCP_MONEY
	
enum XCAP_URL_LIST{
	XCAP_ROOT_LIST,
	XCAP_ROOT_URL,
	XCAP_USER_URL,
	XCAP_USER_PATH,
};

typedef struct _XCAP_ROOT_INFO_T{
	VOS_UINT8 m_no;
	VOS_UINT8 m_num;
	VOS_CHAR  RootUrlName[XCAP_HOST_LENGTH];
	VOS_UINT32 (* dealFun) (VOS_CHAR * pstrInput, VOS_VOID * pMsgVoid);
}XCAP_ROOT_INFO,*pXCAP_ROOT_INFO;

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
 *         Name:  money_proc_init
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 money_proc_init(VOS_VOID *pVoidMsg);

/* ===  FUNCTION  ==============================================================
 *         Name:  proc_xcap_url_result
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 proc_xcap_url_result(VOS_VOID *pVoidMsg);

/* ===  FUNCTION  ==============================================================
 *         Name:  get_xcap_root_body
 *  Description:  得到root的body
 * ==========================================================================*/
VOS_UINT32 get_xcap_root_body(VOS_CHAR * pstrInput, VOS_VOID * pMsgVoid);

static XCAP_ROOT_INFO g_rootInfo[] = {
	{0, XCAP_ROOT_URL, "/", get_xcap_root_body},
	{1, XCAP_ROOT_URL, "/index.html", get_xcap_root_body},
	{2, XCAP_ROOT_URL, "/index.asp", get_xcap_root_body},
	{3, XCAP_ROOT_URL, "/index.php", get_xcap_root_body},
	{4, XCAP_USER_URL, "/user.html", get_xcap_root_body},
	{5, XCAP_USER_PATH, "/src", get_xcap_root_body},
};

#ifdef _cplusplus
}
#endif
#endif
