/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_imagefunc.c
  Version       : Initial Draft V1.0
  Author        : 
  Created       : Nov 13, 2016
  Last Modified :
  Description   :小目标的图像处理处理函数

  History       :
  1.Date        : Nov 13, 2016
    Author      : 
    Modification: Created file
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <c6x.h>
#include "dsp0_common.h"
#include "dsp0_imagefunc.h"
#include "dsp0_secextern.h"



Target_Hist_t m_Histogram;

//图像存储区



//20170515 add
//使用共享内存存放数据
//图像处理区存储模块
#pragma DATA_SECTION(ImageData_5,"ImageData.SL2")
Int16 ImageData_5[ScanSize];//640*512*16bit---原始图像

#pragma DATA_SECTION(ImageDataTemp1_5,"ImageData.SL2")
Int16 ImageDataTemp1_5[ScanSize];//640*512*16bit---原始图像



#pragma DATA_SECTION(ImageDataTemp2_5,"ImageData.SL2")
Int16 ImageDataTemp2_5[ScanSize];//640*512*16bit---原始图像



//直方图相关数组，计算阈值
#pragma DATA_SECTION(Hist,"ImageData.SL2")
Uint32 Hist[GrayStep];
#pragma DATA_SECTION(Hist_a,"ImageData.SL2")
Uint32 Hist_a[GrayStep];
#pragma DATA_SECTION(Hist_b,"ImageData.SL2")
Uint32 Hist_b[GrayStep];


//处理结果
//#pragma DATA_SECTION(Mark4,"ImageData.SL2")
//int Mark4[1500];
//#pragma DATA_SECTION(Mark4Con,"ImageData.SL2")
//Mark4_t Mark4Con[1500];//4连通标记

//处理结果
#pragma DATA_SECTION(Mark4Con,"ImageData.SL2")
Mark4_t Mark4Con[1500];//4连通标记
#pragma DATA_SECTION(Mark4,"ImageData.SL2")
Int16 Mark4[1500];




//add 20170523
//处理结果
//#pragma DATA_SECTION(Sign,"ImageData.SL2")
//Int32 Sign[1500];





//存放目标结果
int tarNUM;
#pragma DATA_SECTION(targetTemp,"ImageData.SL2")
Target_Track_t targetTemp[100];

#pragma DATA_SECTION(targetTemp1,"ImageData.SL2")
Target_Track_t targetTemp1[100];


 
#pragma DATA_SECTION(ScanCheck5,"ImageData.SL2")
Target_Track_t ScanCheck5[20];


#pragma DATA_SECTION(ScanCheck5last,"ImageData.SL2")
Target_Track_t ScanCheck5last[20];


//20171123调试临时使用
#pragma DATA_SECTION(Scan5record,"ImageData.SL2")
Target_Record_t Scan5record[1000];

#pragma DATA_SECTION(Scan5error,"ImageData.SL2")
Target_Track_t1 Scan5error[1000];

#pragma DATA_SECTION(Scan5error1,"ImageData.SL2")
Target_Track_t1 Scan5error1[1000];





void Pre_Hist(Int16 *pDataIn,int nWidth,int nHeight)
{
	int i,j;
	Int16 nIndex;//图像灰度值
	int WidthAddr,HeightAddr;//dsp存储空间需要的宽度和长度	
	Int16 *pDataIn16;
	
		
	WidthAddr  = nWidth;
	HeightAddr = nHeight;	
	pDataIn16=pDataIn;	
	m_Histogram.nMostGray=0;
	m_Histogram.nMostNum=0;
	m_Histogram.lGray=0;
	m_Histogram.hGray=0;
	
	for(i=0;i<GrayStep;i++)
	{			
		Hist[i]=0;
	}
	for(j=0;j<HeightAddr;j++)
	{		
		for(i=0;i<WidthAddr;i++)
		{			
			nIndex=*(pDataIn16+j*WidthAddr+i);
			Hist[nIndex]++;
			if (Hist[nIndex]>m_Histogram.nMostNum)
			{
				m_Histogram.nMostNum=Hist[nIndex];
				m_Histogram.nMostGray=nIndex;
			}
		}
	}		
}


/**************************************************
* Prototype     : GetMaxValue
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
void GetMaxValue(int nTrimHValue)
{
	int i;
	//Uint32 nMaxGray=0;
	Uint32 nTrimH;
	nTrimH=0;
	for (i=0x3fff;i>0;i--)
	{
		if (Hist[i]!=0)
		{
			nTrimH=nTrimH+Hist[i];
			if (nTrimH>nTrimHValue)//选取第10个最大值
			{
				//nMaxGray=i;
				break;
			}
		}
	}
	m_Histogram.hGray=i;
}



/**************************************************
* Prototype     : GetMinValue
* Description   : 
* Input           :
* Return Value :  

* Date            : 20170516
* Author   	 : 
* Modification  : 
***************************************************/
void GetMinValue(int nTrimLValue)
{
	int i;
	//Uint32 nMinGray=0;
	Uint32 nTrimL=0;
	nTrimL=0;
	for (i=1;i<0x4000;i++)
	{
		if (Hist[i]!=0)
		{
			nTrimL=nTrimL+Hist[i];
			if (nTrimL>nTrimLValue)//m_nTrimL)//AUOT_TRIM_LOW)
			{
				//nMinGray=i;
				break;
			}			
		}
	}
	m_Histogram.lGray=i;
}

/**************************************************
* Prototype     : OtsuTHsec4
* Description   : 利用Otsu方法分四段求分割阈值，
*          用于检测阶段
* Input           :int2x16 *pDataIn——输入数据首地址
*				pThreshP1——分割阈值1
*				pThreshP2——分割阈值2
* Return Value          :
* parameter :    int Hist[];//直方图
*         			 int Hist_a[];
*         			 int Hist_b[]; 

* Date            : 
* Author   	 : 
* Modification  : 
***************************************************/
