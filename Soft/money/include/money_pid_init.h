/*
 * =====================================================================================
 *
 *       Filename:  test_pid_init.h
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
#ifndef TEST_PID_INIT
#define TEST_PID_INIT
#include "cpss_common.h"
#include "cpss_msg.h"
#include "money_common.h"

#if (OS_TYPE == OS_TYPE_WINDOW)
#pragma warning (disable:4098)
#elif (OS_TYPE == OS_TYPE_LINUX)
#endif
/* ===  FUNCTION  ==============================================================
 *         Name:  pid_init_xcap_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 pid_init_proc(VOS_VOID *arg);


/* ===  FUNCTION  ==============================================================
 *         Name:  money_pid_timeout_proc
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 money_pid_timeout_proc(VOS_VOID *argc,VOS_UINT32 argv);
#endif
