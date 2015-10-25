/*
 * =====================================================================================
 *
 *       Filename:  cpss_vk_tm_timer.c
 *
 *    Description:  定时器
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时30分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
*/
#include <sys/timeb.h>
#include <sys/types.h>
#include "cpss_public.h"
#include "cpss_tm_timer.h"

static mul_timer_manage *g_ptimer = NULL;

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time
*  Description: 得到系统时间
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
static VOS_UINT32 cpss_get_time(PCPSS_TIME pszCPsstime)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR sztime[128];
	VOS_CHAR sztimetmp[16];
	memset(sztime, 0, 128);
	memset(sztimetmp, 0, 16);
//    time_t ltime;
//	struct tm *today = NULL;
/*	//time_t ltime;
	//struct timeval 	tv;
	//`struct timezone tz;
	//time(&timeval);
	//ltime = gtodtmp.time; 
	//(t)->tv_usec = _gtodtmp.millitm * 1000;
#if (OS_TYPE == OS_TYPE_WINDOW)
    today = localtime( &ltime );
#elif (OS_TYPE == OS_TYPE_LINUX)
	struct timeb gtodtmp;
	ftime (&gtodtmp);
	today = localtime(&gtodtmp.time);
	//(t)->tv_usec = _gtodtmp.millitm * 1000; 
#endif*/
//	strftime(sztime,128,"%Y-%m-%d %w %H:%M:%S",today);
	//sztimetmp = cpss_get_str_before(sztime,"-");
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time
*  Description: 得到系统时间
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_date_or_time (VOS_STRING pstCPssTime,int date_or_time)
{
	VOS_UINT32 ulRet = VOS_ERR;
	struct tm *today = NULL;
	struct timeb gtodtemp;
	ftime (&gtodtemp);
	today = localtime(&gtodtemp.time);
	if (1 == date_or_time )
	{
		strftime(pstCPssTime,CPSS_MAX_TIME,"%Y-%m-%d",today);
	}
	else if (2 == date_or_time)
	{
		strftime(pstCPssTime,CPSS_MAX_TIME,"%H:%M:%S",today);
	}
	else
	{
		strftime(pstCPssTime,CPSS_MAX_TIME,"%Y-%m-%d %H:%M:%S",today);
	}
	if (NULL != pstCPssTime)
	{
		ulRet = VOS_OK;
	}
	return ulRet;
}		/* -----  end of function cpss_get_current_time  ----- */

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time
*  Description: 得到系统时间
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_current_time (PCPSS_TIME pstCPssTime)
{
	VOS_UINT32 ulRet = VOS_ERR;
	return ulRet;
}		/* -----  end of function cpss_get_current_time  ----- */

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_current_time_to_str
*  Description:	 得到系统当前时间并且返回字符串 
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_current_time_to_str (VOS_STRING psztimer)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_CHAR sztimetmp[CPSS_MAX_TIME]={0};
	struct tm *today = NULL;
	struct timeb gtodtemp;
	int nLen = 0;
	static char strTimehd[32] = { 0 };

	ftime (&gtodtemp);
	today = localtime(&gtodtemp.time);
	strftime(psztimer,CPSS_MAX_TIME,"%Y-%m-%d %H:%M:%S",today);
	if (0 == strTimehd[0] || NULL == strstr(psztimer, strTimehd))
	{
		strncpy(strTimehd, psztimer, 9);
	}
	else
	{
		nLen = 0;
		while (0 != psztimer[9 + nLen])
		{
			psztimer[nLen] = psztimer[9+nLen];
			nLen++;
		}
		psztimer[nLen] = 0;
	}
	sprintf(sztimetmp,".%03d",gtodtemp.millitm);
	strcat(psztimer,sztimetmp);
/*	
	VOS_CHAR sztimer[CPSS_MAX_TIME]={0};	//BZERO(sztimer,CPSS_MAX_TIME);
	VOS_CHAR sztimetmp[CPSS_MAX_TIME]={0};	//BZERO(sztimetmp,CPSS_MAX_TIME);
	struct tm *today=NULL;
	struct timeb *gtodtemp=NULL;
	ftime (gtodtemp);
	//(t)->tv_usec = _gtodtmp.millitm * 1000; 
	today = localtime(gtodtemp->time);
	strftime(psztimer,CPSS_MAX_TIME,"%Y-%m-%d %H:%M:%S",today);
	sprintf(sztimetmp," %d",gtodtemp->millitm);
	strcat(psztimer,sztimetmp);
	free(gtodtemp);*/
	return VOS_OK;
}		/* -----  end of function cpss_get_current_time_to_str  ----- */

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_is_valid_timer_hdl
*  Description:	 定时器回调函数
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/

static VOS_INT32 cpss_is_valid_timer_hdl(ptimer_handle hdl)
{
	if(hdl == NULL)
	{
		return (0);
	}
	else
	{
		return (1);
	}    
}

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_traverse_mul_timer
*  Description:	 定时器回调函数
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
void CALLBACK cpss_traverse_mul_timer(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	struct _mul_timer_manage *ptimer;
	struct _mul_timer_entry *p, *ptmp;
	unsigned int timeout;
	
	ptimer = (struct _mul_timer_manage *)dwUser;
	if(ptimer == NULL)
	{
		return;
	}
	
	timeout = ptimer->time_unit;
	p = ptimer->phead;
	while(p != NULL)
	{
		if(p->timeout <= timeout)
		{
			p->is_use = 1;
			p->timer_proc(p->arg, p->arg_len);
			ptmp = p;
			timeout -= p->timeout;
			p = p->next;
			free(ptmp);
			ptimer->phead = p;
		}
		else
		{
			p->timeout -= timeout;
			p->elapse += timeout;
			ptimer->phead = p;
			break;
		}
	}
	if(p == NULL)
	{
		ptimer->phead = NULL;
	}
	return; 
}


/* ===  FUNCTION  ==============================================================
*         Name:  cpss_init_mul_timer
*  Description:	 初始化定时器
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_init_mul_timer(VOS_UINT32 time_unit)
{
	VOS_UINT32 uRet = VOS_ERR;
	struct _mul_timer_manage *p;

	p = (mul_timer_manage *)VOS_Malloc(sizeof(struct _mul_timer_manage),"msg queue timer");
	if( p == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Init Timer get Malloc Error");
		return uRet;
	}

	p->phead = NULL;
	p->timer_cnt = 0;
	p->time_unit = time_unit;
	p->entry_id = 0;

	uRet = VOS_Mutex_Init(&p->hMutex, "VOS_MUTEX_MSG_TIMER");
	if( p == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Init Timer mutex Error");
		return uRet;
	}

	p->timer_id = timeSetEvent(time_unit, 0, (LPTIMECALLBACK )cpss_traverse_mul_timer, (DWORD)p, TIME_PERIODIC);
	g_ptimer = p;

	return VOS_OK;
}

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
							 VOS_UINT32 *arg_len)
{
	struct _mul_timer_entry *p, *prev, *pnew;
	mul_timer_manage *ptimer = NULL;
	if (VOS_Mutex_Lock(&g_ptimer->hMutex))
	{
		VOS_PrintErr(__FILE__,__LINE__,"Set Timer lock error");
		return NULL;
	}
	ptimer = g_ptimer;
	if(ptimer == NULL || time_out == 0)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Set Timer input Error");
		return NULL;
	}

	pnew = (mul_timer_entry *)VOS_Malloc(sizeof(mul_timer_entry),"get timer entry");
	if( pnew == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Set Timer get Malloc Error");
		return (NULL);
	}
	pnew->is_use = 0;
	pnew->arg = arg;
	pnew->arg_len = arg_len;
	pnew->elapse = 0;
	pnew->timer_proc = timer_proc;
	
	p = ptimer->phead;
	prev = NULL;
	while(p != NULL)
	{
		if(p->timeout < time_out) /* assume the latest time_proc has higher priority */
		{
			time_out = time_out-p->timeout;
			prev = p;
			p = p->next;
		}
		else
		{
			p->timeout -= time_out;
			break;
		}
	}
	
	pnew->timeout = time_out;
	pnew->next = p;
	pnew->handle.ptr = (unsigned long )pnew;
	pnew->handle.entry_id = ptimer->entry_id;
	ptimer->entry_id++;
	
	if(prev == NULL)
	{
		ptimer->phead = pnew;
	}
	else
	{
		prev->next = pnew;
	}
	ptimer->timer_cnt++;
	
	if (VOS_Mutex_Unlock(&g_ptimer->hMutex))
	{
		VOS_PrintErr(__FILE__,__LINE__,"Set Timer lock error");
		return NULL;
	}
	return (&pnew->handle);
}

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_kill_timer
 *  Description:	 删除一个定时器
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_kill_timer(ptimer_handle hdl)
{
	VOS_UINT32 uRet = VOS_ERR;
	mul_timer_manage *ptimer = NULL;
	struct _mul_timer_entry *p, *prev;
	
	ptimer = g_ptimer;
	if(NULL == ptimer)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Kill Timer input Error");
		return uRet;
	}
	
	p = ptimer->phead;
	prev = NULL;
	while(p != NULL)
	{
		if(p->handle.ptr == hdl->ptr && p->handle.entry_id == hdl->entry_id)
		{
			break;
		}
		prev = p;
		p = p->next;
	}

	/* no such timer or timer is in use, return 0 */
	if(p == NULL || (p != NULL && p->is_use == 1)) 
	{
		VOS_PrintWarn(__FILE__,__LINE__,"Kill Timer not find handle %p:hdl:%p",p,hdl);
		return uRet; 
	}
	
	/* has found the timer */
	if(prev == NULL)
	{
		ptimer->phead = p->next;
	}
	else
	{
		prev->next = p->next;
	}
	
	/* revise timeout */
	if(p->next != NULL)
	{
		p->next->timeout += p->timeout;
	}
	
	/* delete the timer */
	VOS_Free(p,sizeof(mul_timer_entry));
	p = NULL;
	ptimer->timer_cnt--;
	uRet = VOS_OK;
	return uRet;
}

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 通过timer句柄得到剩余超时时间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_byhdl(ptimer_handle hdl)
{
	VOS_UINT32 uRet = VOS_ERR;
	struct _mul_timer_entry *p;
	VOS_UINT32 timeout;
	mul_timer_manage *ptimer = NULL;
	
	ptimer = g_ptimer;	
	if(ptimer == NULL || (struct _mul_timer_entry *)(hdl) == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Get Timer time out by handle input Error");
		return uRet;
	}
	timeout = 0;
	p = ptimer->phead;
	while(p != NULL)
	{
		if(p->handle.ptr == hdl->ptr && p->handle.entry_id == hdl->entry_id)
		{
			break;
		}
		timeout += p->timeout;
		p = p->next;
	}
	
	if(p == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"Get Timer time out by handle not find ");
		return uRet;
	}
	else
	{
		return timeout+p->timeout;
	}
}

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 通过timer回调函数得到剩余超时时间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_get_timeout_bytimeproc(VOS_UINT32 (* timer_proc) (VOS_VOID *arg, VOS_UINT32 *arg_len))
{
	VOS_UINT32 uRet = VOS_ERR;
	struct _mul_timer_entry *p;
	VOS_UINT32 timeout = 0;
	mul_timer_manage *ptimer = NULL;

	ptimer = g_ptimer;
	if(ptimer == NULL || timer_proc == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "timer global get timeout by timeproc NULL");
		return uRet;
	}
	
	p = ptimer->phead;
	while((p != NULL) && (p->timer_proc != timer_proc))
	{
		timeout += p->timeout;
		p = p->next;
	}
	
	if(p == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "timer by timeproc not find ");
		return uRet;
	}
	else
	{
		return (timeout+p->timeout);
	}
}

 /* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_timeout_byhdl
 *  Description:	 卸载定时器，删除内存空间
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_uninit_mul_timer()
{
	VOS_UINT32 uRet = VOS_ERR;
	mul_timer_entry *p, *ptmp;
	mul_timer_manage *ptimer = NULL;
	
	ptimer = g_ptimer;
	if(ptimer == NULL)
	{
		VOS_PrintErr(__FILE__, __LINE__, "timer uninit global variable is NULL");
		return uRet;
	}
	
	timeKillEvent(ptimer->timer_id);
	/* delete all timers */
	p = ptimer->phead;
	while(p != NULL)
	{
		ptmp = p;
		p = p->next;
		VOS_Free(ptmp,sizeof(mul_timer_entry));
	}
	
	uRet = VOS_Mutex_Destroy(&ptimer->hMutex);
	if (VOS_OK != uRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "timer uninit destory mutex erroe");
	}
	/* delete timer_manage */
	VOS_Free(ptimer,sizeof(mul_timer_entry));
	ptimer = NULL;	
	return VOS_OK;
}
