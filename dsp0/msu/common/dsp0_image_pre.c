/******************************************************************************

  Copyright (C), 2016 xxxx,   Inc.  All rights reserved.

 ******************************************************************************
  File Name     : dsp0_image_pre.c
  Version       : Initial Draft V1.0
  Author        : 
  Created       : Nov 13, 2016
  Last Modified :
  Description   :图像处理的预处理函数

  History       :
  1.Date        : Nov 13, 2016
    Author      : 
    Modification: Created file
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "dsp0_common.h"
#include "dsp0_imagefunc.h"
#include "dsp0_secextern.h"




/**************************************************
* Prototype     : PreFilter_scan4
* Description   : 
* Input           :int2x16 *pDataIn——输入数据首地址
*		  	  int iDataWidth——处理数据的横向大小
*		   	  int iDataHeight——处理数据的纵向大小         
* Return Value : int2x16 *pDataOut——滤波后数据首地址

* Date            : 
* Author   	 : 
* Modification  : 
***************************************************/
