/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : SRIO_Init.c
  Version       : v1.0
  Author        : 
  Created       : 2015/10/24
  Last Modified :
  Description   : This file is used to initialize the SRIO for memory prote-
                  ction test
  Function List :
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "SRIO_init_drv.h"
#include "SRIO_Init.h"
#include "peripheral.h"

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
SerdesSetup_4links serdes_cfg;
SerdesLinkSetup serdesLinkSetup;
//SRIO_Message_Cfg msg_cfg;
SRIO_Config srio_cfg;
/*up to 16 deviceID can be setup here*/
SRIO_Device_ID_Routing_Config dsp0_device_ID_routing_config[]=
{
     /*idPattern 	idMatchMask 	routeMaintenance*/
	{DSP1_SRIO_BASE_ID+0, 	0xFFFF, 	1},
	//{DSP0_SRIO_BASE_ID+0, 	0xFFFF, 	1}	
};

SRIO_Interrupt_Map interrupt_map_srio[]=
{
    /*interrupt_event*/ /*INTDST_number*/
	{DOORBELL0_0_INT, 	INTDST_16}, 	/*route to core 0*/
	{DOORBELL0_1_INT, 	INTDST_16}, 	/*route to core 0*/
	//{DOORBELL0_2_INT, 	INTDST_16}, 	/*route to core 0*/
	//{DOORBELL0_3_INT, 	INTDST_16}, 	/*route to core 0*/
	//{DOORBELL0_4_INT, 	INTDST_16} 		/*route to core 0*/
};
SRIO_Interrupt_Cfg interrupt_cfg_srio;

/*****************************************************************************
 Prototype    : srio_identify_used_ports_lanes
 Description  : identify used ports lanes according to 1x 2x 4x path config-
                uration
 Input        : SRIO_1x2x4x_Path_Control srio_1x2x4x_path_control  
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
/* identify used ports lanes according to 1x 2x 4x path configuration*/
void srio_identify_used_ports_lanes(
	SRIO_1x2x4x_Path_Control srio_1x2x4x_path_control)
{
	int i;
	Uint32 uiPathMode;
	Uint32 uiLaneEnableMask=0, uiLogicPortEnableMask=0;

	/* SRIO 1x 2x 4x path mode configuration:

	In Configuration 4, a maximum of 4 active ports and 4 lanes per path are supported as follows:
	 Mode 0: Four independent 1x ports in lanes A, B, C, and D
	 Mode 1: One independent 2x port in lanes {A,B}, and two independent 1x ports in lanes C and D
	 Mode 2: Two independent 1x ports in lanes A and B, and one independent 2x port in lanes {C,D}
	 Mode 3: Two independent 2x ports, occupying lanes {A,B} and {C,D} respectively
	 Mode 4: One independent 4x port in lanes {A,B,C,D}*/

	uiPathMode= (srio_1x2x4x_path_control&7)>>0;

	uiLaneEnableMask= 0xF; 	/*1111*/

	if(0==uiPathMode)
		uiLogicPortEnableMask= 0xF; 	/*1111*/
	else if(1==uiPathMode)
		uiLogicPortEnableMask= 0xD; 	/*1101*/
	else if(2==uiPathMode)
		uiLogicPortEnableMask= 0x7; 	/*0111*/
	else if(3==uiPathMode)
		uiLogicPortEnableMask= 0x5; 	/*0101*/
	else if(4==uiPathMode)
		uiLogicPortEnableMask= 0x1; 	/*0001*/


	/*enable ports data path according to 1x 2x 4x path configuration*/
	srio_cfg.blockEn.bBLK5_8_Port_Datapath_EN[0]= uiLaneEnableMask&1;
	srio_cfg.blockEn.bBLK5_8_Port_Datapath_EN[1]= (uiLaneEnableMask>>1)&1;
	srio_cfg.blockEn.bBLK5_8_Port_Datapath_EN[2]= (uiLaneEnableMask>>2)&1;
	srio_cfg.blockEn.bBLK5_8_Port_Datapath_EN[3]= (uiLaneEnableMask>>3)&1;

	/*disable Serdes according to 1x 2x 4x path configuration*/
	for(i= 0; i<4; i++ )
	{
		if(uiLaneEnableMask&(1<<i))
			srio_cfg.serdes_cfg->linkSetup[i]= &serdesLinkSetup;
		else
			srio_cfg.serdes_cfg->linkSetup[i]= NULL;
	}

	/*enable loggical ports according to 1x 2x 4x path configuration*/
	srio_cfg.blockEn.bLogic_Port_EN[0]= uiLogicPortEnableMask&1;
	srio_cfg.blockEn.bLogic_Port_EN[1]= (uiLogicPortEnableMask>>1)&1;
	srio_cfg.blockEn.bLogic_Port_EN[2]= (uiLogicPortEnableMask>>2)&1;
	srio_cfg.blockEn.bLogic_Port_EN[3]= (uiLogicPortEnableMask>>3)&1;	
}
/*****************************************************************************
 Prototype    : SRIO_Init
 Description  : SRIO initialization for memory protection test
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_Init()
{	
	/*clear configuration structrue to make sure unused field is 0*/
	memset(&srio_cfg, 0, sizeof(srio_cfg)); 

	/* SRIOSGMIICLK的时钟信号*/
	serdes_cfg.commonSetup.inputRefClock_MHz = SRIO_CLOCK_MHZ;	
	
	srio_cfg.blockEn.bBLK1_LSU_EN= 1;
	srio_cfg.blockEn.bBLK2_MAU_EN= 1;
	srio_cfg.blockEn.bBLK3_TXU_EN= 1;
	srio_cfg.blockEn.bBLK4_RXU_EN= 1;

	/* SRIO_NO_LOOPBACK */
	srio_cfg.loopback_mode= SRIO_NO_LOOPBACK;
	
	srio_cfg.device_ID_routing_config= dsp0_device_ID_routing_config;
	srio_cfg.uiNumDeviceId= 
		sizeof(dsp0_device_ID_routing_config)/sizeof(SRIO_Device_ID_Routing_Config);

	serdes_cfg.commonSetup.loopBandwidth= SERDES_PLL_LOOP_BAND_MID;
	srio_cfg.serdes_cfg= &serdes_cfg;

	serdesLinkSetup.txOutputSwing    = 15; /*0~15 represents between 100 and 850 mVdfpp  */
	serdesLinkSetup.testPattern      = SERDES_TEST_DISABLED; 
	serdesLinkSetup.rxAlign          = SERDES_RX_COMMA_ALIGNMENT_ENABLE; 
	serdesLinkSetup.rxInvertPolarity = SERDES_RX_NORMAL_POLARITY; 
	serdesLinkSetup.rxTermination    = SERDES_RX_TERM_COMMON_POINT_AC_COUPLE; 
	serdesLinkSetup.rxEqualizerConfig= SERDES_RX_EQ_ADAPTIVE; 
    serdesLinkSetup.rxCDR            = 5;
	serdesLinkSetup.txInvertPolarity = SERDES_TX_NORMAL_POLARITY; 

	/*for 319 project debug:SRIO根据需求修改*/
    serdesLinkSetup.linkSpeed_GHz = SRIO_SPEED_GHZ;
	/*for 319 project debug:SRIO根据需求修改*/
    srio_cfg.srio_1x2x4x_path_control = test_2DSP_cfg.multiple_port_path;
    
	srio_cfg.msg_cfg= NULL;

	//SRIO中断配置
	interrupt_cfg_srio.interrupt_map = interrupt_map_srio;
	interrupt_cfg_srio.uiNumInterruptMap = 
		sizeof(interrupt_map_srio)/sizeof(SRIO_Interrupt_Map);

	/*interrupt rate control is not used in this test*/
	interrupt_cfg_srio.interrupt_rate= NULL;
    interrupt_cfg_srio.uiNumInterruptRateCfg= 0;
	
	interrupt_cfg_srio.doorbell_route_ctl= SRIO_DOORBELL_ROUTE_TO_DEDICATE_INT;
	srio_cfg.interrupt_cfg = &interrupt_cfg_srio;

    srio_identify_used_ports_lanes(srio_cfg.srio_1x2x4x_path_control);

	SRIO_drv_Init(&srio_cfg);   

}
