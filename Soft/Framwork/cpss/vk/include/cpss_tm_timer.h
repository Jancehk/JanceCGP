/*
 * =====================================================================================
 *
 *       Filename:  cpss_vk_tm_timer.h
 *
 *    Description:  ��ʱ��ͷ�ļ�
 *
 *        Version:  1.0
 *        Created:  2011��06��09�� 23ʱ30��48��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 * =====================================================================================
 */
#ifndef  CPSS_TM_TIMER_IN
#define  CPSS_TM_TIMER_IN
#ifdef _cplusplus
extern "C" {
#endif
#include 	"cpss_common.h"
#define CPSS_MAX_TIME		32
#if OS_TYPE == OS_TYPE_WINDOW
#pragma comment(lib,"winmm.lib")
#elif OS_TYPE == OS_TYPE_LINUX
#endif
typedef struct CPSS_TIME_T
{
	VOS_UINT32	nYear;
	VOS_UINT16	nMonth;
	VOS_UINT16  nDay;
	VOS_UINT16  nDayofWeek;		//0--�������� 1--6 ��..
	VOS_UINT16  nHour;
	VOS_UINT16  nMin;
	VOS_UINT16  nSec;
	VOS_UINT16  nUsec;
}CPSS_TIME,*PCPSS_TIME;

typedef struct _timer_handle
{
	VOS_UINT64 ptr;
	VOS_UINT64 entry_id;
}timer_handle,*ptimer_handle;

/* timer entry */
typedef struct _mul_timer_entry
{
	VOS_UINT8 is_use; /* 0, not; 1, yes */
	struct _timer_handle handle;
	VOS_UINT32 timeout;
	VOS_UINT32 elapse; /* */
	VOS_UINT32 (* timer_proc) (VOS_VOID *arg, VOS_UINT32 *arg_len); /* callback function */
	VOS_VOID *arg;
	VOS_UINT32 *arg_len;
	struct _mul_timer_entry *next;
}mul_timer_entry;

typedef struct _mul_timer_manage
{
	VOS_UINT64 entry_id;
	VOS_UINT32 timer_cnt;
	VOS_UINT32 time_unit;
	VOS_MUTEX  hMutex;
	struct _mul_timer_entry *phead;
	VOS_UINT32 timer_id;
}mul_timer_manage;

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time
*  Description: �õ�ϵͳʱ��
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_date_or_time (VOS_STRING pstCPssTime,int date_or_time);
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time_to_str
*  Description:	 �õ�ϵͳ��ǰʱ�䲢�ҷ����ַ��� 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_current_time_to_str (VOS_STRING pszTimer);

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_init_mul_timer
*  Description:	 ��ʼ����ʱ��
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_init_mul_timer(VOS_UINT32 time_unit);

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_set_timer
*  Description:	 ����һ����ʱ��
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
ptimer_handle cpss_set_timer(VOS_UINT32 time_out, 
							 VOS_UINT32 (* timer_proc) (VOS_VOID *arg, VOS_UINT32 *arg_len), 
							 VOS_UINT32 *arg, 
							 VOS_UINT32 *arg_len);

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_kill_timer
 *  Description:	 ɾ��һ����ʱ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_kill_timer(ptimer_handle hdl);

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 ͨ��timer����õ�ʣ�೬ʱʱ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_byhdl(ptimer_handle hdl);

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 ͨ��timer�ص������õ�ʣ�೬ʱʱ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_bytimeproc(VOS_UINT32 (* timer_proc) (VOS_VOID *arg, VOS_UINT32 *arg_len));

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 ͨ��timer�ص������õ�ʣ�೬ʱʱ��
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_uninit_mul_timer();


#ifdef _cplusplus
}
#endif
#endif
