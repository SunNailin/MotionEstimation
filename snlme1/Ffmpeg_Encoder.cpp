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
  
    m_pRGBFrame = new AVFrame[1];//RGB֡���ݸ�ֵ    
    m_pYUVFrame = new AVFrame[1];//YUV֡���ݸ�ֵ    
  
    c = NULL;//������ָ����󸳳�ֵ  
}  
void Ffmpeg_Encoder::Ffmpeg_Encoder_Setpara(CodecID mycodeid, int vwidth, int vheight)
{
	pCodecH264 = avcodec_find_encoder(mycodeid);//����h264������
	if (!pCodecH264)
	{
		fprintf(stderr, "h264 codec not found\n");
		exit(1);
	}
	width = vwidth;
	height = vheight;

	c = avcodec_alloc_context3(pCodecH264);//�������ڷ���һ��AVCodecContext������Ĭ��ֵ�����ʧ�ܷ���NULL��������av_free()�����ͷ�
	c->bit_rate = 400000; //���ò�����������������
	c->width = vwidth;//���ñ�����Ƶ��� 
	c->height = vheight; //���ñ�����Ƶ�߶�
	c->time_base.den = 2;//����֡��,numΪ���Ӻ�denΪ��ĸ�������1/25���ʾ25֡/s
	c->time_base.num = 1;
	c->gop_size = 10; //����GOP��С,��ֵ��ʾÿ10֡�����һ��I֡
	c->max_b_frames = 1;//����B֡�����,��ֵ��ʾ��������B֮֡�䣬����������B֡�����֡��
	c->pix_fmt = PIX_FMT_YUV420P;//�������ظ�ʽ

	av_opt_set(c->priv_data, "tune", "zerolatency", 0);//���ñ���������ʱ�����ǰ��ļ�ʮ֡�������ݵ����

	if (avcodec_open2(c, pCodecH264, NULL) < 0)return;//�򿪱�����

	nDataLen = vwidth*vheight * 3;//����ͼ��rgb����������

	yuv_buff = new uint8_t[nDataLen/2];//��ʼ����������Ϊyuvͼ��֡׼����仺��
	rgb_buff = new uint8_t[nDataLen];//��ʼ����������Ϊrgbͼ��֡׼����仺��
	outbuf_size = 100000;////��ʼ���������������
	outbuf = new uint8_t[outbuf_size];

	scxt = sws_getContext(c->width, c->height, PIX_FMT_BGR24, c->width, c->height, PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);//��ʼ����ʽת������
}	

void Ffmpeg_Encoder::Ffmpeg_Encoder_Encode(FILE *file, uint8_t *data)
{
	memcpy(rgb_buff, data, nDataLen);//����ͼ�����ݵ�rgbͼ��֡������׼������
	avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, PIX_FMT_RGB24, width, height);//��rgb_buff��䵽m_pRGBFrame
	avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, PIX_FMT_YUV420P, width, height);//��yuv_buff��䵽m_pYUVFrame
	sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);// ��RGBת��ΪYUV
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
	avcodec_close(c);//�رձ�����
	av_free(c);
}


