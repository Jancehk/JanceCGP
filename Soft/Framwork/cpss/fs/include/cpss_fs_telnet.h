/*
 * =====================================================================================
 *
 *       Filename:  cpss_fs_telnet.h
 *
 *    Description:  TELNET 服务器
 *
 *        Version:  1.0
 *        Created:  2011年06月09日 23时34分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef _CPSS_FS_TELNET_H
#define _CPSS_FS_TELNET_H

#include "cpss_common.h"
#include "cpss_public.h"
#include "cpss_msg.h"

#define CPSS_TELNET_IAC			0XFF
#define CPSS_TELNET_DONT		0XFE
#define CPSS_TELNET_DO			0XFD
#define CPSS_TELNET_WILLNT		0XFC
#define CPSS_TELNET_WILL		0XFB
#define CPSS_TELNET_SB			0XFA
#define CPSS_TELNET_CTL			0X1B
typedef struct _STU_TELNET_ACI_T
{
	VOS_UINT8 mACI;
	VOS_UINT8 mACI1;
	VOS_UINT8 mACI2;
}TELNET_ACI,*pTELNET_ACI;
/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_init_proc
 *  Description:  初始化telnet服务器
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_init_proc(VOS_VOID *parg);


/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_timeout_proc
 *  Description:  telnet服务器消息超时处理
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_timeout_proc(VOS_VOID *pargc,VOS_UINT32 pargv);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_telnet_init
 *  Description:  初始化telnet服务器
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_telnet_init();

/* ===  FUNCTION  ==============================================================
 *         Name:  telnet_send_data
 *  Description:  初始化telnet服务器
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 telnet_send_data(pCPSS_MSG pMsgInfo, VOS_VOID * strBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);

/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void TELNET_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_TELNET, CPSS_PRINT_INFO, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintError
 *  Description:  
 * ========================================================================*/
void TELNET_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_TELNET, CPSS_PRINT_ERR,
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void TELNET_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_TELNET, CPSS_PRINT_WARN, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintError
 *  Description:  
 * ========================================================================*/
void TELNET_PrintDebug (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_TELNET, CPSS_PRINT_DBUG,
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */
/* ===  FUNCTION  =========================================================
 *         Name:  VOS_PrintInfo
 *  Description:  
 * ========================================================================*/
void TELNET_PrintDump (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if(VOS_OK != cpss_print(CPSS_PID_TELNET, CPSS_PRINT_DUMP, 
				szFilename,	ulLine, fmt, ap))
	{
		printf("print info error!\n");
	}
	va_end(ap);
}		/* -----  end of function VOS_Print_Info  ----- */

#endif
