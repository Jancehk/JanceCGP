// JanDBMain.h: interface for the CJanDBMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JANDDB_TABLE_CREATE_H)
#define AFX_JANDDB_TABLE_CREATE_H

/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 * ==========================================================================*/
VOS_UINT32 jandb_system_tab_create(CAdoConnection * g_adoConnection);
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 * ==========================================================================*/
VOS_UINT32 jandb_notepad_create(CAdoConnection * g_adoConnection);
/* ===  FUNCTION  ==============================================================
 *         Name:  jandb_money_create
 *  Description:  检查数据库文件
 * ==========================================================================*/
VOS_UINT32 jandb_money_create(CAdoConnection * g_adoConnection);
#endif // !defined(AFX_JANDBMAIN_H__8D48BC26_DE7D_4AD8_87F0_A608B60F0C0D__INCLUDED_)
