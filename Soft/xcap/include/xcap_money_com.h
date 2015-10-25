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
#ifndef _XCAP_DBCLI_COM_H
#define _XCAP_DBCLI_COM_H

#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_msg.h"

typedef struct _MONEY_CLIENT_MANAGE_T
{
	VOS_UINT32 uMoneyServerCPuID;
	VOS_UINT32 uMoneyPid;
}MONEY_CLIENT_MANAGE,*pMONEY_CLIENT_MANAGE;

/* ===  FUNCTION  ==============================================================
 *         Name:  money_send_data
 *  Description:  发送telnet的数据
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 money_send_data(VOS_VOID *pVoidMsg, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);

#ifdef _cplusplus
}
#endif
#endif
