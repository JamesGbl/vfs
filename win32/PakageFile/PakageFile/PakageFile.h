
// PakageFile.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPakageFileApp:
// �йش����ʵ�֣������ PakageFile.cpp
//

class CPakageFileApp : public CWinAppEx
{
public:
	CPakageFileApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPakageFileApp theApp;