
// PakageFileDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <string>
#include <vector>
#include "afxwin.h"
#include "MyThread.h"


// CPakageFileDlg �Ի���
class CPakageFileDlg : public CDialog
{
// ����
public:
	CPakageFileDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PAKAGEFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_lcList;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	std::vector<std::wstring> m_FileList;
	CEdit m_editOutput;

	bool PackDirThread();

	mylib::CThread m_Thread;
};
