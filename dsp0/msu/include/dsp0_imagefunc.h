/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_imagefunc.h
  Version       : Initial Draft V1.0
  Author        : 
  Created       : Nov 13, 2016
  Last Modified :
  Description   :存放图像处理函数的声明

  History       :
  1.Date        : Nov 13, 2016
    Author      : 
    Modification: Created file
******************************************************************************/
#ifndef __DSP0_IMAGEFUNC_H_
#define __DSP0_IMAGEFUNC_H_

//#include <tistdtypes.h>
#include "dsp0_image.h"
#include "dsp0_secextern.h"












void Pre_Hist(Int16 *pDataIn,int nWidth,int nHeight);
void GetMaxValue(int nTrimHValue);
void GetMinValue(int nTrimLValue);












#endif
