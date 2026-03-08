/******************************************************************************
 *  ============================================================================
 *  @n   (C) Copyright 2011, 2012, Texas Instruments, Inc.
 *  @n   All Rights Reserved.
 *  @n
 *  @n GENERAL DISCLAIMER
 *  @n ------------------ 
 *  @n All software and related documentation is provided "AS IS" and without   
 *  @n warranty or support of any kind and Texas Instruments expressly disclaims
 *  @n all other warranties, express or implied, including, but not limited to, 
 *  @n the implied warranties of merchantability and fitness for a particular   
 *  @n purpose.  Under no circumstances shall Texas Instruments be liable for   
 *  @n any incidental, special or consequential damages that result from the    
 *  @n use or inability to use the software or related documentation, even if   
 *  @n Texas Instruments has been advised of the liability.                     
 *  ============================================================================
 ******************************************************************************
  File Name     : dsp0_mss.c
  Version       : v1.0  Initial Draft
  Author        : sxf
  Created       : 2017/02/10
  Last Modified :
  Description   : 
       This project shows how to use the protection hardware to build the robust
    system on KeyStone device
  Function List :
              main
  History       :
  1.Date        : 2017/02/10
    Author      : sxf
    Modification: Created file

  2.Date         : 
    Author       : 
    Modification : 
    
    本工程为debug工程,调试完成后注释掉所有////for 319 project debug
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "dsp0_common.h"
#include "DDR_init.h"
//#include "Robust_MNav_Init.h"
#include "SRIO_Init.h"
#include "KeyStone_HyperLink_Init.h"
#include "interrupt.h"
//#include "Robust_watch_dog.h"
//#include "Robust_memory_transfer.h" //add by sxf 20160108
#include "peripheral.h"
#include "UART_Init_drv.h"
//#include "Robust_MSU.h"//add by sxf 20160108
#include "csl_semAux.h"

//#include "Hook.h"

/*for 319 project debug :add by sxf 20160108用于led flash*/
#include "platform.h" 

//#include <cerrno> 
#include <stdlib.h>


//#include "resource_mgr.h"
#include "dsp0_image.h"
#include "dsp0_test.h"
#include "SRIO_Init.h"





/*for 319 project debug :add by sxf 20160108用于led flash*/
#define PLATFORM_USER_LED_CLASS_Test 0
#define PLATFORM_LED_ON_Test  1
#define PLATFORM_LED_OFF_Test 0
#define PLATFORM_SPI_HW_SEM			3
/*for 319 project debug :add by sxf 20160108用于led flash*/




#define WATCH_DOG_CONFIG 		1





extern void SRIO_temperature_test();




/*for 319 project debug :add by sxf 20160108用于MulticoreBoot*/
void MulticoreBoot()
{

    //int i;
	int *pBootMagicAddCore0;
    int *IpcGr0;

	//platform_init_flags  sFlags;
    //platform_init_config sConfig;
    
    int pform_status;
    int coreId = 0;
	coreId=DNUM;
	pform_status=0;
 
	if(coreId==0)
	{
		/*boot start*/
		if (pform_status == 0)
	    {

	       /*write Boot Magic add of other cores and send IPC interrupt*/
	       pBootMagicAddCore0 = (int*)0x1087FFFC;
		   
  	       (*pBootMagicAddCore0)= 0x10800200;// translate to global address

		   IpcGr0  = (int*)0x02620240;
		   //使用多核时，打开下面的程序
		   
#if 0	
	       for(i = 1;i < CORE_NUM_6678; i++)//write the other cores' boot magic address
	       {
				// Unlock Config
				KICK0 = KICK0_UNLOCK;
				KICK1 = KICK1_UNLOCK;

		   		*(pBootMagicAddCore0+ (0x01000000*i)/4 ) = (*pBootMagicAddCore0) + 0x01000000 * i;
				/*warning:when running on no-boot mode,core0~core7 must all be connected to the target*/
				//core0 sent ipc interrupt to
				*(IpcGr0+i) = (*(IpcGr0+i)) | 0x00000001;

				// lock Config
				KICK0 = KICK_LOCK;
				KICK1 = KICK_LOCK;
	       }
#endif		   
	    }
	}
	TSC_delay_ms(50);

}




	
/*interrup drop test can not safely return, thus other tests after this will not run,
to run following tests, this test must be disabled.*/
//#define INT_DROP_TEST 		1 	


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


/*memory protection and address extension configuration table,
for C6678 EVM
Please note, this configuration table can not be put in the memory segment 
that will be modified based on this table. Normally, to put this table 
in LL2 is a good idea.
*/
#pragma DATA_SECTION(XMC_MPAX_cfg_table_C6678_319,".far:Core_MPAX")
MPAX_Config XMC_MPAX_cfg_table_C6678_319[]=
{
    /*BADDR         RADDR           SegementSize      AccessPermisionMask
    32-bit virtual  36-bit physical in byte, must     Access types allowed
    base address    address right   be power of 2     in this address range
                    shift by 4                                            */
    {0x0C000000,    0x00C000000>>4,  4*1024*1024,    MP_SR|MP_SW|MP_SX|MP_UR|MP_UW|MP_UX},/*SL2, RWX*/
    {0x10000000,    0x010000000>>4,  0x08000000,     MP_SR|MP_SW|MP_UR|MP_UW},/*LL2 global address space, RW*/
    {0x18000000,    0x00C3FF000>>4,  4*1024,    	 MP_SR|MP_SW|MP_UR|MP_UW},/*remap Shared L2 to 0x18000000 for read/write*/
    {0x20000000,    0x020000000>>4,  0x02000000,     MP_SR|MP_SW|MP_UR|MP_UW},/*peripherals configuraion, RW*/
    {0x34000000,    0x034000000>>4,  0x00200000,     MP_SR|MP_SW|MP_UR|MP_UW},/*QM, RW*/
    {0x40000000,    0x040000000>>4,  0x10000000,     MP_SR|MP_SW|MP_UR|MP_UW},/*HyperLink mapped memory windows, RW*/
    {0x60000000,    0x060000000>>4,  0x10000000,     MP_SR|MP_SW|MP_UR|MP_UW},/*PCIE mapped memory windows, RW*/
    {0x70000000,    0x070000000>>4,  0x10000000,     MP_SR|MP_SW|MP_SX|MP_UR|MP_UW|MP_UX},/*EMIF, RWX*/
    {0x21000000,    0x100000000>>4,  4*1024,         MP_SR|MP_SW|MP_UR|MP_UW},/*map DDR3 configuration registers at physical address 0x1:00000000 to logical address 0x21000000*/
    {0x80000000,    0x800000000>>4,  0x20000000,     MP_SR|MP_SW|MP_SX|MP_UR|MP_UW|MP_UX},/*512MB DDR, RWX:pbuf*/
    {0xA0000000,    0x820000000>>4,  0x20000000,     MP_SR|MP_SW|MP_SX|MP_UR|MP_UW|MP_UX},/*512MB DDR, RW:core 0*/
};

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
 Prototype    : mss_main
 Description  : system test main function
 Input        : void  
 Output       : None
 Return Value : 
 
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 

*****************************************************************************/
void mss_main(void)
{
	int i;

	DDR_ECC_Config DDR_ECC_cfg;
	
    IER= 0;//disable all interrupts

	TSC_init();	//initialize TSC to measure cycle,目前只在core0配置
	/*所有core配置*/
    CACHE_setL1PSize(CACHE_L1_32KCACHE);
    CACHE_setL1DSize(CACHE_L1_32KCACHE);
    CACHE_setL2Size(CACHE_0KCACHE);
	CACHE_invAllL1p(CACHE_WAIT);
	CACHE_wbInvAllL1d(CACHE_WAIT);
	CACHE_wbInvAllL2(CACHE_WAIT);
	CSL_XMC_invalidatePrefetchBuffer();
	_mfence();
	_mfence();
	

	
	/*所有core配置*/
	/* protect L1 as cache */
   	L1_cache_protection();

	/*enable L1P ED and scrub whole L1P*/	
	L1P_EDC_setup();

   	/*enable LL2 EDC and scrub whole LL2*/	
	LL2_EDC_setup();	
	
	/*Enable MSMC EDC and setup scrubbing cycle counter= 255*1024*/	
	KeyStone_SL2_EDC_enable(255);

	//MAR配置,只在core0配置
	/*make SL2 cacheable, not prefetchable because of
	"Advisory 28 Read Exception and Data Corruption Issue"*/
	for(i=12; i<16; i++)
		gpCGEM_regs->MAR[i]=1;

	/*make other cores local memory cacheable*/
	for(i=16; i<24; i++)
		gpCGEM_regs->MAR[i]=1;
	//20170213   调试效率时，打开调试
	/*make other cores local memory cacheable and prefetchable*/
	//for(i=16; i<24; i++)
		//gpCGEM_regs->MAR[i]=1|(1<<CSL_CGEM_MAR0_PFX_SHIFT);	

	/*make other space non-cacheable and non-prefetchable*/
	for(i=24; i<128; i++)
		gpCGEM_regs->MAR[i]=0;

	/*make DDR cacheable*/
	for(i=128; i<256; i++)
		gpCGEM_regs->MAR[i]=1;
	//20170213   调试效率时，打开调试
	/*make DDR cacheable and prefetchable*/
	//for(i=128; i<256; i++)
		//gpCGEM_regs->MAR[i]=1|(1<<CSL_CGEM_MAR0_PFX_SHIFT); 


		
	//只在core0配置
	/*XMC memory address extension/mapping and memory protection*/
	XMC_MPAX_setup(XMC_MPAX_cfg_table_C6678_319, 0, 
		sizeof(XMC_MPAX_cfg_table_C6678_319)/sizeof(MPAX_Config));
	
	//DSP core speed
	Core_PLL_init (100, 10, 1); 	//for 100MHz input clock
	memset((void *)&DDR_ECC_cfg, 0, sizeof(DDR_ECC_cfg));
	//DDR3配置
	DDR_init (100, 10, 1, &DDR_ECC_cfg);
	//DEBUG时，测试一次，正式代码去掉
	while(DDR3_testflag)		
	{
		//接口test	
		DDR3_test();
	}

	/*core0完成初始化后,IPC wake other cores*/
	//注释掉boot，最终版本需要
	MulticoreBoot();

/**************************************************************/


 
/**************************************************************/
	
	
	EDMA_init(); //initialize the EDMA for test.		
	
	DSP0_GPIO_init();	

	//enable PA for test
	debug_enable_PSC_module(CSL_PSC_PD_PASS, CSL_PSC_LPSC_PKTPROC);
	/* add by sxf 20160108:PKTDMA(core to core TX/RX) */
	//QMSS_init();
	//QMSS_PktDMA_init();
	//checkQueues();	//for 319 project debug


	//enable HyperLink power and clock domain
	//debug_enable_PSC_module(5, 12);//在HyperLink_init中enable
	//hyperlink init
	//HyperLink_init();
	
	/* add by sxf 20160226 for EMIF */
	//On Shannon, EMIF belong to clock domain 3 which need be enabled
	//delete 20170620:查看是否和固化失败有关
	debug_enable_PSC_module(CSL_PSC_PD_ALWAYSON, CSL_PSC_LPSC_EMIF25_SPI);
	TSC_delay_ms(2000);//add 20170620
	EMIF_init();
	//On Shannon, SPI belong to clock domain 3 which need be enabled
	//SPI init		


	//其余core的中断Init
	othercores_Interrupts_Init();

	
	
	

	/*exception configuration, also configure event 23 as the CIC exception input*/
	//KeyStone_Exception_cfg();

	/* Initialize the SRIO to verify the SMS and SES memory protection */
	//SRIO_Init();

	
	/*INTC/CIC initialization*/	
	core0_Interrupts_Init();

	TSC_delay_ms(500);//add 20170619
	/*waiting for FPGA srio handshake:握手成功,fpga发送报文和doorbell*/
	//319整机调试握手，其它调试不握手
	//SRIO_FPGA_Handshake();
	TSC_delay_ms(5);//add 20170619
	/*waiting for FPGA gpio handshake*/
	//319整机调试握手，其它调试不握手
	GPIO_FPGA_Handshake();
	
	//接口test
	//FPGA_EMIF16_RTXtest();
	
		

	//test测试或高低温测试等
	//if(temperature_testflag==1)
	//{
		//while(1)
		//{	
			//接口test
			//FPGA_EMIF16_RTXtest();

			//GPIO输出
			//GPIO_test();
			
			//FPGA_EMIF16_RTX();
			//TSC_delay_ms(20); 	//Wait 600us for HW init to complete
			//i++;	

			//SRIO_temperature_test();
		//}
	//}
	//else
	//{
		//图像算法调试
		//debugDSP0_image();
		//while(1)
		{
			i++;
		}
		DSP0_image();
		
		
		//HyperLink Test
		//HyperLink_TX2_DSP1();

	//}	

	

	

/**************************************************************/


	/*系统资源初始化*/
	for(i=0;i<32;i++)
	{
		SemaphoreSyncMark[i] = 0;
	}
	/*write back data from cache to descriptor RAM*/
	WritebackCache((void *)(SemaphoreSyncMark), 128);

	CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
	CSL_semReleaseSemaphore(SEMAPHORE_NUM1);



/**************************************************************/
	
	/*MSU_Image ，临时使用业务处理代码*/
	//Core0_Image();

	//2017临时删除
	//MSU_Image();
	
/**************************************************************/
/**************************************************************/


/*丢弃定时中断Timer_ISR，触发CSL_GEM_INTERR (在KeyStone_Exception_cfg配置)，
导致异常发生，Exception happened at a place can not safely return!*/
/*异常测试,正式代码去掉*/
//#if INT_DROP_TEST 	
/*interrup drop test can not safely return, thus other tests after this will not run*/
	//INT_drop_test();
//#endif

	puts("test complete.");
   	//while(1)
	//{
		
		
		//业务处理代码core0为中断处理代码
		//eg: core1_image();
		/*MSU_Image ，临时使用业务处理代码*/
		//MSU_Image();

		
	    
	//}
}

