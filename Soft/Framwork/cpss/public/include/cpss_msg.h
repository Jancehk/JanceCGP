/*===============================================================================
 *
 *       Filename:  cpss_msg.h
 *
 *    Description:  ��Ϣ��ʽ����ͷ�ļ�
 *
 *        Version:  1.0
 *        Created:  2011��05��23�� 21ʱ37��19��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
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
	CPSS_MSG_SELF_STAT_FREE = 0x01,		/*����*/
	CPSS_MSG_SELF_STAT_RESERVE,			/*ԤԼ*/
	CPSS_MSG_SELF_STAT_USEING,			/*ʹ����*/
	CPSS_MSG_SELF_STAT_USED,			/*ʹ����*/
};

typedef enum _CPSS_MSG_RS_STAT_M{
	CPSS_MSG_RS_STAT_FREE,				/* ʹ����     */
	CPSS_MSG_RS_STAT_RECVING,			/* ������      */
	CPSS_MSG_RS_STAT_RECVED,			/* ��������    */
	CPSS_MSG_RS_STAT_DOING,				/* ������      */
	CPSS_MSG_RS_STAT_SENDING,			/* ������       */
	CPSS_MSG_RS_STAT_SENDED,			/* ��������     */
};
/****************/
/*   ����ʽ����    */
/****************/
typedef enum CPSS_MSG_TYPE_M{
	CPSS_MSG_INIT = 0x01,				/* ��ʼ�� */
	CPSS_MSG_UNIT,						/* ����ʼ�� */
	CPSS_MSG_DEAL,						/* ���� */
	CPSS_MSG_REQ,						/* ���� */
	CPSS_MSG_RES,						/* Ӧ�� */
	CPSS_MSG_SET,						/* �趨 */
	CPSS_MSG_DEL,						/* ɾ�� */
	CPSS_MSG_REG,						/* ע�� */
	CPSS_MSG_RESU,						/* ע��Ӧ�� */
	CPSS_MSG_CHK,						/* ��� */
	CPSS_MSG_CHKRES,					/* ����� */
};

/****************/
/*   ��������    */
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
/*   ��������   */
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
	VOS_SOCKET_STAT_FREEING=0x11,	/*����*/
	VOS_SOCKET_STAT_RESERVE,		/*ԤԼ*/
	VOS_SOCKET_STAT_USEING,			/*ʹ��*/
	VOS_SOCKET_STAT_USED,			/*ʹ�����*/
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
	VOS_UINT32 			ulMsgLength;	//��Ϣ����
	VOS_UINT32			uType;
	VOS_UINT32			uCmd;
	/*--------------------16byte----------------*/
	VOS_UINT32 			ulParentMsgID;	//����Ϣ��ϢID;
	VOS_UINT32 			ulRecvMsgID;	//������ϢID
	VOS_UINT8 			RFU2[8];		//��һ��Դ��ϢID
	/*--------------------16byte----------------*/
	CPSS_COM_PID 		stDstProc;		//Ŀ���ַ
	CPSS_COM_PID 		stSrcProc;		//Դ��ַ
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
	VOS_VOID 		   * pTimer;			/*��ʱ�����*/
	VOS_UINT8			 nSelfStat;			/*1:����   2:ԤԼ     3:ʹ���� 4:ʹ����*/
	VOS_UINT8			 nRState;			/*5:������ 6:�������� 7:������ 8:��������*/
	VOS_UINT8			 rfu[2];			/*rfu*/
	VOS_VOID		   * pClient;
	VOS_UINT32 			 ulMsgID;			//��ϢID;
	/* ------------------16byte---------------------- */
	VOS_VOID		   * pXcapMgr;
	VOS_UINT32 			 rfu1[3];
	/* -------------------16byte----------------------- */
	VOS_VOID		   * pPid;
	VOS_UINT32 			 ulMsgLength;    //��Ϣ����
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
 