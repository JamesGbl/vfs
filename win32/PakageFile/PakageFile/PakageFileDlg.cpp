
// PakageFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PakageFile.h"
#include "PakageFileDlg.h"
#include "RunApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPakageFileDlg �Ի���




CPakageFileDlg::CPakageFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPakageFileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPakageFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lcList);
	DDX_Control(pDX, IDC_EDIT_DEBUG, m_editOutput);
}

BEGIN_MESSAGE_MAP(CPakageFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CPakageFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPakageFileDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BUTTON_SELECT_A, &CPakageFileDlg::OnBnClickedButtonSelectA)
    ON_BN_CLICKED(IDC_BUTTON_SELECT_R, &CPakageFileDlg::OnBnClickedButtonSelectR)
    ON_BN_CLICKED(IDC_BUTTON_CLEANUP, &CPakageFileDlg::OnBnClickedButtonCleanup)
END_MESSAGE_MAP()


// CPakageFileDlg ��Ϣ�������

BOOL CPakageFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	DWORD dwStyle = m_lcList.GetExtendedStyle(); //��ȡ��ǰ��չ��ʽ
	dwStyle |= LVS_EX_FULLROWSELECT; //ѡ��ĳ��ʹ���и�����report���ʱ��
	dwStyle |= LVS_EX_GRIDLINES; //�����ߣ�report���ʱ��
	dwStyle |= LVS_EX_CHECKBOXES; //��ѡ�report���ʱ��
	m_lcList.SetExtendedStyle(dwStyle); //������չ���

	m_lcList.InsertColumn(0, _T("�ļ���"), LVCFMT_LEFT, 700); //������
	m_lcList.InsertColumn(1, _T("״̬"), LVCFMT_LEFT, 150);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL PackDir(CString strApp,CString strDir,CString& strDebug)
{

	CString szParam;
	szParam.Format(L" %s ",  strDir);

	CString strOutputFile = strDir+ L".pak";
	DeleteFile(strOutputFile);

	CRunApp rApp;
	rApp.Run(strApp, szParam, NULL,SW_SHOW);
	CString strResult = rApp;

	if( !PathFileExists(strOutputFile) )
	{
		strDebug.AppendFormat( L"PackDir Dir=\"%s\" Failed\r\n",strDir );
		if( !strResult.IsEmpty() )
		{
			strDebug += strResult;
			strDebug += L"\r\n";
		}
		return FALSE;
	}

	strDebug.AppendFormat( L"PackDir Dir=\"%s\" OK\r\n",strDir );
	if( !strResult.IsEmpty() )
	{
		strDebug += strResult;
		strDebug += L"\r\n";
	}
	return TRUE;

}

void CPakageFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPakageFileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPakageFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPakageFileDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_Thread.ThreadExecute.bind(this,&CPakageFileDlg::PackDirThread);
	m_Thread.Start();
}

void CPakageFileDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CPakageFileDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	UINT count;
	TCHAR filePath[MAX_PATH];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if(count)
	{
		for( UINT i = 0; i<count; ++i )
		{
			DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
			if( PathFileExists(filePath) && PathIsDirectory(filePath) )
			{

                //���Ŀ¼�Ƿ��Ѿ�����
                BOOL bExist = FALSE;
                std::vector<std::wstring>::iterator it = m_FileList.begin();
                for(;it != m_FileList.end(); ++it )
                {
                    std::wstring strDir = *it;
                    if( strDir == filePath )
                    {
                        bExist = TRUE;
                        break;
                    }
                }

                if( bExist )
                    continue;

				m_FileList.push_back(filePath);
				int nRow = m_lcList.InsertItem(m_lcList.GetItemCount(),filePath);
				m_lcList.SetItemText(nRow, 1,L"ĩ���");
                m_lcList.SetCheck(nRow,TRUE);
				InvalidateRect(NULL);
			}
		}
	}

	DragFinish(hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}


bool CPakageFileDlg::PackDirThread()
{
	CString strApp = L"./tools/pack_dir.exe";
	std::vector<std::wstring>::iterator it = m_FileList.begin();
	CString strDir;
	int nIndex = 0 ;

	m_editOutput.SetWindowText(L"");

	CString strDebug;
	for(;it != m_FileList.end(); ++it )
	{
		strDir = (*it).c_str();

        if( !m_lcList.GetCheck(nIndex))
            continue;

		m_lcList.SetItemText(nIndex, 1,L"���ڴ��");
		if( PackDir(strApp,strDir,strDebug) )
		{
			m_lcList.SetItemText(nIndex, 1,L"����ɹ�");
		}
		else
		{
			m_lcList.SetItemText(nIndex, 1,L"���ʧ��");
		}

		m_editOutput.SetWindowText(strDebug);

		int nLength=m_editOutput.SendMessage(WM_GETTEXTLENGTH);   
		m_editOutput.SetSel(nLength,   nLength);         //������������   
		++nIndex;

	}

	return false;
}
void CPakageFileDlg::OnBnClickedButtonSelectA()
{
    // TODO: Add your control notification handler code here

    std::vector<std::wstring>::iterator it = m_FileList.begin();
    int nIndex = 0 ;
    CString strDebug;
    for(;it != m_FileList.end(); ++it )
    {
        m_lcList.SetCheck(nIndex,TRUE);
        ++nIndex;
    }

    InvalidateRect(NULL);
}

void CPakageFileDlg::OnBnClickedButtonSelectR()
{
    // TODO: Add your control notification handler code here

    std::vector<std::wstring>::iterator it = m_FileList.begin();
    int nIndex = 0 ;
    CString strDebug;
    for(;it != m_FileList.end(); ++it )
    {
        m_lcList.SetCheck(nIndex,!m_lcList.GetCheck(nIndex));
        ++nIndex;
    }

    InvalidateRect(NULL);
}

void CPakageFileDlg::OnBnClickedButtonCleanup()
{
    // TODO: Add your control notification handler code here
    m_FileList.clear();
    m_lcList.DeleteAllItems();

    m_editOutput.SetWindowText(L"");
    int nLength=m_editOutput.SendMessage(WM_GETTEXTLENGTH);   
    m_editOutput.SetSel(nLength,   nLength);         //������������   

    InvalidateRect(NULL);
}
