#pragma once


// CYTADlg �Ի���

class CYTADlg : public CDialog
{
	DECLARE_DYNAMIC(CYTADlg)

public:
	CYTADlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CYTADlg();

// �Ի�������
	enum { IDD = IDD_YTADIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// yuv����·��
	CString m_yuvpath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	
};
