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

#define CPSS_SOCKET_ERROR (-1)

#define  CPSS_MSG_SEND_TCP		0x01
#define  CPSS_MSG_SEND_UDP		0x02
#define  CPSS_MSG_RECV_TCP		0x04
#define  CPSS_MSG_RECV_UDP		0x08

typedef enum _CPSS_MSG_SELF_STAT_M{
	CPSS_MSG_SELF_STAT_FREE = 0x01,		/*空闲*/
	CPSS_MSG_SELF_STAT_RESERVE,			/*预约*/
	CPSS_MSG_SELF_STAT_USEING,			/*使用中*/
	CPSS_MSG_SELF_STAT_USED,			/*使用完*/
};

typedef enum _CPSS_MSG_RS_STAT_M{
	CPSS_MSG_RS_STAT_FREE,				/* 使用完     */
	CPSS_MSG_RS_STAT_RECVING,			/* 接受中      */
	CPSS_MSG_RS_STAT_RECVED,			/* 接受完了    */
	CPSS_MSG_RS_STAT_DOING,				/* 处理中      */
	CPSS_MSG_RS_STAT_SENDING,			/* 发送中       */
	CPSS_MSG_RS_STAT_SENDED,			/* 发送完了     */
};
/****************/
/*   请求方式方法    */
/****************/
typedef enum CPSS_MSG_TYPE_M{
	CPSS_MSG_INIT = 0x01,				/* 初始化 */
	CPSS_MSG_UNIT,						/* 反初始化 */
	CPSS_MSG_DEAL,						/* 处理 */
	CPSS_MSG_REQ,						/* 请求 */
	CPSS_MSG_RES,						/* 应答 */
	CPSS_MSG_SET,						/* 设定 */
	CPSS_MSG_DEL,						/* 删除 */
	CPSS_MSG_REG,						/* 注册 */
	CPSS_MSG_RESU,						/* 注册应答 */
	CPSS_MSG_CHK,						/* 检查 */
	CPSS_MSG_CHKRES,					/* 检查结果 */
};

/****************/
/*   请求内容    */
/****************/
typedef enum CPSS_REQUEST_CONTENT_M{
	CPSS_TYPE_SYS = 0x01,
	DBSVR_TYPE_CPUIDPID,
	DBSVR_TYPE_USER,
	XCAP_TYPE_NEW_CLIENT,
	XCAP_TYPE_URL,
	XCAP_TYPE_DATA,
	XCAP_TYPE_MONCLI_GETURL,
};
/****************/
/*   请求类型   */
/****************/
typedef enum CPSS_REQUEST_TYPE_M{
	CPSS_REQUEST_SYSTEM = 0x01,
	CPSS_REQUEST_TELNET,
	DBSVR_REQUEST_MGR,
	MONEY_REQUEST_MGR,
	XCAP_REQUEST_MONCLI,
	XCAP_REQUEST_URL,
};

/*
typedef enum CPSS_TYPE_SUB_T{
	CPSS_TYPE_CPUID_PID=0x11,
	CPSS_TYPE_CHECK_USE,
	CPSS_TYPE_GET_SUBURL,
};
*/
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
	VOS_SEND_SKT_TYPE_FINISH = 0x00,
	VOS_SEND_SKT_TYPE_INSERT = 0x01,
	VOS_SEND_SKT_TYPE_TCP = 0x02,
	VOS_SEND_SKT_TYPE_UDP = 0x04,
	VOS_SEND_ALL_OF_PID = 0x10,
	VOS_SEND_SAVE_TO_RECV = 0x20,
};

#define  CPSS_COMM_SEG_NAME		"JCPG"
typedef struct _CPSS_COM_PID_T
{
	VOS_UINT32 	ulCpuID;
	VOS_UINT32 	ulPID;
}CPSS_COM_PID,*pCPSS_COM_PID;
typedef struct _CPSS_COM_HEAD_T
{
	VOS_CHAR			strSegName[4];
	VOS_UINT32 			ulMsgLength;	//消息长度
	VOS_UINT32			uType;
	VOS_UINT32			uCmd;
	/*--------------------16byte----------------*/
	VOS_UINT32 			ulParentMsgID;	//父消息消息ID;
	VOS_UINT32 			ulRecvMsgID;	//接受消息ID
	VOS_UINT8 			RFU2[8];		//下一个源消息ID
	/*--------------------16byte----------------*/
	CPSS_COM_PID 		stDstProc;		//目标地址
	CPSS_COM_PID 		stSrcProc;		//源地址
	/*--------------------16byte----------------*/
	/*-----------------------------------------*/
}CPSS_COM_HEAD,*pCPSS_COM_HEAD;

#define  CPSS_MSG_HEAD_SIZE		sizeof(CPSS_COM_HEAD) //+(48)
//#define  CPSS_MSG_BUFFER_USED	(CPSS_MSG_BUFFER_SIZE - 4)
//#define  CPSS_MSG_BUF_HEAD_SIZE	(sizeof(VOS_UINT32)*4)
typedef struct _CPSS_COM_DATA_T
{
	CPSS_COM_HEAD		msghead;
	VOS_CHAR			rfu[12];
	VOS_CHAR*			strDataBuf;
}CPSS_COM_DATA,*pCPSS_COM_DATA;
typedef struct _CPSS_MSG_T
{
	VOS_VOID 		   * pTimer;			/*定时器句柄*/
	VOS_UINT8			 nSelfStat;			/*1:空闲   2:预约     3:使用中 4:使用完*/
	VOS_UINT8			 nRState;			/*5:接受中 6:接受完了 7:发送中 8:发送完了*/
	VOS_UINT8			 rfu[2];			/*rfu*/
	VOS_VOID		   * pClient;
	VOS_UINT32 			 ulMsgID;			//消息ID;
	/* ------------------16byte---------------------- */
	VOS_VOID		   * pXcapMgr;
	VOS_UINT32 			 rfu1[3];
	/* -------------------16byte----------------------- */
	VOS_VOID		   * pPid;
	VOS_UINT32 			 ulMsgLength;    //消息长度
	struct _CPSS_MSG_T * next;
	struct _CPSS_MSG_T * prev;
	/* ------------------16byte---------------------- */
	CPSS_COM_DATA        Body;
	/* ------------------64byte---------------------- */
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
 