// ATYDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "snlme1.h"
#include "ATYDlg.h"
#include "afxdialogex.h"



// CATYDlg 对话框

IMPLEMENT_DYNAMIC(CATYDlg, CDialog)

CATYDlg::CATYDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CATYDlg::IDD, pParent)
	
{

	m_avipath = _T("");
}

CATYDlg::~CATYDlg()
{
}

void CATYDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT1, m_yuvpath);
	DDX_Text(pDX, IDC_EDIT1, m_avipath);
}


BEGIN_MESSAGE_MAP(CATYDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CATYDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CATYDlg 消息处理程序


void CATYDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("avi格式文件(*.avi)|*.avi|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName();
		m_avipath=strFilePath;
	}
		UpdateData(false);
}
