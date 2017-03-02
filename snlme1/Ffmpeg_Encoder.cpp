#include "StdAfx.h"
#include "Ffmpeg_Encoder.h"



Ffmpeg_Encoder::Ffmpeg_Encoder(void)
{
}
Ffmpeg_Encoder::~Ffmpeg_Encoder(void)
{
}

void Ffmpeg_Encoder::Ffmpeg_Encoder_Init()  
{  
    av_register_all();  
    avcodec_register_all();  
  
    m_pRGBFrame = new AVFrame[1];//RGB帧数据赋值    
    m_pYUVFrame = new AVFrame[1];//YUV帧数据赋值    
  
    c = NULL;//解码器指针对象赋初值  
}  
void Ffmpeg_Encoder::Ffmpeg_Encoder_Setpara(CodecID mycodeid, int vwidth, int vheight)
{
	pCodecH264 = avcodec_find_encoder(mycodeid);//查找h264编码器
	if (!pCodecH264)
	{
		fprintf(stderr, "h264 codec not found\n");
		exit(1);
	}
	width = vwidth;
	height = vheight;

	c = avcodec_alloc_context3(pCodecH264);//函数用于分配一个AVCodecContext并设置默认值，如果失败返回NULL，并可用av_free()进行释放
	c->bit_rate = 400000; //设置采样参数，即比特率
	c->width = vwidth;//设置编码视频宽度 
	c->height = vheight; //设置编码视频高度
	c->time_base.den = 2;//设置帧率,num为分子和den为分母，如果是1/25则表示25帧/s
	c->time_base.num = 1;
	c->gop_size = 10; //设置GOP大小,该值表示每10帧会插入一个I帧
	c->max_b_frames = 1;//设置B帧最大数,该值表示在两个非B帧之间，所允许插入的B帧的最大帧数
	c->pix_fmt = PIX_FMT_YUV420P;//设置像素格式

	av_opt_set(c->priv_data, "tune", "zerolatency", 0);//设置编码器的延时，解决前面的几十帧不出数据的情况

	if (avcodec_open2(c, pCodecH264, NULL) < 0)return;//打开编码器

	nDataLen = vwidth*vheight * 3;//计算图像rgb数据区长度

	yuv_buff = new uint8_t[nDataLen/2];//初始化数据区，为yuv图像帧准备填充缓存
	rgb_buff = new uint8_t[nDataLen];//初始化数据区，为rgb图像帧准备填充缓存
	outbuf_size = 100000;////初始化编码输出数据区
	outbuf = new uint8_t[outbuf_size];

	scxt = sws_getContext(c->width, c->height, PIX_FMT_BGR24, c->width, c->height, PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);//初始化格式转换函数
}	

void Ffmpeg_Encoder::Ffmpeg_Encoder_Encode(FILE *file, uint8_t *data)
{
	memcpy(rgb_buff, data, nDataLen);//拷贝图像数据到rgb图像帧缓存中准备处理
	avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, PIX_FMT_RGB24, width, height);//将rgb_buff填充到m_pRGBFrame
	avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, PIX_FMT_YUV420P, width, height);//将yuv_buff填充到m_pYUVFrame
	sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);// 将RGB转化为YUV
	int myoutputlen = avcodec_encode_video(c, outbuf, outbuf_size, m_pYUVFrame);
	fwrite(outbuf, 1, myoutputlen, file);
}

void Ffmpeg_Encoder::Ffmpeg_Encoder_Close()
{
	delete[]m_pRGBFrame;
	delete[]m_pYUVFrame;
	delete[]rgb_buff;
	delete[]yuv_buff;
	delete[]outbuf;
	sws_freeContext(scxt);
	avcodec_close(c);//关闭编码器
	av_free(c);
}


