#pragma once


// CATYDlg 对话框

class CATYDlg : public CDialog
{
	DECLARE_DYNAMIC(CATYDlg)

public:
	CATYDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CATYDlg();

// 对话框数据
	enum { IDD = IDD_ATYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// yuv序列的路径
//	CString m_yuvpath;
	afx_msg void OnBnClickedButton1();
	CString m_avipath;
};
