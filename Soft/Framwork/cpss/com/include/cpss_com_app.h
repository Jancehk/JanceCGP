/*===============================================================================
 *
 *       Filename:  cpss_com_app.h
 *
 *    Description:  CGP ƽ̨��ʼ�� 
 *
 *        Version:  1.0
 *        Created:  2011��05��21�� 22ʱ30��30��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 *=============================================================================*/
#ifndef  CPSS_COM_APP_INC
#define  CPSS_COM_APP_INC
#include "cpss_vk_print.h "
/***************************����ȫ�ֱ��� *********************************/
/***************************���庯��     *********************************/

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init 
 *  Description:	CGP��ʼ��  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 swp_init (int argc, char ** argv );
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_shell_cmd_init
 *  Description:	�������ʼ��  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_shell_cmd_init();	/* -----  end of function cpss_shell_cmd_init  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_init_file
 *  Description:	����ip��port  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 VOS_Registmsgporcess();
#endif   /* ----- #ifndef cpss_com_app_INC  ----- */

