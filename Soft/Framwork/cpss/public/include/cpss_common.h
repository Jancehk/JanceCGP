#ifndef VOS_COMMON
#define VOS_COMMON
#ifdef _cplusplus
extern "C" {
#endif
#if 1
#ifndef OS_TYPE
#define OS_TYPE_LINUX	1
#define OS_TYPE_WINDOW	2
#define OS_TYPE	OS_TYPE_WINDOW
#endif
#endif
#if (OS_TYPE == OS_TYPE_WINDOW)
#pragma warning (disable:4996)
#pragma warning (disable:4013)
#elif (OS_TYPE == OS_TYPE_LINUX)
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <sys/types.h>
//#include <fcnt1.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <IO.H>
#include <time.h>
#if (OS_TYPE == OS_TYPE_WINDOW)
#include <WINDOWS.H>
#elif (OS_TYPE == OS_TYPE_LINUX)
#include <pthread.h>
#endif
#include "cpss_default.h"
#include "cpss_public.h"
	
#if (OS_TYPE == OS_TYPE_WINDOW)
#define _THREAD_ _declspec(thread)
#elif (OS_TYPE == OS_TYPE_LINUX)
#define _THREAD_ __thread
#endif
#define THREAD _THREAD_

#define  CPSS_MSG_BUFFER_SIZE	1024

typedef struct _CPSS_CPUID_HEADER_T{
	VOS_UINT32 ulCount;
	VOS_UINT32 rfu[3];
}CPSS_CPUID_HEADER, *pCPSS_CPUID_HEADER;

typedef struct _CPSS_CPUID_T{
	VOS_UINT32 ulCPuID;
	VOS_UINT32 ulPid;
}CPSS_CPUID, *pCPSS_CPUID;

typedef struct _CPSS_CPUID_INFO_T{
	VOS_UINT32 ulSystemID;
	VOS_UINT32 ulSubsysID;
	CPSS_CPUID stuCPuid;
	//VOS_UINT32 ulCPuID;
	//VOS_UINT32 ulPid;
}CPSS_CPUID_INFO,*pCPSS_CPUID_INFO;


typedef struct _CONFIG_TAB_T
{
	VOS_UINT8 uSocketConfig;
	VOS_UINT8 uAddress;
	struct _CONFIG_TAB_T *next;
}CONFIG_TAB,*pCONFIG_TAB;

/*
 *		用户信息结构体
*/
typedef struct _CPSS_USER_INFO_T{
	VOS_VOID *	pstrVoidCli;
	VOS_UINT32	ulLevel;
	VOS_CHAR	strUser[CPSS_CLIENT_NAME_LENGTH];
	VOS_CHAR	strPass[CPSS_CLIENT_PASS_LENGTH];
	VOS_UINT32	ulResult;
}CPSS_USER_INFO, *pCPSS_USER_INFO;

typedef struct CPSS_CLT_CMD_T
{
	VOS_UINT32  nCmdID;
	VOS_CHAR	strCmdBuff[CPSS_MSG_BUFFER_SIZE];
	VOS_UINT32	nCmdLength;
	struct CPSS_CLT_CMD_T * prev;
	struct CPSS_CLT_CMD_T * next;
}CPSS_CLT_CMD, CPSS_CLT_CMD;
//客户信息
#define CPSS_CLIENT_IPADS_LENGTH	32
typedef struct CPSS_CLIENT_INFO_T
{
	VOS_UINT32			ulID;									// 客户 的序号
	VOS_UINT8			nStat;									// socket的状态 空闲 预约 使用 使用完毕 
	VOS_UINT8			nType;									// msg   的状态
	VOS_UINT8			nLineStat;								// 登录状态 0 没有登录用户 1 设置用户名 2 已经登录
	VOS_UINT8			uClientStat;							// current Client stat
	VOS_UINT32			dwTThreadId;
	SOCKADDR_IN			clientaddr;								//Client Addr
	CPSS_USER_INFO		stuUserInfo;
	VOS_CHAR			strIPaddress[CPSS_CLIENT_IPADS_LENGTH];
	VOS_VOID *			pAccptSocket;					// Accept Parent Socket Handle
	VOS_VOID *			pClientSocket;					// Client Socket Handle
	VOS_UINT32			ulCientHaveTime;
	VOS_VOID *			pCmdEvent;						//开始等待数据
	VOS_UINT8			bIsEvent;						//是否处理完数据 0 没有接受完 1 接受完数据
	VOS_VOID		 *	msg;
	CPSS_CLT_CMD		stuCmdLink;
	VOS_UINT32			nCmdConut;
	struct CPSS_CLIENT_INFO_T *prev;
	struct CPSS_CLIENT_INFO_T *next;
	VOS_CHAR			*pstuBuffer;
	VOS_UINT32			nBufferLeng;
}CPSS_CLIENT_INFO, *pCPSS_CLIENT_INFO;

//常用库函数

#define VOS_Memset(pstaddres, nVal, ulSize)			memset((pstaddres), (nVal) ,(ulSize))
#define VOS_Memcpy(pstSou,pstDes,ulSize)			memcpy((pstSou), (pstDes), (ulSize))
#define VOS_Memcmp(pstSou,pstDes,ulSize)			memcmp((pstSou), (pstDes), (ulSize))
#define VOS_Strcpy(pstSou,pstDes)					strcpy((pstSou), (pstDes))
#define VOS_Strncpy(pstSou,pstDes,nLen)				strncpy((pstSou), (pstDes),(nLen))
#define VOS_Strlen(pstSou)							strlen((pstSou))
#define VOS_Strcat(pstSou,pstDes)					strcat((pstSou), ((const char *)pstDes))
#define VOS_Strstr(pstSou,pstDes)					strstr(((const char *)pstSou), ((const char *)pstDes))
#define VOS_Strcmp(pstSou,pstDes)					strcmp(((const char *)pstSou), ((const char *)pstDes))
#define BZERO(pszStr,nSize)							memset((pszStr),(0),(nSize))

#define VOS_Malloc(ulSize,strInfo)					cpss_mem_malloc((ulSize), (strInfo), (__FILE__),(__LINE__))
#define VOS_Calloc(ulSize,strInfo)					cpss_mem_calloc((ulSize), (strInfo), (__FILE__),(__LINE__))
#define VOS_Realloc(pstrads,ulSize,strInfo)			cpss_mem_realloc((strInfo), (pstrads), (ulSize), (__FILE__),(__LINE__))
#define VOS_Remset(pstrads,strInfo)					cpss_mem_reset((strInfo), (pstrads),(__FILE__),(__LINE__))
#define VOS_Memcls(pstrads, ulSize, strInfo)		cpss_mem_cls((strInfo), (pstrads), (ulSize),(__FILE__),(__LINE__))
#define VOS_Memcat(pstradsA, pstradsB, strInfo)		cpss_mem_cat((strInfo), (pstradsA), (pstradsB),(__FILE__),(__LINE__))
#define VOS_MemcatEx(pstradsA, pstradsB, ulSize, strInfo)	cpss_mem_catex((strInfo), (pstradsA), (pstradsB), (ulSize), (__FILE__),(__LINE__))
#define VOS_MemcatEx2(pstradsA, pstradsB, ulSize, strInfo)	cpss_mem_catex2((strInfo), (pstradsA), (pstradsB), (ulSize), (__FILE__),(__LINE__))
#define VOS_Memsize(pstrads,strInfo)				cpss_mem_getsize((strInfo), (pstrads),(__FILE__),(__LINE__))
#define VOS_Free(pstrads,strInfo)					cpss_mem_free((strInfo), (pstrads),(__FILE__),(__LINE__))
#define VOS_MemErrNo()								cpss_mem_get_lasterr()

#define VOS_CpsStrcat(pstradsA, pstradsB, strInfo)	cpss_str_cat((strInfo), (pstradsA), (pstradsB),(__FILE__),(__LINE__))


/* ===  FUNCTION  =================================================================
 *         Name:  VOS_RegistPidInit
 *  Description:  pid加载初始化
 * ===============================================================================*/
extern VOS_UINT32 VOS_RegistPidInit(
	   VOS_UINT32 ulSubSystem,
	   VOS_UINT32 ulProcessPid,
	   const VOS_CHAR * ulProcessPidName,
	   VOS_UINT32 ulProcessCount,
	   VOS_UINT32 (* ppid_init_proc) (VOS_VOID *parg),
	   VOS_UINT32 (* ppid_timeout_proc) (VOS_VOID *pargc, VOS_UINT32 pargv));	/*end of function VOS_RegistPidInit -*/
/*===  FUNCTION  ============================================================
 *         Name:  VOS_ProcessInit
 *  Description:	子系统初始化入口
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * ==========================================================================*/
extern VOS_UINT32 VOS_ProcessInit ();

/*===  FUNCTION  ==============================================================
 *         Name:  cpss_print
 *  Description:	平台打印函数
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
extern VOS_UINT32 cpss_print(
		VOS_UINT32 	ucProcessPid,		//子模块ID
		VOS_UINT32  ulPrintType,			//日志类型(INFO  ERR  WARM)
		VOS_STRING  szFilename,			//打印日志文件名称
		VOS_UINT32  ulLine,				//打印的行数
		VOS_STRING 	szFormat,
		va_list argptr);				//日志内容
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintBuffer
 *  Description:  
 * ========================================================================*/
extern VOS_UINT32 VOS_PrintBuffer (
		VOS_VOID * pstuBuffer,
		const VOS_STRING fmt, ...);

/* ===  FUNCTION  =========================================================
*         Name:  VOS_PrintBufferRelease
*  Description:
* ========================================================================*/
extern VOS_UINT32 VOS_PrintBufferRelease(VOS_VOID * pstuBuffer);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim
 *  Description:   左右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_STRING cpss_trim(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_left
 *  Description:   右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_STRING cpss_trim_left(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_right
 *  Description:   左截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_STRING cpss_trim_right(VOS_STRING str, VOS_STRING sep);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_split
 *  Description:   分割
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_UINT32 cpss_split(VOS_STRING str, VOS_STRING sep, VOS_INT32 flag);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_getline
 *  Description:   得到一行
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_STRING cpss_getline(VOS_STRING pbuf, VOS_STRING pline, VOS_INT32 skip);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_getline
 *  Description:   得到一行
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_STRING cpss_getver();

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 * ==========================================================================*/
extern VOS_UINT32 cpss_send_data (VOS_VOID *pVoidMsg, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 * ==========================================================================*/
VOS_UINT32 cpss_send_data_msgid (VOS_UINT32 ulMsgID, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_current_path
 *  Description:	  
 * =============================================================================*/
extern VOS_UINT32 cpss_get_current_path (VOS_CHAR * pSelfPath);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid
 *  Description:  得到cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_UINT32 cpss_get_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 ulType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_set_cpuid_pid
 *  Description:  设置cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_UINT32 cpss_set_cpuid_pid (VOS_UINT32 ulSubSys,
		VOS_UINT32 ulSubSysID,
	   VOS_UINT32 IsShowflg, 
	   VOS_UINT32 ulValue, 
	   VOS_UINT32 ulType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid_to_buffer
 *  Description:  得到cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
extern VOS_UINT32 cpss_get_cpuid_pid_to_buffer (
	   VOS_UINT32 ulType,
	   VOS_STRING strBuffer);
/*===  FUNCTION  ===============================================================
*         Name:  cpss_get_file_data
*  Description:  得到文件的内容
*  Input      :
*  OutPut     :
*  Return     :
* =============================================================================*/
extern VOS_CHAR* cpss_get_file_data(VOS_CHAR * pstrPath);

/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_catex
*  Description:  设置内存管理信息 bin+bin
* =============================================================================*/
extern VOS_VOID * cpss_mem_catex(
	VOS_UINT32 nMemRdKey,
	VOS_VOID * vAdressA,
	VOS_VOID * vAdressB,
	VOS_UINT32 nLen,
	VOS_CHAR * strFile,
	VOS_INT32 nLine);
/*===  FUNCTION  ==============================================================
*         Name:  cpss_mem_catex2
*  Description:  设置内存管理信息  str+bin
* =============================================================================*/
extern VOS_VOID * cpss_mem_catex2(
	VOS_UINT32 nMemRdKey,
	VOS_VOID * vAdressA,
	VOS_VOID * vAdressB,
	VOS_UINT32 nLen,
	VOS_CHAR * strFile,
	VOS_INT32 nLine);
#ifdef _cplusplus
}
#endif
#endif
