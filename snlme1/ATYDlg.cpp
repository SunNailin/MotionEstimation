// ATYDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "snlme1.h"
#include "ATYDlg.h"
#include "afxdialogex.h"



// CATYDlg �Ի���

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


// CATYDlg ��Ϣ�������


void CATYDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("avi��ʽ�ļ�(*.avi)|*.avi|�����ļ�(*.*)|*.*||");   
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        strFilePath = fileDlg.GetPathName();
		m_avipath=strFilePath;
	}
		UpdateData(false);
}
