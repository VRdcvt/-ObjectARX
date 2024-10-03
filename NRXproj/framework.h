#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Исключите редко используемые компоненты из заголовков Windows
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // некоторые конструкторы CString будут явными

#include <afxwin.h>         // основные и стандартные компоненты MFC
#include <afxext.h>         // Расширения MFC

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // классы MFC OLE
#include <afxodlgs.h>       // классы диалоговых окон MFC OLE
#include <afxdisp.h>        // классы автоматизации MFC
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // классы баз данных MFC ODBC
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // классы баз данных MFC DAO
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // поддержка MFC для типовых элементов управления Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // поддержка MFC для типовых элементов управления Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma once

//-----------------------------------------------------------------------------
//- 'DEBUG workaround' below prevents the MFC or ATL #include-s from pulling 
//- in "Afx.h" that would force the debug CRT through #pragma-s.
#if defined(_DEBUG) && !defined(NC_FULL_DEBUG)
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#pragma message ("     Compiling header files in release mode.")
#endif

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
//#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:

#include <unknwn.h>

#include "arxHeaders.h"

#pragma pack (pop)

//-----------------------------------------------------------------------------
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif


