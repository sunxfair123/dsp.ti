/****************************************************************************\
 *           Copyright (C) 2011, 2012 Texas Instruments Incorporated.             *
 *                           All Rights Reserved                            *
 *                                                                          *
 * GENERAL DISCLAIMER                                                       *
 * -------------------------------------------------------------------      *
 * All software and related documentation is provided "AS IS" and without   *
 * warranty or support of any kind and Texas Instruments expressly disclaims*
 * all other warranties, express or implied, including, but not limited to, *
 * the implied warranties of merchantability and fitness for a particular   *
 * purpose.  Under no circumstances shall Texas Instruments be liable for   *
 * any incidental, special or consequential damages that result from the    *
 * use or inability to use the software or related documentation, even if   *
 * Texas Instruments has been advised of the liability.                     *
 ****************************************************************************
Example to show memory access and interrupt throughput HyperLink
In this example, DSP1's memory are mapped to DSP0 through HyperLink 
so, DSP0 accesses DSP 1 through the HyperLink memory window just like 
access other memory space. Internal loopback is also supported in this 
example, for this case, DSP0 actually access its own local memory through 
HyperLink memory window.
 ****************************************************************************
 History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

 ***************************************************************************/
#include <C6x.h>
#include <stdio.h>
#include <string.h>
#include "Serdes_init.h"
#include "KeyStone_HyperLink_init.h"
#include "tistdtypes.h"

/*HyperLink test speed at GHz*/
#define HYPERLINK_SPEED_GHZ 	6.25f
#define HYPERLINK_CLOCK_MHZ 	156.25f
/*the base address of the DDR can be accessed through Hyperlink*/
#define DDR_SPACE_ACCESSED_BY_HYPERLINK 	0x88000000
#define StartAddress_BY_HYPERLINK           0x48000000

#pragma DATA_SECTION(serdeshyperLinkSetup,".far:HyperLinkInit")
#pragma DATA_SECTION(hyperLink_cfg,".far:HyperLinkInit")
SerdesLinkSetup serdeshyperLinkSetup;
HyperLink_Config hyperLink_cfg;

CSL_VusrRegs * gpHyperLinkRegs = (CSL_VusrRegs *)CSL_MCM_CONFIG_REGS;
SerdesRegs * hyperLinkSerdesRegs;

/*****************************************************************************
 Prototype    : KeyStone_HyperLink_Interrupt_init
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void KeyStone_HyperLink_Interrupt_init(
	HyperLink_Interrupt_Cfg * int_cfg)
{
	int i;
	
	gpHyperLinkRegs->CTL = gpHyperLinkRegs->CTL
		|(int_cfg->int_local<<CSL_VUSR_CTL_INTLOCAL_SHIFT)
		|(int_cfg->sts_int_enable<<CSL_VUSR_CTL_INTENABLE_SHIFT)
		|(int_cfg->sts_int_vec<<CSL_VUSR_CTL_INTVEC_SHIFT)
		|(int_cfg->int2cfg<<CSL_VUSR_CTL_INT2CFG_SHIFT);

	for(i=0; i<64; i++)
	{
		gpHyperLinkRegs->INT_CTL_IDX = i;
		gpHyperLinkRegs->INT_CTL_VAL= 
			(int_cfg->int_event_cntl[i].Int_en<<CSL_VUSR_INT_CTL_VAL_INTEN_SHIFT)
			|(int_cfg->int_event_cntl[i].Int_type<<CSL_VUSR_INT_CTL_VAL_INTTYPE_SHIFT)
			|(int_cfg->int_event_cntl[i].Int_pol<<CSL_VUSR_INT_CTL_VAL_INTPOL_SHIFT)
			|(int_cfg->int_event_cntl[i].si_en<<CSL_VUSR_INT_CTL_VAL_SIEN_SHIFT)
			|(int_cfg->int_event_cntl[i].mps<<CSL_VUSR_INT_CTL_VAL_MPS_SHIFT)
			|(int_cfg->int_event_cntl[i].vector<<CSL_VUSR_INT_CTL_VAL_VECTOR_SHIFT);
	}
		
	for(i=0; i<NUM_MPS; i++)
	{
		gpHyperLinkRegs->INT_PTR_IDX = i;
		gpHyperLinkRegs->INT_PTR_VAL= int_cfg->int_set_register_pointer[i];
	}

	//clear any pending interrupt
	gpHyperLinkRegs->INT_CLR= 0xFFFFFFFF;
}

/*****************************************************************************
 Prototype    : KeyStone_HyperLink_Addr_Map
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void KeyStone_HyperLink_Addr_Map(
	HyperLink_Address_Map * addr_map)
{
	int i;
	
	gpHyperLinkRegs->TX_SEL_CTL = 
		(addr_map->tx_addr_mask<<CSL_VUSR_TX_SEL_CTL_TXIGNMSK_SHIFT)
		|(addr_map->tx_priv_id_ovl<<CSL_VUSR_TX_SEL_CTL_TXPRIVIDOVL_SHIFT);
		
	gpHyperLinkRegs->RX_SEL_CTL = 
		(addr_map->rx_seg_sel<<CSL_VUSR_RX_SEL_CTL_RXSEGSEL_SHIFT)
		|(addr_map->rx_priv_id_sel<<CSL_VUSR_RX_SEL_CTL_RXPRIVIDSEL_SHIFT);
		
	for(i= 0; i< 16; i++)
	{
		gpHyperLinkRegs->RX_PRIV_IDX= i;
		gpHyperLinkRegs->RX_PRIV_VAL= addr_map->rx_priv_id_map[i];
	}

	for(i= 0; i< 64; i++)
	{
		gpHyperLinkRegs->RX_SEG_IDX= i;
		gpHyperLinkRegs->RX_SEG_VAL= 
			addr_map->rx_addr_segs[i].Seg_Base_Addr
			|addr_map->rx_addr_segs[i].Seg_Length;
	}

}


/*****************************************************************************
 Prototype    : KeyStone_HyperLink_Init
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void KeyStone_HyperLink_Init(HyperLink_Config * hyperLink_cfg)
{		
	debug_enable_PSC_module(5, 12);	

	if(HyperLink_LOOPBACK==hyperLink_cfg->loopback_mode)
	{
		/*disable all portal or remote register operation
		This bit should be set before iloop or reset bits are changed.*/
		gpHyperLinkRegs->CTL |= CSL_VUSR_CTL_SERIAL_STOP_MASK;

		/*Wait until no Remote Pending Request*/
		while(gpHyperLinkRegs->STS&CSL_VUSR_STS_RPEND_MASK);

		gpHyperLinkRegs->CTL |= CSL_VUSR_CTL_LOOPBACK_MASK;
	}
	//force 4 lanes always
	gpHyperLinkRegs->PWR = 
		(7<<CSL_VUSR_PWR_H2L_SHIFT)
		|(7<<CSL_VUSR_PWR_L2H_SHIFT)
		|(1<<CSL_VUSR_PWR_PWC_SHIFT)
		|(1<<CSL_VUSR_PWR_QUADLANE_SHIFT)
		|(0<<CSL_VUSR_PWR_ZEROLANE_SHIFT)
		|(0<<CSL_VUSR_PWR_SINGLELANE_SHIFT);

	/*enable operation*/
	gpHyperLinkRegs->CTL &= ~(CSL_VUSR_CTL_SERIAL_STOP_MASK);

    KeyStone_HyperLink_Addr_Map(&hyperLink_cfg->address_map);

    KeyStone_HyperLink_Interrupt_init(&hyperLink_cfg->interrupt_cfg);

	/*tell all receivers to ignore close to the first 3uS of data at beginning of training sequence*/
	gpHyperLinkRegs->SERDES_CTL_STS1= 0xFFFF0000;

    //PC TEST
	//gpHyperLinkRegs->SERDES_CTL_STS4= 0x000F0000;//No SERDES sleep for four lane mode operation

	hyperLinkSerdesRegs = (SerdesRegs *)&gpBootCfgRegs->VUSR_CFGPLL;
	KeyStone_HyperLink_Serdes_init(&hyperLink_cfg->serdes_cfg, hyperLinkSerdesRegs);
	Wait_Hyperlink_PLL_Lock();

	/*---------wait for link status OK-------------*/
	while(0==(gpHyperLinkRegs->STS&CSL_VUSR_STS_LINK_MASK));
	while(gpHyperLinkRegs->STS&CSL_VUSR_STS_SERIAL_HALT_MASK);
	while(gpHyperLinkRegs->STS&CSL_VUSR_STS_PLL_UNLOCK_MASK);
	while(0==(gpHyperLinkRegs->LINK_STS&CSL_VUSR_LINK_STS_RX_ONE_ID_MASK));

	/*after initialization, change the delay to default value to improve performance*/
	//gpHyperLinkRegs->SERDES_CTL_STS1= 0x092E0000;
}

/*****************************************************************************
 Prototype    : Hyperlink_config
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void Hyperlink_config(void)
{
	int iPrvId, iSeg;
	
	hyperLink_cfg.loopback_mode = HyperLink_LOOPBACK_DISABLE;
    /*------------------Initialize Hyperlink Serdes-------------------------*/ 
	serdeshyperLinkSetup.linkSpeed_GHz    = HYPERLINK_SPEED_GHZ;
	serdeshyperLinkSetup.testPattern      = SERDES_TEST_DISABLED; 
	serdeshyperLinkSetup.txOutputSwing    = 15; /*0~15 represents between 100 and 850 mVdfpp  */
	serdeshyperLinkSetup.txInvertPolarity = SERDES_TX_NORMAL_POLARITY; 
	serdeshyperLinkSetup.rxAlign          = SERDES_RX_COMMA_ALIGNMENT_ENABLE; 
    serdeshyperLinkSetup.rxCDR            = 5;
	serdeshyperLinkSetup.rxInvertPolarity = SERDES_RX_NORMAL_POLARITY; 
	serdeshyperLinkSetup.rxTermination    = SERDES_RX_TERM_COMMON_POINT_AC_COUPLE; 
	serdeshyperLinkSetup.rxEqualizerConfig= SERDES_RX_EQ_ADAPTIVE; 
	if(hyperLink_cfg.loopback_mode == HyperLink_LOOPBACK)
		serdeshyperLinkSetup.loopBack= SERDES_LOOPBACK_ENABLE;

	/*all Serdes Link use same configuration*/
	hyperLink_cfg.serdes_cfg.linkSetup[0]= &serdeshyperLinkSetup;
	hyperLink_cfg.serdes_cfg.linkSetup[1]= &serdeshyperLinkSetup;
	hyperLink_cfg.serdes_cfg.linkSetup[2]= &serdeshyperLinkSetup;
	hyperLink_cfg.serdes_cfg.linkSetup[3]= &serdeshyperLinkSetup;

	hyperLink_cfg.serdes_cfg.commonSetup.loopBandwidth= SERDES_PLL_LOOP_BAND_MID;

	/*----------------Initialize Hyperlink address map----------------------*/ 
	/*use 28 bits address for TX (256 MB) */
	hyperLink_cfg.address_map.tx_addr_mask = TX_ADDR_MASK_0x0FFFFFFF;

	/*overlay PrivID to higher 4 bits of address for TX*/
	hyperLink_cfg.address_map.tx_priv_id_ovl = TX_PRIVID_OVL_ADDR_31_28;

	/*Select higher 4 bits of address as PrivID for RX*/
	hyperLink_cfg.address_map.rx_priv_id_sel = RX_PRIVID_SEL_ADDR_31_28;

	/*map remote PrviID 0~7 to loccal ID 13*/
	hyperLink_cfg.address_map.rx_priv_id_map[0] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[1] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[2] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[3] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[4] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[5] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[6] = 13;
	hyperLink_cfg.address_map.rx_priv_id_map[7] = 13;

	/*map remote PrviID 8~15 to loccal ID 14*/
	hyperLink_cfg.address_map.rx_priv_id_map[8] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[9] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[10] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[11] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[12] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[13] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[14] = 14;
	hyperLink_cfg.address_map.rx_priv_id_map[15] = 14;

	/*use bit 24~29 (4 bits (24~27) MSB address, 2 bits (28~29) 
	remote PriviID) as index to lookup segment/length table*/
	hyperLink_cfg.address_map.rx_seg_sel = RX_SEG_SEL_ADDR_29_24;

	/*for this test, 28 bits address space (256 MB) is splitted into 16 segments
	segment 0 -> core 0 local memory (same for all remote PrviID)
	segment 1 -> core 1 local memory (same for all remote PrviID)
	segment 2 -> core 2 local memory (same for all remote PrviID)
	......
	segment 8 -> DDR3 (same for all remote PrviID)
	segment 9 -> DDR3+0x01000000 (same for all remote PrviID)
	segment A -> DDR3+0x02000000 (same for all remote PrviID)
	segment B -> DDR3+0x03000000 (same for all remote PrviID)
	segment C -> SL2 (same for all remote PrviID)
	segment D -> DDR3+0x04000000+(PrviID[1:0]*3+0)*0x01000000
	segment E -> DDR3+0x04000000+(PrviID[1:0]*3+1)*0x01000000
	segment F -> DDR3+0x04000000+(PrviID[1:0]*3+2)*0x01000000

	Since the Hyperlink memory map window start at 0x40000000, 
	the memory map on a remote master is:
	0x40800000 	Core 0 LL2
	0x41800000 	Core 1 LL2
	0x42800000 	Core 2 LL2
	......
	0x48000000~0x4BFFFFFF 	DDR3
	0x4C000000 	SL2
	0x4D000000~0x4FFFFFFF 	DDR3 (different master may access different section)
	*/

	/*map local memory into the same segments for all PrivID (remote masters)*/
	//实际需要使用的是0x48000000~0x4BFFFFFF 	DDR3
	//使用16MB的DDR地址0x88000000
	for(iSeg= 0; iSeg<8; iSeg++)
	for(iPrvId=0; iPrvId<4; iPrvId++)
	{
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Base_Addr= 
			0x10000000+iSeg*0x01000000;
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Length= 
			RX_SEG_LEN_0x0_0100_0000;
	}

	/*map a part of DDR3 into the same segments for all PrvID (remote masters)*/
	for(iSeg= 8; iSeg<0xC; iSeg++)
	for(iPrvId=0; iPrvId<4; iPrvId++)
	{
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Base_Addr= 
			DDR_SPACE_ACCESSED_BY_HYPERLINK+(iSeg-8)*0x01000000;
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Length= 
			RX_SEG_LEN_0x0_0100_0000;
	}

	/*map SL2 into same segement for all PrvID (remote masters)*/
	for(iPrvId=0; iPrvId<4; iPrvId++)
	{
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|0xC].Seg_Base_Addr= 
			0x0C000000;
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|0xC].Seg_Length= 
			RX_SEG_LEN_0x0_0100_0000;
	}

	/*map different DDR3 sections into the segements 
	of different PrvID (remote masters)*/
	for(iPrvId=0; iPrvId<4; iPrvId++)
	for(iSeg= 0xD; iSeg<=0xF; iSeg++)
	{
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Base_Addr= 
			DDR_SPACE_ACCESSED_BY_HYPERLINK+0x04000000+(iPrvId*3+iSeg-0xD)*0x01000000;
		hyperLink_cfg.address_map.rx_addr_segs[(iPrvId<<4)|iSeg].Seg_Length= 
			RX_SEG_LEN_0x0_0100_0000;
	}

    /*------------------Initialize Hyperlink interrupt----------------------*/ 
	/*map Hyperlink error/status interrupt to interrupt vector 0*/
    hyperLink_cfg.interrupt_cfg.sts_int_enable = 1;
    hyperLink_cfg.interrupt_cfg.sts_int_vec= 0;

	/*interrupt to remote DSP to interrupt vector 1*/
    hyperLink_cfg.interrupt_cfg.int_event_cntl[0].si_en = 1;
    hyperLink_cfg.interrupt_cfg.int_event_cntl[0].mps = 0;
    hyperLink_cfg.interrupt_cfg.int_event_cntl[0].vector = 1;
	/*interrupt to remote DSP to interrupt vector 2*/
    hyperLink_cfg.interrupt_cfg.int_event_cntl[1].si_en = 1;
    hyperLink_cfg.interrupt_cfg.int_event_cntl[1].mps = 0;
    hyperLink_cfg.interrupt_cfg.int_event_cntl[1].vector = 2;

	/*generate interrupt packet to remote DSP when local interrupt event happens*/
    hyperLink_cfg.interrupt_cfg.int_local= 0;
	/*route interrupt packet from remote DSP to interrupt pending register*/
    hyperLink_cfg.interrupt_cfg.int2cfg = 1;

    KeyStone_HyperLink_Init(&hyperLink_cfg);
}
/*this interrupt test is done in loopback mode,
a hardware event is trigger manually, a interrupt packet is generated and 
loopback to this DSP and trigger interrupt to the DSP core. The latency 
between trigger and the entry of the ISR are measured*/
void HyperLink_Interrupt_Test()
{
	Uint32 i=0;
	Uint32 uiStartTSC= TSCL;
	CSL_VusrRegs * gpHyperLinkRemoteRegs;
	gpHyperLinkRemoteRegs = (CSL_VusrRegs*)(0x21400000+128);

	/*manually trigger the hardware event, which will generate interrupt packet to remote side*/
	//这个配置决定了使用哪个HW event,可以配置0:63,发送中断报文
	if(i==2)
	{
		gpHyperLinkRegs->SW_INT= 0;
	}
	
	if(i==3)
	{
		gpHyperLinkRegs->SW_INT= 1;
	}

	/*the interrupt packet is loop back to this DSP and trigger 
	interrupt to this DSP core, here waiting for the interrupt*/
	asm("IDLE");

	/*the time stamp at the entry of the interrupt is recorded in the ISR*/
}


/*****************************************************************************
 Prototype    : HyperLink_TX2_DSP1
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/ 
void HyperLink_TX2_DSP1()
{	
	unsigned int i;
	unsigned int uiByteCount;
	unsigned int uiStartAddress;
	unsigned int uiPattern[10];
    volatile unsigned long long *ulpAddressPointer;	
	
	uiByteCount=16;
	uiPattern[0]=0xA0A0A0A0;
	uiPattern[1]=0x5a5a5a5a;
	uiPattern[2]=0x00000000;
	uiPattern[3]=0xFFFFFFFF;
	uiStartAddress = StartAddress_BY_HYPERLINK;

	
	/*发送数据到另一个DSP的DDR*/
	ulpAddressPointer = (unsigned long long *)uiStartAddress;
	for(i=0; i<uiByteCount/8; i++)
	{
		/* fill with address value */
        *ulpAddressPointer = _itoll(uiPattern[2*i+1], uiPattern[2*i]);	  
        ulpAddressPointer+= 1;
    }
	WritebackInvalidCache((void *)uiStartAddress, uiByteCount);

	//发送中断	
	HyperLink_Interrupt_Test();	
}	
/*****************************************************************************
 Prototype    : HyperLink_Int_Handle
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void HyperLink_Int_Handle()
{
	Uint32 intVector;
	
	/*read the HyperLink interrupt vector*/
	intVector= gpHyperLinkRegs->INT_PRI_VEC;

	while(0==(intVector&CSL_VUSR_INT_PRI_VEC_NOINTPEND_MASK))
	{
		if(0==intVector)//HyperLink error is routed to vector 0 for this test.
		{
			/*disable all portal or remote register operation
			This bit should be set before iloop or reset bits are changed.*/
			gpHyperLinkRegs->CTL |= CSL_VUSR_CTL_SERIAL_STOP_MASK;

			/*Wait until no Remote Pending Request*/
			while(gpHyperLinkRegs->STS&CSL_VUSR_STS_RPEND_MASK);
		
			//clear error status
			gpHyperLinkRegs->STS |= CSL_VUSR_STS_LERROR_MASK
				|CSL_VUSR_STS_RERROR_MASK;
			
			/*enable operation*/
			gpHyperLinkRegs->CTL &= ~(CSL_VUSR_CTL_SERIAL_STOP_MASK);
		}
		if(1==intVector)
		{
			/*disable all portal or remote register operation
			This bit should be set before iloop or reset bits are changed.*/
			gpHyperLinkRegs->CTL |= CSL_VUSR_CTL_SERIAL_STOP_MASK;

			/*Wait until no Remote Pending Request*/
			while(gpHyperLinkRegs->STS&CSL_VUSR_STS_RPEND_MASK);
		
			/*enable operation*/
			gpHyperLinkRegs->CTL &= ~(CSL_VUSR_CTL_SERIAL_STOP_MASK);
		}
		if(2==intVector)
		{
			/*disable all portal or remote register operation
			This bit should be set before iloop or reset bits are changed.*/
			gpHyperLinkRegs->CTL |= CSL_VUSR_CTL_SERIAL_STOP_MASK;

			/*Wait until no Remote Pending Request*/
			while(gpHyperLinkRegs->STS&CSL_VUSR_STS_RPEND_MASK);

			/*enable operation*/
			gpHyperLinkRegs->CTL &= ~(CSL_VUSR_CTL_SERIAL_STOP_MASK);
		}
		/*write back to clear that interrupt*/
		gpHyperLinkRegs->INT_PRI_VEC= intVector;
		gpHyperLinkRegs->INT_CLR= (1<<intVector);

		/*read the HyperLink interrupt vector*/
		intVector= gpHyperLinkRegs->INT_PRI_VEC;
	}
}

/*****************************************************************************
 Prototype    : HyperLink_init
 Description  : 
 Input        : None
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void HyperLink_init()
{	
	memset(&serdeshyperLinkSetup, 0, sizeof(serdeshyperLinkSetup));
	memset(&hyperLink_cfg, 0, sizeof(hyperLink_cfg));
		 
	hyperLink_cfg.serdes_cfg.commonSetup.inputRefClock_MHz = HYPERLINK_CLOCK_MHZ;	
    Hyperlink_config();//hyperlink configuration	
}

