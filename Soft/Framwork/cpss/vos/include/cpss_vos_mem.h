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

typedef enum CPSS_MEM_RECORD_STAT_M{
	CPSS_MEM_RECORD_STAT_FREE = 1,
	CPSS_MEM_RECORD_STAT_RESE,
	CPSS_MEM_RECORD_STAT_USED,
};
typedef enum CPSS_MEM_BUFFER_TYPE_M{
	CPSS_MEM_BUFFER_TYPE_MSG=1,
	CPSS_MEM_BUFFER_TYPE_CMD,
};

typedef struct _CPSS_MEM_RECORD_T
{
	VOS_UINT32	nSize;		/*buferr 长度*/
	VOS_UINT8   nState;		/* malloc state*/
	VOS_VOID	*pstrVoid;	/*buffer 内容*/
	VOS_UINT32	nFileLine;	/*malloc line in file */
	VOS_CHAR	*strFileName; /*malloc in where*/
	struct _CPSS_MEM_RECORD_T * prev;
	struct _CPSS_MEM_RECORD_T * next;
}CPSS_MEM_RECORD, *pCPSS_MEM_RECORD;
typedef struct _CPSS_MEM_RECORD_HEAD_T
{
	VOS_UINT32	uMemSize;
	VOS_UINT32 nTotalCount;
	CPSS_MEM_RECORD * head;
	CPSS_MEM_RECORD * tail;
}CPSS_MEM_RECORD_HEAD, *pCPSS_MEM_RECORD_HEAD;

typedef struct _CPSS_MSG_MEM_MANAGE_T
{
	VOS_MUTEX  hMutex;
	VOS_UINT32 uMemSize;
	//VOS_MUTEX  hMutexBuffer;
	VOS_UINT32 nTotalCount;
	CPSS_MEM_RECORD_HEAD stuMemFHeadList;
	CPSS_MEM_RECORD_HEAD stuMemUHeadList[CPSS_MEM_HEAD_KEY_CPSS_TOTAL];
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
 * =============================================================================*/
VOS_VOID * cpss_mem_malloc(VOS_INT32 ulSize,
						   VOS_UINT32 nMemRdKey,
						   VOS_CHAR * strFile, 
						   VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_free
*  Description:  释放内存空间
* =============================================================================*/
VOS_UINT32 cpss_mem_free(VOS_UINT32 nMemRdKey, void * vAdress);

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
