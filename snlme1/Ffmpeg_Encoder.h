#pragma once
#include "snlme1.h"
class Ffmpeg_Encoder
{
public:
	Ffmpeg_Encoder(void);
	~Ffmpeg_Encoder(void);
	AVFrame *m_pRGBFrame;   //֡����  
    AVFrame *m_pYUVFrame;   //֡����  
    AVCodec *pCodecH264;    //������  
    AVCodecContext *c;      //���������ݽṹ����  
    uint8_t *yuv_buff;      //yuvͼ��������  
    uint8_t *rgb_buff;      //rgbͼ��������  
    SwsContext *scxt;       //ͼ���ʽת������  
    uint8_t *outbuf;        //���������Ƶ���ݻ���  
    int outbuf_size;        //�����������ȥ��С  
    int nDataLen;           //rgbͼ������������  
    int width;              //�����Ƶ���  
    int height;             //�����Ƶ�߶�  
  
public:  
    void Ffmpeg_Encoder_Init();//��ʼ��  
    void Ffmpeg_Encoder_Setpara(CodecID mycodeid,int vwidth,int vheight);//���ò���,��һ������Ϊ������,�ڶ�������Ϊѹ����������Ƶ�Ŀ�ȣ���������Ƶ��Ϊ��߶�  
    void Ffmpeg_Encoder_Encode(FILE *file, uint8_t *data);//���벢д�����ݵ��ļ�  
    void Ffmpeg_Encoder_Close();//�ر�  
};

