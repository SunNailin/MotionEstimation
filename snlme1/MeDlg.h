#pragma once
#include "afxwin.h"
#include "string.h"
#include "stdafx.h"
#include "snlme1.h"


#include "afxdialogex.h"


// CMeDlg �Ի���

class CMeDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeDlg)

public:
	CMeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMeDlg();

//	
// �Ի�������
	enum { IDD = IDD_MEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	//virtual BOOL OnInitDialog();
		//String FSIntro="����ͼ�����X������Y����Ŀ������λ��ΪXmax��Ymax����ô����ͼ���AΪ���ĵģ�M+2*Xmax��N+2*Ymax���ķ�Χ�ڣ���ÿһ�����ܵ�ͼ��鶼����ƥ��ƣ�Ѱ��һ�����ŵ�ƥ��顣ȫ�����㷨��򵥣������㷨��ʱ�Ƚ϶࣬��������";
		//CString str1(_T("����ͼ�����X������Y����Ŀ������λ��ΪXmax��Ymax����ô����ͼ���AΪ���ĵģ�M+2*Xmax��N+2*Ymax���ķ�Χ�ڣ���ÿһ�����ܵ�ͼ��鶼����ƥ��ƣ�Ѱ��һ�����ŵ�ƥ��顣ȫ�����㷨��򵥣������㷨��ʱ�Ƚ϶࣬��������"));
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeCombo1();
	// �ø�combo box�ؼ�ѡ���˶������㷨
	CComboBox m_SelAl;
	// �㷨�����˵��
	CString m_AlIntro;
	afx_msg void OnBnClickedButton1();
	// ���ַ������ڴ洢���ܷ�����������ݲ��������
//	CString m_Analysis;
	// ���ڴ洢֡ƽ�������������������
	float m_Cost;
	// ���ڴ洢ƽ����ֵ����Ȳ��������
	float m_PSNR;
//	CString m_Analysis;
	CListBox m_Analysis;
	// ���е�ַ
	CString m_spath;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();
	// �ο����е�ַ
	CString m_spath_ref;
};
