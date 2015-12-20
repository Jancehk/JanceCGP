/*==============================================================================
 -
 -       Filename:  cpss_linx_entery.c
 -
 -    Description:  CGP 主函数
 -
 -        Version:  1.0
 -        Created:  2011年05月21日 21时59分25秒
 -       Revision:  none
 -       Compiler:  gcc
 -
 -         Author:  Jance (), shenghs2004@163.com
 -        Company:  Jance网络工作室
 -
 - ============================================================================*/
#include "cpss_common.h"
#include "cpss_config.h"
#include "cpss_linux_entery.h"
#include "cpss_vk_print.h"
#include "cpss_vk_shell.h"
#include "cpss_com_app.h"
static VOS_INT32 app_init(int argc,char ** argv)
{
	VOS_UINT32 ulRet = VOS_ERR;
	if (VOS_OK != swp_init(argc,argv))
	{
		return ulRet;
	}
	if (VOS_OK != shell_cmd_init())
	{
		return VOS_ERR;
	}
	
	return VOS_OK;
}
BOOL MyHandler( DWORD dwCtrlType ) 
{
	switch (dwCtrlType) 
    { 
	case CTRL_C_EVENT:  // Handle the CTRL+C signal. 
	//	return TRUE;
	case CTRL_CLOSE_EVENT:   // CTRL+CLOSE: confirm that the user wants to exit. 
		if (VOS_OK != cpss_set_thread_exit())
		{
			return VOS_ERR;
		}
		return TRUE; 
	case CTRL_BREAK_EVENT:  // Pass other signals to the next handler. 
	case CTRL_LOGOFF_EVENT: 
	case CTRL_SHUTDOWN_EVENT: 
	default: 
		VOS_PrintInfo(__FILE__,__LINE__,"accept is error");
		return FALSE; 
    }
    return TRUE;
}
int main(int argc,char ** argv)
{
#if (OS_TYPE == OS_TYPE_LINUX)
#elif (OS_TYPE == OS_TYPE_WINDOW)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)MyHandler, TRUE);
#endif
	if (VOS_OK != app_init(argc,argv))
	{
		goto EXIT_OK;
	}
	if (VOS_OK != cpss_shell_cmd_main())
	{
		goto EXIT_OK;
	}
EXIT_OK:
	if (VOS_OK != cpss_wait_thread_exit())
	{
		return VOS_ERR;
	}
	return VOS_OK;
}
