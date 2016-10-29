/*
 * =====================================================================================
 *
 *       Filename:  DBSvr_user.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011��06��04�� 18ʱ42��18��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 * =====================================================================================
 */
#ifndef _DBSVR_ABOUT_USER_H
#define _DBSVR_ABOUT_USER_H

#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_msg.h"

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_user_proc
 *  Description: 
 * ==========================================================================*/
VOS_UINT32 dbsvr_user_proc(pCPSS_MSG pMsgInfo);

#ifdef _cplusplus
}
#endif
#endif
