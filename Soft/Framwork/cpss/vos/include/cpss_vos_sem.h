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
#ifndef _CPSS_VOS_SEM_H
#define _CPSS_VOS_SEM_H

#include "cpss_common.h"
#include "cpss_public.h"
#include "cpss_msg.h"

#define MOVE_FREE_TO_USED   11
#define MOVE_USED_TO_FREE	12
typedef enum _MSG_TYPE_INFO_M{
	GET_TCP_RECV_USED=1,
	GET_UDP_RECV_USED,
};
typedef enum _MOVE_TYPE_M{
	MOVE_TYPE_USE_MSG_TCP_RECV_FREE_TO_USED = 3,
	MOVE_TYPE_USE_MSG_UDP_RECV_FREE_TO_USED,
	MOVE_TYPE_USE_MSG_TCP_SEND_FREE_TO_USED,
	MOVE_TYPE_USE_MSG_UDP_SEND_FREE_TO_USED,
	MOVE_TYPE_USE_MSG_TCP_RECV_USED_TO_FREE,
	MOVE_TYPE_USE_MSG_UDP_RECV_USED_TO_FREE,
	MOVE_TYPE_USE_MSG_TCP_SEND_USED_TO_FREE,
	MOVE_TYPE_USE_MSG_UDP_SEND_USED_TO_FREE,
	MOVE_TYPE_USE_MID_TCP_RECV_FREE_TO_USED,
	MOVE_TYPE_USE_MID_UDP_RECV_FREE_TO_USED,
	MOVE_TYPE_USE_MID_TCP_SEND_FREE_TO_USED,
	MOVE_TYPE_USE_MID_UDP_SEND_FREE_TO_USED,
	MOVE_TYPE_USE_MID_TCP_RECV_USED_TO_FREE,
	MOVE_TYPE_USE_MID_UDP_RECV_USED_TO_FREE,
	MOVE_TYPE_USE_MID_TCP_SEND_USED_TO_FREE,
	MOVE_TYPE_USE_MID_UDP_SEND_USED_TO_FREE,
	MOVE_TYPE_USE_MSG_RESE_TO_FREE,
};

typedef enum _SET_MSG_TYPE_M{
	SET_MSG_TCP_PRESEND = 10,
	SET_MSG_TCP_SENDING,
	SET_MSG_TCP_SENDED,
	SET_MSG_TCP_PRERECV,
	SET_MSG_TCP_RECVING,
	SET_MSG_TCP_RECVED,
	SET_MSG_UDP_PRESEND,
	SET_MSG_UDP_SENDING,
	SET_MSG_UDP_SENDED,
	SET_MSG_UDP_PRERECV,
	SET_MSG_UDP_RECVING,
	SET_MSG_UDP_RECVED,
};
typedef enum _SET_MSG_DATA_TYPE_M{
	SET_MSG_DATA_TYPE=1,
	SET_MSG_DATA_CMD,
	SET_MSG_DATA_NUM
};
typedef struct _CPSS_MSG_SEM_MANAGE_T
{
	VOS_MUTEX hMutex;
	CPSS_MSG_STYLE msgtab;
}CPSS_MSG_SEM_MANAGE,*pCPSS_MSG_SEM_MANAGE;

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_init_msg_sem
 *  Description:  ƽ̨��ʼ����Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_init_msg_sem();

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_msg_info
 *  Description:  ������Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
CPSS_MSG * cpss_get_msg_info();
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_send_free_to_used
 *  Description:  ��tcp��send��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_send_free_to_used(CPSS_MSG *pMsgInfo);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_send_used_to_free
 *  Description:  ��tcp��send��Ϣ��ʹ�õ��б��ƶ�������
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_send_used_to_free(CPSS_MSG *pMsgInfo);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_recv_used_to_free
 *  Description:  ��tcp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_used_to_free(CPSS_MSG *pMsgInfo);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_recv_free_to_used
 *  Description:  ��tcp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_free_to_used(CPSS_MSG *pMsgInfo);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_send_free_to_used(CPSS_MSG *pMsgInfo);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_send_used_to_free(CPSS_MSG *pMsgInfo);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_used_to_free(CPSS_MSG *pMsgInfo);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_udp_send_free_to_used
 *  Description:  ��udp��recv��Ϣ�ӿ��е��б��ƶ���ʹ����
 * ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_free_to_used(CPSS_MSG *pMsgInfo);


/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
* ==========================================================================*/
VOS_UINT32 cps_set_msg_src_cpuid(CPSS_MSG * msgTmp, VOS_UINT32 ulCpuID, VOS_UINT32 ulPID);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
* ==========================================================================*/
VOS_UINT32 cps_set_msg_dst_cpuid(CPSS_MSG * msgTmp, VOS_UINT32 ulCpuID, VOS_UINT32 ulPID);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_set_msg_data
*  Description:  �趨��Ϣ��Ϣ
* ==========================================================================*/
VOS_UINT32 cps_set_msg_data(CPSS_MSG * msgTmp, VOS_VOID* m_Value, VOS_UINT8 uType);

/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_msg_mem_data
*  Description:  ������Ϣ�Ŀռ�
* ==========================================================================*/
VOS_UINT32 cps_get_msg_mem_data(CPSS_MSG * msgTmp);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_msg_mem_data
*  Description:  �����������еõ���Ϣ����
* ==========================================================================*/
VOS_UINT32 cps_set_msg_type(VOS_UINT8 uObjPid,VOS_UINT8 uReqType, VOS_UINT8 uReqContent,VOS_UINT8 uMsgType);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_objpid_from_msg
*  Description:  ����Ϣ�еõ�����PID
* ==========================================================================*/
VOS_UINT8 cps_get_objpid_from_msg(VOS_UINT32 ulMsgType);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_reqtype_from_msg
*  Description:  ����Ϣ�еõ���������
* ==========================================================================*/
VOS_UINT8 cps_get_reqtype_from_msg(VOS_UINT32 ulMsgType);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_reqcontent_from_msg
*  Description:  ����Ϣ�еõ���������
* ==========================================================================*/
VOS_UINT8 cps_get_reqcontent_from_msg(VOS_UINT32 ulMsgType);
/* ===  FUNCTION  ==============================================================
*         Name:  cps_get_msgtype_from_msg
*  Description:  ����Ϣ�еõ�����ʽ
* ==========================================================================*/
VOS_UINT8 cps_get_msgtype_from_msg(VOS_UINT32 ulMsgType);


/* ===  FUNCTION  ==============================================================
 *         Name:  cps_uninit_msg_sem
 *  Description:  ƽ̨ж����Ϣ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cps_uninit_msg_sem();

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_show_msg_info
 *  Description:  ��ӡ��Ϣ��Ϣ
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cps_show_msg_info(CPSS_MSG * msgTmp);
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_reservefree_to_free
*  Description:  ��reserve��free��Ϣ�ӿ��е��б��ƶ���ʹ����
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_move_reserve_to_free(CPSS_MSG *pMsgInfo);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_move_tcp_recv_used_to_free_use_msgid
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ������Ƴ������ж���
 * ==========================================================================*/
VOS_UINT32 cpss_move_tcp_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID, VOS_UINT8 nType);
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_udp_recv_used_to_free_use_msgid
*  Description:  ������ϢID��tcp ��Ϣ���ܶ������Ƴ������ж���
* ==========================================================================*/
VOS_UINT32 cpss_move_udp_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID, VOS_UINT8 nType);
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_move_recv_used_to_free_use_msgid
*  Description:  ������ϢID����Ϣ���ܶ������Ƴ������ж���
* ==========================================================================*/
VOS_UINT32 cpss_move_recv_used_to_free_use_msgid(VOS_UINT32 ulMsgID);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_recv_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_recv_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_send_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_send_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_udp_msg_for_id(VOS_UINT32 ulMsgID);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_tcp_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_send_msg_for_id(VOS_UINT32 ulMsgID);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_tcp_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_recv_msg_for_id(VOS_UINT32 ulMsgID);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_recv_msg_for_id
 *  Description:  ������ϢID��tcp ��Ϣ���ܶ����еõ�msg
 * ==========================================================================*/
CPSS_MSG * cpss_get_used_msg(VOS_UINT32 ulPID, VOS_UINT32 uType);
#endif
