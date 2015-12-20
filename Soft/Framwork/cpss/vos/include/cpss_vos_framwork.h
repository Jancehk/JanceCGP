/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.h
 *
 *    Description:  �ź���ͷ�ļ�
 *
 *        Version:  1.0
 *        Created:  2011��06��09�� 23ʱ34��39��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
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
 *  Description:  ��ʼ����ͬ������
 * ==========================================================================*/
VOS_UINT32 framwork_init_proc(VOS_VOID *parg);

/* ===  FUNCTION  ==============================================================
 *         Name:  framwork_timeout_proc
 *  Description:  ƽ̨��������Ϣ��ʱ����
 * ==========================================================================*/
VOS_UINT32 framwork_timeout_proc(VOS_VOID *pargc,VOS_UINT32 pargv);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_framwork_init
 *  Description:  ��ʼ��telnet������
 * ==========================================================================*/
VOS_UINT32 cpss_framwork_init();


#endif
