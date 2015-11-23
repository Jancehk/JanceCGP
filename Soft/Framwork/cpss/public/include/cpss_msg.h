/*===============================================================================
 *
 *       Filename:  cpss_msg.h
 *
 *    Description:  消息格式定义头文件
 *
 *        Version:  1.0
 *        Created:  2011年05月23日 21时37分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 *=============================================================================*/

#ifndef  CPSS_MSG_INC_H
#define  CPSS_MSG_INC_H
#include "cpss_public.h"
#if (OS_TYPE == OS_TYPE_WINDOW)
//#include <WINSOCK2.H>
//#include <WINSOCK.H>
#elif (OS_TYPE == OS_TYPE_LINUX)
#endif

#include "cpss_common.h"
//#include "cpss_tm_timer.h"

//客户信息
#define CPSS_CLIENT_IPADS_LENGTH	32
#define CPSS_SOCKET_ERROR (-1)

#define  CPSS_MSG_SEND_TCP		0x01
#define  CPSS_MSG_SEND_UDP		0x02
#define  CPSS_MSG_RECV_TCP		0x04
#define  CPSS_MSG_RECV_UDP		0x08

typedef enum _CPSS_MSG_STAT_M{
	CPSS_MSG_STAT_FREE=0x01,		/*空闲*/
	CPSS_MSG_STAT_RESERVE,			/*预约*/
	CPSS_MSG_STAT_USEING,			/*使用中*/
	CPSS_MSG_STAT_USED,				/*使用完*/
};

typedef enum _CPSS_MSG_TYPE_M{
	CPSS_MSG_TYPE_FREE=4,		/*空闲中*/
	CPSS_MSG_TYPE_PRERECV,		/*接受准备中*/
	CPSS_MSG_TYPE_RECVING,		/*接受中*/
	CPSS_MSG_TYPE_RECVED,			/*接受完了*/
	CPSS_MSG_TYPE_PRESEND,			/*发送准备中*/
	CPSS_MSG_TYPE_SENDING,			/*发送中*/
	CPSS_MSG_TYPE_SENDED,			/*发送完了*/
};
typedef enum CPSS_TYPE_T{
	CPSS_TYPE_SYSTEM_INIT=0x31,
	CPSS_TYPE_SYSTEM_TELNET,
	CPSS_TYPE_SYSTEM_HTTP,
	CPSS_TYPE_SYSTEM_FTP,
	CPSS_TYPE_SYSTEM_TCP,
	CPSS_TYPE_SYSTEM_UDP,
	CPSS_REQ_DBSVR_GET,				/*recv get cpuid or pid info*/
	CPSS_RES_DBSVR_GET,				/*send res cpuid or pid info*/
	CPSS_REQ_DBSVR_USE,				/*req use*/
	CPSS_RES_DBSVR_USE,				/*res use or pass*/
	CPSS_REQ_XCAP_GET,
	CPSS_RES_XCAP_GET,
	CPSS_TYPE_SYSTEM_UNIT,
};
typedef enum CPSS_TYPE_SUB_T{
	CPSS_TYPE_CPUID_PID=0x11,
	CPSS_TYPE_CHECK_USE,			/*check use or pass*/
	CPSS_TYPE_GET_SUBURL,
};
typedef enum CPSS_CMD_T{
	CPSS_CMD_SYSTEM_INIT=0x21,
	CPSS_CMD_SYSTEM_TELNET,
	CPSS_CMD_CPUID_PID,
	CPSS_CMD_SYSTEM_INIT3,
	CPSS_CMD_SYSTEM_INIT4,
	CPSS_CMD_SYSTEM_INIT5,
	CPSS_CMD_SYSTEM_INIT6,
	CPSS_CMD_SYSTEM_UNIT,
};
/* vos socket status*/
typedef enum VOS_SOCKET_STAT_M{
	VOS_SOCKET_STAT_FREEING=0x11,	/*空闲*/
	VOS_SOCKET_STAT_RESERVE,		/*预约*/
	VOS_SOCKET_STAT_USEING,			/*使用*/
	VOS_SOCKET_STAT_USED,			/*使用完毕*/
};

/* vos socket status*/
typedef enum VOS_SEND_SKT_TYPE_M{
	VOS_SEND_SKT_TYPE_TCP =1,
	VOS_SEND_SKT_TYPE_UDP,
	VOS_SEND_SKT_TYPE_INSERT =4,
	VOS_SEND_SKT_TYPE_FINISH=8,
};

typedef struct _CPSS_COM_PID_T
{
	VOS_UINT32 	ulCpuID;
	VOS_UINT32 	ulPID;
}CPSS_COM_PID,*pCPSS_COM_PID;
typedef struct _CPSS_COM_HEAD_T
{
	VOS_CHAR			strSegName[4];
	CPSS_COM_PID 		stDstProc;		//目标地址
	CPSS_COM_PID 		stSrcProc;		//源地址
	VOS_UINT8			uResevr[4];
	/*-----------------------------------------*/
	VOS_UINT32 			ulMsgID;		//消息ID;
	VOS_UINT32 			ulParentMsgID;	//父消息消息ID;
	VOS_UINT32 			ulRecvMsgID;	//源消息ID
	VOS_UINT32 			ulNextMsgID;	//下一个源消息ID
	/*-----------------------------------------*/
	VOS_UINT32			uType;
	//VOS_UINT8			uSubType;
	VOS_UINT32			uCmd;
	//VOS_UINT8			uSubCmd;
	VOS_UINT32 			ulMsgLength;	//消息长度
	VOS_UINT8			RFU[4];
	/*-----------------------------------------*/
}CPSS_COM_HEAD,*pCPSS_COM_HEAD;

#define  CPSS_MSG_HEAD_SIZE		sizeof(CPSS_COM_HEAD) //+(24)
typedef struct _CPSS_COM_DATA_T
{
	CPSS_COM_HEAD		msghead;
	VOS_CHAR*			stuDataBuf;
	//CPSS_MEM_BUFFER     stuSendBuf;
}CPSS_COM_DATA,*pCPSS_COM_DATA;
typedef struct _CPSS_MSG_T
{
	VOS_VOID 		   * pTimer;		/*定时器句柄*/
	VOS_UINT8			 nType;			/*5:接受中 6:接受完了 7:发送中 8:发送完了*/
	VOS_UINT8			 nStat;			/*1:空闲   2:预约     3:使用中 4:使用完*/
	VOS_VOID		   * pClient;
	struct _CPSS_MSG_T * next;
	struct _CPSS_MSG_T * prev;
	VOS_UINT32 			 ulMsgLength;    //消息长度
	VOS_VOID		   * pRecvBuffer[4];
	CPSS_COM_DATA        Body;
}CPSS_MSG,*pCPSS_MSG;
typedef struct _CPSS_MSG_TAB_T
{
	CPSS_MSG *pUseHead;
	CPSS_MSG *pUseTial;
}CPSS_MSG_TAB,*pCPSS_MSG_TAB;
typedef struct _CPSS_MSG_TYPE_T
{
	VOS_UINT32    TcpCount;
	VOS_UINT32    UdpCount;
	CPSS_MSG_TAB  TypeTCP;
	CPSS_MSG_TAB  TypeUDP;
}CPSS_MSG_TYPE,*pCPSS_MSG_TYPE;
typedef struct _CPSS_MSG_STYLE_T
{
	VOS_UINT32    nIDCount;
	VOS_UINT32    nMsgTotalCount;
	VOS_UINT32    nFreeCount;
	CPSS_MSG	  *pFreeHead;
	CPSS_MSG	  *pFreeTial;
	CPSS_MSG_TYPE StyleSend;
	CPSS_MSG_TYPE StyleRecv;
}CPSS_MSG_STYLE,*pCPSS_MSG_STYLE;
#endif   /* ----- #ifndef cpss_msg_INC  ----- */
 