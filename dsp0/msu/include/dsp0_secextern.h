/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_secextern.h
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
#ifndef __DSP0_SECEXTERN_H_
#define __DSP0_SECEXTERN_H_



#include "dsp0_image.h"









//图像存储区



extern Int16  ImageData_5[];
extern Int16  ImageDataTemp1_5[];
extern Int16  ImageDataTemp2_5[];







//存放目标搜索结果
extern Target_Track_t targetTemp[];
extern Target_Track_t targetTemp1[];

extern int tarNUM;



extern Target_Track_t ScanCheck5[];
extern Target_Track_t ScanCheck5last[];




extern Target_Record_t Scan5record[];//20171123调试临时使用

extern Target_Track_t1 Scan5error[];
extern Target_Track_t1 Scan5error1[];





//相关匹配算法参数


//直方图相关数组，计算阈值
extern Uint32 Hist[];
extern Uint32 Hist_a[];
extern Uint32 Hist_b[];


extern Mark4_t Mark4Con[1500];//4连通标记

extern Int16  Mark4[1500];


extern int  Sign[1500];





#endif
