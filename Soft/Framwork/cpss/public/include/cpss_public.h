/* ===========================================================================
 *
 *       Filename:  cpss_public.h
 *
 *    Description: 公共函数定义 
 *
 *        Version:  1.0
 *        Created:  2011年06月17日 07时28分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ========================================================================== */
#ifndef CPSS_PUBLIC_IN
#define CPSS_PUBLIC_IN
#include "cpss_common.h"
#define MAX_PATH	260

#define VOS_LOCK_CRITE 1
#define VOS_LOCK_MUTEX 2
#define VOS_LOCK_EVENT 3
#define VOS_LOCK_TYPE VOS_LOCK_EVENT
#define VOS_MUTEX_LEN	64

typedef struct _VOS_MUTEX_T
{
#if (OS_TYPE == OS_TYPE_WINDOW)	
#if VOS_LOCK_TYPE == VOS_LOCK_CRITE
	CRITICAL_SECTION mcs;
#elif VOS_LOCK_TYPE == VOS_LOCK_MUTEX
	HANDLE hMutex;
#elif VOS_LOCK_TYPE == VOS_LOCK_EVENT
	HANDLE hEvent;
#endif
	VOS_CHAR strmutex[VOS_MUTEX_LEN];
	DWORD mutexRet;
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_mutex_t hMutex;
#endif
}VOS_MUTEX,*pVOS_MUTEX;
typedef struct _VOS_Event_T
{	
#if (OS_TYPE == OS_TYPE_WINDOW)
	VOS_CHAR strmutex[VOS_MUTEX_LEN];
	HANDLE hEvent;
#elif (OS_TYPE == OS_TYPE_LINUX)
	sem_t sem; 
#endif
}VOS_Event,*pVOS_Event;
#define TRIME_KEY	(" \t\n\r")

typedef enum CPSS_MEM_BUFFER_TYPE_M{
	CPSS_MEM_HEAD_KEY_CPSS_LOG=0,
	CPSS_MEM_HEAD_KEY_CPSS_SKT,
	CPSS_MEM_HEAD_KEY_CPSS_MSG,
	CPSS_MEM_HEAD_KEY_CPSS_PID,
	CPSS_MEM_HEAD_KEY_CPSS_SHELL,
	CPSS_MEM_HEAD_KEY_CPSS_TLN,
	CPSS_MEM_HEAD_KEY_CPSS_FILE,
	CPSS_MEM_HEAD_KEY_CPSS_TMER,
	XCAP_MEM_HEAD_KEY_URL_XCAP,
	CPSS_MEM_HEAD_KEY_CPSS_TOTAL,
};

/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_ip
 *  Description:	32位ip转换为cpuid
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_cpuid_from_ip(VOS_UINT32 ulIp);
/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_str
 *  Description:	字符串ip转换为cpuid
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_cpuid_from_str(VOS_CHAR* pszIp);
/*===  FUNCTION  ==============================================================
 *         Name:  get_ip_from_cpuid
 *  Description:	32位cpuid转换为ip
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_ip_from_cpuid(VOS_UINT32 ulCpuid);
/* -----  end of function VOS_ProcessInit  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_str_before
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_get_str_before (VOS_STRING pszInputStr, VOS_STRING szFind);
/* -----  end of function cpss_get_str_before  ----- */

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_is_dir
*  Description:  检查是否为目录
* ==========================================================================*/
VOS_INT32 cpss_is_dir(const VOS_CHAR *filename);

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_file_exists
*  Description:  检查文件是否存在
* ==========================================================================*/
VOS_INT32 cpss_file_exists(const VOS_CHAR *filename);

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Init
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex     
				   VOS_CHAR  * pstr
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Init(VOS_MUTEX * pMutex, VOS_CHAR * pstr);
/* -----  end of function VOS_Mutex_Init  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Lock
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex  
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Lock(VOS_MUTEX * pMutex);
/* -----  end of function VOS_Mutex_Lock  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Unlock
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Unlock(VOS_MUTEX * pMutex);
/* -----  end of function VOS_Mutex_Unlock  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Destroy
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Destroy(VOS_MUTEX * pMutex);
/* -----  end of function VOS_Mutex_Destroy  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Init_Event
 *  Description:	初始化Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Init_Event(VOS_Event * pEvent,VOS_STRING pStrEnevt);
/* -----  end of function VOS_Init_Event  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Set_Event
 *  Description:	设置 Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Set_Event(VOS_Event * pEvent);
/* -----  end of function VOS_Set_Event  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Wait_Event
 *  Description:	等待Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Wait_Event(VOS_Event * pEvent, VOS_UINT32 uTime);

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Destroy_Event
 *  Description:	销毁Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Destroy_Event(VOS_Event * pEvent, VOS_UINT32 uTime);
/* -----  end of function VOS_Destroy_Event  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim
 *  Description:   左右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_trim(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_left
 *  Description:   右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_trim_left(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_right
 *  Description:   左截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_trim_right(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_split
 *  Description:   分割
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_split(VOS_STRING str, VOS_STRING sep, VOS_INT32 flag);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_getline
 *  Description:   得到一行
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_getline(VOS_STRING pbuf, VOS_STRING pline, VOS_INT32 skip);

#endif

