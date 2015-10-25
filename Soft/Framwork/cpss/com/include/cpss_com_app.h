/*===============================================================================
 *
 *       Filename:  cpss_com_app.h
 *
 *    Description:  CGP 平台初始化 
 *
 *        Version:  1.0
 *        Created:  2011年05月21日 22时30分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 *=============================================================================*/
#ifndef  CPSS_COM_APP_INC
#define  CPSS_COM_APP_INC
#include "cpss_vk_print.h "
/***************************定义全局变量 *********************************/
/***************************定义函数     *********************************/

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init 
 *  Description:	CGP初始化  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 swp_init (int argc, char ** argv );
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_shell_cmd_init
 *  Description:	命令函数初始化  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_shell_cmd_init();	/* -----  end of function cpss_shell_cmd_init  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init_file
 *  Description:	设置ip和port  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 VOS_Registmsgporcess();
#endif   /* ----- #ifndef cpss_com_app_INC  ----- */

