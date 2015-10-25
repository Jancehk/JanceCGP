
#include "stdafx.h"
#include "ADO.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]="";
#define new DEBUG_NEW
#endif

/*########################################################################
			  ------------------------------------------------
							    数值类型转换
			  ------------------------------------------------
  ########################################################################*/
COleDateTime vartodate(const _variant_t& var)
{
	COleDateTime value;
	switch (var.vt) 
	{
	case VT_DATE:
		{
			value = var.date;
		}
		break;
	case VT_EMPTY:
	case VT_NULL:
		value.SetStatus(COleDateTime::null);
		break;
	default:
		value.SetStatus(COleDateTime::null);
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}

CTime vartoctime(const _variant_t& var)
{
	CTime value;
	switch (var.vt) 
	{
	case VT_DATE:
		{
			value = CTime(var.date);
		}
		break;
	case VT_EMPTY:
	case VT_NULL:
//		value.SetStatus(CTime::null);
		break;
	default:
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}
COleCurrency vartocy(const _variant_t& var)
{
	COleCurrency value;
	switch (var.vt) 
	{
	case VT_CY:
		value = (CURRENCY)var.cyVal;
		break;
	case VT_EMPTY:
	case VT_NULL:
		value.m_status = COleCurrency::null;
		break;
	default:
		value.m_status = COleCurrency::null;
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}

bool vartobool(const _variant_t& var)
{
	bool value = false;
	switch (var.vt)
	{
	case VT_BOOL:
		value = var.boolVal ? true : false;
	case VT_EMPTY:
	case VT_NULL:
		break;
	default:
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}

BYTE vartoby(const _variant_t& var)
{
	BYTE value = 0;
	switch (var.vt)
	{
	case VT_I1:
	case VT_UI1:
		value = var.bVal;
		break;
	case VT_NULL:
	case VT_EMPTY:
		value = 0;
		break;
	default:
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}	
	return value;
}

short vartoi(const _variant_t& var)
{
	short value = 0;
	switch (var.vt)
	{
	case VT_BOOL:
		value = var.boolVal;
		break;
	case VT_UI1:
	case VT_I1:
		value = var.bVal;
		break;
	case VT_I2:
	case VT_UI2:
		value = var.iVal;
		break;
	case VT_NULL:
	case VT_EMPTY:
		value = 0;
		break;
	default:
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}	
	return value;
}

long vartol(const _variant_t& var)
{
	long value = 0;
	switch (var.vt)
	{
	case VT_BOOL:
		value = var.boolVal;
		break;
	case VT_UI1:
	case VT_I1:
		value = var.bVal;
		break;
	case VT_UI2:
	case VT_I2:
		value = var.iVal;
		break;
	case VT_I4:
	case VT_UI4:
		value = var.lVal;
		break;
	case VT_INT:
		value = var.intVal;
		break;
	case VT_R4:
		value = (long)(var.fltVal + 0.5);
		break;
	case VT_R8:
		value = (long)(var.dblVal + 0.5);
		break;
	case VT_DECIMAL:
		value = (long)var;
		break;
	case VT_CY:
		value = (long)var;
		break;
	case VT_BSTR://字符串
	case VT_LPSTR://字符串
	case VT_LPWSTR://字符串
		value = _wtoi((LPCTSTR)(_bstr_t)var);
		break;
	case VT_NULL:
	case VT_EMPTY:
		value = 0;
		break;
	default:
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}

double vartof(const _variant_t& var)
{
	double value = 0;
	switch (var.vt)
	{
	case VT_R4:
		value = var.fltVal;
		break;
	case VT_R8:
		value = var.dblVal;
		break;
	case VT_DECIMAL:
		value = (double)var;
		break;
	case VT_CY:
		value = (double)var;
		break;
	case VT_BOOL:
		value = var.boolVal;
		break;
	case VT_UI1:
	case VT_I1:
		value = var.bVal;
		break;
	case VT_UI2:
	case VT_I2:
		value = var.iVal;
		break;
	case VT_UI4:
	case VT_I4:
		value = var.lVal;
		break;
	case VT_INT:
		value = var.intVal;
		break;
	case VT_BSTR://字符串
	case VT_LPSTR://字符串
	case VT_LPWSTR://字符串
		value = _wtof((LPCTSTR)(_bstr_t)var);
		break;
	case VT_NULL:
	case VT_EMPTY:
		value = 0;
		break;
	default:
		value = 0;
		TRACE(_T("Warning: 未处理的 _variant_t 类型值;%s;  %d\n"),__LINE__);
	}
	return value;
}

CString vartostr(const _variant_t &var)
{
	CString strValue;
	
	switch (var.vt)
	{
	case VT_BSTR://字符串
	case VT_LPSTR://字符串
	case VT_LPWSTR://字符串
		strValue = (LPCTSTR)(_bstr_t)var;
		break;
	case VT_I1:
	case VT_UI1:
		strValue.Format(_T("%d"), var.bVal);
		break;
	case VT_I2://短整型
		strValue.Format(_T("%d"), var.iVal);
		break;
	case VT_UI2://无符号短整型
		strValue.Format(_T("%d"), var.uiVal);
		break;
	case VT_INT://整型
		strValue.Format(_T("%d"), var.intVal);
		break;
	case VT_I4: //整型
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_I8: //长整型
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_UINT://无符号整型
		strValue.Format(_T("%d"), var.uintVal);
		break;
	case VT_UI4: //无符号整型
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_UI8: //无符号长整型
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_VOID:
		strValue.Format(_T("%8x"), var.byref);
		break;
	case VT_R4://浮点型
		strValue.Format(_T("%.4f"), var.fltVal);
		break;
	case VT_R8://双精度型
		strValue.Format(_T("%.8f"), var.dblVal);
		break;
	case VT_DECIMAL: //小数
		strValue.Format(_T("%.8f"), (double)var);
		break;
	case VT_CY:
		{
			COleCurrency cy = var.cyVal;
			strValue = cy.Format();
		}
		break;
	case VT_BLOB:
	case VT_BLOB_OBJECT:
	case 0x2011:
		strValue = "[BLOB]";
		break;
	case VT_BOOL://布尔型

		strValue = var.boolVal ? "TRUE" : "FALSE";
		break;
	case VT_DATE: //日期型
		{
			DATE dt = var.date;
			COleDateTime da = COleDateTime(dt); 
			strValue = da.Format(_T("%Y-%m-%d %H:%M:%S"));
		}
		break;
	case VT_NULL://NULL值
		strValue = "";
		break;
	case VT_EMPTY://空
		strValue = "";
		break;
	case VT_UNKNOWN://未知类型
	default:
		strValue = "UN_KNOW";
		break;
	}
	return strValue;
}

/*########################################################################
			 ------------------------------------------------
					CAdoConnection class 构造/析构函数
			 ------------------------------------------------
  ########################################################################*/

CAdoConnection::CAdoConnection()
{
	//创建 Connection 对象---------------------------
	m_pConnection.CreateInstance("ADODB.Connection");
	#ifdef _DEBUG
	if (m_pConnection == NULL)
	{
		AfxMessageBox(_T("Connection 对象创建失败! 请确认是否初始化了COM环境\r\n"));
	}
	#endif
	ASSERT(m_pConnection != NULL);
}

CAdoConnection::~CAdoConnection()
{
	if (m_pConnection != NULL)
	{
		Release();
	}
}

/*========================================================================
	Name:		连接到数据源.
	-----------------------------------------------------
	Params:		[lpszConnect]: 连接字符串, 包含连接信息.
				[lOptions]:	可选. 决定该方法是以同步还是异步的方式连接数据
						源. 可以是如下某个常量:
		[常量]					[说明] 
		----------------------------------
		adConnectUnspecified	(默认)同步方式打开连接. 
		adAsyncConnect			异步方式打开连接. Ado用 ConnectComplete 事
						件来通知已经完成连接. 
==========================================================================*/
BOOL CAdoConnection::Open(LPCTSTR lpszConnect, long lOptions)
{
	ASSERT(m_pConnection != NULL);
	ASSERT(AfxIsValidString(lpszConnect));
	
	if (_tcscmp(lpszConnect, _T("")) != 0)
	{
		m_strConnect = lpszConnect;
	}

	if (m_strConnect.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (IsOpen()) Close();

	try
	{
		// 连接数据库 ---------------------------------------------
		return (m_pConnection->Open(_bstr_t(LPCTSTR(m_strConnect)), "", "", lOptions) == S_OK);
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: 连接数据库. :%s\n"), e.ErrorMessage());
		TRACE(_T("%s\r\n"), GetLastErrorText());
		return FALSE;
	} 
	catch (...)
	{
		TRACE(_T("Warning: 连接数据库时发生未知错误:"));
	}
	return FALSE;
}

/*========================================================================
	Name:	连接 SQL SERVER 数据库. 
	-----------------------------------------------------
	Params:		[dbsrc]:	SQL SERVER 服务器名.
				[dbname]:	默认的数据库名.
				[user]:		用户名.
				[pass]:		密码.
==========================================================================*/
BOOL CAdoConnection::ConnectSQLServer(CString dbsrc, CString dbname, CString user, CString pass, long lOptions)
{
	CString strConnect = _T("Provider=SQLOLEDB.1; Data Source=") + dbsrc + 
						 _T("; Initial Catalog=") + dbname  +
						 _T("; User ID=") + user + 
						 _T("; PWD=") + pass;
	return Open(LPCTSTR(strConnect), lOptions);
}

/*========================================================================
	Name:	连接 ACCESS 数据库. 
	-----------------------------------------------------
	Params:		[dbpath]:	MDB 数据库文件路径名.
				[pass]:		访问密码.
===========================================================================*/
BOOL CAdoConnection::ConnectAccess(CString dbpath, CString pass, long lOptions)
{
	CString strConnect = _T("Provider=Microsoft.Jet.OLEDB.4.0; Data Source=") + dbpath;
	if (pass != _T("")) 
	{
		strConnect += _T(";Jet OLEDB:Database Password=") + pass + _T(";");
	}
	return Open(LPCTSTR(strConnect), lOptions);
}

/*========================================================================
	Name:	通过 udl 文件连接数据库. 
	-----------------------------------------------------
	Params:		[strFileName]:	UDL 数据库连接文件路径名.
==========================================================================*/
BOOL CAdoConnection::OpenUDLFile(LPCTSTR strFileName, long lOptions)
{
	CString strConnect = _T("File Name=");
	strConnect += strFileName;
	return Open(LPCTSTR(strConnect), lOptions);
}

/*========================================================================
	Name:	关闭与数据源的连接.
	-----------------------------------------------------
	Remarks: 使用 Close 方法可关闭 Connection 对象以便释放所有关联的系统资源. 
==========================================================================*/
void CAdoConnection::Close()
{
	try
	{
		if (m_pConnection != NULL && IsOpen()) 
		{
			m_pConnection->Close();
		}
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: 关闭数据库. :%s;%d\n"), e.ErrorMessage(),__LINE__);
	} 
}

/*========================================================================
	Name:	关闭连接并释放对象.
	-----------------------------------------------------
	Remarks: 关闭连接并释放connection对象.
==========================================================================*/
void CAdoConnection::Release()
{
	if (IsOpen()) Close();
	m_pConnection.Release();
}

/*========================================================================
	Name:		执行指定的查询、SQL 语句、存储过程等.
    ----------------------------------------------------------
	Remarks:	请参考 CAdoRecordSet 类的Open方法. 返回的 Recordset 对象始
			终为只读、仅向前的游标.
==========================================================================*/
_RecordsetPtr CAdoConnection::Execute(LPCTSTR lpszSQL, long lOptions)
{
	ASSERT(m_pConnection != NULL);
	ASSERT(AfxIsValidString(lpszSQL));

	try
	{
		return m_pConnection->Execute(_bstr_t(lpszSQL), NULL, lOptions);
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: Execute  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
	Remarks:	请参考 CAdoRecordSet 类 Cancel 方法.
==========================================================================*/
BOOL CAdoConnection::Cancel()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->Cancel();
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: Cancel  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
	Name:	取得最后发生的.
==========================================================================*/
CString CAdoConnection::GetLastErrorText()
{
	ASSERT(m_pConnection != NULL);
	CString strErrors = "";
	try
	{
		if (m_pConnection != NULL)
		{
			ErrorsPtr pErrors = m_pConnection->Errors;
			CString strError;
			for (long n = 0; n < pErrors->Count; n++)
			{
				ErrorPtr pError = pErrors->GetItem(n);
				strError.Format(_T("Description:%s\r\nState:%s, Native: %d, Source:%s\r\n"),
								(LPCTSTR)pError->Description,
								(LPCTSTR)pError->SQLState,
										 pError->NativeError,
								(LPCTSTR)pError->Source);
				strErrors += strError;
			}
		}
		return strErrors;
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetLastError  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return strErrors;
	} 
	return strErrors;
}

ErrorsPtr CAdoConnection::GetErrors()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		if (m_pConnection != NULL)
		{
			return m_pConnection->Errors;
		}
		return NULL;
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetErrors  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return NULL;
	} 
	return NULL;
}

ErrorPtr CAdoConnection::GetError(long index)
{
	ASSERT(m_pConnection != NULL);
	try
	{
		if (m_pConnection != NULL)
		{
			ErrorsPtr pErrors =  m_pConnection->Errors;
			if (index >= 0 && index < pErrors->Count)
			{
				return pErrors->GetItem(_variant_t(index));
			}
		}
		return NULL;
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetError  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return NULL;
	} 
	return NULL;
}

/*========================================================================
	Name:		取得连接时间.
==========================================================================*/
long CAdoConnection::GetConnectTimeOut()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->GetConnectionTimeout();
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetConnectTimeOut  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return -1;
	} 
	return -1;
}

/*========================================================================
	Name:		设置连接时间.
==========================================================================*/
BOOL CAdoConnection::SetConnectTimeOut(long lTime)
{
	ASSERT(m_pConnection != NULL);
	try
	{
		m_pConnection->PutConnectionTimeout(lTime);
		return TRUE;
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: SetConnectTimeOut  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
}

/*========================================================================
	Name:		取得默认数据库的名称.
==========================================================================*/
CString CAdoConnection::GetDefaultDatabase()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return CString(LPCTSTR(_bstr_t(m_pConnection->GetDefaultDatabase())));
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetDefaultDatabase  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return CString(_T(""));
	} 
}

/*========================================================================
	Name:		取得 Connection 对象提供者的名称.
==========================================================================*/
CString CAdoConnection::GetProviderName()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return CString(LPCTSTR(_bstr_t(m_pConnection->GetProvider())));
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetProviderName  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return CString(_T(""));
	} 
}

/*========================================================================
	Name:		取得 ADO 的版本号
==========================================================================*/
CString CAdoConnection::GetVersion()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return CString(LPCTSTR(_bstr_t(m_pConnection->GetVersion())));
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetVersion  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return CString(_T(""));
	} 
}

/*========================================================================
	Name:		取得对象的状态(同 Recordset 对象的 GetState 方法).
	-----------------------------------------------------
	returns:	返回下列常量之一的长整型值.
		[常量]				[说明] 
		----------------------------------
		adStateClosed		指示对象是关闭的. 
		adStateOpen			指示对象是打开的. 
	-----------------------------------------------------
	Remarks:		可以随时使用 State 属性取得指定对象的当前状态.
==========================================================================*/
long CAdoConnection::GetState()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->GetState();
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetState . :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return -1;
	} 
	return -1;
}

/*========================================================================
	Name:	检测连接对象是否为打开状态.
==========================================================================*/
BOOL CAdoConnection::IsOpen()
{
	try
	{
		return (m_pConnection != NULL && (m_pConnection->State & adStateOpen));
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: IsOpen  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
	Name:		取得在 Connection 对象中修改数据的可用权限.
    ----------------------------------------------------------
	returns:	返回以下某个 ConnectModeEnum 的值.
		[常量]				 [说明] 
		----------------------------------
		adModeUnknown		 默认值. 表明权限尚未设置或无法确定. 
		adModeRead			 表明权限为只读. 
		adModeWrite			 表明权限为只写. 
		adModeReadWrite		 表明权限为读/写. 
		adModeShareDenyRead  防止其他用户使用读权限打开连接. 
		adModeShareDenyWrite 防止其他用户使用写权限打开连接. 
		adModeShareExclusive 防止其他用户打开连接. 
		adModeShareDenyNone  防止其他用户使用任何权限打开连接.
    ----------------------------------------------------------
	Remarks: 使用 Mode 属性可设置或返回当前连接上提供者正在使用的访问权限.
==========================================================================*/
ConnectModeEnum CAdoConnection::GetMode()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->GetMode();
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: GetMode . :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return adModeUnknown;
	} 
	return adModeUnknown;
}

/*========================================================================
	Name:		设置在 Connection 中修改数据的可用权限. 请参考 GetMode 方法.
    ----------------------------------------------------------
	Remarks:	只能在关闭 Connection 对象时方可设置 Mode 属性.
==========================================================================*/
BOOL CAdoConnection::SetMode(ConnectModeEnum mode)
{
	ASSERT(m_pConnection != NULL);
	ASSERT(!IsOpen());

	try
	{
		m_pConnection->PutMode(mode);
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: SetMode . :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return TRUE;
}

/*========================================================================
    Name:		从数据源获取数据库信息.
	-----------------------------------------------------
	Params:		QueryType  所要运行的模式查询类型.
	-----------------------------------------------------
	returns:	返回包含数据库信息的 Recordset 对象. Recordset 将以只读、静态
			游标打开.
==========================================================================*/
_RecordsetPtr CAdoConnection::OpenSchema(SchemaEnum QueryType)
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->OpenSchema(QueryType, vtMissing, vtMissing);
	}
	catch(_com_error e)
	{
		TRACE(_T("Warning: OpenSchema :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return NULL;
	} 
	return NULL;
}

/*########################################################################
			  ------------------------------------------------
									事务处理
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
    Name:		开始新事务.
	-----------------------------------------------------
	returns:	对于支持嵌套事务的数据库来说, 在已打开的事务中调用 BeginTrans 
	方法将开始新的嵌套事务. 返回值将指示嵌套层次: 返回值为 1 表示已打开顶层
	事务 (即事务不被另一个事务所嵌套), 返回值为 2 表示已打开第二层事务(嵌套
	在顶层事务中的事务), 依次类推.
	-----------------------------------------------------
	Remarks:	一旦调用了 BeginTrans 方法, 在调用 CommitTrans 或 RollbackTrans
	结束事务之前, 数据库将不再立即提交所作的任何更改.
		
==========================================================================*/
long CAdoConnection::BeginTrans()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return m_pConnection->BeginTrans();
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: BeginTrans  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return -1;
	} 
	return -1;
}

/*========================================================================
    Name:		保存任何更改并结束当前事务.
	-----------------------------------------------------
	Remarks:	调用 CommitTrans 或 RollbackTrans 只影响最新打开的事务; 在
	处理任何更高层事务之前必须关闭或回卷当前事务.
==========================================================================*/
BOOL CAdoConnection::CommitTrans()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return SUCCEEDED(m_pConnection->CommitTrans());
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: CommitTrans  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return FALSE;
}

/*========================================================================
    Name:		取消当前事务中所作的任何更改并结束事务.
==========================================================================*/
BOOL CAdoConnection::RollbackTrans()
{
	ASSERT(m_pConnection != NULL);
	try
	{
		return SUCCEEDED(m_pConnection->RollbackTrans());
	}
	catch (_com_error e)
	{
		TRACE(_T("Warning: RollbackTrans  :%s;%d\n"), e.ErrorMessage(),__LINE__);
		return FALSE;
	} 
	return FALSE;
}



BOOL CAdoConnection::Disconnect()
{
	try
	{
		if (m_pConnection != NULL && m_pConnection->State == adStateOpen) 
		{
			if (SUCCEEDED(m_pConnection->Close()))
			{
				return TRUE;
			}
		}
	}
	catch(_com_error e)
	{
		TRACE(_T(":( 关闭数据库:%s\n"), e.ErrorMessage());
		return FALSE;
	} 
	return FALSE;
}

BOOL CAdoConnection::CreateMdb(CString m_Data)
{
	HRESULT hr = S_OK;
	CString DBName="";
	DBName.Format(_T("Provider=Microsoft.JET.OLEDB.4.0;Data source=%s"),m_Data); 
	try 
	{ 
		ADOX::_CatalogPtr m_pCatalog = NULL; 
		hr = m_pCatalog.CreateInstance(__uuidof (ADOX::Catalog)); 
		if(FAILED(hr)) 
		{ 
			_com_issue_error(hr); 
		} 
		else 
		{ 
			m_pCatalog->Create(_bstr_t((LPCTSTR)DBName));
		} 
	}  
	catch(_com_error &e) 
	{ 
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}
	return TRUE;
}


BOOL CAdoConnection::ConnectAccessPWD(CString dbpath, CString pass, long lOptions)
{
	CString strConnect =_T("");
	strConnect.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0; User ID=Admin; Data Source= %s ;Mode=Share Deny Read|Share Deny Write"),dbpath);
	if (pass != _T("")) 
	{
		strConnect += _T("Jet OLEDB:Database Password=") + pass;
	}
	return Open(LPCTSTR(strConnect), 0);
}
