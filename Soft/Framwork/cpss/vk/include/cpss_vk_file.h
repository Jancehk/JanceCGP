/*===============================================================================
 *
 *       Filename:  cpss_vk_file.h
 *
 *    Description:  �ļ�����
 *
 *        Version:  1.0
 *        Created:  2011��05��28�� 22ʱ31��38��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
 *
 *=============================================================================*/
#ifndef CPSS_VK_FILE
#define CPSS_VK_FILE
#include "cpss_public.h"
#ifdef _cplusplus
extern "C" {
#endif
/*===  FUNCTION  ===============================================================
*         Name:  cpss_set_current_path
*  Description:	  
*  Input      :	
*  OutPut     :	
*  Return     :    
* =============================================================================*/
VOS_UINT32 cpss_set_current_path (VOS_CHAR * pSelfPath);
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_current_path
 *  Description:	  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_get_current_path (VOS_CHAR * pSelfPath);/* -----  end of function cpss_get_current_path  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_ip_address
 *  Description:	  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_get_ip_address (VOS_CHAR * pstrCPUID, VOS_CHAR * pstrOutput);/* -----  end of function cpss_get_ipaddress  ----- */
/*===  FUNCTION  ===============================================================
 *         Name:  cpss_get_file_data
 *  Description:	  
 * =============================================================================*/
VOS_CHAR* cpss_get_file_data(VOS_CHAR * pstrPath);

#ifdef _cplusplus
}
#endif
#endif
