/*
 * =====================================================================================
 *
 *       Filename:  cpss_vk_print.h
 *
 *    Description:  打印函数头文件
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 21时26分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef CPSS_VK_PRINT
#define CPSS_VK_PRINT
#ifdef _cplusplus
extern "C" {
#endif
#include <stdarg.h>
#include <sys\stat.h>
#include "cpss_common.h"
#include "cpss_public.h"

#ifndef MY_COLOR
#define MY_COLOR
typedef enum CPSS_PRINT_TYPE_M{
	NONE,
	RED,
	LIGHT_RED,
	GREEN,
	LIGHT_GREEN,
	BLUE,
	LIGHT_BLUE,
	DARY_GRAY,
	CYAN,
	LIGHT_CYAN,
	PURPLE,
	LIGHT_PURPLE,
	BROWN,
	YELLOW,
	LIGHT_GRAY,
	WHITE,
};
#if (OS_TYPE == OS_TYPE_WINDOW)
#define NONE_INT          15
#define RED_INT           12
#define LIGHT_RED_INT     2
#define GREEN_INT         10
#define LIGHT_GREEN_INT   2
#define BLUE_INT          13
#define LIGHT_BLUE_INT    2
#define DARY_GRAY_INT     2
#define CYAN_INT          6
#define LIGHT_CYAN_INT    2
#define PURPLE_INT        2
#define LIGHT_PURPLE_INT  2
#define BROWN_INT         2
#define YELLOW_INT        14
#define LIGHT_GRAY_INT    2
#define WHITE_INT         2
#elif (OS_TYPE == OS_TYPE_LINUX)
#define NONE_STR          "\033[m"    
#define RED_STR           "\033[0;32;31m"    
#define LIGHT_RED_STR     "\033[1;31m"    
#define GREEN_STR         "\033[0;32;32m"    
#define LIGHT_GREEN_STR   "\033[1;32m"    
#define BLUE_STR          "\033[0;32;34m"    
#define LIGHT_BLUE_STR    "\033[1;34m"    
#define DARY_GRAY_STR     "\033[1;30m"    
#define CYAN_STR          "\033[0;36m"    
#define LIGHT_CYAN_STR    "\033[1;36m"    
#define PURPLE_STR        "\033[0;35m"    
#define LIGHT_PURPLE_STR "\033[1;35m"    
#define BROWN_STR         "\033[0;33m"    
#define YELLOW_STR        "\033[1;33m"    
#define LIGHT_GRAY_STR    "\033[0;37m"    
#define WHITE_STR         "\033[1;37m" 
#endif   
#endif
#define PRINTT_PID_LEN	16
//#define PRINTT_TYPE_LEN	16
#define PRINTT_TIME_LEN	32
#define PRINTT_FILE_LEN	16
#define PRINTT_INFO_LEN	128


#define PRINT_TRACE_LOG_MAX_BUFFER		1024
#define PRINT_SLEEP_TIME				1000
#define PRINT_TRACE_SIZE				1000
#define PRINT_LOG_COUNT					300

#define PRINTF_TYPE_HEAD				1
#define PRINTF_TYPE_TAIL				2
typedef enum PRITF_MOVE_TYPE_M{
	PRITF_MOVE_FREE_TO_USED = 3,
	PRITF_MOVE_USED_TO_FREE,
	PRITF_MOVE_GET_FREE,
};
typedef enum PRINT_INFO_M{
	PRINT_INFO_FREE = 3,							//空闲
	PRINT_INFO_RESERVE,								//预约
	PRINT_INFO_USED,								//使用
};

typedef struct _PRINT_INFO_T
{
	//VOS_UINT8	bIsPrint;
	VOS_UINT8	ulState;							//打印状态
	VOS_UINT8	ulLogID;							// log id
	VOS_CHAR	szPrintPid[PRINTT_PID_LEN];			//PID名称	CGP
	VOS_CHAR	szPrintTime[PRINTT_TIME_LEN];		//打印日期  2011-09-31
	VOS_CHAR	szPrintFile[PRINTT_FILE_LEN];		//打印文件	cpss_common
	VOS_UINT32	ulPrintLine;						//打印行数	21
	VOS_UINT32	ulThreadID;							//线程ID		
	VOS_UINT32	ulPrintType;						//打印属性	INFO WARN  ERR DEBUG  DUMP
	VOS_UINT32	ulPrintSize;						//打印大小	100
	VOS_CHAR	*pszPrintInfo;						//打印内容	XXXXX [PRINTT_INFO_LEN]
	struct _PRINT_INFO_T * prev;
	struct _PRINT_INFO_T * next;
}PRINT_INFO,*pPRINT_INFO;

typedef struct MANAGE_PRINT_T
{
	VOS_UINT8	g_printShowlevel;				//日志级别
	VOS_UINT8	g_printlevel;					//日志级别
	VOS_UINT8	g_traceo_on_off;				//日志保存开关
	HANDLE		hPrintHandle;					//日志打印线程句柄
	HANDLE      g_hConsole;						//日志打印窗口句柄
	VOS_Event	g_LogEvent;						//日志打印投递消息句柄
	VOS_Event	g_MsgEvent;						//日志打印空闲空间等待
	VOS_MUTEX	hMutex;							//日志内容共享
	VOS_INT32	dwThreadId;						//日志线程ID
	VOS_UINT32	ulPrintCount;					//日志消息队列总数
	VOS_UINT32	ulFreePrintCount;				//日志当前打印的总数
	VOS_UINT32	ulSaveCount;					//日志保存条数
	PRINT_INFO	*pUsedPrintinfoHead;			//日志占用消息头地址
	PRINT_INFO	*pUsedPrintinfoTail;			//日志占用消息尾地址
	PRINT_INFO	*pFreePrintinfoHead;			//日志空闲消息头地址
	PRINT_INFO	*pFreePrintinfoTail;			//日志空闲消息尾地址
}MANAGE_PRINT,*pMANAGE_PRINT;
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_print_init
 *  Description:	打印平台初始化 
 * ==========================================================================*/
VOS_UINT32 cpss_print_init ();

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_init_print
 *  Description:	初始化线程log 
 * ==========================================================================*/
VOS_VOID cpss_init_print (VOS_UINT32 ulThreadID);
/*===  FUNCTION  ==============================================================
 *         Name:  cpss_print
 *  Description:	平台打印函数
 * =============================================================================*/
VOS_UINT32 cpss_print(
		VOS_UINT32 	ucProcessPid,		//子模块ID
		VOS_UINT32  ulPrintType,			//日志类型(INFO  ERR  WARM)
		VOS_STRING  szFilename,			//打印日志文件名称
		VOS_UINT32  ulLine,				//打印的行数
		VOS_STRING 	szFormat,
		va_list argptr);				//日志内容
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void VOS_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
		/* -----  end of function VOS_Print  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintError
 *  Description:  
 * ========================================================================*/
void VOS_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintWarn
 *  Description:  
 * ========================================================================*/
void VOS_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintDebug
 *  Description:  
 * ========================================================================*/
void VOS_PrintDebug (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintBuffer
 *  Description:  
 * ========================================================================*/
VOS_UINT32 VOS_PrintBuffer (
		VOS_VOID * pstuBuffer,
		const VOS_STRING fmt, ...);

/* ===  FUNCTION  =========================================================
*         Name:  VOS_PrintBufferRelease
*  Description:
* ========================================================================*/
VOS_UINT32 VOS_PrintBufferRelease(VOS_VOID * pstuBuffer);
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintDump
 *  Description:  
 * ========================================================================*/
#define VOS_PrintDump (				\
		 VOS_STRING szFilename,		\
		 VOS_UINT32 ulLine,			\
		 VOS_VOID * pstrVoid,		\
		 VOS_UINT32 ulLeng){		\
	if(VOS_OK != cpss_print_dump(CPSS_PID_MAIN, CPSS_PRINT_DUMP, \
		__FILE__, __LINE__, pstrVoid, ulLeng))		\
	{												\
		printf("print info error!\n");				\
	}												\
}


#ifdef _cplusplus
}
#endif
#endif
