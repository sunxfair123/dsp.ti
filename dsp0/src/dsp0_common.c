/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : dsp0_common.c
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified : 
  Description   : KeyStone common miscellaneous functions and definitions
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tistdtypes.h>
#include <csl_bootcfgAux.h>
#include <csl_pscAux.h>
#include <cslr_chip.h>
#include <csl_edma3.h>
#include "CSL_msmc.h"
#include "CSL_msmcAux.h"
#include "dsp0_common.h"

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
CSL_XmcRegs * gpXMC_regs = (CSL_XmcRegs *) CSL_XMC_CONFIG_REGS;
CSL_CgemRegs * gpCGEM_regs = (CSL_CgemRegs *)CSL_CGEM0_5_REG_BASE_ADDRESS_REGS;
CSL_BootcfgRegs * gpBootCfgRegs = (CSL_BootcfgRegs *)CSL_BOOT_CFG_REGS;
CSL_PllcRegs * gpPLLC_regs = (CSL_PllcRegs * )CSL_PLL_CONTROLLER_REGS;
CSL_PscRegs *  gpPSC_regs =   (CSL_PscRegs *)CSL_PSC_REGS;
CSL_MsmcRegs * gpMSMC_regs = (CSL_MsmcRegs *)CSL_MSMC_CONFIG_REGS;
CSL_GpioRegs * gpGPIO_regs= (CSL_GpioRegs * )CSL_GPIO_REGS;

CSL_CPINTCRegs* gpCIC0_regs = (CSL_CPINTCRegs*)CSL_CP_INTC_0_REGS;
CSL_CPINTCRegs* gpCIC1_regs = (CSL_CPINTCRegs*)CSL_CP_INTC_1_REGS;
/*The register pointer for the CIC routing events DSP core.
By default, it is CIC0; but for core 4~7 of C6678, it is CIC1*/
CSL_CPINTCRegs* gpCIC_regs= (CSL_CPINTCRegs*)CSL_CP_INTC_0_REGS;

CSL_TpccRegs*  gpEDMA_CC0_regs  = (CSL_TpccRegs*)CSL_EDMA0CC_REGS;
CSL_TpccRegs*  gpEDMA_CC1_regs  = (CSL_TpccRegs*)CSL_EDMA1CC_REGS;
CSL_TpccRegs*  gpEDMA_CC2_regs  = (CSL_TpccRegs*)CSL_EDMA2CC_REGS;
CSL_TpccRegs*  gpEDMA_CC_regs[3]  = {
	(CSL_TpccRegs*)CSL_EDMA0CC_REGS,
	(CSL_TpccRegs*)CSL_EDMA1CC_REGS,
	(CSL_TpccRegs*)CSL_EDMA2CC_REGS
};

CSL_TptcRegs * gpEDMA_TC_0_0_regs=(CSL_TptcRegs *) CSL_EDMA0TC0_REGS;
CSL_TptcRegs * gpEDMA_TC_0_1_regs=(CSL_TptcRegs *) CSL_EDMA0TC1_REGS;
CSL_TptcRegs * gpEDMA_TC_1_0_regs=(CSL_TptcRegs *) CSL_EDMA1TC0_REGS;
CSL_TptcRegs * gpEDMA_TC_1_1_regs=(CSL_TptcRegs *) CSL_EDMA1TC1_REGS;
CSL_TptcRegs * gpEDMA_TC_1_2_regs=(CSL_TptcRegs *) CSL_EDMA1TC2_REGS;
CSL_TptcRegs * gpEDMA_TC_1_3_regs=(CSL_TptcRegs *) CSL_EDMA1TC3_REGS;
CSL_TptcRegs * gpEDMA_TC_2_0_regs=(CSL_TptcRegs *) CSL_EDMA2TC0_REGS;
CSL_TptcRegs * gpEDMA_TC_2_1_regs=(CSL_TptcRegs *) CSL_EDMA2TC1_REGS;
CSL_TptcRegs * gpEDMA_TC_2_2_regs=(CSL_TptcRegs *) CSL_EDMA2TC2_REGS;
CSL_TptcRegs * gpEDMA_TC_2_3_regs=(CSL_TptcRegs *) CSL_EDMA2TC3_REGS;

CSL_TptcRegs * gpEDMA_TC_regs[10]= {
	(CSL_TptcRegs *) CSL_EDMA0TC0_REGS,
	(CSL_TptcRegs *) CSL_EDMA0TC1_REGS,
	(CSL_TptcRegs *) CSL_EDMA1TC0_REGS,
	(CSL_TptcRegs *) CSL_EDMA1TC1_REGS,
	(CSL_TptcRegs *) CSL_EDMA1TC2_REGS,
	(CSL_TptcRegs *) CSL_EDMA1TC3_REGS,
	(CSL_TptcRegs *) CSL_EDMA2TC0_REGS,
	(CSL_TptcRegs *) CSL_EDMA2TC1_REGS,
	(CSL_TptcRegs *) CSL_EDMA2TC2_REGS,
	(CSL_TptcRegs *) CSL_EDMA2TC3_REGS
};

CSL_TmrRegs * gpTimer0Regs = (CSL_TmrRegs *)CSL_TIMER_0_REGS;
CSL_TmrRegs * gpTimer1Regs = (CSL_TmrRegs *)CSL_TIMER_1_REGS;
CSL_TmrRegs * gpTimer2Regs = (CSL_TmrRegs *)CSL_TIMER_2_REGS;
CSL_TmrRegs * gpTimer3Regs = (CSL_TmrRegs *)CSL_TIMER_3_REGS;
CSL_TmrRegs * gpTimer4Regs = (CSL_TmrRegs *)CSL_TIMER_4_REGS;
CSL_TmrRegs * gpTimer5Regs = (CSL_TmrRegs *)CSL_TIMER_5_REGS;
CSL_TmrRegs * gpTimer6Regs = (CSL_TmrRegs *)CSL_TIMER_6_REGS;
CSL_TmrRegs * gpTimer7Regs = (CSL_TmrRegs *)CSL_TIMER_7_REGS;
CSL_TmrRegs * gpTimer8Regs = (CSL_TmrRegs *)(CSL_TIMER_7_REGS+(CSL_TIMER_7_REGS-CSL_TIMER_6_REGS));
CSL_TmrRegs * gpTimerRegs[9] = {
	(CSL_TmrRegs *)CSL_TIMER_0_REGS,
	(CSL_TmrRegs *)CSL_TIMER_1_REGS,
	(CSL_TmrRegs *)CSL_TIMER_2_REGS,
	(CSL_TmrRegs *)CSL_TIMER_3_REGS,
	(CSL_TmrRegs *)CSL_TIMER_4_REGS,
	(CSL_TmrRegs *)CSL_TIMER_5_REGS,
	(CSL_TmrRegs *)CSL_TIMER_6_REGS,
	(CSL_TmrRegs *)CSL_TIMER_7_REGS,
	(CSL_TmrRegs *)(CSL_TIMER_7_REGS+(CSL_TIMER_7_REGS-CSL_TIMER_6_REGS))
};

/*MPU for peripherals registers and data space*/
CSL_MpuRegs * gpMPU0_regs= (CSL_MpuRegs *)CSL_MPU_0_REGS;
CSL_MpuRegs * gpMPU1_regs= (CSL_MpuRegs *)CSL_MPU_1_REGS;
CSL_MpuRegs * gpMPU2_regs= (CSL_MpuRegs *)CSL_MPU_2_REGS;
CSL_MpuRegs * gpMPU3_regs= (CSL_MpuRegs *)CSL_MPU_3_REGS;

CSL_Emif4fRegs * gpDDR_regs= (CSL_Emif4fRegs *)CSL_DDR3_EMIF_CONFIG_REGS;
unsigned int cycle_measure_overhead=50;
unsigned int gDSP_Core_Speed_Hz= 1000000000; 	//DSP core clock speed in Hz


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
 Prototype    : Core_PLL_init
 Description  : DSP core PLL configuration
  	DSP core will be configured to run at ref_clock_MHz*multiplier/divisor
 Input        : float ref_clock_MHz     
                unsigned int multiplier: 1~4096  
                unsigned int divisor: 1~64  
 Output       : 
 Return Value : 
 
  History        :
  1.Date         : 2017-02-13
    Author       : 
    Modification : Created function
*****************************************************************************/
void Core_PLL_init (float ref_clock_MHz,
	unsigned int multiplier, unsigned int divisor)
{
	unsigned int i;

	hBootCfg->KICK_REG0 = 0x83e70b13;
    hBootCfg->KICK_REG1 = 0x95a4f1e0;

	gDSP_Core_Speed_Hz= 1000000000;
	//printf("Initialize DSP main clock = %.2fMHz/%dx%d = %dMHz\n", 
		//ref_clock_MHz, divisor, multiplier, gDSP_Core_Speed_Hz/1000000);

	for(i=0; i< 20000; i++)
		asm(" nop 5");

	if(gpPLLC_regs->SECCTL&0x00800000)
	{			
		gpBootCfgRegs->CORE_PLL_CTL1 |= 0x00000040;

		gpPLLC_regs->PLLCTL &= 0xfffffffe;

		gpPLLC_regs->PLLCTL &= 0xffffffdf;

		for(i=0; i< 4; i++)
			asm(" nop 5");

		
		gpPLLC_regs->SECCTL |= 0x00800000;

		
		gpPLLC_regs->PLLCTL |= 0x00000002;	//Power down the PLL

		
		for(i=0; i< 1000; i++)
			asm(" nop 5");

		
		gpPLLC_regs->PLLCTL &= 0xfffffffd;         // Power up PLL
	}
	else
	{
		gpPLLC_regs->PLLCTL &= 0xfffffffe;

		gpPLLC_regs->PLLCTL &= 0xffffffdf;

		for(i=0; i< 40; i++)
			asm(" nop 5");
	}

	                                        
	gpBootCfgRegs->CORE_PLL_CTL0 = 0x09000000;
	
	gpPLLC_regs->PLLM= 0x00000013;
	
	gpBootCfgRegs->CORE_PLL_CTL1 &= 0xfffffff0; 
	gpBootCfgRegs->CORE_PLL_CTL1 |= 0;	/*BWADJ[11:8]*/
	
	gpPLLC_regs->SECCTL &= 0xff87ffff;
	
	gpPLLC_regs->SECCTL |= 0x00080000;
	
	gpPLLC_regs->PLLCTL |= 0x00000008;
	
	for(i=0; i< 2000; i++)
		asm(" nop 5");

	gpPLLC_regs->PLLCTL &= 0xfffffff7;

	for(i=0; i< 4000; i++)
		asm(" nop 5");
	
	gpPLLC_regs->SECCTL &= 0xff7fffff;
	
	gpPLLC_regs->PLLCTL |= 0x00000001;

}

/*****************************************************************************
 Prototype    : PLL_init
 Description  : Config the PLL of PA and DDR
 	target clock speed will be ref_clock_MHz/inputDivisor*multiplier/outputDivisor
 Input        : unsigned int inputDivisor  
                unsigned int multiplier  
                unsigned int outputDivisor  
 Output       : None
 Return Value : 0 for success, other value for error
 
  History        :
  1.Date         : May 18, 2013
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
int PLL_init (PLL_ControlRegs * PLL_Regs, unsigned int inputDivisor, 
	unsigned int multiplier, unsigned int outputDivisor)
{
	hBootCfg->KICK_REG0 = 0x83e70b13;
    hBootCfg->KICK_REG1 = 0x95a4f1e0;

	PLL_Regs->PLL_CTL1 = 0x00000040;

	PLL_Regs->PLL_CTL0 |= 0x00800000;

	PLL_Regs->PLL_CTL0 = ((multiplier/2-1)<<24)
		|((outputDivisor-1)<<19)
		|((multiplier-1)<<6)
		|(inputDivisor-1);

	PLL_Regs->PLL_CTL1 &= 0xfffffff0;
	PLL_Regs->PLL_CTL1 |= (multiplier/2-1)>>8; /*BWADJ[11:8]*/

	PLL_Regs->PLL_CTL1 |= 0x00004000;      //Set RESET bit = 1

	TSC_delay_us(7);

	if(PLL_Regs==(PLL_ControlRegs *)gpBootCfgRegs->PA_PLL_CTL0)
		PLL_Regs->PLL_CTL1 |= 0x00002000;

	PLL_Regs->PLL_CTL1 &= 0xffffbfff;   //Clear RESET bit

	TSC_delay_us(500);

	PLL_Regs->PLL_CTL0 &= 0xff7fffff ;

	return 0;
}
/*****************************************************************************
 Prototype    : KeyStone_PASS_PLL_init
 Description  : Config the PASS PLL
 	target clock speed will be ref_clock_MHz/divisor*multiplier
 Input        : float ref_clock_MHz
                unsigned int multiplier: 1~4096
                unsigned int divisor: 1~64
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2013-2-14
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void KeyStone_PASS_PLL_init (float ref_clock_MHz,
	unsigned int multiplier, unsigned int divisor)
{
	//output divisor for PASS PLL should be 2
	if(0==PLL_init((PLL_ControlRegs *)&gpBootCfgRegs->PA_PLL_CTL0, 
		divisor, multiplier*2, 2))
	{
		printf("Initialize PASS clock = %.2fMHzx%d/%d = %.3fMHz\n", 
			ref_clock_MHz, multiplier, divisor, 
			ref_clock_MHz*multiplier/divisor);
	}
}

/*****************************************************************************
 Prototype    : DDR_PLL_init
 Description  : Config the DDR PLL
 	target clock speed will be ref_clock_MHz/divisor*multiplier
 Input        : float ref_clock_MHz
                unsigned int multiplier: 1~4096
                unsigned int divisor: 1~64
 Output       : None
 Return Value : 
 
  History		:
  1.Date         : 2017-02-13
  Author         : 
  Modification	: Created function  

*****************************************************************************/
void DDR_PLL_init (float ref_clock_MHz,
	unsigned int multiplier, unsigned int divisor)
{
	//output divisor for DDR PLL should be 2
	if(0==PLL_init((PLL_ControlRegs *)&gpBootCfgRegs->DDR3_PLL_CTL0, 
		divisor, multiplier, 2))
	{
		printf("Initialize DDR speed = %.2fMHzx%d/%d = %.3fMTS\n", 
			ref_clock_MHz, multiplier, divisor, 
			ref_clock_MHz*multiplier/divisor);
	}
}

/*===============================TSC===================================*/


/*****************************************************************************
 Prototype    : TSC_init
 Description  : Initialize Time stamp counter to measure cycles
 Input        : None
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function   

*****************************************************************************/
void TSC_init()
{
	TSCL = 0; 	/* Enable the TSC */
}

/*****************************************************************************
 Prototype    : TSC_delay_ms
 Description  : Implement the delay function in millisecond
 Input        : Uint32 ms  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2010/12/12
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void TSC_delay_ms(Uint32 ms)
{
	volatile unsigned long long startTSC, currentTSC;
	unsigned long long delay_cycles;
	Uint32 tscl, tsch;

	tscl= TSCL;
	tsch= TSCH;
	startTSC= _itoll(tsch,tscl); 
	
	delay_cycles= ((unsigned long long)ms*gDSP_Core_Speed_Hz/1000);

	do
	{
		tscl= TSCL;
		tsch= TSCH;
		currentTSC= _itoll(tsch,tscl); 
	}
	while((currentTSC-startTSC)<delay_cycles);
}

/*****************************************************************************
 Prototype    : TSC_delay_us
 Description  : Implement the delay function in microsecond
 Input        : Uint32 us  
 Output       : None
 Return Value : 
*****************************************************************************/
void TSC_delay_us(Uint32 us)
{
	volatile unsigned long long startTSC, currentTSC;
	unsigned long long delay_cycles;
	Uint32 tscl, tsch;

	tscl= TSCL;
	tsch= TSCH;
	startTSC= _itoll(tsch,tscl); 
	
	delay_cycles= ((unsigned long long)us*gDSP_Core_Speed_Hz/1000000);

	do
	{
		tscl= TSCL;
		tsch= TSCH;
		currentTSC= _itoll(tsch,tscl); 
	}
	while((currentTSC-startTSC)<delay_cycles);
}

/*===============================Timer=================================*/
/*****************************************************************************
 Prototype    : Reset_Timer
 Description  : Reset the general timer value
 Input        : int timer_num  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2010/12/12
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/





/*===============================PSC===================================*/
/*****************************************************************************
 Prototype    : debug_enable_PSC_module
 Description  : Enable the PSC module in KeyStone device
 Input        : Uint32 pwrDmnNum  
                Uint32 moduleNum  
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
Int32 debug_enable_PSC_module (Uint32 pwrDmnNum, Uint32 moduleNum)
{
    /* Set Power domain to ON */
    CSL_PSC_enablePowerDomain (pwrDmnNum);

    /* Enable the clocks too*/
    CSL_PSC_setModuleNextState (moduleNum, PSC_MODSTATE_ENABLE);

    /* Start the state transition */
    CSL_PSC_startStateTransition (pwrDmnNum);

    /* Wait until the state transition process is completed. */
    while (!CSL_PSC_isStateTransitionDone (pwrDmnNum));

    /* Return PSC status */
    if ((CSL_PSC_getPowerDomainState(pwrDmnNum) == PSC_PDSTATE_ON) &&
        (CSL_PSC_getModuleState (moduleNum) == PSC_MODSTATE_ENABLE))
    {
        /*Ready for use */
        return 0;
    }
    else
    {
        /*Return error */
        return -1;
    }
}
/*****************************************************************************
 Prototype    : debug_disable_PSC_module
 Description  : Disable the PSC module in KeyStone device
 Input        : Uint32 pwrDmnNum  
                Uint32 moduleNum  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
Int32 debug_disable_PSC_module (Uint32 pwrDmnNum, Uint32 moduleNum)
{
    /* disable the clocks*/
    CSL_PSC_setModuleNextState (moduleNum, PSC_MODSTATE_SWRSTDISABLE);

    /* Start the state transition */
    CSL_PSC_startStateTransition (pwrDmnNum);

    /* Wait until the state transition process is completed. */
    while (!CSL_PSC_isStateTransitionDone (pwrDmnNum));

    /* Return PSC status */
    if (CSL_PSC_getModuleState (moduleNum) == PSC_MODSTATE_SWRSTDISABLE)
    {
        /*Ready for use */
        return 0;
    }
    else
    {
        /*Return error */
        return -1;
    }

}
/*****************************************************************************
 Prototype    : debug_disable_PSC_Power_Domain
 Description  : Disable the PSC power domain
 Input        : Uint32 pwrDmnNum  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
Int32 debug_disable_PSC_Power_Domain (Uint32 pwrDmnNum)
{
    /* Set Power domain to OFF */
    CSL_PSC_disablePowerDomain (pwrDmnNum);

    /* Start the state transition */
    CSL_PSC_startStateTransition (pwrDmnNum);

    /* Wait until the state transition process is completed. */
    while (!CSL_PSC_isStateTransitionDone (pwrDmnNum));

    /* Return PSC status */
    if (CSL_PSC_getPowerDomainState(pwrDmnNum) == PSC_PDSTATE_OFF)
    {
        /*Ready for use */
        return 0;
    }
    else
    {
        /*Return error */
        return -1;
    }

}
/*****************************************************************************
 Prototype    : EDMA_init
 Description  : Initialize all EDMA registers and clear the event
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void EDMA_init ()
{
	int i;
	unsigned int * uipPaRAM;

	/*clear PaRAM*/
	uipPaRAM= (unsigned int *)&(gpEDMA_CC0_regs->PARAMSET[0]);
	for(i=0; i<8*CSL_EDMA3_TPCC0_NUM_PARAMSETS; i++) 	
		*uipPaRAM++=0;

	uipPaRAM= (unsigned int *)&(gpEDMA_CC1_regs->PARAMSET[0]);
	for(i=0; i<8*CSL_EDMA3_TPCC1_NUM_PARAMSETS; i++) 	
		*uipPaRAM++=0;

	uipPaRAM= (unsigned int *)&(gpEDMA_CC2_regs->PARAMSET[0]);
	for(i=0; i<8*CSL_EDMA3_TPCC2_NUM_PARAMSETS; i++) 	
		*uipPaRAM++=0;
		
	/*Assign PaRAM for different channels*/
	for(i=0; i<CSL_EDMA3_TPCC0_NUM_DMACH; i++) 	
		gpEDMA_CC0_regs->TPCC_DCHMAP[i] = i<< CSL_TPCC_TPCC_DCHMAP0_PAENTRY_SHIFT;

	for(i=0; i<CSL_EDMA3_TPCC1_NUM_DMACH; i++) 	
		gpEDMA_CC1_regs->TPCC_DCHMAP[i] = i<< CSL_TPCC_TPCC_DCHMAP0_PAENTRY_SHIFT;

	for(i=0; i<CSL_EDMA3_TPCC2_NUM_DMACH; i++) 	
		gpEDMA_CC2_regs->TPCC_DCHMAP[i] = i<< CSL_TPCC_TPCC_DCHMAP0_PAENTRY_SHIFT;

	/*Assign TC/Queue for different channels*/
	gpEDMA_CC0_regs->TPCC_DMAQNUM[0]= 0x10101010;
	gpEDMA_CC0_regs->TPCC_DMAQNUM[1]= 0x10101010;

	gpEDMA_CC1_regs->TPCC_DMAQNUM[0]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[1]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[2]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[3]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[4]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[5]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[6]= 0x32103210;
	gpEDMA_CC1_regs->TPCC_DMAQNUM[7]= 0x32103210;

	gpEDMA_CC2_regs->TPCC_DMAQNUM[0]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[1]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[2]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[3]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[4]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[5]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[6]= 0x32103210;
	gpEDMA_CC2_regs->TPCC_DMAQNUM[7]= 0x32103210;

	/*clear any events and status*/
	gpEDMA_CC0_regs->TPCC_ECR= 0xFFFF;
	gpEDMA_CC0_regs->TPCC_EECR= 0xFFFF;
	gpEDMA_CC0_regs->TPCC_ICR= 0xFFFFF;
	gpEDMA_CC0_regs->TPCC_IECR= 0xFFFF;
	gpEDMA_CC0_regs->TPCC_EMCR= 0xFFFF;
	gpEDMA_CC0_regs->TPCC_QEMCR= 0xFFFF;
    gpEDMA_CC0_regs->TPCC_CCERRCLR= 0xFFFF;
	gpEDMA_CC0_regs->TPCC_SECR= 0xFFFF;

	gpEDMA_CC1_regs->TPCC_ECR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_ECRH= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_EECR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_EECRH= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_ICR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_ICRH= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_IECR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_IECRH= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_EMCR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_EMCRH= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_QEMCR= 0xFFFFFFFF;
    gpEDMA_CC1_regs->TPCC_CCERRCLR= 0xFFFF;
	gpEDMA_CC1_regs->TPCC_SECR= 0xFFFFFFFF;
	gpEDMA_CC1_regs->TPCC_SECRH= 0xFFFFFFFF;

	gpEDMA_CC2_regs->TPCC_ECR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_ECRH= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_EECR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_EECRH= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_ICR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_ICRH= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_IECR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_IECRH= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_EMCR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_EMCRH= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_QEMCR= 0xFFFFFFFF;
    gpEDMA_CC2_regs->TPCC_CCERRCLR= 0xFFFF;
	gpEDMA_CC2_regs->TPCC_SECR= 0xFFFFFFFF;
	gpEDMA_CC2_regs->TPCC_SECRH= 0xFFFFFFFF;

	for(i=0; i<NUM_EDMA_TC; i++)
	{
		//Clear TC error
		gpEDMA_TC_regs[i]->TPTC_ERRCLR= 0xF;

		//enable error interrupt
		gpEDMA_TC_regs[i]->TPTC_ERREN= 
			(1<<CSL_TPTC_TPTC_ERREN_MMRAERR_SHIFT)
			|(1<<CSL_TPTC_TPTC_ERREN_TRERR_SHIFT)
			|(1<<CSL_TPTC_TPTC_ERREN_BUSERR_SHIFT);
	}
}



/*****************************************************************************
 Prototype    : EDMA_copy_Type3
 Description  : EDMA copy function with manual trigger
 Input        : unsigned int srcAddr     
                unsigned int dstAddr     
                unsigned int byteCount   
                EDMA_CC_Channel_Num CC_channel 
                DMA_transfer_cfg
                DMA_Wait wait
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/01/08
    Author       : sxf
    Modification : Created function

传输640*512的数据
byteCount:ACNT 每个像元1个字节
BCNT:640
*****************************************************************************/
void EDMA_copy_Type3(unsigned int uiSrcAddr, unsigned int uiDstAddr, 
	EDMA_CC_Channel_Num CC_channel, DMA_MEM_Tran_Config * DMA_transfer_cfg, DMA_Wait wait)
{
	Uint32 A_B_CNT;
	Uint32 CCNT;
	Uint32 SRC_DST_BIDX;
	Uint32 SRC_DST_CIDX;
	

	CSL_TpccRegs*  EDMACCRegs;
	Uint32 uiChannel;
    volatile Uint32 * TPCC_ESR;
    //volatile Uint32 * TPCC_IPR;
    //volatile Uint32 * TPCC_ICR;

	EDMACCRegs= gpEDMA_CC_regs[CC_channel>>16];
	uiChannel = (Uint32)(CC_channel&0xFF);

	A_B_CNT = DMA_transfer_cfg->A_B_CNT;
	CCNT = (Uint32)DMA_transfer_cfg->CCNT;
	SRC_DST_BIDX = DMA_transfer_cfg->SRC_DST_BIDX;
	SRC_DST_CIDX = DMA_transfer_cfg->SRC_DST_CIDX;

	EDMACCRegs->PARAMSET[uiChannel].OPT= 
		CSL_EDMA3_OPT_MAKE(CSL_EDMA3_ITCCH_EN, 
			CSL_EDMA3_TCCH_DIS, 
			CSL_EDMA3_ITCINT_DIS, 
			CSL_EDMA3_TCINT_DIS,
			uiChannel,
			CSL_EDMA3_TCC_NORMAL,
			CSL_EDMA3_FIFOWIDTH_NONE, 
			CSL_EDMA3_STATIC_DIS, 
			CSL_EDMA3_SYNC_AB, 
			CSL_EDMA3_ADDRMODE_INCR, 
			CSL_EDMA3_ADDRMODE_INCR);
	EDMACCRegs->PARAMSET[uiChannel].SRC= GLOBAL_ADDR(uiSrcAddr);
	EDMACCRegs->PARAMSET[uiChannel].A_B_CNT= A_B_CNT;//每个像元4个字节byteCount=4
	EDMACCRegs->PARAMSET[uiChannel].DST= GLOBAL_ADDR(uiDstAddr);
	EDMACCRegs->PARAMSET[uiChannel].SRC_DST_BIDX= SRC_DST_BIDX;//每个像元4个字节
	EDMACCRegs->PARAMSET[uiChannel].LINK_BCNTRLD= 0xFFFF;
	EDMACCRegs->PARAMSET[uiChannel].SRC_DST_CIDX= SRC_DST_CIDX;//每个像元4个字节
	EDMACCRegs->PARAMSET[uiChannel].CCNT= CCNT;
	
	
	TPCC_ESR= &EDMACCRegs->TPCC_ESR;
	//TPCC_IPR= &EDMACCRegs->TPCC_IPR;
	//TPCC_ICR= &EDMACCRegs->TPCC_ICR;
	
	/*Manually trigger the EDMA*/
	(*TPCC_ESR)= 1<<(uiChannel);
}

/*****************************************************************************
 Prototype    : IDMA_copy
 Description  : implement the data copy via IDMA
 Input        : unsigned int srcAddr    
                unsigned int dstAddr    
                unsigned int byteCount  
                IDMA_Wait wait          
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/
void IDMA_copy(unsigned int srcAddr, unsigned int dstAddr, 
	unsigned int byteCount, DMA_Wait wait)
{
	gpCGEM_regs->IDMA1_SOURCE= srcAddr;
	gpCGEM_regs->IDMA1_DEST= dstAddr;
	gpCGEM_regs->IDMA1_COUNT= byteCount&0xFFFC;

	if(wait)
	{
		/* Wait until copying is completed */
		while (gpCGEM_regs->IDMA1_STAT); 
	}
}


/*======================memory protection==============================*/
/*code and data related to CorePac MPAX configuration should be allocated in LL2*/
#pragma DATA_SECTION(mem_prot_key,".far:Core_MPAX")
unsigned int mem_prot_key[4] = {0xBFBFBFBF, 0xFE29, 3, 4};
/*****************************************************************************
 Prototype    : lock_mem_prot_regs
 Description  : lock memory protection registers
 Input        : None
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
*****************************************************************************/
#pragma CODE_SECTION (lock_mem_prot_regs, ".text:Core_MPAX")
void lock_mem_prot_regs()
{
	int i;
	
	/*1. Write a 1 to the KEYR field of the MPLKCMD register. This resets some internal
	status for the MPLK0 through MPLK3 registers.*/
	gpCGEM_regs->MPLKCMD = (1<<CSL_CGEM_MPLKCMD_KEYR_SHIFT);
	
	/*2. Write the key to MPLK0 through MPLK3. All four registers must be written
	exactly once. They may be written in any order.*/
	for(i=0; i<4; i++)
	{
		gpCGEM_regs->MPLK[i] = mem_prot_key[i];
	}

	/*3. Write a 1 to the LOCK field of the MPLKCMD register. This engages the lock.*/
	gpCGEM_regs->MPLKCMD = (1<<CSL_CGEM_MPLKCMD_LOCK_SHIFT);

	/*wait to make sure it is locked*/
	while(0==(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK));
}

/*****************************************************************************
 Prototype    : unlock_mem_prot_regs
 Description  : unlock memory protection registers
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
#pragma CODE_SECTION (unlock_mem_prot_regs, ".text:Core_MPAX")
void unlock_mem_prot_regs()
{
	int i;
	
	/*1. Write a 1 to the KEYR field in the MPLKCMD register. This resets some internal
	status for the MPLK0 through the MPLK3 registers.*/
	gpCGEM_regs->MPLKCMD = (1<<CSL_CGEM_MPLKCMD_KEYR_SHIFT);

	/*2. Write the unlock key to MPLK0 through the MPLK3 registers. The hardware
	compares the written value with the stored key value. Software must write to all
	four registers exactly once. The writes can arrive in any order.*/
	for(i=0; i<4; i++)
	{
		gpCGEM_regs->MPLK[i] = mem_prot_key[i];
	}

	/*3. Write a 1 to the UNLOCK field in the MPLKCMD register. If the key written in
	step 2 matches the stored key, the hardware disengages the lock. If the key written
	in step 2 does not match, the hardware signals an exception. The hardware
	reports the fault address as the address of the MPLKCMD register.*/
	gpCGEM_regs->MPLKCMD = (1<<CSL_CGEM_MPLKCMD_UNLOCK_SHIFT);

	/*wait to make sure it is unlocked*/
	while(1==(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK));
}

/*****************************************************************************
 Prototype    : L1_MPPA_setup
 Description  : setup the memory protection for local L1 memory
 Input        : volatile Uint32 MPPA_regs[], Uint32 MPPA[]  
 Output       : None
 Return Value : 
usage example:
 Uint32 MPPA[16]=
 {
	MP_LOCAL|MP_SR|MP_UR, 			//page 0 for read only
	MP_LOCAL|MP_SR|MP_SW|MP_UR|MP_UW, 	//page 1 for read/write
	......
 }
 ......
	L1_MPPA_setup(gpCGEM_regs->L1PMPPA, MPPA);

  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/

void L1_MPPA_setup(volatile Uint32 MPPA_regs[], Uint32 MPPA[])
{
	int i;

	if(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK)
		unlock_mem_prot_regs();
		
	/*clear any memory protection fault*/
	gpCGEM_regs->L1PMPFCR = 1;
	gpCGEM_regs->L1DMPFCR = 1;

	/*setup the memory protection attributes for 32 pages*/
	for(i=0; i<16; i++)
	{
		MPPA_regs[i] = MPPA[i];
	}

	lock_mem_prot_regs();
}

/*****************************************************************************
 Prototype    : L1P_memory_protection_cfg
 Description  : config "AccessPermisionMask" for L1P
 Input        : Uint32 AccessPermisionMask  
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
    
*****************************************************************************/
void L1P_memory_protection_cfg(Uint32 AccessPermisionMask)
{
	int i;

	if(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK)
		unlock_mem_prot_regs();

	/*clear any memory protection fault*/
	gpCGEM_regs->L1PMPFCR = 1;

	/*set L1P only for cacheable local execution, disable all external access*/
	for(i=0; i<16; i++)
	{
		gpCGEM_regs->L1PMPPA[i] = AccessPermisionMask;
	}

	lock_mem_prot_regs();
}
/*****************************************************************************
 Prototype    : L1D_memory_protection_cfg
 Description  : config "AccessPermisionMask" for L1P
 Input        : Uint32 AccessPermisionMask  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/10/5
    Author       : Zhan
    Modification : Created function

  2.Date         : 2012/10/29
    Author       : Brighton Feng
    Modification : Add accesspermision mask for input

  3.Date         :2016/01/08
    Author       : 
    Modification : Reserved

*****************************************************************************/	
void L1D_memory_protection_cfg(Uint32 AccessPermisionMask)
{
	int i;

	if(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK)
		unlock_mem_prot_regs();

	/*clear any memory protection fault*/
	gpCGEM_regs->L1DMPFCR = 1;

	/*set L1P only for cacheable local execution, disable all external access*/
	for(i=0; i<16; i++)
	{
		gpCGEM_regs->L1DMPPA[i] = AccessPermisionMask;
	}

	lock_mem_prot_regs();
}
/*****************************************************************************
 Prototype    : L1_cache_protection
 Description  : protect L1 as cache
 Input        : None
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/10/29
    Author       : Brighton Feng
    Modification :  Created function

  2.Date         : 2016/01/08
    Author       : Brighton Feng
    Modification : Reserved 

*****************************************************************************/
void L1_cache_protection()
{
	/*set L1P only for cacheable local execution, disable all external access*/
	L1P_memory_protection_cfg(0);
	
	/*set L1D only for cacheable local read/write, disable all external access*/
	L1D_memory_protection_cfg(0);
}
/*****************************************************************************
 Prototype    : LL2_MPPA_setup
 Description  : setup the memory protection for local L2 memory
 Input        : Uint32 MPPA[]  
 Output       : None
 Return Value : 
usage example:
 Uint32 MPPA[32]=
 {
	MP_LOCAL|MP_SX|MP_UX, 			//page 0 for local code only
	MP_LOCAL|MP_SR|MP_SW|MP_UR|MP_UW, 	//page 1 for local data only
	......
 }
 ......
	LL2_MPPA_setup(MPPA);

  History        :
  1.Date         : 2012/10/5
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
void LL2_MPPA_setup(Uint32 MPPA[])
{
	int i;

	if(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK)
		unlock_mem_prot_regs();
		
	/*clear any memory protection fault*/
	gpCGEM_regs->L2MPFCR = 1;

	/*setup the memory protection attributes for 32 pages*/
	for(i=0; i<32; i++)
	{
		gpCGEM_regs->L2MPPA[i] = MPPA[i];
	}

	lock_mem_prot_regs();
}


/*****************************************************************************
 Prototype    : MPAX_seg_setup
 Description  : configure one MPAX segment
 Input        : MPAX_Regs * MPAX_regs       
                Uint32 BADDR                
                Uint32 RADDR                
                Uint32 SegementSize         
                Uint32 AccessPermisionMask  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
#pragma CODE_SECTION (MPAX_seg_setup, ".text:Core_MPAX")
void MPAX_seg_setup(MPAX_Regs * MPAX_regs, Uint32 BADDR, 
	Uint32 RADDR, Uint32 SegementSize, Uint32 AccessPermisionMask)
{
	Uint32 SegSizeMask, SegSizeBits;

	if(SegementSize<4*1024)
	{
		printf("Segment size %d is less than 4KB. ", SegementSize);
		SegementSize = 4*1024;
		printf("Extended segment size to 4KB.\n");
	}
	
	SegSizeBits= 31 - _lmbd(1, SegementSize);
	SegSizeMask= (1<<SegSizeBits)-1;
	if(SegementSize&SegSizeMask)
	{
		printf("Segment size 0x%x is not power of 2. ", SegementSize);
		SegSizeBits += 1;
		SegementSize= (1<<SegSizeBits);
		SegSizeMask= (1<<SegSizeBits)-1;
		printf("Extended segment size to 0x%x.\n", SegementSize);
	}

	if(BADDR&SegSizeMask)
	{
		printf("Segment base address 0x%x does not start from power-of-2 boundary corresponds to segment size 0x%x.\n", 
			BADDR, SegementSize);
		BADDR &= (~SegSizeMask);
		printf("Extended base address to 0x%x.\n", BADDR);
	}
	if(RADDR&(SegSizeMask>>4))
	{
		printf("Segment replacement address 0x%x does not start from power-of-2 boundary corresponds to segment size 0x%x.\n", 
			RADDR, SegementSize);
		RADDR &= (~(SegSizeMask>>4));
		printf("Extended replacement address to 0x%x.\n", RADDR);
	}
	
	MPAX_regs->MPAXH= BADDR|(SegSizeBits-1); 
	MPAX_regs->MPAXL= RADDR|AccessPermisionMask; 

}

/*****************************************************************************
 Prototype    : XMC_MPAX_setup
 Description  : configure multiple MPAX segments in XMC with a configuration
                table.
	Please note, the code of this function or the data used by this function
	(stack, parameters...) can not be put in the segment that will be modified
	by this function. Normally, to put this function and its related data in 
	LL2 is a good idea.
 Input        : MPAX_Config MPAX_cfg[]  
                Uint32 firstSeg         
                Uint32 numSegs          
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
#pragma CODE_SECTION (XMC_MPAX_setup, ".text:Core_MPAX")
void XMC_MPAX_setup(MPAX_Config MPAX_cfg[], 
	Uint32 firstSeg, Uint32 numSegs)
{
	int i;

    if(firstSeg >= 16)
    {
        printf("The XMC MPAX segment number %d >= 16, not supported.\n ", firstSeg);
        return;
    }

	if(gpCGEM_regs->MPLKSTAT&CSL_CGEM_MPLKSTAT_LK_MASK)
		unlock_mem_prot_regs();

	/*CAUTION:
	A MPAX segment can only be modified when there is no access to the space 
	of this segment. Any data in cache or prefetch buffer from the space of this 
	segment must be writeback and invalidated before the segment modification.	
	It is recommended to configure the MPAX at the very beginning of application
	software before any shared memory is used. Code and data for CorePac MPAX 
	configuration should be allocated in LL2.
	If a MPAX segment must be modified on-the-fly, the safer way is, to write 
	the new configuration to a unused higher segment, and then clear the old 
	segment. This is based on the fact that higher numbered segments take 
	precedence over lower numbered segments.	*/
	CACHE_wbInvAllL2(CACHE_WAIT);
	CSL_XMC_invalidatePrefetchBuffer();
	_mfence();
	_mfence(); 	//ensure all writeback invalidate have completed.

	for(i=numSegs-1; i>=0 ; i--)
    {   
		MPAX_seg_setup((MPAX_Regs *)&gpXMC_regs->XMPAX[firstSeg+i], 
			MPAX_cfg[i].BADDR, MPAX_cfg[i].RADDR, 
			MPAX_cfg[i].SegementSize, MPAX_cfg[i].AccessPermisionMask);
    }

	lock_mem_prot_regs();
}


/*==========Peripherals registers and internal data buffer MPU=============*/
/* delete for peripheral_MP_test测试*/


/*======================memory EDC=====================================*/
/*****************************************************************************
 Prototype    : KeyStone_SL2_EDC_enable
 Description  : Enable MSMC EDC
 Input        : scrubCnt, number of MSMC clock cycles between scrubbing
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void KeyStone_SL2_EDC_enable(Uint32 scrubCnt)
{
	if(gpMSMC_regs->CFGLCKSTAT&CSL_MSMC_CFGLCKSTAT_WSTAT_MASK)
		CSL_MSMC_unlockNonMPAX();

	/*Software must wait for the PRR (Parity RAM Ready) bit before making 
	the first read access to MSMC RAM after reset.*/
	while(0==(gpMSMC_regs->SMEDCC&CSL_MSMC_SMEDCC_PRR_MASK));

	/* set scrubbing period value */
	if(scrubCnt>255)
		scrubCnt= 255;
	CSL_MSMC_setCounterBankRefreshRead(scrubCnt); //the scrubbing engine works every scrubCnt*1024 cycle*/

	/* clear EDC errors and enable EDC event*/
	gpMSMC_regs->SMIRC = 0xf;
	gpMSMC_regs->SMIESTAT |= 0x0000000f;

	//enable SL2 EDC
	CSL_MSMC_setECM(1);

	CSL_MSMC_lockNonMPAX();
}



/*****************************************************************************
 Prototype    : LL2_EDC_setup
 Description  : Eenable LL2 EDC and scrub whole LL2
 Input        : None
 Output       : None
 Return Value : 
 
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
void LL2_EDC_setup()
{
	int i;
	unsigned int uiByteCnt= 512*1024;
	//TDSP_Board_Type DSP_Board_Type;
	
	/* 1. Disable the EDC */
	CSL_CGEM_disableL2EDC();

	/* 2. Clear any EDC errors */
	CSL_CGEM_clearL2EDCErrorStatus(1, 1, 1, 1);

	/* 3. Memory Scrubbing with IDMA, generate the parity bits*/
	#if 0
	DSP_Board_Type = KeyStone_Get_dsp_board_type();
	if((DUAL_NYQUIST_EVM == DSP_Board_Type)
		||(C6670_EVM == DSP_Board_Type)
		||(TCI6614_EVM == DSP_Board_Type))
	{
		uiByteCnt= 1024*1024;
	}
	#endif

	/*Each IDMA can transfer up to 65532 bytes,
	here we transfer 32KB each time*/
	for(i=0; i< (uiByteCnt>>15); i++)
	{
		IDMA_copy((0x00800000 + i*(1<<15)), (0x00800000 + i*(1<<15)), 
			(1<<15), DMA_WAIT);
	}

	/* 4. Enable the EDC*/
	CSL_CGEM_enableL2EDC();
	gpCGEM_regs->L2EDCEN= 0x0000001f;
	
}




/*****************************************************************************
 Prototype    : L1P_EDC_setup
 Description  : enable L1P ED and scrub whole L1P
 Input        : None
 Output       : None
 Return Value : 
 
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
*****************************************************************************/
void L1P_EDC_setup()
{
	Uint32 preL1PMPPA[16];

	/* 1. Disable the EDC */
	CSL_CGEM_disablePMCErrorDetection();

	/* 2. Clear any EDC errors */
	CSL_CGEM_clearPMCErrorDetectionStatus(1, 1);

	/* 3. Memory Scrubbing with IDMA, generate the parity bits*/
	memcpy(preL1PMPPA, (void *)gpCGEM_regs->L1PMPPA, 64);//save protection attributes
	L1P_memory_protection_cfg(0xFFFF); //enable IDMA access to L1P
	IDMA_copy(0x00E00000, 0x00E00000, 32*1024, DMA_WAIT);
	L1_MPPA_setup(gpCGEM_regs->L1PMPPA, preL1PMPPA);//restore protection for L1

	/* 4. Enable the EDC*/
	CSL_CGEM_enablePMCErrorDetection();
}




Exception_Record exception_record;

/*some exception events routed from CIC to INTC and then to exception moduel,
all these events are routed to one input of INTC*/
Uint32 guwIntcCICExcEvtIdx= 23; /*INTC event number mapping to CIC exception events*/
Uint32 gCIC_EXC_out_num = 65; 	//CIC output event number
Uint32 gCIC_EXC_EN_MASK[8]= {0,0,0,0,0,0,0,0};
/*****************************************************************************
 Prototype    : KeyStone_CIC_exception_events_mapping
 Description  : CIC exception event mapping to the NMI external exception
 Input        : void  
 Output       : None
 Return Value : 
 
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
*****************************************************************************/

void KeyStone_CIC_exception_events_mapping(void)
{	
	if(DNUM<4)
	{
		gCIC_EXC_out_num= 34+11*DNUM;
	}
	else
	{
		gCIC_EXC_out_num= 34+11*(DNUM-4);
		gpCIC_regs= gpCIC1_regs;
	}

	/*-----------------------CP_INTC configuration---------------------*/
	/* Disable Global host interrupts. */
	gpCIC_regs->GLOBAL_ENABLE_HINT_REG= 0;

	/* route following system events to CIC out (event 22 of INTC), and then to exception
	TCI6614, C6670, C6678:
	CIC: 0 EDMA3CC1 EDMACC_ERRINT EDMA3CC1 error interrupt
	CIC: 2 EDMA3CC1 EDMATC_ERRINT0 EDMA3CC1 EDMA3TC0 error interrupt
	CIC: 3 EDMA3CC1 EDMATC_ERRINT1 EDMA3CC1 EDMA3TC1 error interrupt
	CIC: 4 EDMA3CC1 EDMATC_ERRINT2 EDMA3CC1 EDMA3TC2 error interrupt
	CIC: 5 EDMA3CC1 EDMATC_ERRINT3 EDMA3CC1 EDMA3TC3 error interrupt
	CIC: 16 EDMA3CC2 EDMACC_ERRINT EDMA3CC2 error interrupt
	CIC: 18 EDMA3CC2 EDMATC_ERRINT0 EDMA3CC2 EDMA3TC0 error interrupt
	CIC: 19 EDMA3CC2 EDMATC_ERRINT1 EDMA3CC2 EDMA3TC1 error interrupt
	CIC: 20 EDMA3CC2 EDMATC_ERRINT2 EDMA3CC2 EDMA3TC2 error interrupt
	CIC: 21 EDMA3CC2 EDMATC_ERRINT3 EDMA3CC2 EDMA3TC3 error interrupt
	CIC: 32 EDMA3CC0 EDMACC_ERRINT EDMA3CC0 error interrupt
	CIC: 34 EDMA3CC0 EDMATC_ERRINT0 EDMA3CC0 EDMA3TC0 error interrupt
	CIC: 35 EDMA3CC0 EDMATC_ERRINT1 EDMA3CC0 EDMA3TC1 error interrupt
	CIC: 99 MSMC_dedc_nc_error Non-correctable soft error detected on SRAM read
	CIC: 100 MSMC_scrub_nc_error Non-correctable soft error detected during scrub cycle
	CIC: 102 MSMC_mpf_error8 Memory protection fault indicators for each system master PrivID
	CIC: 103 MSMC_mpf_error9 Memory protection fault indicators for each system master PrivID
	CIC: 104 MSMC_mpf_error10 Memory protection fault indicators for each system master PrivID
	CIC: 105 MSMC_mpf_error11 Memory protection fault indicators for each system master PrivID
	CIC: 106 MSMC_mpf_error12 Memory protection fault indicators for each system master PrivID
	CIC: 107 MSMC_mpf_error13 Memory protection fault indicators for each system master PrivID
	CIC: 108 MSMC_mpf_error14 Memory protection fault indicators for each system master PrivID
	CIC: 109 MSMC_mpf_error15 Memory protection fault indicators for each system master PrivID
	CIC: 110 DDR3_ERR DDR3 ECC Error Interrupt
	TCI6614, C6670:
	CIC: 170 MSMC_mpf_error4 Memory protection fault indicators for each system master PrivID
	CIC: 171 MSMC_mpf_error5 Memory protection fault indicators for each system master PrivID
	CIC: 172 MSMC_mpf_error6 Memory protection fault indicators for each system master PrivID
	CIC: 173 MSMC_mpf_error7 Memory protection fault indicators for each system master PrivID
	*/
	KeyStone_CIC_event_map(gpCIC_regs, 0 , gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 2 , gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 3 , gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 4 , gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 5 , gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 16, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 18, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 19, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 20, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 21, gCIC_EXC_out_num);
	gCIC_EXC_EN_MASK[0] |=  
                 (1<<0 )
                |(1<<2 )
                |(1<<3 )
                |(1<<4 )
                |(1<<5 )
                |(1<<16)
                |(1<<18)
                |(1<<19)
                |(1<<20)
                |(1<<21);
		
	KeyStone_CIC_event_map(gpCIC_regs, 32, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 34, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 35, gCIC_EXC_out_num);
	gCIC_EXC_EN_MASK[1] |=  
                 (1<<(32-32))
                |(1<<(34-32))
                |(1<<(35-32));

	KeyStone_CIC_event_map(gpCIC_regs, 99, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 100, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 102, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 103, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 104, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 105, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 106, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 107, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 108, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 109, gCIC_EXC_out_num);
	KeyStone_CIC_event_map(gpCIC_regs, 110, gCIC_EXC_out_num);
	gCIC_EXC_EN_MASK[3] |=  
                 (1<<(99 -32*3))
                |(1<<(100-32*3))
                |(1<<(102-32*3))
                |(1<<(103-32*3))
                |(1<<(104-32*3))
                |(1<<(105-32*3))
                |(1<<(106-32*3))
                |(1<<(107-32*3))
                |(1<<(108-32*3))
                |(1<<(109-32*3))
                |(1<<(110-32*3));
    /* ----------- Mapping MPU interrupt for different chips start -----------------------*/
    //if(C6670_EVM==DSP_Board_Type
		//||DUAL_NYQUIST_EVM==DSP_Board_Type
		//||C6678_EVM==DSP_Board_Type)
    {
    /*  route the following system events to CIC out
        CIC: 90 MPU0 addressing violation interrupt and protection violation interrupt 
    	CIC: 92 MPU1 addressing violation interrupt and protection violation interrupt
    	CIC: 94 MPU2 addressing violation interrupt and protection violation interrupt
    	CIC: 96 MPU3 addressing violation interrupt and protection violation interrupt     	
    */
        KeyStone_CIC_event_map(gpCIC_regs, 90, gCIC_EXC_out_num);
		KeyStone_CIC_event_map(gpCIC_regs, 92, gCIC_EXC_out_num);
		KeyStone_CIC_event_map(gpCIC_regs, 94, gCIC_EXC_out_num);
		KeyStone_CIC_event_map(gpCIC_regs, 96, gCIC_EXC_out_num);
		gCIC_EXC_EN_MASK[2] |=  
	                  (1<<(90 -32*2))
	                 |(1<<(92 -32*2))
	                 |(1<<(94 -32*2));
		gCIC_EXC_EN_MASK[3] |=  
	                 (1<<(96 -32*3));
    }
    /* ----------- Mapping MPU interrupt for different chips end -----------------------*/

	/* Enable Global host interrupts. */
	gpCIC_regs->GLOBAL_ENABLE_HINT_REG= 1;

}

/*****************************************************************************
 Prototype    : KeyStone_Exception_cfg
 Description  : Config the system  err and external exception to generate the NMI.
                Call this function as last step after all 
                configuration/initialization complete
 Input        : None
 Output       : None
 Return Value : 
 
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
*****************************************************************************/
void KeyStone_Exception_cfg()
{

	puts("Enable Exception handling...");

	TSCL = 0; 	/* Enable the TSC */

	memset(&exception_record, 0, sizeof(exception_record));

	/*clear all exception events*/
	gpCGEM_regs->EVTCLR[0] = 0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[1] = 0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[2] = 0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[3] = 0xFFFFFFFF;

	/*clear memory falut*/
	gpCGEM_regs->L1PMPFCR = 1;
	gpCGEM_regs->L1DMPFCR = 1;
	gpCGEM_regs->L2MPFCR = 1;
	gpXMC_regs->XMPFCR = 1;

	/*clear configuration bus error*/
	gpCGEM_regs->ICFGMPFCR= 1;
	gpCGEM_regs->ECFGERRCLR= 1;

	/* clear the MDMAERR */
	gpCGEM_regs->MDMAERRCLR = 1;

	/*clear interrupt drop staus*/
	gpCGEM_regs->INTXCLR = 1;

	gpCGEM_regs->INTDMASK= ~IER; /*only monitor drop of enabled interrupts*/

	/*enable events:
	10 MSMC_mpf_error_n MSMC Memory protection fault  caused by local CorePac
	96 INTERR Dropped CPU interrupt event
	97 EMC_IDMAERR Invalid IDMA parameters
	110 MDMAERREVT XMC VBUSM error event
	113 PMC_ED Single bit error detected during DMA accesses L1P
	117 UMC_ED2 Uncorrected bit error detected LL2
	119 SYS_CMPA CPU Memory Protection Fault for local configuration of INTC and power control
	120 PMC_CMPA CPU memory protection fault for L1P
	121 PMC_DMPA DMA memory protection fault for L1P
	122 DMC_CMPA CPU memory protection fault for L1D (and other memory read finally goes through the L1D controller)
	123 DMC_DMPA DMA memory protection fault for L1D
	124 UMC_CMPA CPU memory protection fault for L2
	125 UMC_DMPA DMA memory protection fault for L2
	126 EMC_CMPA CPU memory protection fault for other local configuration space between 0x01000000 - 0x01BFFFFF 
	127 EMC_BUSERR Bus Error Interrupt for global configuration space between 0x01C00000 - 0x07FFFFFF 
	*/
	gpCGEM_regs->EXPMASK[0]= ~(1<<CSL_GEM_MSMC_MPF_ERRORN);
	gpCGEM_regs->EXPMASK[1]= 0xFFFFFFFF;
	gpCGEM_regs->EXPMASK[2]= 0xFFFFFFFF;
	gpCGEM_regs->EXPMASK[3]= ~((1<<(CSL_GEM_INTERR - 96))|
		(1<<(CSL_GEM_EMC_IDMAERR-96))|
		(1<<(CSL_GEM_MDMAERREVT-96))|
		(1<<(CSL_GEM_PMC_ED-96))|
		(1<<(CSL_GEM_UMC_ED2-96))|
		(1<<(CSL_GEM_SYS_CMPA-96))|
		(1<<(CSL_GEM_PMC_CMPA-96))|
		(1<<(CSL_GEM_PMC_DMPA-96))|
		(1<<(CSL_GEM_DMC_CMPA-96))|
		(1<<(CSL_GEM_DMC_DMPA-96))|
		(1<<(CSL_GEM_UMC_CMPA-96))|
		(1<<(CSL_GEM_UMC_DMPA-96))|
		(1<<(CSL_GEM_EMC_CMPA-96))|
		(1<<(CSL_GEM_EMC_BUSERR-96)));

	//if(0==DNUM)
	{
		/*enable exception events routed from CIC,
		please note, this should be only configured for one DSP core*/
		KeyStone_CIC_exception_events_mapping();
		gpCGEM_regs->EXPMASK[guwIntcCICExcEvtIdx>>5] &= (~(1<<(guwIntcCICExcEvtIdx&0x1F)));        
	}
	
	/*clear exception flag*/
	ECR = EFR;
	IERR = 0; 
    
	/*Interrupt Service Table Pointer to begining of LL2 memory*/
	//ISTP= 0x800000;


	/*Eanble external exception, global exception enable*/
	TSR = TSR|
		(1<<CSL_CHIP_TSR_XEN_SHIFT)|
		(1<<CSL_CHIP_TSR_GEE_SHIFT);

	/*enable NMI exception*/
	IER = IER|(1<<CSL_CHIP_IER_NMI_SHIFT);
	
}


#pragma NMI_INTERRUPT( Exception_service_routine );
void Exception_service_routine(void)
{
	unsigned int efr, nrp, ntsr, tscl, tsch, irp, itsr;

	/*record timestamp*/
	tscl = TSCL;
	exception_record.TSCL= tscl;
	tsch = TSCH;
	exception_record.TSCH= tsch;
	
	nrp = NRP;   /*record NRP */
	exception_record.NRP= nrp;
	ntsr = NTSR; /*record NTSR */
	exception_record.NTSR= ntsr;
	efr = EFR;   /*record EFR */
	exception_record.EFR= efr;

	irp = IRP;   /*record IRP */
	itsr = ITSR; /*record ITSR */
	exception_record.IRP= irp;
	exception_record.ITSR= itsr;

	if(efr)
	{
		/*Clear excpeiton flag*/
		ECR = efr;
#if 0
		/*check all possible exceptions*/
		if (efr & CSL_CHIP_EFR_OXF_MASK) {
			/* S/W generated exception */
			EXC_swgen();		
		}
		if (efr & CSL_CHIP_EFR_IXF_MASK) {
			/* internal exception */
			EXC_internal();		
		}
		if (efr & CSL_CHIP_EFR_EXF_MASK) {
			/* external exception */
			KeyStone_EXC_external();		
		}
		if (efr & CSL_CHIP_EFR_NXF_MASK) {
			/* legacy NMI exception */
			EXC_NMI();		
		}
#endif		
		printf("EFR= 0x%x\n",efr);
	}
	else
		puts("Enter Exception service routine, but excpetion flage is 0!");
		
	printf("NRP=0x%x, NTSR=0x%x, IRP=0x%x, ITSR=0x%x, TSCH= 0x%x, TSCL= 0x%x\n", 
		nrp, ntsr, irp, itsr, tsch, tscl);

	printf(" B3=0x%x, A4=0x%x, B4= 0x%x, B14= 0x%x, B15= 0x%x\n", 
		exception_record.B3, exception_record.A4,
		exception_record.B4, exception_record.B14,
		exception_record.B15);

	/*
	It is not always possible to safely exit the exception handling routine. Conditions that
	can prevent a safe return from exceptions include:
	1. SPLOOPs that are terminated by an exception cannot be resumed correctly. The
	SPLX bit in NTSR should be verified to be 0 before returning.
	2. Exceptions that occur when interrupts are blocked cannot be resumed correctly.
	The IB bit in NTSR should be verified to be 0 before returning.
	3. Exceptions that occur at any point in the code that cannot be interrupted safely
	(for example, a tight loop containing multiple assignments) cannot be safely
	returned to. The compiler will normally disable interrupts at these points in the
	program; check the GIE bit in NTSR to be 1 to verify that this condition is met.
	4. NRP is not in valid address space*/
	if((ntsr&CSL_CHIP_TSR_SPLX_MASK)
		||(ntsr&CSL_CHIP_TSR_IB_MASK)
		||(0==(ntsr&CSL_CHIP_TSR_GIE_MASK))
		||(0x800000>NRP))
	{
		puts("Exception happened at a place can not safely return!");
#ifdef STM_DEBUG
        ETB_dump();
#endif
		exit(0);
		//while(1);		//trap
	}

}


