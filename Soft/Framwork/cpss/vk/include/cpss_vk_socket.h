/*===============================================================================
 *
 *       Filename:  cpss_vk_socket.h
 *
 *    Description:  socket 头文件
 *
 *        Version:  1.0
 *        Created:  2011年05月23日 22时21分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 *=============================================================================*/
#ifndef CPSS_VK_SOCKET_IN
#define CPSS_VK_SOCKET_IN
#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_public.h"
#include "cpss_vk_thread.h"
#include "cpss_vos_sem.h"
#include "cpss_com_pid.h"
#include "cpss_msg.h"
//#include "cpss_msg.h"
#if (OS_TYPE == OS_TYPE_WINDOW)
//#include <WINSOCK.H>
//#include <time.h>
#pragma comment (lib,"WS2_32.lib")
//#pragma comment(lib,"wsock32.lib")
#elif (OS_TYPE == OS_TYPE_LINUX)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#if (OS_TYPE == OS_TYPE_WINDOW)	
typedef struct sockaddr_in SOCKADDR_IN;
#elif (OS_TYPE == OS_TYPE_LINUX)
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr    SOCKADDR;
#endif


/* vos client status*/
typedef enum VOS_CLIENT_STAT_M{
	CPSS_CLIENT_OFFLINE=3,
	CPSS_CLIENT_USERNAME,
	CPSS_CLIENT_ONLINE,
};


#define CPSS_SOCKET_PACKAGE_ON  0x01
#define CPSS_SOCKET_PACKAGE_OFF 0x00

#define CPSS_SOCKET_XCAP	(0x10 | CPSS_SOCKET_PACKAGE_OFF)
#define CPSS_SOCKET_TELNET	(0x20 | CPSS_SOCKET_PACKAGE_OFF)
//#define CPSS_MSG_PENDING		2
//#define CPSS_MSG_CHECKIN		3
//#define CPSS_MSG_CHECKED		4


typedef enum CPSS_CLIENT_CMD_M{
	CPSS_CLIENT_WAIT_CMD = 1,	//1 需要
	CPSS_CLIENT_NOWAIT_CMD,		//2 不需要 
	CPSS_CLIENT_CMD_DO,			//3 开始处理命令
	CPSS_CLIENT_CMD_DOING,		//4 正在处理命令
	CPSS_CLIENT_CMD_DOED,		//5 命令处理完成
};
typedef enum CPSS_SKT_STAT_M{
	CPSS_SKT_STAT_CLOSE = 1,	//1 关闭
	CPSS_SKT_STAT_CREATE,		//2 创建
	CPSS_SKT_STAT_OPEN,			//3 打开
	CPSS_SKT_STAT_OPENING,		//4 打开中
	CPSS_SKT_STAT_OPENED,		//5 打开完了
};
typedef enum _CPSS_IOCP_THREAD_LIST_M
{
	CPSS_IOCP_THREAD_ACCEPT,
	CPSS_IOCP_THREAD_TSEND,
	CPSS_IOCP_THREAD_TRECV,
	CPSS_IOCP_THREAD_USEND,
	CPSS_IOCP_THREAD_URECV,
	CPSS_IOCP_THREAD_COUNT,
};
typedef struct CPSS_SKT_FD_SET_T
{
	fd_set		fdread;
	fd_set		fdwrite;
	fd_set		fderror;
	//VOS_INT32   nCurrent;
	VOS_INT32	nMaxfd;
}CPSS_SKT_FD_SET,*pCPSS_SKT_FD_SET;

typedef struct CPSS_SOCKET_LINK_T
{
	VOS_UINT8			uID;				// ID
	VOS_UINT8			uIStat;				// Socket Stat
	VOS_UINT8			nlSocketType;		// TCP UDP socket type
	VOS_UINT8			rfu1;				// RFU
	VOS_UINT16			uPort;				// Port
	VOS_UINT16			rfu2;				// RFU
	VOS_INT32			nlSocketfd;			// socket fd
	CPSS_PID_TABLE *	pstuPid;			// Pid Handle
	CPSS_CPUID			stuSrcCPuID;		// CPUID/PID
	VOS_VOID *			pstuClientInfo;		// Client Handle
	struct CPSS_SOCKET_LINK_T * next;
	struct CPSS_SOCKET_LINK_T * prev;
}CPSS_SOCKET_LINK, *pCPSS_SOCKET_LINK;

typedef struct _CPSS_IOCP_THREAD_INFO_T
{
	VOS_THREAD_INFO hThread;
	VOS_Event	pMsgEvent;
	ThreadFun    FunProc;
	VOS_CHAR	strThreadName[32];
}CPSS_IOCP_THREAD_INFO, *pCPSS_IOCP_THREAD_INFO;
typedef struct CPSS_IOCP_MANAGE_T
{
	CPSS_IOCP_THREAD_INFO hIOThread[CPSS_IOCP_THREAD_COUNT];
	CPSS_PID_THREAD_INFO pPidListInfo;
	VOS_UINT8	usExitSystem;
	VOS_UINT32  ulClientIndex;
	VOS_UINT32  ulClientCount;
	VOS_UINT32  ulClientOnline;
	VOS_MUTEX			hSocketMutex;
	CPSS_SOCKET_LINK *	pUsedSocketHead;		// Socket Used List Head
	CPSS_SOCKET_LINK *	pUsedSocketTail;		// Socket Used List Tail
	CPSS_SOCKET_LINK *	pFreeSocketHead;		// Socket Used List Head
	CPSS_SOCKET_LINK *	pFreeSocketTail;		// Socket Used List Tail
	CPSS_MSG_SEM_MANAGE stMsgTL;
	VOS_MUTEX			hClientMutex;
	pCPSS_CLIENT_INFO	pUsedTCPClientHead;  //
	pCPSS_CLIENT_INFO	pUsedTCPClientTial;
	pCPSS_CLIENT_INFO	pFreeTCPClientHead;
	pCPSS_CLIENT_INFO	pFreeTCPClientTial;
}CPSS_IOCP_MANAGE,*PCPSS_IOCP_MANAGE;

CPSS_IOCP_MANAGE g_handleiocpmanage;

#define __AATEST_
#define  VOS_PrintMsg(pstrinfo,pMsgInfo) 
#ifndef __AATEST_
#define __AATEST_
#define  VOS_PrintMsg(pstrinfo,pMsgInfo)  {							\
	if(NULL != pMsgInfo)												\
	{																	\
		VOS_PrintInfo(__FILE__,__LINE__,"==%s====\nMsg:%p ID:%d Stat:%d",	\
			(NULL != pstrinfo)?pstrinfo:"",								\
			pMsgInfo,													\
			pMsgInfo->Body.msghead.ulMsgID,								\
			pMsgInfo->nStat);											\
	}																	\
}
#endif
/* ===  FUNCTION  ==============================================================
*         Name:  cpss_socket_init
*  Description:  平台套接口初始化
*  Input      :    
*  OutPut     :    
*  Return     :  
* ==========================================================================*/
VOS_UINT32 cpss_socket_init ();
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_iocp_init
 *  Description: 平台套接字初始化
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_iocp_init ();
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_subsystem_init
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_subsystem_init (VOS_UINT32 uType, VOS_UINT32 uCmd);
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_iocp_init
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_VOID cpss_iocp_close ();
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_msgtab
 *  Description: 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
CPSS_MSG_SEM_MANAGE * cpss_get_msgtab ();

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
VOS_UINT32 cpss_send_data (	
	VOS_VOID *	pVoidMsg, 
	VOS_VOID *	strBuffer, 
	VOS_UINT32	uBufLen, 
	VOS_UINT32	uType);


/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 * ==========================================================================*/
VOS_UINT32 cpss_send_data_msgid (
	VOS_UINT32 ulMsgID, 
	VOS_VOID * strBuffer, 
	VOS_UINT32 uBufLen, 
	VOS_UINT32 uType);

/* ===  FUNCTION  =================================================================
 *         Name:  cpss_get_msg_tab_info
 *  Description:  打印消息队列
 * ===============================================================================*/
VOS_VOID cpss_get_msg_tab_info(VOS_VOID * pClient, VOS_UINT8 bStat);

/* ===  FUNCTION  =================================================================
 *         Name:  cpss_get_msg_tab_info
 *  Description:  打印客户端队列
 * ===============================================================================*/
VOS_VOID cpss_get_cli_tab_info(VOS_VOID * pClient, VOS_UINT8 bStat);
#ifdef _cplusplus
}

#endif
#endif
