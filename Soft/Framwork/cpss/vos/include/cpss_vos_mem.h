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
	CPSS_MEM_RECORD_STAT_REMV,
	CPSS_MEM_RECORD_STAT_USED,
};

typedef struct _CPSS_MEM_RECORD_T
{
	VOS_UINT32	nTagID;					/* mem manager tag ID */
	VOS_UINT32	nSize;					/* buferr 长度 */
	VOS_UINT8   nState;					/* malloc state */
	VOS_VOID	*pstrVoid;				/* buffer 内容 */
	VOS_UINT32	nFileLine;				/* malloc line in file */
	VOS_CHAR	strFileName[32];		/* malloc in where */
	struct _CPSS_MEM_RECORD_T * prev;
	struct _CPSS_MEM_RECORD_T * next;
}CPSS_MEM_RECORD, *pCPSS_MEM_RECORD;
typedef struct _CPSS_MEM_RECORD_HEAD_T
{
	VOS_UINT32	uMemSize;		/* mem size */
	VOS_UINT32 nTotalCount;		/* mem record count */
	CPSS_MEM_RECORD * head;
	CPSS_MEM_RECORD * tail;
}CPSS_MEM_RECORD_HEAD, *pCPSS_MEM_RECORD_HEAD;

typedef struct _CPSS_MSG_MEM_MANAGE_T
{
	VOS_MUTEX  hMutex;
	VOS_UINT32 uMemSize;
	VOS_UINT32 nTotalCount;
	VOS_UINT32 rfu;
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
*         Name:  cpss_mem_calloc
*  Description:  申请内存空间
* =============================================================================*/
VOS_VOID * cpss_mem_calloc(VOS_INT32 ulSize,
	VOS_UINT32 nMemRdKey,
	VOS_CHAR * strFile,
	VOS_INT32 nLine);
/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_realloc
*  Description:
* =============================================================================*/
VOS_VOID * cpss_mem_realloc(VOS_UINT32 nMemRdKey, void * vAdress, VOS_INT32 ulSize, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_reset
*  Description:  设置内存管理信息
* =============================================================================*/
VOS_VOID * cpss_mem_reset(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_cat
*  Description:  连接两段内存
* =============================================================================*/
VOS_VOID * cpss_mem_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine);
/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_reset
*  Description:  得到memory的大小
* =============================================================================*/
VOS_UINT32 cpss_mem_getsize(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_free
*  Description:  释放内存空间
* =============================================================================*/
VOS_UINT32 cpss_mem_free(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);


/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_getsize
*  Description:  清空内存数据
* =============================================================================*/
VOS_UINT32 cpss_mem_cls(VOS_UINT32 nMemRdKey, void * vAdress, VOS_UINT32 nMemSize, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_str_cat
*  Description:  字符串连接
* =============================================================================*/
VOS_CHAR* cpss_str_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine);

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_uninit_mem
 *  Description:  平台卸载内存管理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_uninit_mem();


#endif
