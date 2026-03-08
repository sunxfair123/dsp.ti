/******************************************************************************
 *  ============================================================================
 *  @n   (C) Copyright 2011, 2012, Texas Instruments, Inc.
 *  @n   All Rights Reserved.
 *  @n
 *  @n GENERAL DISCLAIMER
 *  @n ------------------ 
 *  @n All software and related documentation is provided "AS IS" and without   
 *  @n warranty or support of any kind and Texas Instruments expressly disclaims
 *  @n all other warranties, express or implied, including, but not limited to, 
 *  @n the implied warranties of merchantability and fitness for a particular   
 *  @n purpose.  Under no circumstances shall Texas Instruments be liable for   
 *  @n any incidental, special or consequential damages that result from the    
 *  @n use or inability to use the software or related documentation, even if   
 *  @n Texas Instruments has been advised of the liability.                     
 *  ============================================================================
 ******************************************************************************
  File Name     : dsp0_main.c
  Version       : v1.0  Initial Draft
  Author        : sxf
  Created       : 2017/02/10
  Last Modified :
  Description   : 
       This project shows how to use the protection hardware to build the robust
    system on KeyStone device
  Function List :
              main
  History       :
  1.Date        : 2017/02/10
    Author      : sxf
    Modification: Created file

  2.Date         : 
    Author       : 
    Modification : 
    
    本工程为debug工程,调试完成后注释掉所有////for 319 project debug
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "dsp0_msu.h"





 
extern void mss_main();
/*****************************************************************************
 Prototype    : main
 Description  : system test main function
 Input        : void  
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
void main(void)
{
	//int i;
	mss_main();
	
	//业务处理代码core0为中断处理代码
	//eg: core1_image();
	/*MSU_Image ，临时使用业务处理代码*/
	MSU_Image();
}

