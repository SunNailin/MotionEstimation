#pragma once


// CATYDlg �Ի���

class CATYDlg : public CDialog
{
	DECLARE_DYNAMIC(CATYDlg)

public:
	CATYDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CATYDlg();

// �Ի�������
	enum { IDD = IDD_ATYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// yuv���е�·��
//	CString m_yuvpath;
	afx_msg void OnBnClickedButton1();
	CString m_avipath;
};
