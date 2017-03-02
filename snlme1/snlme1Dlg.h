
// snlme1Dlg.h : 头文件
//

#pragma once
#include "CWMPPlayer4.h"
#include "MeDlg.h"

// Csnlme1Dlg 对话框
class Csnlme1Dlg : public CDialogEx
{
// 构造
public:
	Csnlme1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SNLME1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 视频文件的路径
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
