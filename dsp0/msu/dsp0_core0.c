/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : dsp0_core0.c
  Version       : Initial Draft
  Author        : sxf
  Created       : 2016/01/08
  Last Modified :
  Description   : This file is the entrance of MSU                
  Function List :
              LL2_EDC_ISR
              Robust_Interrupts_Init
              Timer_ISR
  History       :
  1.Date        : 
    Author      : 
    Modification: 

******************************************************************************/
#include <stdio.h>
#include <c6x.h>
#include <tistdtypes.h>
#include <csl_cpintc.h>
#include "dsp0_common.h"
#include "CSL_msmc.h"
#include "CSL_msmcAux.h"
#include <cslr_chip.h>

#include "dsp0_core0.h"
//#include "../src/Robust_memory_transfer.h"
//#include "../src/Robust_MNav_Init.h"


/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/


/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
//[cpp] view plain copyprint?
#include <stdlib.h>  
//#include <stdio.h> 
#include <math.h>
#include <time.h>  




//#include <time.h>
//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/timeb.h>
#include <string.h>
#include "dsp0_image.h"



/*****************************************************************************
 Prototype    : Core0_Image
 Description  : 运行图像处理
 Input        : None
 Output       : None
 Return Value : None
 
  History        :
  1.Date         : 2016/03/03
    Author       : sxf
    Modification : Created function
 Ps:

*****************************************************************************/
unsigned int Core0_Image()
{
	Uint32 i;
	Uint32 ret=0;
	/* for 319 project debug */
	//Uint32 cycles;
	unsigned long long cycles64;

#if 1
	cycles64=_itoll(TSCH, TSCL);
	//for(i=0;i<1000000000;i++)
		ret=i;
	cycles64= TSC_count_cycle64_from(cycles64);
	printf("test consumes %lld cycles64\n", cycles64);
#endif /* 0 */

	/* for 319 project debug */
	//cycles= TSCL;
	/*移植imagetarget工程到C6678上,进行测试*/
	//ret=main_imagetarget();
	//cycles= TSC_count_cycle_from(cycles);		
	//printf("consumes %5d cycles\n", cycles);
	
	//cycles64=_itoll(TSCH, TSCL);
	/*移植imagetarget工程到C6678上,进行测试*/
	//ret=main_imagetarget();
		
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("consumes %lld cycles\n", cycles64);

	/*移植算法，进行测试*/
	DSP0_image();

	return ret;
}



