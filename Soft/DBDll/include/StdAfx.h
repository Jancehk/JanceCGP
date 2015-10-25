// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B9E82EBF_88E2_4E68_B657_2F29B139047F__INCLUDED_)
#define AFX_STDAFX_H__B9E82EBF_88E2_4E68_B657_2F29B139047F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <windows.h>
#include "Ado.h"
//#pragma comment(lib,"nafxcwd.lib")
#include "CJanceDBCommon.h"
#include "cpss_common.h"

#pragma warning(disable:4089)
#pragma warning(disable:4089)
#pragma warning(disable:4244)

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B9E82EBF_88E2_4E68_B657_2F29B139047F__INCLUDED_)
