// JanDBMain.h: interface for the CJanDBMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JANDBMAIN_H__8D48BC26_DE7D_4AD8_87F0_A608B60F0C0D__INCLUDED_)
#define AFX_JANDBMAIN_H__8D48BC26_DE7D_4AD8_87F0_A608B60F0C0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"
class CJanDBMain  
{
public:
	typedef struct _StuAdoConn_T
	{
		CAdoConnection * pstuAdoConn;
		VOS_UINT32  m_index;
	}StuAdoConn,*pStuAdoConn;
	typedef struct _StuAdoRecd_T
	{
		CAdoRecordSet * pstuAdoRecd;
		VOS_UINT32  m_index;
	}StuAdoRecd,*pStuAdoRecd;
	VOS_UINT32 AddConn(CAdoConnection * pstuAdoConn);
	VOS_UINT32 AddRecd(CAdoRecordSet * pstuAdoRecd);
	CAdoConnection * GetConn(VOS_UINT32 uAdoConn);
	CAdoRecordSet  * GetRecd(VOS_UINT32 uAdoRecd);
	VOS_UINT32 DelConn(VOS_UINT32 uAdoConn);
	VOS_UINT32 DelRecd(VOS_UINT32 uAdoRecd);
	CJanDBMain();
	virtual ~CJanDBMain();
private:
	void MyULock();
	void MyLock();
	CRITICAL_SECTION mcs;
	VOS_UINT32 nConnCount;
	VOS_UINT32 nRecdCount;
	CArray <StuAdoConn,StuAdoConn> ptConnArray;
	CArray <StuAdoRecd,StuAdoRecd> ptRecdArray;
};

#endif // !defined(AFX_JANDBMAIN_H__8D48BC26_DE7D_4AD8_87F0_A608B60F0C0D__INCLUDED_)
