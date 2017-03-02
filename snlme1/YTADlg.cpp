// YTADlg.cpp : 实现文件
//

#include "stdafx.h"
#include "snlme1.h"
#include "YTADlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <string.h>
#include "Ffmpeg_Encoder.h"
#include<cv.h>
#include "highgui.h"


// CYTADlg 对话框

IMPLEMENT_DYNAMIC(CYTADlg, CDialog)

CYTADlg::CYTADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYTADlg::IDD, pParent)
	, m_yuvpath(_T(""))
{

}

CYTADlg::~CYTADlg()
{
}

void CYTADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT1, m_yuvpath);
}


BEGIN_MESSAGE_MAP(CYTADlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CYTADlg::OnBnClickedButton1)
//	ON_BN_CLICKED(IDC_BUTTON2, &CYTADlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CYTADlg 消息处理程序


void CYTADlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecuteA(NULL, "open", "JM8.6\\lencod.dsw", NULL, NULL, SW_SHOW);
	OnCancel();
}

