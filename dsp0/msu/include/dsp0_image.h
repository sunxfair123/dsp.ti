/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_image.h
  Version       : Initial Draft V1.0
  Author        : 
  Created       : Nov 13, 2016
  Last Modified :
  Description   :DSP Init

  History       :
  1.Date        : Nov 13, 2016
    Author      : 
    Modification: Created file
******************************************************************************/
#ifndef __DSP0_IMAGE_H_
#define __DSP0_IMAGE_H_



#include <tistdtypes.h>



/*******************************************************
系统相关信息的地址
*******************************************************/
/*#define MS1	0x38000080*/
//#define dsp0_MS1_W	     0x38000080	//写缓存区
//#define dsp0_XYadd       0x380000f0 //坐标



#define dsp0_RecImageAdd   0x100008 //dsp0 接收数据地址
#define dsp0_ImageDataAdd   0x40000 //dsp0 接收数据地址

#define dsp0_EMIF16_RxAdd   0x70000000 //dsp0 接收数据地址
#define dsp0_EMIF16_TxAdd   0x70000100 //dsp0 发送数据到FPGA地址

#define dsp0_EMIF16_Tx_Time           0x70000156 //数据时标高 数据时标高
#define dsp0_EMIF16_Inquiry_Check     0x70000182//通信质询 通信质询 回复（ 回复（ 8bit bit）

#define dsp0_EMIF16_TVAdd   		  0x700001E0 //dsp0 显示通信接口
#define dsp0_EMIF16_upflagAdd  		  0x700001FA //Flag更新标识add

//#define angle_pixel   		  0.0018 //搜索用:一个pixel对应的角度偏移度数
//#define angle_2_16bit   	  182//搜索用:65536/360
//#define angle_2_32bit   	  1000//搜索用:0.001角度为1
#define pixel_2angle_32bit   	  1.8//搜索用:0.001角度为1
#define pixel_2_16bit         16 //跟踪用:一个pixel偏差为16


/*******************************************************
与图像有关的宏定义
*******************************************************/
//搜索模式图像大小640*512*16bit---原始图像
//转化成32bit，变量后缀加Addr(方便不同的寻址
//方式改写，字寻址或者字节寻址)
#define ScanWidth    640
#define ScanHeight   512
#define ScanSize     327680

//搜索模式处理图像大小 :原始图像*0.875
#define ScanW1	560
#define ScanH1	448
#define Scan1Size	250880


//跟踪模式图像大小
#define TrackWidth    640
#define TrackHeight   512
//跟踪模式处理图像大小
#define TrackW1	560
#define TrackH1	448


//原始数据右移一位，灰度最大为0x7fff
#define GrayStep  0x4000//灰度有效位宽,增加宽度可以提高检测能力，同时也增加虚警率

//代码最后用abcdef代替
//#define GrayRange  0x3fff//灰度有效位宽,增加宽度可以提高检测能力，同时也增加虚警率


//质心检测算法的边界处理范围
//代码最后用abcdef代替
//#define wCenterMin  5//质心检测算法的处理边界限制
//#define hCenterMin  10

#define wMorphlimit  10//形态滤波检测算法的处理边界限制
#define hMorphlimit  10

#define wOfflimit_C 10//质心检测算法的处理边界限制
#define hOfflimit_C  10

#define WSacnLimit 10//质心检测算法的处理边界限制
#define HSacnLimit  10

#define  Command_Scan          2
#define  Command_Track         3
#define  Command_ReCapture     7//重新捕获状态，报误差



//处理模式
#define  Mode_Scan          2
#define  Mode_Track         3
#define  Mode_Scan1     	4
#define  Mode_Scan2     	5
#define  Mode_ReCapture     7

//模式处理次数
#define ScanNum          1     
#define CaptureNum       2
#define TrackNum         20
#define Scan1Num     30
#define Scan2Num     40


//图像跟踪范围
#define TrackWindow0  0
#define TrackWindow1  1
#define TrackWindow2  2



//匹配相关需要的大小信息
#define  T0height  256//HEIGHT  
#define  T0width   320//WIDTH   	  //图像一级即原始图像
#define  T1height  256//HEIGHT
#define  T1width   160//WIDTH>>1   //图像二级(1,1/2)
#define  T2height  128//HEIGHT>>1
#define  T2width   80//WIDTH>>2	  //图像三级(1/2,1/4)

#define tarT0w   280
#define tarT0h   130//模板一级Datatemp0
#define tarT1w   140
#define tarT1h   130//模板二级Datatemp1
#define tarT2w   70
#define tarT2h   65//模板三级Datatemp2


//scan大小
//图像一级即原始图像
#define  scanW0th   416//WIDTH  
#define  scanH0th   320//  HEIGHT 	
#define  scanW1   208//WIDTH  
#define  scanH1   320//  HEIGHT 	
#define  scanW2   104//WIDTH  
#define  scanH2   160//  HEIGHT 	
//对应模板大小
#define  scanTempW0th  352
#define  scanTempH0th  160
#define  scanTempW1  176
#define  scanTempH1  160
#define  scanTempW2  88
#define  scanTempH2  80

//track图像大小
//图像零级即原始图像
#define  trackW0th   416//WIDTH  //图像零级即原始图像
#define  trackH0th   256//  HEIGHT 	
//图像一级即原始图像
#define  trackW1st   208//WIDTH>>1   //图像一级(1,1/2)
#define  trackH1st   256//  HEIGHT 	
//图像二级即原始图像
#define  trackW2nd   104//WIDTH>>2    图像二级(1/2,1/4)
#define  trackH2nd   128//HEIGHT>>1

//对应模板大小
#define  trackTempW0th  352
#define  trackTempH0th  128
#define  trackTempW1st  176
#define  trackTempH1st  128
#define  trackTempW2nd  88
#define  trackTempH2nd  64

#define  trackTempWAdd0th  176
#define  trackTempWAdd1st  88
#define  trackTempWAdd2nd  44


#define  trackTemp0Size  45056
#define  trackTemp1Size  22528
#define  trackTemp2Size  3872





#define  trackCenter1          1//质心跟踪算法，使用大津法求阈值
#define  trackMorph          2//质心跟踪算法，使用形态学法求阈值


//跟踪算法
typedef enum
{
	Center=0,					// 质心跟踪算法
	CenterMorph=1,			// 质心跟踪算法，使用形态学法求阈值
	CenterMiddle=2			// 质心跟踪算法，使用中值滤波求阈值
}TRACK_MODE;



/******************************************************************************/
//DSP与FPGA通信———信息变量定义
/******************************************************************************/
typedef struct
{
	volatile Int16 uiXangle;//目标1方位角度（16bit）
	volatile Int16 uiYangle;//目标1俯仰角度（16bit）
	volatile Int16 uiLum;//目标1亮度（16bit）
	volatile Int16 uiSize;//目标1面积（16bit）
} Target_info_t;


typedef struct
{
	volatile Int16 uiworksta;//工作状态（8bit）
	volatile Int16 uierrmsg;//故障信息（8bit）
	volatile Int16 uiTargetNum;//目标个数（8bit）	
	Target_info_t TarInfo[10];
} image_2fpga_t;


typedef struct
{
	volatile int uiXadd;//Xadd方位坐标
	volatile int uiYadd;//Yadd俯仰坐标
} video_display_t;

/******************************************************************************/
//获取目标使用的结构体
/******************************************************************************/
//分割时使用的，统计目标信息
typedef struct
{
   int x;
   int y;       
   int colMin;   
   int colMax;   
   int rowMin;
   int rowMax;
   int cnt;
   int sumgray;
}Mark4_t;

//目标捕获时临时用到的信息结构体
typedef struct
{
	int No;//在搜索到的目标ScanCheck1中的号码位置
	int offset;//和中心点的偏移距离
}Target_Temp1_t;


//跟踪目标信息结构体
typedef struct Target_Track
{
	int col;//xWidth列
	int row;//yHeight行	
	int cnt;//包含像素点个数
	int gray;
	int colsize;
	int rowsize;
	int times;//合并目标次数
	int res2;
}Target_Track_t;

typedef struct Target_Track1
{
	int col;//xWidth列
	int row;//yHeight行
	int colsize;
	int rowsize;
	int cnt;//包含像素点个数
	int gray;
	int x;
	int y;
}Target_Track_t1;


//20171123调试临时使用
typedef struct Target_Record
{	
	int TXangle;
	int TYangle;
	int col;//xWidth列
	int row;//yHeight行
	int RXangle;
	int RYangle;
	int RXanglelast;//包含像素点个数
	int RYanglelast;
}Target_Record_t;


//直方图使用
typedef	struct Target_Hist
{
	Uint32 nMostGray;
	Uint32 nMostNum;
	Uint16 lGray;
	Uint16 hGray;
	Uint32 res;	
}Target_Hist_t;

//分割时产生标号信息
typedef struct Object_Info
{	
	int cx;
	int cy;
	int nObjmark;//目标标号
	int cnt;
}Object_t;


extern volatile Int16 uiXadd;//Xadd方位坐标
extern volatile Int16 uiYadd;//Yadd俯仰坐标
extern volatile Int16 uiXaddlast;//Xadd方位坐标
extern volatile Int16 uiYaddlast;//Yadd俯仰坐标
extern volatile Int16 ScanCheck5Num;
extern volatile Int16 ScanCheck5lastNum;


extern Target_Track_t FPGACheck;//上报FPGA目标位置
extern Target_Track_t FPGAChecklast;//上报FPGA目标位置


extern  Int16  dsp0_updateflag;
extern  int  dsp0_updateflag1;
 
//debug参数，用于调试，调试完成后，删除
extern  int Debug_WorkMode;
//debug参数，用于调试，调试完成后，删除


//控制流程变量:中断使用
extern volatile Int16 CommandMode;
extern volatile int RxFrameFlag;//图像接收标记,1为接收完成

//控制流程变量:处理函数使用
extern int WorkMode;
extern int SmallFlag;//用于区分大小目标搜索
extern int dspselfcheck;//DSP自检,1是自检
extern int CheckCycles;//检测目标次数



//scan参数
extern  int ScanCheckflag;//用于标识使用哪种scan算法

//Capture参数
extern  int capturecycles;//捕获次数
extern  int capturelost;//捕获丢失次数
extern  int capturecheckflag;//用于标识使用哪种捕获算法

//跟踪参数
extern  volatile int TrackMethod;         //1:第一种跟踪算法,2:第二种跟踪算法,3:融合算法
extern  volatile int TrackM1;         //质心跟踪算法使用哪种方法求阈值
extern  volatile int TrackM2;         //模板匹配算法使用哪种方法求阈值
extern  volatile int TrackM2Flag;     //当算法由其它算法变成模板匹配时，置1
extern  volatile int TrackM3;         //混合算法使用哪种方法求阈值

//跟踪控制参数
extern  int tracklostflag;//每次跟踪丢失标识
extern  int tracklostNum;//跟踪丢失帧数
extern  int tracklostmax;//跟踪丢失允许的最大帧数
extern  int trackcnt;//连续跟踪次数，用来判断是否连续跟踪

//跟踪目标信息参数
extern  Target_Track_t trackInfo;//记录当前跟踪信息
extern  Target_Track_t pretrackInfo;//上一次的跟踪信息
extern  Target_Track_t trackseq[10];

extern  int track1flag;
extern  int track2flag;
extern  int temptrackflag;//用于标识track跟踪结果，1代表有跟踪目标
extern  Target_Track_t track1Info;//算法1的跟踪信息
extern  Target_Track_t track2Info;//算法2的跟踪信息
extern  Target_Track_t temptrackInfo;//

  
extern  float TempCorRmax;//匹配系数，用于主函数中对匹配可信度的判断及模板更新

//20170516 add
extern  Target_Hist_t m_Histogram;






//function 定义
void debugDSP0_image(void);
void DSP0_image(void);
void global_parameters_init(void);

void dsp0_scan(void);
inline void AcquireData(Int16 *pRecImage,int iworkMode,Int16 *pImageData);
void Target_scan1(int iworkmode);
void Morphscan3(int iworkmode);




inline void dsp0_capture(void);
void target_capture(void);
void TarManualCapture(void);






void dsp0_track(void);
void window0_track(void);
void window1_track(void);
void window2_track(void);
void dsp0_window1_track(void);
void dsp0_window2_track(void);

void Bright_ObjTrack(int uiworkmode,int uismallflag);
void MorphologicTrack(int uiworkmode,int uismallflag);



extern void FPGA_EMIF16_RTX();




#endif
