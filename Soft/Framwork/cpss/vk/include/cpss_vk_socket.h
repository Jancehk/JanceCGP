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
typedef struct CPSS_CLT_CMD_T
{
	VOS_UINT32  nCmdID;
	VOS_CHAR	strCmdBuff[CPSS_MSG_BUFFER_SIZE]; 
	VOS_UINT32	nCmdLength;
	struct CPSS_CLT_CMD_T * prev;
	struct CPSS_CLT_CMD_T * next;
}CPSS_CLT_CMD,CPSS_CLT_CMD;
typedef struct CPSS_SKT_FD_SET_T
{
	fd_set		fdread;
	fd_set		fdwrite;
	fd_set		fderror;
	VOS_INT32   nCurrent;
	VOS_INT32	nMaxfd;
}CPSS_SKT_FD_SET,*pCPSS_SKT_FD_SET;

typedef struct CPSS_SOCKET_LINK_T
{
	VOS_UINT8		 uID;					// ID
	VOS_UINT8		 uIStat;				// Socket Stat
	VOS_UINT8		 nlSocketType;			// TCP UDP socket type
	VOS_UINT8		 rfu1;					// RFU
	VOS_UINT16		 uPort;					// Port
	VOS_UINT16		 rfu2;					// RFU
	VOS_INT32		 nlSocketfd;			// socket fd
	CPSS_PID_TABLE   *pstuPid;				// Pid Handle
	VOS_VOID		 *pstuClientInfo;		// Client Handle
	//VOS_VOID		 *pstuRecvMsg;			// Recv Msg Handle
	//VOS_VOID		 *pstuSendMsg;			// Send Msg Handle
	struct CPSS_SOCKET_LINK_T * next;
	struct CPSS_SOCKET_LINK_T * prev;
}CPSS_SOCKET_LINK, *pCPSS_SOCKET_LINK;

typedef struct CPSS_CLIENT_INFO_T
{
	VOS_UINT32  ulID;									// 客户 的序号
	VOS_UINT8   nStat;									// socket的状态 空闲 预约 使用 使用完毕 
	VOS_UINT8   nType;									// msg   的状态
	VOS_UINT8   nLineStat;								// 登录状态 0 没有登录用户 1 设置用户名 2 已经登录
	VOS_UINT8   uClientStat;							// current Client stat
	VOS_UINT32  dwTThreadId;
	SOCKADDR_IN clientaddr;								//Client Addr
	CPSS_USER_INFO stuUserInfo;
	VOS_CHAR    strIPaddress[CPSS_CLIENT_IPADS_LENGTH];
	CPSS_SOCKET_LINK *	pAccptSocket;					// Accept Parent Socket Handle
	//CPSS_SOCKET_LINK *	pClientSocket;					// Client Socket Handle
	VOS_UINT32			ulCientHaveTime;
	VOS_Event			hCmdEvent;						//开始等待数据
	VOS_UINT8			bIsEvent;						//是否处理完数据 0 没有接受完 1 接受完数据
	VOS_VOID		 *	msg;
	CPSS_CLT_CMD	    stuCmdLink;
	struct CPSS_CLIENT_INFO_T *prev;
	struct CPSS_CLIENT_INFO_T *next;
	VOS_UINT32			nCmdConut;
	VOS_CHAR			*pstuBuffer;
	VOS_UINT32			nBufferLeng;
}CPSS_CLIENT_INFO, *pCPSS_CLIENT_INFO;

typedef struct CPSS_IOCP_MANAGE_T
{
	HANDLE	    ulTcpAccept;	//Tcp Handle
	HANDLE		hTcpRecv;
	HANDLE		hUdpRecv;
	HANDLE		hTcpSend;
	HANDLE		hUdpSend;
	VOS_UINT32	dwTThreadId;
	VOS_UINT32	dwUThreadId;
	VOS_UINT8	usExitSystem;
	VOS_UINT32  ulClientIndex;
	VOS_UINT32  ulClientCount;
	VOS_UINT32  ulClientOnline;
	VOS_Event	pTSendMsgEvent;
	VOS_Event	pUSendMsgEvent;
	VOS_Event	pTRecvMsgEvent;
	VOS_Event	pURecvMsgEvent;
	VOS_MUTEX			hSocketMutex;
	CPSS_SOCKET_LINK *	pUsedSocketHead;		// Socket Used List Head
	CPSS_SOCKET_LINK *	pUsedSocketTail;		// Socket Used List Tail
	CPSS_SOCKET_LINK *	pFreeSocketHead;		// Socket Used List Head
	CPSS_SOCKET_LINK *	pFreeSocketTail;		// Socket Used List Tail
	//CPSS_MANAGE_PID	 * hManagePid;		// PID List head
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
VOS_UINT32 cpss_subsystem_init (VOS_UINT8 uType, VOS_UINT8 uCmd);
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
 *         Name:  cpss_tcp_send_msg
 *  Description: 
 *  Input      : 
 *  OutPut     : 
 *  Return     : 
 * ==========================================================================*/
//VOS_UINT32 cpss_tcp_send_msg (VOS_VOID *pVoidMsg);
VOS_UINT32 cpss_send_data (VOS_VOID *pVoidMsg, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_copy_msg
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 cpss_copy_msg(VOS_VOID *pVoidMsgRecv, VOS_VOID * pVoidMsgSend);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_send_data
 * ==========================================================================*/
VOS_UINT32 cpss_send_data_msgid (VOS_UINT32 ulMsgID, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);

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
