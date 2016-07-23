/*===============================================================================
 *
 *       Filename:  cpss_DEFAULT.h
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

#ifndef  CPSS_DEFAULT_INC_H
#define  CPSS_DEFAULT_INC_H
//客户信息
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef VOS_TRUE
#define VOS_TRUE 		0
#define VOS_FALSE 		1
#endif
#ifndef VOS_OK
#define VOS_OK 		0
#define VOS_ERR 	1
#define VOS_OK1 	2
//#define VOS_ABN 	3
#endif
#if (OS_TYPE == OS_TYPE_WINDOW)
	typedef long	 			VOS_INT64;
	typedef unsigned long 		VOS_UINT64;
#elif (OS_TYPE == OS_TYPE_LINUX)
	typedef long long 			VOS_INT64;
	typedef unsigned long long 	VOS_UINT64;
#endif
	typedef int					VOS_INT32;
	typedef unsigned int 		VOS_UINT32;
	typedef short 				VOS_INT16;
	typedef unsigned short 		VOS_UINT16;
	typedef char 				VOS_INT8;
	typedef unsigned char 		VOS_UINT8;
	
	typedef int 				VOS_BOOL;
	typedef void 				VOS_VOID;
#ifdef _UNICODE
	typedef TCHAR				VOS_CHAR;
#else
	typedef char				VOS_CHAR;
#endif
	typedef char *				VOS_STRING;

/************************************************************************/
/*						 网络连接信息设置                               */
/************************************************************************/

#ifndef  CPSS_TCP_TELNET
#define  CPSS_TCP_TELNET	230
#endif
#ifndef  CPSS_TCP_FTP
#define  CPSS_TCP_FTP		210
#endif
#ifndef CPSS_CGP_PORT
#define CPSS_CGP_PORT		12
#endif

#ifndef CPSS_CGP_DEFAUT
#define CPSS_CGP_DEFAUT		0
#endif

#ifndef  CPSS_TCP_LINK_MAX	
#define  CPSS_TCP_LINK_MAX  10240
#define  CPSS_UDP_LINK_MAX  10240
#endif
typedef enum _VOS_CLIENT_STAT_M{
	VOS_CLIENT_UNCONNECT = 11,
	VOS_CLIENT_CONNECT,
	VOS_CLIENT_CONNECTTING,
	VOS_CLIENT_CONNECTED,
	VOS_CLIENT_DISCONNECT,
};

#define VOS_SOCKET_CHEKC	0x000C0000
#define VOS_SOCKET_TYPE		0x00030000
#define VOS_SOCKET_STAT		0x000F0000
#define VOS_SOCKET_PORT		0X0000FFFF

#define VOS_SOCKET_NO		0x04000000
#define VOS_SOCKET_IN		0x08000000
#define VOS_SOCKET_OUT		0x0C000000

typedef enum _VOS_SOCKET_M
{
	VOS_SOCKET_UDP=0x01,
	VOS_SOCKET_TCP=0x02,
	VOS_SOCKET_ACC=0x04,
};

#define VOS_TCP_PID			(VOS_SOCKET_TCP<<16)
#define VOS_UDP_PID			(VOS_SOCKET_UDP<<16)
#define VOS_ACC_PID			(VOS_SOCKET_ACC<<16)

#if (OS_TYPE == OS_TYPE_WINDOW)
#define VOS_Sleep(x)  Sleep(x*10)
#elif (OS_TYPE == OS_TYPE_LINUX)
#define VOS_Sleep(x)  sleep(x)
#endif

typedef enum _CPSS_GET_SKT_LINK_NUM_T{
	CPSS_GET_SKT_LINK_SOCKET =11,
};

typedef enum _CPSS_SET_GET_TYPE_NUM_T{
	CPSS_GET_TYPE_CPUID =11,
	CPSS_GET_TYPE_PID,
	CPSS_SET_TYPE_CPUID,
	CPSS_SET_TYPE_PID,
	CPSS_SET_TO_BUFFER,
	CPSS_SET_TO_STUCPUID,
};

typedef enum _VOS_SYSTEM_TYPE_NUM_T{
	CPSS_SYSTEM_TYPE_SELF = 3,
	CPSS_SYSTEM_TYPE_DBSVR,
	CPSS_SYSTEM_TYPE_XCAP,
	CPSS_SYSTEM_TYPE_MONEY,
	CPSS_SYSTEM_TYPE_NUM,
};

typedef enum _VOS_SUBSYS_TYPE_NUM_T{
	CPSS_SUBSYS_TYPE_SELF = 20,
	CPSS_SUBSYS_TYPE_FW,
	CPSS_SUBSYS_TYPE_TLN,
	CPSS_SUBSYS_TYPE_FTP,
	CPSS_SUBSYS_TYPE_DBSVR,
	CPSS_SUBSYS_TYPE_XCAP,
	CPSS_SUBSYS_TYPE_MOCLI,
	CPSS_SUBSYS_TYPE_MONEY,
	CPSS_SUBSYS_TYPE_NUM,
};
#define CPSS_STRING_CGP		"CGP"
#define CPSS_STRING_FW		"FRAM"
#define CPSS_STRING_TELNET	"TELNET"
#define CPSS_STRING_FTP		"FTP"
#define CPSS_STRING_DBSVR	"DBS"
#define CPSS_STRING_XCAP	"XCAP"
#define CPSS_STRING_MOCLI	"MOCLI"
#define CPSS_STRING_MONEY	"MONEY"

#define CPSSPIDMAXNAME   32
/***********************************************************************/
/*								About Thread						   */
/***********************************************************************/
#if   (OS_TYPE == OS_TYPE_WINDOW)
typedef void * HANDLE;
#elif (OS_TYPE == OS_TYPE_LINUX)
typedef pthread_t * HANDLE;
#endif
//平台模块定义
#define CPSS_PID_MAIN	VOS_SOCKET_IN | VOS_TCP_PID |CPSS_CGP_PORT
#define CPSS_PID_FW		VOS_SOCKET_IN | VOS_UDP_PID |CPSS_CGP_DEFAUT
#define CPSS_PID_TELNET	VOS_SOCKET_IN | VOS_TCP_PID |CPSS_CGP_DEFAUT//CPSS_TCP_TELNET
#define CPSS_PID_FTP	VOS_SOCKET_IN | VOS_TCP_PID |CPSS_TCP_FTP

//时间定义
typedef struct CPSS_TIME_T	TIME;

//打印级别设置
#define  CPSS_PRINT_STR_INFO	"INFO"
#define  CPSS_PRINT_STR_ERR		"ERRO"
#define  CPSS_PRINT_STR_WARN	"WARN"
#define  CPSS_PRINT_STR_DBUG	"DBUG"
#define  CPSS_PRINT_STR_DUMP	"DUMP"
typedef enum CPSS_PRINT_LEVEL_M{
	CPSS_PRINT_ERR = 1,
	CPSS_PRINT_WARN,
	CPSS_PRINT_INFO,
	CPSS_PRINT_DBUG,
	CPSS_PRINT_DUMP,
};
typedef enum CPSS_PRINT_LEVEL{
	CPSS_PRINT_FUN_CLOSE,			//屏幕log出力无效	
	CPSS_PRINT_ALL_INFO,			//所有信息
	CPSS_PRINT_NO_FILELINE,			//没有文件和行号
	CPSS_PRINT_NO_DATETIME,			//日期不显示
	CPSS_PRINT_ONLY_PIDDATA,		//显示进程信息和内容
	CPSS_PRINT_ONLY_DATA,			//只显示内容
};
#define  CPSS_NO_PRINTF				0x80000000
#define  CPSS_PRINTF_BUFFER			0x90000000
#define  CPSS_PRINTF_BUFCTL			0xa0000000
//#define  CPSS_PRINTF_BUFFER		0xb0000000
//#define  CPSS_PRINTF_BUFFER		0xc0000000
//#define  CPSS_PRINTF_BUFFER		0xd0000000
//#define  CPSS_PRINTF_BUFFER		0xe0000000
//#define  CPSS_PRINTF_BUFFER		0xf0000000

#define  GET_CPUID_FROMCHAR(g_szIP)
#define  CPSS_SEP_BUFF		"\r\n\t"

#define CPSS_CLIENT_NAME_LENGTH		128
#define CPSS_CLIENT_PASS_LENGTH		128

#if 0
/***********************************************************************/
/*								About Xcap						   */
/***********************************************************************/
typedef struct _XCAP_SER_MGR_T{
	VOS_UINT8  uStat;
	VOS_VOID * Req_Mgr;
	VOS_CHAR   RootName[28];
	VOS_CHAR   URL;
}XCAP_SER_MGR,*pXCAP_SER_MGR;
#endif

#endif   /* ----- #ifndef cpss_msg_INC  ----- */
 