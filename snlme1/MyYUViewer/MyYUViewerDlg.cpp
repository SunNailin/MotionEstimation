// MyYUViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyYUViewer.h"
#include "ChildWindow.h"
#include "MyYUViewerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BOOL g_bPlay;
int g_nFrameNumber = 0;
int g_nOldFrameNumber = -1000; 
BOOL g_Play = true;

int g_nStartFrame = 0;
int g_nEndFrame = 10000;
int g_nCurrentFrame = 0; 
BOOL g_bReversePlay = FALSE;

UINT PlayVideo( LPVOID pParam )
{
	int i;
	BOOL bPlay = g_bPlay;
	BOOL bEof = FALSE;
	CMyYUViewerDlg *pWin = (CMyYUViewerDlg *)pParam;
	UINT picsize = pWin->m_nWidth*pWin->m_nHeight;
	int timespan = 1000/atoi(pWin->m_sFrameRate);	
	if(g_nCurrentFrame < g_nStartFrame) g_nCurrentFrame = g_nStartFrame;
	if(g_nCurrentFrame > g_nEndFrame) g_nCurrentFrame = g_nEndFrame;

	for(i=0; i<pWin->m_iCount; i++)
	{
		pWin->m_pFile[i]->Seek(g_nCurrentFrame*picsize*3/2, SEEK_SET);
		pWin->m_pWnd[i]->nPicShowOrder = g_nCurrentFrame;
	}
	
	HANDLE hPlayTemp1 = OpenMutex(MUTEX_ALL_ACCESS,FALSE,"播放");	
	while(g_nCurrentFrame >= g_nStartFrame && g_nCurrentFrame <= g_nEndFrame && !bEof)
	{
		DWORD t2=GetTickCount();
		g_nFrameNumber = g_nCurrentFrame;//j;
 
		if ( WAIT_OBJECT_0 == WaitForSingleObject(hPlayTemp1,INFINITE) )
			ReleaseMutex( hPlayTemp1 );
		
		for(i=0; i<pWin->m_iCount; i++)
		{
			if(!pWin->m_Color)
			pWin->m_pFile[i]->Seek(g_nCurrentFrame*picsize*3/2, SEEK_SET);
			else
			pWin->m_pFile[i]->Seek(g_nCurrentFrame*picsize, SEEK_SET);

			if(picsize != pWin->m_pFile[i]->Read(pWin->m_pWnd[i]->Y,picsize))
			{
				AfxMessageBox("Get to end of file");
				bEof = TRUE;
				break;
			}
			if(!pWin->m_Color) 
			{
				if(picsize/4 != pWin->m_pFile[i]->Read(pWin->m_pWnd[i]->Cb,picsize/4))
				{
					AfxMessageBox("Get to end of file");
					bEof = TRUE;
					break;
				}
				if(picsize/4 != pWin->m_pFile[i]->Read(pWin->m_pWnd[i]->Cr,picsize/4))
				{
					AfxMessageBox("Get to end of file");
					bEof = TRUE;
					break;
				}
			}
			else
			{
		        memset(pWin->m_pWnd[i]->Cb,128,picsize/4);
		        memset(pWin->m_pWnd[i]->Cr,128,picsize/4);
			}

			pWin->m_pWnd[i]->InvalidateRect (NULL,FALSE);
			pWin->m_pWnd[i]->UpdateWindow ();
			pWin->m_pWnd[i]->nPicShowOrder=g_nCurrentFrame;
		}

		if(g_bReversePlay == FALSE)//反播
			g_nCurrentFrame++;
		else 
			g_nCurrentFrame--;

		int t1=GetTickCount()-t2;
		if(t1 < timespan) 
			Sleep(timespan - t1); // sleep time in milliseconds
	}
	
	pWin->m_pWinThread = NULL;
	AfxEndThread(0);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyYUViewerDlg dialog

CMyYUViewerDlg::CMyYUViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyYUViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyYUViewerDlg)
	m_nZoom = FALSE;
	m_size = 0;
	m_sFrameRate = _T("30");
	m_nHeight=288;
	m_nWidth=352;
	m_Color = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyYUViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyYUViewerDlg)
	DDX_Control(pDX, ID_PAUSEPLAY, m_buttonPausePlay);
	DDX_Control(pDX, IDC_Zoom, m_Zoom);
	DDX_Check(pDX, IDC_Zoom, m_nZoom);
	DDX_Radio(pDX, IDC_CIF, m_size);
	DDX_CBString(pDX, IDC_COMBO1, m_sFrameRate);
	DDX_Check(pDX, IDC_COLOR, m_Color);
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CMyYUViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CMyYUViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_FILE_OPEN, OnFileOpen)
	ON_BN_CLICKED(IDC_CIF, OnCif)
	ON_BN_CLICKED(IDC_QCIF, OnQcif)
	ON_BN_CLICKED(IDC_Zoom, OnZoom)
	ON_BN_CLICKED(ID_PAUSEPLAY, OnPauseplay)
	ON_BN_CLICKED(ID_CLOSEALL, OnCloseall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyYUViewerDlg message handlers

BOOL CMyYUViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Zoom.SetCheck(0);
	Disable(IDC_Zoom);
	m_iCount=0;
	m_bPlay = true;
	m_pWinThread = NULL;

	HANDLE hPlay = NULL;
	if( (hPlay=OpenMutex(MUTEX_ALL_ACCESS,FALSE,"播放"))==NULL)
	{
		//如果没有其他进程创建这个互斥量，则重新创建
		hPlay = CreateMutex(NULL,FALSE,"播放");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyYUViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyYUViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyYUViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void getSeqName(char *inseqpath, char *seqname)
{
  int lastSlashPos, lastDotPos; // the last dot is located after the last slash "\"
  int lastNonZeroPos; // last pos that tmp != 0
  int i=0;
  char tmp = '0';

  while(tmp != 0)
  {
    tmp = inseqpath[i++];
    if(tmp == '\\')
      lastSlashPos = i-1;
    if(tmp == '.')
      lastDotPos = i-1;
  }
  lastNonZeroPos = i-1;

  if(lastDotPos < lastSlashPos)
    lastDotPos = -1; // that means the file name with no extention, such as "c:\seq\forman".

  if(lastDotPos != -1)
  {
    for(i=lastSlashPos+1; i<lastDotPos; i++)
      seqname[i-lastSlashPos-1] = inseqpath[i];
    seqname[lastDotPos-lastSlashPos-1] = 0;
  }
  else
  {
    for(i=lastSlashPos+1; i<lastNonZeroPos+1; i++)
      seqname[i-lastSlashPos-1] = inseqpath[i];
    seqname[lastNonZeroPos-lastSlashPos] = 0;
  }
}



void CMyYUViewerDlg::OnFileOpen() 
{
	UpdateData(TRUE);
	UINT picsize = m_nWidth*m_nHeight;
	m_pFile[m_iCount] = new CFile();
	char BASED_CODE szFilter[] = "YUV Files (*.yuv)|*.yuv||";
	CFileDialog dlg( TRUE, "yuv", NULL, OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir="D:dinggg\\book";
  	if(dlg.DoModal()!=IDOK) return; 
    sprintf( inSeqence[m_iCount], "%s", dlg.GetPathName() );
    getSeqName(inSeqence[m_iCount], inSeqName[m_iCount]);
	if(m_pFile[m_iCount]->Open(inSeqence[m_iCount], CFile::modeRead)==0) 
	{
		AfxMessageBox("Can't open input file");
		return;
	}
	m_pWnd[m_iCount]=new CChildWindow((CFrameWnd*)this, m_nWidth, m_nHeight,1);
	if(picsize != m_pFile[m_iCount]->Read(m_pWnd[m_iCount]->Y,picsize))
	{
		MessageBox("Get to end of file");
		return;
	}
	if(!m_Color) 
	{
		if(picsize/4 != m_pFile[m_iCount]->Read(m_pWnd[m_iCount]->Cb,picsize/4))
		{
			MessageBox("Get to end of file");
			return;
		}
		if(picsize/4 != m_pFile[m_iCount]->Read(m_pWnd[m_iCount]->Cr,picsize/4))
		{
			MessageBox("Get to end of file");
			return;
		}
	}
	else
	{
		memset(m_pWnd[m_iCount]->Cb,128,picsize/4);
		memset(m_pWnd[m_iCount]->Cr,128,picsize/4);
	}
	m_pWnd[m_iCount]->ShowWindow(SW_SHOW);
	if(m_nZoom == 0) m_pWnd[m_iCount]->CenterWindow(m_nWidth,m_nHeight);
	else if(m_nZoom == 1) m_pWnd[m_iCount]->CenterWindow(m_nWidth*2,m_nHeight*2);
	m_iCount++;//*/
	return;
}

void CMyYUViewerDlg::OnCif() 
{
	UpdateData(TRUE);
	m_nWidth = 352;
	m_nHeight = 288;
	Disable(IDC_Zoom);
	UpdateData(FALSE);	
}

void CMyYUViewerDlg::OnQcif() 
{
	UpdateData(TRUE);
	m_nWidth = 176;
	m_nHeight = 144;
	Enable(IDC_Zoom);
	UpdateData(FALSE);	

}

void CMyYUViewerDlg::Enable(int nID)
{
	CWnd *pObject1;
	pObject1 = GetDlgItem(nID);
	pObject1->EnableWindow(TRUE);
}

void CMyYUViewerDlg::Disable(int nID)
{
	CWnd *pObject1;
	pObject1 = GetDlgItem(nID);
	pObject1->EnableWindow(FALSE);
}

void CMyYUViewerDlg::OnCancel() 
{
	int i;

	for(i=0; i<m_iCount; i++)
	{
		if(m_pFile[i])
			m_pFile[i]->Close();
		if(m_pWnd[i])
			m_pWnd[i]->DestroyWindow();
	}	
	CDialog::OnCancel();
}



void CMyYUViewerDlg::OnZoom() 
{
	if(m_nZoom)
	{	m_Zoom.SetWindowText("100%");
	    m_nZoom=0;
	}
	else
	{	m_Zoom.SetWindowText("200%");	
	    m_nZoom=1;
	}
	UpdateData(false);	
}

void CMyYUViewerDlg::OnPauseplay() 
{

	UpdateData(TRUE);

/*	g_nStartFrame = m_nFrameFrom;
	if(m_nFrameTo != 0) g_nEndFrame = m_nFrameTo;
	else g_nEndFrame = 10000;*/
	g_nStartFrame = 0;
	g_nEndFrame = 10000;

	// create a new thread
	if (m_bPlay)
	{
		m_buttonPausePlay.SetWindowText("暂停");
		m_bPlay = false;
		g_Play = true;
	}
	else
	{
		m_buttonPausePlay.SetWindowText("播放");
		m_bPlay = true;
	}

	char chTitle[10];
	m_buttonPausePlay.GetWindowText(chTitle,10);
	hPlayTemp = NULL;
	hPlayTemp=OpenMutex(MUTEX_ALL_ACCESS,FALSE,"播放");
	if ( strcmp( chTitle,"播放" ) == 0 )
	{
		WaitForSingleObject( hPlayTemp,0);
		
	}
	else
		ReleaseMutex(hPlayTemp);

	if ( m_pWinThread == NULL)
		m_pWinThread = AfxBeginThread( (AFX_THREADPROC)PlayVideo , (void*)this);	
}

void CMyYUViewerDlg::OnCloseall() 
{
	int i;

	for(i=0; i<m_iCount; i++)
	{
		if(m_pFile[i])
			m_pFile[i]->Close();
		if(m_pWnd[i])
			m_pWnd[i]->DestroyWindow();
	}	
	m_iCount=0;
	m_bPlay = true;
	g_nCurrentFrame = 0; 
}
