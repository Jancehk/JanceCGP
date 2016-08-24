/*=================================================================================
 *
 *       Filename:  cpss_com_pid.c
 *
 *    Description:  PID信息管理平台
 *
 *        Version:  1.0
 *        Created:  2011年06月11日 04时42分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * ==============================================================================*/
#include 	"cpss_com_pid.h"
#include 	"cpss_config.h"
#include 	"cpss_public.h"
#include 	"cpss_vk_print.h"
#include	"cpss_vk_file.h"
#include	"cpss_vos_sem.h"
#include	"cpss_vk_socket.h"

#define VOS_Pid_Malloc(ulSize)			VOS_Malloc((ulSize), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Realloc(pstrads,ulSize)	VOS_Realloc((pstrads), (ulSize), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Remset(pstrads)			VOS_Remset((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Memcls(pstrads)			VOS_Memcls((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Memcat(pstrA,pstrB)		VOS_Memcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Memsize(pstrads)		VOS_Memsize((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_PID))
#define VOS_Pid_Free(pstrads)			VOS_Free((pstrads), (CPSS_MEM_HEAD_KEY_CPSS_PID))

#define VOS_Pid_Strcat(pstrA,pstrB)		VOS_CpsStrcat((pstrA), (pstrB), (CPSS_MEM_HEAD_KEY_CPSS_PID))

static CPSS_CPUID_PID_MAP g_CPuID_CPuID_Manage[] = {
	{CPSS_SYSTEM_TYPE_SELF,		CPSS_STRING_CGP},
	{CPSS_SYSTEM_TYPE_DBSVR,	CPSS_STRING_DBSVR},
	{CPSS_SYSTEM_TYPE_XCAP,		CPSS_STRING_XCAP},
	{CPSS_SYSTEM_TYPE_MONEY,	CPSS_STRING_MONEY},
};
	
static CPSS_CPUID_PID_MAP g_CPuID_PID_Manage[] = {
	{CPSS_SUBSYS_TYPE_SELF,		CPSS_STRING_CGP},
	{CPSS_SUBSYS_TYPE_FW,		CPSS_STRING_FW},
	{CPSS_SUBSYS_TYPE_TLN,		CPSS_STRING_TELNET},
	{CPSS_SUBSYS_TYPE_FTP,		CPSS_STRING_FTP},
	{CPSS_SUBSYS_TYPE_DBSVR,	CPSS_STRING_DBSVR},
	{CPSS_SUBSYS_TYPE_XCAP,		CPSS_STRING_XCAP},
	{CPSS_SUBSYS_TYPE_MOCLI,	CPSS_STRING_MOCLI},
	{CPSS_SUBSYS_TYPE_MONEY,	CPSS_STRING_MONEY},
};

static VOS_UINT32 g_ulSystemID;
static CPSS_MANAGE_PID g_handleManagePid;   //管理PID的全局变量
/* ===  FUNCTION  ======================================================================
 *         Name:  cpss_check_pid
 *  Description: 检查PID的合法性 
 * =====================================================================================
 */
static VOS_UINT32 cpss_check_pid (VOS_UINT32 ulProcessPid)
{
	VOS_UINT32 ulRet = VOS_ERR;
	PCPSS_PID_TABLE cpsspidtmp = g_handleManagePid.pstuPidList;
	VOS_UINT32 uType = 0,uPort = 0;

	uType = ulProcessPid & VOS_SOCKET_TYPE;
	uPort = ulProcessPid & VOS_SOCKET_PORT;
	uType >>= 16;
	if (VOS_SOCKET_UDP != uType && VOS_SOCKET_TCP != uType )
	{
		VOS_PrintErr(__FILE__,__LINE__,"PID is invalid ,TYPE:%d PID:%d",
			uType,
			uPort);
		return ulRet;
	}
	
	if (0xffff < uPort)
	{
		VOS_PrintErr(__FILE__,__LINE__,"PID is invalid ,SOCK:%d PORT:%d",
			uType,
			uPort);
		return ulRet;
	}
	while(NULL != cpsspidtmp)
	{
		if (cpsspidtmp->ulProcessPID != ulProcessPid)
		{
			cpsspidtmp = cpsspidtmp->next;
			continue;
		}
		if (0 == (cpsspidtmp->ulProcessPID & VOS_SOCKET_PORT))
		{
			cpsspidtmp = cpsspidtmp->next;
			continue;
		}
		VOS_PrintErr(__FILE__,__LINE__,"PID is exist %d",ulProcessPid);
		return ulRet;
	}	
	return VOS_OK;
}		/* -----  end of function cpss_check_pid  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_cpuid
 *  Description:  得到一个空闲的cpuid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static pCPSS_CPUID_TABLE cpss_get_free_cpuid()
{
	pCPSS_CPUID_TABLE pstCpuID = NULL;
	pstCpuID = (pCPSS_CPUID_TABLE)VOS_Pid_Malloc(sizeof(CPSS_CPUID_TABLE));
	if (pstCpuID == NULL)
	{
		VOS_PrintErr(__FILE__,__LINE__,"malloc a new cpuid address is error");
		return NULL;
	}
	BZERO(pstCpuID,sizeof(CPSS_CPUID_TABLE));
	return pstCpuID;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_add_cpuid
 *  Description:  将一个cpuid添加到列表中
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_add_cpuid(pCPSS_CPUID_TABLE pstCpuID)
{
	pCPSS_CPUID_TABLE pstCpuIDTmp = NULL;
	pCPSS_CPUID_TABLE pstCpuIDPre = NULL;
	pstCpuIDTmp = g_handleManagePid.pstuCPuIDList;
	while (pstCpuIDTmp != NULL)
	{
		pstCpuIDPre = pstCpuIDTmp;
		pstCpuIDTmp = pstCpuIDTmp->next;
	}
	if (NULL == pstCpuIDPre)
	{
		g_handleManagePid.pstuCPuIDList = pstCpuID;
		pstCpuID->next = NULL;
		pstCpuID->prev = NULL;
	}
	else
	{
		pstCpuIDPre->next = pstCpuID;
		pstCpuID->prev = pstCpuIDPre;
		pstCpuID->next = NULL;

	}
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_del_cpuid
 *  Description:  从链表中删除改
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_UINT32 cpss_del_cpuid(pCPSS_CPUID_TABLE pstCpuID)
{
	pCPSS_CPUID_TABLE pstCpuIDTmp = NULL;
	pstCpuIDTmp = g_handleManagePid.pstuCPuIDList;
	while (pstCpuIDTmp != NULL)
	{
		if (pstCpuID->stuCPuID_Info.ulSubsysID == pstCpuIDTmp->stuCPuID_Info.ulSubsysID)
		{
			break;
		}
		pstCpuIDTmp = pstCpuIDTmp->next;
	}
	if (NULL == pstCpuIDTmp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "del cpuid %d", pstCpuID->stuCPuID_Info.ulSubsysID);
		VOS_Free(pstCpuID,sizeof(CPSS_CPUID_TABLE));
	}
	else
	{
		if (pstCpuIDTmp->prev == NULL &&
			pstCpuIDTmp->next == NULL)
		{
			g_handleManagePid.pstuCPuIDList;
		}else//first
		if (pstCpuIDTmp->prev == NULL &&
			pstCpuIDTmp->next != NULL)
		{
			g_handleManagePid.pstuCPuIDList = pstCpuIDTmp->next;
			g_handleManagePid.pstuCPuIDList->prev = NULL;
		}else// last
		if (pstCpuIDTmp->prev != NULL &&
			pstCpuIDTmp->next == NULL)
		{
			pstCpuIDTmp->prev->next = NULL;
		}
		else
		{
			pstCpuIDTmp->prev->next = pstCpuIDTmp->next;
			pstCpuIDTmp->next->prev = pstCpuIDTmp->prev;
		}
		pstCpuIDTmp->next = NULL;
		pstCpuIDTmp->prev = NULL;
	
		VOS_Free(pstCpuIDTmp,sizeof(CPSS_CPUID_TABLE));
	}
	return VOS_OK;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_free_cpuid
 *  Description:  得到一个空闲的cpuid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_get_id_for_pid_str(VOS_STRING strPid)
{
	VOS_UINT32 uIndex = 0;
	VOS_UINT32 uCount = sizeof(g_CPuID_PID_Manage)/sizeof(CPSS_CPUID_PID_MAP);
	for (uIndex=0; uIndex < uCount; uIndex ++)
	{
		if (0 == VOS_Strcmp(g_CPuID_PID_Manage[uIndex].szPidName, strPid))
		{
			return g_CPuID_PID_Manage[uIndex].ulIndex;
		}
	}
	return 0;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_pid_init
 *  Description:  平台pid初始化
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_pid_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;//strcat(buffer,"\\");printf("Please Input Local Ip:");
	VOS_UINT32 ulProcessCPuID = 0;
	pCPSS_CPUID_TABLE pstCpuID = NULL;
	g_ulSystemID = 0;
	ulProcessCPuID = cpss_get_ip_address("\\local.cfg", "Please Input Local Ip:");
	if (0 == ulProcessCPuID)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Init local CPuID is Error");
		return ulRet;
	}
	ulRet = cpss_set_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_SELF, 
		CPSS_STAT_ENABLE, ulProcessCPuID, CPSS_SET_TYPE_CPUID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set local cpuid is error");
		return ulRet;
	}
/*	ulRet = cpss_set_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_SELF,
		CPSS_STAT_ENABLE,0, CPSS_GET_TYPE_PID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set DBSvr pid is error");
		return ulRet;
	}*/
	ulProcessCPuID = cpss_get_ip_address("\\DBSvr.cfg", "Please Input DBSvr Ip:");
	if (0 == ulProcessCPuID)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Init DBSvr CPuID is Error");
		return ulRet;
	}
	ulRet = cpss_set_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_DBSVR, 
		CPSS_STAT_ENABLE, ulProcessCPuID, CPSS_SET_TYPE_CPUID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set DBSvr cpuid is error");
		return ulRet;
	}
	
	ulRet = cpss_set_cpuid_pid(CPSS_SYSTEM_TYPE_SELF, CPSS_SUBSYS_TYPE_DBSVR, 
		CPSS_STAT_ENABLE, VOS_SOCKET_IN | VOS_UDP_PID |5001, CPSS_SET_TYPE_PID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set DBSvr pid is error");
		return ulRet;
	}
	return ulRet;
}		/* -----  end of function cpss_pid_init  ----- */

/* ===  FUNCTION  ==============================================================
*         Name:  cpss_pid_check
*  Description:  检查pid初始化有没有问题
* ==========================================================================*/
VOS_UINT32 cpss_pid_check()
{
	if (NULL == g_handleManagePid.pstuCPuIDList || NULL == g_handleManagePid.pstuPidList)
	{
		return VOS_ERR;
	}
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_systemid
*  Description:  取得当前系统的ID
* ==========================================================================*/
VOS_UINT32 cpss_get_systemid()
{
	return g_ulSystemID;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_cpuid_header
*  Description:  取得cpuid的头地址信息
* ==========================================================================*/
pCPSS_CPUID_TABLE cpss_get_cpuid_header()
{
	return g_handleManagePid.pstuCPuIDList;
}
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_get_cpuid_header
*  Description:  取得cpuid的头地址信息
* ==========================================================================*/
PCPSS_PID_TABLE cpss_get_pid_header()
{
	return g_handleManagePid.pstuPidList;
}
/* ===  FUNCTION  =================================================================
*         Name:  VOS_RegistPidInit
*  Description:  pid加载初始化
* ===============================================================================*/
VOS_UINT32 VOS_RegistPidInit(VOS_UINT32 ulSubSystem,
	VOS_UINT32 ulProcessPid,
	const VOS_CHAR * ulProcessPidName,
	VOS_UINT32 ulProcessCount,
	VOS_UINT32(*ppid_init_proc) (VOS_VOID *parg),
	VOS_UINT32(*ppid_timeout_proc) (VOS_VOID *pargc, VOS_UINT32 pargv))
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 ulPidIndex = 0;
	PCPSS_PID_TABLE pstcpsspid = NULL, pstcpsspidtmp = g_handleManagePid.pstuPidList;
	CPSS_PID_THREAD_INFO * pPidThreadInfo = NULL;
	pCPSS_CPUID_TABLE	pstuCPuIDInfo;

	if (NULL == ppid_init_proc)
	{
		VOS_PrintErr(__FILE__, __LINE__, "pid proc is NULL");
		return ulRet;
	}

	if (g_ulSystemID == 0 &&
		ulSubSystem == 0)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set subsystem is error");
		return ulRet;
	}
	if (VOS_OK != cpss_check_pid(ulProcessPid) ||
		0 == CPSSCPUID)
	{
		VOS_PrintErr(__FILE__, __LINE__, "Pid Error! C:%u P:%u N:%s",
			CPSSCPUID,
			ulProcessPid, ulProcessPidName);
		return ulRet;
	}
	ulPidIndex = cpss_get_id_for_pid_str((VOS_CHAR*)ulProcessPidName);
	if (ulPidIndex <= CPSS_SUBSYS_TYPE_SELF)
	{
		VOS_PrintErr(__FILE__, __LINE__, "get pid index is error");
		return ulRet;
	}

	pstcpsspid = (PCPSS_PID_TABLE)VOS_Pid_Malloc(sizeof(CPSS_PID_TABLE));
	BZERO(pstcpsspid, sizeof(CPSS_PID_TABLE));
	if (NULL == pstcpsspid)
	{
		VOS_PrintErr(__FILE__, __LINE__, " mem error!");
		return ulRet;
	}
	pPidThreadInfo = (CPSS_PID_THREAD_INFO*)VOS_Pid_Malloc(sizeof(CPSS_PID_THREAD_INFO)*ulProcessCount);
	BZERO(pPidThreadInfo, sizeof(CPSS_PID_THREAD_INFO)*ulProcessCount);
	if (NULL == pPidThreadInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, " mem error!");
		VOS_Pid_Free(pstcpsspid);
		return ulRet;
	}
	pstcpsspid->ulProcessPID = ulProcessPid;
	pstcpsspid->ulPidCount = ulProcessCount;
	pstcpsspid->pPidListInfo = pPidThreadInfo;
	VOS_Strcpy(pstcpsspid->szPidName, ulProcessPidName);
	pstcpsspid->ppid_init_proc = ppid_init_proc;
	pstcpsspid->ppid_timeout_proc = ppid_timeout_proc;
	pstcpsspid->next = NULL;
	pstcpsspid->prev = NULL;
	if (NULL == pstcpsspidtmp)
	{
		g_handleManagePid.pstuPidList = pstcpsspid;

	}
	else
	{
		while (NULL != pstcpsspidtmp->next)
		{
			pstcpsspidtmp = pstcpsspidtmp->next;
		}
		pstcpsspidtmp->next = pstcpsspid;
		pstcpsspid->prev = pstcpsspidtmp;
	}

	if (g_ulSystemID == 0 &&
		ulSubSystem != 0)
	{
		g_ulSystemID = ulSubSystem;
	}

	ulRet = cpss_set_cpuid_pid(g_ulSystemID, ulPidIndex,
		CPSS_STAT_ENABLE, CPSSCPUID, CPSS_SET_TYPE_CPUID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set %s cpuid is error",
			pstcpsspid->szPidName);
		return ulRet;
	}
	ulRet = cpss_set_cpuid_pid(g_ulSystemID, ulPidIndex,
		CPSS_STAT_ENABLE, ulProcessPid, CPSS_SET_TYPE_PID);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set %s pid is error",
			pstcpsspid->szPidName);
		VOS_Pid_Free(pPidThreadInfo);
		VOS_Pid_Free(pstcpsspid);
		return ulRet;
	}
	pstuCPuIDInfo = g_handleManagePid.pstuCPuIDList;
	while (NULL != pstuCPuIDInfo)
	{
		if (pstuCPuIDInfo->stuCPuID_Info.ulSubsysID == ulPidIndex &&
			pstuCPuIDInfo->stuCPuID_Info.ulSystemID == g_ulSystemID)
		{
			break;
		}
		pstuCPuIDInfo = pstuCPuIDInfo->next;
	}
	if (NULL == pstuCPuIDInfo)
	{
		VOS_PrintErr(__FILE__, __LINE__, "not found %s cpuid for Regist Pid Init",
			pstcpsspid->szPidName);
		return ulRet;
	}
	//pstcpsspid->pPid = &pstuCPuIDInfo->stuCPuID_Info.ulPid;
	pstcpsspid->pCPuID = &pstuCPuIDInfo->stuCPuID_Info;
	return ulRet;
}		/* -----  end of function VOS_RegistPidProcess  ----- */

/* ===  FUNCTION  ============================================================
 *         Name:  cpss_get_pid_name(VOS_UINT32 ulProcessPid)
 *  Description:  通过PID得到PID对应的名称
 * =========================================================================*/
VOS_STRING cpss_get_pid_name(VOS_UINT32 ulProcessPid)
{
	PCPSS_PID_TABLE cpsspid = g_handleManagePid.pstuPidList;
	while(NULL != cpsspid)
	{
		if (cpsspid->ulProcessPID != ulProcessPid)
		{
			cpsspid = cpsspid->next;
		}
		else
		{
			return cpsspid->szPidName;
		}
	}
	return CPSS_STRING_CGP;
}		/* -----  end of function cpss_get_pid_name(VOS_UINT32 ulProcessPid)  ----- */

/* ===  FUNCTION  ============================================================
 *         Name:  cpss_get_info_for_pid(VOS_UINT32 ulProcessPid)
 *  Description:  通过PID得到PID对应的名称
 * =========================================================================*/
VOS_VOID * cpss_get_info_for_pid(VOS_UINT32 ulProcessPid, VOS_UINT32 ulType)
{
	VOS_VOID * pstuVoid = NULL;
	CPSS_SOCKET_LINK * hSocketLink = g_handleiocpmanage.pUsedSocketHead;
	while(NULL != hSocketLink)
	{
		if (NULL == hSocketLink->pstuPid)
		{
			hSocketLink = hSocketLink->next;
			continue;
		}
		if (hSocketLink->pstuPid->pCPuID->stuCPuid.ulPid == ulProcessPid)
		{
			break;
		}
		if (VOS_SOCKET_TCP == hSocketLink->nlSocketType &&
			hSocketLink->stuSrcCPuID.ulPid == ulProcessPid)
		{
			break;
		}
		hSocketLink = hSocketLink->next;
	}
	if (NULL != hSocketLink)
	{
		switch(ulType)
		{
		case CPSS_GET_SKT_LINK_SOCKET:
			pstuVoid = hSocketLink;
				break;
		default:
			pstuVoid = NULL;
			break;
		}
	}
	return pstuVoid;
}		/* -----  end of function cpss_get_pid_name(VOS_UINT32 ulProcessPid)  ----- */
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_set_cpuid_pid
 *  Description:  设置cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_set_cpuid_pid(VOS_UINT32 ulSystemID, VOS_UINT32 ulSubSysID, VOS_UINT32 IsShowflg, VOS_UINT32 ulValue, VOS_UINT32 ulType)
{
	VOS_UINT32			ulRet = VOS_ERR;
	VOS_UINT32			uIsGet = 1;
	pCPSS_CPUID_TABLE	pstuCPuIDTemp = NULL;//cpss_get_free_cpuid();
	
	pstuCPuIDTemp = g_handleManagePid.pstuCPuIDList;
	while(NULL != pstuCPuIDTemp)
	{
		if (pstuCPuIDTemp->stuCPuID_Info.ulSystemID == ulSystemID &&
			pstuCPuIDTemp->stuCPuID_Info.ulSubsysID == ulSubSysID)
		{
			uIsGet =1;
			break;
		}
		pstuCPuIDTemp = pstuCPuIDTemp->next;
	}

	if (NULL == pstuCPuIDTemp)
	{
		pstuCPuIDTemp = cpss_get_free_cpuid();
		uIsGet = 0;
	}
	if (NULL == pstuCPuIDTemp)
	{
		VOS_PrintErr(__FILE__, __LINE__, " get cpuid or pid type is error!");
		return ulRet;
	}
	pstuCPuIDTemp->ulStat = IsShowflg;
	pstuCPuIDTemp->stuCPuID_Info.ulSystemID = ulSystemID;
	pstuCPuIDTemp->stuCPuID_Info.ulSubsysID = ulSubSysID;

	if (CPSS_SET_TYPE_CPUID == ulType)
	{
		pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulCPuID = ulValue;
		ulRet = VOS_OK;
	}else 
	if (CPSS_SET_TYPE_PID == ulType)
	{
		pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulPid = ulValue;
		ulRet = VOS_OK;
	}
	else
	{
		ulRet = VOS_ERR;
		VOS_PrintErr(__FILE__, __LINE__, " get cpuid or pid type is error!");
	}
	if (0 == uIsGet)
	{
		cpss_add_cpuid(pstuCPuIDTemp);
	}
	return ulRet;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid
 *  Description:  得到cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_get_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 ulType)
{
	pCPSS_CPUID_TABLE pstuCPuIDTemp = NULL;
	pstuCPuIDTemp = g_handleManagePid.pstuCPuIDList;

	while(NULL != pstuCPuIDTemp)
	{
		if (pstuCPuIDTemp->stuCPuID_Info.ulSubsysID == 0 ||
			pstuCPuIDTemp->stuCPuID_Info.ulSystemID == 0)
		{
			pstuCPuIDTemp = pstuCPuIDTemp->next;
			continue;
		}

		if (pstuCPuIDTemp->stuCPuID_Info.ulSubsysID == ulNo &&
			pstuCPuIDTemp->stuCPuID_Info.ulSystemID == ulSubSys)
		{
			break;
		}
		pstuCPuIDTemp = pstuCPuIDTemp->next;
	}
	if (NULL == pstuCPuIDTemp)
	{
		return 0;
	}
	if (pstuCPuIDTemp->ulStat == CPSS_STAT_DISABLE)
	{
		return 0;
	}
	if (CPSS_GET_TYPE_CPUID == ulType)
	{
		return pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulCPuID;
	}else 
	if (CPSS_GET_TYPE_PID == ulType)
	{
		return pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulPid;
	}
	else
	{
		VOS_PrintErr(__FILE__, __LINE__, " get cpuid or pid type is error!");
	}
	return 0;
}	/* -----  end of function cpss_get_pid_name(VOS_UINT32 ulProcessPid)  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_update_cpuid_pid
 *  Description:  更新cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_update_cpuid_pid (VOS_UINT32 ulSubSys, VOS_UINT32 ulNo, VOS_UINT32 IsShowflg, VOS_UINT32 ulValue, VOS_UINT32 ulType)
{
	VOS_UINT32		ulRet = VOS_ERR;
	pCPSS_CPUID_TABLE pstuCPuIDTemp = NULL;//cpss_get_free_cpuid();
	pstuCPuIDTemp = g_handleManagePid.pstuCPuIDList;
	while(NULL != pstuCPuIDTemp)
	{
		if (pstuCPuIDTemp->stuCPuID_Info.ulSubsysID == ulNo &&
			pstuCPuIDTemp->stuCPuID_Info.ulSystemID == ulSubSys)
		{
			break;
		}
		pstuCPuIDTemp = pstuCPuIDTemp->next;
	}
	if (NULL == pstuCPuIDTemp)
	{
		VOS_PrintErr(__FILE__, __LINE__, " get cpuid or pid type is error!");
		return ulRet;
	}
	pstuCPuIDTemp->ulStat = IsShowflg;
	
	if (CPSS_GET_TYPE_CPUID == ulType)
	{
		pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulCPuID = ulValue;
		ulRet = VOS_OK;
	}else 
	if (CPSS_GET_TYPE_PID == ulType)
	{
		pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulPid = ulValue;
		ulRet = VOS_OK;
	}
	else
	{
		ulRet = VOS_ERR;
		VOS_PrintErr(__FILE__, __LINE__, " get cpuid or pid type is error!");
	}
	return ulRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_cpuid_pid
 *  Description:  打印出cpuid的信息
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
static VOS_VOID cpss_print_cpuid_pid (VOS_CHAR * strType, pCPSS_CPUID_INFO pstuCPuID)
{
	if (pstuCPuID == NULL)
	{
		return ;
	}
	VOS_PrintDebug(__FILE__, __LINE__, "%s [%s:%s] CPuID[%u] PID[%d]",
		strType,
		g_CPuID_CPuID_Manage[pstuCPuID->ulSystemID - CPSS_SYSTEM_TYPE_SELF].szPidName,
		g_CPuID_PID_Manage[pstuCPuID->ulSubsysID - CPSS_SUBSYS_TYPE_SELF].szPidName,
		pstuCPuID->stuCPuid.ulCPuID,
		pstuCPuID->stuCPuid.ulPid);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_cpuid_pid_to_buffer
 *  Description:  得到cpss系统的cpuid和pid
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_get_cpuid_pid_to_buffer (VOS_UINT32 ulType,VOS_STRING strBuffer)
{
	VOS_UINT32 ulRet = VOS_ERR;
	VOS_UINT32 ulOffset = 0;
	pCPSS_CPUID_HEADER pstrCpuidCount = NULL;
	VOS_CHAR *		   pstrCPuIDBuffer = NULL;
	pCPSS_CPUID_TABLE pstuCPuIDTemp = g_handleManagePid.pstuCPuIDList;
	CPSS_CPUID_INFO stuCPuID;
	if (NULL == pstuCPuIDTemp)
	{
		VOS_PrintErr(__FILE__, __LINE__, "link head is null");
		goto END_PROC;
	}
	if (NULL == strBuffer )
	{
		VOS_PrintErr(__FILE__, __LINE__, "set to buffer param is null:%p",
			strBuffer);
		goto END_PROC;
	}
	pstrCpuidCount = (pCPSS_CPUID_HEADER)strBuffer;
	pstrCPuIDBuffer = strBuffer + sizeof(CPSS_CPUID_HEADER);
	if (CPSS_SET_TO_BUFFER == ulType)
	{
		ulOffset = 0;
		while(NULL != pstuCPuIDTemp)
		{
			if (pstuCPuIDTemp->ulStat == CPSS_STAT_DISABLE ||
				pstuCPuIDTemp->stuCPuID_Info.ulSystemID == 0 ||
				pstuCPuIDTemp->stuCPuID_Info.ulSubsysID == 0 ||
				pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulCPuID == 0 ||
				pstuCPuIDTemp->stuCPuID_Info.stuCPuid.ulPid == 0)
			{
				pstuCPuIDTemp = pstuCPuIDTemp->next;
				continue;
			}
			if (CPSS_SYSTEM_TYPE_SELF == pstuCPuIDTemp->stuCPuID_Info.ulSystemID)
			{
				pstuCPuIDTemp = pstuCPuIDTemp->next;
				continue;
			}
			VOS_Memcpy(pstrCPuIDBuffer + ulOffset, &pstuCPuIDTemp->stuCPuID_Info, sizeof(CPSS_CPUID_INFO));
			ulOffset += sizeof(CPSS_CPUID_INFO);

			cpss_print_cpuid_pid("Get",&pstuCPuIDTemp->stuCPuID_Info);

			pstuCPuIDTemp = pstuCPuIDTemp->next;
		}
		pstrCpuidCount->ulCount = ulOffset / sizeof(CPSS_CPUID_INFO);
	}else
	if (CPSS_SET_TO_STUCPUID == ulType)
	{
		ulOffset = 0;
		while (ulOffset < pstrCpuidCount->ulCount*sizeof(CPSS_CPUID_INFO))
		{
			VOS_Memcpy(&stuCPuID, pstrCPuIDBuffer + ulOffset, sizeof(CPSS_CPUID_INFO));
			if (stuCPuID.ulSystemID == g_ulSystemID)
			{
				ulOffset += sizeof(CPSS_CPUID_INFO);
				continue;
			}
			//*ulCurIndex = stuCPuID.ulSystemID;
			ulRet = cpss_set_cpuid_pid(
				stuCPuID.ulSystemID,
				stuCPuID.ulSubsysID,
				CPSS_STAT_ENABLE,
				stuCPuID.stuCPuid.ulCPuID,
				CPSS_SET_TYPE_CPUID);
			if (VOS_OK != ulRet)
			{
				cpss_print_cpuid_pid("Set Cpu",&stuCPuID);
				goto END_PROC;
			}
			
			ulRet = cpss_set_cpuid_pid(
				stuCPuID.ulSystemID,
				stuCPuID.ulSubsysID,
				CPSS_STAT_ENABLE,
				stuCPuID.stuCPuid.ulPid,
				CPSS_SET_TYPE_PID);
			if (VOS_OK != ulRet)
			{
				cpss_print_cpuid_pid("Set Pid",&stuCPuID);
				goto END_PROC;
			}
			cpss_print_cpuid_pid("Set",&stuCPuID);
			
			ulOffset += sizeof(CPSS_CPUID_INFO);
		}
	}else
	{
		VOS_PrintErr(__FILE__, __LINE__, " set to buffer type is error!");
		goto END_PROC;
	}
	ulRet = VOS_OK;
END_PROC:
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "set to buffer is error");
	}
	return ulRet;
}