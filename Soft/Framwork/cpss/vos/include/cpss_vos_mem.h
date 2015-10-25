/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.h
 *
 *    Description:  内存管理模块头文件
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时34分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef _CPSS_VOS_MEM_H
#define _CPSS_VOS_MEM_H

#include "cpss_common.h"
#include "cpss_public.h"

typedef enum CPSS_MEM_BUFFER_OPERATE_M{
	CPSS_MEM_BUFFER_OPERATE_IN=1,
	CPSS_MEM_BUFFER_OPERATE_OUT,
};
typedef enum CPSS_MEM_BUFFER_STAT_M{
	CPSS_MEM_BUFFER_STAT_FREE=1,
	CPSS_MEM_BUFFER_STAT_RESE,
	CPSS_MEM_BUFFER_STAT_USEI,
	CPSS_MEM_BUFFER_STAT_USED,
};

typedef enum CPSS_MEM_BUFFER_TYPE_M{
	CPSS_MEM_BUFFER_TYPE_MSG=1,
	CPSS_MEM_BUFFER_TYPE_CMD,
};


typedef struct _CPSS_USE_MEM_MGR_T
{
	VOS_UINT8	uMem_Stat;				/**/
	VOS_UINT8	uMem_Type;				/**/
	VOS_UINT16	uMem_Resever;			/**/
	VOS_UINT32	uUsedCount;				/*used count*/
	CPSS_MEM_BUFFER * pstuUsedBufHead;	/*head */
	CPSS_MEM_BUFFER * pstuUsedBufTail;	/*tail */
}CPSS_USE_MEM_MGR, *pCPSS_USE_MEM_MGR;
typedef struct _CPSS_MSG_MEM_MANAGE_T
{
	VOS_MUTEX  hMutex;
	VOS_UINT32 uMemSize;
	VOS_MUTEX  hMutexBuffer;
	VOS_UINT32 nTotalCount;
	VOS_UINT32 nFreeCount;
	//CPSS_USE_MEM_MGR  pstuUsedMgr[128];
	CPSS_MEM_BUFFER * pstuFreeBufHead;
	CPSS_MEM_BUFFER * pstuFreeBufTail;
}CPSS_MSG_MEM_MANAGE,*pCPSS_MSG_MEM_MANAGE;

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_init_mem
 *  Description:  平台初始化内存管理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_init_mem();

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_print_mem_info
 *  Description:  打印内存空间
 * =============================================================================*/
VOS_VOID cpss_print_mem_info(VOS_STRING pstrStat);
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_mem_malloc
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_VOID * cpss_mem_malloc(VOS_INT32 ulSize, 
						   VOS_CHAR * strInfo, 
						   VOS_CHAR * strFile, 
						   VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  申请内存空间
 * =============================================================================*/
CPSS_MEM_BUFFER * cpss_get_mem_buffer(CPSS_MEM_BUFFER * pStuInput,VOS_UINT32 nNum);

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  得到下一个空间
 * =============================================================================*/
CPSS_MEM_BUFFER * cpss_get_next_buffer(CPSS_MEM_BUFFER * pStuInput);

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_mem_buffer
 *  Description:  申请内存空间
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_VOID cpss_free_mem_buffer(CPSS_MEM_BUFFER * pstuMemBuffer);

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_uninit_mem
 *  Description:  平台卸载内存管理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_uninit_mem();


#endif
