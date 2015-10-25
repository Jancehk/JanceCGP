/*
* =====================================================================================
*
*       Filename:  cpss_vk_shell.h
*
*    Description:  �������ͷ�ļ�
*
*        Version:  1.0
*        Created:  2011��11��15�� 00ʱ04��58��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Jance (), shenghs2004@163.com
*        Company:  Jance���繤����
*
* =====================================================================================
*/
#ifndef CPSS_VK_SHELL
#define CPSS_VK_SHELL
#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
	
#define  CPSS_CMD_BUFFER_SIZE	512
typedef struct _SHELL_CMD_MGR_T{
	VOS_Event  hCmdEvent;
	VOS_UINT8  bIsEvent;
	HANDLE	   hExec;
	VOS_UINT32 ThreadID;
	VOS_CHAR * pstrCommand;
	VOS_VOID * pVoidClient;
}SHELL_CMD_MGR,*pSHELL_CMD_MGR;

typedef struct _SHELL_CMD_TIMER_T{
	VOS_UINT32 ThreadID;
}SHELL_CMD_TIMER,*pSHELL_CMD_TIMER;

/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_init
-  Description:	�������ʼ��  
- =============================================================================*/
VOS_UINT32 shell_cmd_init ();
/*===  FUNCTION  ===============================================================
-         Name:  shell_user_check
-  Description:	�������ʼ��  
- =============================================================================*/
VOS_UINT32 shell_user_check (VOS_VOID * pVoidClient);

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_result_use_info
 *  Description: 
 * ==========================================================================*/
VOS_UINT32 cpss_result_use_info(pCPSS_USER_INFO pstuUserInfo);
/*===  FUNCTION  ===============================================================
-         Name:  cpss_system
-  Description:	  ִ������    
- =============================================================================*/
VOS_UINT32 cpss_system(VOS_VOID * pVoidClient, VOS_CHAR * command);
/*===  FUNCTION  ===============================================================
-         Name:  shell_cmd_main
-  Description:	�������ʼ��  
- =============================================================================*/
VOS_UINT32 shell_cmd_main ();
#ifdef _cplusplus
}
#endif
#endif