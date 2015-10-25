/*
 * =====================================================================================
 *
 *       Filename:  cpss_vk_tm_timer.h
 *
 *    Description:  定时器头文件
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时30分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
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
	VOS_UINT16  nDayofWeek;		//0--是星期天 1--6 是..
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
*  Description: 得到系统时间
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_date_or_time (VOS_STRING pstCPssTime,int date_or_time);
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time_to_str
*  Description:	 得到系统当前时间并且返回字符串 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_current_time_to_str (VOS_STRING pszTimer);

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_init_mul_timer
*  Description:	 初始化定时器
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_init_mul_timer(VOS_UINT32 time_unit);

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_set_timer
*  Description:	 设置一个定时器
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
 *  Description:	 删除一个定时器
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_kill_timer(ptimer_handle hdl);

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 通过timer句柄得到剩余超时时间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_byhdl(ptimer_handle hdl);

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 通过timer回调函数得到剩余超时时间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_bytimeproc(VOS_UINT32 (* timer_proc) (VOS_VOID *arg, VOS_UINT32 *arg_len));

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 通过timer回调函数得到剩余超时时间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_uninit_mul_timer();


#ifdef _cplusplus
}
#endif
#endif
