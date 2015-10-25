// JanDBMain.cpp: implementation of the CJanDBMain class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "JanDBTableCreate.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// This is an example of an exported variable
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_system_tab_create(CAdoConnection * g_adoConnection)
{
	CString strCommand;
	//系统cpuid和pid信息表
	g_adoConnection->Execute(_T("CREATE TABLE CPUIDPID (SubSys NUMBER,m_Index NUMBER,ProcessCPuID VARCHAR(50),Pid NUMBER)"));
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_notepad_create(CAdoConnection * g_adoConnection)
{
	CString strCommand;
	//文档目录
	g_adoConnection->Execute(_T("CREATE TABLE textPar (m_id NUMBER,name VARCHAR(50),type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (1,\"我的文档\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (2,\"婷婷文档\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (3,\"插件管理\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (4,\"VC编程\",1)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (5,\"随便收集\",1)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (6,\"重要资料\",1)"));
	
	//插件管理
	g_adoConnection->Execute(_T("CREATE TABLE textPlug (m_id NUMBER,name VARCHAR(50),path VARCHAR(50),type NUMBER)"));
	//TMP
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (7,\"日常\",3)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPlug (m_id,name,path,type) VALUES (1,\"记账管理\",\"MoneyManage.dll\",7)"));

	//文档内容
	g_adoConnection->Execute(_T("CREATE TABLE textMyt (m_id NUMBER,m_myid NUMBER,name VARCHAR(50),files TEXT)"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (1,\"Socket\",4,\"Socket\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (2,\"多线程\",4,\"多线程\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (3,\"笑话\",5,\"笑话\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (4,\"命令大全\",5,\"命令大全\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (5,\"密码信息\",6,\"密码信息\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (6,\"资料合集\",6,\"资料合集\")"));
	
	//记事本设置
	strCommand.Format(_T("CREATE TABLE %s(writer VARCHAR(20),fontname VARCHAR(10),fontcolor NUMBER,fontsize NUMBER,backcolor NUMBER,nowfile NUMBER,status NUMBER)"),_T("TextConfig"));
	g_adoConnection->Execute(strCommand);
	strCommand.Format(_T("INSERT INTO TextConfig(writer,fontname, fontcolor,fontsize,backcolor,nowfile,status) VALUES (\"%s\", \"%s\",%d,%d,%d,%d,%d)"),_T("毒蝎子"),_T("宋体"),255,300,0,2,0);
	g_adoConnection->Execute(strCommand);

	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  jandb_money_create
 *  Description:  检查数据库文件
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_money_create(CAdoConnection * g_adoConnection)
{
	//记账管理_用户信息表
	g_adoConnection->Execute(_T("CREATE TABLE ManageUse (m_id NUMBER,usename VARCHAR(20),passwd VARCHAR(50),power VARCHAR(20))"));
	g_adoConnection->Execute(_T("INSERT INTO ManageUse (m_id, usename, passwd, power) VALUES (1, \"老公\", \"123\", \"11111111\")"));
	g_adoConnection->Execute(_T("INSERT INTO ManageUse (m_id, usename, passwd, power) VALUES (2, \"老婆\", \"123\", \"11111111\")"));
	
	//记账管理_内容表
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageConfigJzLx (m_id NUMBER,name VARCHAR(30),reason TEXT,remark TEXT,type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (1, \"工资\",\"公司发工资\",\"...\", 1)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (2, \"生意\",\"....\",\"...\", 1)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (3, \"买衣服\",\"买衣服\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (4, \"吃饭\",\"一天吃饭\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (5, \"房租\",\"自己一个月的房租\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (6, \"电话费\",\"自己的电话费\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (7, \"旅游\",\"去...玩\",\"...\", 2)"));
	//记账管理_帐单表
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageAccount (m_id NUMBER,type NUMBER,fs VARCHAR(30),m_time DATE,m_User VARCHAR(30),m_money NUMBER,m_from NUMBER,reason TEXT,remark TEXT)"));
	//记账管理_理账表
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageLZJl (m_id NUMBER,type NUMBER,fs VARCHAR(30),m_timefrom DATE,m_timeto DATE,m_total NUMBER,m_write VARCHAR(30),m_date DATE,reason TEXT)"));
	//记账管理_日志表
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageLog (m_id NUMBER,type NUMBER,m_time DATE,m_User VARCHAR(30),reason TEXT)"));
	//记账管理_来源表
	g_adoConnection->Execute(_T("CREATE TABLE MoneySource (m_id NUMBER,name VARCHAR(50),type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (1,\"工资卡\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (2,\"招行性用卡\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (3,\"交行性用卡\",0)"));
	
	return VOS_OK;
}