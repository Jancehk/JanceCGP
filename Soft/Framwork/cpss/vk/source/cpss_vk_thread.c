/* ===========================================================================
 *
 *       Filename:  cpss_vk_thread.c
 *
 *    Description:  线程管理模块
 *
 *        Version:  1.0
 *        Created:  2011年06月17日 13时54分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ==========================================================================*/
#include "cpss_vk_thread.h"
#include "cpss_vk_print.h"
#include "cpss_msg.h"
VOS_THREAD_MANAGE g_thread_manage = {0};
THREAD VOS_PARAMATER_MANAGER g_stuParamater = {0};
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init_thread
 *  Description:	启动线程
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static VOS_UINT32	cpss_init_thread()
{
	VOS_UINT32 ulRet = VOS_ERR;
	
	ulRet = VOS_Mutex_Init(&g_thread_manage.hMutex, "VOS_MUTEX_THREAD_TRACE");
	if (VOS_OK != ulRet)
	{
		printf("cpss init print trace mutex error\n");
		return ulRet;
	}
	
	ulRet = VOS_Init_Event(&g_thread_manage.hExitEvent, "VOS_THREAD_INFO_STRING");
	if (VOS_OK != ulRet)
	{
		printf("cpss init print log event error\n");
		return ulRet;
	}
	g_thread_manage.uThreadCount=0;
	g_thread_manage.uInitFlg = THREAD_INIT_FLG_OK;
	g_thread_manage.uIsExitFlg = VOS_ERR;
	return ulRet;
}

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init_thread
 *  Description:	启动线程
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static VOS_UINT32	cpss_add_del_thread(VOS_UINT32 ulType)
{
	VOS_UINT32 ulRet = VOS_ERR;
	ulRet = VOS_Mutex_Lock(&g_thread_manage.hMutex);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss thread count lock faild");
		goto END_PROC;
	}
	if (THREAD_COUNT_ADD == ulType)
	{
		g_thread_manage.uThreadCount ++;
	}else
	if (THREAD_COUNT_DEL == ulType)
	{
		g_thread_manage.uThreadCount --;
	}
	else
	{
		VOS_PrintErr(__FILE__, __LINE__, "thread count type is error ：%d",ulType);
	}
END_PROC:
	ulRet = VOS_Mutex_Unlock(&g_thread_manage.hMutex);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss thread count unlock faild");
	}
	return ulType;
}
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_createthread
 *  Description:	启动线程
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/

#if (OS_TYPE == OS_TYPE_WINDOW)
VOS_UINT32 __stdcall  cpss_open_thread_proc (VOS_VOID * lpParameter)
#elif (OS_TYPE == OS_TYPE_LINUX)
VOS_UINT32 cpss_open_thread_proc (VOS_VOID * lpParameter)
#endif
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32	  dwThreadId;
	HANDLE		  hThread;
	ThreadFun     FunProc;
	VOS_VOID	* pParameter;
	VOS_Event   * phEvent=NULL;
	VOS_CHAR	  strInfo[215]={0};
	pVOS_PARAMATER_MANAGER pstuParamater = (pVOS_PARAMATER_MANAGER)lpParameter;

	if (NULL == pstuParamater)
	{
		printf("cpss_print_trace_proc parameter is NULL");
		return VOS_ERR;
	}
	if (NULL == pstuParamater->lpParameter)
	{
		printf("cpss_print_trace_proc Parameter is NULL");
		goto END_PROC;
	}
	if (NULL == pstuParamater->FunProc)
	{
		printf("cpss_print_trace_proc Funproc is NULL");
		goto END_PROC;
	}
	while(0 == pstuParamater->pdwThreadId ||
		  0 == pstuParamater->phandle)
	{
		VOS_Sleep(1);
	}
	dwThreadId = pstuParamater->pdwThreadId;
	hThread = pstuParamater->phandle;
	FunProc = pstuParamater->FunProc;
	phEvent = pstuParamater->phEvent;
	pParameter = pstuParamater->lpParameter;
	cpss_init_print(dwThreadId);
	if (0 != pstuParamater->strInfo[0])
	{
		if (0 != VOS_Strcmp(pstuParamater->strInfo, "Log"))
		{
			VOS_Strcpy(strInfo, pstuParamater->strInfo);
			VOS_PrintInfo(__FILE__, __LINE__, "%s Init", strInfo);
		}
	}
	else
	{
		VOS_PrintErr(__FILE__, __LINE__, "thread name is NULL");
		goto END_PROC;
	}

	ulRet = VOS_Set_Event(phEvent);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "thread set event is timeout");
		goto END_PROC;
	}
	ulRet = FunProc(pParameter);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "thread FunProc Faild");
		goto END_PROC;
	}
	if (0 != strInfo[0])
	{
		VOS_PrintInfo(__FILE__,__LINE__,"the thread %s close ",strInfo);
	}
	//VOS_PrintInfo(__FILE__, __LINE__, "thread [%d] is exit",hThread);
END_PROC:
	cpss_close_thread(hThread, &dwThreadId);
	return ulRet;
}

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_createthread
 *  Description:	创建线程
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32	cpss_create_thread
(
	HANDLE     * handle,							//线程ID
	STACKSIZE    StackSize,					//优先级 初始栈大小
	ThreadFun    FunProc,						//回调函数
	VOS_VOID   * lpParameter,					//入参
	VOS_UINT32 * dwThreadId,				// Thread address
	VOS_STRING   pstrInfo)				
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 udwThreadId = 0;
	HANDLE     nHandle = 0;
	/**/
	if (THREAD_INIT_FLG_OK != g_thread_manage.uInitFlg)
	{
		if (VOS_OK != cpss_init_thread())
		{
			printf("init thread Error");
			return 0;
		}
	}
	ulRet = VOS_Mutex_Lock(&g_thread_manage.hMutex);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss thread count lock faild");
		goto END_PROC;
	}
	/*VOS_Memset(&g_stuParamater, sizeof(VOS_PARAMATER_MANAGER));*/

	g_stuParamater.lpParameter = lpParameter;
	g_stuParamater.phEvent     = &g_thread_manage.hExitEvent;
	g_stuParamater.FunProc     = FunProc;
	if (0 != *pstrInfo)
	{
		VOS_Strcpy(g_stuParamater.strInfo, pstrInfo);
	}
#if   (OS_TYPE == OS_TYPE_WINDOW)
	nHandle =(HANDLE)_beginthreadex(NULL,					// Security
								   StackSize,				// Stack size - use default
								   cpss_open_thread_proc,	// Thread fn entry point
								   &g_stuParamater,			//lpParameter
								   0,						// Init flag
								   &udwThreadId);				// Thread address
	if (NULL == nHandle)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss thread create  faild");
		goto END_PROC;
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_create(nHandle, StackSize, cpss_open_thread_proc, lpParameter);
#endif
	*dwThreadId = g_stuParamater.pdwThreadId = udwThreadId;
	*handle = g_stuParamater.phandle     = nHandle;
	ulRet = VOS_Wait_Event(&g_thread_manage.hExitEvent, INFINITE);
	if (VOS_OK != ulRet)
	{
		printf("cpss init print log event error\n");
		goto END_PROC;
	}
END_PROC:
	ulRet = VOS_Mutex_Unlock(&g_thread_manage.hMutex);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "cpss thread count lock faild");
	}
	if (VOS_OK == ulRet)
	{
		cpss_add_del_thread(THREAD_COUNT_ADD);
	}
	return VOS_OK;
}			/* -----  end of function VOS_CreateThread  ----- */

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_closethread
 *  Description:	启动线程
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32	cpss_close_thread
(
	HANDLE handle,							//线程ID
	VOS_UINT32 * dwThreadId)				// Thread address
{
	VOS_UINT32 ulRet = VOS_OK;
	if (NULL == handle ||
		NULL == dwThreadId)
	{
		VOS_PrintInfo(__FILE__, __LINE__, "close thrad paramater is null");
		return ulRet;
	}
#if   (OS_TYPE == OS_TYPE_WINDOW)
	if (0 != *dwThreadId)
	{
	//	CloseHandle(handle);
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	if (0 != *dwThreadId)
	{
		_endthreadex(*dwThreadId);
	}
//	pthread_create(&handle, StackSize, FunProc, lpParameter);
#endif
	cpss_add_del_thread(THREAD_COUNT_DEL);
	ulRet = VOS_Set_Event(&g_thread_manage.hExitEvent);
	if (VOS_OK != ulRet)
	{
		printf("cpss init print log event error\n");
		return ulRet;
	}
	return ulRet;
}			/* -----  end of function cpss_closethread  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_check_thread_is_exit
 *  Description:	检查线程是要要退出
 * =============================================================================*/
VOS_UINT32 cpss_check_thread_is_exit(VOS_UINT32 ulType)
{
	if (1 == ulType)
	{
		if (VOS_OK == g_thread_manage.uIsExitFlg &&
			1 == g_thread_manage.uThreadCount)
		{
			VOS_PrintInfo(__FILE__, __LINE__, "\n================\nCGP Exit is OK\n==============");
		}
		if (1 < g_thread_manage.uThreadCount)
		{
			return VOS_ERR;
		}
	}
	return g_thread_manage.uIsExitFlg;
}
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_wait_thread_exit
 *  Description:	检查线程是要要退出
 * =============================================================================*/
VOS_UINT32 cpss_wait_thread_exit()
{
	VOS_UINT32 ulRet = VOS_OK;
	g_thread_manage.uIsExitFlg = VOS_OK;
	ulRet = cpss_subsystem_init(cps_set_msg_type(CPSS_REQUEST_SYSTEM, CPSS_TYPE_SYS, CPSS_MSG_UNIT), CPSS_CMD_SYSTEM_UNIT);
	if (VOS_OK != ulRet)
	{
		printf("wait exit time out\n");
	}
	while (g_thread_manage.uThreadCount > 0)
	{
		ulRet = VOS_Wait_Event(&g_thread_manage.hExitEvent,INFINITE);
		if (VOS_OK != ulRet)
		{
			printf("wait exit time out\n");
		}
	}
	return ulRet;
}

/*===  FUNCTION  ===============================================================
*         Name:  cpss_thread_success
*  Description:	线程启动成功
* =============================================================================*/
VOS_UINT32	cpss_thread_success(
	VOS_STRING   pstrInfo)
{
	if (NULL == pstrInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "%p proc Init Error", pstrInfo);
		return VOS_ERR;
	}
	VOS_PrintInfo(__FILE__, __LINE__, "%s Begin", pstrInfo);
	return VOS_OK;
}