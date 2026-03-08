/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : Serdes_init.c
  Version       : v1.0
  Author        : 
  Created       : 2017-02-13
  Last Modified :
  Description   :  example for Serdes configuration on DSP

 
  History       :
  1.Date        : 2017-02-13
    Author      : 
    Modification: Created file

  2.Date         : 2017-02-14
    Author       : 
    Modification : Add the SGMII SERDES configuration

******************************************************************************/
#include "Serdes_init.h"
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
/*calculate some PLL parameters according to other input parameters*/
void Serdes_PLL_config(
	SerdesSetup_4links * serdes_cfg)
{
	int i;

	if(NULL== serdes_cfg)
		return;
		
	/*find lowest link speed*/
	serdes_cfg->commonSetup.serdesPllClock_GHz= 12.5;
	for(i= 0; i<4; i++)
	{
		if(serdes_cfg->linkSetup[i])
			if(serdes_cfg->commonSetup.serdesPllClock_GHz > 
				serdes_cfg->linkSetup[i]->linkSpeed_GHz)
				serdes_cfg->commonSetup.serdesPllClock_GHz = 
					serdes_cfg->linkSetup[i]->linkSpeed_GHz;
	}

	/*Serdes PLL output must be between 1.5625 and 3.125*/
	if(serdes_cfg->commonSetup.serdesPllClock_GHz>6.25)
		serdes_cfg->commonSetup.serdesPllClock_GHz /= 4;
	else if(serdes_cfg->commonSetup.serdesPllClock_GHz>3.125)
		serdes_cfg->commonSetup.serdesPllClock_GHz /= 2;
	else if(serdes_cfg->commonSetup.serdesPllClock_GHz<1.5625)
		serdes_cfg->commonSetup.serdesPllClock_GHz *= 2;

	/*set VCO range according to the PLL output speed*/
	if(serdes_cfg->commonSetup.serdesPllClock_GHz<2.17)
		serdes_cfg->commonSetup.vcoRange= SERDES_PLL_VCO_RANGE_HIGH;
	else
		serdes_cfg->commonSetup.vcoRange= SERDES_PLL_VCO_RANGE_LOW;

	/*calculate PLL MPY factor according to input reference clock speed*/
	serdes_cfg->commonSetup.pllMpy= serdes_cfg->commonSetup.serdesPllClock_GHz*1000/
		serdes_cfg->commonSetup.inputRefClock_MHz;

	//debug 20170331
	//serdes_cfg->commonSetup.pllMpy= 10;
	
	/*PLL multiply factors between 4 and 60*/
	if(serdes_cfg->commonSetup.pllMpy<4 || serdes_cfg->commonSetup.pllMpy>60)
		serdes_cfg->commonSetup.pllMpy= 4;

}

/*****************************************************************************
 Prototype    : SRIO_Serdes_init
 Description  : Configure the SRIO SERDES
 Input        : SerdesSetup_4links * serdes_cfg  
                SerdesRegs * serdesRegs          
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

  2.Date         : 2017-02-14
    Author       : 
    Modification : Add the status check
*****************************************************************************/
void SRIO_Serdes_init(
	SerdesSetup_4links * serdes_cfg, SerdesRegs * serdesRegs)
{
	int i;
	Uint32 CFGTX;
	float rateScale;
	SerdesLinkRate linkRateScale; 

	if(NULL== serdes_cfg)
		return;

	/*calculate some PLL parameters according to other input parameters*/
	Serdes_PLL_config(serdes_cfg);
	
	CSL_BootCfgUnlockKicker();

	serdesRegs->CFGPLL = (1<<SERDES_PLL_CFG_ENPLL_SHIFT)|
		(serdes_cfg->commonSetup.loopBandwidth<<SERDES_PLL_CFG_LOOPBANDWIDTH_SHIFT)|
		(serdes_cfg->commonSetup.vcoRange<<SERDES_PLL_CFG_VRANGE_SHIFT)|
		((Uint32)(serdes_cfg->commonSetup.pllMpy*4)<<SERDES_PLL_CFG_MPY_SHIFT);

	for(i=0; i<4; i++)
	{
		if(serdes_cfg->linkSetup[i])
		{
			rateScale=serdes_cfg->linkSetup[i]->linkSpeed_GHz/
				serdes_cfg->commonSetup.serdesPllClock_GHz;
			if(rateScale<1)
				linkRateScale = SRIO_SERDES_LINK_RATE_div2;
			else if(rateScale<2)
				linkRateScale = SRIO_SERDES_LINK_RATE_x1;
			else if(rateScale<4)
				linkRateScale = SRIO_SERDES_LINK_RATE_x2;
			else
				linkRateScale = SRIO_SERDES_LINK_RATE_x4;

			//debug 20170331
			//linkRateScale = SRIO_SERDES_LINK_RATE_x2;
				
			serdesRegs->link[i].CFGRX= (1<<SERDES_RX_CFG_ENRX_SHIFT)|
				(2<<SERDES_RX_CFG_BUSWIDTH_SHIFT)| 	/*Bus width must be 010b (20 bit)*/
				(serdes_cfg->linkSetup[i]->testPattern<<SERDES_RX_CFG_TESTPATTERN_SHIFT)|
				(serdes_cfg->linkSetup[i]->loopBack<<SERDES_RX_CFG_LOOPBACK_SHIFT)|
				(1<<SERDES_RX_CFG_ENOC_SHIFT)| 	/*Enable offset compensation*/
				(serdes_cfg->linkSetup[i]->rxEqualizerConfig<<SERDES_RX_CFG_EQ_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxCDR<<SERDES_RX_CFG_CDR_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxLos<<SERDES_RX_CFG_LOS_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxAlign<<SERDES_RX_CFG_ALIGN_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxTermination<<SERDES_RX_CFG_TERM_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxInvertPolarity<<SERDES_RX_CFG_INVPAIR_SHIFT)|
				(linkRateScale<<SERDES_RX_CFG_RATE_SHIFT);

			CFGTX= (1<<SERDES_TX_CFG_ENTX_SHIFT)|
				(2<<SERDES_TX_CFG_BUSWIDTH_SHIFT)| 	/*Bus width must be 010b (20 bit)*/
				(serdes_cfg->linkSetup[i]->testPattern<<SERDES_TX_CFG_TESTPATTERN_SHIFT)|
				(serdes_cfg->linkSetup[i]->loopBack<<SERDES_TX_CFG_LOOPBACK_SHIFT)|
				(1<<SERDES_TX_CFG_FIRUPT_SHIFT)| 	/*FIRUPT = 1 Transmitter pre and post cursor FIR filter update*/
				/*TWPST1: Adjacent Post Cursor Tap Weight. 
				If trace length is 4”, start with 20 (-10%). 
				If trace length is between 4” and 10”, start with 27 (-27.5%).*/
				(18<<SERDES_TX_CFG_TWPST1_SHIFT)| 	
				(1<<SERDES_TX_CFG_TWPRE_SHIFT)| 	/*TWPRE: The settings range from 0 to -17.5% in 2.5% steps.*/
				(serdes_cfg->linkSetup[i]->txOutputSwing<<SERDES_TX_CFG_SWING_SHIFT)|
				(serdes_cfg->linkSetup[i]->txInvertPolarity<<SERDES_TX_CFG_INVPAIR_SHIFT)|
				(linkRateScale<<SERDES_TX_CFG_RATE_SHIFT);

			if(i==0) //lane 0 is Synchronization master
				serdesRegs->link[i].CFGTX= CFGTX|
					(1<<SERDES_TX_CFG_MYSNC_SHIFT);
			else
				serdesRegs->link[i].CFGTX= CFGTX|
					(0<<SERDES_TX_CFG_MYSNC_SHIFT);
		}
	}

}
/*****************************************************************************
 Prototype    : KeyStone_HyperLink_Serdes_init
 Description  : Hyperlink SERDES configuration
 Input        : SerdesSetup_4links * serdes_cfg  
                SerdesRegs * serdesRegs          
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
和SRIO_Serdes_init差别不大，以后可以和函数SRIO_Serdes_init合并
*****************************************************************************/
void KeyStone_HyperLink_Serdes_init(
	SerdesSetup_4links * serdes_cfg, SerdesRegs * serdesRegs)
{
	int i;
	Uint32 CFGTX;
	float rateScale;
	SerdesLinkRate linkRateScale; 

	if(NULL== serdes_cfg)
		return;
		
	/*calculate some PLL parameters according to other input parameters*/
	Serdes_PLL_config(serdes_cfg);
	
	CSL_BootCfgUnlockKicker();
	for(i=0; i<4; i++)
	{
		if(serdes_cfg->linkSetup[i])
		{
			rateScale=serdes_cfg->linkSetup[i]->linkSpeed_GHz/
				serdes_cfg->commonSetup.serdesPllClock_GHz;
			if(rateScale<1)
				linkRateScale = HYPERLINK_SERDES_LINK_RATE_div2;
			else if(rateScale<2)
				linkRateScale = HYPERLINK_SERDES_LINK_RATE_x1;
			else if(rateScale<4)
				linkRateScale = HYPERLINK_SERDES_LINK_RATE_x2;
			else
				linkRateScale = HYPERLINK_SERDES_LINK_RATE_x4;

			/*Serdes Loopback is controled by Hyperlink control
			register, Serdes loopback register has no effect*/
			/*HyperLink automatically sets LOS to 0 for loopback. 
			User must set LOS to 4 for non-loopack*/
			serdes_cfg->linkSetup[i]->rxLos = (SerdesRxLos)4;

			serdesRegs->link[i].CFGRX= (1<<SERDES_RX_CFG_ENRX_SHIFT)|
				(2<<SERDES_RX_CFG_BUSWIDTH_SHIFT)| 	/*Bus width must be 010b (20 bit)*/
				(serdes_cfg->linkSetup[i]->testPattern<<SERDES_RX_CFG_TESTPATTERN_SHIFT)|
				(1<<SERDES_RX_CFG_ENOC_SHIFT)| 	/*Enable offset compensation*/
				(serdes_cfg->linkSetup[i]->rxEqualizerConfig<<SERDES_RX_CFG_EQ_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxCDR<<SERDES_RX_CFG_CDR_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxLos<<SERDES_RX_CFG_LOS_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxAlign<<SERDES_RX_CFG_ALIGN_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxTermination<<SERDES_RX_CFG_TERM_SHIFT)|
				(serdes_cfg->linkSetup[i]->rxInvertPolarity<<SERDES_RX_CFG_INVPAIR_SHIFT)|
				(linkRateScale<<SERDES_RX_CFG_RATE_SHIFT);

			CFGTX= (1<<SERDES_TX_CFG_ENTX_SHIFT)|
				(2<<SERDES_TX_CFG_BUSWIDTH_SHIFT)| 	/*Bus width must be 010b (20 bit)*/
				(serdes_cfg->linkSetup[i]->testPattern<<SERDES_TX_CFG_TESTPATTERN_SHIFT)|
				(serdes_cfg->linkSetup[i]->loopBack<<SERDES_TX_CFG_LOOPBACK_SHIFT)|
				(1<<SERDES_TX_CFG_FIRUPT_SHIFT)| 	/*FIRUPT = 1 Transmitter pre and post cursor FIR filter update*/
				/*TWPST1: Adjacent Post Cursor Tap Weight. 
				If trace length is 4”, start with 20 (-10%). 
				If trace length is between 4” and 10”, start with 27 (-27.5%).*/
				(18<<SERDES_TX_CFG_TWPST1_SHIFT)| 	
				(1<<SERDES_TX_CFG_TWPRE_SHIFT)| 	/*TWPRE: The settings range from 0 to -17.5% in 2.5% steps.*/
				(serdes_cfg->linkSetup[i]->txOutputSwing<<SERDES_TX_CFG_SWING_SHIFT)|
				(serdes_cfg->linkSetup[i]->txInvertPolarity<<SERDES_TX_CFG_INVPAIR_SHIFT)|
				(linkRateScale<<SERDES_TX_CFG_RATE_SHIFT);
			
			if(i==0) //lane 0 is Synchronization master
				serdesRegs->link[i].CFGTX= CFGTX|
					(1<<SERDES_TX_CFG_MYSNC_SHIFT);
			else
				serdesRegs->link[i].CFGTX= CFGTX|
					(0<<SERDES_TX_CFG_MYSNC_SHIFT);
		}
	}
	TSC_delay_ms(50);

	/*write to CFGPLL will start the Hyperlink training sequence, so, 
	this should be the last register been write for HyperLink initialization*/
	serdesRegs->CFGPLL = (1<<SERDES_PLL_CFG_ENPLL_SHIFT)|
		(serdes_cfg->commonSetup.loopBandwidth<<SERDES_PLL_CFG_LOOPBANDWIDTH_SHIFT)|
		(serdes_cfg->commonSetup.vcoRange<<SERDES_PLL_CFG_VRANGE_SHIFT)|
		((Uint32)(serdes_cfg->commonSetup.pllMpy*4)<<SERDES_PLL_CFG_MPY_SHIFT);

}
/*****************************************************************************
 Prototype    : Serdes_disable
 Description  : SERDES disable for KeyStone device
 Input        : SerdesRegs * serdesRegs  
                Uint32 uiLinkNum         
 Output       : None
 Return Value : 
 
   History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
void Serdes_disable(
	SerdesRegs * serdesRegs, Uint32 uiLinkNum)
{
	int i;
	
	CSL_BootCfgUnlockKicker();

	//disable PLL
	serdesRegs->CFGPLL = 0;

	//disable TX/RX links
	for(i= 0; i< uiLinkNum; i++)
	{
		serdesRegs->link[i].CFGRX= 0;
		serdesRegs->link[i].CFGTX= 0;
	}
}


