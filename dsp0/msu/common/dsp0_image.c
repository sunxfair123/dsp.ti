/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_image.c
  Version       : Initial Draft V1.0
  Author        : 
  Created       : Nov 13, 2016
  Last Modified :
  Description   :用于小目标的算法

  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

******************************************************************************/


//************************* Includes ************************************
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dsp0_common.h"
//#include "dsp0_section.h"
#include "dsp0_secextern.h"
#include "dsp0_imagefunc.h"


extern volatile Int16 targetempty;

volatile Int16 uiXadd;//Xadd方位坐标
volatile Int16 uiYadd;//Yadd俯仰坐标
volatile Int16 uiXaddlast;//Xadd方位坐标
volatile Int16 uiYaddlast;//Yadd俯仰坐标
volatile Int16 ScanCheck5Num;
volatile Int16 ScanCheck5lastNum;





Int16 uiXadd1;//Xadd方位坐标
Int16 uiYadd1;//Yadd俯仰坐标

Int16  dsp0_updateflag;
int  dsp0_updateflag1;


//debug参数，用于调试，调试完成后，删除
int Debug_WorkMode;
//debug参数，用于调试，调试完成后，删除



//控制流程变量:中断使用
volatile Int16 CommandMode;
volatile Int16 modeswitch;
volatile int RxFrameFlag;//图像接收标记,1为接收完成

//控制流程变量:处理函数使用
int WorkMode;
int SmallFlag;//用于区分大小目标搜索,1是小目标
int dspselfcheck;//DSP自检,1是自检
int CheckCycles;//检测目标次数







//scan参数
int ScanCheckflag;//用于标识使用哪种scan算法

//Capture参数
int capturecycles;//捕获次数
int capturelost;//捕获丢失次数
int capturecheckflag;//用于标识使用哪种捕获算法


//跟踪参数
volatile int TrackMethod;         //1:第一种跟踪算法,2:第二种跟踪算法,3:融合算法
volatile int TrackM1;         //质心跟踪算法使用哪种方法求阈值
volatile int TrackM2;         //模板匹配算法使用哪种方法求阈值
volatile int TrackM2Flag;     //当算法由其它算法变成模板匹配时，置1
volatile int TrackM3;         //混合算法使用哪种方法求阈值

//跟踪控制参数
int tracklostflag;//每次跟踪丢失标识
int tracklostNum;//跟踪丢失帧数
int tracklostmax;//跟踪丢失允许的最大帧数
int trackcnt;//连续跟踪次数，用来判断是否连续跟踪

//跟踪目标信息参数
Target_Track_t trackInfo;//记录当前跟踪信息
Target_Track_t pretrackInfo;//上一次的跟踪信息
Target_Track_t trackseq[10];

int track1flag;
int track2flag;
int temptrackflag;//用于标识track跟踪结果，1代表有跟踪目标
Target_Track_t track1Info;//算法1的跟踪信息
Target_Track_t track2Info;//算法2的跟踪信息
Target_Track_t temptrackInfo;//


float TempCorRmax;//匹配系数，用于主函数中对匹配可信度的判断及模板更新



//dsp0 相关变量
float Kconstant=5.0;//5.0


float KKK=4.0;//

//中值滤波使用
//debug调试需要修改
float K4_scan=3.9;//5.0
float K4_track=3.0;//
float K4_scan1=3.9;//5.0
float K4_scan1con=0.1;

/*存放中间结果*/
#pragma DATA_SECTION(m_fLineMean,"core0_Data.SL2")
int m_fLineMean[512];
#pragma DATA_SECTION(m_fLineFcha,"core0_Data.SL2")
int m_fLineFcha[512];
#pragma DATA_SECTION(m_fDela,"core0_Data.SL2")
int m_fDela[512];


#pragma DATA_SECTION(pseuobject2,"core0_Data.SL2")
Object_t pseuobject2[512];
#pragma DATA_SECTION(pseuobject,"core0_Data.SL2")
Object_t pseuobject[1152];






float MultiM2=4.5;

float Multiply1=0.12;
float Multiply2=1.3;


//Pre_MeanFilter 去除最大值使用的参数
//float K1st_Mean=4;
//float K1st_Fcha=1.6;

//float TotalMean=4;
//float TotalFcha=2.2;

//float K1st_Mean1=4;
//float K1st_Fcha1=1.2;

//中值滤波使用的参数
float K1st_Mean=5;
float K1st_Fcha=1.8;

float TotalMean=7.0;
float TotalFcha=2.8;

float K1st_Mean1=5;
float K1st_Fcha1=1.8;







int nW_K1=6;//目标点区域1大小
int nH_K1=6;
//int nW_K2=5;//目标点区域2大小(范围小于区域1)
//int nH_K2=5;
int nW_K21=4;//目标点区域2大小(范围小于区域1)
int nH_K21=4;

int nW_K3=8;//目标点区域3大小
int nH_K3=8;
int offsetPixelX_K1=3;
int offsetPixelY_K1=3;
float K1_Mean=1.0;
float K1_Fcha=0.9;
float K1_Compensate1=0.1;
float K2_Fcha=0.8;

float K1_Mean4=1.2;
float K1_Fcha4=0.4;


int offsetPixelX3=6;
int offsetPixelY3=6;
int offsetPixelX31=7;
int offsetPixelY31=7;

float K1_Compensate3=0.1;


int nW_K2=7;//目标点区域2大小(范围小于区域1)
int nH_K2=7;
//int offsetPixelX3=4;
//int offsetPixelY3=4;









int npixelcnt;//图像分割pixel个数
int npixcntlimit1=2;//合并后目标最小像元个数
int npixcntlimit1_1=2;//第一次合并后目标最小像元个数
int pixlimit=2;

//下面参数是否改为2，根据实际情况确定
int npixcntlimit2_3=3;//
int npixcntlimit2=3;//
int npixcntlimit=3;//合并大目标使用
int nthCNTlimit=3;



int det1st=3;
int det1st1=5;
int det1st2=11;

int det3rd1=3;
int det3rd=5;
int det2nd=20;


float mthreshratio1=0.65;
float mthreshratio2=1.1;
float mthreshratio3=0.9;
float mthreshratio4=1.0;
float mthreshratio5=1.8;
float mthreshratio5ftr=0.8;

float m4threshratio1=0.9;
float m4threshratio2=1.1;
float m4threshratio3=0.9;
float m4threshratio4=1.0;
float m4threshratio5=1.8;
float m4threshratio5ftr=0.8;



//下面参数是否改为0.6,0.9，根据实际情况确定
float mthreshratio6=0.75;//0.6
float mthreshratio7=1.0;//0.9
float mthreshratio8=1.0;//0.9








float para1thresh1=0.52;
float para1thresh2=0.74;
float para1thresh3=0.88;
float para3n1=2;
float para3n2=6;

int para1num1;
int para1num2=9;
int para1num3=18;

float para2thresh0=0.52;
float para2thresh1=0.65;
float para2thresh2=0.74;
float para2thresh3=0.90;


int para2num1=8;
int para2num2=12;
int para2num3=18;

float para3thresh1=0.52;
float para3thresh2=0.74;
float para3thresh3=0.88;
int para3num1;
int para3num2=8;
int para3num3=18;


float para4thresh0=0.52;
float para4thresh1=0.65;
float para4thresh2=0.74;
float para4thresh3=0.90;
int para4num1=7;
int para4num2=12;
int para4num3=18;


float para5th1=0.52;
float para5th2=0.60;
float para5thresh0=0.52;
float para5thresh1=0.78;
float para5thresh2=0.74;
float para5thresh3=0.88;
float para5add1=1.1;
int para5num1=9;
int para5num2=40;
int para5num3=18;









float para2thresh[25]={
	0.52,0.56,0.61,0.65,0.70,
	0.52,0.56,0.61,0.65,0.70,	
	0.52,0.56,0.61,0.65,0.70,
	0.52,0.56,0.61,0.65,0.70,
	0.52,0.56,0.61,0.65,0.70
};












extern volatile Uint8 SRIO_temperatureflag1;
extern volatile Uint8 SRIO_temperatureflag2;
extern volatile Uint32 DoorbellNum;










//************************* Externs *************************************




//************************** Main Code *********************************
/**************************************************
* Prototype     : msg_send
* Description   : 发送给fpga或者中控的信息
* Return Value :  pImageData

* Date            : 
* Author   	 : 
* Modification  : 
***************************************************/
void msg_send(void)
{
	
	uiXadd = trackInfo.col;//发送col信息
	uiYadd = trackInfo.row;///发送row信息
}


/*****************************************************************************
 Prototype    : FPGA_EMIF16_RTX
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
Int16 colpara1=0;//捕获或跟踪时,中控下发的中心位置
Int16 rowpara1=-256;

Int32 RXanglelast=0;//上帧图像伺服角度
Int32 RYanglelast=0;
Int32 RXanglelast1=0;//上上帧图像伺服角度
Int32 RYanglelast1=0;






//下面是debug使用，调试完成后，全部删除
Target_Track_t FPGACheck;//上报FPGA目标位置
Target_Track_t FPGAChecklast;//上报FPGA目标位置


Uint32 rtxnumber=0;

Uint32 rtxnumberref=0;


int colpara=319;//坐标转换使用
int rowpara=-1;

Int32 RXanglelcnt=1152*3;//允许偏离pixel帧数640*1.8*5;
Int32 RXanglercnt=1152*3;//允许偏离pixel个数
Int32 RYangletcnt=921;//允许偏离pixel个数
Int32 RYanglebcnt=921;//允许偏离pixel个数512*1.8
Int32 RXcenter=993;
Int32 RYcenter=6190;


Uint32 Rposition=0;


int g4=1;
int targetnum=0;
int targetnumref=400;


int g7=1;
int g8=11;

int g9=0;//用于FPGA_EMIF16_RTX2 function


int g10=0;
int	g12=0;




Uint32	Scan5errornum=0;
Uint32	Scan5errornum1=0;
Uint32	Scan5errorcnt=0;
Int16 xref=317;
Int16 xmaxref=30;

int gtrackWidth1  = 80;
int gtrackHeight1 = 64;

float m_thresh;
float m_thresh1;





//跟踪上报误差
//用于整机测试:使用本次的位置信息
void FPGA_EMIF16_RTX1()
{	
	Uint32 i;
	Uint32 j;
	Uint32 Rposition1;
	Int32 Targetcnt;
	Int32 RXangle;
	Int32 RYangle;
	Int32 TXangle;
	Int32 TYangle;

	Int32 RXanglel;//允许偏离pixel个数
	Int32 RXangler;//允许偏离pixel个数
	Int32 RYanglet;//允许偏离pixel个数
	Int32 RYangleb;//允许偏离pixel个数

	Int16 *pTxadd;
	Int16 *pTxaddtemp;
	Int16 *pRxadd;

	
	//debug使用
		//unsigned int imageadd;
		//unsigned int imageadd1;
		//DMA_MEM_Tran_Config   DMA_transfer_cfg;
	
		
		//DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
		//DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
		//DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
		//DMA_transfer_cfg.CCNT=512;	
		//imageadd = 0x83000000+((Scan5errornum+Scan5errornum1)%100)*1310720;//记录当前图像信息
	
		//imageadd1 = 0x8B000000+((Scan5errornum+Scan5errornum1)%100)*655360;//记录当前图像信息

	
	rtxnumber++;
	Targetcnt=0;
	pRxadd=(Int16 *)dsp0_EMIF16_RxAdd;
	pTxadd=(Int16 *)dsp0_EMIF16_TxAdd;
	pTxaddtemp = (Int16 *)0x70000106;

	//读取控制命令
	CommandMode = *(pRxadd+0);
	//通信质询回复
	*(Int16 *)dsp0_EMIF16_Inquiry_Check= (*(pRxadd+3)&0xff);

	if(ScanCheck5Num>0)
		Targetcnt=1;
	
	//发送坐标到FPGA
	//*(Int16 *)0x700001E0=(Int16)(-1-uiYadd);//方位坐标
	//*(Int16 *)0x700001E2=(Int16)(319-uiXadd);
	uiYadd1=(Int16)(-1-uiYadd);//方位坐标
	uiXadd1=(Int16)(319-uiXadd);

	*(Int16 *)0x700001E0=uiYadd1;//方位坐标
	*(Int16 *)0x700001E2=uiXadd1;
	*(Int16 *)0x700001E8=Targetcnt;//FPGA地址0xF4,0-没有目标1-有目标
	//20170830 delete :FPGA只需要一次更新通知
	*(Int16 *)0x700001FA=0x1;//通知FPGA更新显示信息
	
	//debug调试
	if(g4==1)
	{		
		//根据工作状态返回信息
		switch (CommandMode)
		{
			case Command_Scan:        
				RXangle=(Int32)(*(Uint16*)(0x70000008)<<16 | *(Uint16*)(0x7000000a));//伺服方位
				RYangle=(Int32)(*(Uint16*)(0x7000000C)<<16 | *(Uint16*)(0x7000000e));//伺服俯仰			
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x2;

				//目标个数（8bit）
				//*(pTxadd+2) = ScanCheck5Num;
				//20171121 debug:临时限制调试角度，角度过大，发送的图像数据有问题
				RXanglel=RXcenter-RXanglelcnt;
				RXangler=RXcenter+RXanglercnt;		
				RYanglet=RYcenter+RYangletcnt;
				RYangleb=RYcenter-RYangletcnt;		
				if((RXangle<RXanglel)||(RXangle>RXangler))
					ScanCheck5Num=0;
		
				//ScanCheck5Num=2;
				//20171121 debug:临时限制调试角度，
				if(ScanCheck5Num>0)
				{
					//目标个数（8bit）
					//*(pTxadd+2) = 2;
					*(pTxadd+2) = g7;

					//目标角度
					TXangle=((colpara-FPGACheck.col)*pixel_2angle_32bit+RXanglelast1);
					TYangle=((rowpara-FPGACheck.row)*pixel_2angle_32bit+RYanglelast1);		
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = FPGACheck.gray;
					*(pTxaddtemp+5) = FPGACheck.cnt;
					//20171121 debug:临时限制调试角度，
					*(pTxaddtemp+6) = (RXangle>>16);
					*(pTxaddtemp+7) = (RXangle&0xffff);
					*(pTxaddtemp+8) = (RYangle>>16);
					*(pTxaddtemp+9) = (RYangle&0xffff);			
					*(pTxaddtemp+10) = 0x123;
					*(pTxaddtemp+11) = 20;


					//20171123调试临时使用
					//记录当前图像目标信息
					j = DoorbellNum%1000;
					Scan5record[j].TXangle=TXangle;
					Scan5record[j].TYangle=TYangle;
					Scan5record[j].col=FPGACheck.col;
					Scan5record[j].row=FPGACheck.row;
					Scan5record[j].RXangle=RXangle;
					Scan5record[j].RYangle=RYangle;
					Scan5record[j].RXanglelast=RXanglelast;
					Scan5record[j].RYanglelast=RYanglelast;

					Rposition1=Rposition*4;
					Rposition++;

					if(Rposition>39999)
						Rposition=0;

					*(Uint32 *)(0x82000000+Rposition1)=j;	
					*(Uint32 *)(0x82100000+Rposition1)=DoorbellNum;

					targetnum++;
					if(targetnum>targetnumref)
					{
						targetnum=0;
					}
				}
				else
				{
					*(pTxadd+2) = 0;
				}
			
				//数据时标
				*(pTxadd+63) = 0x1;
				*(pTxadd+64) = 0xffff;

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
	        
			case Command_Track:
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x3;

				//ScanCheck5Num=6;
				//20171121 debug:临时限制调试角度，
				//上送一个目标
	#if 0			
				if(ScanCheck5Num>0)
				{
					//目标个数（8bit）
					*(pTxadd+2) = Targetcnt;

					//中心点坐标
					colpara1 = *(Int16 *)0x70000010;
					rowpara1 = *(Int16 *)0x70000012;
					TXangle = (Int32)((319-FPGACheck.col-colpara1)*16);
					TYangle=  (Int32)((-1-FPGACheck.row-rowpara1)*16);

					//目标偏差1					
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);
					*(pTxaddtemp+4) = FPGACheck.gray;
					*(pTxaddtemp+5) = FPGACheck.cnt;	
				}
				else
				{
					*(pTxadd+2) = 0;
				}
		#endif	
				//使用跟踪调试搜索，上送所有目标
				*(pTxadd+2) = ScanCheck5Num;//目标个数（8bit）

				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				/*目标角度*/
				for(i=0;i<ScanCheck5Num;i++)
				{					
					TXangle = (Int32)((319-ScanCheck5[i].col-colpara1)*16);
					TYangle=  (Int32)((-1-ScanCheck5[i].row-rowpara1)*16);		
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = ScanCheck5[i].gray;
					*(pTxaddtemp+5) = ScanCheck5[i].cnt;					
					pTxaddtemp += 6;
				}
			/*获得pbuf数据传递一个640*512的数组*/			
		

				//20171222调试dsp0_trackmode add
				if(ScanCheck5Num>0)
				{
					if(abs(uiXadd-xref)>xmaxref)
					{	
						Scan5error[Scan5errornum].col=uiXadd;
						Scan5error[Scan5errornum].row=uiYadd;
						Scan5error[Scan5errornum].colsize=DoorbellNum;
						Scan5error[Scan5errornum].rowsize=CheckCycles;
						Scan5error[Scan5errornum].cnt=ScanCheck5Num;
						Scan5error[Scan5errornum].gray=Scan5errorcnt;
						Scan5errornum++;
						Scan5errorcnt++;

						
					}
					
				}				
				if(ScanCheck5Num==0)
				{
					Scan5error1[Scan5errornum1].col=uiXadd;
					Scan5error1[Scan5errornum1].row=uiYadd;
					Scan5error1[Scan5errornum1].colsize=DoorbellNum;
					Scan5error1[Scan5errornum1].rowsize=CheckCycles;
					Scan5error1[Scan5errornum1].gray=Scan5errorcnt;
					Scan5errornum1++;
					Scan5errorcnt++;		
				}
	

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;

			case Command_ReCapture:
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x7;
	
		
				//20171121 debug:临时限制调试角度，
				if(ScanCheck5Num>0)
				{					
					//目标个数（8bit）
					*(pTxadd+2) = Targetcnt;

					//中心点坐标
					colpara1 = *(Int16 *)0x70000010;
					rowpara1 = *(Int16 *)0x70000012;
					//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
					//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);
					TXangle = (Int32)((319-FPGACheck.col-colpara1)*16);
					TYangle=  (Int32)((-1-FPGACheck.row-rowpara1)*16);
					
					//目标偏差
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);
					*(pTxaddtemp+4) = FPGACheck.gray;
					*(pTxaddtemp+5) = FPGACheck.cnt;					
				}
				else
				{
					*(pTxadd+2) = 0;
				}					
				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
				
			default:      
				//目标状态	
				*(pTxadd+0) = (Int16)CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x10;
				//目标个数（8bit）
				*(pTxadd+2) = 0x04;	
				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
			
		}
	}
	else
	{
		//根据工作状态返回信息
		switch (CommandMode)
		{
			case Command_Scan:
				RXangle=(Int32)(*(Uint16*)(0x70000008)<<16 | *(Uint16*)(0x7000000a));//伺服方位
				RYangle=(Int32)(*(Uint16*)(0x7000000C)<<16 | *(Uint16*)(0x7000000e));//伺服俯仰				
				*(pTxadd+0) = (Int16)CommandMode;//目标状态						
				*(pTxadd+1) = 0x2;	//故障信息					
				*(pTxadd+2) = ScanCheck5Num;//目标个数（8bit）
				
				/*目标角度*/
				for(i=0;i<ScanCheck5Num;i++)
				{					
					TXangle=((colpara-ScanCheck5[i].col)*pixel_2angle_32bit+RXangle);
					TYangle=((rowpara-ScanCheck5[i].row)*pixel_2angle_32bit+RYangle);			
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = ScanCheck5[i].gray;
					*(pTxaddtemp+5) = ScanCheck5[i].cnt;					
					pTxaddtemp += 6;
				}
				/*数据时标*/
				*(pTxadd+63) = 0x1;
				*(pTxadd+64) = 0xffff;
				/* 握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
	        
			case Command_Track:
				/*中心点坐标*/
				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
				//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);

				TXangle = (Int32)((319-FPGACheck.col-colpara1)*16);
				TYangle=  (Int32)((-1-FPGACheck.row-rowpara1)*16);
				
				
				*(pTxadd+0) = (Int16)CommandMode;//目标状态						
				*(pTxadd+1) = 0x3;	//故障信息	
				*(pTxadd+2) = Targetcnt;//目标个数（8bit）	
				
				/*单目标跟踪误差*/									
				*pTxaddtemp = (TXangle>>16);//目标偏差
				*(pTxaddtemp+1) = (TXangle&0xffff);
				*(pTxaddtemp+2) = (TYangle>>16);
				*(pTxaddtemp+3) = (TYangle&0xffff);
				*(pTxaddtemp+4) = FPGACheck.gray;
				*(pTxaddtemp+5) = FPGACheck.cnt;					
			
				/*多目标跟踪误差*/
				//*(pTxadd+2) = ScanCheck5Num;//目标个数（8bit）				
//				for(i=0;i<ScanCheck5Num;i++)
//				{	
//					TXangle = (Int32)((colpara1-ScanCheck5[i].col)*16);
//					TYangle=  (Int32)((rowpara1-(ScanCheck5[i].row+512))*16);
//					*pTxaddtemp = (TXangle>>16);//目标偏差
//					*(pTxaddtemp+1) = (TXangle&0xffff);
//					*(pTxaddtemp+2) = (TYangle>>16);
//					*(pTxaddtemp+3) = (TYangle&0xffff);	
//					*(pTxaddtemp+4) = ScanCheck5[i].gray;
//					*(pTxaddtemp+5) = ScanCheck5[i].cnt;					
//					pTxaddtemp += 6;					
//				}				
				/* 握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;

			case Command_ReCapture:				
				/*中心点坐标*/
				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
				//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);

				TXangle = (Int32)((319-FPGACheck.col-colpara1)*16);
				TYangle=  (Int32)((-1-FPGACheck.row-rowpara1)*16);
				
				/*目标误差:上报一个*/					
				*(pTxadd+0) = CommandMode;//目标状态					
				*(pTxadd+1) = 0x7;	//故障信息					
				*(pTxadd+2) = Targetcnt;//目标个数（8bit）				
				*pTxaddtemp = (TXangle>>16);//目标偏差
				*(pTxaddtemp+1) = (TXangle&0xffff);
				*(pTxaddtemp+2) = (TYangle>>16);
				*(pTxaddtemp+3) = (TYangle&0xffff);
				*(pTxaddtemp+4) = FPGACheck.gray;
				*(pTxaddtemp+5) = FPGACheck.cnt;		
				
				/*握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
				
			default:      					
				*(pTxadd+0) = (Int16)CommandMode;//目标状态				
				*(pTxadd+1) = 0x11;//故障信息					
				*(pTxadd+2) = 0x04;	//目标个数（8bit）
				/*握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;			
		}
	}
	
	RXanglelast1 =RXanglelast;
	RYanglelast1 =RYanglelast;
	RXanglelast =RXangle;
	RYanglelast =RYangle;
}


//跟踪上报误差
//用于整机测试:使用上次的位置信息
void FPGA_EMIF16_RTX()
{	
	Uint32 i;
	Uint32 j;
	Uint32 Rposition1;
	Int32 Targetcnt;
	Int32 RXangle;
	Int32 RYangle;
	Int32 TXangle;
	Int32 TYangle;

	Int32 RXanglel;//允许偏离pixel个数
	Int32 RXangler;//允许偏离pixel个数
	Int32 RYanglet;//允许偏离pixel个数
	Int32 RYangleb;//允许偏离pixel个数

	Int16 *pTxadd;
	Int16 *pTxaddtemp;
	Int16 *pRxadd;

	
	//debug使用
		//unsigned int imageadd;
		//unsigned int imageadd1;
		//DMA_MEM_Tran_Config   DMA_transfer_cfg;
	
		
		//DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
		//DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
		//DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
		//DMA_transfer_cfg.CCNT=512;	
		//imageadd = 0x83000000+((Scan5errornum+Scan5errornum1)%100)*1310720;//记录当前图像信息
	
		//imageadd1 = 0x8B000000+((Scan5errornum+Scan5errornum1)%100)*655360;//记录当前图像信息

	
	rtxnumber++;
	Targetcnt=0;
	pRxadd=(Int16 *)dsp0_EMIF16_RxAdd;
	pTxadd=(Int16 *)dsp0_EMIF16_TxAdd;
	pTxaddtemp = (Int16 *)0x70000106;

	//读取控制命令
	CommandMode = *(pRxadd+0);

	/*工作状态发生改变时，切换参数*/
	//搜索模式
#if 0	
	if((CommandMode<Command_Track)&&(modeswitch==0))
	{
		modeswitch=1;

		//正式代码使用，下面参数
		//临时调试，先不用
		para1num2=9;
		TotalMean=4;
		TotalFcha=2.5;

		//TotalMean=4.5;
		//TotalFcha=2.1;

	}
	//其他模式
	if((CommandMode>Command_Scan)&&(modeswitch==1))
	{
		modeswitch=0;		
		para1num2=10;
		TotalMean=4;
		TotalFcha=2.2;

		//TotalMean=4.5;
		//TotalFcha=1.8;

	}
#endif		
	//通信质询回复
	*(Int16 *)dsp0_EMIF16_Inquiry_Check= (*(pRxadd+3)&0xff);

	if(ScanCheck5lastNum>0)
		Targetcnt=1;
	
	//发送坐标到FPGA
	//*(Int16 *)0x700001E0=(Int16)(-1-uiYadd);//方位坐标
	//*(Int16 *)0x700001E2=(Int16)(319-uiXadd);
	uiYadd1=(Int16)(-1-uiYaddlast);//方位坐标
	uiXadd1=(Int16)(319-uiXaddlast);

	*(Int16 *)0x700001E0=uiYadd1;//方位坐标
	*(Int16 *)0x700001E2=uiXadd1;
	*(Int16 *)0x700001E8=Targetcnt;//FPGA地址0xF4,0-没有目标1-有目标
	//20170830 delete :FPGA只需要一次更新通知
	*(Int16 *)0x700001FA=0x1;//通知FPGA更新显示信息
	
	//debug调试
	if(g4==1)
	{		
		//根据工作状态返回信息
		switch (CommandMode)
		{
			case Command_Scan:        
				RXangle=(Int32)(*(Uint16*)(0x70000008)<<16 | *(Uint16*)(0x7000000a));//伺服方位
				RYangle=(Int32)(*(Uint16*)(0x7000000C)<<16 | *(Uint16*)(0x7000000e));//伺服俯仰			
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x2;

				//目标个数（8bit）
				//*(pTxadd+2) = ScanCheck5Num;
				//20171121 debug:临时限制调试角度，角度过大，发送的图像数据有问题
				RXanglel=RXcenter-RXanglelcnt;
				RXangler=RXcenter+RXanglercnt;		
				RYanglet=RYcenter+RYangletcnt;
				RYangleb=RYcenter-RYangletcnt;		
				if((RXangle<RXanglel)||(RXangle>RXangler))
					ScanCheck5lastNum=0;
		
				//ScanCheck5Num=2;
				//20171121 debug:临时限制调试角度，
				if(ScanCheck5lastNum>0)
				{
					//目标个数（8bit）
					//*(pTxadd+2) = 2;
					*(pTxadd+2) = g7;

					//目标角度
					TXangle=((colpara-FPGAChecklast.col)*pixel_2angle_32bit+RXanglelast1);
					TYangle=((rowpara-FPGAChecklast.row)*pixel_2angle_32bit+RYanglelast1);		
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = FPGAChecklast.gray;
					*(pTxaddtemp+5) = FPGAChecklast.cnt;
					//20171121 debug:临时限制调试角度，
					*(pTxaddtemp+6) = (RXangle>>16);
					*(pTxaddtemp+7) = (RXangle&0xffff);
					*(pTxaddtemp+8) = (RYangle>>16);
					*(pTxaddtemp+9) = (RYangle&0xffff);			
					*(pTxaddtemp+10) = 0x123;
					*(pTxaddtemp+11) = 20;


					//20171123调试临时使用
					//记录当前图像目标信息
					j = DoorbellNum%1000;
					Scan5record[j].TXangle=TXangle;
					Scan5record[j].TYangle=TYangle;
					Scan5record[j].col=FPGAChecklast.col;
					Scan5record[j].row=FPGAChecklast.row;
					Scan5record[j].RXangle=RXangle;
					Scan5record[j].RYangle=RYangle;
					Scan5record[j].RXanglelast=RXanglelast;
					Scan5record[j].RYanglelast=RYanglelast;

					Rposition1=Rposition*4;
					Rposition++;

					if(Rposition>39999)
						Rposition=0;

					*(Uint32 *)(0x82000000+Rposition1)=j;	
					*(Uint32 *)(0x82100000+Rposition1)=DoorbellNum;

					targetnum++;
					if(targetnum>targetnumref)
					{
						targetnum=0;
					}
				}
				else
				{
					*(pTxadd+2) = 0;
				}
			
				//数据时标
				*(pTxadd+63) = 0x1;
				*(pTxadd+64) = 0xffff;

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
	        
			case Command_Track:
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x3;

				//ScanCheck5Num=6;
				//20171121 debug:临时限制调试角度，
				//上送一个目标
	#if 0			
				if(ScanCheck5Num>0)
				{
					//目标个数（8bit）
					*(pTxadd+2) = Targetcnt;

					//中心点坐标
					colpara1 = *(Int16 *)0x70000010;
					rowpara1 = *(Int16 *)0x70000012;
					TXangle = (Int32)((319-FPGACheck.col-colpara1)*16);
					TYangle=  (Int32)((-1-FPGACheck.row-rowpara1)*16);

					//目标偏差1					
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);
					*(pTxaddtemp+4) = FPGACheck.gray;
					*(pTxaddtemp+5) = FPGACheck.cnt;	
				}
				else
				{
					*(pTxadd+2) = 0;
				}
		#endif	
				//使用跟踪调试搜索，上送所有目标
				*(pTxadd+2) = ScanCheck5lastNum;//目标个数（8bit）

				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				/*目标角度*/
				for(i=0;i<ScanCheck5lastNum;i++)
				{					
					TXangle = (Int32)((319-ScanCheck5last[i].col-colpara1)*16);
					TYangle=  (Int32)((-1-ScanCheck5last[i].row-rowpara1)*16);		
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = ScanCheck5last[i].gray;
					*(pTxaddtemp+5) = ScanCheck5last[i].cnt;					
					pTxaddtemp += 6;
				}
			/*获得pbuf数据传递一个640*512的数组*/			
	#if 0	

				//20171222调试dsp0_trackmode add
				if(ScanCheck5lastNum>0)
				{
					if(abs(uiXadd-xref)>xmaxref)
					{	
						Scan5error[Scan5errornum].col=uiXadd;
						Scan5error[Scan5errornum].row=uiYadd;
						Scan5error[Scan5errornum].colsize=DoorbellNum;
						Scan5error[Scan5errornum].rowsize=CheckCycles;
						Scan5error[Scan5errornum].cnt=ScanCheck5lastNum;
						Scan5error[Scan5errornum].gray=ScanCheck5Num;
						Scan5errornum++;
						Scan5errorcnt++;

						
					}
					
				}				
				if(ScanCheck5lastNum==0)
				{
					Scan5error1[Scan5errornum1].col=uiXadd;
					Scan5error1[Scan5errornum1].row=uiYadd;
					Scan5error1[Scan5errornum1].colsize=DoorbellNum;
					Scan5error1[Scan5errornum1].rowsize=CheckCycles;
					Scan5error1[Scan5errornum1].gray=Scan5errorcnt;
					Scan5errornum1++;
					Scan5errorcnt++;		
				}
	#endif

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;

			case Command_ReCapture:
				//目标状态	
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x7;
	
		
				//20171121 debug:临时限制调试角度，
				if(ScanCheck5lastNum>0)
				{					
					//目标个数（8bit）
					*(pTxadd+2) = Targetcnt;

					//中心点坐标
					colpara1 = *(Int16 *)0x70000010;
					rowpara1 = *(Int16 *)0x70000012;
					//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
					//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);
					TXangle = (Int32)((319-FPGAChecklast.col-colpara1)*16);
					TYangle=  (Int32)((-1-FPGAChecklast.row-rowpara1)*16);
					
					//目标偏差
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);
					*(pTxaddtemp+4) = FPGAChecklast.gray;
					*(pTxaddtemp+5) = FPGAChecklast.cnt;					
				}
				else
				{
					*(pTxadd+2) = 0;
				}					
				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
				
			default:      
				//目标状态	
				*(pTxadd+0) = (Int16)CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x10;
				//目标个数（8bit）
				*(pTxadd+2) = 0x04;	
				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
			
		}
	}
	else
	{
		//根据工作状态返回信息
		switch (CommandMode)
		{
			case Command_Scan:
				RXangle=(Int32)(*(Uint16*)(0x70000008)<<16 | *(Uint16*)(0x7000000a));//伺服方位
				RYangle=(Int32)(*(Uint16*)(0x7000000C)<<16 | *(Uint16*)(0x7000000e));//伺服俯仰				
				*(pTxadd+0) = (Int16)CommandMode;//目标状态						
				*(pTxadd+1) = 0x2;	//故障信息					
				*(pTxadd+2) = ScanCheck5lastNum;//目标个数（8bit）
				
				/*目标角度*/
				for(i=0;i<ScanCheck5lastNum;i++)
				{					
					TXangle=((colpara-ScanCheck5last[i].col)*pixel_2angle_32bit+RXangle);
					TYangle=((rowpara-ScanCheck5last[i].row)*pixel_2angle_32bit+RYangle);			
					*pTxaddtemp = (TXangle>>16);
					*(pTxaddtemp+1) = (TXangle&0xffff);
					*(pTxaddtemp+2) = (TYangle>>16);
					*(pTxaddtemp+3) = (TYangle&0xffff);			
					*(pTxaddtemp+4) = ScanCheck5last[i].gray;
					*(pTxaddtemp+5) = ScanCheck5last[i].cnt;					
					pTxaddtemp += 6;
				}
				/*数据时标*/
				*(pTxadd+63) = 0x1;
				*(pTxadd+64) = 0xffff;
				/* 握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
	        
			case Command_Track:
				/*中心点坐标*/
				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
				//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);

				TXangle = (Int32)((319-FPGAChecklast.col-colpara1)*16);
				TYangle=  (Int32)((-1-FPGAChecklast.row-rowpara1)*16);
				
				
				*(pTxadd+0) = (Int16)CommandMode;//目标状态						
				*(pTxadd+1) = 0x3;	//故障信息	
				*(pTxadd+2) = Targetcnt;//目标个数（8bit）	
				
				/*单目标跟踪误差*/									
				*pTxaddtemp = (TXangle>>16);//目标偏差
				*(pTxaddtemp+1) = (TXangle&0xffff);
				*(pTxaddtemp+2) = (TYangle>>16);
				*(pTxaddtemp+3) = (TYangle&0xffff);
				*(pTxaddtemp+4) = FPGAChecklast.gray;
				*(pTxaddtemp+5) = FPGAChecklast.cnt;					
			
				/*多目标跟踪误差*/
				//*(pTxadd+2) = ScanCheck5Num;//目标个数（8bit）				
//				for(i=0;i<ScanCheck5Num;i++)
//				{	
//					TXangle = (Int32)((colpara1-ScanCheck5[i].col)*16);
//					TYangle=  (Int32)((rowpara1-(ScanCheck5[i].row+512))*16);
//					*pTxaddtemp = (TXangle>>16);//目标偏差
//					*(pTxaddtemp+1) = (TXangle&0xffff);
//					*(pTxaddtemp+2) = (TYangle>>16);
//					*(pTxaddtemp+3) = (TYangle&0xffff);	
//					*(pTxaddtemp+4) = ScanCheck5[i].gray;
//					*(pTxaddtemp+5) = ScanCheck5[i].cnt;					
//					pTxaddtemp += 6;					
//				}				
				/* 握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;

			case Command_ReCapture:				
				/*中心点坐标*/
				colpara1 = *(Int16 *)0x70000010;
				rowpara1 = *(Int16 *)0x70000012;
				//TXangle = (Int32)((colpara1-ScanCheck5[0].col)*16);
				//TYangle=  (Int32)((ScanCheck5[0].row+512-rowpara1)*16);

				TXangle = (Int32)((319-FPGAChecklast.col-colpara1)*16);
				TYangle=  (Int32)((-1-FPGAChecklast.row-rowpara1)*16);
				
				/*目标误差:上报一个*/					
				*(pTxadd+0) = CommandMode;//目标状态					
				*(pTxadd+1) = 0x7;	//故障信息					
				*(pTxadd+2) = Targetcnt;//目标个数（8bit）				
				*pTxaddtemp = (TXangle>>16);//目标偏差
				*(pTxaddtemp+1) = (TXangle&0xffff);
				*(pTxaddtemp+2) = (TYangle>>16);
				*(pTxaddtemp+3) = (TYangle&0xffff);
				*(pTxaddtemp+4) = FPGAChecklast.gray;
				*(pTxaddtemp+5) = FPGAChecklast.cnt;		
				
				/*握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
				
			default:      					
				*(pTxadd+0) = (Int16)CommandMode;//目标状态				
				*(pTxadd+1) = 0x11;//故障信息					
				*(pTxadd+2) = 0x04;	//目标个数（8bit）
				/*握手标识位*/
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;			
		}
	}
	
	RXanglelast1 =RXanglelast;
	RYanglelast1 =RYanglelast;
	RXanglelast =RXangle;
	RYanglelast =RYangle;
}



//跟踪上报误差
//用于图像板(非整机)测试代码
void FPGA_EMIF16_RTX2()
{	
	Uint32 i;
	Uint32 j;	
	Int32 Targetcnt;
	Int16 uiXpos;//Xadd方位坐标
	Int16 uiYpos;//Yadd俯仰坐标
	Int16 *pTxadd;
	Int16 *pTxaddtemp;
	Int16 *pRxadd;
	
	rtxnumber++;
	Targetcnt=0;
	pRxadd=(Int16 *)dsp0_EMIF16_RxAdd;
	pTxadd=(Int16 *)dsp0_EMIF16_TxAdd;
	pTxaddtemp = (Int16 *)0x70000106;

	//读取控制命令
	CommandMode = *(pRxadd+0);
	//通信质询回复
	*(Int16 *)dsp0_EMIF16_Inquiry_Check= (*(pRxadd+3)&0xff);

	//20180827add debug使用
	CommandMode=2;
	
	//发送坐标到FPGA
	//*(Int16 *)0x700001E0=(Int16)(-1-uiYadd);//方位坐标
	//*(Int16 *)0x700001E2=(Int16)(319-uiXadd);

	//只报一个目标信息
	if(g9==1)
	{
		if(ScanCheck5lastNum>0)
		Targetcnt=1;

		uiYadd1=(Int16)(-1-uiYaddlast);//方位坐标
		uiXadd1=(Int16)(319-uiXaddlast);

		*(Int16 *)0x700001E0=uiYadd1;//方位坐标
		*(Int16 *)0x700001E2=uiXadd1;
		*(Int16 *)0x700001E8=Targetcnt;//FPGA地址0xF4,0-没有目标1-有目标
		//20170830 delete :FPGA只需要一次更新通知
		*(Int16 *)0x700001FA=0x1;//通知FPGA更新显示信息
	}
	else
	{
		//上报所有目标信息		
		//根据工作状态返回信息
		switch (CommandMode)
		{
			case Command_Scan:        
				*(pTxadd+0) = CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x2;

				//目标个数（8bit）
				*(pTxadd+2) = ScanCheck5lastNum;	
				
				//20180820确定使用这个地址发送目标位置给FPGA
				for(i=0;i<ScanCheck5lastNum;i++)
				{					
					uiYpos=(Int16)(-1-ScanCheck5last[i].row);//方位坐标
					uiXpos=(Int16)(319-ScanCheck5last[i].col);//方位坐标					
					
					*pTxaddtemp =uiYpos;
					*(pTxaddtemp+1) = uiXpos;					
					
					pTxaddtemp += 2;
				}

							
				*(Int16 *)0x700001E8=ScanCheck5lastNum;//FPGA地址0xF4,0-没有目标1-有目标
				//20170830 delete :FPGA只需要一次更新通知
				//*(Int16 *)0x700001FA=0x1;//通知FPGA更新显示信息

				//数据时标
				*(pTxadd+63) = 0x1;
				*(pTxadd+64) = 0xffff;

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
	 			
			default:      
				//目标状态	
				*(pTxadd+0) = (Int16)CommandMode;
				//故障信息	
				*(pTxadd+1) = 0x10;
				//目标个数（8bit）
				*(pTxadd+2) = 0xff;	

				*(Int16 *)0x700001E8=0xff;//FPGA地址0xF4,0-没有目标1-有目标
				//20170830 delete :FPGA只需要一次更新通知
				//*(Int16 *)0x700001FA=0x1;//通知FPGA更新显示信息

				//握手标识位
				*(pTxadd+67) = 0;
				*(pTxadd+66) = 0x01;
				break;
			
		}
	}
}

/**************************************************
* Prototype     : AcquireData5
* Description   : 从原始图像中获得iWidth*iHeight大小的图像数据进行处理;
原始图像数据宽度为16bit，通过Visual DSP++ image view功能，发现使用12bit或者10bit gray查看图像时，
图像对比度得到增强。对图像数据进行移位操作，下面做法作为参考，根据场景进行调整。
将图像数据向高位移2位，提高图像数据的计算精度及图像灰度的对比度，
提高3位时，虚警会增加很多，需要考虑其他的滤波方法及去虚警方法

至少右移1位，避免有符号数16位运算的错误
* Input           :pRecImage——输入数据的地址指针(图像数据本身为无符号型数据16bit)
		   	  iworkMode——系统工作模式		   	  
		         pImageData——图像数据经过预处理后的的地址指针(图像数据本身为无符号型数据16bit)
* Output         : 
* Return Value :  pImageData

* Date            : 
* Author   	 : 
* Modification  : 
***************************************************/
inline void AcquireData5(Int16 *pRecImage,Int16 *pImageData,int nWidth,int nHeight)
{
	int i,j;
	unsigned int rightshift;	
	int WAddRec,HAddRec;//fpga传送图像大小
	long long pixel;
	long long pixelresult;
	
	//int WidthAddr,HeightAddr;//dsp存储空间需要的宽度和长度
	//int wleftAddr,htopAddr;//dsp接收到的图像和获得的图像:宽度和长度在存储空间差值
	long long *pRecImageAdd;
	long long *pImageDataAdd;
		
		
	WAddRec  = nWidth>>2;
	HAddRec  = nHeight;		
	

	
		
	rightshift=1;//右移1位
	pRecImageAdd=(long long *)pRecImage;
	pImageDataAdd=(long long *)pImageData;
	for(j=0;j<HAddRec;j++)
	{
		for(i=0;i<WAddRec;i++)
		{			
			pixel=*(pRecImageAdd+j*WAddRec+i);
			//一次移4个16bit数据		
			pixelresult=_dshru2(pixel,rightshift);
			//获得处理后的数据
			*(pImageDataAdd+j*WAddRec+i)=pixelresult;
		}
	}		
}

/**************************************************
* Prototype     : Pre_DataMove
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
void Pre_DataMove(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int WidthAddr,HeightAddr;//dsp存储空间需要的宽度和长度	
	//int nWidth1;//图像宽度个数	
	int nTrimHValue;
	int nTrimLValue;
	Int32 Graysrc1;
	Int32 Graysrc2;
	Int32 Graytemp;
	Int32 *pDataIn32;
	Int32 *pDataOut32;
	
	
	//WidthAddr  = nWidth/2;
	//HeightAddr = nHeight;

			
	WidthAddr  = nWidth>>1;
	HeightAddr = nHeight;	
	//nWidth1 = nWidth;
	
	

	nTrimHValue=10;//选取第10个最大值
	nTrimLValue=200;//选取第200个最大值
	//HISTOGRAM Histogram;
	Pre_Hist(pDataIn, nWidth,nHeight);
	GetMaxValue(nTrimHValue);
	GetMinValue(nTrimLValue);

	//和xushi代码不一样
	if(m_Histogram.lGray>m_Histogram.hGray)
	{	
		m_Histogram.lGray=m_Histogram.hGray;
	}	
	
	Graysrc1 = (m_Histogram.lGray<<16|m_Histogram.lGray);
	pDataIn32 =(Int32 *)pDataIn;
	pDataOut32=(Int32 *)pDataOut;
	for(j=0;j<HeightAddr;j++)
	{		
		for(i=0;i<WidthAddr;i++)
		{			
			
			Graysrc2= *(pDataIn32+j*WidthAddr+i);
			Graytemp=_sub2(Graysrc2,Graysrc1);//_sub2结果:(Graysrc2-Graysrc1	)	
			*(pDataOut32+j*WidthAddr+i)=_max2(0,Graytemp);	
		}
	}		
}




/**************************************************
* Prototype     : Pre_IntermFilter
* Description   : 使用中值法滤波
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_IntermFilter(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeigh)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nCur,nLeft,nUp,nRight,nDown;
	int array[8];
	Int16 *pDataIn16,*pDataOut16;
	
				
	Width  = nWidth;
	Height = nHeigh;			
	pDataIn16=pDataIn;	
	pDataOut16=pDataOut;

	//使用5值求中值方法，去除单点和滤波
	for(j=1;j<Height-1;j++)
	{		
		for(i=1;i<Width-1;i++)
		{				
			nUp       =*(pDataIn16+(j-1)*Width+(i));						
			nLeft     =*(pDataIn16+(j)*Width+(i-1));
			nCur      =*(pDataIn16+(j)*Width+(i));
			nRight    =*(pDataIn16+(j)*Width+(i+1));			
			nDown     =*(pDataIn16+(j+1)*Width+(i));
			//获取四个数值的中间2个
			array[0] = _max2(nUp,nLeft);
			array[1] = _min2(nUp,nLeft);
			array[2] = _max2(nDown,nRight);
			array[3] = _min2(nDown,nRight);
			array[4] = _max2(array[1],array[3]);
			array[5] = _min2(array[0],array[2]);
			array[6] = _max2(array[4],array[5]);
			array[7] = _min2(array[4],array[5]);
			//获取中值
			if(nCur>array[6])
				nCur=array[6];
			if(nCur<array[7])
				nCur=array[7];
			
			*(pDataOut16+(j)*Width+(i))=nCur;	
		}
		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+1);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+638);
	}	
}

/**************************************************
* Prototype     : Pre_MeanFilter
* Description   : 使用8连通，去除单点
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_MeanFilter(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeigh)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	//Int16 hMaxGray,lMaxGray,hMinGray,lMinGray;	
	int nCur=0,nLeft=0,nLeftUp=0,nUp=0,nRightUp=0,nRight=0,nRightDown=0,nDown=0,nLeftDown=0;
	int nMaxGray=0;	
	int   lMax[10];

	Int16 *pDataIn16,*pDataOut16;
	
	
			
	Width  = nWidth;
	Height = nHeigh;			
	pDataIn16=pDataIn;	
	pDataOut16=pDataOut;

	//使用8连通，去除单点
	for(j=1;j<Height-1;j++)
	{		
		for(i=1;i<Width-1;i++)
		{			
			nLeftUp   =*(pDataIn16+(j-1)*Width+(i-1));
			nUp       =*(pDataIn16+(j-1)*Width+(i));
			nRightUp  =*(pDataIn16+(j-1)*Width+(i+1));			
			nLeft     =*(pDataIn16+(j)*Width+(i-1));
			nCur      =*(pDataIn16+(j)*Width+(i));
			nRight    =*(pDataIn16+(j)*Width+(i+1));
			nLeftDown =*(pDataIn16+(j+1)*Width+(i-1));
			nDown     =*(pDataIn16+(j+1)*Width+(i));
			nRightDown=*(pDataIn16+(j+1)*Width+(i+1));
						
			//*(pOutData+(i)*Width+(j))=(nCur+nLeft+nLeftUp+nUp+nRightUp+nRight+nRightDown+nDown+nLeftDown)/9;
			//*(pOutData+(i)*Width+(j))=(nCur+nLeft+nUp+nRight+nDown)/5;
			//*(pOutData+(i)*Width+(j))=(nCur+nLeftUp+nRightUp+nRightDown+nLeftDown)/5;
			//lMax[0]=nLeftUp|(nUp<<16);
			//lMax[1]=nLeft|(nRight<<16);
			//lMax[2]=nLeftDown|(nDown<<16);
			//lMax[3]=nRightUp|(nRightDown<<16);

			nMaxGray=_max2(nLeftUp,nUp);
			nMaxGray=_max2(nRightUp,nMaxGray);
			nMaxGray=_max2(nLeft,nMaxGray);
			nMaxGray=_max2(nRight,nMaxGray);			
			nMaxGray=_max2(nLeftDown,nMaxGray);			
			nMaxGray=_max2(nDown,nMaxGray);
			nMaxGray=_max2(nRightDown,nMaxGray);
		
	#if 0		
			//获取第二个最大值
			if(nCur>=nMaxGray)
			{				
				//多个最大值，同时nCur是最大值
				if(nCur==nMaxGray)
				{
					nCur =((nCur+nLeft+nRight+nLeftUp+nUp+nRightUp+nRightDown+nDown+nLeftDown)*0.1111111);
				}
				else
				{
					nCur = nMaxGray;//nCur是唯一最大值
				}				
			}
	#endif		
			//多个最大值，同时nCur是最大值	,认为是目标点
			//获取第二个最大值
			if(nCur>nMaxGray)
			{				
							
				nCur = nMaxGray;//nCur是唯一最大值								
			}		
			*(pDataOut16+(j)*Width+(i))=nCur;	
		}

		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+1);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+638);
	}	
}

/**************************************************
* Prototype     : Pre_BKForecast
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_BKForecast(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int nWidthL,nWidthR;
	int Width,Height;
	int nTempa=0,nTempb=0,nTempc=0,nTempd=0;
	int nTemp[5]={0,0,0,0,0};
	
	Int16 *pImageDataIn16;
	Int16 *pDataIn16;
	Int16 *pDataOut16;

	Width = nWidth;
	Height=nHeight;
	nWidthL=2;
	nWidthR=nWidth/2-nWidthL;

	pImageDataIn16=pDataIn;
	pDataIn16=pDataIn;
	pDataOut16=pDataOut;

	
	for (j=5;j<Height-5;j++)
	{
		for (i=4;i<636;i++)
		{
 			nTemp[0]=*(pDataIn16+j*Height+i)*2; 			
			nTemp[1]=*(pDataIn16+(j+1)*Height+i-3);
 			nTemp[2]=*(pDataIn16+(j-4)*Height+i+3);
			nTemp[3]=*(pDataIn16+(j-4)*Height+i-3);
 			nTemp[4]=*(pDataIn16+(j+1)*Height+i+3);
			
			nTempa=nTemp[0]-*(pDataIn16+j*Height+i-4)-*(pDataIn16+j*Height+i+4);
			nTempb=nTemp[0]-*(pDataIn16+(j-4)*Height+i)-*(pDataIn16+(j+4)*Height+i);
			
			//nTemp[0]=*(pDataIn16+j*Width+i)*2;				
			//nTemp[1]=*(pDataIn16+(j+3)*Width+(i-1));
			//nTemp[2]=*(pDataIn16+(j+3)*Width+(i+4));
			//nTemp[3]=*(pDataIn16+(j+3)*Width+(i+1));
			//nTemp[4]=*(pDataIn16+(j+3)*Width+(i-4));
			//nTempa=nTemp[0]-*(pDataIn16+(j+4)*Width+i)-*(pDataIn16+j*Width+i+4);
			//nTempb=nTemp[0]-*(pDataIn16+(j+1)*Width+i-3)-*(pDataIn16+(j+1)*Width+i+3);

			nTempc=	nTemp[0]-nTemp[1]-nTemp[2];
			nTempd=	nTemp[0]-nTemp[3]-nTemp[4];				

			nTempa=MAX(nTempa,0);
			nTempb=MAX(nTempb,0);
			nTempc=MAX(nTempc,0);
			nTempd=MAX(nTempd,0);			

			*(pDataOut16+j*Width+i)=nTempa+nTempb+nTempc+nTempd;
		
		}
		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+6);
		*(pDataOut16+j*Width+1)=*(pDataOut16+j*Width+7);
		*(pDataOut16+j*Width+2)=*(pDataOut16+j*Width+4);
		*(pDataOut16+j*Width+3)=*(pDataOut16+j*Width+5);
		
		*(pDataOut16+j*Width+636)=*(pDataOut16+j*Width+634);
		*(pDataOut16+j*Width+637)=*(pDataOut16+j*Width+635);
		*(pDataOut16+j*Width+638)=*(pDataOut16+j*Width+632);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+633);
		
	}
}


/**************************************************
* Prototype     : Pre_BKForecast
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
备份文件
***************************************************/
inline void Pre_BKForecast1(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int nWidthL,nWidthR;
	int Width,Height;
	int nTempa=0,nTempb=0,nTempc=0,nTempd=0;
	int nTemp[5]={0,0,0,0,0};
	Int16 *pDataIn16;
	Int16 *pDataOut16;

	Width = nWidth;
	Height=nHeight;
	nWidthL=2;
	nWidthR=nWidth/2-nWidthL;
	
	pDataIn16=pDataIn;
	pDataOut16=pDataOut;	
	for (j=5;j<Height-5;j++)
	{
		for (i=nWidthL;i<nWidthR;i++)
		{
// 			nTemp[0]=*(pInData+j*Height+i)*2;
// 			nTempa=nTemp[0]-*(pInData+j*Height+i-4)-*(pInData+j*Height+i+4);
// 			nTempb=nTemp[0]-*(pInData+(j-4)*Height+i)-*(pInData+(j+4)*Height+i);
// 			//
// 			nTemp[1]=*(pInData+(j+1)*Height+i-3);
// 			nTemp[2]=*(pInData+(j-4)*Height+i+3);
// 			nTemp[3]=*(pInData+(j-4)*Height+i-3);
// 			nTemp[4]=*(pInData+(j+1)*Height+i+3);
			//
			nTemp[0]=*(pDataIn16+j*Width+2*i)*2;
			nTempa=nTemp[0]-*(pDataIn16+(j-4)*Width+2*i)-*(pDataIn16+(j+4)*Width+2*i);
			nTempb=nTemp[0]-*(pDataIn16+j*Width+2*i-4)-*(pDataIn16+j*Width+2*i+4);
			//
			nTemp[1]=*(pDataIn16+(j-3)*Width+(2*i+1));
			nTemp[2]=*(pDataIn16+(j+3)*Width+(2*i-4));
			nTemp[3]=*(pDataIn16+(j-3)*Width+(2*i-4));
			nTemp[4]=*(pDataIn16+(j+3)*Width+(2*i+1));
			
			//
			nTempc=	nTemp[0]-nTemp[1]-nTemp[2];
			nTempd=	nTemp[0]-nTemp[3]-nTemp[4];
			//
			nTempa=MAX(nTempa,0);
			nTempb=MAX(nTempb,0);
			nTempc=MAX(nTempc,0);
			nTempd=MAX(nTempd,0);
			
			*(pDataOut16+j*Width+2*i)=nTempa+nTempb+nTempc+nTempd;	

			nTemp[0]=*(pDataIn16+j*Width+2*i+1)*2;
			nTempa=nTemp[0]-*(pDataIn16+(j-4)*Width+2*i+1)-*(pDataIn16+(j+4)*Width+2*i+1);
			nTempb=nTemp[0]-*(pDataIn16+j*Width+2*i-3)-*(pDataIn16+j*Width+2*i+5);
			//
			nTemp[1]=*(pDataIn16+(j-3)*Width+(2*i+2));
			nTemp[2]=*(pDataIn16+(j+3)*Width+(2*i-3));
			nTemp[3]=*(pDataIn16+(j-3)*Width+(2*i-3));
			nTemp[4]=*(pDataIn16+(j+3)*Width+(2*i+2));			
			//
			nTempc=	nTemp[0]-nTemp[1]-nTemp[2];
			nTempd=	nTemp[0]-nTemp[3]-nTemp[4];
			//
			nTempa=MAX(nTempa,0);
			nTempb=MAX(nTempb,0);
			nTempc=MAX(nTempc,0);
			nTempd=MAX(nTempd,0);
			
			*(pDataOut16+j*Width+2*i+1)=nTempa+nTempb+nTempc+nTempd;
		}
		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+6);
		*(pDataOut16+j*Width+1)=*(pDataOut16+j*Width+7);
		*(pDataOut16+j*Width+2)=*(pDataOut16+j*Width+4);
		*(pDataOut16+j*Width+3)=*(pDataOut16+j*Width+5);
		
		*(pDataOut16+j*Width+636)=*(pDataOut16+j*Width+634);
		*(pDataOut16+j*Width+637)=*(pDataOut16+j*Width+635);
		*(pDataOut16+j*Width+638)=*(pDataOut16+j*Width+632);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+633);		
	}
}



/**************************************************
* Prototype     : Pre_BKForecast2
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_BKForecast2(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int nWidthL,nWidthR;
	int Width,Height;
	int nTempa=0,nTempb=0,nTempc=0,nTempd=0;
	int nTemp[5]={0,0,0,0,0};
	
	Int16 *pImageDataIn16;
	Int16 *pDataIn16;
	Int16 *pDataOut16;

	Width = nWidth;
	Height=nHeight;
	nWidthL=2;
	nWidthR=nWidth/2-nWidthL;

	pImageDataIn16=pDataIn;
	pDataIn16=pDataIn;
	pDataOut16=pDataOut;

	
	for (j=5;j<Height-5;j++)
	{
		for (i=4;i<636;i++)
		{
// 			nTemp[0]=*(pInData+j*Height+i)*2;
// 			nTempa=nTemp[0]-*(pInData+j*Height+i-4)-*(pInData+j*Height+i+4);
// 			nTempb=nTemp[0]-*(pInData+(j-4)*Height+i)-*(pInData+(j+4)*Height+i);
// 			//
// 			nTemp[1]=*(pInData+(j+1)*Height+i-3);
// 			nTemp[2]=*(pInData+(j-4)*Height+i+3);
// 			nTemp[3]=*(pInData+(j-4)*Height+i-3);
// 			nTemp[4]=*(pInData+(j+1)*Height+i+3);
			//
			
			
			nTemp[0]=*(pDataIn16+j*Width+i)*2;	

			nTempa=nTemp[0]-*(pDataIn16+j*Width+i-4)-*(pDataIn16+j*Width+i+4);
			nTempb=nTemp[0]-*(pDataIn16+(j-4)*Width+i)-*(pDataIn16+(j+4)*Width+i);
			//
			nTemp[1]=*(pDataIn16+(j+1)*Width+i-3);
			nTemp[2]=*(pDataIn16+(j-4)*Width+i+3);
			nTemp[3]=*(pDataIn16+(j-4)*Width+i-3);
			nTemp[4]=*(pDataIn16+(j+1)*Width+i+3);						

			nTempc=	nTemp[0]-nTemp[1]-nTemp[2];
			nTempd=	nTemp[0]-nTemp[3]-nTemp[4];				

			nTempa=MAX(nTempa,0);
			nTempb=MAX(nTempb,0);
			nTempc=MAX(nTempc,0);
			nTempd=MAX(nTempd,0);			

			*(pDataOut16+j*Width+i)=nTempa+nTempb+nTempc+nTempd;
		
		}
		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+6);
		*(pDataOut16+j*Width+1)=*(pDataOut16+j*Width+7);
		*(pDataOut16+j*Width+2)=*(pDataOut16+j*Width+4);
		*(pDataOut16+j*Width+3)=*(pDataOut16+j*Width+5);
		
		*(pDataOut16+j*Width+636)=*(pDataOut16+j*Width+634);
		*(pDataOut16+j*Width+637)=*(pDataOut16+j*Width+635);
		*(pDataOut16+j*Width+638)=*(pDataOut16+j*Width+632);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+633);
		
	}
}


/**************************************************
* Prototype     : Pre_GrayEnhanceFilter
* Description   : 增强目标点灰度
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_GrayEnhanceFilter(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeigh)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	//Int16 hMaxGray,lMaxGray,hMinGray,lMinGray;	
	int nCur=0,nLeft=0,nLeftUp=0,nUp=0,nRightUp=0,nRight=0,nRightDown=0,nDown=0,nLeftDown=0;
	float naverage=0;
	float naverage1,naverage2;
	int   lMax[10];

	Int16 *pDataIn16,*pDataOut16;
	
	
			
	Width  = nWidth;
	Height = nHeigh;			
	pDataIn16=pDataIn;	
	pDataOut16=pDataOut;

	//获取9个点
	for(j=5;j<Height-5;j++)
	{		
		for(i=4;i<Width-4;i++)
		{
			nLeftUp   =*(pDataIn16+(j-1)*Width+(i-1));
			nUp       =*(pDataIn16+(j-1)*Width+(i));
			nRightUp  =*(pDataIn16+(j-1)*Width+(i+1));			
			nLeft     =*(pDataIn16+(j)*Width+(i-1));
			nCur      =*(pDataIn16+(j)*Width+(i));
			nRight    =*(pDataIn16+(j)*Width+(i+1));
			nLeftDown =*(pDataIn16+(j+1)*Width+(i-1));
			nDown     =*(pDataIn16+(j+1)*Width+(i));
			nRightDown=*(pDataIn16+(j+1)*Width+(i+1));			

			naverage=(nLeftUp+nUp+nRightUp+nLeft+nCur
				+nRight+nLeftDown+nDown+nRightDown);

			naverage1 = naverage*Multiply1;
			//naverage2 = naverage*Multiply3;
						
			//增大目标点灰度
			if(nCur>naverage1)
			{								
				nCur=nCur*Multiply2;								
			}
			//else
				//nCur=nCur*Multiply3;

			//if(nCur<naverage2)
			//{
				//nCur=nCur*Multiply4;
			//}
			*(pDataOut16+(j)*Width+(i))=nCur;	
		}	
		*(pDataOut16+j*Width+0)=*(pDataOut16+j*Width+6);
		*(pDataOut16+j*Width+1)=*(pDataOut16+j*Width+7);
		*(pDataOut16+j*Width+2)=*(pDataOut16+j*Width+4);
		*(pDataOut16+j*Width+3)=*(pDataOut16+j*Width+5);
		
		*(pDataOut16+j*Width+636)=*(pDataOut16+j*Width+634);
		*(pDataOut16+j*Width+637)=*(pDataOut16+j*Width+635);
		*(pDataOut16+j*Width+638)=*(pDataOut16+j*Width+632);
		*(pDataOut16+j*Width+639)=*(pDataOut16+j*Width+633);
	}	
}




/**************************************************
* Prototype     : Pre_GrayEnhanceFilter
* Description   : 增强目标点灰度
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
inline void Pre_GrayEnhanceFilter5(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	//Int16 hMaxGray,lMaxGray,hMinGray,lMinGray;	
	int nCur=0,nLeft=0,nLeftUp=0,nUp=0,nRightUp=0,nRight=0,nRightDown=0,nDown=0,nLeftDown=0;
	Uint32 nMean=0;
	float naverage=0;
	float naverage1;
	int   lMax[10];

	Int16 *pDataIn16,*pDataOut16;
	
	
			
	Width  = (nWidth-4)/2;
	Height = nHeight-5;			
	pDataIn16=pDataIn;	
	pDataOut16=pDataOut;


	/*获取均值*/
	for (j=5;j<Height;j++)
	{
		for (i=2;i<Width;i++)
		{			
			nMean+=(*(pDataIn16+j*nWidth+2*i)+*(pDataIn16+j*nWidth+2*i+1));				
		}		
	}
	nMean=nMean*MultiM2/317264;//502*632

	//获取9个点
	for(j=5;j<nHeight-5;j++)
	{		
		for(i=4;i<nWidth-4;i++)
		{
			nCur      =*(pDataIn16+(j)*nWidth+(i));
			if(nCur<nMean)
			{
				*(pDataOut16+(j)*nWidth+(i))=nCur;
				continue;
			}				

			nLeftUp   =*(pDataIn16+(j-1)*nWidth+(i-1));
			nUp       =*(pDataIn16+(j-1)*nWidth+(i));
			nRightUp  =*(pDataIn16+(j-1)*nWidth+(i+1));			
			nLeft     =*(pDataIn16+(j)*nWidth+(i-1));
			//nCur      =*(pDataIn16+(j)*nWidth+(i));
			nRight    =*(pDataIn16+(j)*nWidth+(i+1));
			nLeftDown =*(pDataIn16+(j+1)*nWidth+(i-1));
			nDown     =*(pDataIn16+(j+1)*nWidth+(i));
			nRightDown=*(pDataIn16+(j+1)*nWidth+(i+1));			

			naverage=(nLeftUp+nUp+nRightUp+nLeft+nCur
				+nRight+nLeftDown+nDown+nRightDown);

			naverage1 = naverage*Multiply1;
			//naverage2 = naverage*Multiply3;
						
			//增大目标点灰度
			if(nCur>naverage1)
			{								
				nCur=nCur*Multiply2;								
			}
			//else
				//nCur=nCur*Multiply3;

			//if(nCur<naverage2)
			//{								
				//nCur=nCur*Multiply4;								
			//}
			*(pDataOut16+(j)*nWidth+(i))=nCur;	
		}	
		*(pDataOut16+j*nWidth+0)=*(pDataOut16+j*nWidth+6);
		*(pDataOut16+j*nWidth+1)=*(pDataOut16+j*nWidth+7);
		*(pDataOut16+j*nWidth+2)=*(pDataOut16+j*nWidth+4);
		*(pDataOut16+j*nWidth+3)=*(pDataOut16+j*nWidth+5);
		
		*(pDataOut16+j*nWidth+636)=*(pDataOut16+j*nWidth+634);
		*(pDataOut16+j*nWidth+637)=*(pDataOut16+j*nWidth+635);
		*(pDataOut16+j*nWidth+638)=*(pDataOut16+j*nWidth+632);
		*(pDataOut16+j*nWidth+639)=*(pDataOut16+j*nWidth+633);
	}	
}

/**************************************************
* Prototype     : OBJ_DetectA1
* Description   : 按照500mk时的目标点能够找到来确定阈值，这个会造成虚警升高，
*                     同时使用npixcntlimit1限制虚假目标
* Input           :pMarkDatatemp:只是用来查看分割结果，正式代码不需要获取pOutData
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
OBJ_DetectA1优化

***************************************************/

void OBJ_Detect1st(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nCur=0,nUp=0,nDown=0;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	Int16 nPixel;
	Int16 nLeft;
	Int16 curPixel,rightPixel;
	Int16 result1,result2;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	m_thresh=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1 = nMean*K1st_Mean+nFcha*K1st_Fcha;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		result2	=0;
		rightPixel  =(*(pDataIn16+j*nWidth+4)>m_fLineMean[j])?1:0;
		//rightPixel  =0;
		for (i=4;i<Width;i++)
		{			
			curPixel	  =rightPixel;
			//rightPixel	  =(*(pDataIn16+j*nWidth+i)>m_fLineMean[j])?1:0;
			rightPixel	  =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;;
			result1=result2;
			result2=((curPixel!=0)&&(rightPixel!=0));			
			
			if (result1||result2)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				pseuobject2[i].cx  +=pseuobject[j].cx;
				pseuobject2[i].cy  +=pseuobject[j].cy;
				pseuobject2[i].cnt +=pseuobject[j].cnt;
    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	

	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1_1)
			continue;
		nObjCnt1++;
		pseuobject[nObjCnt1].cx=pseuobject2[i].cx/pseuobject2[i].cnt;
		pseuobject[nObjCnt1].cy=pseuobject2[i].cy/pseuobject2[i].cnt;
		pseuobject[nObjCnt1].nObjmark=1;//和次数复用
		pseuobject[nObjCnt1].cnt=pseuobject2[i].cnt;
	}	

	//再次合并交叉的像素
	for(i=1;i<=nObjCnt1;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;
		for(j=nObjCnt1;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st1)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st1))
			 {    	 		
				pseuobject2[i].cx  +=pseuobject[j].cx;
				pseuobject2[i].cy  +=pseuobject[j].cy;
				pseuobject2[i].cnt +=pseuobject[j].cnt;
    	        pseuobject2[i].nObjmark+= 1; 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		//j= pseuobject2[i].cnt>2?0:1;//小目标时，不使用第一个像素位置计算2次分割阈值，否则阈值过大
		pseuobject[nObjCnt2].cx=pseuobject2[i].cx/pseuobject2[i].nObjmark+1;
		pseuobject[nObjCnt2].cy=pseuobject2[i].cy/pseuobject2[i].nObjmark;		
	}	

#if 1
	//去除第二次分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject[i].cnt==0)
			continue;
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st2)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st2))
			{    	 		
				if(pseuobject[i].cnt>pseuobject[j].cnt)
					pseuobject[j].cnt=0;
				else
					pseuobject[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject[i].cy;
		pseuobject[nObjCnt3].cnt=pseuobject[i].cnt;
	}	
#endif
	/*获取1*3区域最大值*/
	for(nObjCnt=1;nObjCnt<=nObjCnt3;nObjCnt++)
	{
		i=pseuobject[nObjCnt].cx;
		j=pseuobject[nObjCnt].cy;
		nUp       =*(pDataIn16+(j-1)*nWidth+(i));		
		nCur      =*(pDataIn16+(j)*nWidth+(i));		
		nDown     =*(pDataIn16+(j+1)*nWidth+(i));

		if(nUp>nCur)
		{
			pseuobject[nObjCnt].cy=j-1;
			nCur=nUp;
		}
		if(nDown>nCur)
		{
			pseuobject[nObjCnt].cy=j+1;
			nCur=nUp;
		}

	}
	
	npixelcnt = nObjCnt3;
}

/*第一次分割获取一定范围内最大值为
pseudo目标点的初始函数,代码中有bug*/
void OBJ_Detect1stmax(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nCur=0,nUp=0,nDown=0;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	Int16 nPixel;
	Int16 nLeft;
	Int16 curPixel,rightPixel;
	Int16 result1,result2;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	m_thresh=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		result2	=0;
		rightPixel  =(*(pDataIn16+j*nWidth+4)>m_fLineMean[j])?1:0;
		//rightPixel  =0;
		for (i=4;i<Width;i++)
		{			
			curPixel	  =rightPixel;
			//rightPixel	  =(*(pDataIn16+j*nWidth+i)>m_fLineMean[j])?1:0;
			rightPixel	  =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;;
			result1=result2;
			result2=((curPixel!=0)&&(rightPixel!=0));			
			
			if (result1||result2)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject2[i].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[i].nObjmark)
				{
					pseuobject2[i].cx = pseuobject[j].cx;
					pseuobject2[i].cy = pseuobject[j].cy;
					pseuobject2[i].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[i].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	

	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1_1)
			continue;
		nObjCnt1++;
		pseuobject[nObjCnt1].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt1].cy=pseuobject2[i].cy;
		pseuobject[nObjCnt1].nObjmark=pseuobject2[i].nObjmark;//灰度值
		pseuobject[nObjCnt1].cnt=pseuobject2[i].cnt;
	}	

	//再次合并交叉的像素
	for(i=1;i<=nObjCnt1;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;//灰度值
		for(j=nObjCnt1;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject[j].cx)<det1st1)&&
				(abs(pseuobject2[i].cy-pseuobject[j].cy)<det1st1))
			 {   
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[i].nObjmark)
				{
					pseuobject2[i].cx = pseuobject[j].cx;
					pseuobject2[i].cy = pseuobject[j].cy;
					pseuobject2[i].nObjmark= pseuobject[j].nObjmark;
				}				
				pseuobject2[i].cnt +=pseuobject[j].cnt;    	        	
				pseuobject[j].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt2].cy=pseuobject2[i].cy;	
		pseuobject[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完后可以去掉
	}	


	//去除第二次分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject[i].cnt==0)
			continue;
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st2)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st2))
			{    	 		
				if(pseuobject[i].cnt>pseuobject[j].cnt)
					pseuobject[j].cnt=0;
				else
					pseuobject[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject[i].cy;
		pseuobject[nObjCnt3].cnt=pseuobject[i].cnt;//调试完后可以去掉
	}
	/*获取1*3区域最大值*/
	/*和OBJ_Detect1stmaxv1区别:OBJ_Detect1stmaxv1在上面
	获取目标值时，已经处理了nUp和nDown，所有
	OBJ_Detect1stmaxv1不需要获取1*3区域最大值*/
	for(nObjCnt=1;nObjCnt<=nObjCnt3;nObjCnt++)
	{
		i=pseuobject[nObjCnt].cx;
		j=pseuobject[nObjCnt].cy;
		nUp       =*(pDataIn16+(j-1)*nWidth+(i));		
		nCur      =*(pDataIn16+(j)*nWidth+(i));		
		nDown     =*(pDataIn16+(j+1)*nWidth+(i));

		if(nUp>nCur)
		{
			pseuobject[nObjCnt].cy=j-1;
			nCur=nUp;
		}
		if(nDown>nCur)
		{
			pseuobject[nObjCnt].cy=j+1;
			nCur=nUp;
		}

	}
	npixelcnt = nObjCnt3;
}




//再次合并交叉的像素,代码存在bug
void OBJ_Detect1stmaxv1(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nCur=0,nUp=0,nDown=0;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	Int16 nPixel;
	Int16 nLeft;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		//curPixel  =*(pDataIn16+j*nWidth+3);
		curPixel  =0;
		rightPixel  =(*(pDataIn16+j*nWidth+4)>m_fLineMean[j])?1:0;
		for (i=4;i<Width;i++)
		{			
			leftPixel = curPixel;
			curPixel  =rightPixel;
			rightPixel	  =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;
			if(*(pDataIn16+j*nWidth+i)<m_thresh1)
				continue;
			
			upPixel = (*(pDataIn16+(j-1)*nWidth+i)>m_fLineMean[j])?1:0;
			downPixel = (*(pDataIn16+(j+1)*nWidth+i)>m_fLineMean[j])?1:0;

			
	
			result1 = leftPixel+curPixel+rightPixel+upPixel+downPixel;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>1)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject2[i].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[i].nObjmark)
				{
					pseuobject2[i].cx = pseuobject[j].cx;
					pseuobject2[i].cy = pseuobject[j].cy;
					pseuobject2[i].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[i].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	

	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1_1)
			continue;
		nObjCnt1++;
		pseuobject[nObjCnt1].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt1].cy=pseuobject2[i].cy;
		pseuobject[nObjCnt1].nObjmark=pseuobject2[i].nObjmark;//灰度值
		pseuobject[nObjCnt1].cnt=pseuobject2[i].cnt;
	}	

	//再次合并交叉的像素
	/*下面代码存在错误，pseuobject2已经使用，使用前没有
	再次初始化，导致部分问题产生*/
	for(i=1;i<=nObjCnt1;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;//灰度值
		for(j=nObjCnt1;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject[j].cx)<det1st1)&&
				(abs(pseuobject2[i].cy-pseuobject[j].cy)<det1st1))
			 {   
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[i].nObjmark)
				{
					pseuobject2[i].cx = pseuobject[j].cx;
					pseuobject2[i].cy = pseuobject[j].cy;
					pseuobject2[i].nObjmark= pseuobject[j].nObjmark;
				}				
				pseuobject2[i].cnt +=pseuobject[j].cnt;    	        	
				pseuobject[j].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt2].cy=pseuobject2[i].cy;	
		pseuobject[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完后可以去掉
	}	


	//去除第二次分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject[i].cnt==0)
			continue;
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st2)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st2))
			{    	 		
				if(pseuobject[i].cnt>pseuobject[j].cnt)
					pseuobject[j].cnt=0;
				else
					pseuobject[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject[i].cy;
		pseuobject[nObjCnt3].cnt=pseuobject[i].cnt;//调试完后可以去掉
	}
	
	npixelcnt = nObjCnt3;
}








//OBJ_Detect1stmaxv1再次合并交叉的像素,代码存在bug
//OBJ_Detect1stmaxv1修订函数
void OBJ_Detect1stmaxv1rev(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nCur=0,nUp=0,nDown=0;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	int nObjCnt4;
	Int16 nPixel;
	Int16 nLeft;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	nObjCnt4=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		//curPixel  =*(pDataIn16+j*nWidth+3);
		curPixel  =0;
		rightPixel  =(*(pDataIn16+j*nWidth+4)>m_fLineMean[j])?1:0;
		for (i=4;i<Width;i++)
		{			
			leftPixel = curPixel;
			curPixel  =rightPixel;
			rightPixel	  =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;
			if(*(pDataIn16+j*nWidth+i)<m_thresh1)
				continue;
			
			upPixel = (*(pDataIn16+(j-1)*nWidth+i)>m_fLineMean[j])?1:0;
			downPixel = (*(pDataIn16+(j+1)*nWidth+i)>m_fLineMean[j])?1:0;

			
	
			result1 = leftPixel+curPixel+rightPixel+upPixel+downPixel;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>1)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;
		pseuobject2[i].cx =pseuobject[i].cx;
		pseuobject2[i].cy =pseuobject[i].cy;
		pseuobject2[i].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[i].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject2[i].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[i].nObjmark)
				{
					pseuobject2[i].cx = pseuobject[j].cx;
					pseuobject2[i].cy = pseuobject[j].cy;
					pseuobject2[i].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[i].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	

	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1_1)
			continue;
		nObjCnt1++;
		pseuobject[nObjCnt1].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt1].cy=pseuobject2[i].cy;
		pseuobject[nObjCnt1].nObjmark=pseuobject2[i].nObjmark;//灰度值
		pseuobject[nObjCnt1].cnt=pseuobject2[i].cnt;
	}	

	//再次合并交叉的像素
	/*下面代码存在错误，pseuobject2已经使用，使用前没有
	再次初始化，导致部分问题产生，使用nObjCnt2解决问题*/
	for(i=1;i<=nObjCnt1;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;

		nObjCnt2++;
		pseuobject2[nObjCnt2].cx =pseuobject[i].cx;
		pseuobject2[nObjCnt2].cy =pseuobject[i].cy;
		pseuobject2[nObjCnt2].cnt=pseuobject[i].cnt;
		pseuobject2[nObjCnt2].nObjmark=pseuobject[i].nObjmark;//灰度值
		for(j=nObjCnt1;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[nObjCnt2].cx-pseuobject[j].cx)<det1st1)&&
				(abs(pseuobject2[nObjCnt2].cy-pseuobject[j].cy)<det1st1))
			 {   
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[nObjCnt2].nObjmark)
				{
					pseuobject2[nObjCnt2].cx = pseuobject[j].cx;
					pseuobject2[nObjCnt2].cy = pseuobject[j].cy;
					pseuobject2[nObjCnt2].nObjmark= pseuobject[j].nObjmark;
				}				
				pseuobject2[nObjCnt2].cnt +=pseuobject[j].cnt;    	        	
				pseuobject[j].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject2[i].cy;	
		pseuobject[nObjCnt3].cnt=pseuobject2[i].cnt;
		pseuobject[nObjCnt3].nObjmark=pseuobject2[i].nObjmark;//调试完成后可以去掉
	}	


	//去除第二次分割时可能重合的目标位置
	for(i=1;i<=nObjCnt3;i++)
	{
		//跳过已经去除的目标
		if(pseuobject[i].cnt==0)
			continue;
		for(j=nObjCnt3;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[i].cx-pseuobject[j].cx)<det1st2)&&
				(abs(pseuobject[i].cy-pseuobject[j].cy)<det1st2))
			{    	 		
				if(pseuobject[i].cnt>pseuobject[j].cnt)
					pseuobject[j].cnt=0;
				else
					pseuobject[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt3;i++)
	{
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt4++;
		pseuobject[nObjCnt4].cx=pseuobject[i].cx;
		pseuobject[nObjCnt4].cy=pseuobject[i].cy;
		pseuobject[nObjCnt4].cnt=pseuobject[i].cnt;//调试完成后可以去掉
	}
	
	npixelcnt = nObjCnt4;
}


/*简化OBJ_Detect1stmaxv1rev函数，结果一样*/
void OBJ_Detect1stmaxv2(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nCur=0,nUp=0,nDown=0;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	Int16 nPixel;
	Int16 nLeft;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		//curPixel  =*(pDataIn16+j*nWidth+3);
		curPixel  =0;
		rightPixel  =(*(pDataIn16+j*nWidth+4)>m_fLineMean[j])?1:0;
		for (i=4;i<Width;i++)
		{			
			leftPixel = curPixel;
			curPixel  =rightPixel;
			rightPixel	  =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;
			if(*(pDataIn16+j*nWidth+i)<m_thresh1)
				continue;
			
			upPixel = (*(pDataIn16+(j-1)*nWidth+i)>m_fLineMean[j])?1:0;
			downPixel = (*(pDataIn16+(j+1)*nWidth+i)>m_fLineMean[j])?1:0;

			
	
			result1 = leftPixel+curPixel+rightPixel+upPixel+downPixel;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>1)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;

		nObjCnt1++;
		pseuobject2[nObjCnt1].cx =pseuobject[i].cx;
		pseuobject2[nObjCnt1].cy =pseuobject[i].cy;
		pseuobject2[nObjCnt1].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[nObjCnt1].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[nObjCnt1].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject2[nObjCnt1].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[nObjCnt1].nObjmark)
				{
					pseuobject2[nObjCnt1].cx = pseuobject[j].cx;
					pseuobject2[nObjCnt1].cy = pseuobject[j].cy;
					pseuobject2[nObjCnt1].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[nObjCnt1].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject2[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject2[nObjCnt2].cy=pseuobject2[i].cy;
		pseuobject2[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject2[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完成后可以去掉
	}

	//去除分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject2[i].cnt==0)
			continue;
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject2[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject2[j].cx)<det1st2)&&
				(abs(pseuobject2[i].cy-pseuobject2[j].cy)<det1st2))
			{    	 		
				if(pseuobject2[i].cnt>pseuobject2[j].cnt)
					pseuobject2[j].cnt=0;
				else
					pseuobject2[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject2[i].cy;
		pseuobject[nObjCnt3].cnt=pseuobject2[i].cnt;//调试完成后可以去掉
	}
	
	npixelcnt = nObjCnt3;
}




/*使用9连通获取分割点*/
void OBJ_Detect1stmaxv3(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	Int16 nPixel;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 nCur,nLeft,nRight,nLeftUp,nUp,nRightUp,nRightDown,nDown,nLeftDown;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=6;j<nHeight-6;j++)
	{
		for (i=4;i<Width;i++)
		{			
			if(*(pDataIn16+j*nWidth+i)<m_fLineMean[j])
				continue;

			nLeftUp=(*(pDataIn16+(j-1)*nWidth+i-1)>m_fLineMean[j-1])?1:0;
			nUp=(*(pDataIn16+(j-1)*nWidth+i)>m_fLineMean[j-1])?1:0;
			nRightUp =(*(pDataIn16+(j-1)*nWidth+i+1)>m_fLineMean[j-1])?1:0;

			nLeft=(*(pDataIn16+j*nWidth+i-1)>m_fLineMean[j])?1:0;
			nRight =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;

			nLeftDown=(*(pDataIn16+(j+1)*nWidth+i-1)>m_fLineMean[j+1])?1:0;
			nDown=(*(pDataIn16+(j+1)*nWidth+i)>m_fLineMean[j+1])?1:0;
			nRightDown =(*(pDataIn16+(j+1)*nWidth+i+1)>m_fLineMean[j+1])?1:0;
			
			result1 = nLeft+nRight+nLeftUp+nUp+nRightUp+nRightDown+nDown+nLeftDown;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>0)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=6;j<nHeight-6;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;

		nObjCnt1++;
		pseuobject2[nObjCnt1].cx =pseuobject[i].cx;
		pseuobject2[nObjCnt1].cy =pseuobject[i].cy;
		pseuobject2[nObjCnt1].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[nObjCnt1].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject2[nObjCnt1].cx-pseuobject[j].cx)<det1st)&&
				(abs(pseuobject2[nObjCnt1].cy-pseuobject[j].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[nObjCnt1].nObjmark)
				{
					pseuobject2[nObjCnt1].cx = pseuobject[j].cx;
					pseuobject2[nObjCnt1].cy = pseuobject[j].cy;
					pseuobject2[nObjCnt1].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[nObjCnt1].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject2[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject2[nObjCnt2].cy=pseuobject2[i].cy;
		pseuobject2[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject2[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完成后可以去掉
	}

	//去除分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject2[i].cnt==0)
			continue;
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject2[j].cnt==0)
				continue;
			if((abs(pseuobject2[i].cx-pseuobject2[j].cx)<det1st2)&&
				(abs(pseuobject2[i].cy-pseuobject2[j].cy)<det1st2))
			{    	 		
				if(pseuobject2[i].cnt>pseuobject2[j].cnt)
					pseuobject2[j].cnt=0;
				else
					pseuobject2[i].cnt=0;				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt2;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt3++;
		pseuobject[nObjCnt3].cx=pseuobject2[i].cx;
		pseuobject[nObjCnt3].cy=pseuobject2[i].cy;
		pseuobject[nObjCnt3].cnt=pseuobject2[i].cnt;//调试完成后可以去掉
	}
	
	npixelcnt = nObjCnt3;
}






/*使用6连通获取分割点*/
void OBJ_Detect1stmaxv4(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度	
	int nMean,nFcha;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	int nObjCnt4;
	Int16 nPixel;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 nCur,nLeft,nRight,nLeftUp,nUp,nRightUp,nRightDown,nDown,nLeftDown;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;	
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	nObjCnt4=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=6;j<nHeight-6;j++)
	{
		for (i=4;i<Width;i++)
		{			
			if(*(pDataIn16+j*nWidth+i)<m_fLineMean[j])
				continue;

			nLeftUp=(*(pDataIn16+(j-1)*nWidth+i-1)>m_fLineMean[j-1])?1:0;
			nUp=(*(pDataIn16+(j-1)*nWidth+i)>m_fLineMean[j-1])?1:0;
			nRightUp =(*(pDataIn16+(j-1)*nWidth+i+1)>m_fLineMean[j-1])?1:0;

			nLeft=(*(pDataIn16+j*nWidth+i-1)>m_fLineMean[j])?1:0;
			nRight =(*(pDataIn16+j*nWidth+i+1)>m_fLineMean[j])?1:0;

			
			
			result1 = nLeft+nRight+nLeftUp+nUp+nRightUp;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>0)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	for (j=6;j<nHeight-6;j++)
	{
		for (i=4;i<Width;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并3*3区域交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;

		nObjCnt1++;
		pseuobject2[nObjCnt1].cx =pseuobject[i].cx;
		pseuobject2[nObjCnt1].cy =pseuobject[i].cy;
		pseuobject2[nObjCnt1].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[nObjCnt1].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[j].cx-pseuobject2[nObjCnt1].cx)<det1st)&&
				(abs(pseuobject[j].cy-pseuobject2[nObjCnt1].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[nObjCnt1].nObjmark)
				{
					pseuobject2[nObjCnt1].cx = pseuobject[j].cx;
					pseuobject2[nObjCnt1].cy = pseuobject[j].cy;
					pseuobject2[nObjCnt1].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[nObjCnt1].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject2[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject2[nObjCnt2].cy=pseuobject2[i].cy;
		pseuobject2[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject2[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完成后可以去掉
	}

	//去除5*5区域内分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject2[i].cnt==0)
			continue;

		nObjCnt3++;
		pseuobject[nObjCnt3].cx =pseuobject2[i].cx;
		pseuobject[nObjCnt3].cy =pseuobject2[i].cy;
		pseuobject[nObjCnt3].nObjmark=pseuobject2[i].nObjmark;
		pseuobject[nObjCnt3].cnt=pseuobject2[i].cnt;
		
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject2[j].cnt==0)
				continue;
			if((abs(pseuobject2[j].cx-pseuobject[nObjCnt3].cx)<det1st2)&&
			    (abs(pseuobject2[j].cy-pseuobject[nObjCnt3].cy)<det1st2))
			{    	 		
				if(pseuobject2[j].cnt>pseuobject[nObjCnt3].cnt)
				{
					pseuobject[nObjCnt3].cx=pseuobject2[j].cx;
					pseuobject[nObjCnt3].cy=pseuobject2[j].cy;
					pseuobject[nObjCnt3].nObjmark=pseuobject2[i].nObjmark;
					
				}
				pseuobject[nObjCnt3].cnt+=pseuobject2[j].cnt;
				pseuobject2[j].cnt=0;
			}
			
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt3;i++)
	{	
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt4++;
		pseuobject[nObjCnt4].cx=pseuobject[i].cx;
		pseuobject[nObjCnt4].cy=pseuobject[i].cy;
		pseuobject[nObjCnt4].cnt=pseuobject[i].cnt;//调试完成后可以去掉
	}
	
	npixelcnt = nObjCnt4;
}








/*使用新的3连通获取分割点*/
void OBJ_Detect1stmaxv5(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int Width,Height;//dsp存储空间需要的宽度和长度
	int Width2,Height2;//2次分割范围
	int nMean,nFcha;
	int nObjCnt;
	int nObjCnt1;
	int nObjCnt2;
	int nObjCnt3;
	int nObjCnt4;
	Int16 nPixel;
	Int16 leftPixel,curPixel,rightPixel,upPixel,downPixel;
	Int16 nCur,nLeft,nRight,nLeftUp,nUp,nRightUp,nRightDown,nDown,nLeftDown;
	Int16 result1;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	long long *pMarkData64; 
	
	//初始化变量	
	Width = nWidth-4;
	Height=nHeight-5;
	Width2 = nWidth-8;
	Height2=nHeight-9;
	nMean=0;
	nFcha=0;
	nObjCnt=0;
	nObjCnt1=0;
	nObjCnt2=0;
	nObjCnt3=0;
	nObjCnt4=0;
	m_thresh=0;
	m_thresh1=0;
	pDataIn16=pDataIn;	
	pMarkData16=pMarkDatatemp;
	pMarkData64 = (long long *)pMarkDatatemp;
	
	
	//初始化pMarkData64,生成分割图像	
	for (j=0;j<81920;j++)
	{		
		pMarkData64[j]=0;		
	}

	/*获取行均值*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineMean[j]+=*(pDataIn16+j*nWidth+i);				
		}		
		m_fLineMean[j]=m_fLineMean[j]/(nWidth-4*2);
		nMean+=m_fLineMean[j];//nMean最大值不会超过0x7FFFFFFFF	
	}
	nMean=nMean/(nHeight-5*2);


	
	
	/*获取行方差*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			m_fLineFcha[j]+=(*(pDataIn16+j*nWidth+i)-m_fLineMean[j])*(*(pDataIn16+j*nWidth+i)-m_fLineMean[j]);			
		}
		m_fLineFcha[j]=sqrt(m_fLineFcha[j]/(nWidth-4*2));
		nFcha+=m_fLineFcha[j];//nFcha最大值不会超过0x7FFFFFFFF
	}
	nFcha=nFcha/(nHeight-5*2);

	m_thresh = 	nMean*TotalMean+nFcha*TotalFcha;
	m_thresh1= 	nMean*K1st_Mean1+nFcha*K1st_Fcha1;

	//得到行阈值
	for (i=5;i<Height;i++)
	{
		//m_fLineMean[i]=MAX(m_fLineMean[i],nMean);
		//m_fLineFcha[i]=MAX(m_fLineFcha[i],nFcha);

		m_fLineMean[i]=m_fLineMean[i]>nMean?m_fLineMean[i]:nMean;
		m_fLineFcha[i]=m_fLineFcha[i]>nFcha?m_fLineFcha[i]:nFcha;
		//nLineMean[i]=nLineFCha[i]*3;
		//pfLineMean[i]=pfLineMean[i]*2+4*pfLineFcha[i];
		m_fLineMean[i]=m_fLineMean[i]*K1st_Mean+m_fLineFcha[i]*K1st_Fcha;
	}	
	
	//pDataIn16数据有效范围,i:5到(nWidth-5),j:5到(nWidth-5)
	/*去掉孤点，生成标号图像pMarkData16*/
	for (j=5;j<Height;j++)
	{
		for (i=4;i<Width;i++)
		{			
			
			if(*(pDataIn16+j*nWidth+i)<m_thresh1)
				continue;
			
			leftPixel   =(m_fLineMean[j]>*(pDataIn16+j*nWidth+i-1))?0:1;
			curPixel  	=  (m_fLineMean[j]>*(pDataIn16+j*nWidth+i))?0:1;
			rightPixel	=(m_fLineMean[j]>*(pDataIn16+j*nWidth+i+1))?0:1;

			
	
			result1 = leftPixel+curPixel+rightPixel;
			//3个相邻数据2个为1，就认为是pseudo目标
			if (result1>1)
			{
				*(pMarkData16+j*nWidth+i)=600;
			}		
		}
		*(pMarkData16+j*nWidth+4)=0;
		//*(pMarkData16+j*nWidth+5)=0;
		//*(pMarkData16+j*nWidth+634)=0;
		*(pMarkData16+j*nWidth+635)=0;
	}

	/*获取分割坐标*/
	/*考虑2次分割获取有效边界点，需避开边界*/
	for (j=9;j<Height2;j++)
	{
		for (i=8;i<Width2;i++)
		{
			if (*(pMarkData16+j*nWidth+i)>0)
			{
				nObjCnt++;
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				pseuobject[nObjCnt].cnt=1;
				pseuobject[nObjCnt].nObjmark=*(pDataIn16+j*nWidth+i);//获取图像灰度
			}		
		}
		
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	

	//合并3*3区域交叉的像素
	for(i=1;i<=nObjCnt;i++)
	{
		//去除已经合并的目标
		if(pseuobject[i].cnt==0)
			continue;

		nObjCnt1++;
		pseuobject2[nObjCnt1].cx =pseuobject[i].cx;
		pseuobject2[nObjCnt1].cy =pseuobject[i].cy;
		pseuobject2[nObjCnt1].nObjmark=pseuobject[i].nObjmark;
		pseuobject2[nObjCnt1].cnt=pseuobject[i].cnt;
		for(j=nObjCnt;j>i;j--)
		{
			//去除已经合并的目标
			if(pseuobject[j].cnt==0)
				continue;
			if((abs(pseuobject[j].cx-pseuobject2[nObjCnt1].cx)<det1st)&&
				(abs(pseuobject[j].cy-pseuobject2[nObjCnt1].cy)<det1st))
			 {    	 		
				//获取最大灰度值的位置为中心
				if(pseuobject[j].nObjmark>pseuobject2[nObjCnt1].nObjmark)
				{
					pseuobject2[nObjCnt1].cx = pseuobject[j].cx;
					pseuobject2[nObjCnt1].cy = pseuobject[j].cy;
					pseuobject2[nObjCnt1].nObjmark= pseuobject[j].nObjmark;
				}
				pseuobject2[nObjCnt1].cnt +=pseuobject[j].cnt;    	        	 		
				pseuobject[j].cnt=0;
				
			}
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt1;i++)
	{
		if(pseuobject2[i].cnt<npixcntlimit1)
			continue;
		nObjCnt2++;
		pseuobject2[nObjCnt2].cx=pseuobject2[i].cx;
		pseuobject2[nObjCnt2].cy=pseuobject2[i].cy;
		pseuobject2[nObjCnt2].cnt=pseuobject2[i].cnt;
		pseuobject2[nObjCnt2].nObjmark=pseuobject2[i].nObjmark;//调试完成后可以去掉
	}

	//去除5*5区域内分割时可能重合的目标位置
	for(i=1;i<=nObjCnt2;i++)
	{
		//跳过已经去除的目标
		if(pseuobject2[i].cnt==0)
			continue;

		nObjCnt3++;
		pseuobject[nObjCnt3].cx =pseuobject2[i].cx;
		pseuobject[nObjCnt3].cy =pseuobject2[i].cy;
		pseuobject[nObjCnt3].nObjmark=pseuobject2[i].nObjmark;
		pseuobject[nObjCnt3].cnt=pseuobject2[i].cnt;
		
		for(j=nObjCnt2;j>i;j--)
		{
			//跳过已经去除的目标
			if(pseuobject2[j].cnt==0)
				continue;
			if((abs(pseuobject2[j].cx-pseuobject[nObjCnt3].cx)<det1st2)&&
			    (abs(pseuobject2[j].cy-pseuobject[nObjCnt3].cy)<det1st2))
			{    
				//获取最大灰度值的位置为中心	 		
				if(pseuobject2[j].nObjmark>pseuobject[nObjCnt3].nObjmark)
				{
					pseuobject[nObjCnt3].cx=pseuobject2[j].cx;
					pseuobject[nObjCnt3].cy=pseuobject2[j].cy;
					pseuobject[nObjCnt3].nObjmark=pseuobject2[i].nObjmark;
					
				}
				pseuobject[nObjCnt3].cnt+=pseuobject2[j].cnt;
				pseuobject2[j].cnt=0;
			}
			
		}
	}	
	/*获取和并后的目标信息*/
	for(i=1;i<=nObjCnt3;i++)
	{	
		if(pseuobject[i].cnt<npixcntlimit1)
			continue;
		nObjCnt4++;
		pseuobject[nObjCnt4].cx=pseuobject[i].cx;
		pseuobject[nObjCnt4].cy=pseuobject[i].cy;
		pseuobject[nObjCnt4].cnt=pseuobject[i].cnt;//调试完成后可以去掉
	}
	
	npixelcnt = nObjCnt4;
}


/**************************************************
* Prototype     : Pre_DataMove2
* Description   : 
* Input           :
* Return Value :  

* Date            : 20181009
* Author   	 : 
* Modification  : 
***************************************************/
void Pre_DataMove2(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
{
	int i,j;
	int Height,Width;
	Int32 Graysrc1;
	Int32 Graysrc2;
	Int32 Graytemp;
	Int32 *pDataIn32;
	Int32 *pDataOut32;
	
	
	//WidthAddr  = nWidth/2;
	//HeightAddr = nHeight;

			
	Width  = nWidth>>1;
	Height = nHeight;	
	//nWidth1 = nWidth;
	pDataIn32 =(Int32 *)pDataIn;
	pDataOut32=(Int32 *)pDataOut;
	for (j=5;j<Height;j++)
	{
		for (i=2;i<Width-2;i++)
		{			
			
			Graysrc2= *(pDataIn32+j*Width+i);
			Graytemp=_sub2(Graysrc2,m_fLineMean[j]);//_sub2结果:(Graysrc2-Graysrc1	)	
			*(pDataOut32+j*Width+i)=_max2(0,Graytemp);	
		}
	}		
}

/**************************************************
* Prototype     : OBJ_Detect2ndOut
* Description   : 根据工作模式在对应的成像获取目标位置
* Input           :pMarkDatatemp,查看分割结果图像，正式代码不需要获取
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
第2次分割结果

***************************************************/
void OBJ_Detect2ndOut(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	int nWidth1,nHeight1;
	int nObjCnt;

	Int16 *pDataIn16;
	Int16 *pMarkData16;	
	long long *pseuobject64;
	long long *pMarkData64;
	//初始化变量	
	nObjCnt =npixelcnt;
	
	
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	pseuobject64 = (long long *)pseuobject;
	pMarkData64 = (long long *)pMarkDatatemp;

	//使用跟踪调试搜索，上送所有目标
	if((WorkMode==Mode_Scan)||(WorkMode==Mode_ReCapture))
	{		
		nWidth1  = 8;
		nHeight1 = 8;
	}	
	else	
	{			
		//nWidth1  = 80;
		//nHeight1 = 64;
		
		//nWidth1  = 4;
		//nHeight1 = 5;
		//整机漏光现象，临时调整
		nWidth1  = gtrackWidth1;
		nHeight1 = gtrackHeight1;
		
	}
	//输出标号图像需要初始化	
	//初始化pMarkDataOut,生成分割的标号图像	
	for (i=0;i<81920;i++)
	{		
		pMarkData64[i]=0;		
	}
	/*获取中间过程需要的WH图像*/
	for (i=1;i<=nObjCnt;i++)
	{	
		if(m_fDela[i]==0x7fff)
			continue;
		cxleft=pseuobject[i].cx-nW_K3/2;
		cytop =pseuobject[i].cy-nH_K3/2;

		//OBJ_Detect1stmaxv5中考虑了边界问题，
		//减小了获取阈值范围，不需要下面判断了
		//cxleft=(cxleft<4)?4:(cxleft>(nWidth-4-nW_K3))?(nWidth-4-nW_K3):cxleft;
		//cytop=(cytop<5)?5:(cytop>(nHeight-5-nH_K3))?(nHeight-5-nH_K3):cytop;


		for (a=cytop;a<cytop+nH_K3;a++)
		{
			for (b=cxleft;b<cxleft+nW_K3;b++)
			{					
				*(pMarkData16+a*nWidth+b)=*(pDataIn16+a*nWidth+b)>m_fDela[i]?600:0;				
			}
		}
	}
	/*初始化pseuobject*/
	for (j=0;j<1024;j++)
	{	
		pseuobject64[j]=0;
	}
	nObjCnt=0;
	
	
	/*获取2次分割后的目标位置*/
	for (j=nHeight1;j<nHeight-nHeight1;j++)
	{
		for (i=nWidth1;i<nWidth-nWidth1;i++)
		{
			if (*(pMarkData16+j*nWidth+i)!=0)
			{
				nObjCnt++;				
				pseuobject[nObjCnt].cx=i;
				pseuobject[nObjCnt].cy=j;
				//调试过程中显示分割目标，正式代码去掉
				*(pMarkData16+j*nWidth+i)=nObjCnt+600;
			}
		}
		if(nObjCnt>480) 
		{
			nObjCnt=480;
			break;
		}
	}
	npixelcnt=nObjCnt;
}



//优化2
/**************************************************
* Prototype     : OBJ_Detect2
* Description   : pMarkDatatemp:查看分割结果图像，正式代码不需要获取
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
第2次分割

OBJ_Detect优化2

***************************************************/

void OBJ_Detect2nd(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	
	int nObjCnt;
	int K1Width;//目标点区域1大小
	int K1Height;
	int K2Width;//目标点区域2大小(范围小于区域1)
	int K2Height;
	
	int nMaxX;
	int nMaxY;
	int nCNT2,nCNT2b,nCNT2l,nthCNT;	
	Int16 leftupgray,leftdowngray,rightupgray,rightdowngray;
	Int16 nMaxGray;
	Int16 nSubMaxGray;
	
	float m_fLocalMean1sum;
	float m_fLocalMean1;
	float m_fLocalMean2sum;
	float m_fLocalMean2;
	float m_fLocalMean2bsum;
	float m_fLocalMean2b;
	float m_fLocalFcha2sum;
	float m_fLocalMean2l;
	float para1;
	float para2;
	float para3;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	
	//////////////////////////////////////////////////////////////////////////
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	
	K2Width = nW_K2;
	K2Height = nW_K2;
	nObjCnt = npixelcnt;
	nCNT2=nW_K2*nH_K2;
	
	nMaxX=0;
	nMaxY=0;
	for (i=1;i<=nObjCnt;i++)
	{
		m_fLocalMean2sum=0.01;
		m_fLocalMean1sum=0;
		m_fLocalMean2bsum=0;
		m_fLocalMean2b=0;
		m_fLocalMean2l=0.01;
		nCNT2b=0;
		nCNT2l=0;
		nthCNT=0;
		cxleft=pseuobject[i].cx-K2Width/2;
		cytop=pseuobject[i].cy-K2Height/2;	
		
		//获得目标点中心区域2的均值
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalMean2sum+=*(pDataIn16+a*nWidth+b);
				if (*(pDataIn16+a*nWidth+b)>m_thresh)
				{
					nthCNT++;
				}
			}

		}
		m_fLocalMean2=m_fLocalMean2sum/nCNT2;

		//5*5区域内没有3个超过阈值的，认为这个区域没有目标
		if(nthCNT<3)		
			continue;
		
		//获得目标点中心区域2的大于均值的个数
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				if (*(pDataIn16+a*nWidth+b)>m_fLocalMean2)
				{
					nCNT2b++;
					m_fLocalMean2bsum+=*(pDataIn16+a*nWidth+b);
				}
				
			}
		}

		//20180328debug 信息，最后去掉
		nCNT2l=nCNT2-nCNT2b;
		if(nCNT2l!=0)
		m_fLocalMean2l = (m_fLocalMean2sum - m_fLocalMean2bsum)/nCNT2l;
		//20180328debug 信息，最后去掉
		
		if(nCNT2b!=0)
		m_fLocalMean2b =m_fLocalMean2bsum/nCNT2b;
				
		//20180328debug 信息，最后去掉
		j= i%100;
		m_fLineFcha[5*j]=m_fLocalMean2;
		m_fLineFcha[5*j+1]=m_fLocalMean2b;
		m_fLineFcha[5*j+2]=nCNT2b;
		m_fLineFcha[5*j+3]=m_fLocalMean2l;
		m_fLineFcha[5*j+4]=nCNT2l;
		//20180328debug 信息，最后去掉

		para1 = m_fLocalMean2bsum/m_fLocalMean2sum;
		para2 = nCNT2b/nCNT2;
		para3=m_fLocalMean2b/m_fLocalMean2l;

//调试去掉下面的，使用全局变量代替
#if 0
		//条件old0
		if((m_fLocalMean2b<m_thresh*0.70)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		if((m_fLocalMean2b<m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh;
		if((m_fLocalMean2b>m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*1.1;
		if((m_fLocalMean2>m_thresh*1.1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2;


		if(para1<0.55)
			m_fLocalMean2b=0x7fff;
		if((para1>0.74)&&(nCNT2b<11))
			m_fLocalMean2b=0x7fff;
		if((para1>0.88)&&(nCNT2b<18))
			m_fLocalMean2b=0x7fff;
#endif	

#if 0	
		//条件old1,和条件old0一样，使用全局变量方便调试
		//测试600mk实验室视多帧频数据0-200frame，目标搜索稳定，
		//虚警比条件2和条件3少，但是也有一定的虚警，3-4个虚警

		//测试400mk实验室视多帧频数据0-100frame，目标搜索稳定，
		//存在几帧数据丢失目标现象，也有一定的虚警，3-4个虚警
		if((m_fLocalMean2b<m_thresh*mthreshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		if((m_fLocalMean2b<m_thresh*mthreshratio3)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio3;
		//if((m_fLocalMean2b<m_thresh)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=m_thresh;
		if((m_fLocalMean2b>m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio2;
		if((m_fLocalMean2>m_thresh*mthreshratio2)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2;


		if(para1<para1thresh1)
			m_fLocalMean2b=0x7fff;
		if((para1>para1thresh2)&&(nCNT2b<para1num2))
			m_fLocalMean2b=0x7fff;
		if((para1>para1thresh3)&&(nCNT2b<para1num3))
			m_fLocalMean2b=0x7fff;
#endif
#if 0
		//条件old2
		//简化old1,这个方法效果不好，基本不用
		//代替case1的代码，从能量角度分析
		//效果不好
		
		if((para1<para1thresh1)||(para3<para3n1)||(para3>para3n2))
			m_fLocalMean2b=0x7fff;

		if((m_fLocalMean2b<m_thresh*mthreshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		if((m_fLocalMean2b<m_thresh*mthreshratio3)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio3;
		//if((m_fLocalMean2b<m_thresh)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=m_thresh;
		if((m_fLocalMean2b>m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio2;
		if((m_fLocalMean2>m_thresh*mthreshratio2)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2;
#endif
#if 0
		//条件old3
		//测试600mk实验室视多帧频数据0-200frame，目标搜索不稳定，虚警高
		//测试400mk实验室视多帧频数据0-100frame，目标搜索不稳定，虚警高

		if((m_fLocalMean2b<m_thresh*mthreshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		if((m_fLocalMean2b<m_thresh*mthreshratio3)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio3;
		//if((m_fLocalMean2b<m_thresh)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=m_thresh;
		if((m_fLocalMean2b>m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh*mthreshratio2;
		if((m_fLocalMean2>m_thresh*mthreshratio2)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2;

		//根据高斯能量分布分析和实际图像调试得出
		//下面和条件old1不一样

		
		if(nCNT2b>=para2num3)
			m_fLocalMean2b=(para1<para2thresh3)?0x7fff:m_fLocalMean2b;
		
		else if(nCNT2b>=para2num2)
			m_fLocalMean2b=(para1<para2thresh2)?0x7fff:m_fLocalMean2b;

		else if(nCNT2b>=para2num1)
			m_fLocalMean2b=(para1<para2thresh1)?0x7fff:m_fLocalMean2b;

		if(para1<para2thresh0)
			m_fLocalMean2b=0x7fff;
#endif
#if 0
		//条件2
		//测试600mk实验室视多帧频数据0-200frame，目标搜索不稳定，虚警高
		//对试验室数据不是很好
		//青海外场数据很好

		//虚警高
		//代替case1的代码，从能量角度分析

		if((m_fLocalMean2b<m_thresh*mthreshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		//上面已经要求5*5区域至少有3个值超过阈值
		//对于小于阈值的，m_fLocalMean2b用阈值代替
		if((m_fLocalMean2b<m_thresh*mthreshratio4)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh;
		//if((m_fLocalMean2b<m_thresh*mthreshratio5)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=(m_fLocalMean2b*mthreshratio5>m_thresh)?m_fLocalMean2b*mthreshratio5:m_thresh;
		//提高亮目标的有效个数
		if((m_fLocalMean2b>m_thresh*mthreshratio5)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2b*mthreshratio5ftr;
		

		//根据高斯能量分布分析和实际图像调试得出 	
		if(nCNT2b>=para2num3)
			m_fLocalMean2b=(para1<para2thresh3)?0x7fff:m_fLocalMean2b;
		
		else if(nCNT2b>=para2num2)
			m_fLocalMean2b=(para1<para2thresh2)?0x7fff:m_fLocalMean2b;

		else if(nCNT2b>=para2num1)
			m_fLocalMean2b=(para1<para2thresh1)?0x7fff:m_fLocalMean2b;

		if(para1<para2thresh0)
			m_fLocalMean2b=0x7fff;	

#endif
#if 1	
		//条件3
		//测试600mk实验室视多帧频数据0-200frame，目标搜索稳定，虚警高
		//对试验室数据和qh外场数据 综合考虑
		//qh外场数据没有条件2效果好
		//外场数据不是很好
		
		//代替case1的代码，从能量角度分析

		if((m_fLocalMean2b<m_thresh*mthreshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		//上面已经要求5*5区域至少有3个值超过阈值
		//对于小于阈值的，m_fLocalMean2b用阈值代替
		if((m_fLocalMean2b<m_thresh*mthreshratio4)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh;
		//if((m_fLocalMean2b<m_thresh*mthreshratio5)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=(m_fLocalMean2b*mthreshratio5>m_thresh)?m_fLocalMean2b*mthreshratio5:m_thresh;
		//提高亮目标的有效个数
		if((m_fLocalMean2b>m_thresh*mthreshratio5)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2b*mthreshratio5ftr;
		
		//和条件2区别:使用para1作为if条件
		//根据高斯能量分布分析和实际图像调试得出 		
		if(para1>para3thresh3)
			m_fLocalMean2b=(nCNT2b<para3num3)?0x7fff:m_fLocalMean2b;
				
		else if(para1>para3thresh2)
			m_fLocalMean2b=(nCNT2b<para3num2)?0x7fff:m_fLocalMean2b;

		if(para1<para3thresh1)
			m_fLocalMean2b=0x7fff;

#endif
	
		m_fDela[i]=m_fLocalMean2b;
		
	}	
}

//优化2
/**************************************************
* Prototype     : OBJ_Detect2
* Description   : pMarkDatatemp:查看分割结果图像，正式代码不需要获取
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
第2次分割

OBJ_Detect优化2
使用阈值的方法进行第二次分割,初始版本
20181101确定已经效果不是很好，不使用这种方法
***************************************************/

void OBJ_Detect2nd1(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	
	int nObjCnt;
	int K1Width;//目标点区域1大小
	int K1Height;
	int K2Width;//目标点区域2大小(范围小于区域1)
	int K2Height;
	
	int nMaxX;
	int nMaxY;
	int nCNTl,nCNT2;	
	Int16 leftupgray,leftdowngray,rightupgray,rightdowngray;
	Int16 nMaxGray;
	Int16 nSubMaxGray;
	
	float m_fLocalMean1sum;	
	float m_fLocalFcha2sum;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	
	//////////////////////////////////////////////////////////////////////////
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	
	K1Width = nW_K1;
	K1Height = nH_K1;
	K2Width = nW_K2;
	K2Height = nW_K2;
	nObjCnt = npixelcnt;
	nCNTl=nW_K1*nH_K1;
	//nCNTl=nW_K2*nH_K2;
	nMaxX=0;
	nMaxY=0;
	for (i=1;i<=nObjCnt;i++)
	{		
		//获得目标点中心区域1的均值				
		nMaxGray=0;		
		m_fLocalMean1sum=0;		
		cxleft=pseuobject[i].cx-K1Width/2;
		cytop=pseuobject[i].cy-K1Height/2;
	
		for (a=cytop;a<cytop+nH_K1;a++)
		{
			for (b=cxleft;b<cxleft+nW_K1;b++)
			{
				if (*(pDataIn16+a*nWidth+b)>nMaxGray)
				{
					nMaxGray=*(pDataIn16+a*nWidth+b);
					nMaxY=a;
					nMaxX=b;
				}
				m_fLocalMean1sum+=*(pDataIn16+a*nWidth+b);
			}
		}
		m_fLocalMean1sum=m_fLocalMean1sum/nCNTl;

	
		//获得目标点中心区域2的方差		
		nCNT2=0;
		m_fLocalFcha2sum=0.01;
		cxleft=pseuobject[i].cx-K2Width/2;
		cytop=pseuobject[i].cy-K2Height/2;	
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				if (*(pDataIn16+a*nWidth+b)>m_fLocalMean1sum)
				{					
					m_fLocalFcha2sum+=(*(pDataIn16+a*nWidth+b)-m_fLocalMean1sum)*(*(pDataIn16+a*nWidth+b)-m_fLocalMean1sum);
					nCNT2++;
				}					
			}
		}	
		m_fLocalFcha2sum=(int)_rsqrsp(nCNT2/m_fLocalFcha2sum);
		
		//获取区域1的sub-maxgray
		nSubMaxGray=0;
		leftupgray = *(pDataIn16+(nMaxY-offsetPixelY_K1)*nWidth+nMaxX-offsetPixelX_K1);
		rightupgray = *(pDataIn16+(nMaxY-offsetPixelY_K1)*nWidth+nMaxX+offsetPixelX_K1);
		leftdowngray = *(pDataIn16+(nMaxY+offsetPixelY_K1)*nWidth+nMaxX-offsetPixelX_K1);
		rightdowngray = *(pDataIn16+(nMaxY+offsetPixelY_K1)*nWidth+nMaxX+offsetPixelX_K1);

		nSubMaxGray=	((leftupgray<nMaxGray)&&(leftupgray>nSubMaxGray))?leftupgray:nSubMaxGray;//leftup
		nSubMaxGray=	((rightupgray<nMaxGray)&&(rightupgray>nSubMaxGray))?rightupgray:nSubMaxGray;//rightup
		nSubMaxGray= ((leftdowngray<nMaxGray)&&(leftdowngray>nSubMaxGray))?leftdowngray:nSubMaxGray;//leftdown
		nSubMaxGray=((rightdowngray<nMaxGray)&&(rightdowngray>nSubMaxGray))?rightdowngray:nSubMaxGray;//rightdown
	

		/*nSubMaxGray小的目标点亮度强，nSubMaxGray大的背景噪音大*/
		if(nSubMaxGray>m_fLocalMean1sum*K1_Compensate1)
			nSubMaxGray = m_fLocalMean1sum*K1_Compensate1;

		/*方差比均值大的目标区域，背景噪音大*/
		/*选取目标区域的中心点在最外侧时，方差波动很大，造成目标无法检出*/
		/*OBJ_Detect1st合并中心点，减少在最外侧的概率*/
		if(m_fLocalMean1sum>m_fLocalFcha2sum)
			m_fLocalFcha2sum = K2_Fcha*m_fLocalFcha2sum*m_fLocalFcha2sum/m_fLocalMean1sum;
		
		m_fDela[i]=m_fLocalMean1sum*K1_Mean+m_fLocalFcha2sum*K1_Fcha+nSubMaxGray;	
	}	
}





//优化2
/**************************************************
* Prototype     : OBJ_Detect2nd2
* Description   : pMarkDatatemp:查看分割结果图像，正式代码不需要获取
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
第2次分割

使用修正阈值的方法进行第二次分割
20181101确定已经效果不是很好，不使用这种方法
***************************************************/

void OBJ_Detect2nd2(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	
	int nObjCnt;
	int K1Width;//目标点区域1大小
	int K1Height;
	int K2Width;//目标点区域2大小(范围小于区域1)
	int K2Height;
	
	int nMaxX;
	int nMaxY;
	int nCNT2,nCNT2b,nCNT2l,nthCNT,nthresh;	
	Int16 leftupgray,leftdowngray,rightupgray,rightdowngray;
	Int16 nMaxGray;
	Int16 nSubMaxGray;
	
	float m_fLocalMean1sum;
	float m_fLocalMean1;
	float m_fLocalMean2sum;
	float m_fLocalMean2;
	float m_fLocalMean2bsum;
	float m_fLocalMean2b;
	float m_fLocalFcha2sum;
	float m_fLocalMean2l;
	float m_fLocalMean2lsum;
	
	float fthreshsum;
	float fthresh;
	float para1;
	float para2;
	float para3;
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	
	//////////////////////////////////////////////////////////////////////////
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	
	K2Width = nW_K2;
	K2Height = nW_K2;
	nObjCnt = npixelcnt;
	nCNT2=nW_K2*nH_K2;
	
	nMaxX=0;
	nMaxY=0;
	for (i=1;i<=nObjCnt;i++)
	{
		m_fLocalMean2sum=0.01;
		m_fLocalMean1sum=0;
		m_fLocalMean2bsum=0;		
		m_fLocalMean2b=0;
		m_fLocalMean2l=0.01;
		nCNT2b=0;
		nCNT2l=0;
		nthCNT=0;
		
		cxleft=pseuobject[i].cx-K2Width/2;
		cytop=pseuobject[i].cy-K2Height/2;	
		
		//获得目标点中心区域2的均值
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalMean2sum+=*(pDataIn16+a*nWidth+b);
				if (*(pDataIn16+a*nWidth+b)>m_thresh)
				{
					nthCNT++;
				}
			}

		}
		m_fLocalMean2=m_fLocalMean2sum/nCNT2;

		//5*5区域内没有3个超过阈值的，认为这个区域没有目标
		if(nthCNT<3)		
			continue;
		
		//获得目标点中心区域2的大于均值的个数
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				if (*(pDataIn16+a*nWidth+b)>m_fLocalMean2)
				{
					nCNT2b++;
					m_fLocalMean2bsum+=*(pDataIn16+a*nWidth+b);
				}
				
			}
		}

		nCNT2l=nCNT2-nCNT2b;
		if(nCNT2l!=0)
		m_fLocalMean2l = (m_fLocalMean2sum - m_fLocalMean2bsum)/nCNT2l;		
		if(nCNT2b!=0)
		m_fLocalMean2b =m_fLocalMean2bsum/nCNT2b;	
		
		//修正阈值的大小
		m_fLocalMean2lsum = m_fLocalMean2sum-m_fLocalMean2bsum;
		fthresh = m_fLocalMean2b+(m_fLocalMean2b-m_fLocalMean2)
				*(m_fLocalMean2lsum/m_fLocalMean2sum);

		para1 = m_fLocalMean2bsum/m_fLocalMean2sum;
		para2 = nCNT2b/nCNT2;
		para3=m_fLocalMean2b/m_fLocalMean2l;
		

		//20180328debug 信息，最后去掉
		j= i%100;
		m_fLineFcha[5*j]=m_fLocalMean2;
		m_fLineFcha[5*j+1]=m_fLocalMean2b;
		m_fLineFcha[5*j+2]=nCNT2b;
		m_fLineFcha[5*j+3]=m_fLocalMean2l;
		m_fLineFcha[5*j+4]=fthresh;
		//20180328debug 信息，最后去掉
		
		
		//m_fLocalMean2b = fthresh;

		if((m_fLocalMean2b<m_thresh*m4threshratio1)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=0x7fff;
		//上面已经要求5*5区域至少有3个值超过阈值
		//对于小于阈值的，m_fLocalMean2b用阈值代替
		if((m_fLocalMean2b<m_thresh)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_thresh;
		//if((m_fLocalMean2b<m_thresh*mthreshratio5)&&(m_fLocalMean2b!=0x7fff))
			//m_fLocalMean2b=(m_fLocalMean2b*mthreshratio5>m_thresh)?m_fLocalMean2b*mthreshratio5:m_thresh;
		//提高亮目标的有效个数
		if((m_fLocalMean2b>m_thresh*m4threshratio5)&&(m_fLocalMean2b!=0x7fff))
			m_fLocalMean2b=m_fLocalMean2b*mthreshratio5ftr;


#if 1
		//条件1023-00
		//测试600mk实验室视多帧频数据0-200frame，目标搜索稳定，
		//虚警比条件old1少，虚警少

		//测试400mk实验室视多帧频数据0-100frame，目标搜索不是太稳定，
		//虚警比条件old1少，虚警少

		//qh外场数据丢失目标严重，虚警少




		//case3条件限制严格，某些帧存在搜索目标失败问题	
		//根据高斯能量分布分析和实际图像调试得出 		
		if(para1>para3thresh3)
			m_fLocalMean2b=(nCNT2b<para4num3)?0x7fff:m_fLocalMean2b;
				
		else if(para1>para3thresh2)
			m_fLocalMean2b=(nCNT2b<para4num2)?0x7fff:m_fLocalMean2b;

		if(para1<para3thresh1)
			m_fLocalMean2b=0x7fff;
	
#endif		
		
#if 0

		//条件1023-01
		
		//测试600mk实验室视多帧频数据0-200frame，目标搜索稳定，
		//虚警比条件2和条件3少，比条件old1多，有一定的虚警

		//测试400mk实验室视多帧频数据0-100frame，目标搜索稳定，
		//虚警比条件old1多，存在几帧数据丢失目标现象，有一定的虚警
		
		//根据高斯能量分布分析和实际图像调试得出 	
		if(nCNT2b>=para4num3)
			m_fLocalMean2b=(para1<para4thresh3)?0x7fff:m_fLocalMean2b;
		
		else if(nCNT2b>=para4num2)
			m_fLocalMean2b=(para1<para4thresh2)?0x7fff:m_fLocalMean2b;

		else if(nCNT2b>=para4num1)
			m_fLocalMean2b=(para1<para4thresh1)?0x7fff:m_fLocalMean2b;

		if(para1<para4thresh0)
			m_fLocalMean2b=0x7fff;	
	
		

		
#endif		


		
		


	
		m_fDela[i]=m_fLocalMean2b;
		
	}	
}







/**************************************************
* Prototype     : OBJ_Detect2nd3
* Description   : pMarkDatatemp:查看分割结果图像，正式代码不需要获取
* Input           :
* Return Value :  

* Date            : 20181009
* Author   	 : 
* Modification  : 
第2次分割

使用阈值的方法进行第二次分割，区域5*5范围
20181101确定已经效果不是很好，不使用这种方法

***************************************************/

void OBJ_Detect2nd3(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	
	int nObjCnt;
	int K1Width;//目标点区域1大小
	int K1Height;
	int K2Width;//目标点区域2大小(范围小于区域1)
	int K2Height;
	
	int ncenterX;
	int ncenterY;
	int nCNT2,nCNT2b,nthCNT;
	int nthcnt;//20181025调试完后删除
	Int16 leftupgray,leftdowngray,rightupgray,rightdowngray;
	Int16 array[4];
	Int16 nMaxGray;
	Int16 nSubMaxGray;
	
	float m_fLocalMean1sum;
	float m_fLocalMean1;
	float m_fLocalMean2sum;
	float m_fLocalMean2;
	float m_fLocalMean2bsum;
	float m_fLocalMean2b;
	float m_fLocalFcha2sum;
	float m_fLocalFcha2;
	

	float fthresh;
	float para1;
	float para2;
	
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	
	//////////////////////////////////////////////////////////////////////////
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	
	K2Width = nW_K2;
	K2Height = nW_K2;
	nObjCnt = npixelcnt;
	nCNT2=nW_K2*nH_K2;
	
	ncenterX=0;
	ncenterY=0;
	for (i=1;i<=nObjCnt;i++)
	{
		m_fLocalMean1sum=0;
		m_fLocalMean2sum=0;
		m_fLocalMean2bsum=0;
		m_fLocalFcha2sum=0.00;
		m_fLocalMean2b=0;
		fthresh =0;
		
		
		nCNT2b=0;		
		nthCNT=0;
		nthcnt=0;//20181025调试完后删除
		
		cxleft=pseuobject[i].cx-K2Width/2;
		cytop=pseuobject[i].cy-K2Height/2;	
		
		//获得目标点中心区域2的均值
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalMean2sum+=*(pDataIn16+a*nWidth+b);
				if (*(pDataIn16+a*nWidth+b)>m_thresh)
				{
					nthCNT++;
				}
			}

		}
		m_fLocalMean2=m_fLocalMean2sum/nCNT2;

		
		
		

		//获得目标点中心区域2的方差				
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalFcha2sum+=(*(pDataIn16+a*nWidth+b)-m_fLocalMean2)*(*(pDataIn16+a*nWidth+b)-m_fLocalMean2);
				if (*(pDataIn16+a*nWidth+b)>m_fLocalMean2)
				{					
					m_fLocalMean2bsum+=*(pDataIn16+a*nWidth+b);					
					nCNT2b++;
				}
			}
		}	
		m_fLocalFcha2=sqrt(m_fLocalFcha2sum/nCNT2);	
		
		
		//5*5区域内没有3个超过阈值的，认为这个区域没有目标
		//nCNT2b为0不是真实图像数据
		if((nthCNT<3)||(nCNT2b==0))		
			continue;
			
		m_fLocalMean2b =m_fLocalMean2bsum/nCNT2b;	
		//均匀背景或者
		//m_fLocalMean2b太小的，没有目标值
		//if((m_fLocalMean2*1.3>m_fLocalMean2b)||(m_thresh1>m_fLocalMean2b))
			//continue;
		
		//m_fLocalMean2b = fthresh;
		ncenterX = pseuobject[i].cx;
		ncenterY = pseuobject[i].cy;
		//nMaxGray = *(pDataIn16+ncenterY*nWidth+ncenterX);

		//获取区域1的sub-maxgray
		//nSubMaxGray=0;
		leftupgray = *(pDataIn16+(ncenterY-offsetPixelY3)*nWidth+ncenterX-offsetPixelX3);
		rightupgray = *(pDataIn16+(ncenterY-offsetPixelY3)*nWidth+ncenterX+offsetPixelX3);
		leftdowngray = *(pDataIn16+(ncenterY+offsetPixelY3)*nWidth+ncenterX-offsetPixelX3);
		rightdowngray = *(pDataIn16+(ncenterY+offsetPixelY3)*nWidth+ncenterX+offsetPixelX3);

		//nSubMaxGray=	((leftupgray<nMaxGray)&&(leftupgray>nSubMaxGray))?leftupgray:nSubMaxGray;//leftup
		//nSubMaxGray=	((rightupgray<nMaxGray)&&(rightupgray>nSubMaxGray))?rightupgray:nSubMaxGray;//rightup
		//nSubMaxGray= ((leftdowngray<nMaxGray)&&(leftdowngray>nSubMaxGray))?leftdowngray:nSubMaxGray;//leftdown
		//nSubMaxGray=((rightdowngray<nMaxGray)&&(rightdowngray>nSubMaxGray))?rightdowngray:nSubMaxGray;//rightdown
		//获取第二个最大值
		array[0]=_max2(leftupgray,rightupgray);		
		array[1]=_max2(leftdowngray,rightdowngray);
		array[2]=_min2(leftupgray,rightupgray);
		array[3]=_min2(leftdowngray,rightdowngray);
		array[0]=_min2(array[0],array[1]);
		array[1]=_max2(array[2],array[3]);
		nSubMaxGray=_max2(array[0],array[1]);
		
		fthresh=m_fLocalMean2*K1_Mean+m_fLocalFcha2*K1_Fcha+nSubMaxGray;
		
		//20180328debug 信息，最后去掉
		j= i%100;
		m_fLineFcha[5*j]=m_fLocalMean2;
		m_fLineFcha[5*j+1]=m_fLocalFcha2;
		m_fLineFcha[5*j+2]=m_fLocalMean2b;
		m_fLineFcha[5*j+3]=nSubMaxGray;
		m_fLineFcha[5*j+4]=nCNT2b;
		//20180328debug 信息，最后去掉

		
		
		if((fthresh<m_thresh*mthreshratio1)&&(fthresh!=0x7fff))
			fthresh=0x7fff;
		if((fthresh<m_thresh*mthreshratio3)&&(fthresh!=0x7fff))
			fthresh=m_thresh*mthreshratio3;
		
		//去除不符合条件的目标
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				
				if (*(pDataIn16+a*nWidth+b)>fthresh)
				{
					m_fLocalMean1sum+=*(pDataIn16+a*nWidth+b);
					nthcnt++;//20181025调试完后删除
					
				}
			}

		}
		para1=m_fLocalMean1sum/m_fLocalMean2sum;
		para2=m_fLocalMean2bsum/m_fLocalMean2sum;
		if((para1<para5th1)||(para2<para5th2))
			fthresh=0x7fff;


		
		m_fDela[i]=fthresh;
	}	
}










/**************************************************
* Prototype     : OBJ_Detect2nd4
* Description   : pMarkDatatemp:查看分割结果图像，正式代码不需要获取
* Input           :
* Return Value :  

* Date            : 20181009
* Author   	 : 
* Modification  : 
第2次分割

使用阈值的方法进行第二次分割，区域7*7范围

***************************************************/


void OBJ_Detect2nd4(Int16 *pDataIn, Int16 *pMarkDatatemp, int nWidth,int nHeight)
{
	int i,j;
	int a,b;
	int cxleft,cytop;
	
	int nObjCnt;
	int K1Width;//目标点区域1大小
	int K1Height;
	int K2Width;//目标点区域2大小(范围小于区域1)
	int K2Height;
	
	int ncenterX;
	int ncenterY;
	int nCNT2,nCNT2b,nthCNT;
	int nthcnt;//20181025调试完后删除
	Int16 leftupgray,leftdowngray,rightupgray,rightdowngray,lgray,rgray,ugray,dgray;
	Int16 array[8];
	Int16 nMaxGray;
	Int16 nSubMaxGray;
	
	float m_fLocalMean1sum;
	float m_fLocalMean1;
	float m_fLocalMean2sum;
	float m_fLocalMean2;
	float m_fLocalMean2add;
	float m_fLocalMean2bsum;
	float m_fLocalMean2b;
	float m_fLocalFcha2sum;
	float m_fLocalFcha2;
	
	float thresh;
	float fthresh;
	float para1;
	float para2;
	
	
	Int16 *pDataIn16;
	Int16 *pMarkData16;
	
	//////////////////////////////////////////////////////////////////////////
	pDataIn16=pDataIn;
	pMarkData16 = pMarkDatatemp;
	
	K2Width = nW_K2;
	K2Height = nH_K2;
	nObjCnt = npixelcnt;
	nCNT2=nW_K2*nH_K2;
	
	ncenterX=0;
	ncenterY=0;
	for (i=1;i<=nObjCnt;i++)
	{
		m_fLocalMean1sum=0;
		m_fLocalMean2sum=0;
		m_fLocalMean2bsum=0;
		m_fLocalFcha2sum=0.00;
		m_fLocalMean2b=0;
		//fthresh =m_thresh*mthreshratio7;
		
		
		nCNT2b=0;		
		nthCNT=0;
		nthcnt=0;//20181025调试完后删除

		
		cxleft=pseuobject[i].cx-K2Width/2;
		cytop=pseuobject[i].cy-K2Height/2;
		ncenterX = pseuobject[i].cx;
		ncenterY = pseuobject[i].cy;		
		//thresh = _max2(m_fLineMean[ncenterY],m_thresh);
		//获得目标点中心区域2的均值
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalMean2sum+=*(pDataIn16+a*nWidth+b);
				if (*(pDataIn16+a*nWidth+b)>m_thresh)
				{
					nthCNT++;
				}
			}

		}
		m_fLocalMean2=m_fLocalMean2sum/nCNT2;

		
		
		
		m_fLocalMean2add=m_fLocalMean2*para5add1;
		//获得目标点中心区域2的方差				
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				m_fLocalFcha2sum+=(*(pDataIn16+a*nWidth+b)-m_fLocalMean2)*(*(pDataIn16+a*nWidth+b)-m_fLocalMean2);
				if (*(pDataIn16+a*nWidth+b)>m_fLocalMean2add)
				{					
					m_fLocalMean2bsum+=*(pDataIn16+a*nWidth+b);					
					nCNT2b++;
				}
			}
		}	
		m_fLocalFcha2=sqrt(m_fLocalFcha2sum/nCNT2);	
		
		
		//5*5区域内没有3个超过阈值的，认为这个区域没有目标
		//nCNT2b为0不是真实图像数据
		//这个阈值在微弱目标时为2
		if((nthCNT<nthCNTlimit)||(nCNT2b==0))		
			continue;
			
		m_fLocalMean2b =m_fLocalMean2bsum/nCNT2b;	
		//均匀背景或者
		//m_fLocalMean2b太小的，没有目标值
		//if((m_fLocalMean2*1.3>m_fLocalMean2b)||(m_thresh1>m_fLocalMean2b))
			//continue;
		
		//m_fLocalMean2b = fthresh;
		//ncenterX = pseuobject[i].cx;
		//ncenterY = pseuobject[i].cy;
		//nMaxGray = *(pDataIn16+ncenterY*nWidth+ncenterX);

		//获取区域1的sub-maxgray
		//nSubMaxGray=0;
		leftupgray = *(pDataIn16+(ncenterY-offsetPixelY3)*nWidth+ncenterX-offsetPixelX3);
		rightupgray = *(pDataIn16+(ncenterY-offsetPixelY3)*nWidth+ncenterX+offsetPixelX3);
		leftdowngray = *(pDataIn16+(ncenterY+offsetPixelY3)*nWidth+ncenterX-offsetPixelX3);
		rightdowngray = *(pDataIn16+(ncenterY+offsetPixelY3)*nWidth+ncenterX+offsetPixelX3);

		lgray = *(pDataIn16+ncenterY*nWidth+ncenterX-offsetPixelX31);
		rgray = *(pDataIn16+ncenterY*nWidth+ncenterX+offsetPixelX31);
		ugray = *(pDataIn16+(ncenterY-offsetPixelY31)*nWidth+ncenterX);
		dgray = *(pDataIn16+(ncenterY+offsetPixelY31)*nWidth+ncenterX);

		//nSubMaxGray=	((leftupgray<nMaxGray)&&(leftupgray>nSubMaxGray))?leftupgray:nSubMaxGray;//leftup
		//nSubMaxGray=	((rightupgray<nMaxGray)&&(rightupgray>nSubMaxGray))?rightupgray:nSubMaxGray;//rightup
		//nSubMaxGray= ((leftdowngray<nMaxGray)&&(leftdowngray>nSubMaxGray))?leftdowngray:nSubMaxGray;//leftdown
		//nSubMaxGray=((rightdowngray<nMaxGray)&&(rightdowngray>nSubMaxGray))?rightdowngray:nSubMaxGray;//rightdown
		//获取第二个最大值
		//array[0]=_max2(leftupgray,rightupgray);		
		//array[1]=_max2(leftdowngray,rightdowngray);
		//array[2]=_min2(leftupgray,rightupgray);
		//array[3]=_min2(leftdowngray,rightdowngray);
		//array[0]=_min2(array[0],array[1]);
		//array[1]=_max2(array[2],array[3]);
		//nSubMaxGray=_max2(array[0],array[1]);

		//获取最大值
		array[0]=_max2(leftupgray,rightupgray);		
		array[1]=_max2(leftdowngray,rightdowngray);		
		array[2]=_max2(lgray,rgray);
		array[3]=_max2(ugray,dgray);
		array[0]=_max2(array[0],array[1]);
		array[1]=_max2(array[2],array[3]);
		nSubMaxGray = _max2(array[0],array[1]);


		
		
		fthresh=m_fLocalMean2*K1_Mean4+m_fLocalFcha2*K1_Fcha4+nSubMaxGray;

		
		
		//20180328debug 信息，最后去掉
		j= i%100;
		m_fLineFcha[5*j]=m_fLocalMean2;
		m_fLineFcha[5*j+1]=m_fLocalFcha2;
		m_fLineFcha[5*j+2]=m_fLocalMean2b;
		m_fLineFcha[5*j+3]=nSubMaxGray;
		m_fLineFcha[5*j+4]=nCNT2b;
		//20180328debug 信息，最后去掉

		
#if 0	
		if((fthresh<m_thresh*mthreshratio1)&&(fthresh!=0x7fff))
			fthresh=0x7fff;
		if((fthresh<m_thresh*mthreshratio3)&&(fthresh!=0x7fff))
			fthresh=m_thresh*mthreshratio3;
		
		//去除不符合条件的目标
		for (a=cytop;a<cytop+nH_K2;a++)
		{
			for (b=cxleft;b<cxleft+nW_K2;b++)
			{					
				
				if (*(pDataIn16+a*nWidth+b)>fthresh)
				{
					m_fLocalMean1sum+=*(pDataIn16+a*nWidth+b);
					nthcnt++;//20181025调试完后删除
					
				}
			}

		}
		para1=m_fLocalMean1sum/m_fLocalMean2sum;
		para2=m_fLocalMean2bsum/m_fLocalMean2sum;
		if((para1<para5th1)||(para2<para5th2))
			fthresh=0x7fff;
#endif
		if(g12==1)
		{
			//条件1025-01	
			//根据高斯能量分布分析和实际图像调试得出 	
			para1 = m_fLocalMean2bsum/m_fLocalMean2sum;			
					
			if(nCNT2b>para5num2)
				fthresh=(para1<para5thresh2)?0x7fff:fthresh;

			if(nCNT2b<para5num1)
				fthresh=(para1>para5thresh1)?0x7fff:fthresh;

			if(para1<para5thresh0)
				fthresh=0x7fff;
			
		}
		//para1 = m_fLocalMean2bsum/m_fLocalMean2sum;	
		//if((nCNT2b<para5num1)&&(para1>para5thresh1))
			//fthresh=0x7fff;

		//if((fthresh<m_thresh*mthreshratio1)&&(fthresh!=0x7fff))
			//fthresh=0x7fff;
		//if((fthresh<m_thresh*mthreshratio3)&&(fthresh!=0x7fff))
			//fthresh=m_thresh*mthreshratio3;

		//thresh = _max2(m_fLineMean[ncenterY],m_thresh);
		para1 = m_fLocalMean2bsum/m_fLocalMean2sum;	
		if(para1<para5thresh0)
			fthresh=0x7fff;
		if((nCNT2b<para5num1)&&(para1>para5thresh1))
			fthresh=0x7fff;
		if((fthresh<m_thresh*mthreshratio6)&&(fthresh!=0x7fff))
			fthresh=0x7fff;
		if((fthresh<m_thresh*mthreshratio7)&&(fthresh!=0x7fff))
			fthresh=m_thresh*mthreshratio8;

		m_fDela[i]=fthresh;
	}	
}

/**************************************************
* Prototype     : OBJ_Synthesis
* Description   : 
* Input           :pDataIn,目标图像信息
			   pMarkDataIn,标号图像

* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/

Uint32 cycles;
Uint32 cycles12;
Uint32 cycles13;

int g3=0;
int gtrack4=0;




void OBJ_Synthesis(Int16 *pDataIn, Int16 *pMarkDatatemp,int nWidth,int nHeight)
{
	int i,j;
	int targetcnt;
	int targetcnt1;
	int targetcnt2;
	int targetcnt3;
	int targetcnt4;
	int nWidth1,nHeight1;//dsp存储空间需要的宽度和长度
	int nObjCnt;
	int cx,cy;
	Int16 nLeftP,nLeftUpP,nUpP,nRightUpP,nRightP;
	Int16 Mark4point1,Mark4point2,Mark4point3,Mark4point4;
	Int16 merge1;
	int signcnt;
	int signNum1;
	Int16 MarkNum;//最多搜索疑似目标个数
	int chainNum;
	Int16 *pDataIn16;	
	Int16 *pMarkData16;
	Int16 *Mark4temp;
	long long *pMarkData64;
	Object_t *pobj;
	Mark4_t *Mark4Contemp;
	Target_Track_t *ptargetTemp;
	Target_Track_t *ptargetTemp1;
	
	
	nWidth1  = nWidth;
	nHeight1 = nHeight;		
	nObjCnt=npixelcnt;//i从1开始，nObjCnt加1，才能对最后一个目标操作
	signcnt = 0;
	targetcnt = 0;
	targetcnt1 = 0;
	targetcnt2 = 0;
	targetcnt3 = 0;
	targetcnt4 = 0;
	pobj = pseuobject;
	pDataIn16=pDataIn;
	pMarkData16=pMarkDatatemp;
	Mark4temp=Mark4;
	ptargetTemp = targetTemp;
	ptargetTemp1= targetTemp1;
	Mark4Contemp=Mark4Con;

	
	



	pMarkData64 = (long long *)pMarkDatatemp;		
	for (i=0;i<81920;i++)
	{		
		pMarkData64[i]=0;		
	}
	

	//标点图像:合并标号数据
	for (i=1;i<=nObjCnt;i++)
	{		
		//获取标号图像数据
		//pobj[i].cx=pobj[i].pobj[i].cx;
		//pobj[i].cy=pobj[i].pobj[i].cy;
		
		MarkNum=0x400;
		nLeftP		=*(pMarkData16+(pobj[i].cy+0)*nWidth+(pobj[i].cx-1));
		nLeftUpP	=*(pMarkData16+(pobj[i].cy-1)*nWidth+(pobj[i].cx-1));
		nUpP		=*(pMarkData16+(pobj[i].cy-1)*nWidth+(pobj[i].cx+0));
		nRightUpP	=*(pMarkData16+(pobj[i].cy-1)*nWidth+(pobj[i].cx+1));
		
	
		//找连通的点中标号最小的标号，最小标号从1开始	
		MarkNum=(MarkNum>nLeftP && nLeftP!=0)?nLeftP:MarkNum;
		MarkNum=(MarkNum>=Mark4temp[nLeftP] && Mark4temp[nLeftP]!=0)?Mark4temp[nLeftP]:MarkNum;
		//
		MarkNum=(MarkNum>nLeftUpP && nLeftUpP!=0)?nLeftUpP:MarkNum;
		MarkNum=(MarkNum>=Mark4temp[nLeftUpP] && Mark4temp[nLeftUpP]!=0)?Mark4temp[nLeftUpP]:MarkNum;
		//
		MarkNum=(MarkNum>nUpP && nUpP!=0)?nUpP:MarkNum;
		MarkNum=(MarkNum>=Mark4temp[nUpP] && Mark4temp[nUpP]!=0)?Mark4temp[nUpP]:MarkNum;
		//
		MarkNum=(MarkNum>nRightUpP && nRightUpP!=0)?nRightUpP:MarkNum;
		MarkNum=(MarkNum>=Mark4temp[nRightUpP] && Mark4temp[nRightUpP]!=0)?Mark4temp[nRightUpP]:MarkNum;



		//找到连通点
		if (MarkNum!=0x400)
		{			
			Mark4point1=Mark4temp[nLeftP];
			Mark4point2=Mark4temp[nLeftUpP];
			Mark4point3=Mark4temp[nUpP];
			Mark4point4=Mark4temp[nRightUpP];
			//20181123 add:已经连通的点也指向最小值
			Mark4temp[Mark4point1]=Mark4point1>MarkNum?MarkNum:Mark4temp[Mark4point1];
			Mark4temp[Mark4point2]=Mark4point2>MarkNum?MarkNum:Mark4temp[Mark4point2];
			Mark4temp[Mark4point3]=Mark4point3>MarkNum?MarkNum:Mark4temp[Mark4point3];
			Mark4temp[Mark4point4]=Mark4point4>MarkNum?MarkNum:Mark4temp[Mark4point4];

			Mark4temp[nLeftP]=(nLeftP>MarkNum)?MarkNum:Mark4temp[nLeftP];
			Mark4temp[nLeftUpP]=(nLeftUpP>MarkNum )?MarkNum:Mark4temp[nLeftUpP];
			Mark4temp[nUpP]=(nUpP>MarkNum)?MarkNum:Mark4temp[nUpP];
			Mark4temp[nRightUpP]=(nRightUpP>MarkNum )?MarkNum:Mark4temp[nRightUpP];
			
			
			signNum1 = MarkNum;
		}
		else
		{
			//signNum++;
			//signNum1 = signNum;

			signcnt++;
			signNum1 = signcnt;	
		}
		
		*(pMarkData16+pobj[i].cy*nWidth+pobj[i].cx)=signNum1;
			
		//统计目标信息	
		Mark4Contemp[signNum1].x+=pobj[i].cx;		
		Mark4Contemp[signNum1].y+=pobj[i].cy;
		Mark4Contemp[signNum1].cnt+=1;		
		Mark4Contemp[signNum1].colMin=MIN(Mark4Contemp[signNum1].colMin,pobj[i].cx);		
		Mark4Contemp[signNum1].colMax=MAX(Mark4Contemp[signNum1].colMax,pobj[i].cx);		
		Mark4Contemp[signNum1].rowMin=MIN(Mark4Contemp[signNum1].rowMin,pobj[i].cy);
		Mark4Contemp[signNum1].rowMax=MAX(Mark4Contemp[signNum1].rowMax,pobj[i].cy);		
		Mark4Contemp[signNum1].sumgray+=*(pDataIn16+pobj[i].cy*nWidth+pobj[i].cx);//所有灰度值之和


		//if(signNum>1498)//1499-1
		if(signcnt>1498)//1499-1
		{
			break;
			
		}			
		
	}




	//获取signNumMerge:合并后目标总个数(包括只有一个像素的目标)
	//对图像连通情况进行修正检测,统计目标信息480*280
	//nSignsub=1;
	//for(i=1;i<=signNum;i++)
	merge1=0;
	for(i=1;i<=signcnt;i++)
   	{	
   		//20181123 modified
		if(0==Mark4temp[i])
 		{			
			merge1++;
			Mark4temp[i]=merge1;
 		}
		else
 		{
			//Mark4[i]=Mark4[Mark4[i]];
			merge1++;
			chainNum=Mark4temp[Mark4temp[i]];
			Mark4Contemp[chainNum].x+=Mark4Contemp[i].x;
			Mark4Contemp[chainNum].y+=Mark4Contemp[i].y;
			Mark4Contemp[chainNum].cnt+=Mark4Contemp[i].cnt; 
			Mark4Contemp[chainNum].colMin=MIN(Mark4Contemp[chainNum].colMin,Mark4Contemp[i].colMin);
			Mark4Contemp[chainNum].colMax=MAX(Mark4Contemp[chainNum].colMax,Mark4Contemp[i].colMax);
			Mark4Contemp[chainNum].rowMin=MIN(Mark4Contemp[chainNum].rowMin,Mark4Contemp[i].rowMin);
			Mark4Contemp[chainNum].rowMax=MAX(Mark4Contemp[chainNum].rowMax,Mark4Contemp[i].rowMax);
			Mark4Contemp[chainNum].sumgray+=Mark4Contemp[i].sumgray;	
			Mark4Contemp[i].cnt=0;
 		}	
	}	
	//获取合并后有效目标总个数(包括只有一个像素的目标)
	/*合并连通之后的目标*/
	for(i=signcnt;i>=1;i--)
	{
		//if(Mark4Contemp[i].cnt==0)
		if(Mark4Contemp[i].cnt<pixlimit)			
		{
			//debug使用，20170525，如果需要查看标号图像的话
			//*(pDataIn16+Mark4Con[i].y*nWidth+Mark4Con[i].x)=0;
			continue;
		}
			
		ptargetTemp[targetcnt].col=Mark4Contemp[i].x/Mark4Contemp[i].cnt;
		ptargetTemp[targetcnt].row=Mark4Contemp[i].y/Mark4Contemp[i].cnt;
		ptargetTemp[targetcnt].cnt=Mark4Contemp[i].cnt;
		ptargetTemp[targetcnt].gray=(Mark4Contemp[i].sumgray/Mark4Contemp[i].cnt);
		ptargetTemp[targetcnt].colsize=Mark4Contemp[i].colMax-Mark4Contemp[i].colMin+1;
		ptargetTemp[targetcnt].rowsize=Mark4Contemp[i].rowMax-Mark4Contemp[i].rowMin+1;
		targetcnt++;	

		//debug使用，20170525，如果需要查看标号图像的话
		//只是改变了中心像素标号
		//*(pDataIn16+targetTemp[tarNUM].row*nWidth+targetTemp[tarNUM].col)=tarNUM;
		//防止越界行为
		if(targetcnt>99)
		{
			break;
		}
	}
	//tarNUM=targetcnt;

	//合并3*3区域交叉的像素
	for(i=0;i<targetcnt;i++)
	{
		//去除已经合并的目标
		if(ptargetTemp[i].cnt==0)
			continue;
		ptargetTemp1[targetcnt1].col =ptargetTemp[i].col;
		ptargetTemp1[targetcnt1].row =ptargetTemp[i].row;
		ptargetTemp1[targetcnt1].cnt =ptargetTemp[i].cnt;
		ptargetTemp1[targetcnt1].gray=ptargetTemp[i].gray;
		//ptargetTemp1[targetcnt1].times=1;
		for(j=targetcnt-1;j>i;j--)
		{
			//去除已经合并的目标
			if(ptargetTemp[j].cnt==0)
				continue;
			//if((abs(ptargetTemp[i].col-ptargetTemp[j].col)<((ptargetTemp[i].colsize+ptargetTemp[j].colsize+6)>>1))&&
				//(abs(ptargetTemp[i].row-ptargetTemp[j].row)<((ptargetTemp[i].rowsize+ptargetTemp[j].rowsize+6)>>1)))
			if((abs(ptargetTemp[j].col-ptargetTemp1[targetcnt1].col)<det3rd1)&&
				(abs(ptargetTemp[j].row-ptargetTemp1[targetcnt1].row)<det3rd1))	
			 {    	 		
				//DetXX=abs(targetTemp[i].col-targetTemp[j].col);
				//DetYY=abs(targetTemp[i].row-targetTemp[j].row);
				//获取最大灰度值的位置为中心
				if(ptargetTemp[j].gray>ptargetTemp1[targetcnt1].gray)
				{
					ptargetTemp1[targetcnt1].col = ptargetTemp[j].col;
					ptargetTemp1[targetcnt1].row = ptargetTemp[j].row;
					ptargetTemp1[targetcnt1].gray=ptargetTemp[j].gray;
				}
				ptargetTemp1[targetcnt1].cnt +=ptargetTemp[j].cnt;    	        	 		
				ptargetTemp[j].cnt=0;
				
				
				//targetTemp1[i].colsize=DetXX+(targetTemp[i].colsize+targetTemp[j].colsize)>>1;
				//targetTemp1[i].rowsize=DetYY+(targetTemp[i].rowsize+targetTemp[j].rowsize)>>1;      	 		
				      	 		
				//ptargetTemp[j].cnt=0;
			}
		}
		targetcnt1++;
	}
	//获取和并后的目标信息
	for(i=0;i<targetcnt1;i++)
	{
		if(ptargetTemp1[i].cnt<npixcntlimit2_3)
			continue;
		ptargetTemp[targetcnt2].col =ptargetTemp1[i].col;
		ptargetTemp[targetcnt2].row =ptargetTemp1[i].row;	
		ptargetTemp[targetcnt2].cnt =ptargetTemp1[i].cnt;
		ptargetTemp[targetcnt2].gray=ptargetTemp1[i].gray;

		targetcnt2++;	 
		//if(targetcnt2>99)//防止越界行为
		//{
			//break;
		//}
	}	
	
	//去除5*5区域内多余的目标
	for(i=0;i<targetcnt2;i++)
	{
		//去除已经合并的目标
		if(ptargetTemp[i].cnt==0)
			continue;
		ptargetTemp1[targetcnt3].col =ptargetTemp[i].col;
		ptargetTemp1[targetcnt3].row =ptargetTemp[i].row;
		ptargetTemp1[targetcnt3].cnt =ptargetTemp[i].cnt;
		ptargetTemp1[targetcnt3].gray=ptargetTemp[i].gray;
		ptargetTemp1[targetcnt3].times=1;
		for(j=targetcnt2-1;j>i;j--)
		{
			//去除已经合并的目标
			if(ptargetTemp[j].cnt==0)
				continue;
			//if((abs(ptargetTemp[i].col-ptargetTemp[j].col)<((ptargetTemp[i].colsize+ptargetTemp[j].colsize+6)>>1))&&
				//(abs(ptargetTemp[i].row-ptargetTemp[j].row)<((ptargetTemp[i].rowsize+ptargetTemp[j].rowsize+6)>>1)))
			if((abs(ptargetTemp[j].col-ptargetTemp1[targetcnt3].col)<det3rd)&&
				(abs(ptargetTemp[j].row-ptargetTemp1[targetcnt3].row)<det3rd))	
			 {    	 		
				//DetXX=abs(targetTemp[i].col-targetTemp[j].col);
				//DetYY=abs(targetTemp[i].row-targetTemp[j].row);
				//获取最大灰度值的位置为中心
				if(ptargetTemp[j].gray>ptargetTemp1[targetcnt3].gray)
				{
					ptargetTemp1[targetcnt3].col = ptargetTemp[j].col;
					ptargetTemp1[targetcnt3].row = ptargetTemp[j].row;
					ptargetTemp1[targetcnt3].gray=ptargetTemp[j].gray;
				}
				ptargetTemp1[targetcnt3].cnt +=ptargetTemp[j].cnt;    	        	 		
				ptargetTemp[j].cnt=0;
				
				
				//targetTemp1[i].colsize=DetXX+(targetTemp[i].colsize+targetTemp[j].colsize)>>1;
				//targetTemp1[i].rowsize=DetYY+(targetTemp[i].rowsize+targetTemp[j].rowsize)>>1;      	 		
				      	 		
				//ptargetTemp[j].cnt=0;
			}
		}
		targetcnt3++;
	}
	//获取和并后的目标信息
	for(i=0;i<targetcnt3;i++)
	{
		if(ptargetTemp1[i].cnt<npixcntlimit2)
			continue;
		ptargetTemp[targetcnt4].col =ptargetTemp1[i].col;
		ptargetTemp[targetcnt4].row =ptargetTemp1[i].row;	
		ptargetTemp[targetcnt4].cnt =ptargetTemp1[i].cnt;
		ptargetTemp[targetcnt4].gray=ptargetTemp1[i].gray;

		targetcnt4++;	 
		if(targetcnt4>49)//防止越界行为
		{
			break;
		}
	}	
	tarNUM=targetcnt4;
}


/**************************************************
 Prototype     : dsp0_targetmerge
 Description   : 根据需要获得目标信息
 Input           : void  
 Return Value :   

 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

***************************************************/




void dsp0_targetmerge(int iworkmode)
{
	int i,j;
	//int DetXX,DetYY;
	int	targetseq;
	int	targetgrayseq;
	int	targetmaxcnt;
	int	graymax;
	int minspace;
	int space;
	int targetcnt;
	Int16 targetmergecnt;
	long long *ptargetTemp1;

	DMA_MEM_Tran_Config   DMA_transfer_cfg;
	unsigned int imageadd;

	/*获得pbuf数据传递一个640*512的数组*/			
	DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
	DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
	DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
	DMA_transfer_cfg.CCNT=512;	
	imageadd = 0x84000000+(Scan5errorcnt%1000)*655360;//记录当前图像信息

	

	targetseq=0;
	targetgrayseq =0;
	targetmaxcnt =0;
	graymax=0;
	targetmergecnt=0;
	minspace=0x7fffffff;
	targetcnt=0;
	ptargetTemp1 = (long long *)targetTemp1;

	for(i=0;i<400;i++)
	{
		ptargetTemp1[i]=0;

	}

	//对于大目标，只保留cnt最大的那个目标
	for(i=0;i<tarNUM;i++)
	{
		//去除已经合并的目标
		if(targetTemp[i].cnt==0)
			continue;
		targetTemp1[targetcnt].col =targetTemp[i].col;
		targetTemp1[targetcnt].row =targetTemp[i].row;
		targetTemp1[targetcnt].cnt =targetTemp[i].cnt;
		targetTemp1[targetcnt].gray=targetTemp[i].gray;
		targetTemp1[targetcnt].times=1;
		for(j=tarNUM-1;j>i;j--)
		{
			//去除已经合并的目标
			if(targetTemp[j].cnt==0)
				continue;
			if((abs(targetTemp[j].col-targetTemp1[targetcnt].col)<det2nd)&&
				(abs(targetTemp[j].row-targetTemp1[targetcnt].row)<det2nd))
			{    	 		
				//DetXX=abs(targetTemp[i].col-targetTemp[j].col);
				//DetYY=abs(targetTemp[i].row-targetTemp[j].row);
				//targetTemp1[i].col +=targetTemp[j].col;
				//targetTemp1[i].row +=targetTemp[j].row;
				//targetTemp1[i].cnt +=targetTemp[j].cnt;
				//targetTemp1[i].gray+=targetTemp[j].gray;
				//targetTemp1[i].times+=1;
				//targetTemp1[i].colsize=DetXX+(targetTemp[i].colsize+targetTemp[j].colsize)>>1;
				//targetTemp1[i].rowsize=DetYY+(targetTemp[i].rowsize+targetTemp[j].rowsize)>>1;      	 		
				      	 		
				//targetTemp[j].cnt=0;


				//20*20区域内cnt最大的目标为结果
				//cnt暂时只统计cnt最大值，不累加其它目标的cnt
				if(targetTemp[j].cnt>targetTemp1[targetcnt].cnt)
				{
					targetTemp1[targetcnt].col = targetTemp[j].col;
					targetTemp1[targetcnt].row = targetTemp[j].row;
					targetTemp1[targetcnt].gray=targetTemp[j].gray;
					targetTemp1[targetcnt].cnt =targetTemp[j].cnt;
				} 	 
				//targetTemp1[targetcnt].cnt +=targetTemp[j].cnt;
				targetTemp[j].cnt=0;
			}
		}
		targetcnt++;
	}

	//获取和并后的目标信息
	for(i=0;i<targetcnt;i++)
	{
		if(targetTemp1[i].cnt<npixcntlimit)
			continue;
		ScanCheck5[targetmergecnt].col =targetTemp1[i].col;
		ScanCheck5[targetmergecnt].row =targetTemp1[i].row;	
		ScanCheck5[targetmergecnt].cnt =targetTemp1[i].cnt;
		ScanCheck5[targetmergecnt].gray=0x1000+targetTemp1[i].gray-m_fLineMean[targetTemp1[i].row];

		targetmergecnt++;	 
		if(targetmergecnt>19)//防止越界行为
		{
			break;
		}
	}
	ScanCheck5Num = targetmergecnt;

	//20171128 add 单目标跟踪处理,找最接近中心的目标点
#if 0	
	if(iworkmode==Mode_Track)
	{
		//搜索到目标
		for(i=0;i<ScanCheck5Num;i++)
	    {
			space=(319-ScanCheck5[i].col-colpara1)*(319-ScanCheck5[i].col-colpara1)+
				 (-1-ScanCheck5[i].row-rowpara1)*(-1-ScanCheck5[i].row-rowpara1);

			//TXangle = (Int32)((319-ScanCheck5[0].col-colpara1)*16);
			//TYangle=  (Int32)((-1-ScanCheck5[0].row-rowpara1)*16);

			//space=(ScanCheck5[i].col-colpara1)*(ScanCheck5[i].col-colpara1)+
				 //(ScanCheck5[i].row-rowpara1)*(ScanCheck5[i].row-rowpara1);
			
			if(minspace>space)
	    	{
				minspace =space;
				targetseq=i;
			}

	    }		
		//搜索到目标
		if(ScanCheck5Num>0)
		{
			//发送目标信息			
			uiXadd = ScanCheck5[targetseq].col;
			uiYadd = ScanCheck5[targetseq].row;


			//20171123debug调试使用
			FPGACheck.col=ScanCheck5[targetseq].col;
			FPGACheck.row = ScanCheck5[targetseq].row;
			FPGACheck.cnt=ScanCheck5[targetseq].cnt;
			FPGACheck.gray=ScanCheck5[targetseq].gray;
		}
	}
	else
	{
		for(i=0;i<ScanCheck5Num;i++)
	    {
	    	if(ScanCheck5[i].cnt>targetmaxcnt)
	    	{	//发送像元个数最多的目标
				targetmaxcnt =ScanCheck5[i].cnt;
				targetseq=i;
			}
	    }
		
		for(i=0;i<ScanCheck5Num;i++)
	    {
	    	if(ScanCheck5[i].gray>graymax)
	    	{	//发送像元个数最多的目标
				graymax =ScanCheck5[i].gray;
				targetgrayseq=i;
			}
	    }
		
		//搜索到目标
		if(ScanCheck5Num>0)
		{	/*考虑目标灰度和个数，找最优*/
			if(graymax>ScanCheck5[targetseq].gray*1.6)
				targetseq = targetgrayseq;

			//发送目标信息			
			uiXadd = ScanCheck5[targetseq].col;
			uiYadd = ScanCheck5[targetseq].row;


			//20171123debug调试使用
			FPGACheck.col=ScanCheck5[targetseq].col;
			FPGACheck.row = ScanCheck5[targetseq].row;
			FPGACheck.cnt=ScanCheck5[targetseq].cnt;
			FPGACheck.gray=ScanCheck5[targetseq].gray;	
		}
	}	
#endif

	//20180424调试 :暂时上报最接近中心的目标点
	{
		//搜索到目标
		for(i=0;i<ScanCheck5Num;i++)
	    {
			space=(ScanCheck5[i].col-319)*(ScanCheck5[i].col-319)+
				 (ScanCheck5[i].row-255)*(ScanCheck5[i].row-255);

			//TXangle = (Int32)((319-ScanCheck5[0].col-colpara1)*16);
			//TYangle=  (Int32)((-1-ScanCheck5[0].row-rowpara1)*16);

			//space=(ScanCheck5[i].col-colpara1)*(ScanCheck5[i].col-colpara1)+
				 //(ScanCheck5[i].row-rowpara1)*(ScanCheck5[i].row-rowpara1);
			
			if(minspace>space)
	    	{
				minspace =space;
				targetseq=i;
			}

	    }		
		//搜索到目标
		if(ScanCheck5Num>0)
		{
			//发送目标信息			
			uiXadd = ScanCheck5[targetseq].col;
			uiYadd = ScanCheck5[targetseq].row;


			//20171123debug调试使用
			FPGACheck.col=ScanCheck5[targetseq].col;
			FPGACheck.row = ScanCheck5[targetseq].row;
			FPGACheck.cnt=ScanCheck5[targetseq].cnt;
			FPGACheck.gray=ScanCheck5[targetseq].gray;
		}
	}
#if 0

	//20171222调试dsp0_trackmode add
	i= Scan5errornum%1000;
	j= Scan5errornum1%1000;
	if(ScanCheck5Num>0)
	{
		if(abs(uiXadd-xref)>xmaxref)
		{	
			Scan5error[i].col=rtxnumber;
			Scan5error[i].row=DoorbellNum;
			Scan5error[i].colsize=CheckCycles;
			Scan5error[i].rowsize=Scan5errornum;
			Scan5error[i].cnt=ScanCheck5lastNum;
			Scan5error[i].gray=ScanCheck5Num;
			Scan5error[i].x=uiXadd;
			Scan5error[i].y=uiYadd;
			Scan5errornum++;
			
			//EDMA_copy_Type3(0x0c040000, imageadd, 
				//0, &DMA_transfer_cfg, 0);

			EDMA_copy_Type3(0x0c040000, imageadd, 
				0, &DMA_transfer_cfg, 0);
		}
		
		
	}				
	if(ScanCheck5Num==0)
	{
		Scan5error1[j].col=rtxnumber;
		Scan5error1[j].row=DoorbellNum;
		Scan5error1[j].colsize=CheckCycles;
		Scan5error1[j].rowsize=Scan5errornum1;
		Scan5error1[j].cnt=ScanCheck5lastNum;
		Scan5error1[j].gray=ScanCheck5Num;
		Scan5error1[j].x=uiXadd;
		Scan5error1[j].y=uiYadd;
		Scan5errornum1++;
		
		EDMA_copy_Type3(0x0c040000, imageadd, 
				0, &DMA_transfer_cfg, 0);
	}
	Scan5errorcnt = Scan5errornum1+Scan5errornum;
#endif
}		

/**************************************************
 Prototype     : dsp0_scan6
 Description   : 用于调试程序
 Input           : void  
 Return Value :   

 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
***************************************************/
void dsp0_scan6(void)
{
	int i,j;
	int DetXX,DetYY;
	int nWidth,nHeight;//dsp存储空间需要的宽度和长度
	int iworkmode;
	
	long long *ptargetTemp;
	long long *ptargetTemp1;
	long long *pScanCheck5;
	long long *Mark4Contemp;	
	long long *pseuobjecttemp;
	long long *pseuobjecttemp2;


	Uint32 cycles1;
	Uint32 cycles2;

	cycles1= TSCL;
	cycles2= TSCL;
	//初始化变量	
	nWidth  = ScanWidth;
	nHeight = ScanHeight;
	iworkmode = WorkMode;
	npixelcnt =0;
	uiXadd =0;
	uiYadd =0;
	FPGACheck.col  = 0;
	FPGACheck.row  = 0;  
	FPGACheck.cnt  = 0;  
	FPGACheck.gray = 0;
	tarNUM= 0;
	ScanCheck5Num=0;
	Mark4Contemp = (long long *)Mark4Con;
	pseuobjecttemp = (long long *)pseuobject;
	pseuobjecttemp2 = (long long *)pseuobject2;
	ptargetTemp = (long long *)targetTemp;
	ptargetTemp1 = (long long *)targetTemp1;
	pScanCheck5 = (long long *)ScanCheck5;
	for(i=0;i<1500;i++)
	{	   
	   Mark4Contemp[4*i]=0;
	   Mark4Contemp[4*i+1]=0xffff;
	   Mark4Contemp[4*i+2]=0xffff;
	   Mark4Contemp[4*i+3]=0;
	   Mark4[i]=0;
	} 
	for (j=0;j<nHeight;j++)
	{
		m_fLineMean[j]=0;
		m_fLineFcha[j]=0;
		m_fDela[j]=0x7fff;
	
		pseuobjecttemp[2*j]=0;
	    pseuobjecttemp[2*j+1]=0;
		pseuobjecttemp2[2*j]=0;
	    pseuobjecttemp2[2*j+1]=0;
	}
	for(i=0;i<400;i++)
	{
		ptargetTemp[i]=0;
		ptargetTemp1[i]=0;

	}
	for(j=0;j<80;j++)
	{
		pScanCheck5[j]=0;
	}
	
	
	//从原始图像数据中获得处理的数据
	AcquireData5(ImageData_5,ImageDataTemp2_5,nWidth,nHeight);

	//预处理
	//图像最大数据是否超过0x8000
	Pre_DataMove(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);

	//滤波，去除单点虚假目标
	//if(g10==1)
	//Pre_MeanFilter(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
	
	//20181030中值滤波
	//if(g10==0)
	Pre_IntermFilter(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
	
	

	//目标图像预测
	Pre_BKForecast(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);


	
	

	/*先图像预测后去单点，结果不好*/
	//目标图像预测
	//Pre_BKForecast(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
	//滤波，去除单点虚假目标
	//Pre_MeanFilter(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);
	
	cycles13= TSC_count_cycle_from(cycles2);
	
	//下面为测试3连通获取分割点的情况
	/*目标图像预测，图像不增强
	使用新的第一次分割*/
	//6连通说明虚警太高
	//if(g8==11)
	//{
		
		
		

		OBJ_Detect1stmaxv5(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);

		//Pre_DataMove2(Int16 *pDataIn,Int16 *pDataOut,int nWidth,int nHeight)
		//2nd 分割	
		//OBJ_Detect2nd(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
		//OBJ_Detect2nd2(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);

		//OBJ_Detect2nd3(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);

		OBJ_Detect2nd4(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
		OBJ_Detect2ndOut(ImageDataTemp1_5,ImageDataTemp2_5, nWidth,nHeight);
	
	
		//目标融合	
		OBJ_Synthesis(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);
	//}

#if 0	
	if(g8==12)
	{
		//目标图像预测
		Pre_BKForecast(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);

		//20180529 增强目标灰度值，用于第一次分割
		/* 使用下面函数，对300mk 14frame图像进行测试，
		结果是第一次分割后产生的pseudo更多*/
		Pre_GrayEnhanceFilter5(ImageDataTemp1_5,ImageDataTemp2_5,nWidth,nHeight);

		OBJ_Detect1stmaxv5(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);
		//2nd 分割	
		OBJ_Detect2nd(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);
		OBJ_Detect2ndOut(ImageDataTemp2_5,ImageDataTemp1_5, nWidth,nHeight);
	

		//目标融合	
		OBJ_Synthesis(ImageDataTemp2_5,ImageDataTemp1_5,nWidth,nHeight);
	}
#endif
	
	dsp0_targetmerge(iworkmode);
	cycles= TSC_count_cycle_from(cycles1);
	
					
}





/**************************************************
 Prototype     : dsp0_scan
 Description   : DSP0 scan
 Input           : void  
 Return Value :   

 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

***************************************************/
void dsp0_scanmode(void)
{	
	//asm(" NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;");

	//流程控制参数	
	SmallFlag=1;
	WorkMode = Mode_Scan;
	//WorkMode=(CheckCycles<ScanNum)?WorkMode:Mode_Capture;

	if(ScanCheckflag==6)
	{
		//目标检测算法6
		//添加弱小目标检测算法			
		dsp0_scan6();
	}		

	CheckCycles++;
	
	
}

/**************************************************
 Prototype     : dsp0_trackmode
 Description   : DSP0 track
 Input           : void  
 Return Value :   

 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

***************************************************/
void dsp0_trackmode(void)
{	
	//asm(" NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;");

	//流程控制参数	
	SmallFlag=1;
	WorkMode = Mode_Track;
	//WorkMode=(CheckCycles<ScanNum)?WorkMode:Mode_Capture;

	if(ScanCheckflag==6)
	{
		//目标检测算法6
		//添加弱小目标检测算法			
		dsp0_scan6();
	}		

	CheckCycles++;
	
	
}


/**************************************************
 Prototype     : dsp0_recapturemode
 Description   : dsp0_recapturemode
 Input           : void  
 Return Value :   

 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

***************************************************/
void dsp0_recapturemode(void)
{	
	//asm(" NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;");

	//流程控制参数	
	SmallFlag=1;
	WorkMode = Mode_ReCapture;
	//WorkMode=(CheckCycles<ScanNum)?WorkMode:Mode_Capture;

	if(ScanCheckflag==6)
	{
		//目标检测算法6
		//添加弱小目标检测算法			
		dsp0_scan6();
	}		

	CheckCycles++;
	
	
}


dsp0_ddr3verify()
{
	int i,j;
	Int16 * pRxadd1;
	Int16 * pRxadd2;

	
	pRxadd1=(Int16 *)(0x80000000);
	pRxadd2=(Int16 *)(0x80600000);

	pRxadd1=(pRxadd1+639);
	pRxadd2=(pRxadd2+639);

	for(i=0;i<512;i++)
	{
		if(*pRxadd1<5)
			
		pRxadd1=(pRxadd1+640);
		
	}

	for(j=0;j<512;j++)
	{
		if(*pRxadd2<5)

		pRxadd2=(pRxadd2+640);
		
	}
















}

/**************************************************
 Prototype     : global_parameters_init
 Description   : 流程控制参数初始化
 Input           : void  
 Output         : None
 Return Value :   

 Date            : 
 Author   	      : 
 Modification  : 
***************************************************/
void global_parameters_init(void)
{
	int i,j;
	long long *pScanCheck5last;

	
	ScanCheckflag=6;

	
	//初始化变量	
	CheckCycles=0;//检测次数
	ScanCheck5lastNum=0;	
	targetempty =0;
	modeswitch=0;

	//K1st_Fcha=1.6;
	//TotalFcha=2.2;
	pScanCheck5last= (long long *)ScanCheck5last;
	for(j=0;j<80;j++)
	{
		pScanCheck5last[j]=0;
	}
	#if 0
	WorkMode = Mode_Scan;
	

	dspselfcheck=1;//dsp初始设置为不进行自检	
	SmallFlag=1;//用于区分大小目标搜索，1为小目标
	
	//使用形态学滤波
	//scan参数设置
	ScanCheckflag=3;//用于标识使用哪种scan算法3:形态学滤波 4:中值滤波
	//capture参数设置
	capturecheckflag=3;//用于标识使用哪种捕获算法
	TrackM1=CenterMorph;

	//track参数设置
	TrackMethod=1;//使用哪种跟踪算法
	TrackM2Flag=0;//初始匹配跟踪没有变化	
	temptrackflag=0;

	//使用中值滤波
	//ScanCheckflag=4;//用于标识使用哪种scan算法3:形态学滤波 4:中值滤波
	//capturecheckflag=4;//用于标识使用哪种捕获算法
	//TrackM1=CenterMiddle;

	RxFrameFlag=0;
	SRIO_temperatureflag1=0;
	SRIO_temperatureflag2=0;
	#endif
}

/**************************************************
 Prototype     : DSP0_image
 Description   : 
 Input           : void  
 Output         : None
 Return Value :   

 Date            : 
 Author   	      : 
 Modification  : 
***************************************************/
void DSP0_image(void)
{
	Uint32 i;
	Uint32 cycles1;

	
	unsigned int imageadd;
	DMA_MEM_Tran_Config   DMA_transfer_cfg;
	
	//流程控制参数初始化
	global_parameters_init();	
	RxFrameFlag=0;

	
	
	
	while(1)
	{
		//收到一帧图像后，进行处理
		
		//debug代码，用于调试，调试完成后，删除
		
		//{
			//debug代码，用于调试，调试完成后，删除
			//Debug_WorkMode=Mode_Track;
			//AcquireData(Dsp0ImageLink2Rx,1,ImageData); 
			//dsp0_AcquireData_loop(208,256);
			//debug代码，用于调试，调试完成后，删除	

			//dsp0_GetMedianNum_loop((Uint32 *)(0x100008),(Uint32 *)(0x40000),9);			
			//dsp0_scan();
		//}

		asm(" NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;");

		
		

		/*获得pbuf数据传递一个640*512的数组*/			
	DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
	DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
	DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
	DMA_transfer_cfg.CCNT=512;
#if 0	
	/*使用EDMA_CC0_CH1,传递一个640*512的数组*/
	//EDMA_copy_Type3(0x90000000, 0x0c040000,1, &DMA_transfer_cfg, 0);
	//EDMA_copy_Type3(0x0c340000, 0x0c040000,1, &DMA_transfer_cfg, 0);

	
		//if(CheckCycles%2==0)
		//EDMA_copy_Type3(0x80600000, 0x0c040000,1, &DMA_transfer_cfg, 0);
		//else
		//EDMA_copy_Type3(0x80000000, 0x0c040000,1, &DMA_transfer_cfg, 0);
	
	//CommandMode= 3;

	imageadd = 0x80000000+(CheckCycles%4)*655360;
	EDMA_copy_Type3(imageadd, 0x0c040000,1, &DMA_transfer_cfg, 0);


	
	RxFrameFlag=1;//delete after debug

#endif		
		
		

		

		//20171222调试dsp0_trackmode add
		//CommandMode= 2;
		
		if(gtrack4==1)
		{
			DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
			DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
			DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
			DMA_transfer_cfg.CCNT=512;

			/*使用EDMA_CC0_CH1,传递一个640*512的数组*/
			//EDMA_copy_Type3(0x90000000, 0x0c040000,1, &DMA_transfer_cfg, 0);
			if(rtxnumber>rtxnumberref)
			{
				RxFrameFlag=1;
				rtxnumberref = rtxnumber;

				if(rtxnumber%2==0)
				EDMA_copy_Type3(0x80600000, 0x0c040000,1, &DMA_transfer_cfg, 0);
				else
				EDMA_copy_Type3(0x80000000, 0x0c040000,1, &DMA_transfer_cfg, 0);
			}			
		}

		//evm6678 开发板调试单帧调试
		//gtrack4=2;
		CommandMode=2;
		if(gtrack4==2)
		{
			imageadd = 0x80000000;
			EDMA_copy_Type3(imageadd, 0x0c040000,1, &DMA_transfer_cfg, 0);

			RxFrameFlag=1;//delete after debug
		}
		

		
		//begin:20171121	调试使用:用于查看收到的图像数据
		
		//debug代码，用于调试，调试完成后，打开代码
		if(RxFrameFlag>0)
		{
			RxFrameFlag=0;
			cycles1= TSCL;
			
			if(g3==1)
			{
				EDMA_copy_Type3(0x80000000, 0x0c0e0000,0, &DMA_transfer_cfg, 0);
				
			}
			
			
			switch(CommandMode)
			{
				case Command_Scan:
						dsp0_scanmode();
						//dsp0_scan();
						//WorkMode = Mode_Scan;												
						break;
						
				case Command_Track:
						dsp0_trackmode();
						//dsp0_scan();
						//WorkMode=(CheckCycles<ScanNum)?WorkMode:Mode_ReCapture;						
						break;
						
				case Command_ReCapture:
						dsp0_recapturemode();
						//dsp0_scan();
						//WorkMode=(CheckCycles<ScanNum)?WorkMode:Mode_ReCapture;						
						break;
						
				default:
						//dsp0_ddr3verify();//用于验证DDR3的srio数据接收是否正确
						dsp0_scanmode();							
						break;						
				
			}
			//debug代码，用于调试，调试完成后，去掉代码
			//cycles12= TSC_count_cycle_from(cycles1);
		}
		

		asm(" NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;NOP;;");

		//更新发送数据

		

		//Flag更新标识

		
		//end:20171121	调试使用
		
		
	}
}

