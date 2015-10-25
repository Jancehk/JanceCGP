// JanDBMain.cpp: implementation of the CJanDBMain class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "JanDBMain.h"
#include "JanDBTableCreate.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// This is an example of an exported variable
//Jance_API_EXP int nWwww=0;
CJanDBMain g_JanAdoMgr;
CJanDBMain::CJanDBMain()
{
	ptConnArray.RemoveAll();
	ptRecdArray.RemoveAll();
	nConnCount = 0;
	nRecdCount = 0;
	InitializeCriticalSection(&mcs);
}

CJanDBMain::~CJanDBMain()
{
	DeleteCriticalSection(&mcs);	
}

VOS_UINT32 CJanDBMain::AddConn(CAdoConnection *pstuAdoConn)
{
	StuAdoConn tItem;
	MyLock();
	if (nConnCount>0x8FFFFFFF)
	{
		nConnCount = 0;
	}
	nConnCount++;
	MyULock();
	tItem.pstuAdoConn = pstuAdoConn;
	tItem.m_index = nConnCount;
	if (ptConnArray.Add(tItem) > 0)
	{
		return -1;
	}
	return tItem.m_index;
}

VOS_UINT32 CJanDBMain::AddRecd(CAdoRecordSet * pstuAdoRecd)
{
	StuAdoRecd tItem;
	MyLock();
	if (nRecdCount>0x8FFFFFFF)
	{
		nRecdCount = 0;
	}
	nRecdCount++;
	MyULock();
	tItem.pstuAdoRecd = pstuAdoRecd;
	tItem.m_index = nRecdCount;
	if (ptRecdArray.Add(tItem))
	{
		return -1;
	}
	return tItem.m_index;
}

CAdoConnection * CJanDBMain::GetConn(VOS_UINT32 uAdoConn)
{
	StuAdoConn tItem;
	VOS_UINT32 m_idx = 0,m_count = ptConnArray.GetSize();
	for(m_idx = 0; m_idx <m_count; m_idx++ ) 
	{
		tItem = ptConnArray.GetAt(m_idx);
		if (tItem.m_index == uAdoConn)
		{
			return tItem.pstuAdoConn;
		}
	}
	return NULL;
}
CAdoRecordSet * CJanDBMain::GetRecd(VOS_UINT32 uAdoRecd)
{
	StuAdoRecd tItem;
	VOS_UINT32 m_idx = 0,m_count = ptRecdArray.GetSize();
	for(m_idx = 0; m_idx <m_count; m_idx++ ) 
	{
		tItem = ptRecdArray.GetAt(m_idx);
		if (tItem.m_index == uAdoRecd)
		{
			return tItem.pstuAdoRecd;
		}
	}
	return NULL;
}
VOS_UINT32 CJanDBMain::DelConn(VOS_UINT32 uAdoConn)
{
	StuAdoConn tItem;
	VOS_UINT32 m_idx = 0,m_count = ptConnArray.GetSize();
	for(m_idx = 0; m_idx <m_count; m_idx++ ) 
	{
		tItem = ptConnArray.GetAt(m_idx);
		if (tItem.m_index == uAdoConn)
		{
			ptConnArray.RemoveAt(m_idx);
			delete tItem.pstuAdoConn;
			return VOS_OK;
		}
	}
	return VOS_ERR;
}
VOS_UINT32 CJanDBMain::DelRecd(VOS_UINT32 uAdoRecd)
{
	StuAdoRecd tItem;
	VOS_UINT32 m_idx = 0,m_count = ptRecdArray.GetSize();
	for(m_idx = 0; m_idx <m_count; m_idx++ ) 
	{
		tItem = ptRecdArray.GetAt(m_idx);
		if (tItem.m_index == uAdoRecd)
		{
			ptRecdArray.RemoveAt(m_idx);
			delete tItem.pstuAdoRecd;
			return VOS_OK;
		}
	}
	return VOS_ERR;
}


void CJanDBMain::MyLock()
{
	EnterCriticalSection(&mcs);
}

void CJanDBMain::MyULock()
{
	LeaveCriticalSection(&mcs);	
}

// This is an example of an exported function.
Jance_API_EXP VOS_UINT32 CreateAccMDB(VOS_CHAR * m_strPath)
{
	VOS_UINT32 uRet = VOS_ERR;
	CAdoConnection g_adoConnection;
	if (0 == m_strPath[0])
	{
		return uRet;
	}
	if (!g_adoConnection.CreateMdb(m_strPath))
	{
		uRet = -1;
		goto END_PROC;
	}
	if (!g_adoConnection.ConnectAccessPWD(m_strPath))
	{
		//AfxMessageBox("connect access is error%s",m_DbPath);
		uRet = -2;
		goto END_PROC;
	}
	//g_adoConnection.Execute("ALTER DATABASE PASSWORD [JanceHeiter] NULL");
	if (VOS_OK != jandb_system_tab_create(&g_adoConnection))
	{
		uRet = -3;
		goto END_PROC;
	}
	printf("system tab create ok\n");
	if (VOS_OK != jandb_notepad_create(&g_adoConnection))
	{
		uRet = -4;
		goto END_PROC;
	}
	printf("notepad tab create ok\n");
	if (VOS_OK != jandb_money_create(&g_adoConnection))
	{
		uRet = -5;
		goto END_PROC;
	}
	printf("money tab create ok\n");
	g_adoConnection.Close();
	uRet = VOS_OK;
END_PROC:
	return uRet;
}
Jance_API_EXP VOS_UINT32 OpenAccMDB(VOS_CHAR * m_strPath, VOS_UINT32 *adoConnect)
{
	TCHAR lpszError[255] = {0};
	CAdoConnection * g_adoConnection = NULL;

	g_adoConnection = new CAdoConnection;
	VOS_UINT32 uRet = VOS_ERR;
	try
	{
		if (!g_adoConnection->ConnectAccess(m_strPath))//,"JanceHeiter"
		{
			//sprintf(lpszError,"打开数据库出错%s",m_strPath);
			uRet = -1;
			goto END_PROC;
		}
	}
	catch (CMemoryException* e)
	{
		e->GetErrorMessage(lpszError,255);
		AfxMessageBox(lpszError,MB_ICONERROR);
		uRet = -2;
		goto END_PROC;
	}
	catch (CFileException* e)
	{
		e->GetErrorMessage(lpszError,255);
		AfxMessageBox(lpszError,MB_ICONERROR);
		uRet = -3;
		goto END_PROC;
	}
	catch (CException* e)
	{
		e->GetErrorMessage(lpszError,255);
		AfxMessageBox(lpszError,MB_ICONERROR);
		uRet = -4;
		goto END_PROC;
	}
	*adoConnect = g_JanAdoMgr.AddConn(g_adoConnection);
	if (*adoConnect > 0)
	{
		uRet = VOS_OK;		
	}
END_PROC:
	return uRet;
}

Jance_API_EXP VOS_UINT32 CloseAccMDB(VOS_UINT32 adoConnect)
{
	CAdoConnection * g_adoConnection = NULL;
	g_adoConnection = (CAdoConnection *)g_JanAdoMgr.GetConn(adoConnect);
	if (NULL == g_adoConnection)
	{
		return VOS_OK;
	}
	g_adoConnection->Close();
	if (VOS_OK != g_JanAdoMgr.DelConn(adoConnect))
	{
		return -2;
	}
	return VOS_OK;
}
Jance_API_EXP VOS_UINT32 ExecCommand(VOS_UINT32 adoConnect,VOS_CHAR * pstrCmd)
{
	CAdoConnection * g_adoConnection = NULL;
	g_adoConnection = (CAdoConnection *)g_JanAdoMgr.GetConn(adoConnect);
	if (NULL == g_adoConnection)
	{
		return -1;
	}
	if (NULL == g_adoConnection->Execute(pstrCmd))
	{
		return -2;
	}
	return VOS_OK;
}

Jance_API_EXP VOS_UINT32 ExecRecd(VOS_UINT32 adoConnect,VOS_UINT32 *adoRecord, VOS_CHAR * pstrCmd)
{
	CAdoRecordSet * m_record = NULL;
	if (NULL == *adoRecord)
	{
		m_record = new CAdoRecordSet;
		*adoRecord = g_JanAdoMgr.AddRecd(m_record);
	}
	else
	{
		m_record = g_JanAdoMgr.GetRecd(*adoRecord);
	}
	if (NULL == m_record)
	{
		return -1;
	}
	m_record->SetAdoConnection(g_JanAdoMgr.GetConn(adoConnect));
	if (!m_record->SetCursorLocation())
	{
		AfxMessageBox(_T("dd1"));
		return -2;
	}
	m_record->Close();
	if (!m_record->Open(pstrCmd))
	{
		AfxMessageBox(_T("dd2"));
		return -3;
	}
	return VOS_OK;
}
Jance_API_EXP VOS_UINT32 FreeRecd(VOS_UINT32 adoRecord)
{
	CAdoRecordSet * m_record = g_JanAdoMgr.GetRecd(adoRecord);
	if (NULL == m_record)
	{
		return -1;
	}
	m_record->Close();
	return g_JanAdoMgr.DelRecd(adoRecord);
}
Jance_API_EXP VOS_UINT32 GetRecordCount(VOS_UINT32 adoRecord)
{
	CAdoRecordSet * m_record = g_JanAdoMgr.GetRecd(adoRecord);
	if (NULL == m_record)
	{
		return -1;
	}
	return m_record->GetRecordCount();
}
