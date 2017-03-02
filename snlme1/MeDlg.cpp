// MeDlg.cpp : 实现文件
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
////////////以下是运动估计的相关定义////////
typedef int  int32;
typedef short  int16;
typedef char  int8;
typedef unsigned short  uint16;
typedef unsigned int  uint32;
typedef unsigned char  uint8;

#if 1
#define XX 288
#define YY 352
#define OPEN_FILE "E:\\Yun\\学习\\毕设\\视频图像\\foreman.yuv"
#define REF_FILE  "E:\\Yun\\学习\\毕设\\视频图像\\foreman_ref.yuv"
#else
#define XX 144		//XX为视频的高度
#define YY 176		//YY为视频的宽度
#define OPEN_FILE "E:\\Yun\\学习\\毕设\\视频图像\\foreman.yuv"
#define REF_FILE  "E:\\Yun\\学习\\毕设\\视频图像\\foreman_ref.yuv"
#endif
#define BLOCK_HEIGTH 16			//宏块的高度
#define BLOCK_WIDTH 16			//宏块的宽度
#define MAX_MOTION 16			//最大位移？？？？？？？！！
const int SEARCH_RANGE=MAX_MOTION*2+1;		//搜索范围是最大位移乘以2加1，不晓得是啥意思，看看代码先
const int X=XX/BLOCK_HEIGTH;		//视频的高度/宏块的高度得到在竖直方向上的数目
const int Y=YY/BLOCK_WIDTH;			//水平方向上的数目

typedef struct{
	int16 dx;
	int16 dy;
}MV;		//定义运动矢量结构体

uint8 _flag_search[SEARCH_RANGE][SEARCH_RANGE];		//最大搜索范围？？？
uint8 (*const flag_search)[SEARCH_RANGE]=
	(uint8 (*)[SEARCH_RANGE])&_flag_search[MAX_MOTION][MAX_MOTION];		//划定搜索范围？？

uint8 current_frame[XX][YY],ref_frame[XX][YY];		//定义了两个帧，当前帧和参考帧

MV _mv_buffer[2][X][Y];				//mv缓冲区

struct{
	MV (*mv)[Y];			//运动矢量
	MV (*prev_mv)[Y];	//预测运动矢量
	uint32 sad[X][Y];		//每一块的sad
	uint32 prev_sad[X][Y];	//每一块的前sad
	uint32 frame_sad;		//帧总的SAD
	uint32 sum_sad;		//sad的和
	uint32 frame_pot;		//帧中块的数目
	uint32 sum_pot;		//所有帧块的总数
	uint32 frame_sse;		//帧的MSE
	uint32 sum_sse;		//所有帧的MSE的和
}frame_info;				//定义了帧信息结构体

FILE *fp_cur,*fp_ref;		//用来存储读取的视频文件的指针
///////////////end//////////////////////////
//下面是各个运动估计各个算法函数的定义等
#define MIN(a,b) ((a)<(b))?(a):(b);		//宏定义，得到ab中最小的数字
#define MAX(a,b) ((a)>(b))?(a):(b);		//宏定义，得到ab中最大的数字

//SAD函数实现了SAD的计算，输入ox表示数值方向上的偏置，oy表示水平方向上的偏置，dx、dy表示步长？？，返回值best_sad
int SAD(const int ox,const int oy,const int dx,const int dy,const int height,const int width,uint32 &best_sad)
{
	const int rx=ox+dx,ry=oy+dy;
	if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION || flag_search[dx][dy]>0 )		//步长是否大于最大步长，有问题就返回
		return 0;
	if( rx<0 || ry<0 || rx+height>XX || ry+width>YY )			//判断是否超出了搜索范围
		return 0;
	uint32 sad=0;
	frame_info.frame_pot++;		//每算一次sad，frame_pot加一
	const uint8 *p1=&current_frame[ox][oy],*p2=&ref_frame[rx][ry];
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			sad+=abs(*(p1++)-*(p2++));			//计算残差的绝对值
		}
		p1+=(YY-width);p2+=(YY-width);			//移动
	}
	flag_search[dx][dy]=1+sad;					
	if(sad>=best_sad)
		return 0;
	best_sad=sad;		//得到了最佳SAD
	return 1;
}

void rebuilt(const int x,const int y,const int height,const int width)	//帧重建函数，x、y分别代表块所在的行列的位置
{
	const int dx=frame_info.mv[x][y].dx,dy=frame_info.mv[x][y].dy;		//得到该帧的运动矢量
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;						//得到偏置
	const int rx=ox+dx,ry=oy+dy;										//将块移到的位置，即重建块的位置
	int tmp;															//临时变量
	const uint8 *p1=&current_frame[ox][oy],*p2=&ref_frame[rx][ry];		//读取的当前帧和参考帧
	if( abs(dx)>MAX_MOTION || abs(dy)>MAX_MOTION )						//判断是否超出范围
		exit(0);
	if( rx<0 || ry<0 || rx+height>XX || ry+width>YY )					//判断是否溢出
		exit(0);
	for(int i=0;i<height;i++)											
	{
		for(int j=0;j<width;j++)
		{
			tmp=*(p1++)-*(p2++);										//获得残差
			frame_info.frame_sse+=tmp*tmp;								//这是残差的平方
		}
		p1+=(YY-width);p2+=(YY-width);									//内存地址加
	}
}
//全搜索算法，其中x表示竖直方向上的块的序数，y表示水平方向上的块的序数，height表示宏块高度，width表示宏块宽度
void search_FS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;		//ox表示竖直方向上的偏置像素数，oy表示水平方向上的偏置像素数
	//const int ox=x*BLOCK_HEIGTH*2,oy=y*BLOCK_WIDTH*2;	
	uint32 sad=0xffffff;		//初始有符号为0
	MV mv;
	for(int i=-MAX_MOTION;i<=MAX_MOTION;i++)
	{
		for(int j=-MAX_MOTION;j<=MAX_MOTION;j++)			//为什么这么搜呢？因为这是全搜索算法 ，嗯yes
		{
			if(SAD(ox,oy,i,j,heigth,width,sad)==1)			//输入数值方向上的编制像素数，水平方向上的偏置像素数，i、j分别是像素位置，heigth,width分别是宏块的高度和宽度
			{												//sad表示最好的sad，mv为运动矢量
				mv.dx=i;mv.dy=j;
			}
		}
	}
	frame_info.mv[x][y]=mv;		//运动矢量
	frame_info.sad[x][y]=sad;	//最小的sad
	frame_info.frame_sad+=sad;	//帧总的sad
}
//////宏定义函数由于C语言宏定义只有一行故使用"\"表示转入下一行//这是搜索模板的宏定义，pattern表示搜索模板的名称，num表示搜索点数，flag？？
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

//四步搜索算法
void search_4SS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int L4SS[9][2]={{0,0},{0,2},{-2,2},{-2,0},{-2,-2},{0,-2},{2,-2},{2,0},{2,2}};		//定义了半径为2的大正方形模板
	const int S4SS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};		//定义了半径为1的小正方形模板
	uint32 sad=0xffffff;		//表示0
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(L4SS,9,1)
	PATTERN_SEARCH(S4SS,9,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

//梯度下降搜索算法
void search_BBGDS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int BBGDS[9][2]={{0,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};		//半径为1的正方形搜索模板
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(BBGDS,9,1)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
//菱形搜索算法
void search_DS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};		//大菱形模板
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};					//小菱形模板
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;
	//利用模板进行搜索
	PATTERN_SEARCH(LDS,9,1)		//搜索一次
	PATTERN_SEARCH(SDS,5,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

//六边形搜索算法
void search_HS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LHS[7][2]={{0,0},{0,2},{-2,1},{-2,-1},{0,-2},{2,-1},{2,1}};		//大六边形模板
	const int SHS[9][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0},{1,1},{1,-1},{-1,1},{-1,-1}};		//小六边形模板
	uint32 sad=0xffffff;
	MV mv={0,0};int mvx,mvy;

	PATTERN_SEARCH(LHS,7,1)
	PATTERN_SEARCH(SHS,9,0)

	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}
//宏定义，由于C语言的宏定义只有一行，用\表示转入下一行
#define CHECK_ONE_PIXEL(i,j) \
	if(SAD(ox,oy,i,j,heigth,width,sad)==1)\
	{\
		mv.dx=i;mv.dy=j;\
	}
//取得a,b,c的中间值
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

//获取常用的预测运动矢量列表，用于ARPS搜索
void Get_MVp(const int x,const int y,MV *pre_mv,int &mvx,int &mvy,uint32 *sad=NULL)
{
	uint32 num[10];if(sad==NULL) sad=num;
	/***************D  B  C*****************/
	/***************A  E   *****************/
	if(y>0)		//注意！！！x代表纵坐标，y代表横坐标
	{
		pre_mv[0]=frame_info.mv[x][y-1];
		sad[0]=frame_info.sad[x][y-1];	//存放A，MVA
	}
	else
	{
		pre_mv[0].dx=pre_mv[0].dy=0;
		sad[0]=0;
	}
	if(x>0)
	{
		pre_mv[1]=frame_info.mv[x-1][y];
		sad[1]=frame_info.sad[x-1][y];		//存放B MVB
	}
	else
	{
		pre_mv[1].dx=pre_mv[1].dy=0;
		sad[1]=0;
	}
	if(x>0 && y<Y-1)
	{
		pre_mv[2]=frame_info.mv[x-1][y+1];
		sad[2]=frame_info.sad[x-1][y+1];		//存放C MVC
	}
	else if(x>0)			///x>0,y=Y-1//到达右边界
	{
		pre_mv[2]=frame_info.mv[x-1][y-1];	
		sad[2]=frame_info.sad[x-1][y-1];	//存放C用MVB代替 MVC
	}
	else		//x<=0 
	{
		pre_mv[2].dx=pre_mv[2].dy=0;
		sad[2]=0;
	}
	if(x>0&&y>0)
	{
		pre_mv[3]=frame_info.mv[x-1][y-1];
		sad[3]=frame_info.sad[x-1][y-1];		//存放D MVD
	}
	else
	{
		pre_mv[3].dx=pre_mv[3].dy=0;
		sad[3]=0;
	}

	pre_mv[4]=frame_info.prev_mv[x][y];		//存放以前的信息MVE
	sad[4]=frame_info.prev_sad[x][y];		

	pre_mv[5].dx=2*frame_info.prev_mv[x][y].dx-frame_info.mv[x][y].dx;
	pre_mv[5].dy=2*frame_info.prev_mv[x][y].dy-frame_info.mv[x][y].dy;
	//本程序没有用到这个量 //注意!这里frame_info.mv[x][y]因为当前帧并没编码故代表上上帧的运动矢量
	if(x==0)
	{
		mvx=pre_mv[0].dx;
		mvy=pre_mv[0].dy;
		return;
	}
	mvx=x264_median(pre_mv[0].dx,pre_mv[1].dx,pre_mv[2].dx);
	mvy=x264_median(pre_mv[0].dy,pre_mv[1].dy,pre_mv[2].dy);
}

//自适应十字搜索算法
void search_ARPS(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int SCS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};		//小十字模板
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;
	const uint32 T=512;
	//判断前一帧相同位置是不是最佳匹配块
	CHECK_ONE_PIXEL(0,0)		
	if(sad<T)	goto END;
	{
		Get_MVp(x,y,pre_mv,mvx,mvy);	//利用临近块的MV
		int Length=abs(pre_mv[0].dx)>abs(pre_mv[0].dy)?abs(pre_mv[0].dx):abs(pre_mv[0].dy);
		CHECK_ONE_PIXEL(mvx,mvy)
		CHECK_ONE_PIXEL( Length,0)
		CHECK_ONE_PIXEL(-Length,0)
		CHECK_ONE_PIXEL(0, Length)
		CHECK_ONE_PIXEL(0,-Length)//遍历与小十字模板相对应的的各个位置
	}//遍历与小十字模板相对应的的各个位置
	PATTERN_SEARCH(SCS,5,1)
END:
	frame_info.mv[x][y]=mv;
	frame_info.sad[x][y]=sad;
	frame_info.frame_sad+=sad;
}

////以下搜索算法没有在本毕设中使用到
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
{///确定搜索半径令L=max{ |mv.x| +|mv.y| } mv∈{MVA、MVB、MVC}
	if(x==0&&y==0)
		return 2;
	int L=0,num;
	num=abs(pre_mv[0].dx-mvx)+abs(pre_mv[0].dy-mvy);
	if(L<num)L=num;
	num=abs(pre_mv[1].dx-mvx)+abs(pre_mv[1].dy-mvy);
	if(L<num)L=num;
	num=abs(pre_mv[2].dx-mvx)+abs(pre_mv[2].dy-mvy);
	if(L<num)L=num;
	return L;	//Lmax = Max(L(MVA), L(MVB), L(MVC))代表MV搜索半径的界限
}

//运动场矢量自适应搜索算法
void search_MVFAST(const int x,const int y,const int heigth,const int width)
{
	const int ox=x*BLOCK_HEIGTH,oy=y*BLOCK_WIDTH;
	const int LDS[9][2]={{0,0},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1},{2,0},{1,1}};	//大菱形模板
	const int SDS[5][2]={{0,0},{0,1},{-1,0},{0,-1},{1,0}};		//小菱形模板
	uint32 sad=0xffffff;
	MV mv={0,0},pre_mv[10];int mvx,mvy;
	const uint32 T=512,L1=1,L2=2;		//一个T,L1,L2代表MV搜索半径的界限
	
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
}////以下搜索算法没有在本毕设中使用到
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


// CMeDlg 对话框
CString FSIntro(_T("        FS又被称为穷尽搜索算法，该算法的基本思路如下：算法对搜索范围((M+2dxmax)×(N+2dymax))内所有的可能候选位置计算MAD/SAD，其从中选取最小的值，相对应的偏移量即为MV。\n算法描述如下（参见论文中图2-1）：\n第一步，从搜索原点出发，按照顺时针螺旋方向从近到远进行遍历，计算出所有位置的MAD/SAD；\n第二步，从计算结果中选取最小的值，同时比较原点可以取得MV。\n该算法形式简单，效果最好，但是计算量较大，计算所需时间较长。虽然FS的结果是全局最优的，但是由于其缺点也十分显著，所以通常在编解码时我们不使用该算法，FS一般作为其它搜索算法的对照。"));
CString _4SSIntro(_T("        FSS与TSS相类似，算法中考虑了宏块的中心偏置特性，同时兼顾了物体的小范围移动和大范围移动，在这两种情况下均能取得较好的效果。该算法的基本思路如下：与算法采用了5×5的搜索窗口，每一步的搜索窗口大小由上一步的最佳匹配点来决定，并且将搜索原点移至上一步的最佳匹配点，前三步的搜索是定步长搜索，最后一步搜索改变步长，从而得到最后的最佳匹配点。算法描述如下（参见论文中图2-3）：第一步，设置搜索起点，以其为原点划定搜索窗口，对中心点以及按照一定的步长设置的搜索窗口正方形范围边缘8个点计算SAD，如果中心点的SAD最小，则跳到第四步。第二步，搜索窗口大小不变，搜索中心点移至上一步得到的最佳匹配点。如果该点位于上一步搜索窗口的四个角点上，则对其余5个点进行匹配运算；如果该点位于上一步搜索窗口的四个顶点上，则对其余3个点进行匹配运算。第三步，取得上一步的最佳匹配点，重复第二步的步骤。第四步，缩小搜索窗口，计算SAD，得到最佳匹配点即可得MV。该算法与TSS相比，其搜索幅度较为平缓，复杂度较低，具有更强的鲁棒性，同时与FS相比也大大减少了计算量。"));
CString DSIntro(_T("        DS算法的基本思路如下：为了防止搜索窗口过大或过小所产生的不良影响，DS算法采用了两个不同的搜索模板，大模板拥有9个检测点，小模板拥有5个检测点。搜索时先利用大模板进行计算，当最佳匹配点位于中心点时，启用小模板，再进行搜索，得到最终的最佳匹配点。算法描述如下（参见论文中图2-5）：第一步，利用大模板的进行搜索，计算搜索窗口中心以及四周8个点处的SAD，取最小得到最佳匹配点，如果最佳匹配点为中心点，则进行第三步，否则进行第二步。第二步，将第一步得到的最佳匹配点设为中心点，利用大模板进行搜索，重复第一步的相关步骤。第三步，将第一步得到的最佳匹配点设为中心点，切换到小模板进行搜索，计算窗口中心点以及四周4个点处的SAD，取最小得到最终的最佳匹配点，同时得到MV。该算法分析并利用了视频图像中物体运动的基本规律，采用了两种大小不同的模板进行搜索，先利用大模板进行粗略定位，再利用小模板进行精确定位，既不会陷入局部最优，也保证了搜索的精度，同时也提高了搜索的速度。"));
CString HSIntro(_T("        HS算法的基本思路如下：为了克服DS算法中最佳匹配点选择的矛盾，引出了HS算法，该算法可以使搜索点均匀地分布在以中心搜索点为圆心的圆周上，从而使搜索结果更加优化。算法定义了两个搜索模板，一个大六边形模板和一个小六边形模板，如论文中图2-6(a)与图2-6(b)所示。搜索时先使用大六边形模板，若SAD最小点位于中心点时启用小六边形模板。算法描述如下：第一步，使用大六边形模板进行搜索，计算搜索区域中心点以及六个定点处的SAD，获得最小的SAD及其所在位置，若最佳匹配点不在中心处，跳至第三步，否则进行第二步。第二步，以上一步获得的最佳匹配点为本步的中心点，用大六边形模板进行搜索，找到最佳匹配点，如果最佳匹配点不在中心点，跳至第三步，否则重复第二步。第三步，启用小六边形模板进行搜索，找到SAD最小的最佳匹配点，即得到该宏块的MV。该算法在继承了DS算法的优点的前提下，减少了搜索步数，同时也不会陷入局部最优，提高了搜索速度和精度。"));
CString ARPSIntro(_T("        ARPS的基本思路如下：利用当前搜索块周围的块的运动趋势来对当前搜索块的运动趋势进行预测，一般来说采用的是左侧块的运动趋势，得到该块的MV。该算法主要包含两个步骤，第一个是自适应初步搜索，第二个是精确的局部搜索。在初步搜索中，构造一个十字型的搜索模板，设置该搜索模板的搜索步长为当前搜索块左侧块MV在竖直和水平分量中较大的一个，搜索点分别是十字型中心点和四个顶点。算法描述如下：第一步，计算该搜索点本身的预测MV，如果该点的SAD少于设置的阈值，那么跳到第三步。第二步，搜索十字型的四个顶点，得到SAD最小的最佳匹配点。第三步，进行局部精确搜索，搜索模板切换为小菱形，不断进行搜索。该算法能够根据前一个块的MV有效地预估当前块的搜索范围，相对于DS算法在计算速度上有了显著的提升。"));
CString BBGDSIntro(_T("        BBGDS的基本思路如下：与上述TSS和FSS不同，该算法只是采用了一个搜索步长最小的模板不断地进行搜索，直至找到的SAD最小的点位于搜索模板的中心或者达到了搜索窗口的边缘时才停止。类似于FSS的某些搜索步骤，BBGDS的后续的每个搜索步骤均增加3到5个搜索点。算法描述如下（参见图2-4）：第一步，利用搜索模板进行搜索，计算搜索窗口中心以及四周8个点出的SAD，取最小得到最佳匹配点，如果最佳匹配点为中心点，则结束，得到MV，否则进行第二步。第二步，以上一步得到的最佳匹配点为中心点，利用相同的搜索模板进行搜索，本次需要搜索的点数在3到5个，如果最佳匹配点为中心点，则结束，得到MV，否则重复本步。该算法尤其适合运动量较小的场合。在每一步搜索过程中，该算法使用的不是匹配块而是中心匹配块，这进一步降低了陷入局部最优的可能。搜索方向由梯度下降的方向决定，从而可以对该方向进行重点搜索，这就减少或者避免了不必要的搜索，很大程度上降低了算法的复杂度。"));
CString MVFASTIntro(_T("        MVFAST算法的核心是对运动模式（MA, Motion Activity）的判定，其搜索模板与DS算法相似，仍然采用大小两个菱形模板，基本思路如下：①对视频帧图像进行预搜索，通过预搜索消除静止块。②确定局部运动量，即通过该宏块周围的若干个（本算法中为3）个宏块的MV来确定该宏块的运动模式，分别设周围三个宏块的MV为V1，V2，V3，其中Vi=(Xvi,Yvi)，Xvi与Yvi分别为Vi的水平和竖直分量，并设Lvi=|Xvi|+|Yvi|，L1=1，L2=2，取L=MAX(Lvi)，对所有的Vi，当前宏块的运动方式定义如下：若L<=L1，则MA=LOW；若L1<L<L2，则MA=MEDIUM；若L1<L2<L，则MA=HIGH。③确定搜索中心，如果MA为LOW或者MEDIUM，那么搜索中心取为原点，如果MA为HIGH，那么搜索中心取产生SAD最小值的矢量。④选择搜索策略，如果MA为LOW或者HIGH，那么采用小菱形模板，如果MA为MEDIUM，先采用大菱形模板，再采用小菱形模板进行搜索。算法描述与算法思路相同，不赘述。由于MVFAST算法预先去除了MV为零或极小的块，同时视频帧图像中很多块的MV为零，所以该算法大大减少了不必要的计算量。同时该算法对运动场矢量进行了预测，定义了不同的MA，并根据MA选取不同的搜索模板，也使得搜索更加精确。"));
//CString CDHSIntro(_T("        蝶形运动估计算法简介"));
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
	/*m_SelAl.AddString(_T("1.全搜索算法（FS）"));
	m_SelAl.AddString(_T("2.四步搜索算法（4SS）"));
	m_SelAl.AddString(_T("3.梯度下降搜索算法（BBGDS）"));
	m_SelAl.AddString(_T("4.菱形搜索算法（DS）"));
	m_SelAl.AddString(_T("5.六边形搜索算法（HS）"));
	m_SelAl.AddString(_T("6.自适应十字搜索算法（ARPS）"));
	m_SelAl.AddString(_T("7.运动矢量场自适应搜索算法（MVFAST）"));
	m_SelAl.AddString(_T("8.蝶形运动估计算法（CDHS）"));*/
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


// CMeDlg 消息处理程序


void CMeDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void CMeDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString AlType;  
    int nSel;     

    // 获取组合框控件的列表框中选中项的索引   
    nSel = m_SelAl.GetCurSel();   
    // 根据选中项索引获取该项字符串   
    m_SelAl.GetLBText(nSel, AlType);  
	if(AlType == "1.全搜索算法（FS）")
	{
		AlTypeNum = 1;
		m_AlIntro=FSIntro;
	}
	/*else if(AlType == "2.四步搜索算法（FSS）")
	{
		AlTypeNum = 2;
		m_AlIntro=_4SSIntro;
	}
	else if(AlType == "3.梯度下降搜索算法（BBGDS）")
	{
		AlTypeNum = 3;
		m_AlIntro=BBGDSIntro;
	}*/
	else if(AlType == "2.菱形搜索算法（DS）")
	{
		AlTypeNum = 4;
		m_AlIntro=DSIntro;
	}
	else if(AlType == "3.六边形搜索算法（HS）")
	{
		AlTypeNum = 5;
		m_AlIntro=HSIntro;
	}
	/*else if(AlType == "6.自适应十字搜索算法（ARPS）")
	{
		AlTypeNum = 6;
		m_AlIntro=ARPSIntro;
	}*/
	else if(AlType == "4.运动矢量场自适应搜索算法（MVFAST）")
	{
		AlTypeNum = 7;
		m_AlIntro=MVFASTIntro;
	}
//	else if(AlType == "8.蝶形运动估计算法（CDHS）")
//	{
//		AlTypeNum = 8;
//		m_AlIntro=CDHSIntro;
//	}
	else if(AlType == "")
	{
		AlTypeNum=0;
		m_AlIntro="请选择相应的运动估计算法";
	}

	UpdateData(false);

	
}
void useAl(int Alnum)
{
	
}

void CMeDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//useAl(AlTypeNum);
	m_Analysis.ResetContent();
	m_Analysis.AddString(_T("帧数        帧间块均COST        PSNR"));
	char Ana[80];
	char cnum[3];
	float tpsnr,tcost;
	CString temp;
	int num;
	int j=0;
	if((fp_cur = fopen((CW2A)m_spath,"rb"))==NULL)	MessageBox(_T("加载视频序列时出现问题"));	//读取视频序列
	//if((fp_ref = fopen( REF_FILE,"rb"))==NULL)	return;

	//CString m_spath_ref = sprintf()
	if((fp_ref = fopen( (CW2A)m_spath,"rb"))==NULL)	return;		//读取参考序列
	frame_info.sum_pot=frame_info.sum_sad=frame_info.sum_sse=0;		//先全部置零
	frame_info.mv=_mv_buffer[0];		//mv_buffer应该是mv的缓存区
	frame_info.prev_mv=_mv_buffer[1];	//预测mv
	//printf("frame\tcost\tpsnr\n");
	for(num=0;num<100;num++)
	{
		int i,j;
		fseek(fp_cur,XX*YY*3/2*(num+1),SEEK_SET);	//读取待进行运动估计文件指定块处的数据
		if(fread(current_frame[0],XX*YY,1,fp_cur)==0)break;	//读取当前首帧的数据文件
		fseek(fp_ref,XX*YY*3/2*(num+0),SEEK_SET);			//读取参考视频序列的数据
		if(fread(	 ref_frame[0],XX*YY,1,fp_ref)==0)break;
		frame_info.frame_pot=frame_info.frame_sad=frame_info.frame_sse=0;		//每循环一遍开始都要置零
		for( i=0;i<X;i++)			//注意！！！X竖直方向上，Y是水平方向上
		{
			for( j=0;j<Y;j++)
			{
				memset(_flag_search,0,SEARCH_RANGE*SEARCH_RANGE);		//内存空间的初始化
				switch (AlTypeNum)			//选择算法的类型
				{
				case 0:
					MessageBox(_T("请选择一个算法！"));
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
				
				rebuilt(i,j,BLOCK_HEIGTH,BLOCK_WIDTH);		//进行帧重建，输入i、j为块所在的行列位置
			}
		}
		frame_info.sum_pot+=frame_info.frame_pot;		//应该是总的块数
		frame_info.sum_sad+=frame_info.frame_sad;		//总的SAD
		frame_info.sum_sse+=frame_info.frame_sse;		//总的SSE          以上三个都是用来算性能的
		MV (*mv_tmp)[Y]=frame_info.mv;frame_info.mv=frame_info.prev_mv;frame_info.prev_mv=mv_tmp;
		memcpy(frame_info.prev_sad,frame_info.sad,X*Y*sizeof(uint32));
		tpsnr=10*log10(XX*YY*255*255.0/frame_info.frame_sse);		//计算PSNR
		tcost=(float)frame_info.frame_pot/X/Y;						//计算块平均搜索点数
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
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("yuv格式文件(*.yuv)|*.yuv|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName();
		m_spath=strFilePath;
	    UpdateData(false);
    }   
}




void CMeDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("refyuv格式文件(*.refyuv)|*.refyuv|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName();
		m_spath_ref=strFilePath;
	    UpdateData(false);
    }   
}
 