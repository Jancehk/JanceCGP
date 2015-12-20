/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.h
 *
 *    Description:  信号量头文件
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时34分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef _CPSS_VOS_FRAMWORK_H
#define _CPSS_VOS_FRAMWORK_H

#include "cpss_common.h"
#include "cpss_public.h"
#include "cpss_msg.h"

/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_init_proc
 *  Description:  初始化共同服务器
 * ==========================================================================*/
VOS_UINT32 framwork_init_proc(VOS_VOID *parg);

/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_timeout_proc
 *  Description:  平台服务器消息超时处理
 * ==========================================================================*/
VOS_UINT32 framwork_timeout_proc(VOS_VOID *pargc,VOS_UINT32 pargv);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_framwork_init
 *  Description:  初始化telnet服务器
 * ==========================================================================*/
VOS_UINT32 cpss_framwork_init();


#endif
