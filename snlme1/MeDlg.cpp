// MeDlg.cpp : ʵ���ļ�
//
#include "StdAfx.h"

#include "MeDlg.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "sys/timeb.h"
#include "math.h"
//#include "MEAlgorithm.h"
using namespace std;
////////////�������˶����Ƶ���ض���////////
typedef int  int32;
typedef short  int16;
typedef char  int8;
typedef unsigned short  uint16;
typedef unsigned int  uint32;
typedef unsigned char  uint8;

#if 1
#define XX 288
#define YY 352
#define OPEN_FILE "E:\\Yun\\ѧϰ\\����\\��Ƶͼ��\\foreman.yuv"
#define REF_FILE  "E:\\Yun\\ѧϰ\\����\\��Ƶͼ��\\foreman_ref.yuv"
#else
#define XX 144		//XXΪ��Ƶ�ĸ߶�
#define YY 176		//YYΪ��Ƶ�Ŀ��
#define OPEN_FILE "E:\\Yun\\ѧϰ\\����\\��Ƶͼ��\\foreman.yuv"
#define REF_FILE  "E:\\Yun\\ѧϰ\\����\\��Ƶͼ��\\foreman_ref.yuv"
#endif
#define BLOCK_HEIGTH 16			//���ĸ߶�
#define BLOCK_WIDTH 16			//���Ŀ��
#define MAX_MOTION 16			//���λ�ƣ�����������������
const int SEARCH_RANGE=MAX_MOTION*2+1;		//������Χ�����λ�Ƴ���2��1����������ɶ��˼������������
const int X=XX/BLOCK_HEIGTH;		//��Ƶ�ĸ߶�/���ĸ߶ȵõ�����ֱ�����ϵ���Ŀ
const int Y=YY/BLOCK_WIDTH;			//ˮƽ�����ϵ���Ŀ

typedef struct{
	int16 dx;
	int16 dy;
}MV;		//�����˶�ʸ���ṹ��

uint8 _flag_search[SEARCH_RANGE][SEARCH_RANGE];		//���������Χ������
uint8 (*const flag_search)[SEARCH_RANGE]=
	(uint8 (*)[SEARCH_RANGE])&_flag_search[MAX_MOTION][MAX_MOTION];		//����������Χ����

uint8 current_frame[XX][YY],ref_frame[XX][YY];		//����������֡����ǰ֡�Ͳο�֡

MV _mv_buffer[2][X][Y];				//mv������

struct{
	MV (*mv)[Y];			//�˶�ʸ��
	MV (*prev_mv)[Y];	//Ԥ���˶�ʸ��
	uint32 sad[X][Y];		//ÿһ���sad
	uint32 prev_sad[X][Y];	//ÿһ���ǰsad
	uint32 frame_sad;		//֡�ܵ�SAD
	uint32 sum_sad;		//sad�ĺ�
	uint32 frame_pot;		//֡�п����Ŀ
	uint32 sum_pot;		//����֡�������
	uint32 frame_sse;		//֡��MSE
	uint32 sum_sse;		//����֡��MSE�ĺ�
}frame_info;				//������֡��Ϣ�ṹ��

FILE *fp_cur,*fp_ref;		//�����洢��ȡ����Ƶ�ļ���ָ��
///////////////end//////////////////////////
//�����Ǹ����˶����Ƹ����㷨�����Ķ����
#define MIN(a,b) ((a)<(b))?(a):(b);		//�궨�壬�õ�ab����С������
#define MAX(a,b) ((a)>(b))?(a):(b);		//�궨�壬�õ�ab����������

//SAD����ʵ����SAD�ļ��㣬����ox��ʾ��ֵ�����ϵ�ƫ�ã�oy��ʾˮƽ�����ϵ�ƫ�ã�dx��dy��ʾ��������������ֵbest_sad
int SAD(const int ox,const int oy,const int dx,const int dy,const int height,const int width,uint32 &best_sad)
{
	const int rx=ox+dx,ry=oy+dy;
	if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION || flag_search[dx][dy]>0 )		//�����Ƿ������󲽳���������ͷ���
		return 0;
	if( rx<0 || ry<0 || rx+height>XX || ry+width>YY )			//�ж��Ƿ񳬳���������Χ
		return 0;
	uint32 sad=0;
	frame_info.frame_pot++;		//ÿ��һ��sad��frame_pot��һ
	const uint8 *p1=&current_frame[ox][oy],*p2=&ref_frame[rx][ry];
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			sad+=abs(*(p1++)-*(p2++));			//����в�ľ���ֵ
		}
		p1+=(YY-width);p2+=(YY-width);			//�ƶ�
	}
	flag_search[dx][dy]=1+sad;					
	if(sad>=best_sad)
		return 0;
	best_sad=sad;		//�õ������SAD
	return 1;
}

void rebuilt(const int x,const int y,const int height,const int width)	//֡�ؽ�������x��y�ֱ��������ڵ����е�λ��
{
	const int dx=frame_info.mv[x][y].dx,dy=frame_info.mv[x][y].dy;		//�õ���֡���˶�ʸ��
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;						//�õ�ƫ��
	const int rx=ox+dx,ry=oy+dy;										//�����Ƶ���λ�ã����ؽ����λ��
	int tmp;															//��ʱ����
	const uint8 *p1=&current_frame[ox][oy],*p2=&ref_frame[rx][ry];		//��ȡ�ĵ�ǰ֡�Ͳο�֡
	if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION )						//�ж��Ƿ񳬳���Χ
		exit(0);
	if( rx<0 || ry<0 || rx+height>XX || ry+width>YY )					//�ж��Ƿ����
		exit(0);
	for(int i=0;i<height;i++)											
	{
		for(int j=0;j<width;j++)
		{
			tmp=*(p1++)-*(p2++);										//��òв�
			frame_info.frame_sse+=tmp*tmp;								//���ǲв��ƽ��
		}
		p1+=(YY-width);p2+=(YY-width);									//�ڴ��ַ��
	}
}
//ȫ�����㷨������x��ʾ��ֱ�����ϵĿ��������y��ʾˮƽ�����ϵĿ��������height��ʾ���߶ȣ�width��ʾ�����
void search_FS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;		//ox��ʾ��ֱ�����ϵ�ƫ����������oy��ʾˮƽ�����ϵ�ƫ��������
	//const int ox=x*BLOCK_HEIGTH*2,oy=y*BLOCK_WIDTH*2;	
	uint32 sad=0xffffff;		//��ʼ�з���Ϊ0
	MV mv;
	for(int i=-MAX_MOTION;i<=MAX_MOTION;i++)
	{
		for(int j=-MAX_MOTION;j<=MAX_MOTION;j++)			//Ϊʲô��ô���أ���Ϊ����ȫ�����㷨 ����yes
		{
			if(SAD(ox,oy,i,j,heigth,width,sad)==1)			//������ֵ�����ϵı�����������ˮƽ�����ϵ�ƫ����������i��j�ֱ�������λ�ã�heigth,width�ֱ��Ǻ��ĸ߶ȺͿ��
			{												//sad��ʾ��õ�sad��mvΪ�˶�ʸ��
				mv.dx=i;mv.dy=j;
			}
		}
	}
	frame_info.mv[x][y]=mv;		//�˶�ʸ��
	frame_info.sad[x][y]=sad;	//��С��sad
	frame_info.frame_sad+=sad;	//֡�ܵ�sad
}
//////�궨�庯������C���Ժ궨��ֻ��һ�й�ʹ��"\"��ʾת����һ��//��������ģ��ĺ궨�壬pattern��ʾ����ģ������ƣ�num��ʾ����������flag����
#define PATTERN_SEARCH(pattern,num,flag) \
	do\
	{\
		mvx=mv.dx;mvy=mv.dy;\
		for(int i=0;i<num;i++)\
		{\
			if(SAD(ox,oy,mvx+pattern[i][0],mvy+pattern[i][1],heigth,width,sad)==1)\
			{\
				mv.dx=mvx+pattern[i][0];mv.dy=mvy+pattern[i][1];\
			}\
		}\
	}while ( (mv.dx!=mvx || mv.dy!=mvy) && flag==1 );

//�Ĳ������㷨
void search_4SS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int L4SS[9][2]={{0,0},{0,2},{-2,2},{-2,0},{-2,-2},{0,-2},{2,-2},{2,0},{2,2}};		//�����˰뾶Ϊ2�Ĵ�������ģ��
	const int S4SS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};		//�����˰뾶Ϊ1��С������ģ��
	uint32 sad=0xffffff;		//��ʾ0
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(L4SS,9,1)
	PATTERN_SEARCH(S4SS,9,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

//�ݶ��½������㷨
void search_BBGDS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int BBGDS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};		//�뾶Ϊ1������������ģ��
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(BBGDS,9,1)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
//���������㷨
void search_DS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};		//������ģ��
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};					//С����ģ��
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;
	//����ģ���������
	PATTERN_SEARCH(LDS,9,1)		//����һ��
	PATTERN_SEARCH(SDS,5,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

//�����������㷨
void search_HS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LHS[7][2]={{0,0},{0,2},{-2,1},{-2,-1},{0,-2},{2,-1},{2,1}};		//��������ģ��
	const int SHS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{1,1},{1,-1},{-1,1},{-1,-1}};		//С������ģ��
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(LHS,7,1)
	PATTERN_SEARCH(SHS,9,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
//�궨�壬����C���Եĺ궨��ֻ��һ�У���\��ʾת����һ��
#define CHECK_ONE_PIXEL(i,j) \
	if(SAD(ox,oy,i,j,heigth,width,sad)==1)\
	{\
		mv.dx=i;mv.dy=j;\
	}
//ȡ��a,b,c���м�ֵ
int x264_median( int a, int b, int c=0 )
{
    int min = a, max =a;
    if( b < min )
        min = b;
    else
        max = b;

    if( c < min )
        min = c;
    else if( c > max )
        max = c;
    return a + b + c - min - max;
}

//��ȡ���õ�Ԥ���˶�ʸ���б�����ARPS����
void Get_MVp(const int x,const int y,MV *pre_mv,int &mvx,int &mvy,uint32 *sad=NULL)
{
	uint32 num[10];if(sad==NULL) sad=num;
	/***************D  B  C*****************/
	/***************A  E   *****************/
	if(y>0)		//ע�⣡����x���������꣬y���������
	{
		pre_mv[0]=frame_info.mv[x][y-1];
		sad[0]=frame_info.sad[x][y-1];	//���A��MVA
	}
	else
	{
		pre_mv[0].dx=pre_mv[0].dy=0;
		sad[0]=0;
	}
	if(x>0)
	{
		pre_mv[1]=frame_info.mv[x-1][y];
		sad[1]=frame_info.sad[x-1][y];		//���B MVB
	}
	else
	{
		pre_mv[1].dx=pre_mv[1].dy=0;
		sad[1]=0;
	}
	if(x>0 && y<Y-1)
	{
		pre_mv[2]=frame_info.mv[x-1][y+1];
		sad[2]=frame_info.sad[x-1][y+1];		//���C MVC
	}
	else if(x>0)			///x>0,y=Y-1//�����ұ߽�
	{
		pre_mv[2]=frame_info.mv[x-1][y-1];	
		sad[2]=frame_info.sad[x-1][y-1];	//���C��MVB���� MVC
	}
	else		//x<=0 
	{
		pre_mv[2].dx=pre_mv[2].dy=0;
		sad[2]=0;
	}
	if(x>0&&y>0)
	{
		pre_mv[3]=frame_info.mv[x-1][y-1];
		sad[3]=frame_info.sad[x-1][y-1];		//���D MVD
	}
	else
	{
		pre_mv[3].dx=pre_mv[3].dy=0;
		sad[3]=0;
	}

	pre_mv[4]=frame_info.prev_mv[x][y];		//�����ǰ����ϢMVE
	sad[4]=frame_info.prev_sad[x][y];		

	pre_mv[5].dx=2*frame_info.prev_mv[x][y].dx-frame_info.mv[x][y].dx;
	pre_mv[5].dy=2*frame_info.prev_mv[x][y].dy-frame_info.mv[x][y].dy;
	//������û���õ������ //ע��!����frame_info.mv[x][y]��Ϊ��ǰ֡��û����ʴ�������֡���˶�ʸ��
	if(x==0)
	{
		mvx=pre_mv[0].dx;
		mvy=pre_mv[0].dy;
		return;
	}
	mvx=x264_median(pre_mv[0].dx,pre_mv[1].dx,pre_mv[2].dx);
	mvy=x264_median(pre_mv[0].dy,pre_mv[1].dy,pre_mv[2].dy);
}

//����Ӧʮ�������㷨
void search_ARPS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int SCS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};		//Сʮ��ģ��
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;
	const uint32 T=512;
	//�ж�ǰһ֡��ͬλ���ǲ������ƥ���
	CHECK_ONE_PIXEL(0,0)		
	if(sad<T)	goto END;
	{
		Get_MVp(x,y,pre_mv,mvx,mvy);	//�����ٽ����MV
		int Length=abs(pre_mv[0].dx)>abs(pre_mv[0].dy)?abs(pre_mv[0].dx):abs(pre_mv[0].dy);
		CHECK_ONE_PIXEL(mvx,mvy)
		CHECK_ONE_PIXEL( Length,0)
		CHECK_ONE_PIXEL(-Length,0)
		CHECK_ONE_PIXEL(0, Length)
		CHECK_ONE_PIXEL(0,-Length)//������Сʮ��ģ�����Ӧ�ĵĸ���λ��
	}//������Сʮ��ģ�����Ӧ�ĵĸ���λ��
	PATTERN_SEARCH(SCS,5,1)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

////���������㷨û���ڱ�������ʹ�õ�
/*
void search_ARPS3(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int SCS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;
	const uint32 T=512;
	
	CHECK_ONE_PIXEL(0,0)
	if(sad<T)	goto END;
	{
		Get_MVp(x,y,pre_mv,mvx,mvy);
		int max_x=MAX(pre_mv[0].dx,pre_mv[1].dx);max_x=MAX(max_x,pre_mv[2].dx)max_x=MAX(max_x,pre_mv[3].dx);
		int max_y=MAX(pre_mv[0].dy,pre_mv[1].dy);max_y=MAX(max_y,pre_mv[2].dy)max_y=MAX(max_y,pre_mv[3].dy);
		int min_x=MIN(pre_mv[0].dx,pre_mv[1].dx);min_x=MIN(min_x,pre_mv[2].dx)min_x=MIN(min_x,pre_mv[3].dx);
		int min_y=MIN(pre_mv[0].dy,pre_mv[1].dy);min_y=MIN(min_y,pre_mv[2].dy)min_y=MIN(min_y,pre_mv[3].dy);
		CHECK_ONE_PIXEL(mvx,mvy)
		CHECK_ONE_PIXEL( max_x,mvy)
		CHECK_ONE_PIXEL( min_x,mvy)
		CHECK_ONE_PIXEL( mvx,max_y)
		CHECK_ONE_PIXEL( mvx,min_y)
	}
	PATTERN_SEARCH(SCS,5,1)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
*/
int Get_Mv_Length(int x,int y,MV *pre_mv,int mvx=0,int mvy=0)
{///ȷ�������뾶��L=max{ |mv.x| +|mv.y| } mv��{MVA��MVB��MVC}
	if(x==0&&y==0)
		return 2;
	int L=0,num;
	num=abs(pre_mv[0].dx-mvx)+abs(pre_mv[0].dy-mvy);
	if(L<num)L=num;
	num=abs(pre_mv[1].dx-mvx)+abs(pre_mv[1].dy-mvy);
	if(L<num)L=num;
	num=abs(pre_mv[2].dx-mvx)+abs(pre_mv[2].dy-mvy);
	if(L<num)L=num;
	return L;	//Lmax = Max(L(MVA), L(MVB), L(MVC))����MV�����뾶�Ľ���
}

//�˶���ʸ������Ӧ�����㷨
void search_MVFAST(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};	//������ģ��
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};		//С����ģ��
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;
	const uint32 T=512,L1=1,L2=2;		//һ��T,L1,L2����MV�����뾶�Ľ���
	
	CHECK_ONE_PIXEL(0,0)
	if(sad<T)	goto END;
	{
		Get_MVp(x,y,pre_mv,mvx,mvy);
		int L=Get_Mv_Length(x,y,pre_mv);
		if(L<=L1)goto SMALL_SEARCH;
		if(L>L2)
		{
//			CHECK_ONE_PIXEL(mvx,mvy)
			CHECK_ONE_PIXEL(pre_mv[0].dx,pre_mv[0].dy)
			CHECK_ONE_PIXEL(pre_mv[1].dx,pre_mv[1].dy)
			CHECK_ONE_PIXEL(pre_mv[2].dx,pre_mv[2].dy)
			goto SMALL_SEARCH;
		}
	}
	PATTERN_SEARCH(LDS,9,1)
SMALL_SEARCH:
	PATTERN_SEARCH(SDS,5,1)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}////���������㷨û���ڱ�������ʹ�õ�
/*
int equal_mv(MV mv1,MV mv2)
{
	if(mv1.dx==mv2.dx && mv1.dy==mv2.dy)
		return 1;
	else
		return 0;
}
void search_PMVFAST(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;

	int thresa=512,thresb=1024,Found=0,PredEq=0;
	if(x>0 && y>0)
	{
		thresa=MIN(frame_info.sad[x-1][y],frame_info.sad[x][y-1]);
		thresa=MIN(thresa,frame_info.sad[x-1][y+1]);
		thresb=thresa+256;
		if(thresa<512) thresa=512;
		if(thresa>1024)thresa=1024;
		if(thresb>1792)thresb=1792;
	}
	Get_MVp(x,y,pre_mv,mvx,mvy);
	if(x>0 && equal_mv(pre_mv[0],pre_mv[1]) && equal_mv(pre_mv[0],pre_mv[2]) )
		PredEq=1;

	int Distance=abs(mvx)+abs(mvy);
	if( PredEq==1 && mvx==pre_mv[4].dx && mvy==pre_mv[4].dy )
		Found=2;

	CHECK_ONE_PIXEL(mvx,mvy)
	if( equal_mv(mv,pre_mv[4]) && sad<frame_info.prev_sad[x][y] )
		goto END;
	if(sad<256) 
		goto END;
	CHECK_ONE_PIXEL(0,0)
	CHECK_ONE_PIXEL(pre_mv[0].dx,pre_mv[0].dy)
	CHECK_ONE_PIXEL(pre_mv[1].dx,pre_mv[1].dy)
	CHECK_ONE_PIXEL(pre_mv[2].dx,pre_mv[2].dy)
	CHECK_ONE_PIXEL(pre_mv[4].dx,pre_mv[4].dy)
	if(sad<thresa)
		goto END;
	if( equal_mv(mv,pre_mv[4]) && sad<frame_info.prev_sad[x][y] )
		goto END;
	if( Distance>0 || thresb<1536 || PredEq==1 )
		goto SMALL_SEARCH;
	PATTERN_SEARCH(LDS,9,Found!=2)
SMALL_SEARCH:
	PATTERN_SEARCH(SDS,5,Found!=2)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
void search_CDHS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LCS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{0,2},{-2,0},{0,-2},{2,0}};
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
	const int HHS[7][2]={{0,0},{0,2},{0,-2},{1,1},{1,-1},{-1,1},{-1,-1}};
	const int VHS[7][2]={{0,0},{2,0},{-2,0},{1,1},{1,-1},{-1,1},{-1,-1}};
	uint32 sad=0xffffff;
	MV mv={0,0},mv_tmp;int mvx,mvy,flag;

	PATTERN_SEARCH(SDS,5,0)
	if(mv.dx==0&&mv.dy==0)goto END;
	PATTERN_SEARCH(SDS,5,0)
	if(abs(mv.dx)+abs(mv.dy)==1)goto END;
	mv_tmp=mv;mv.dx=mv.dy=0;
	PATTERN_SEARCH(LCS,9,0)
	if(mv.dx==0&&mv.dy==0) mv=mv_tmp;
	if(abs(mv.dx)==1 && abs(mv.dy)==1)
		flag=0;
	else if(abs(mv.dx)==0 && abs(mv.dy)==2)
		flag=1;
	else if(abs(mv.dx)==2 && abs(mv.dy)==0)
		flag=2;
	else
		printf("error\n");
	do
	{
		mvx=mv.dx;mvy=mv.dy;
		if(flag==0)
		{
			for(int i=0;i<9;i++)
			{
				if(SAD(ox,oy,mvx+LDS[i][0],mvy+LDS[i][1],heigth,width,sad)==1)
				{
					mv.dx=mvx+LDS[i][0];mv.dy=mvy+LDS[i][1];
					if		(abs(LDS[i][1])==2) flag=1;
					else if	(abs(LDS[i][0])==2) flag=2;
					else						flag=0;
				}
			}
		}
		else if(flag==1)
		{
			for(int i=0;i<7;i++)
			{
				if(SAD(ox,oy,mvx+HHS[i][0],mvy+HHS[i][1],heigth,width,sad)==1)
				{
					mv.dx=mvx+HHS[i][0];mv.dy=mvy+HHS[i][1];
					flag = abs(HHS[i][1])==2 ? 1 : 0;
				}
			}
		}
		else
		{
			for(int i=0;i<7;i++)
			{
				if(SAD(ox,oy,mvx+VHS[i][0],mvy+VHS[i][1],heigth,width,sad)==1)
				{
					mv.dx=mvx+VHS[i][0];mv.dy=mvy+VHS[i][1];
					flag = abs(VHS[i][0])==2 ? 2 : 0;
				}
			}
		}
	}while ( mv.dx!=mvx || mv.dy!=mvy );
	PATTERN_SEARCH(SDS,5,0)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
void search_AVPS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};
	const int LHS[7][2]={{0,0},{0,2},{-2,1},{-2,-1},{0,-2},{2,-1},{2,1}};
	const int SHS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
	const int L4SS[9][2]={{0,0},{0,2},{-2,2},{-2,0},{-2,-2},{0,-2},{2,-2},{2,0},{2,2}};
	const int S4SS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];uint32 pre_sad[10];int mvx,mvy;

	int thresa=512,thresb=1024,PredEq=0;
	Get_MVp(x,y,pre_mv,mvx,mvy,pre_sad);
	int sad_max=0xffffff,sad_min=0;
	if(x>0 && y>0)
	{
		if(equal_mv(mv,pre_mv[0]))			
			sad_min=MAX(sad_min,pre_sad[0])
		else
			sad_max=MIN(sad_max,pre_sad[0]);
		if(equal_mv(mv,pre_mv[1]))			
			sad_min=MAX(sad_min,pre_sad[1])
		else
			sad_max=MIN(sad_max,pre_sad[1]);                                                                                                                                                                                                                        
		if(equal_mv(mv,pre_mv[2]))			
			sad_min=MAX(sad_min,pre_sad[2])
		else
			sad_max=MIN(sad_max,pre_sad[2]);
//		if(equal_mv(mv,pre_mv[3]))			
//			sad_min=MAX(sad_min,pre_sad[3])
//		else
//			sad_max=MIN(sad_max,pre_sad[3]);
		if(sad_min==0)sad_min=512;
		if(sad_max==0xffffff)sad_max=512;
		thresa=MAX(sad_max,sad_min);
				
		sad_max=MAX(pre_sad[0],pre_sad[1]);
		sad_max=MAX(sad_max,pre_sad[2]);
		sad_max=MAX(sad_max,pre_sad[3]);
		sad_min=MIN(pre_sad[0],pre_sad[1]);
		sad_min=MIN(sad_min,pre_sad[2]);
		sad_min=MIN(sad_min,pre_sad[3]);

		thresb=MAX(sad_max,thresa+256);
		if(thresa<512) thresa=512;
		if(thresa>1024)thresa=1024;
		if(thresb>1792)thresb=1792;
	}
	else
	{
		sad_min=0;
		sad_max=2048;
	}
	if(x>0 && equal_mv(pre_mv[0],pre_mv[1]) && equal_mv(pre_mv[0],pre_mv[2]) )
		PredEq=1;

	CHECK_ONE_PIXEL(mvx,mvy)
	if(sad<256)
		goto END;
	CHECK_ONE_PIXEL(0,0)
	CHECK_ONE_PIXEL(pre_mv[0].dx,pre_mv[0].dy)
	CHECK_ONE_PIXEL(pre_mv[1].dx,pre_mv[1].dy)
	CHECK_ONE_PIXEL(pre_mv[2].dx,pre_mv[2].dy)
	CHECK_ONE_PIXEL(pre_mv[4].dx,pre_mv[4].dy)
	if(sad<thresa)
		goto END;
	if(sad<thresb||sad_max-sad_min<256||(x>0&&y>0&&Get_Mv_Length(x,y,pre_mv,mvx,mvy)<2))
		goto SMALL_SEARCH2;
	else
	{
		CHECK_ONE_PIXEL(pre_mv[5].dx,pre_mv[5].dy)
		goto LARGE_SEARCH0;
	}
//	{
//	CHECK_ONE_PIXEL(pre_mv[5].dx,pre_mv[5].dy)
//	PATTERN_SEARCH(LHS,7,1)
//	int sad_sub=0xffffff;mvx=mvy=0;
//	for(int i=1;i<7;i++)
//	{
//		int dx=mv.dx+LHS[i][0],dy=mv.dy+LHS[i][1];
//		const int rx=ox+dx,ry=oy+dy;
//		if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION)
//			continue;
//		if( rx<0 || ry<0 || rx+heigth>XX || ry+width>YY )
//			continue;
//		if( flag_search[dx][dy]<sad_sub )
//		{
//			sad_sub=flag_search[dx][dy];
//			mvx=LHS[i][0];mvy=LHS[i][1];
//		}
//	}
//	int shsxy[3][2];
//	if(mvx==0)
//	{
//		shsxy[0][0]=-1;shsxy[1][0]=0;shsxy[2][0]=1;shsxy[0][1]=shsxy[1][1]=shsxy[2][1]=mvy/2;
//	}
//	else
//	{
//		shsxy[0][0]=mvx/2;shsxy[0][1]=mvy;
//		shsxy[1][0]=mvx/2;shsxy[1][1]=0;
//		shsxy[2][0]=    0;shsxy[2][1]=mvy;
//	}
//	PATTERN_SEARCH(shsxy,3,0);
//	PATTERN_SEARCH(SDS,5,0);
//	goto END;
//	}
LARGE_SEARCH0:
	PATTERN_SEARCH(LDS,9,1)
	PATTERN_SEARCH(SDS,5,0)
	goto END;
LARGE_SEARCH1:
	PATTERN_SEARCH(LHS,7,1)
	PATTERN_SEARCH(SHS,9,0)
	goto END;
LARGE_SEARCH2:
	PATTERN_SEARCH(L4SS,9,1)
	PATTERN_SEARCH(S4SS,9,0)
	goto END;
SMALL_SEARCH0:
	PATTERN_SEARCH(SHS,9,1)
		goto END;
SMALL_SEARCH1:
	PATTERN_SEARCH(SDS,5,1)
		goto END;
SMALL_SEARCH2:
	do
	{
		MV tmp_mv=mv;uint32 tmp_sad=sad;
		mvx=mv.dx;mvy=mv.dy;sad=0xffffff;
		for(int i=1;i<5;i++)
		{
			if(SAD(ox,oy,mvx+SDS[i][0],mvy+SDS[i][1],heigth,width,sad)==1)
			{
				mv.dx=mvx+SDS[i][0];mv.dy=mvy+SDS[i][1];
			}
		}
		if(sad<tmp_sad)
			continue;
		if(sad/(float)tmp_sad>1.15)
		{
			mv=tmp_mv;sad=tmp_sad;
			break;
		}
		if(mv.dy!=mvy)
		{
			CHECK_ONE_PIXEL(mv.dx-1,mv.dy)
			CHECK_ONE_PIXEL(mv.dx+1,mv.dy)
		}
		if(mv.dx!=mvx)
		{
			CHECK_ONE_PIXEL(mv.dx,mv.dy-1)
			CHECK_ONE_PIXEL(mv.dx,mv.dy+1)
		}
		if(sad>=tmp_sad)
		{
			mv=tmp_mv;sad=tmp_sad;
			break;
		}
	}while ( 1 );
	goto END;
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
#undef PATTERN_SEARCH*/

////////////////////////end//////////////////////////


// CMeDlg �Ի���
CString FSIntro(_T("        FS�ֱ���Ϊ������㷨�����㷨�Ļ���˼·���£��㷨��������Χ((M+2dxmax)��(N+2dymax))�����еĿ��ܺ�ѡλ�ü���MAD/SAD�������ѡȡ��С��ֵ�����Ӧ��ƫ������ΪMV��\n�㷨�������£��μ�������ͼ2-1����\n��һ����������ԭ�����������˳ʱ����������ӽ���Զ���б��������������λ�õ�MAD/SAD��\n�ڶ������Ӽ�������ѡȡ��С��ֵ��ͬʱ�Ƚ�ԭ�����ȡ��MV��\n���㷨��ʽ�򵥣�Ч����ã����Ǽ������ϴ󣬼�������ʱ��ϳ�����ȻFS�Ľ����ȫ�����ŵģ�����������ȱ��Ҳʮ������������ͨ���ڱ����ʱ���ǲ�ʹ�ø��㷨��FSһ����Ϊ���������㷨�Ķ��ա�"));
CString _4SSIntro(_T("        FSS��TSS�����ƣ��㷨�п����˺�������ƫ�����ԣ�ͬʱ����������С��Χ�ƶ��ʹ�Χ�ƶ���������������¾���ȡ�ýϺõ�Ч�������㷨�Ļ���˼·���£����㷨������5��5���������ڣ�ÿһ�����������ڴ�С����һ�������ƥ��������������ҽ�����ԭ��������һ�������ƥ��㣬ǰ�����������Ƕ��������������һ�������ı䲽�����Ӷ��õ��������ƥ��㡣�㷨�������£��μ�������ͼ2-3������һ��������������㣬����Ϊԭ�㻮���������ڣ������ĵ��Լ�����һ���Ĳ������õ��������������η�Χ��Ե8�������SAD��������ĵ��SAD��С�����������Ĳ����ڶ������������ڴ�С���䣬�������ĵ�������һ���õ������ƥ��㡣����õ�λ����һ���������ڵ��ĸ��ǵ��ϣ��������5�������ƥ�����㣻����õ�λ����һ���������ڵ��ĸ������ϣ��������3�������ƥ�����㡣��������ȡ����һ�������ƥ��㣬�ظ��ڶ����Ĳ��衣���Ĳ�����С�������ڣ�����SAD���õ����ƥ��㼴�ɵ�MV�����㷨��TSS��ȣ����������Ƚ�Ϊƽ�������ӶȽϵͣ����и�ǿ��³���ԣ�ͬʱ��FS���Ҳ�������˼�������"));
CString DSIntro(_T("        DS�㷨�Ļ���˼·���£�Ϊ�˷�ֹ�������ڹ�����С�������Ĳ���Ӱ�죬DS�㷨������������ͬ������ģ�壬��ģ��ӵ��9�����㣬Сģ��ӵ��5�����㡣����ʱ�����ô�ģ����м��㣬�����ƥ���λ�����ĵ�ʱ������Сģ�壬�ٽ����������õ����յ����ƥ��㡣�㷨�������£��μ�������ͼ2-5������һ�������ô�ģ��Ľ��������������������������Լ�����8���㴦��SAD��ȡ��С�õ����ƥ��㣬������ƥ���Ϊ���ĵ㣬����е�������������еڶ������ڶ���������һ���õ������ƥ�����Ϊ���ĵ㣬���ô�ģ������������ظ���һ������ز��衣������������һ���õ������ƥ�����Ϊ���ĵ㣬�л���Сģ��������������㴰�����ĵ��Լ�����4���㴦��SAD��ȡ��С�õ����յ����ƥ��㣬ͬʱ�õ�MV�����㷨��������������Ƶͼ���������˶��Ļ������ɣ����������ִ�С��ͬ��ģ����������������ô�ģ����д��Զ�λ��������Сģ����о�ȷ��λ���Ȳ�������ֲ����ţ�Ҳ��֤�������ľ��ȣ�ͬʱҲ������������ٶȡ�"));
CString HSIntro(_T("        HS�㷨�Ļ���˼·���£�Ϊ�˿˷�DS�㷨�����ƥ���ѡ���ì�ܣ�������HS�㷨�����㷨����ʹ��������ȵطֲ���������������ΪԲ�ĵ�Բ���ϣ��Ӷ�ʹ������������Ż����㷨��������������ģ�壬һ����������ģ���һ��С������ģ�壬��������ͼ2-6(a)��ͼ2-6(b)��ʾ������ʱ��ʹ�ô�������ģ�壬��SAD��С��λ�����ĵ�ʱ����С������ģ�塣�㷨�������£���һ����ʹ�ô�������ģ��������������������������ĵ��Լ��������㴦��SAD�������С��SAD��������λ�ã������ƥ��㲻�����Ĵ���������������������еڶ������ڶ���������һ����õ����ƥ���Ϊ���������ĵ㣬�ô�������ģ������������ҵ����ƥ��㣬������ƥ��㲻�����ĵ㣬�����������������ظ��ڶ�����������������С������ģ������������ҵ�SAD��С�����ƥ��㣬���õ��ú���MV�����㷨�ڼ̳���DS�㷨���ŵ��ǰ���£�����������������ͬʱҲ��������ֲ����ţ�����������ٶȺ;��ȡ�"));
CString ARPSIntro(_T("        ARPS�Ļ���˼·���£����õ�ǰ��������Χ�Ŀ���˶��������Ե�ǰ��������˶����ƽ���Ԥ�⣬һ����˵���õ���������˶����ƣ��õ��ÿ��MV�����㷨��Ҫ�����������裬��һ��������Ӧ�����������ڶ����Ǿ�ȷ�ľֲ��������ڳ��������У�����һ��ʮ���͵�����ģ�壬���ø�����ģ�����������Ϊ��ǰ����������MV����ֱ��ˮƽ�����нϴ��һ����������ֱ���ʮ�������ĵ���ĸ����㡣�㷨�������£���һ��������������㱾���Ԥ��MV������õ��SAD�������õ���ֵ����ô�������������ڶ���������ʮ���͵��ĸ����㣬�õ�SAD��С�����ƥ��㡣�����������оֲ���ȷ����������ģ���л�ΪС���Σ����Ͻ������������㷨�ܹ�����ǰһ�����MV��Ч��Ԥ����ǰ���������Χ�������DS�㷨�ڼ����ٶ�������������������"));
CString BBGDSIntro(_T("        BBGDS�Ļ���˼·���£�������TSS��FSS��ͬ�����㷨ֻ�ǲ�����һ������������С��ģ�岻�ϵؽ���������ֱ���ҵ���SAD��С�ĵ�λ������ģ������Ļ��ߴﵽ���������ڵı�Եʱ��ֹͣ��������FSS��ĳЩ�������裬BBGDS�ĺ�����ÿ���������������3��5�������㡣�㷨�������£��μ�ͼ2-4������һ������������ģ����������������������������Լ�����8�������SAD��ȡ��С�õ����ƥ��㣬������ƥ���Ϊ���ĵ㣬��������õ�MV��������еڶ������ڶ���������һ���õ������ƥ���Ϊ���ĵ㣬������ͬ������ģ�����������������Ҫ�����ĵ�����3��5����������ƥ���Ϊ���ĵ㣬��������õ�MV�������ظ����������㷨�����ʺ��˶�����С�ĳ��ϡ���ÿһ�����������У����㷨ʹ�õĲ���ƥ����������ƥ��飬���һ������������ֲ����ŵĿ��ܡ������������ݶ��½��ķ���������Ӷ����ԶԸ÷�������ص���������ͼ��ٻ��߱����˲���Ҫ���������ܴ�̶��Ͻ������㷨�ĸ��Ӷȡ�"));
CString MVFASTIntro(_T("        MVFAST�㷨�ĺ����Ƕ��˶�ģʽ��MA, Motion Activity�����ж���������ģ����DS�㷨���ƣ���Ȼ���ô�С��������ģ�壬����˼·���£��ٶ���Ƶ֡ͼ�����Ԥ������ͨ��Ԥ����������ֹ�顣��ȷ���ֲ��˶�������ͨ���ú����Χ�����ɸ������㷨��Ϊ3��������MV��ȷ���ú����˶�ģʽ���ֱ�����Χ��������MVΪV1��V2��V3������Vi=(Xvi,Yvi)��Xvi��Yvi�ֱ�ΪVi��ˮƽ����ֱ����������Lvi=|Xvi|+|Yvi|��L1=1��L2=2��ȡL=MAX(Lvi)�������е�Vi����ǰ�����˶���ʽ�������£���L<=L1����MA=LOW����L1<L<L2����MA=MEDIUM����L1<L2<L����MA=HIGH����ȷ���������ģ����MAΪLOW����MEDIUM����ô��������ȡΪԭ�㣬���MAΪHIGH����ô��������ȡ����SAD��Сֵ��ʸ������ѡ���������ԣ����MAΪLOW����HIGH����ô����С����ģ�壬���MAΪMEDIUM���Ȳ��ô�����ģ�壬�ٲ���С����ģ������������㷨�������㷨˼·��ͬ����׸��������MVFAST�㷨Ԥ��ȥ����MVΪ���С�Ŀ飬ͬʱ��Ƶ֡ͼ���кܶ���MVΪ�㣬���Ը��㷨�������˲���Ҫ�ļ�������ͬʱ���㷨���˶���ʸ��������Ԥ�⣬�����˲�ͬ��MA��������MAѡȡ��ͬ������ģ�壬Ҳʹ���������Ӿ�ȷ��"));
//CString CDHSIntro(_T("        �����˶������㷨���"));
int AlTypeNum=0;

IMPLEMENT_DYNAMIC(CMeDlg, CDialog)

CMeDlg::CMeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMeDlg::IDD, pParent)
	, m_AlIntro(_T(""))
	, m_Cost(0)
	, m_PSNR(0)
	, m_spath(_T(""))
	, m_spath_ref(_T(""))
{

}

CMeDlg::~CMeDlg()
{
}

void CMeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_SelAl);
	/*m_SelAl.AddString(_T("1.ȫ�����㷨��FS��"));
	m_SelAl.AddString(_T("2.�Ĳ������㷨��4SS��"));
	m_SelAl.AddString(_T("3.�ݶ��½������㷨��BBGDS��"));
	m_SelAl.AddString(_T("4.���������㷨��DS��"));
	m_SelAl.AddString(_T("5.�����������㷨��HS��"));
	m_SelAl.AddString(_T("6.����Ӧʮ�������㷨��ARPS��"));
	m_SelAl.AddString(_T("7.�˶�ʸ��������Ӧ�����㷨��MVFAST��"));
	m_SelAl.AddString(_T("8.�����˶������㷨��CDHS��"));*/
	//m_SelAl.SetCurSel(0); 

	DDX_Text(pDX, IDC_EDIT1, m_AlIntro);
	//  DDX_Text(pDX, IDC_EDIT3, m_Analysis);
	DDX_Text(pDX, IDC_EDIT4, m_Cost);
	DDX_Text(pDX, IDC_EDIT5, m_PSNR);
	//  DDX_Text(pDX, IDC_EDIT3, m_Analysis);
	DDX_Control(pDX, IDC_LIST1, m_Analysis);
	DDX_Text(pDX, IDC_EDIT2, m_spath);
	DDX_Text(pDX, IDC_EDIT3, m_spath_ref);
}


BEGIN_MESSAGE_MAP(CMeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CMeDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMeDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &CMeDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMeDlg::OnBnClickedButton3)
//	ON_LBN_SELCHANGE(IDC_LIST1, &CMeDlg::OnLbnSelchangeList1)
ON_BN_CLICKED(IDC_BUTTON4, &CMeDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMeDlg ��Ϣ�������


void CMeDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();
}


void CMeDlg::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString AlType;  
    int nSel;     

    // ��ȡ��Ͽ�ؼ����б����ѡ���������   
    nSel = m_SelAl.GetCurSel();   
    // ����ѡ����������ȡ�����ַ���   
    m_SelAl.GetLBText(nSel, AlType);  
	if(AlType == "1.ȫ�����㷨��FS��")
	{
		AlTypeNum = 1;
		m_AlIntro=FSIntro;
	}
	/*else if(AlType == "2.�Ĳ������㷨��FSS��")
	{
		AlTypeNum = 2;
		m_AlIntro=_4SSIntro;
	}
	else if(AlType == "3.�ݶ��½������㷨��BBGDS��")
	{
		AlTypeNum = 3;
		m_AlIntro=BBGDSIntro;
	}*/
	else if(AlType == "2.���������㷨��DS��")
	{
		AlTypeNum = 4;
		m_AlIntro=DSIntro;
	}
	else if(AlType == "3.�����������㷨��HS��")
	{
		AlTypeNum = 5;
		m_AlIntro=HSIntro;
	}
	/*else if(AlType == "6.����Ӧʮ�������㷨��ARPS��")
	{
		AlTypeNum = 6;
		m_AlIntro=ARPSIntro;
	}*/
	else if(AlType == "4.�˶�ʸ��������Ӧ�����㷨��MVFAST��")
	{
		AlTypeNum = 7;
		m_AlIntro=MVFASTIntro;
	}
//	else if(AlType == "8.�����˶������㷨��CDHS��")
//	{
//		AlTypeNum = 8;
//		m_AlIntro=CDHSIntro;
//	}
	else if(AlType == "")
	{
		AlTypeNum=0;
		m_AlIntro="��ѡ����Ӧ���˶������㷨";
	}

	UpdateData(false);

	
}
void useAl(int Alnum)
{
	
}

void CMeDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//useAl(AlTypeNum);
	m_Analysis.ResetContent();
	m_Analysis.AddString(_T("֡��        ֡����COST        PSNR"));
	char Ana[80];
	char cnum[3];
	float tpsnr,tcost;
	CString temp;
	int num;
	int j=0;
	if((fp_cur = fopen((CW2A)m_spath,"rb"))==NULL)	MessageBox(_T("������Ƶ����ʱ��������"));	//��ȡ��Ƶ����
	//if((fp_ref = fopen( REF_FILE,"rb"))==NULL)	return;

	//CString m_spath_ref = sprintf()
	if((fp_ref = fopen( (CW2A)m_spath,"rb"))==NULL)	return;		//��ȡ�ο�����
	frame_info.sum_pot=frame_info.sum_sad=frame_info.sum_sse=0;		//��ȫ������
	frame_info.mv=_mv_buffer[0];		//mv_bufferӦ����mv�Ļ�����
	frame_info.prev_mv=_mv_buffer[1];	//Ԥ��mv
	//printf("frame\tcost\tpsnr\n");
	for(num=0;num<100;num++)
	{
		int i,j;
		fseek(fp_cur,XX*YY*3/2*(num+1),SEEK_SET);	//��ȡ�������˶������ļ�ָ���鴦������
		if(fread(current_frame[0],XX*YY,1,fp_cur)==0)break;	//��ȡ��ǰ��֡�������ļ�
		fseek(fp_ref,XX*YY*3/2*(num+0),SEEK_SET);			//��ȡ�ο���Ƶ���е�����
		if(fread(	 ref_frame[0],XX*YY,1,fp_ref)==0)break;
		frame_info.frame_pot=frame_info.frame_sad=frame_info.frame_sse=0;		//ÿѭ��һ�鿪ʼ��Ҫ����
		for( i=0;i<X;i++)			//ע�⣡����X��ֱ�����ϣ�Y��ˮƽ������
		{
			for( j=0;j<Y;j++)
			{
				memset(_flag_search,0,SEARCH_RANGE*SEARCH_RANGE);		//�ڴ�ռ�ĳ�ʼ��
				switch (AlTypeNum)			//ѡ���㷨������
				{
				case 0:
					MessageBox(_T("��ѡ��һ���㷨��"));
					goto AYAAYA;
					break;
				case 1:
					search_FS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
				case 2:
					search_4SS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					
					break;
				case 3:
					search_BBGDS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
				case 4:
					search_DS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
				case 5:
					search_HS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
				case 6:
					search_ARPS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
				case 7:
					search_MVFAST(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
					break;
	//			case 8:
//					search_CDHS(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);
//					break;
				}
				
				rebuilt(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);		//����֡�ؽ�������i��jΪ�����ڵ�����λ��
			}
		}
		frame_info.sum_pot+=frame_info.frame_pot;		//Ӧ�����ܵĿ���
		frame_info.sum_sad+=frame_info.frame_sad;		//�ܵ�SAD
		frame_info.sum_sse+=frame_info.frame_sse;		//�ܵ�SSE          ���������������������ܵ�
		MV (*mv_tmp)[Y]=frame_info.mv;frame_info.mv=frame_info.prev_mv;frame_info.prev_mv=mv_tmp;
		memcpy(frame_info.prev_sad,frame_info.sad,X*Y*sizeof(uint32));
		tpsnr=10*log10(XX*YY*255*255.0/frame_info.frame_sse);		//����PSNR
		tcost=(float)frame_info.frame_pot/X/Y;						//�����ƽ����������
		sprintf(Ana,"%4d          %4f         %4f",num,tcost,tpsnr);
		temp = Ana;
		CString temp2;
		char abc[50]="dfdkljfldkjl";
		temp2 = Ana;
		//CStringT des(src);
		//itoa(num,cnum,10); 
		m_Analysis.AddString((LPCTSTR)temp2);
		/*CString s1;
		CString s0;
		GetDlgItem(IDC_EDIT3)->GetWindowText(s0);
		s0 += Ana;
		GetDlgItem(IDC_EDIT3)->SetWindowText(s0);*/
		UpdateData(false);
		//printf("%d\t%.2f\t%.2f\n",num,(float)frame_info.frame_pot/X/Y,10*log10(XX*YY*255*255.0/frame_info.frame_sse));
	}
	//printf("\nAvg:\t%.2f\t%.2f\n",(float)frame_info.sum_pot/X/Y/num,10*log10(XX*YY*255*255.0*num/frame_info.sum_sse));
	fclose(fp_cur);fclose(fp_ref);
	m_Cost = (float)frame_info.sum_pot/X/Y/num;
	m_PSNR = 10*log10(XX*YY*255*255.0*num/frame_info.sum_sse);
	AYAAYA: UpdateData(false);
}



void CMeDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("yuv��ʽ�ļ�(*.yuv)|*.yuv|�����ļ�(*.*)|*.*||");   
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        strFilePath = fileDlg.GetPathName();
		m_spath=strFilePath;
	    UpdateData(false);
    }   
}




void CMeDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("refyuv��ʽ�ļ�(*.refyuv)|*.refyuv|�����ļ�(*.*)|*.*||");   
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        strFilePath = fileDlg.GetPathName();
		m_spath_ref=strFilePath;
	    UpdateData(false);
    }   
}
 