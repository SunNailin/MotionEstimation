
// snlme1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "snlme1.h"
#include "snlme1Dlg.h"
#include "afxdialogex.h"
#include "YTADlg.h"
#include "ATYDlg.h"
//#include "YUVPlayer.h"
//#include "MyYUVViewerDlg.h"
#include "Ffmpeg_Encoder.h"
#include<cv.h>
#include "highgui.h"

#include <math.h>
#include <cv.h>
#include <highgui.h>
#include "ocx1.h"
#include "math.h"
#define NUM_FRAME 100

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//�˶�������ʾ����
static const double pi = 3.14159265358979323846;

inline static double square(int a)
{
	return a * a;
}

inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	return;

	*img = cvCreateImage( size, depth, channels );

}
//��Ƶ�������������������岿��

//��Ƶ��֡
void shipinfenzhen(CString VideoPath)
{
	CvCapture* capture = cvCaptureFromAVI((CW2A)VideoPath);
if(!capture){MessageBox(NULL,_T("������Ƶ�ļ�ʱ��������") , _T("��Ƶ��֡ʧ��") , MB_OK | MB_ICONWARNING);}
else{

//��ȡ��Ƶ��Ϣ
cvQueryFrame(capture);
int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
int numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);


//����ͳ�ʼ������
int i = 0;
IplImage* img = 0;
char image_name[13];

cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );

//��ȡ����ʾ
while(1)
{
  
   img = cvQueryFrame(capture); //��ȡһ֡ͼƬ
   cvShowImage( "mainWin", img ); //������ʾ
   char key = cvWaitKey(20);
  
   sprintf(image_name, "%s%d%s", "image", ++i, ".bmp");//�����ͼƬ��
  
   cvSaveImage( image_name, img);   //����һ֡ͼƬ


   if(i == NUM_FRAME) break;
}

cvReleaseCapture(&capture);
cvDestroyWindow("mainWin");
}

}
//��֡����
void fenzhenchongzu(CString VidepPath)
{
	Ffmpeg_Encoder ffmpegobj;
	ffmpegobj.Ffmpeg_Encoder_Init();//��ʼ��������
	ffmpegobj.Ffmpeg_Encoder_Setpara(CODEC_ID_H264,800,600);//���ñ���������

	//ͼ�����
	FILE *f = NULL;
	char * filename = "myData.h264";
	fopen_s(&f, filename, "wb");//���ļ��洢�����������

	IplImage* img = NULL;//OpenCVͼ�����ݽṹָ��  
	IplImage* resizeimg = NULL;//�ߴ�
	int picturecount = 1;
	while (picturecount != 80)
	{
		/**�˲����õ���OpenCV����ͼ�����ȡ��ͼ�����������Ҳ�����ñ�ķ������ͼ��������**/
		char chpicname[100];
		sprintf(chpicname, "%s%d%s", "image", picturecount, ".bmp");//���ͼƬ·��
		//sprintf(chpicname, "1.jpg", picturecount);//���ͼƬ·��
		img = cvLoadImage(chpicname, 1);//��ͼ��
		//����OpenCVͼ������������BGR���еģ�����Ҫ��������ת��Ϊ������RGB���в�������һ����ѹ������Ȼѹ��������Ƶ��ɫ�᲻��ȷ
		uchar* data = (uchar*)(img->imageData);
		uchar mid = 0;
		for (int row = 0; row<img->height; row++)
		for (int cols = 0; cols < img->width; cols++)
		{
			mid = data[row*img->widthStep / sizeof(uchar)+cols*img->nChannels + 0];//G   
			data[row*img->widthStep / sizeof(uchar)+cols*img->nChannels + 0] = data[row*img->widthStep / sizeof(uchar)+cols*img->nChannels + 2];
			data[row*img->widthStep / sizeof(uchar)+cols*img->nChannels + 2] = mid;
		}
		resizeimg = cvCreateImage(cvSize(800, 600), 8, 3);
		cvResize(img, resizeimg, CV_INTER_LINEAR);//����ͼ���С
		/**�˲����õ���OpenCV����ͼ�����ȡ��ͼ�����������Ҳ�����ñ�ķ������ͼ��������**/

		ffmpegobj.Ffmpeg_Encoder_Encode(f, (uchar*)resizeimg->imageData);//����

		cvReleaseImage(&img);//�ͷ�ͼ�����ݽṹָ�������ָ���� 
		cvReleaseImage(&resizeimg);
		picturecount++;
	}
	fclose(f);
	ffmpegobj.Ffmpeg_Encoder_Close();

	MessageBox(NULL,_T("��֡����ɹ� ��") , _T("��֡����ɹ��������ļ�Ϊ��Ŀ¼��myData.h264") , MB_OK | MB_ICONWARNING);
	/*
	int i = 0;
IplImage* img = 0;
char image_name[13];


//��ʼ����Ƶ��д��
CvVideoWriter *writer = 0;
int isColor = 1;
int fps     = 20; // �ô�������Ϊ��Ƶ��֡��
int frameW = 400; // д������Ƶ�Ŀ��
int frameH = 240; // д������Ƶ�ĸ߶�
writer=cvCreateVideoWriter("out.avi",CV_FOURCC('X','V','I','D'),fps,cvSize(frameW,frameH),isColor);


//��������


while(i<NUM_FRAME)
{
   sprintf(image_name, "%s%d%s", "image", ++i, ".bmp");	//����ͼ���ļ���
   img = cvLoadImage(image_name);	//��ȡͼ���ļ�
   if(!img)
   {
    MessageBox(NULL,_T("����ͼƬ֡�ļ�ʱ��������") , _T("��֡����ʧ��") , MB_OK | MB_ICONWARNING );
    break;
   }
   cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );	
   cvShowImage("mainWin", img);
   char key = cvWaitKey(20);
   cvWriteFrame(writer, img);	//д����Ƶ�ļ���
}

cvReleaseVideoWriter(&writer);
cvDestroyWindow("mainWin");*/
}
//�Ҷȴ���
void huiduchuli(CString VideoPath)
{
	CvCapture *capture = cvCreateFileCapture((CW2A)VideoPath);
	if(!capture){MessageBox( NULL,_T("������Ƶ�ļ�ʱ��������") , _T("�Ҷȴ���ʧ��") , MB_OK | MB_ICONWARNING ) ;}
	else{
	cvNamedWindow("�Ҷȴ���", CV_WINDOW_AUTOSIZE);
	IplImage *frame;
	IplImage *frame_gray;

	frame = cvQueryFrame(capture);
	frame_gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

	while(1)
	{
		if(!frame)
		{
			break;
		}		
		
		cvConvertImage(frame, frame_gray, 0);		

		cvShowImage("�Ҷȴ���", frame_gray);

		char c = cvWaitKey(33);
		if (c == 27)
		{
			break;
		}
		frame = cvQueryFrame(capture);
	}

	cvReleaseCapture(&capture);
	
	cvDestroyAllWindows();
	}
}
//��Ե��ȡ
void bianyuantiqu(CString VideoPath)
{
	CvCapture *capture = cvCreateFileCapture((CW2A)VideoPath);
	if(!capture)
	{MessageBox( NULL,_T("������Ƶ�ļ�ʱ��������") , _T("��Ե��ȡʧ��") , MB_OK | MB_ICONWARNING ) ; }
	else{
	cvNamedWindow("��Ե��ȡ", CV_WINDOW_AUTOSIZE);
	IplImage *frame;
	IplImage *frame_gray;
	IplImage *frame_canny;

	frame = cvQueryFrame(capture);
	frame_gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	frame_canny = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

	while(1)
	{
		if(!frame)
		{
			break;
		}		
		
		cvConvertImage(frame, frame_gray, 0);		
		cvCanny(frame_gray, frame_canny, 10, 100, 3);

		cvShowImage("��Ե��ȡ", frame_canny);

		char c = cvWaitKey(33);
		if (c == 27)
		{
			break;
		}
		frame = cvQueryFrame(capture);
	}

	cvReleaseCapture(&capture);
	
	cvDestroyAllWindows();
	}
}





// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg(); 

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Csnlme1Dlg �Ի���




Csnlme1Dlg::Csnlme1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Csnlme1Dlg::IDD, pParent)
	, m_vpath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_vfps = 0;
	m_vlength = 0;
	m_vheight = 0;
	m_vfs = 0;
	m_vwidth = 0;
}

void Csnlme1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VPATH, m_vpath);
	DDX_Text(pDX, IDC_VFPS, m_vfps);
	DDX_Text(pDX, IDC_VLENGTH, m_vlength);
	DDX_Text(pDX, IDC_VHEIGHT, m_vheight);
	DDX_Text(pDX, IDC_VFS, m_vfs);
	DDX_Text(pDX, IDC_VWIDTH, m_vwidth);
	DDX_Control(pDX, IDC_OCX1, m_Player);//�󶨲������ؼ� 
}

BEGIN_MESSAGE_MAP(Csnlme1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ME, &Csnlme1Dlg::OnBnClickedMe)
	ON_BN_CLICKED(IDC_SPATH, &Csnlme1Dlg::OnBnClickedSpath)
	ON_EN_CHANGE(IDC_VPATH, &Csnlme1Dlg::OnEnChangeVpath)
	ON_BN_CLICKED(IDC_FENZHEN, &Csnlme1Dlg::OnBnClickedFenzhen)
	ON_BN_CLICKED(IDC_CHONGZU, &Csnlme1Dlg::OnBnClickedChongzu)
	ON_BN_CLICKED(IDC_gray, &Csnlme1Dlg::OnBnClickedgray)
	ON_COMMAND(ID_32774, &Csnlme1Dlg::OnFileCloseMenu)
	ON_COMMAND(ID_32776, &Csnlme1Dlg::OnMeMenu)
	ON_COMMAND(ID_32779, &Csnlme1Dlg::OnFenzhenMenu)
	ON_COMMAND(ID_32780, &Csnlme1Dlg::On32780)
	ON_COMMAND(ID_32781, &Csnlme1Dlg::OnBianyuanMenu)
	ON_COMMAND(ID_32782, &Csnlme1Dlg::OnHuiduMenu)
	ON_BN_CLICKED(IDC_YANSHI, &Csnlme1Dlg::OnBnClickedYanshi)
	ON_BN_CLICKED(IDC_CANNY, &Csnlme1Dlg::OnClickedCanny)
//	ON_BN_CLICKED(IDC_YANSHI2, &Csnlme1Dlg::OnBnClickedYanshi2)
	ON_BN_CLICKED(IDC_PlayYUV, &Csnlme1Dlg::OnBnClickedPlayyuv)
	ON_BN_CLICKED(IDC_BUTTON3, &Csnlme1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &Csnlme1Dlg::OnBnClickedButton1)
	ON_COMMAND(ID_32783, &Csnlme1Dlg::OnMEonMENU)
	ON_COMMAND(ID_32791, &Csnlme1Dlg::OnYTAonMENU)
	ON_COMMAND(ID_32792, &Csnlme1Dlg::OnATYonMENU)
	ON_COMMAND(ID_32793, &Csnlme1Dlg::OnPlayYUVonMENU)
	ON_COMMAND(ID_32794, &Csnlme1Dlg::On32794)
	ON_COMMAND(ID_32795, &Csnlme1Dlg::On32795)
	ON_COMMAND(ID_32796, &Csnlme1Dlg::On32796)
END_MESSAGE_MAP()


// Csnlme1Dlg ��Ϣ�������

BOOL Csnlme1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Csnlme1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Csnlme1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Csnlme1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Csnlme1Dlg::OnBnClickedMe()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMeDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedSpath()
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
		m_vpath=strFilePath;
		CvCapture* capture =cvCaptureFromAVI((CW2A)m_vpath);
		if(!capture) MessageBox(_T("�޷���ȡ���ļ���������ѡ��"));
		cvQueryFrame(capture);
		int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
		int numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
		m_vfps = fps;
		m_vfs = numFrames;
		m_vwidth = frameW;
		m_vheight = frameH;
		m_vlength = m_vfs/m_vfps;
		m_Player.put_URL(m_vpath);	
	    UpdateData(false);
    }   
}


void Csnlme1Dlg::OnEnChangeVpath()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	//UpdateData(true);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void Csnlme1Dlg::OnBnClickedFenzhen()			//�����Ƶ��֡��ť������shipinfenzhen()����������Ƶ��֡
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	shipinfenzhen(m_vpath);
	MessageBox(_T("��Ƶ��֡�ɹ�����򿪳����Ŀ¼�鿴��"));

}



void Csnlme1Dlg::OnBnClickedChongzu()			//�����֡���鰴ť������fenzhenchongzu()�������з�֡����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	fenzhenchongzu(m_vpath);
}


void Csnlme1Dlg::OnBnClickedCanny()  //�����Ե��ȡ��ť������bianyuantiqu()�������б�Ե��ȡ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
}

void Csnlme1Dlg::OnClickedCanny()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bianyuantiqu(m_vpath);	
}



void Csnlme1Dlg::OnBnClickedgray()		//����Ҷȴ���ť������huiduchuli()�������лҶȴ���
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	huiduchuli(m_vpath);	
}


void Csnlme1Dlg::OnFileCloseMenu()
{
	// TODO: �ڴ���������������
	exit(0);
}


void Csnlme1Dlg::OnMeMenu()
{
	// TODO: �ڴ���������������
	CMeDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnFenzhenMenu()
{
	// TODO: �ڴ���������������
	shipinfenzhen(m_vpath);
}


void Csnlme1Dlg::On32780()
{
	// TODO: �ڴ���������������
	fenzhenchongzu(m_vpath);
}


void Csnlme1Dlg::OnBianyuanMenu()
{
	// TODO: �ڴ���������������
	bianyuantiqu(m_vpath);
}


void Csnlme1Dlg::OnHuiduMenu()
{
	// TODO: �ڴ���������������
	huiduchuli(m_vpath);
}



void Csnlme1Dlg::OnBnClickedYanshi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CvCapture *capture =0;

IplImage *frame=NULL; 
IplImage *frame1=NULL;
IplImage *frame1_1C=NULL;
IplImage *frame2_1C=NULL; 
IplImage *eig_image=NULL;
IplImage *temp_image=NULL;
IplImage *pyramid1=NULL;
IplImage *pyramid2=NULL;

long frameNUM;
long current_frame=0;
long temp=0;
int flag=0;
CString path;
capture = cvCaptureFromAVI((CW2A)m_vpath);
	 CvSize image_size;
         image_size.height =(int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT );
	     image_size.width =(int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH );


		 cvSetCaptureProperty( capture, CV_CAP_PROP_POS_AVI_RATIO, 1. );
		 frameNUM= (int) cvGetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES );
		 cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, 0. );

		 cvNamedWindow("���ڹ������˶�����", 0);



		 while(current_frame<frameNUM-1)
		 {
			 cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, current_frame );
			 frame = cvQueryFrame( capture );   //��ȡ��ǰ��

			 allocateOnDemand( &frame1_1C, image_size, IPL_DEPTH_8U, 1 );
			 cvConvertImage(frame, frame1_1C, CV_CVTIMG_FLIP);

			 allocateOnDemand( &frame1, image_size, IPL_DEPTH_8U, 3 );
	         cvConvertImage(frame, frame1, CV_CVTIMG_FLIP);

			 frame = cvQueryFrame( capture );   //��ȡ��һ��

		     allocateOnDemand( &frame2_1C, image_size, IPL_DEPTH_8U, 1 );
		     cvConvertImage(frame, frame2_1C, CV_CVTIMG_FLIP);
			 allocateOnDemand( &eig_image, image_size, IPL_DEPTH_32F, 1 );
		     allocateOnDemand( &temp_image, image_size, IPL_DEPTH_32F, 1 );

             CvPoint2D32f feature1[40];
			 int featureNUM;
			 featureNUM = 40;
			 cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image, feature1, &featureNUM, .01, .01, NULL);

             CvPoint2D32f feature2[40];
			 char optical_flow_feature[40];
			 float optical_flow_feature_error[40];
			 CvSize optical_flow_window = cvSize(3,3);
			 CvTermCriteria optical_flow_termination_criteria= cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

			 allocateOnDemand( &pyramid1, image_size, IPL_DEPTH_8U, 1 );
		     allocateOnDemand( &pyramid2, image_size, IPL_DEPTH_8U, 1 );

			 cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, feature1, feature2, featureNUM, optical_flow_window, 5, optical_flow_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );
/*********�����ٶ��뷽��*********/
			 for(int i = 0; i < featureNUM; i++)
			 {

				 if ( optical_flow_feature[i] == 0 )	continue;
				 int line_thickness=1;
				 CvScalar line_color;		
				 line_color = CV_RGB(0,0,255);

				 CvPoint p,q;
			     p.x = (int) feature1[i].x;
			     p.y = (int) feature1[i].y;
			     q.x = (int) feature2[i].x;
			     q.y = (int) feature2[i].y;
/*********���Ƽ�ͷ����*********/
				 double angle;	
				 angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
			     double hypotenuse;	
				 hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );

				 q.x = (int) (p.x - 4 * hypotenuse * cos(angle));
			     q.y = (int) (p.y - 4 * hypotenuse * sin(angle));
				 cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
/*********���Ƽ�ͷ*********/
				 p.x = (int) (q.x + 5 * cos(angle + pi / 4));
			     p.y = (int) (q.y + 5 * sin(angle + pi / 4));
			     cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );

			     p.x = (int) (q.x + 5 * cos(angle - pi / 4));
			     p.y = (int) (q.y + 5 * sin(angle - pi / 4));
			     cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );

			 }
			 current_frame++;

			 if(!frame) 
			 break;
			 cvShowImage("���ڹ������˶�����", frame1);

			 cvWaitKey(50);

		 }
		 cvReleaseCapture(&capture);
}




void Csnlme1Dlg::OnBnClickedYanshi2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CYUVPlayer dlg; 
//	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedPlayyuv()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//		CMyYUVViewerDlg dlg; 
//		dlg.DoModal();
	ShellExecuteA(NULL, "open", "res\\MyYUViewer.exe", NULL, NULL, SW_SHOW);
}


void Csnlme1Dlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CYTADlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CATYDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnMEonMENU()
{
	// TODO: �ڴ���������������
	OnMeMenu();
}


void Csnlme1Dlg::OnYTAonMENU()
{
	// TODO: �ڴ���������������
	OnBnClickedButton3();
}


void Csnlme1Dlg::OnATYonMENU()
{
	// TODO: �ڴ���������������
	OnBnClickedButton1();
}


void Csnlme1Dlg::OnPlayYUVonMENU()
{
	// TODO: �ڴ���������������
	ShellExecuteA(NULL, "open", "res\\MyYUViewer.exe", NULL, NULL, SW_SHOW);
}


void Csnlme1Dlg::On32794()
{
	// TODO: �ڴ���������������
	OnBnClickedYanshi();
}


void Csnlme1Dlg::On32795()
{
	// TODO: �ڴ���������������
	OnFenzhenMenu();
}


void Csnlme1Dlg::On32796()
{
	// TODO: �ڴ���������������
	OnBnClickedChongzu();

}
