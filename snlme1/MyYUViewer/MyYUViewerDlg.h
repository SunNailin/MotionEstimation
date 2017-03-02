// MyYUViewerDlg.h : header file
//

#if !defined(AFX_MYYUVIEWERDLG_H__1A2C2338_7A07_400A_8677_AB6DCA1F46EF__INCLUDED_)
#define AFX_MYYUVIEWERDLG_H__1A2C2338_7A07_400A_8677_AB6DCA1F46EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyYUViewerDlg dialog

class CMyYUViewerDlg : public CDialog
{
// Construction
public:
	BOOL m_bPlay;	
	CWinThread* m_pWinThread;
	CFile *m_pFile[36];
	CChildWindow *m_pWnd[36];
	HANDLE hPlayTemp;
	CMyYUViewerDlg(CWnd* pParent = NULL);	// standard constructor
	int m_iCount;  //打开的窗口数
    char inSeqName[36][64]; //文件名
    char inSeqence[36][_MAX_PATH];//文件路径名
	int		m_nHeight;
	int		m_nWidth;

// Dialog Data
	//{{AFX_DATA(CMyYUViewerDlg)
	enum { IDD = IDD_MYYUVIEWER_DIALOG };
	CButton	m_buttonPausePlay;
	CButton	m_Zoom;
	BOOL	m_nZoom;
	int		m_size;
	CString	m_sFrameRate;
	BOOL	m_Color;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyYUViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void Disable(int nID);
	void Enable(int nID);

	// Generated message map functions
	//{{AFX_MSG(CMyYUViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFileOpen();
	afx_msg void OnCif();
	afx_msg void OnQcif();
	virtual void OnCancel();
	afx_msg void OnZoom();
	afx_msg void OnPauseplay();
	afx_msg void OnCloseall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYYUVIEWERDLG_H__1A2C2338_7A07_400A_8677_AB6DCA1F46EF__INCLUDED_)
