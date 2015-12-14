/*
 * =====================================================================================
 *
 *       Filename:  cpss_com_pid.h
 *
 *    Description:  PID信息管理
 *
 *        Version:  1.0
 *        Created:  2011年06月11日 04时42分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef CPSS_COM_PID_IN
#define CPSS_COM_PID_IN
#include "cpss_common.h"
#include "cpss_public.h"
typedef enum _CPSS_STAT_TYPE_M{
	CPSS_STAT_ENABLE = 3,
	CPSS_STAT_DISABLE,
	CPSS_TYPE_SHOW,
	CPSS_TYPE_NOSHOW,
};
typedef struct _CPSS_PID_THREAD_INFO_T
{
	VOS_UINT32		nPID_ID;
	HANDLE			hPidDist;
	VOS_UINT32		dwPidThreadId;
	VOS_VOID		* pPidTbl;
}CPSS_PID_THREAD_INFO, *pCPSS_PID_THREAD_INFO;
typedef struct CPSS_PID_TABLE_T
{
	union {
		VOS_UINT32 		ulProcessPID;
		VOS_UINT8		ulSockInOut;
		VOS_UINT8		ulSockType;
		VOS_UINT16		ulSockPort;
	};
	VOS_UINT32	ulPidCount;				// PID Count
	VOS_UINT32  *pCPuID;				// CPUID
	VOS_UINT32  *pPid;					// PID
	VOS_CHAR	*pszPidName;			// [CPSSPIDMAXNAME];
	VOS_Event	pMsgEvent;				// 消息信号
	VOS_UINT32(*ppid_init_proc) (VOS_VOID *arg);
	VOS_UINT32(*ppid_timeout_proc) (VOS_VOID *pargc, VOS_UINT32 pargv);
	VOS_VOID * pSocketInfo;
	CPSS_PID_THREAD_INFO * pPidListInfo;
	VOS_CHAR szPidName[CPSSPIDMAXNAME];
	struct CPSS_PID_TABLE_T * prev;
	struct CPSS_PID_TABLE_T * next;
}CPSS_PID_TABLE,*PCPSS_PID_TABLE;

typedef struct _CPSS_CPUID_TABLE_T{
	VOS_UINT8	ulStat;			//
	VOS_UINT8	ulType;			//
	VOS_UINT8	ulReser;		//
	CPSS_CPUID_INFO stuCPuID_Info;
	struct _CPSS_CPUID_TABLE_T * next;
	struct _CPSS_CPUID_TABLE_T * prev;
}CPSS_CPUID_TABLE,*pCPSS_CPUID_TABLE;

typedef struct CPSS_MANAGE_PID_T
{
	pCPSS_CPUID_TABLE	pstuCPuIDList;
	PCPSS_PID_TABLE		pstuPidList;
}CPSS_MANAGE_PID,*PCPSS_MANAGE_PID;

typedef struct _CPSS_CPUID_PID_MAP_T
{
	VOS_UINT8	ulIndex;
	VOS_CHAR	szPidName[CPSSPIDMAXNAME];
}CPSS_CPUID_PID_MAP,*pCPSS_CPUID_PID_MAP;

CPSS_MANAGE_PID g_handleManagePid ;   //管理PID的全局变量
VOS_UINT32 g_ulSubSystem;
#define CPSSCPUID	cpss_get_cpuid_pid(CPSS_CONNECT_SUB_SELF,CPSS_CONNECT_SELF, CPSS_GET_TYPE_CPUID)
//g_handleManagePid.pstuCPuIDList[].ulProcessCPuID
#define DBSVRCPUID	cpss_get_cpuid_pid(CPSS_CONNECT_SUB_SELF, CPSS_CONNECT_DBSVR, CPSS_GET_TYPE_CPUID)
#define DBSVRPID	cpss_get_cpuid_pid(CPSS_CONNECT_SUB_SELF, CPSS_CONNECT_DBSVR, CPSS_GET_TYPE_PID)

#define CPSSFWCPUID		cpss_get_cpuid_pid(g_ulSubSystem, CPSS_CONNECT_FW, CPSS_GET_TYPE_CPUID)
#define CPSSFWPID		cpss_get_cpuid_pid(g_ulSubSystem, CPSS_CONNECT_FW, CPSS_GET_TYPE_PID)

#define SKTYPE(hSocketLink)		(((hSocketLink->nlSocketType) & VOS_SOCKET_TYPE)>>16)

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_pid_init
 *  Description:  平台pid初始化
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_pid_init ();

/* ===  FUNCTION  ============================================================
 *         Name:  cpss_get_info_for_pid(VOS_UINT32 ulProcessPid)
 *  Description:  通过PID得到PID对应的名称
 * =========================================================================*/
VOS_VOID * cpss_get_info_for_pid(VOS_UINT32 ulProcessPid, VOS_UINT32 ulType);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_set_cpuid_pid
 *  Description:  设置cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_set_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 IsShowflg, VOS_UINT32 ulValue, VOS_UINT32 ulType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_update_cpuid_pid
 *  Description:  更新cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_update_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 IsShowflg, VOS_UINT32 ulValue, VOS_UINT32 ulType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid_to_buffer
 *  Description:  得到cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_get_cpuid_pid_to_buffer (VOS_UINT32 ulType,VOS_UINT32 *ulCurIndex,VOS_STRING strBuffer,VOS_UINT32 * puCount);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_cpuid
 *  Description:  得到一个空闲的cpuid
 * ==========================================================================*/
VOS_UINT32 cpss_get_id_for_pid_str(VOS_STRING strPid);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid
 *  Description:  得到平台的cpuid和pid
 * ==========================================================================*/
VOS_UINT32 cpss_get_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 ulType);

/* ===  FUNCTION  ============================================================
 *         Name:  cpss_get_pid_name(VOS_UINT32 ulProcessPid)
 *  Description:  通过PID得到PID对应的名称
 * =========================================================================*/
VOS_STRING cpss_get_pid_name(VOS_UINT32 ulProcessPid);
/* ===  FUNCTION  =================================================================
 *         Name:  VOS_RegistPidInit
 *  Description:  pid加载初始化
 * ===============================================================================*/
VOS_UINT32 VOS_RegistPidInit (
	   VOS_UINT32 ulSubSystem,
	   VOS_UINT32 ulProcessPid,
	   const VOS_CHAR * ulProcessPidName,
	   VOS_UINT32 ulProcessCount,
	   VOS_UINT32 (* ppid_init_proc) (VOS_VOID *parg),
	   VOS_UINT32 (* ppid_timeout_proc) (VOS_VOID *pargc, VOS_UINT32 pargv));	/*end of function VOS_RegistPidInit -*/

#endif
