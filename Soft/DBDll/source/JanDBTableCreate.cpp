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
 *  Description:  ������ݿ��ļ�
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_system_tab_create(CAdoConnection * g_adoConnection)
{
	CString strCommand;
	//ϵͳcpuid��pid��Ϣ��
	g_adoConnection->Execute(_T("CREATE TABLE CPUIDPID (SubSys NUMBER,m_Index NUMBER,ProcessCPuID VARCHAR(50),Pid NUMBER)"));
	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  dbsvr_check_file
 *  Description:  ������ݿ��ļ�
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_notepad_create(CAdoConnection * g_adoConnection)
{
	CString strCommand;
	//�ĵ�Ŀ¼
	g_adoConnection->Execute(_T("CREATE TABLE textPar (m_id NUMBER,name VARCHAR(50),type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (1,\"�ҵ��ĵ�\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (2,\"�����ĵ�\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (3,\"�������\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (4,\"VC���\",1)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (5,\"����ռ�\",1)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (6,\"��Ҫ����\",1)"));
	
	//�������
	g_adoConnection->Execute(_T("CREATE TABLE textPlug (m_id NUMBER,name VARCHAR(50),path VARCHAR(50),type NUMBER)"));
	//TMP
	g_adoConnection->Execute(_T("INSERT INTO  textPar (m_id,name,type) VALUES (7,\"�ճ�\",3)"));
	g_adoConnection->Execute(_T("INSERT INTO  textPlug (m_id,name,path,type) VALUES (1,\"���˹���\",\"MoneyManage.dll\",7)"));

	//�ĵ�����
	g_adoConnection->Execute(_T("CREATE TABLE textMyt (m_id NUMBER,m_myid NUMBER,name VARCHAR(50),files TEXT)"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (1,\"Socket\",4,\"Socket\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (2,\"���߳�\",4,\"���߳�\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (3,\"Ц��\",5,\"Ц��\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (4,\"�����ȫ\",5,\"�����ȫ\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (5,\"������Ϣ\",6,\"������Ϣ\")"));
	g_adoConnection->Execute(_T("INSERT INTO  textMyt (m_id,name,m_myid ,files) VALUES (6,\"���Ϻϼ�\",6,\"���Ϻϼ�\")"));
	
	//���±�����
	strCommand.Format(_T("CREATE TABLE %s(writer VARCHAR(20),fontname VARCHAR(10),fontcolor NUMBER,fontsize NUMBER,backcolor NUMBER,nowfile NUMBER,status NUMBER)"),_T("TextConfig"));
	g_adoConnection->Execute(strCommand);
	strCommand.Format(_T("INSERT INTO TextConfig(writer,fontname, fontcolor,fontsize,backcolor,nowfile,status) VALUES (\"%s\", \"%s\",%d,%d,%d,%d,%d)"),_T("��Ы��"),_T("����"),255,300,0,2,0);
	g_adoConnection->Execute(strCommand);

	return VOS_OK;
}
/* ===  FUNCTION  ==============================================================
 *         Name:  jandb_money_create
 *  Description:  ������ݿ��ļ�
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 jandb_money_create(CAdoConnection * g_adoConnection)
{
	//���˹���_�û���Ϣ��
	g_adoConnection->Execute(_T("CREATE TABLE ManageUse (m_id NUMBER,usename VARCHAR(20),passwd VARCHAR(50),power VARCHAR(20))"));
	g_adoConnection->Execute(_T("INSERT INTO ManageUse (m_id, usename, passwd, power) VALUES (1, \"�Ϲ�\", \"123\", \"11111111\")"));
	g_adoConnection->Execute(_T("INSERT INTO ManageUse (m_id, usename, passwd, power) VALUES (2, \"����\", \"123\", \"11111111\")"));
	
	//���˹���_���ݱ�
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageConfigJzLx (m_id NUMBER,name VARCHAR(30),reason TEXT,remark TEXT,type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (1, \"����\",\"��˾������\",\"...\", 1)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (2, \"����\",\"....\",\"...\", 1)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (3, \"���·�\",\"���·�\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (4, \"�Է�\",\"һ��Է�\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (5, \"����\",\"�Լ�һ���µķ���\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (6, \"�绰��\",\"�Լ��ĵ绰��\",\"...\", 2)"));
	g_adoConnection->Execute(_T("INSERT INTO MoneyManageConfigJzLx (m_id,name,reason,remark,type) VALUES (7, \"����\",\"ȥ...��\",\"...\", 2)"));
	//���˹���_�ʵ���
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageAccount (m_id NUMBER,type NUMBER,fs VARCHAR(30),m_time DATE,m_User VARCHAR(30),m_money NUMBER,m_from NUMBER,reason TEXT,remark TEXT)"));
	//���˹���_���˱�
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageLZJl (m_id NUMBER,type NUMBER,fs VARCHAR(30),m_timefrom DATE,m_timeto DATE,m_total NUMBER,m_write VARCHAR(30),m_date DATE,reason TEXT)"));
	//���˹���_��־��
	g_adoConnection->Execute(_T("CREATE TABLE MoneyManageLog (m_id NUMBER,type NUMBER,m_time DATE,m_User VARCHAR(30),reason TEXT)"));
	//���˹���_��Դ��
	g_adoConnection->Execute(_T("CREATE TABLE MoneySource (m_id NUMBER,name VARCHAR(50),type NUMBER)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (1,\"���ʿ�\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (2,\"�������ÿ�\",0)"));
	g_adoConnection->Execute(_T("INSERT INTO  MoneySource (m_id,name,type) VALUES (3,\"�������ÿ�\",0)"));
	
	return VOS_OK;
}