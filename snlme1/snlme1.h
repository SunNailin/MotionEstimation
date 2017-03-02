
// snlme1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif


//FFmpeg ���
#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif 

#ifdef __cplusplus 
extern "C" {
#endif 
	/*Include ffmpeg header file*/
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 

#include <libavutil/imgutils.h>  
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>

#ifdef __cplusplus 
}
#endif 


#include "resource.h"		// ������


// Csnlme1App:
// �йش����ʵ�֣������ snlme1.cpp
//

class Csnlme1App : public CWinApp
{
public:
	Csnlme1App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Csnlme1App theApp;