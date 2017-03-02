#pragma once
#include "afxwin.h"
#include "string.h"
#include "stdafx.h"
#include "snlme1.h"


#include "afxdialogex.h"


// CMeDlg 对话框

class CMeDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeDlg)

public:
	CMeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMeDlg();

//	
// 对话框数据
	enum { IDD = IDD_MEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//virtual BOOL OnInitDialog();
		//String FSIntro="假设图像块在X方向与Y方向的可以最大位移为Xmax与Ymax，那么在以图像块A为中心的（M+2*Xmax，N+2*Ymax）的范围内，对每一个可能的图像块都进行匹配计，寻找一个最优的匹配块。全搜索算法最简单，但是算法耗时比较多，计算量大。";
		//CString str1(_T("假设图像块在X方向与Y方向的可以最大位移为Xmax与Ymax，那么在以图像块A为中心的（M+2*Xmax，N+2*Ymax）的范围内，对每一个可能的图像块都进行匹配计，寻找一个最优的匹配块。全搜索算法最简单，但是算法耗时比较多，计算量大。"));
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeCombo1();
	// 用该combo box控件选择运动估计算法
	CComboBox m_SelAl;
	// 算法简介与说明
	CString m_AlIntro;
	afx_msg void OnBnClickedButton1();
	// 该字符串用于存储性能分析的相关内容并进行输出
//	CString m_Analysis;
	// 用于存储帧平均搜索点数并进行输出
	float m_Cost;
	// 用于存储平均峰值信噪比并进行输出
	float m_PSNR;
//	CString m_Analysis;
	CListBox m_Analysis;
	// 序列地址
	CString m_spath;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();
	// 参考序列地址
	CString m_spath_ref;
};
