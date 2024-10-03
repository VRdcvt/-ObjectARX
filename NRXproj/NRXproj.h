// NRXproj.h: основной файл заголовка для библиотеки DLL NRXproj
//

#pragma once

#ifndef __AFXWIN_H__
#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CNRXprojApp
// Реализацию этого класса см. в файле NRXproj.cpp
//

class CNRXprojApp : public CWinApp
{
public:
	CNRXprojApp();

	// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
