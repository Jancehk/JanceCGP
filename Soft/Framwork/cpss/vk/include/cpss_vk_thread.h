/* ===========================================================================
 *
 *       Filename:  cpss_vk_thread.h
 *
 *    Description:  �̹߳���
 *
 *        Version:  1.0
 *        Created:  2011��06��17�� 13ʱ58��20��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
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
	VOS_UINT32	uThreadCount;					//�̼߳�����
	VOS_MUTEX	hMutex;							//�̼߳���������
	VOS_Event	hExitEvent;						//�߳��˳��¼�
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
 *  Description:	�����߳�
 * =============================================================================*/
VOS_UINT32	cpss_create_thread(
	HANDLE *handle,			//�߳�ID
	STACKSIZE StackSize,	//���ȼ� ��ʼջ��С
	ThreadFun FunProc,		//�ص�����
	VOS_VOID * lpParameter,
	VOS_UINT32 * dwThreadId,
	VOS_STRING   pstrInfo);	//�̹߳ؼ���
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_close_thread
 *  Description:	�ر��߳�
 * =============================================================================*/
VOS_UINT32	cpss_close_thread(
	HANDLE handle,							//�߳�ID
	VOS_UINT32 * dwThreadId);				// Thread address
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_check_thread_is_exit
 *  Description:	����߳���ҪҪ�˳�
 * =============================================================================*/
VOS_UINT32 cpss_check_thread_is_exit(VOS_UINT32 ulType);
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_wait_thread_exit
 *  Description:	����߳���ҪҪ�˳�
 * =============================================================================*/
VOS_UINT32 cpss_wait_thread_exit();
/*===  FUNCTION  ===============================================================
*         Name:  cpss_thread_success
*  Description:	�߳������ɹ�
* =============================================================================*/
VOS_UINT32	cpss_thread_success(
	VOS_STRING   pstrInfo);	//�̹߳ؼ���

#endif

