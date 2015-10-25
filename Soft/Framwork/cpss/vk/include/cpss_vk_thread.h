/* ===========================================================================
 *
 *       Filename:  cpss_vk_thread.h
 *
 *    Description:  线程管理
 *
 *        Version:  1.0
 *        Created:  2011年06月17日 13时58分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ========================================================================== */
#ifndef CPSS_VK_THREAD_IN
#define CPSS_VK_THREAD_IN
#include "cpss_common.h"
#include "cpss_public.h"
#if   (OS_TYPE == OS_TYPE_WINDOW)
#include <process.h>
typedef VOS_UINT32 ( __stdcall * ThreadFun)( VOS_VOID *);
//typedef VOS_VOID * HANDLE;
typedef VOS_INT32 STACKSIZE;
#elif (OS_TYPE == OS_TYPE_LINUX)
#include <pthread.h>
typedef VOS_UINT32 ( * ThreadFun)( VOS_VOID *);
//typedef pthread_t * HANDLE;
typedef pthread_attr_t * STACKSIZE;
#endif
typedef struct _VOS_THREAD_MANAGE_T{
	VOS_UINT8	uInitFlg;
	VOS_UINT32	uIsExitFlg;
	VOS_UINT32	uThreadCount;					//线程计数器
	VOS_MUTEX	hMutex;							//线程计数器枷锁
	VOS_Event	hExitEvent;						//线程退出事件
}VOS_THREAD_MANAGE,*pVOS_THREAD_MANAGE;
typedef struct _VOS_PARAMATER_MANAGER_T{
	VOS_Event	* phEvent;
	VOS_UINT32	  pdwThreadId;
	HANDLE		  phandle;
	ThreadFun     FunProc;
	VOS_VOID	* lpParameter;
	VOS_CHAR	  strInfo[215];
}VOS_PARAMATER_MANAGER,*pVOS_PARAMATER_MANAGER;
typedef enum _THREAD_COUNT_MID_M{
	THREAD_COUNT_ADD=3,
	THREAD_COUNT_DEL,
	THREAD_INIT_FLG_OK,
};
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_createthread
 *  Description:	启动线程
 * =============================================================================*/
VOS_UINT32	cpss_create_thread(
	HANDLE *handle,			//线程ID
	STACKSIZE StackSize,	//优先级 初始栈大小
	ThreadFun FunProc,		//回调函数
	VOS_VOID * lpParameter,
	VOS_UINT32 * dwThreadId,
	VOS_STRING   pstrInfo);	//线程关键字
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_close_thread
 *  Description:	关闭线程
 * =============================================================================*/
VOS_UINT32	cpss_close_thread(
	HANDLE handle,							//线程ID
	VOS_UINT32 * dwThreadId);				// Thread address
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_check_thread_is_exit
 *  Description:	检查线程是要要退出
 * =============================================================================*/
VOS_UINT32 cpss_check_thread_is_exit(VOS_UINT32 ulType);
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_wait_thread_exit
 *  Description:	检查线程是要要退出
 * =============================================================================*/
VOS_UINT32 cpss_wait_thread_exit();
/*===  FUNCTION  ===============================================================
*         Name:  cpss_thread_success
*  Description:	线程启动成功
* =============================================================================*/
VOS_UINT32	cpss_thread_success(
	VOS_STRING   pstrInfo);	//线程关键字

#endif

