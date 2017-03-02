
// snlme1.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif


//FFmpeg 相关
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


#include "resource.h"		// 主符号


// Csnlme1App:
// 有关此类的实现，请参阅 snlme1.cpp
//

class Csnlme1App : public CWinApp
{
public:
	Csnlme1App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Csnlme1App theApp;