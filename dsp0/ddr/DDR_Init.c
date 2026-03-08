/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : DDR_Init.c
  Version       : v1.0
  Author        : 
  Created       : 20170213
  Last Modified :
  Description   : DDR configuration
  
  History	   :
  1.Date		 : 20170213
  Author		 :  
  Modification: Modify configuration according to update document
    	"sprabl2a_KeyStone DDR3 Initialization.pdf"
******************************************************************************/
#include <stdio.h>
#include <csl_bootcfgaux.h>
#include <cslr_xmc.h>
#include "DDR_Init.h"
#include "dsp0_common.h"



#pragma DATA_SECTION(DDR_REGS_MPAX_cfg_table_319,".far:Core_MPAX")
MPAX_Config DDR_REGS_MPAX_cfg_table_319[]=
{
    /*BADDR         RADDR           SegementSize      AccessPermisionMask
    32-bit virtual  36-bit physical in byte, must     Access types allowed
    base address    address right   be power of 2     in this address range
                    shift by 4                                            */
    {0x21000000,    0x100000000>>4,  4*1024,         MP_SR|MP_SW|MP_UR|MP_UW}/*map DDR2 configuration registers at physical address 0x1:00000000 to logical address 0x21000000*/
};

//check the if the DDR3 registers are mapped. If not, map it
void DDR3_registers_adress_map()
{
	int i=0;
	for(i=0; i<16; i++)
	{
		if(0x10000000==(gpXMC_regs->XMPAX[i].XMPAXL&CSL_XMC_XMPAXL_RADDR_MASK))
		{//DDR3 registers at 0x1:00000000 is mapped in the XMPAX
			break;
		}
	}
	if(i==16) //not mapped
	{
		XMC_MPAX_setup(DDR_REGS_MPAX_cfg_table_319, 15,
			sizeof(DDR_REGS_MPAX_cfg_table_319)/sizeof(MPAX_Config));
	}		
}

/*****************************************************************************
 Prototype    : DDR_Config_Init
 Description  : configure DDR according to the clock speed
                please note, clock_MHz is the clock speed in MHz, not data
                rate. 
                For example, clock speed for 1333.333M data rate is 666.667-
                MHz
 Input        : float clock_MHz  
 Output       : None
 Return Value : 
 
History        :
  1.Date         : 2017-02-13
    Author       : 
    Modification : Created function    

*****************************************************************************/
void DDR_Config_Init(float clock_MHz, DDR_ECC_Config * ecc_cfg)
{
	Uint32 uwStatus;
	Uint32 uiRev= gpBootCfgRegs->DEVICE_ID_REG0&0xF0000000;
	
	hBootCfg->KICK_REG0 = 0x83e70b13;
    hBootCfg->KICK_REG1 = 0x95a4f1e0;

	/*Invert Clock Out*/
	// clear ctrl_slave_ratio field
	gpBootCfgRegs->DDR3_CONFIG_REG[0] &= ~(0x007FE000);  	
	// set ctrl_slave_ratio to 0x100
	gpBootCfgRegs->DDR3_CONFIG_REG[0] |= (0x100<<13);
	// Set invert_clkout = 1
	gpBootCfgRegs->DDR3_CONFIG_REG[12] |= 0x08000000;
	// set dll_lock_diff to 15
	gpBootCfgRegs->DDR3_CONFIG_REG[0] |= 15; 	

	if(0==uiRev)
	{//Rev 1.0 does not support read eye leveling
		//Set bit 9 = 1 to use forced ratio leveling for read DQS
		//gel配置DDR3_CONFIG_REG[23]:0A619A34
		gpBootCfgRegs->DDR3_CONFIG_REG[23] |= 0x00000200; 
	}
	
	//initial vale for leveling
	/*WRLVL_INIT_RATIO*/
	//使用gel配置
	gpBootCfgRegs->DDR3_CONFIG_REG[2] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[3] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[4] = 0x5E;
	gpBootCfgRegs->DDR3_CONFIG_REG[5] = 0x51;
	gpBootCfgRegs->DDR3_CONFIG_REG[6] = 0x38;
	gpBootCfgRegs->DDR3_CONFIG_REG[7] = 0x3A;
	gpBootCfgRegs->DDR3_CONFIG_REG[8] = 0x24;
	gpBootCfgRegs->DDR3_CONFIG_REG[9] = 0x20;
	gpBootCfgRegs->DDR3_CONFIG_REG[10] = 0x44;

	/*GTLVL_INIT_RATIO*/
	gpBootCfgRegs->DDR3_CONFIG_REG[14] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[15] = 0xDD;
	gpBootCfgRegs->DDR3_CONFIG_REG[16] = 0xBE;
	gpBootCfgRegs->DDR3_CONFIG_REG[17] = 0xCA;
	gpBootCfgRegs->DDR3_CONFIG_REG[18] = 0xA9;
	gpBootCfgRegs->DDR3_CONFIG_REG[19] = 0xA7;
	gpBootCfgRegs->DDR3_CONFIG_REG[20] = 0x9E;
	gpBootCfgRegs->DDR3_CONFIG_REG[21] = 0xA1;
	gpBootCfgRegs->DDR3_CONFIG_REG[22] = 0xBA;

	/*the PHY_RESET is pulsed (0 -> 1 -> 0) to latch 
	leveling configuration values into the PHY logic.*/
	gpDDR_regs->DDR_PHY_CTRL_1 &= ~(0x00008000);
	gpDDR_regs->DDR_PHY_CTRL_1 |= (0x00008000);
	gpDDR_regs->DDR_PHY_CTRL_1 &= ~(0x00008000);

	
	gpDDR_regs->SDRAM_REF_CTRL = 0x00005162; // enable configuration	
	gpDDR_regs->SDRAM_TIM_1 = 0x1113783C;
	gpDDR_regs->SDRAM_TIM_2   = 0x30717FE3;
	gpDDR_regs->SDRAM_TIM_3   = 0x559F86AF;
	//DDR_DDRPHYC  
	gpDDR_regs->DDR_PHY_CTRL_1  = 0x0010010F;
	gpDDR_regs->ZQ_CONFIG  = 0x70073214; 
	gpDDR_regs->PWR_MGMT_CTRL = 0;
	gpDDR_regs->SDRAM_CONFIG = 0x63062ab2;	
	TSC_delay_us(600); 	//Wait 600us for HW init to complete
	gpDDR_regs->SDRAM_REF_CTRL	  = 0x1450;
	
	/*enable full leveling*/
	gpDDR_regs->RDWR_LVL_RMP_CTRL = (unsigned int)(1<<31);
	/*start full leveling*/
	gpDDR_regs->RDWR_LVL_CTRL = (unsigned int)(1<<31);//触发后RDWR_LVL_CTRL为0
	/*Read back any of the DDR3 controller registers.
	This ensures full leveling is complete because this step is executed 
	only after full	leveling completes.*/
	uwStatus= gpDDR_regs->RDWR_LVL_RMP_CTRL; 	//dummy read
	//Wait 3ms for leveling to complete
	TSC_delay_ms(3); 	
	if(gpDDR_regs->STATUS&(0x00000070))
	{
		printf("DDR3 full leveling has failed, STATUS = 0x%x\n", gpDDR_regs->STATUS); 
	}
    
	
}

/*****************************************************************************
 Prototype    : DDR_init
 Description  : Initialize the DDR3
 Input        : float ref_clock_MHz    
                unsigned int DDR_PLLM  
                unsigned int DDR_PLLD  
 Output       : None
 Return Value : 
 
History        :
  1.Date         : 2017-02-13
    Author       : 
    Modification : Created function    

*****************************************************************************/
void DDR_init(float ref_clock_MHz, unsigned int DDR_PLLM, 
	unsigned int DDR_PLLD, DDR_ECC_Config * ecc_cfg)
{	
	float DDR_Speed_MHz, DDR_Clock_MHz;

	//check the if the DDR3 configuration registers are mapped. If not, map it
	DDR3_registers_adress_map();	

	DDR_Speed_MHz= ref_clock_MHz*DDR_PLLM/DDR_PLLD;
	DDR_Clock_MHz= DDR_Speed_MHz/2; 	//data speed is double of clock speed
	DDR_PLL_init(ref_clock_MHz, DDR_PLLM, DDR_PLLD);		
	DDR_Config_Init(DDR_Clock_MHz, ecc_cfg);
	
	printf("DDR initialization process!\n");	
}
