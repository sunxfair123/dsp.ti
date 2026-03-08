/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : KeyStone_SRIO_Init_drv.c
  Version       : Initial Draft
  Author        : Brighton Feng
  Created       : 2011/6/11
  Last Modified :
  Description   :  example for SRIO configuration and transfer driver on KeyStone DSP

  Function List :
              
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
******************************************************************************/
#include <stdio.h>
#include "csl_srioAux.h"
#include "csl_pscAux.h"
#include "SRIO_init_drv.h"
#include "Navigator_init_drv.h"

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

CSL_SrioRegs *  gpSRIO_regs =   (CSL_SrioRegs *)CSL_SRIO_CONFIG_REGS;
SerdesRegs * srioSerdesRegs;


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



/*****************************************************************************
 Prototype    : SRIO_GlobalEnable
 Description  : enable globally used blocks including MMR block in SRIO
 Input        : void  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_GlobalEnable(void)
{
  	gpSRIO_regs->RIO_GBL_EN = 1;   
	gpSRIO_regs->BLOCK_ENABLE_STATUS[0].RIO_BLK_EN= 1; //MMR_EN

	//wait for enable completion
	while(0x3 != (gpSRIO_regs->RIO_GBL_EN_STAT&0x3));

}

/*****************************************************************************
 Prototype    : SRIO_enable_blocks
 Description  : Enable SRIO blocks
 Input        : SRIO_Block_Enable * blockEn  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_enable_blocks(
	SRIO_Block_Enable * blockEn)
{
	gpSRIO_regs->BLOCK_ENABLE_STATUS[5].RIO_BLK_EN= blockEn->bBLK5_8_Port_Datapath_EN[0];
	gpSRIO_regs->BLOCK_ENABLE_STATUS[6].RIO_BLK_EN= blockEn->bBLK5_8_Port_Datapath_EN[1];
	gpSRIO_regs->BLOCK_ENABLE_STATUS[7].RIO_BLK_EN= blockEn->bBLK5_8_Port_Datapath_EN[2];
	gpSRIO_regs->BLOCK_ENABLE_STATUS[8].RIO_BLK_EN= blockEn->bBLK5_8_Port_Datapath_EN[3];

	gpSRIO_regs->BLOCK_ENABLE_STATUS[1].RIO_BLK_EN= blockEn->bBLK1_LSU_EN  ;
	gpSRIO_regs->BLOCK_ENABLE_STATUS[2].RIO_BLK_EN= blockEn->bBLK2_MAU_EN  ;
	gpSRIO_regs->BLOCK_ENABLE_STATUS[3].RIO_BLK_EN= blockEn->bBLK3_TXU_EN  ;
	gpSRIO_regs->BLOCK_ENABLE_STATUS[4].RIO_BLK_EN= blockEn->bBLK4_RXU_EN  ;

	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[5].RIO_BLK_EN_STAT != blockEn->bBLK5_8_Port_Datapath_EN[0]);
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[6].RIO_BLK_EN_STAT != blockEn->bBLK5_8_Port_Datapath_EN[1]);
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[7].RIO_BLK_EN_STAT != blockEn->bBLK5_8_Port_Datapath_EN[2]);
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[8].RIO_BLK_EN_STAT != blockEn->bBLK5_8_Port_Datapath_EN[3]);

	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[1].RIO_BLK_EN_STAT != blockEn->bBLK1_LSU_EN  );
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[2].RIO_BLK_EN_STAT != blockEn->bBLK2_MAU_EN  );
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[3].RIO_BLK_EN_STAT != blockEn->bBLK3_TXU_EN  );
	while(gpSRIO_regs->BLOCK_ENABLE_STATUS[4].RIO_BLK_EN_STAT != blockEn->bBLK4_RXU_EN  );

}

/*****************************************************************************
 Prototype    : SRIO_disable_all_blocks
 Description  : Disable all SRIO blocks
 Input        : None
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
void SRIO_disable_all_blocks()
{
	gpSRIO_regs->BLOCK_ENABLE_STATUS[1].RIO_BLK_EN= 0; //LSU_EN  
	gpSRIO_regs->BLOCK_ENABLE_STATUS[2].RIO_BLK_EN= 0; //MAU_EN  
	gpSRIO_regs->BLOCK_ENABLE_STATUS[3].RIO_BLK_EN= 0; //TXU_EN  
	gpSRIO_regs->BLOCK_ENABLE_STATUS[4].RIO_BLK_EN= 0; //RXU_EN  

	gpSRIO_regs->BLOCK_ENABLE_STATUS[5].RIO_BLK_EN= 0; //PORT0_EN
	gpSRIO_regs->BLOCK_ENABLE_STATUS[6].RIO_BLK_EN= 0; //PORT1_EN
	gpSRIO_regs->BLOCK_ENABLE_STATUS[7].RIO_BLK_EN= 0; //PORT2_EN
	gpSRIO_regs->BLOCK_ENABLE_STATUS[8].RIO_BLK_EN= 0; //PORT3_EN

	gpSRIO_regs->BLOCK_ENABLE_STATUS[0].RIO_BLK_EN= 0; //MMR_EN

	gpSRIO_regs->RIO_GBL_EN = 0;

	//wait for disable completion
	while(gpSRIO_regs->RIO_GBL_EN_STAT&1);	
	
}

/*****************************************************************************
 Prototype    : SRIO_soft_reset
 Description  : soft shutdown and reset SRIO
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_soft_reset()
{
	int i, j, k;
	
	/*shut down TXU/RXU transaction*/
	for(i=0; i<SRIO_PKTDMA_MAX_CH_NUM; i++)
	{
		pktDma_RxCh_teardown(gpSRIO_DMA_RxChCfgRegs, i, PKTDMA_WAIT_FOREVER);
		pktDma_TxCh_teardown(gpSRIO_DMA_TxChCfgRegs, i, PKTDMA_WAIT_FOREVER);
	}

	for(i= 0; i<SRIO_MAX_LSU_NUM ; i++)
	{
		/*flash LSU transfer for all Source ID*/
		for(j=0; j< SRIO_MAX_DEVICEID_NUM; j++)
		{
			gpSRIO_regs->LSU_CMD[i].RIO_LSU_REG6 = 
				CSL_SRIO_RIO_LSU_REG6_FLUSH_MASK| /*flash*/
				(j<<CSL_SRIO_RIO_LSU_REG6_SCRID_MAP_SHIFT); 

			/*This can take more than one cycle to do the flush. 
			wait for a while*/
			for(k=0; k< 100; k++)
				asm(" nop 5");
		}
	}

	/*disable the PEREN bit of the PCR register to stop all
	new logical layer transactions.*/
	gpSRIO_regs->RIO_PCR &= (~CSL_SRIO_RIO_PCR_PEREN_MASK);

	/*Wait one second to finish any current DMA transfer.*/
	for(i=0; i< 100000000; i++)
		asm(" nop 5");

	//reset all logic blocks in SRIO
	SRIO_disable_all_blocks();

	//disable Serdes
	Serdes_disable(srioSerdesRegs, 4);
	
	//disable SRIO through PSC
	debug_disable_PSC_module(CSL_PSC_PD_SRIO, CSL_PSC_LPSC_SRIO);
	debug_disable_PSC_Power_Domain(CSL_PSC_PD_SRIO);
}

/*****************************************************************************
 Prototype    : SRIO_set_1x2x4x_Path
 Description  : configure SRIO 1x 2x or 4x path mode
 Input        : SRIO_1x2x4x_Path_Control srio_1x2x4x_path_control  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_set_1x2x4x_Path(
	SRIO_1x2x4x_Path_Control srio_1x2x4x_path_control)
{
	/*This register is a global register, even though it can be accessed 
	from any port. So you do not need to program from each port, it is 
	basically a single register.
	The PathID is a RO value, that is driven by the H/W. You cannot modify it*/
	gpSRIO_regs->RIO_PLM[0].RIO_PLM_SP_PATH_CTL= 
		(gpSRIO_regs->RIO_PLM[0].RIO_PLM_SP_PATH_CTL&(~SRIO_1x2x4x_PATH_CONTROL_MASK))|
		srio_1x2x4x_path_control;

}

/*****************************************************************************
 Prototype    : SRIO_Timeout_Config
 Description  : SRIO timeout configuration in microsecond
 Input        : SRIO_Config * srio_cfg            
                Uint32 logicalRepsonseTimeout_us  
                Uint32 physicalPortTimeout_us     
                Uint32 linkInitSilenceTimeout_us  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_Timeout_Config(SRIO_Config * srio_cfg,
	Uint32 logicalRepsonseTimeout_us, 
	Uint32 physicalPortTimeout_us, 
	Uint32 linkInitSilenceTimeout_us)
{
	Uint32 uiTimeout, uiMaxTimeout;
	Uint32 byteClockMHz;

	/*PRESCALAR_SELECT is used to divide VBUSM clock(normally 333 to 400MHz, 
	here use 350MHz), (VBUS_M clock)/(PRESCALAR_SELECT+1), 
	to get about 50MHz clock with about 20ns period.*/
	gpSRIO_regs->RIO_PER_SET_CNTL= (gpSRIO_regs->RIO_PER_SET_CNTL
		&(~CSL_SRIO_RIO_PER_SET_CNTL_PRESCALER_SELECT_MASK))
		|((gDSP_Core_Speed_Hz/1000000/3/50-1)<<CSL_SRIO_RIO_PER_SET_CNTL_PRESCALER_SELECT_SHIFT);

	/*logical layer response timeout
	logicalRepsonseTimeout = 15 x (scaled VBUS_M clock period) x TIMEOUT_VALUE),
	TIMEOUT_VALUE = logicalRepsonseTimeout/(scaled VBUS_M clock period)/15 */
	uiTimeout= logicalRepsonseTimeout_us*1000/20/15;
	uiMaxTimeout=CSL_SRIO_RIO_SP_RT_CTL_TVAL_MASK
		>>CSL_SRIO_RIO_SP_RT_CTL_TVAL_SHIFT;
	if(uiTimeout>uiMaxTimeout)
		uiTimeout= uiMaxTimeout;
	if(0==uiTimeout)
		uiTimeout= 1;
	gpSRIO_regs->RIO_SP_RT_CTL= uiTimeout<<CSL_SRIO_RIO_SP_RT_CTL_TVAL_SHIFT;

	/*SRV_CLK should be scaled to about 10MHz (about 100ns period).
	SRV_CLK= (a SRIO internal IP clock)/ PRESCALAR_SRV_CLK.
	The SRIO internal IP clock is the byte clock of 
	one of the lanes (selected by SYS_CLK_SEL). 
	(Byte clock) = (link rate)/20.*/
	byteClockMHz= 
		srio_cfg->serdes_cfg->linkSetup[srio_cfg->SYS_CLK_SEL]->linkSpeed_GHz*1000/20;
	gpSRIO_regs->RIO_PRESCALAR_SRV_CLK= byteClockMHz/10;

	/*physical layer response timeout.
	physicalPortTimeout = SRV_CLK period * TIMEOUT_VALUE * 3,
	TIMEOUT_VALUE = physicalPortTimeout/SRV_CLK period/3 */
	uiTimeout= physicalPortTimeout_us*1000/100/3;
	uiMaxTimeout=CSL_SRIO_RIO_SP_LT_CTL_TVAL_MASK
		>>CSL_SRIO_RIO_SP_LT_CTL_TVAL_SHIFT;
	if(uiTimeout>uiMaxTimeout)
		uiTimeout= uiMaxTimeout;
	if(0==uiTimeout)
		uiTimeout= 1;
	gpSRIO_regs->RIO_SP_LT_CTL= uiTimeout<<CSL_SRIO_RIO_SP_LT_CTL_TVAL_SHIFT;
	
	/*port silence timeout
	The SRIO starts in the SILENT state. The link output driver is disabled 
	to force the link partner to initialize regardless of its current state. 
	The duration of the SILENT state is controlled by the silence_timer. 
	The duration must be long enough to ensure that the link partner detects 
	the silence (as a loss of lane_sync) and is forced to initialize but short 
	enough that it is readily distinguished from a link break.
	linkInitSilenceTimeout is SRV_CLK period X 410 X SILENCE_TIMER,
	SILENCE_TIMER= linkInitSilenceTimeout/SRV_CLK period/410*/
	uiTimeout= linkInitSilenceTimeout_us*1000/100/410;
	uiMaxTimeout=CSL_SRIO_RIO_PLM_SP_SILENCE_TIMER_SILENCE_TIMER_MASK
		>>CSL_SRIO_RIO_PLM_SP_SILENCE_TIMER_SILENCE_TIMER_SHIFT;
	if(uiTimeout>uiMaxTimeout)
		uiTimeout= uiMaxTimeout;
	if(0==uiTimeout)
		uiTimeout= 1;
	uiTimeout= uiTimeout
		<<CSL_SRIO_RIO_PLM_SP_SILENCE_TIMER_SILENCE_TIMER_SHIFT;
	if(srio_cfg->blockEn.bLogic_Port_EN[0])
		gpSRIO_regs->RIO_PLM[0].RIO_PLM_SP_SILENCE_TIMER= uiTimeout;
	if(srio_cfg->blockEn.bLogic_Port_EN[1])
		gpSRIO_regs->RIO_PLM[1].RIO_PLM_SP_SILENCE_TIMER= uiTimeout;
	if(srio_cfg->blockEn.bLogic_Port_EN[2])
		gpSRIO_regs->RIO_PLM[2].RIO_PLM_SP_SILENCE_TIMER= uiTimeout;
	if(srio_cfg->blockEn.bLogic_Port_EN[3])
		gpSRIO_regs->RIO_PLM[3].RIO_PLM_SP_SILENCE_TIMER= uiTimeout;
	
	
}

/*****************************************************************************
 Prototype    : SRIO_CSR_CAR_Config
 Description  : configure SRIO standard Command Status Capability registers
 Input        : SRIO_Config * srio_cfg  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_CSR_CAR_Config(SRIO_Config * srio_cfg)
{
	int i;
	Uint32 uiRegisterValue;
	Uint32 uiSpeed_MHz;
	
    gpSRIO_regs->RIO_DEV_ID	= 0x009d0030;

	/*The lower 4b should match the 4b from the JTAG Variant field of 
	the DeviceID register.*/
    gpSRIO_regs->RIO_DEV_INFO	= gpBootCfgRegs->DEVICE_ID_REG0>>28; 
    
    gpSRIO_regs->RIO_PE_FEAT	= 0x200001f9;
		
    gpSRIO_regs->RIO_SW_PORT    = 0x00000004;
    
    gpSRIO_regs->RIO_SRC_OP     = 0x0004fdf4;
		
    gpSRIO_regs->RIO_DEST_OP    = 0x0000fc04;   

	/*PE supports 34 bit addresses*/
	gpSRIO_regs->RIO_PE_LL_CTL= 0x00000001;

	/*The host base device ID lock CSR contains the base device ID value for 
	the processing element in the system that is responsible for initializing
	this processing element.*/
    gpSRIO_regs->RIO_HOST_BASE_ID_LOCK	= 
    	srio_cfg->device_ID_routing_config[0].idPattern;

	/*Software defined component tag for the PE. 
	Useful for devices without device IDs.*/
	gpSRIO_regs->RIO_COMP_TAG	= 0 ;  // not touched
    
    /* port general control */
    gpSRIO_regs->RIO_SP_GEN_CTL 		=  0x40000000;

	/*port control*/
	for(i= 0; i< SRIO_MAX_PORT_NUM; i++)
	{
		if(FALSE==srio_cfg->blockEn.bLogic_Port_EN[i])
			continue;

		uiRegisterValue= 0x00600001;
			
		if(NULL!=srio_cfg->flowControlID)
			uiRegisterValue|= 
				(1<<CSL_SRIO_RIO_SP_CTL_FLOW_CTRL_SHIFT   )
				|(1<<CSL_SRIO_RIO_SP_CTL_FLOW_ARB_SHIFT    );
		gpSRIO_regs->RIO_SP[i].RIO_SP_CTL = uiRegisterValue;

		uiSpeed_MHz= (Uint32)(srio_cfg->serdes_cfg->linkSetup[i]->linkSpeed_GHz*1000);
		if(1250==uiSpeed_MHz)
			gpSRIO_regs->RIO_SP[i].RIO_SP_CTL2 = 
				1<<CSL_SRIO_RIO_SP_CTL2_GB_1P25_EN_SHIFT;
		if(2500==uiSpeed_MHz)
			gpSRIO_regs->RIO_SP[i].RIO_SP_CTL2 = 
				1<<CSL_SRIO_RIO_SP_CTL2_GB_2P5_EN_SHIFT;
		if(3125==uiSpeed_MHz)
			gpSRIO_regs->RIO_SP[i].RIO_SP_CTL2 = 
				1<<CSL_SRIO_RIO_SP_CTL2_GB_3P125_EN_SHIFT;
		if(5000==uiSpeed_MHz)
			gpSRIO_regs->RIO_SP[i].RIO_SP_CTL2 = 
				1<<CSL_SRIO_RIO_SP_CTL2_GB_5P0_EN_SHIFT;
		if(6250==uiSpeed_MHz)
			gpSRIO_regs->RIO_SP[i].RIO_SP_CTL2 = 
				1<<CSL_SRIO_RIO_SP_CTL2_GB_6P25_EN_SHIFT;

		/*disable port write*/
		gpSRIO_regs->RIO_PLM[i].RIO_PLM_SP_ALL_PW_EN = 0;
	}
	
	/*clear port write dest Id, port write not used*/
    gpSRIO_regs->RIO_PW_TGT_ID= 0;
	/*disable port write*/
    gpSRIO_regs->RIO_EM_DEV_PW_EN = 0;
    
    /* Register Reset Control 
    Allows the SELF_RST and PWDN_PORT resets to clear sticky register bits 
    in addition to the normal configuration registers.*/
    gpSRIO_regs->RIO_REG_RST_CTL = 
    	1 << CSL_SRIO_RIO_REG_RST_CTL_CLEAR_STICKY_SHIFT;


}

/*
port write feature should be disabled with code like below:
    gpSRIO_regs->SP_IP_MODE |=  
                        CSL_FMKT(SRIO_SP_IP_MODE_PW_DIS, DISABLE) |
*/

/*****************************************************************************
 Prototype    : SRIO_set_device_ID
 Description  : 配置本设备device_ID，
 inbound报文携带正确的目的ID，送入本设备，TLM接收报文
 Input        : SRIO_Device_ID_Routing_Config * device_id_routing_config  
                Uint32 uiDeviceIdNum                                      
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_set_device_ID(
	SRIO_Device_ID_Routing_Config * device_id_routing_config,
	Uint32 uiDeviceIdNum)
{
	int i;

	/*The TLM_SP(n)_BRR_x_PATTERN_MATCH registers hold the 15 allowable DestIDs, 
	note that the first register is not used.  We use the RIO_BASE_ID register 
	to hold the first ID */
	gpSRIO_regs->RIO_BASE_ID= device_id_routing_config[0].idPattern| 	/*Large ID*/
		((device_id_routing_config[0].idPattern&0xFF)<<16); 		/*small ID*/

	uiDeviceIdNum= _min2(SRIO_MAX_DEVICEID_NUM, uiDeviceIdNum);
	for(i=1;i<uiDeviceIdNum;i++)
	{
		/*please note, SRIO block 5~8 must be eanbled for corresponding
		RIO_TLM[0:3] taking effect*/
	    gpSRIO_regs->RIO_TLM[i/4].brr[i&3].RIO_TLM_SP_BRR_CTL = 
			(device_id_routing_config[i].routeMaintenance<<0x0000001A)|
			(0<<0x00000018)|(1<<0x0000001F);

	    gpSRIO_regs->RIO_TLM[i/4].brr[i&3].RIO_TLM_SP_BRR_PATTERN_MATCH = 
			(device_id_routing_config[i].idPattern<<
				CSL_SRIO_RIO_TLM_SP_BRR_1_PATTERN_MATCH_PATTERN_SHIFT)|
			(device_id_routing_config[i].idMatchMask<<
				CSL_SRIO_RIO_TLM_SP_BRR_1_PATTERN_MATCH_MATCH_SHIFT);

	}	
}
/*****************************************************************************
 Prototype    : SRIO_RxMode_Setup
 Description  : Rx Mode configuration
 Input        : SRIO_RX_Mode * rxMode  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_RxMode_Setup(SRIO_RX_Mode * rxMode)
{
	int i;
	
	if(rxMode)
	{
		gpSRIO_regs->RIO_PER_SET_CNTL = (gpSRIO_regs->RIO_PER_SET_CNTL&
			(~CSL_SRIO_RIO_PER_SET_CNTL_LOG_TGT_ID_DIS_MASK))|
			(rxMode->accept_data_with_any_ID
				<<CSL_SRIO_RIO_PER_SET_CNTL_LOG_TGT_ID_DIS_SHIFT);

		/*set RX mode for all ports*/
		for(i=0; i<SRIO_MAX_PORT_NUM; i++)
		{
			gpSRIO_regs->RIO_TLM[i].RIO_TLM_SP_CONTROL = 
				(gpSRIO_regs->RIO_TLM[i].RIO_TLM_SP_CONTROL&
				(~(CSL_SRIO_RIO_TLM_SP_CONTROL_TGT_ID_DIS_MASK
				|CSL_SRIO_RIO_TLM_SP_CONTROL_MTC_TGT_ID_DIS_MASK)))
				|(rxMode->port_rx_mode[i].accept_maintenance_with_any_ID
				<<CSL_SRIO_RIO_TLM_SP_CONTROL_MTC_TGT_ID_DIS_SHIFT)
				|(rxMode->port_rx_mode[i].support_multicast_forwarding
				<<CSL_SRIO_RIO_TLM_SP_CONTROL_TGT_ID_DIS_SHIFT);
		}
	}
}

/*****************************************************************************
 Prototype    : KeyStone_SRIO_little_endian_swap
 Description  : Little endian swap
 Input        : SRIO_Little_Endian_Swap_Mode leSwapMode  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2011/6/11
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_SRIO_little_endian_swap(
	SRIO_Little_Endian_Swap_Mode leSwapMode)
{
#ifdef _LITTLE_ENDIAN
	//??? not sure how endian match with another processor in big-endian
	CSL_SRIO_SetLSUSwappingMode(gpSRIO_regs, leSwapMode);
	CSL_SRIO_SetTXURXUSwappingMode(gpSRIO_regs, leSwapMode);
#endif
}
/*****************************************************************************
 Prototype    : SRIO_Interrupt_init
 Description  : SRIO interrupt configuration
 Input        : SRIO_Interrupt_Cfg * interrupt_cfg  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_Interrupt_init(SRIO_Interrupt_Cfg * interrupt_cfg)
{
    Uint32 i;
    Uint32 reg, shift;
	volatile Uint32 * ICRR= (volatile Uint32 *)gpSRIO_regs->DOORBELL_ICRR;

	if(NULL == interrupt_cfg)
		return;
		
    /* Clear all the interrupts */
    for(i=0; i<2; i++)
    {
        gpSRIO_regs->LSU_ICSR_ICCR[i].RIO_LSU_ICCR	= 0xFFFFFFFF ;  
    }
    for(i=0; i<4; i++)
    {
        gpSRIO_regs->DOORBELL_ICSR_ICCR[i].RIO_DOORBELL_ICCR = 0xFFFFFFFF;        
    }
	gpSRIO_regs->RIO_ERR_RST_EVNT_ICCR = 0xFFFFFFFF;

    if(NULL != interrupt_cfg->interrupt_map)
    {
	    for(i=0; i<interrupt_cfg->uiNumInterruptMap; i++)
	    {
	        /* Get register index for the interrupt source*/
	        reg = interrupt_cfg->interrupt_map[i].interrupt_event >> 16;

	        /* Get shift value for the interrupt source*/
	        shift = interrupt_cfg->interrupt_map[i].interrupt_event & 0x0000FFFF;

	    	ICRR[reg]= (ICRR[reg]&(~(0xF<<shift))) 	/*clear the field*/
	    		|(interrupt_cfg->interrupt_map[i].INTDST_number<<shift);

	    }
	}

	gpSRIO_regs->RIO_INTERRUPT_CTL = interrupt_cfg->doorbell_route_ctl;

	/*disable interrupt rate control*/
	gpSRIO_regs->RIO_INTDST_RATE_DIS= 0xFFFF;
	for(i= 0; i< 16; i++)
	{
		gpSRIO_regs->RIO_INTDST_RATE_CNT[i]= 0;
	}
	
	if(NULL != interrupt_cfg->interrupt_rate)
	{
		/*setup interrupt rate for specific INTDST*/
		for(i= 0; i<interrupt_cfg->uiNumInterruptRateCfg; i++)
		{
			/*enable rate control for this INTDST*/
			gpSRIO_regs->RIO_INTDST_RATE_DIS &= 
				~(1<<interrupt_cfg->interrupt_rate[i].INTDST_number);

			/*set interrupt rate counter for this INTDST*/
			gpSRIO_regs->RIO_INTDST_RATE_CNT[i]= 
				interrupt_cfg->interrupt_rate[i].interrupt_rate_counter;
		}
	}    
}
/*****************************************************************************
 Prototype    : SRIO_Prioirity_Permission_Setup
 Description  : Priority setup
 Input        : SRIO_priority_permission * priority_permission  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_Prioirity_Permission_Setup(
	SRIO_priority_permission * priority_permission)
{
	if(NULL != priority_permission)
	{
		gpSRIO_regs->RIO_PER_SET_CNTL = (gpSRIO_regs->RIO_PER_SET_CNTL
			&(~(CSL_SRIO_RIO_PER_SET_CNTL_CBA_TRANS_PRI_MASK))) 	/*clear the field*/
			|(priority_permission->uiVbusPriority
				<<CSL_SRIO_RIO_PER_SET_CNTL_CBA_TRANS_PRI_SHIFT);
		gpSRIO_regs->RIO_SUPERVISOR_ID = 
			(priority_permission->supervisorHostID_8b<<
				CSL_SRIO_RIO_SUPERVISOR_ID_8B_SUPRVSR_ID_SHIFT)
			|(priority_permission->supervisorHostID_16b<<
				CSL_SRIO_RIO_SUPERVISOR_ID_16B_SUPRVSR_ID_SHIFT);
	}
}

/*****************************************************************************
 Prototype    : SRIO_Flow_Control
 Description  : SRIO flow control setup
 Input        : SRIO_Flow_Control_ID * flowControlID  
                Uint32 uiNumFlowControlID             
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void SRIO_Flow_Control(
    SRIO_Flow_Control_ID * flowControlID,
    Uint32 uiNumFlowControlID)
{
	int i;
	Uint32 uiFlowMask = 0;

	if(NULL != flowControlID)
	{
		uiNumFlowControlID= _min2(14, uiNumFlowControlID);
		for(i= 0; i< uiNumFlowControlID; i++)
			gpSRIO_regs->RIO_FLOW_CNTL[i]= 
				(flowControlID[i].tt<<CSL_SRIO_RIO_FLOW_CNTL_TT_SHIFT)
				|flowControlID[i].flow_control_ID;

		uiFlowMask= _set(uiFlowMask, 0, uiNumFlowControlID-1);
		uiFlowMask= _pack2(uiFlowMask, uiFlowMask);

		/*enable flow control in all TX modules*/
		for(i= 0; i< SRIO_MAX_LSU_NUM/2; i++)
			gpSRIO_regs->RIO_LSU_FLOW_MASKS[i]= uiFlowMask;

		for(i= 0; i< SRIO_PKTDMA_MAX_CH_NUM/2; i++)
			gpSRIO_regs->RIO_TX_CPPI_FLOW_MASKS[i]= uiFlowMask;
	}		
}

/*****************************************************************************
 Prototype    : SRIO_drv_Init
 Description  : SRIO initialization
 Input        : SRIO_Config * srio_cfg  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


  2.Date         : 2017/2/15
    Author       : 
    Modification : Modify the LSU setup and SERDES configuration
*****************************************************************************/
void SRIO_drv_Init(SRIO_Config * srio_cfg)
{
    Uint32 cfgValue = 0;
    Uint32 i = 0;
    
	srioSerdesRegs = (SerdesRegs *)&gpBootCfgRegs->SRIO_SERDES_CFGPLL;

    if ((CSL_PSC_getPowerDomainState(CSL_PSC_PD_SRIO) == PSC_PDSTATE_ON) &&
        (CSL_PSC_getModuleState (CSL_PSC_LPSC_SRIO) == PSC_MODSTATE_ENABLE))
	{
		if(gpSRIO_regs->RIO_PCR&CSL_SRIO_RIO_PCR_PEREN_MASK)
			SRIO_soft_reset();	//soft reset SRIO if it is already enabled
	}
	
	//enable SRIO power and clock domain
	debug_enable_PSC_module(CSL_PSC_PD_SRIO, CSL_PSC_LPSC_SRIO);

    /*Clear BOOT_COMPLETE bit*/
    gpSRIO_regs->RIO_PER_SET_CNTL &= (~(1 << CSL_SRIO_RIO_PER_SET_CNTL_BOOT_COMPLETE_SHIFT));

	/*enable globally used blocks including MMR block in SRIO*/
    SRIO_GlobalEnable();

    /*The LSU setup registers are only programmable
    while the LSU is disabled while the peripheral is enabled.*/
	if(srio_cfg->lsu_cfg)
	{
	    /*setup the shadow registers allocation between LSU*/
	    gpSRIO_regs->RIO_LSU_SETUP_REG0 = 
	    	(srio_cfg->lsu_cfg->lsuGrp0ShadowRegsSetup << CSL_SRIO_RIO_LSU_SETUP_REG0_SHADOW_GRP0_SHIFT)|
	        (srio_cfg->lsu_cfg->lsuGrp1ShadowRegsSetup << CSL_SRIO_RIO_LSU_SETUP_REG0_SHADOW_GRP1_SHIFT);

	    /*setup LSU interrupt based on LSU number or Source ID*/
	    cfgValue = 0;
	    for(i=0; i<SRIO_MAX_LSU_NUM; i++)
	    {
	        cfgValue |= srio_cfg->lsu_cfg->lsuIntSetup[i] << i;         
	    }
	    gpSRIO_regs->RIO_LSU_SETUP_REG1 = cfgValue;
	}

	/*enable other optional blocks*/
	SRIO_enable_blocks(&srio_cfg->blockEn);

	if(SRIO_SERDES_LOOPBACK==srio_cfg->loopback_mode)
	{
		if(srio_cfg->serdes_cfg->linkSetup[0])
			srio_cfg->serdes_cfg->linkSetup[0]->loopBack = SERDES_LOOPBACK_ENABLE;
		if(srio_cfg->serdes_cfg->linkSetup[1])
			srio_cfg->serdes_cfg->linkSetup[1]->loopBack = SERDES_LOOPBACK_ENABLE;
		if(srio_cfg->serdes_cfg->linkSetup[2])
			srio_cfg->serdes_cfg->linkSetup[2]->loopBack = SERDES_LOOPBACK_ENABLE;
		if(srio_cfg->serdes_cfg->linkSetup[3])
			srio_cfg->serdes_cfg->linkSetup[3]->loopBack = SERDES_LOOPBACK_ENABLE;
	}

	SRIO_Serdes_init(srio_cfg->serdes_cfg, srioSerdesRegs);
	Wait_SRIO_PLL_Lock();

	SRIO_set_1x2x4x_Path(srio_cfg->srio_1x2x4x_path_control);

	SRIO_set_device_ID(srio_cfg->device_ID_routing_config, 
		srio_cfg->uiNumDeviceId);

    SRIO_CSR_CAR_Config(srio_cfg);

	/*Allocates ingress Data Nodes and Tags based on priority.
	These registers must only be changed while boot_complete is 
	deasserted or while the port is held in reset.*/
	for(i=0;i<SRIO_MAX_PORT_NUM;i++)
	{
		if(FALSE == srio_cfg->blockEn.bBLK5_8_Port_Datapath_EN[i])
			continue;

		/*maximum data nodes and tags are 72 (0x48).
		 Each data node stores 32 bytes of data*/
		gpSRIO_regs->RIO_PBM[i].RIO_PBM_SP_IG_WATERMARK0 = 
			(36<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK0_PRIO0_WM_SHIFT)
			|(32<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK0_PRIO0CRF_WM_SHIFT);
		gpSRIO_regs->RIO_PBM[i].RIO_PBM_SP_IG_WATERMARK1 = 
			(28<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK1_PRIO1_WM_SHIFT)
			|(24<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK1_PRIO1CRF_WM_SHIFT);
		gpSRIO_regs->RIO_PBM[i].RIO_PBM_SP_IG_WATERMARK2 = 
			(20<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK2_PRIO2_WM_SHIFT)
			|(16<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK2_PRIO2CRF_WM_SHIFT);
		gpSRIO_regs->RIO_PBM[i].RIO_PBM_SP_IG_WATERMARK3 = 
			(12<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK3_PRIO3_WM_SHIFT)
			|(8<<CSL_SRIO_RIO_PBM_SP_IG_WATERMARK3_PRIO3CRF_WM_SHIFT);
	}
	
	/*setup timeout value in microsecond*/
	/*for 319 project debug:SRIO 根据需求修改*/
	SRIO_Timeout_Config(srio_cfg, 500000, 50, 100);
	//SRIO_Timeout_Config(srio_cfg, 500000, 50, 100);//例程配置

    SRIO_RxMode_Setup(srio_cfg->rxMode);

    SRIO_Flow_Control(srio_cfg->flowControlID,srio_cfg->uiNumFlowControlID);
    
	//SRIO_CSR_CAR_Config(srio_cfg);//例程程序在这个地方就已经注释掉
	
	SRIO_Prioirity_Permission_Setup(srio_cfg->priority_permission);

    SRIO_Interrupt_init(srio_cfg->interrupt_cfg);

	
    /*Set BOOT_COMPLETE bit*/
    gpSRIO_regs->RIO_PER_SET_CNTL |= (1 << CSL_SRIO_RIO_PER_SET_CNTL_BOOT_COMPLETE_SHIFT);

    /*This should be the last enable bit to toggle when bringing the 
    device out of reset to begin normal operation.*/
    gpSRIO_regs->RIO_PCR|= CSL_SRIO_RIO_PCR_PEREN_MASK;

	/*---------wait all enabled ports OK-------------*/
	for(i=0; i<SRIO_MAX_PORT_NUM; i++)
	{
		if(srio_cfg->blockEn.bLogic_Port_EN[i])
		{
			while(0==(gpSRIO_regs->RIO_SP[i].RIO_SP_ERR_STAT&
				CSL_SRIO_RIO_SP_ERR_STAT_PORT_OK_MASK));
		}
	}
}

