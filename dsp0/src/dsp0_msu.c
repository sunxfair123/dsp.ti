/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : dsp0_msu.c
  Version       : Initial Draft
  Author        : sxf
  Created       : 2016/01/08
  Last Modified :
  Description   : This file is the entrance of MSU                
  Function List :
              LL2_EDC_ISR
              Robust_Interrupts_Init
              SL2_EDC_ISR
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

#include "dsp0_msu.h"
//#include "Robust_MNav_Init.h"
#include "KeyStone_Packet_Descriptor.h"



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


/* for 319 project debug */
/* 构造数据用于验证 */
Uint32 packetBuffer_DDR_Check[64]=
{
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd,
0x12345678, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0xdddddddd,
0xffffffff, 0xaaaaaaaa, 0x55555555, 0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x12345678, 0x89abcdef, 0xffffffff, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd,
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xffffffff, 0xaaaaaaaa, 0x55555555
};

/*****************************************************************************
 Prototype    : MSU_Image
 Description  : MSU image module
 Input        : None
 Output       : None
 Return Value : None
 
  History        :
  1.Date         : 2016/01/08
    Author       : sxf
    Modification : Created function
 Ps:
WritebackCache 限制uiByteCount最大256k
MSU_Image 最后在core的中断函数中处理   
测试函数运行效率，使用cycles(max 4s) and cycles64
*****************************************************************************/
void MSU_Image()
{
	Uint32 eventFlag;
	Uint32 uicircles;
	Uint32 uiStartIdx;
	Uint32 i;
	Uint32 j;

	Uint32 uiEntryCount;
	Uint32 uiByteCount;
	//HostPacketDescriptor * hostDescriptor;
	Uint32 * uipDstBufPtr;

	
	//处理函数

	

	/*set the parameter,留作以后根据实际需求改写*/
	//if(eventFlag==1)
	{
		uiStartIdx=0;
		uicircles=1;
	}
	
#ifdef CORE0_SOURCE	
	for(i=0;i<0x10;i++)
	{
		/* for 319 project debug */
		//MSU_Core1_Image();

		/* for 319 project debug :add by sxf 20130303*/
		Core0_Image();
	}









	
#endif	


#if 0


#ifdef CORE1_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		//cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core1_Image*/
		MSU_Core1_Image();
		
		//cycles64= TSC_count_cycle64_from(cycles64);
		//printf("consumes %lld cycles\n", cycles64);
	}
#endif




#ifdef CORE2_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core2_Image*/
		MSU_Core2_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif

#ifdef CORE3_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core3_Image*/
		MSU_Core3_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif


#ifdef CORE4_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core4_Image*/
		MSU_Core4_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif

#ifdef CORE5_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core3_Image*/
		MSU_Core5_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif


#ifdef CORE6_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core2_Image*/
		MSU_Core6_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif

#ifdef CORE7_SOURCE
	/*poll the packet descriptor in destination queue*/	
	while(1)
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);
		
		/*MSU_Core7_Image*/
		MSU_Core7_Image();
		
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}
#endif
#endif


}



