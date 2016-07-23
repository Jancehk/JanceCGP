/*
 * =====================================================================================
 *
 *       Filename:  cpss_vos_sem.h
 *
 *    Description:  �ڴ����ģ��ͷ�ļ�
 *
 *        Version:  1.0
 *        Created:  2011��06��09�� 23ʱ34��39��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
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
	VOS_UINT32	nSize;					/* buferr ���� */
	VOS_UINT8   nState;					/* malloc state */
	VOS_VOID	*pstrVoid;				/* buffer ���� */
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
 *  Description:  ƽ̨��ʼ���ڴ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_init_mem();

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_get_print_mem_info
 *  Description:  ��ӡ�ڴ�ռ�
 * =============================================================================*/
VOS_VOID cpss_print_mem_info(VOS_STRING pstrStat);
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_mem_malloc
 *  Description:  �����ڴ�ռ�
 * =============================================================================*/
VOS_VOID * cpss_mem_malloc(VOS_INT32 ulSize,
						   VOS_UINT32 nMemRdKey,
						   VOS_CHAR * strFile, 
						   VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_calloc
*  Description:  �����ڴ�ռ�
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
*  Description:  �����ڴ������Ϣ
* =============================================================================*/
VOS_VOID * cpss_mem_reset(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_cat
*  Description:  ���������ڴ�
* =============================================================================*/
VOS_VOID * cpss_mem_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine);
/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_reset
*  Description:  �õ�memory�Ĵ�С
* =============================================================================*/
VOS_UINT32 cpss_mem_getsize(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_free
*  Description:  �ͷ��ڴ�ռ�
* =============================================================================*/
VOS_UINT32 cpss_mem_free(VOS_UINT32 nMemRdKey, void * vAdress, VOS_CHAR * strFile, VOS_INT32 nLine);


/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_getsize
*  Description:  ����ڴ�����
* =============================================================================*/
VOS_UINT32 cpss_mem_cls(VOS_UINT32 nMemRdKey, void * vAdress, VOS_UINT32 nMemSize, VOS_CHAR * strFile, VOS_INT32 nLine);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_str_cat
*  Description:  �ַ�������
* =============================================================================*/
VOS_CHAR* cpss_str_cat(VOS_UINT32 nMemRdKey, void * vAdressA, void * vAdressB, VOS_CHAR * strFile, VOS_INT32 nLine);

/* ===  FUNCTION  ==============================================================
 *         Name:  cps_uninit_mem
 *  Description:  ƽ̨ж���ڴ����
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_uninit_mem();


#endif
