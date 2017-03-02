#pragma once


// CYTADlg 对话框

class CYTADlg : public CDialog
{
	DECLARE_DYNAMIC(CYTADlg)

public:
	CYTADlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CYTADlg();

// 对话框数据
	enum { IDD = IDD_YTADIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// yuv序列路径
	CString m_yuvpath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	
};
