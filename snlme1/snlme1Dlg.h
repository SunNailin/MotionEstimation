
// snlme1Dlg.h : ͷ�ļ�
//

#pragma once
#include "CWMPPlayer4.h"
#include "MeDlg.h"

// Csnlme1Dlg �Ի���
class Csnlme1Dlg : public CDialogEx
{
// ����
public:
	Csnlme1Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SNLME1_DIALOG };

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
	// ��Ƶ�ļ���·��
	CString m_vpath;
	afx_msg void OnBnClickedMe();
	afx_msg void OnBnClickedSpath();
	int m_vfps;
	int m_vlength;
	int m_vheight;
	int m_vfs;
	int m_vwidth;
	afx_msg void OnEnChangeVpath();
	CWMPPlayer4 m_Player;
	afx_msg void OnBnClickedFenzhen();
	afx_msg void OnBnClickedChongzu();
	afx_msg void OnBnClickedCanny();
	afx_msg void OnBnClickedgray();
	afx_msg void OnFileCloseMenu();
	afx_msg void OnMeMenu();
	afx_msg void OnFenzhenMenu();
	afx_msg void On32780();
	afx_msg void OnBianyuanMenu();
	afx_msg void OnHuiduMenu();
	afx_msg void OnBnClickedYanshi();
	afx_msg void OnClickedCanny();
	afx_msg void OnBnClickedYanshi2();
	afx_msg void OnBnClickedPlayyuv();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnMEonMENU();
	afx_msg void OnYTAonMENU();
	afx_msg void OnATYonMENU();
	afx_msg void OnPlayYUVonMENU();
	afx_msg void On32794();
	afx_msg void On32795();
	afx_msg void On32796();
};
