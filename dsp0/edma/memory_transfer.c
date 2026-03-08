/******************************************************************************
	
 Copyright (C), 2001-2012, Texas Instrument.

******************************************************************************
File Name 	: memory_transfer.c
Version		: Initial Draft
Author		: sxf
Created		: 2016/01/08
Last Modified :
Description	: memory_transfe file
Function List :
History		:

  1.Date		: 2016/01/08
    Author	: sxf
    Modification: Created file
	
******************************************************************************/

#include <stdio.h>
#include <soc.h>
#include "memory_transfer.h" //add by sxf 20151209


/********************************************************
TC channel暂时这么使用，以后按需求调整
core 0:EDMA_CC0_CH0 EDMA_CC0_CH1
core 1:EDMA_CC1_CH0 EDMA_CC1_CH1
core 2:EDMA_CC1_CH2 EDMA_CC1_CH3
core 3:EDMA_CC2_CH0 EDMA_CC2_CH1
core 4:EDMA_CC2_CH2 EDMA_CC2_CH3
********************************************************/


