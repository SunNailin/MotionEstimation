
// snlme1Dlg.cpp : 实现文件
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
//运动分析演示部分
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
//视频基础处理函数声明及定义部分

//视频分帧
void shipinfenzhen(CString VideoPath)
{
	CvCapture* capture = cvCaptureFromAVI((CW2A)VideoPath);
if(!capture){MessageBox(NULL,_T("加载视频文件时出现问题") , _T("视频分帧失败") , MB_OK | MB_ICONWARNING);}
else{

//获取视频信息
cvQueryFrame(capture);
int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
int numFrames = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);


//定义和初始化变量
int i = 0;
IplImage* img = 0;
char image_name[13];

cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );

//读取和显示
while(1)
{
  
   img = cvQueryFrame(capture); //获取一帧图片
   cvShowImage( "mainWin", img ); //将其显示
   char key = cvWaitKey(20);
  
   sprintf(image_name, "%s%d%s", "image", ++i, ".bmp");//保存的图片名
  
   cvSaveImage( image_name, img);   //保存一帧图片


   if(i == NUM_FRAME) break;
}

cvReleaseCapture(&capture);
cvDestroyWindow("mainWin");
}

}
//分帧重组
void fenzhenchongzu(CString VidepPath)
{
	Ffmpeg_Encoder ffmpegobj;
	ffmpegobj.Ffmpeg_Encoder_Init();//初始化编码器
	ffmpegobj.Ffmpeg_Encoder_Setpara(CODEC_ID_H264,800,600);//设置编码器参数

	//图象编码
	FILE *f = NULL;
	char * filename = "myData.h264";
	fopen_s(&f, filename, "wb");//打开文件存储编码完成数据

	IplImage* img = NULL;//OpenCV图像数据结构指针  
	IplImage* resizeimg = NULL;//尺寸
	int picturecount = 1;
	while (picturecount != 80)
	{
		/**此部分用的是OpenCV读入图像对象并取得图像的数据区，也可以用别的方法获得图像数据区**/
		char chpicname[100];
		sprintf(chpicname, "%s%d%s", "image", picturecount, ".bmp");//获得图片路径
		//sprintf(chpicname, "1.jpg", picturecount);//获得图片路径
		img = cvLoadImage(chpicname, 1);//打开图像
		//由于OpenCV图像数据区是以BGR排列的，所以要将其数据转换为正常的RGB排列才能做进一步的压缩，不然压出来的视频颜色会不正确
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
		cvResize(img, resizeimg, CV_INTER_LINEAR);//调整图像大小
		/**此部分用的是OpenCV读入图像对象并取得图像的数据区，也可以用别的方法获得图像数据区**/

		ffmpegobj.Ffmpeg_Encoder_Encode(f, (uchar*)resizeimg->imageData);//编码

		cvReleaseImage(&img);//释放图像数据结构指针对像所指内容 
		cvReleaseImage(&resizeimg);
		picturecount++;
	}
	fclose(f);
	ffmpegobj.Ffmpeg_Encoder_Close();

	MessageBox(NULL,_T("分帧重组成功 ！") , _T("分帧重组成功，重组文件为根目录下myData.h264") , MB_OK | MB_ICONWARNING);
	/*
	int i = 0;
IplImage* img = 0;
char image_name[13];


//初始化视频编写器
CvVideoWriter *writer = 0;
int isColor = 1;
int fps     = 20; // 该处的设置为视频的帧数
int frameW = 400; // 写出的视频的宽度
int frameH = 240; // 写出的视频的高度
writer=cvCreateVideoWriter("out.avi",CV_FOURCC('X','V','I','D'),fps,cvSize(frameW,frameH),isColor);


//创建窗口


while(i<NUM_FRAME)
{
   sprintf(image_name, "%s%d%s", "image", ++i, ".bmp");	//设置图像文件名
   img = cvLoadImage(image_name);	//读取图像文件
   if(!img)
   {
    MessageBox(NULL,_T("加载图片帧文件时出现问题") , _T("分帧重组失败") , MB_OK | MB_ICONWARNING );
    break;
   }
   cvNamedWindow( "mainWin", CV_WINDOW_AUTOSIZE );	
   cvShowImage("mainWin", img);
   char key = cvWaitKey(20);
   cvWriteFrame(writer, img);	//写入视频文件中
}

cvReleaseVideoWriter(&writer);
cvDestroyWindow("mainWin");*/
}
//灰度处理
void huiduchuli(CString VideoPath)
{
	CvCapture *capture = cvCreateFileCapture((CW2A)VideoPath);
	if(!capture){MessageBox( NULL,_T("加载视频文件时出现问题") , _T("灰度处理失败") , MB_OK | MB_ICONWARNING ) ;}
	else{
	cvNamedWindow("灰度处理", CV_WINDOW_AUTOSIZE);
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

		cvShowImage("灰度处理", frame_gray);

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
//边缘提取
void bianyuantiqu(CString VideoPath)
{
	CvCapture *capture = cvCreateFileCapture((CW2A)VideoPath);
	if(!capture)
	{MessageBox( NULL,_T("加载视频文件时出现问题") , _T("边缘提取失败") , MB_OK | MB_ICONWARNING ) ; }
	else{
	cvNamedWindow("边缘提取", CV_WINDOW_AUTOSIZE);
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

		cvShowImage("边缘提取", frame_canny);

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





// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg(); 

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// Csnlme1Dlg 对话框




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
	DDX_Control(pDX, IDC_OCX1, m_Player);//绑定播放器控件 
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


// Csnlme1Dlg 消息处理程序

BOOL Csnlme1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Csnlme1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Csnlme1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Csnlme1Dlg::OnBnClickedMe()
{
	// TODO: 在此添加控件通知处理程序代码
	CMeDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedSpath()
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
		m_vpath=strFilePath;
		CvCapture* capture =cvCaptureFromAVI((CW2A)m_vpath);
		if(!capture) MessageBox(_T("无法读取该文件，请重新选择"));
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	//UpdateData(true);
	// TODO:  在此添加控件通知处理程序代码
}


void Csnlme1Dlg::OnBnClickedFenzhen()			//点击视频分帧按钮，调用shipinfenzhen()函数进行视频分帧
{
	// TODO: 在此添加控件通知处理程序代码
	shipinfenzhen(m_vpath);
	MessageBox(_T("视频分帧成功！请打开程序根目录查看！"));

}



void Csnlme1Dlg::OnBnClickedChongzu()			//点击分帧重组按钮，调用fenzhenchongzu()函数进行分帧重组
{
	// TODO: 在此添加控件通知处理程序代码
	fenzhenchongzu(m_vpath);
}


void Csnlme1Dlg::OnBnClickedCanny()  //点击边缘提取按钮，调用bianyuantiqu()函数进行边缘提取
{
	// TODO: 在此添加控件通知处理程序代码
	
}

void Csnlme1Dlg::OnClickedCanny()
{
	// TODO: 在此添加控件通知处理程序代码
	bianyuantiqu(m_vpath);	
}



void Csnlme1Dlg::OnBnClickedgray()		//点击灰度处理按钮，调用huiduchuli()函数进行灰度处理
{
	// TODO: 在此添加控件通知处理程序代码
	huiduchuli(m_vpath);	
}


void Csnlme1Dlg::OnFileCloseMenu()
{
	// TODO: 在此添加命令处理程序代码
	exit(0);
}


void Csnlme1Dlg::OnMeMenu()
{
	// TODO: 在此添加命令处理程序代码
	CMeDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnFenzhenMenu()
{
	// TODO: 在此添加命令处理程序代码
	shipinfenzhen(m_vpath);
}


void Csnlme1Dlg::On32780()
{
	// TODO: 在此添加命令处理程序代码
	fenzhenchongzu(m_vpath);
}


void Csnlme1Dlg::OnBianyuanMenu()
{
	// TODO: 在此添加命令处理程序代码
	bianyuantiqu(m_vpath);
}


void Csnlme1Dlg::OnHuiduMenu()
{
	// TODO: 在此添加命令处理程序代码
	huiduchuli(m_vpath);
}



void Csnlme1Dlg::OnBnClickedYanshi()
{
	// TODO: 在此添加控件通知处理程序代码
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

		 cvNamedWindow("基于光流的运动分析", 0);



		 while(current_frame<frameNUM-1)
		 {
			 cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, current_frame );
			 frame = cvQueryFrame( capture );   //获取当前祯

			 allocateOnDemand( &frame1_1C, image_size, IPL_DEPTH_8U, 1 );
			 cvConvertImage(frame, frame1_1C, CV_CVTIMG_FLIP);

			 allocateOnDemand( &frame1, image_size, IPL_DEPTH_8U, 3 );
	         cvConvertImage(frame, frame1, CV_CVTIMG_FLIP);

			 frame = cvQueryFrame( capture );   //获取下一祯

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
/*********绘制速度与方向*********/
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
/*********绘制箭头长度*********/
				 double angle;	
				 angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
			     double hypotenuse;	
				 hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );

				 q.x = (int) (p.x - 4 * hypotenuse * cos(angle));
			     q.y = (int) (p.y - 4 * hypotenuse * sin(angle));
				 cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
/*********绘制箭头*********/
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
			 cvShowImage("基于光流的运动分析", frame1);

			 cvWaitKey(50);

		 }
		 cvReleaseCapture(&capture);
}




void Csnlme1Dlg::OnBnClickedYanshi2()
{
	// TODO: 在此添加控件通知处理程序代码
//	CYUVPlayer dlg; 
//	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedPlayyuv()
{
	// TODO: 在此添加控件通知处理程序代码
//		CMyYUVViewerDlg dlg; 
//		dlg.DoModal();
	ShellExecuteA(NULL, "open", "res\\MyYUViewer.exe", NULL, NULL, SW_SHOW);
}


void Csnlme1Dlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CYTADlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CATYDlg dlg; 
	dlg.DoModal();
}


void Csnlme1Dlg::OnMEonMENU()
{
	// TODO: 在此添加命令处理程序代码
	OnMeMenu();
}


void Csnlme1Dlg::OnYTAonMENU()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButton3();
}


void Csnlme1Dlg::OnATYonMENU()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedButton1();
}


void Csnlme1Dlg::OnPlayYUVonMENU()
{
	// TODO: 在此添加命令处理程序代码
	ShellExecuteA(NULL, "open", "res\\MyYUViewer.exe", NULL, NULL, SW_SHOW);
}


void Csnlme1Dlg::On32794()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedYanshi();
}


void Csnlme1Dlg::On32795()
{
	// TODO: 在此添加命令处理程序代码
	OnFenzhenMenu();
}


void Csnlme1Dlg::On32796()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedChongzu();

}
