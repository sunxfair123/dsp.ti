/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : Robust_INTC.c
  Version       : Initial Draft
  Author        : Brighton Feng
  Created       : 2012/10/7
  Last Modified :
  Description   : This file setup interrupt controller, and includes:
                  1, ISR for memory protection and EDC/ECC.
                  2, function to manually tigger two interrupts to show
                  the interrup drop monitor
  Function List :
              INT_drop_test
              LL2_EDC_ISR
              core0_Interrupts_Init
              SL2_EDC_ISR
              Timer_ISR
  History       :
  1.Date        : 2012/10/7
    Author      : Brighton Feng
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <c6x.h>
#include <tistdtypes.h>
#include <csl_cpintc.h>
#include "dsp0_common.h"
#include "SRIO_Init_drv.h"

#include "CSL_msmc.h"
#include "CSL_msmcAux.h"
#include "csl_semAux.h"
#include <cslr_chip.h>
#include "interrupt.h"
#include "SRIO_Init.h"
#include "KeyStone_HyperLink_init.h"
#include "UART_Init_drv.h"
#include "dsp0_secextern.h"
#include "dsp0_image.h"
#include "dsp0_test.h"





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
 
extern volatile Uint8 SRIO_temperatureflag1;
extern volatile Uint8 SRIO_temperatureflag2;
extern volatile Uint8 DDR3_temperatureflag1;
extern volatile Uint32 SRIO_nodoorbellcount;
extern volatile int RxFrameFlag;

volatile Int16 targetempty;//连续获取目标次数









#pragma DATA_SECTION (SemaphoreSyncMark, ".sync");
#pragma DATA_ALIGN (SemaphoreSyncMark, 4)
Uint32  SemaphoreSyncMark[32];


 


 
 //Uint32 gCIC_out_num21 = 32;//reserved
 Uint32 gCIC_out_num22 = 33;
 //Uint32 gCIC_out_num23 = 34;//used by Uint32 guwIntcCICExcEvtIdx= 23;
 Uint32 gCIC_out_num24 = 35;
 Uint32 gCIC_out_num25 = 36;
 Uint32 gCIC_out_num26 = 37;
 Uint32 gCIC_out_num27 = 38;
 Uint32 gCIC_out_num28 = 39;
 Uint32 gCIC_out_num29 = 40;
 Uint32 gCIC_out_num30 = 41;
 Uint32 gCIC_out_num31 = 42;




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

/*add by sxf 20160108 for IPC interrupt*/
/* IPCGR Info */
Uint32 iIPCGRInfo[CORENUM] = {
								IPCGR0,
								IPCGR1,
								IPCGR2,
								IPCGR3,
								IPCGR4,
								IPCGR5,
								IPCGR6,
								IPCGR7
							 };
/* IPCAR Info */
Uint32 iIPCARInfo[CORENUM] = {
								IPCAR0,
								IPCAR1,
								IPCAR2,
								IPCAR3,
								IPCAR4,
								IPCAR5,
								IPCAR6,
								IPCAR7
							 };




//Uint32 uiDoorbell_TSC; 	//timestamp when doorbell interrupt happens

/*define the message convey by doorbell for this test*/
//char * doorbell_msg_str[]=
//{
	//"doorbell for latency test.", 
	//"doorbell which indicates NREAD operation done.",
	//"doorbell which indicates NWRITE operation done.",
	//"doorbell which indicates NWRITE_R operation done.",  
	//"doorbell which indicates SWRITE operation done."
//};

/*****************************************************************************
 Prototype    : GPIO_Interrupts_Init
 Description  : Interrupt initilization for robust system test
 Input        : void  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void GPIO_Interrupts_Init(void)
{
	//combine all GPIO interrupts to INTC event 2
	//中断GPIO[8:9]
	gpCGEM_regs->EVTMASK[2]= ~(		
		(1<<(CSL_GEM_GPINT8-64))|
		(1<<(CSL_GEM_GPINT9-64))|
		(1<<(CSL_GEM_GPINT10-64))
		);
}

/*****************************************************************************
 Prototype    : core0_Interrupts_Init
 Description  : Interrupt initilization for robust system test
 Input        : void  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void core0_Interrupts_Init(void)
{
	gpCGEM_regs->INTMUX1 = 0;
	gpCGEM_regs->INTMUX2 = 0;
	gpCGEM_regs->INTMUX3 = 0;

	/* disable event combine */
	gpCGEM_regs->EVTMASK[0] = 0xffffffff;
	gpCGEM_regs->EVTMASK[1] = 0xffffffff;
	gpCGEM_regs->EVTMASK[2] = 0xffffffff;
	gpCGEM_regs->EVTMASK[3] = 0xffffffff;	
	
	/*-----------------------CP_INTC configuration---------------------*/
	/* Disable Global host interrupts. */
	gpCIC_regs->GLOBAL_ENABLE_HINT_REG= 0;	

	/* route following system events to CIC out (event 23 of INTC), and then to Core interrupt
	CIC: 87 MSMC_scrub_cerror Correctable (1-bit) soft error detected during scrub cycle
	CIC: 98 MSMC_dedc_cerror Correctable (1-bit) soft error detected on SRAM read
	*/
	//KeyStone_CIC_event_map(gpCIC_regs, 87, gCIC_out_num);
	//KeyStone_CIC_event_map(gpCIC_regs, 98, gCIC_out_num);


	/*map RX EVT to CIC out*/
	//delete uart,20170614
	//KeyStone_CIC_event_map(gpCIC_regs, CSL_INTC0_UARTINT, gCIC_out_num24);

	/*map HyperLink Interrupt events (111) to CIC0 out33*/
	KeyStone_CIC_event_map(gpCIC_regs, CSL_INTC0_VUSR_INT_O, gCIC_out_num22);

	/* Enable Global host interrupts. */
	gpCIC_regs->GLOBAL_ENABLE_HINT_REG= 1;


	/*GPIO中断设置*/
	GPIO_Interrupts_Init();

	

	
	/*-----------------------Core INTC configuration---------------------*/
	/*map LL2 correctable soft error to DSP core interrupt 4,
	map MSMC correctable soft error (from CIC) to DSP core interrupt 5*/
	//gpCGEM_regs->INTMUX1 = 
		//(CSL_GEM_UMC_ED1<<CSL_CGEM_INTMUX1_INTSEL4_SHIFT)|
		//(22<<CSL_CGEM_INTMUX1_INTSEL5_SHIFT);

	/*map IPC interrupt to interrupt 5 */
	/*map SRIO interrupt to interrupt 6,INTDST(n+16) */
	//map hyperlink(this event 22) to interrupt 7 	
	gpCGEM_regs->INTMUX1 = 		
		(CSL_GEM_IPC_LOCAL<<CSL_CGEM_INTMUX1_INTSEL5_SHIFT)|
		(CSL_GEM_INTDST_N_PLUS_16<<CSL_CGEM_INTMUX1_INTSEL6_SHIFT)|
		(22<<CSL_CGEM_INTMUX1_INTSEL7_SHIFT);

	/*map uart interrupt to interrupt 10 */
	//delete uart,20170614
	//gpCGEM_regs->INTMUX2 = 
	//(CSL_GEM_INTC0_OUT_35_PLUS_11_MUL_N_OR_INTC0_OUT_35_PLUS_11_MUL_N_MINUS_4<<CSL_CGEM_INTMUX2_INTSEL10_SHIFT);
	
	//map GPIO INTC event 2 to interrupt 15
	//map timer 4 on Appleton/Nyquist or timer 8 on Shannon interrupt to interrupt14
	//map timer DNUM (watchdog)on Shannon interrupt to interrupt13
	gpCGEM_regs->INTMUX3 = 
	(CSL_GEM_EVT2<<CSL_CGEM_INTMUX3_INTSEL15_SHIFT);
	//(66<<CSL_CGEM_INTMUX3_INTSEL14_SHIFT)|
	//(64<<CSL_CGEM_INTMUX3_INTSEL13_SHIFT);
	
	/*Clear all DSP core events*/
	gpCGEM_regs->EVTCLR[0]= 	0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[1]= 	0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[2]= 	0xFFFFFFFF;
	gpCGEM_regs->EVTCLR[3]= 	0xFFFFFFFF;

	//clear DSP core interrupt flag
	ICR= IFR;

	//enable interrupts
	//IER |= (1<<1)|(1<<5)|(1<<13)|(1<<14)|(1<<15);
	//delete uart,20170614
	//IER = 3|(1<<5)|(1<<6)|(1<<7)|(1<<10)|(1<<15);
	IER = 3|(1<<5)|(1<<6)|(1<<7)|(1<<15);
	//KeyStone_Exception_cfg配置了(1<<1)，最终版本需要考虑,
	//是否在这里enable NMI exception，还是只在KeyStone_Exception_cfg配置就可以了
	/*enable NMI exception*/
	//IER = IER|(1<<CSL_CHIP_IER_NMI_SHIFT);
	
	gpCGEM_regs->INTDMASK= ~IER; /*only monitor drop of enabled interrupts*/

	/*Interrupt Service Table Pointer to begining of LL2 memory*/
	ISTP= 0x800000;

	//enable GIE
	TSR |= 1;

	
}


/*****************************************************************************
 Prototype    : othercores_Interrupts_Init
 Description  : Interrupt initilization for robust system test
 Input        : void  
 Output       : None
 Return Value : 
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
void othercores_Interrupts_Init(void)
{
	Uint32 CoreNum;

	//IPC中断
	CoreNum = DNUM;
}

/*****************************************************************************
 Prototype    : INT_drop_test
 Description  : Interrupt dorp test 
 Input        : None
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2012/10/29
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
/*丢弃定时中断Timer_ISR，触发CSL_GEM_INTERR (在KeyStone_Exception_cfg配置)，
导致异常发生，Exception happened at a place can not safely return!
*/


/*****************************************************************************
 Prototype    : LL2_EDC_ISR
 Description  : LL2 correctable EDC interrupt
 Input        : None
 Output       : None
 Return Value : void
 
  History        :
  1.Date         : 2012/10/29
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/
/*****************************************************************************
 Prototype    : SL2_EDC_ISR
 Description  : SL2 correctable EDC interrupt
 Input        : None
 Output       : None
 Return Value : interrupt
 
  History        :
  1.Date         : 2012/10/29
    Author       : Brighton Feng
    Modification : Created function

*****************************************************************************/

/*****************************************************************************
 Prototype    : IPC_ISR
 Description  : Interrupt service routine for IPC
 Input        : void  
 Output       : None
 Return Value : interrupt
 
  History        :
  1.Date         : 2016/01/08
    Author       : sxf
    Modification : Created function
用于传递不同的动作模式
*****************************************************************************/
interrupt void IPC_ACT_ISR(void)
{
	volatile Uint32 read_ipcgr;
	//Uint32 i;
	//Uint32 j;
    Uint32 CoreNum;
	//Uint32 result;
    //Uint32 iPrevCore;
    Uint32 * uipSemaphoreAdd;

	/*for 319 project debug :add by sxf 20160108用于led flash*/
	Uint32 CoreNum1; //用于发生IPC core0中断的核号
	Uint32 Lednum; 
	Uint32 circles;
	Uint32 NextCore;
		
	Uint32 interruptAct=0;

	
	CoreNum = DNUM;

    read_ipcgr = *(volatile Uint32 *) iIPCGRInfo[CoreNum];

    *(volatile Uint32 *) iIPCARInfo[CoreNum] = read_ipcgr; //clear the related source info

    //printf("Receive interrupt from Core %d with info 0x%x, cycle = %d\n,uirecord=%d", read_ipcgr, TSCL,uirecord);


    //printf("Set interrupt from Core %x to Core %d, cycle = %d,uirecord=%d\n", CoreNum, iNextCore, TSCL,uirecord);
	

    /*send action mode to other cores*/	
	interruptAct +=16;

	/*for 319 project debug:用于验证8个核的led flash*/
	if(CoreNum==0)
	{
		//for 319 project debug,已经验证OK
		printf("Finished000:IPC_ACT_ISR CoreNum%d\n",CoreNum);

		/*for 319 project debug,semaphore operation:比如某些动作需要信号量操作*/
		if(read_ipcgr==16)
		{			
			/* Check for the semaphore you want to use is available or not */
        	if (CSL_semIsFree(SEMAPHORE_NUM0) != 1)
			{
				printf("Core %d Error: The Semaphore %d is not free\n", DNUM, SEMAPHORE_NUM0);
				CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
			}
			while(0==CSL_semAcquireDirect(SEMAPHORE_NUM0));
			/*获得信号量SEMAPHORE_NUM0的core*/
			//result = CSL_semGetResourceOwner(SEMAPHORE_NUM0);

			uipSemaphoreAdd=(Uint32 *)(0x0C2f8000+16*4);//SemaphoreSyncMark的地址
			/*invalid cache before read descriptor RAM*/
			InvalidCache((void *)(uipSemaphoreAdd), 4);

			CoreNum1=*uipSemaphoreAdd;
			if((0<CoreNum1)&&(CoreNum1<4))
			{
				Lednum=1;
				circles =10;
				//NextCore = (CoreNum1);
				NextCore = (CoreNum1+1);
				
				//Led_Flash(1, Lednum, circles);
				//Led_Flash(2, Lednum, circles);
				//Led_Flash(3, Lednum, circles);

				//*uipSemaphoreAdd=NextCore;
				/*write back data from cache to descriptor RAM*/
				//WritebackCache((void *)(uipSemaphoreAdd), 4);

				CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
				// Unlock Config
				KICK0 = KICK0_UNLOCK;
				KICK1 = KICK1_UNLOCK;

				*(volatile Uint32 *) iIPCGRInfo[NextCore] = interruptAct;

		   		*(volatile Uint32 *) iIPCGRInfo[NextCore] |= 1;

				// lock Config
				KICK0 = KICK_LOCK;
				KICK1 = KICK_LOCK;
				
			}
			if((3<CoreNum1)&&(CoreNum1<7))
			{
				Lednum=2;
				circles =10;
				NextCore = (CoreNum1+1);
				

				//*uipSemaphoreAdd=NextCore;
				/*write back data from cache to descriptor RAM*/
				//WritebackCache((void *)(uipSemaphoreAdd), 4);

				CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
				// Unlock Config
				KICK0 = KICK0_UNLOCK;
				KICK1 = KICK1_UNLOCK;

				*(volatile Uint32 *) iIPCGRInfo[NextCore] = interruptAct;

		   		*(volatile Uint32 *) iIPCGRInfo[NextCore] |= 1;

				// lock Config
				KICK0 = KICK_LOCK;
				KICK1 = KICK_LOCK;
				
			}	
			if(CoreNum1==7)
			{
				Lednum=4;
				circles =10;
				NextCore = 1;
				

				//*uipSemaphoreAdd=NextCore;
				/*write back data from cache to descriptor RAM*/
				//WritebackCache((void *)(uipSemaphoreAdd), 4);

				CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
				// Unlock Config
				KICK0 = KICK0_UNLOCK;
				KICK1 = KICK1_UNLOCK;

				*(volatile Uint32 *) iIPCGRInfo[NextCore] = interruptAct;

		   		*(volatile Uint32 *) iIPCGRInfo[NextCore] |= 1;

				// lock Config
				KICK0 = KICK_LOCK;
				KICK1 = KICK_LOCK;
				
			}			
		}			
	}
	else
	{
		//for 319 project debug,已经验证OK
		printf("Finished111:IPC_ACT_ISR CoreNum%d\n",CoreNum);

		/*for 319 project debug,semaphore operation:比如某些动作需要信号量操作*/
		if(read_ipcgr==16)
		{			
			/* Check for the semaphore you want to use is available or not */
        	if (CSL_semIsFree(SEMAPHORE_NUM0) != 1)
			{
				printf("Core %d Error: The Semaphore %d is not free\n", DNUM, SEMAPHORE_NUM0);
				CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
			}
			while(0==CSL_semAcquireDirect(SEMAPHORE_NUM0));
			/*获得信号量SEMAPHORE_NUM0的core*/
			//result = CSL_semGetResourceOwner(SEMAPHORE_NUM0);

			uipSemaphoreAdd=(Uint32 *)(0x0C2f8000+16*4);
			/*invalid cache before read descriptor RAM*/
			InvalidCache((void *)(uipSemaphoreAdd), 4);

			*uipSemaphoreAdd=CoreNum;
			
			/*write back data from cache to descriptor RAM*/
			WritebackCache((void *)(uipSemaphoreAdd), 4);

			CSL_semReleaseSemaphore(SEMAPHORE_NUM0);
			// Unlock Config
			KICK0 = KICK0_UNLOCK;
			KICK1 = KICK1_UNLOCK;

			*(volatile Uint32 *) iIPCGRInfo[0] = interruptAct;

	   		*(volatile Uint32 *) iIPCGRInfo[0] |= 1;//Creates an Inter-DSP interrupt

			// lock Config
			KICK0 = KICK_LOCK;
			KICK1 = KICK_LOCK;				
		}
    }
	
	//for 319 project debug,已经验证OK
	//printf("Finished:IPC_ACT_ISR CoreNum%d\n",CoreNum);
}


volatile Uint32 DoorbellNum=0;
volatile Uint32 DoorbellNum1=0;
volatile Uint32 DoorbellNum2=0;
volatile Uint32 DoorbellNum3=0;
volatile Uint32 DoorbellNum4=0;


/*****************************************************************************
 Prototype    : SRIO_Doorbell_ISR
 Description  : Interrupt service routine for timer
 Input        : void  
 Output       : None
 Return Value : interrupt
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
interrupt void SRIO_Doorbell_ISR()
{
	Uint32 doorbell;
	Int16 i;
	DMA_MEM_Tran_Config   DMA_transfer_cfg;
	unsigned int imageadd;
	
	//read doorbell. this test only use doorbell reg 0
	doorbell= gpSRIO_regs->DOORBELL_ICSR_ICCR[0].RIO_DOORBELL_ICSR;

	//clear doorbell interrupt
	gpSRIO_regs->DOORBELL_ICSR_ICCR[0].RIO_DOORBELL_ICCR= doorbell; 

	SRIO_nodoorbellcount=0;//清除没有接收到SRIO的次数
	/*获得pbuf数据传递一个640*512的数组*/			
	DMA_transfer_cfg.A_B_CNT=(320<<16)|(4&0xFFFF);//
	DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
	DMA_transfer_cfg.SRC_DST_CIDX=(1280<<16)|(1280);//每个像元4个字节
	DMA_transfer_cfg.CCNT=512;	
	imageadd = 0x83000000+(DoorbellNum%1000)*655360;//记录当前图像信息

	switch (doorbell)
	{
		case 2:
        {
            //接收SRIO报文
            //接收数据，地址为0x80000000            
			//for 319 project debug :临时使用DDR3_DATA空间，最终按照需求使用
			//pucSrioData = (Uint32 *)(0x80000000);
			DoorbellNum1++;

			/*使用EDMA_CC0_CH1,传递一个640*512的数组*/			
			EDMA_copy_Type3(0x80000000, 0x0c040000, 
				1, &DMA_transfer_cfg, 0);

			//高低温调试使用
			SRIO_temperatureflag1=1;

			//20171123调试临时使用//记录当前图像信息
			//EDMA_copy_Type3(0x80000000, imageadd, 
				//0, &DMA_transfer_cfg, 0);
			
			break;
        }
		case 4:
        {
            //接收数据，地址为0x80600000
			//for 319 project debug :临时使用DDR3_DATA空间，最终按照需求使用
			//pucSrioData = (Uint32 *)(0x80600000);
			DoorbellNum2++;

			
			/*获得pbuf数据传递一个640*512的数组*/			
			EDMA_copy_Type3(0x80600000, 0x0c040000,
				1, &DMA_transfer_cfg, 0);

			
			//高低温调试使用
			SRIO_temperatureflag2=1;

			//20171123调试临时使用//记录当前图像信息
			//EDMA_copy_Type3(0x80600000, imageadd, 
				//0, &DMA_transfer_cfg, 0);
			
            break;
        }		
		case 6:
        {			
            DoorbellNum3++;
				break;
        }		
		default:
        {      
			DoorbellNum4++;
			break;
        }	
	}
	DoorbellNum++;
	//获取上报目标信息
	/*连续2次以上没有获取到目标，不上报*/

	if((CommandMode>Command_Scan)&&(ScanCheck5Num==0))
	{
		if(targetempty<5)		
			targetempty++;
		else
			ScanCheck5lastNum=ScanCheck5Num;
	}	
	else
	{
		targetempty=0;
		ScanCheck5lastNum=ScanCheck5Num;
		//发送目标信息			
		uiXaddlast = uiXadd;
		uiYaddlast = uiYadd;


		//20171123debug调试使用
		FPGAChecklast.col=   FPGACheck.col;
		FPGAChecklast.row  = FPGACheck.row;  
		FPGAChecklast.cnt  = FPGACheck.cnt;  
		FPGAChecklast.gray = FPGACheck.gray;
		for(i=0;i<ScanCheck5lastNum;i++)
		{		
			ScanCheck5last[i].col=ScanCheck5[i].col;
			ScanCheck5last[i].row=ScanCheck5[i].row;
			ScanCheck5last[i].cnt=ScanCheck5[i].cnt;
			ScanCheck5last[i].gray=ScanCheck5[i].gray;
		}
	}

	RxFrameFlag=1;

}

/*****************************************************************************
 Prototype    : HyperLinkISR
 Description  : Interrupt service routine for HyperLink
 Input        : void  
 Output       : None
 Return Value : interrupt
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 

*****************************************************************************/
interrupt void HyperLinkISR(void)
{
	/* Disable the host interrupt */
	gpCIC0_regs->HINT_ENABLE_CLR_INDEX_REG = gCIC_out_num22;

	if(gpCIC0_regs->ENA_STATUS_REG[CSL_INTC0_VUSR_INT_O/32]&(1<<(CSL_INTC0_VUSR_INT_O%32)))
	{
		/* Clear HyperLink Interrupt events (111) in INTC*/
		KeyStone_CIC_clear_system_event(gpCIC_regs, CSL_INTC0_VUSR_INT_O);

		HyperLink_Int_Handle();
	}
	/* Enable the host interrupt */
	gpCIC0_regs->HINT_ENABLE_SET_INDEX_REG = gCIC_out_num22;
}


/*****************************************************************************
 Prototype    : GPIO_ISR
 Description  : Interrupt service routine for GPIO
 Input        : None
 Output       : None
 Return Value : interrupt
 
  History        :
  1.Date         : 2015/10/29
    Author       : 
    Modification : Created function

  2.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Modified  

    use gpio[8:15]
*****************************************************************************/
Uint32 GPIO_NUM0=0;
Uint32 GPIO_NUM1=0;
Uint32 GPIO_NUM2=0;
Uint32 GPIO_NUM3=0;
Uint32 GPIO_NUM4=0;
Uint32 GPIO_NUM5=0;

interrupt void GPIO_ISR()
{
	Uint32 eventFlag;

	/*read interrupt flag*/
	eventFlag= gpCGEM_regs->MEVTFLAG[2];

	/*clear interrupt flag*/
	gpCGEM_regs->EVTCLR[2]= eventFlag;

	GPIO_NUM0++;	

	/*GPIO 中断*/
	switch (eventFlag)
	{
		case GPIO8_eventFlag_ISR:
        {
            /*GPIO 8中断:FPGA转发中控信息*/
			if(temperature_testflag==0)
			{
				//用于整机测试
				//FPGA_EMIF16_RTX();
				//用于图像板(非整机)测试代码
				FPGA_EMIF16_RTX2();
				GPIO_NUM1++;
				
			}
			else
			{
				//20170502 debug
				GPIO_NUM1++;
				if(GPIO_NUM1>5)
				{
					//20170605高低温测试使用
					GPIO_temperature_test();			
				}
			}	
			
			
			break;
        }
		case GPIO9_eventFlag_ISR:
        {
            /*GPIO 9中断*/
			if(temperature_testflag==0)
			{				
				GPIO_NUM2++;
			}
            break;
        }	
		case GPIO89_eventFlag_ISR:
        {
            /*GPIO 8中断*/
			if(temperature_testflag==0)
			{
				GPIO_NUM3++;
			}
			else
			{
				GPIO_temperature_test();
				GPIO_NUM3++;
			}
			break;
        }
		case GPIO10_eventFlag_ISR:
        {
            /*GPIO 10中断*/
			//20170605高低温测试使用
			DDR3_temperatureflag1=1;
			//DDR3_temperature_test();
			GPIO_NUM4++;
			break;
        }		
		default:        
            //printf("GPIO15_default_ISR \r\n");
			GPIO_NUM5++;
			break;
        	
	}	
}


