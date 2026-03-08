/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : SRIO_Init.h
  Version       : 
  Author        : 
  Created       : 
  Last Modified :
  Description   :  SRIO_Init.c header file
  Function List :
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

******************************************************************************/

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

#ifndef __SRIO_INIT_H__
#define __SRIO_INIT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "SRIO_init_drv.h"

//8bit device ID
//#define DSP0_SRIO_BASE_ID	0xA0
//#define DSP1_SRIO_BASE_ID	0xB0
//16bit device ID
#define DSP0_SRIO_BASE_ID	0x00B0
#define DSP1_SRIO_BASE_ID	0x00B0

 /*SRIO test speed at GHz*/
#define SRIO_SPEED_GHZ 	3.125f
#define SRIO_CLOCK_MHZ 	156.25f



typedef struct 
{
	SRIO_Packet_Type packet_type;

	/*for directIO test, this is source address in memory;
	for message test, this is source queue number*/
	Uint32 source; 	

	/*for directIO test, this is destination address in memory;
	for message test, this is dest device ID, different dest ID map to
	different flow to different memory in this test*/
	Uint32 dest;

	Uint32 byteCount;
	
}SRIO_Transfer_Param;

typedef struct 
{
	SRIO_1x2x4x_Path_Control multiple_port_path;
	/*test parameters for 4 ports, set 0 for unused port*/
	SRIO_Transfer_Param transfer_param[4]; 	
}SRIO_Multiple_Test_Config;


extern SRIO_Config srio_cfg;

extern void SRIO_Init();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SRIO_INIT_H__ */
