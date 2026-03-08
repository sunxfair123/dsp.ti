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
  File Name     : dsp0_test.c
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
    
    ±¾¹¤³ÌÎªdebug¹¤³Ì,µ÷ÊÔÍê³Éºó×¢ÊÍµôËùÓĞ////for 319 project debug
******************************************************************************/
#include <stdio.h>
#include <string.h>
//
//#include "DDR_init.h"
//#include "Robust_MNav_Init.h"
//#include "SRIO_Init.h"
//#include "KeyStone_HyperLink_Init.h"
//#include "interrupt.h"
//#include "Robust_watch_dog.h"
//#include "Robust_memory_transfer.h" //add by sxf 20160108
//#include "peripheral.h"
//#include "UART_Init_drv.h"
//#include "Robust_MSU.h"//add by sxf 20160108
//#include "csl_semAux.h"

//#include "Hook.h"

/*for 319 project debug :add by sxf 20160108ÓÃÓÚled flash*/
//#include "platform.h" 

//#include <cerrno> 
//#include <stdlib.h>


//#include "resource_mgr.h"
#include "dsp0_common.h"
#include "dsp0_image.h"





#define DDR_INIT_MAX_RETRY_NUM 	10






	
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
 Prototype	  : Memory_Data_DDR3_OneCircle_Test
 Description  : this function test the data bus to detect bit stuck, and
				interference between bits.
				ÓÃÓÚ²âÊÔ8Bytes³¤¶È
 Input		  : unsigned int uiBaseAddress 
				unsigned int uiBusWidth, number of bits of the data bus.
 Output 	  : None
 Return Value : fail count
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/
unsigned int Memory_Data_DDR3_OneCircle_Test(unsigned int uiBaseAddress,
	unsigned int uiNumBytes)
{
	int i, j;
	unsigned int uiBusWidth,uiNum8Bytes;
	unsigned int uiNumBitShift, uiBusWidth_64bit;
	unsigned int uiFailCount, oldMAR, uiMAR_index;
	volatile unsigned long long *ulpAddressPointer;
	volatile unsigned long long ulReadBack;
	unsigned long long ulBitMask;
	unsigned long long ulDataPattern[8]={0x5a5a5a5a5a5a5a5a,0xaaaaaaaaaaaaaaaa, 
		0xcccccccccccccccc, 0xf0f0f0f0f0f0f0f0, 0xff00ff00ff00ff00, 
		0xffff0000ffff0000, 0xffffffff00000000, 0};

	uiBusWidth=64;
	uiBusWidth_64bit= (uiBusWidth+63)/64; //bus width in number of 64-bit

	/*cache should be disabled for this test, otherwise, the write and 
	read back data may stay in the cache, and can not test real bus*/
	uiMAR_index= uiBaseAddress/16/1024/1024;
	oldMAR= gpCGEM_regs->MAR[uiMAR_index]; //save MAR
	if(oldMAR)
	{
		gpCGEM_regs->MAR[uiMAR_index]=0; //disable cache
		WritebackInvalidAllCache((void *)uiBaseAddress);
	}
	
	ulpAddressPointer = (unsigned long long *)uiBaseAddress;

	uiFailCount=0;

	uiNum8Bytes = uiNumBytes/8;
	/*----------write/readback pattern to detect stuck bit-----------*/
	//²âÊÔ8×éÊı¾İ,Õû¸öDDR3²âÊÔ16´Î
	//for (i = 0; i < 8; i++)
	//²âÊÔ1×éÊı¾İ,Õû¸öDDR3²âÊÔ2´Î
	for (i = 0; i < 1; i++)	
	{
		ulpAddressPointer = (unsigned long long *)uiBaseAddress;
		for(j=0; j<uiNum8Bytes; j++)
		{
			//write a pattern
			*ulpAddressPointer = ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
			//ulpAddressPointer[uiNum8Bytes]= ~ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
			if (ulReadBack != ulDataPattern[i])
			{
				//printf(" Data pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (ulDataPattern[i]), ulReadBack);
				uiFailCount++;
			}				

			//write the inversion of the pattern
			*ulpAddressPointer = ~ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
			//±ÈÈçÕû¸ö¿Õ¼äuiNum8Bytes×î´óÎª8£ulpAddressPointer[uiNum8Bytes]¬
			//×î´ó¿ÉÒÔÊ¹ÓÃµÄÊı×éÎª7,Ê¹ÓÃ8»áÔ½½çÒç³ö
			//ulpAddressPointer[uiNum8Bytes]= ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
			if (ulReadBack != (~ulDataPattern[i]))
			{
				//printf(" Data inversion pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (~ulDataPattern[i]), ulReadBack);
				uiFailCount++;
			}

			ulpAddressPointer++;
		}
	}

	if(oldMAR)
		gpCGEM_regs->MAR[uiMAR_index]= oldMAR; //restore MAR

	return uiFailCount;
}
/*****************************************************************************
 Prototype    : Memory_Data_DDR3_OneCircle_WTest
 Description  : ÓÃÓÚ²âÊÔ4Bytes³¤¶È
 Input        : unsigned int uiBaseAddress 
                unsigned int uiBusWidth, number of bits of the data bus.
 Output       : None
 Return Value : fail count
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/
unsigned int Memory_Data_DDR3_OneCircle_WTest(unsigned int uiBaseAddress,
	unsigned int uiNumBytes)
{
    int i, j;
	unsigned int uiBusWidth,uiNum4Bytes;
    unsigned int uiNumBitShift, uiBusWidth_64bit;
    unsigned int uiFailCount, oldMAR, uiMAR_index;
    volatile unsigned int *uipAddressPointer;
    volatile unsigned int ulReadBack;
    unsigned long long ulBitMask;
    unsigned int ulDataPattern[8]={0x5a5a5a5a,0xaaaaaaaa, 
    	0xcccccccc, 0xf0f0f0f0, 0xff00ff00, 
    	0xffff0000, 0xffff0000, 0};

	uiBusWidth=64;
    uiBusWidth_64bit= (uiBusWidth+63)/64; //bus width in number of 64-bit

    /*cache should be disabled for this test, otherwise, the write and 
    read back data may stay in the cache, and can not test real bus*/
	uiMAR_index= uiBaseAddress/16/1024/1024;
	oldMAR= gpCGEM_regs->MAR[uiMAR_index]; //save MAR
	if(oldMAR)
	{
		gpCGEM_regs->MAR[uiMAR_index]=0; //disable cache
		WritebackInvalidAllCache((void *)uiBaseAddress);
	}
	
	uipAddressPointer = (unsigned int *)uiBaseAddress;

	uiFailCount=0;

	uiNum4Bytes = uiNumBytes/4;
	/*----------write/readback pattern to detect stuck bit-----------*/
	//²âÊÔ8×éÊı¾İ,Õû¸öDDR3²âÊÔ8´Î
    for (i = 0; i < 8; i++)
    {
		uipAddressPointer = (unsigned int *)uiBaseAddress;
		for(j=0; j<uiNum4Bytes; j++)
        {
	        //write a pattern
	        *uipAddressPointer = ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //uipAddressPointer[uiNum4Bytes]= ~ulDataPattern[i]; //avoid floating bus
			ulReadBack= *uipAddressPointer;
	        if (ulReadBack != ulDataPattern[i])
			{
				printf(" Data pattern Test fails at 0x%8x, Write 0x%8x, Readback 0x%8x\n", (unsigned int)uipAddressPointer, (ulDataPattern[i]), ulReadBack);
	            uiFailCount++;
			}	            

	        //write the inversion of the pattern
	        *uipAddressPointer = ~ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //uipAddressPointer[uiNum4Bytes]= ulDataPattern[i]; //avoid floating bus
			ulReadBack= *uipAddressPointer;
	        if (ulReadBack != (~ulDataPattern[i]))
			{
				printf(" Data inversion pattern Test fails at 0x%8x, Write 0x%8x, Readback 0x%8x\n", (unsigned int)uipAddressPointer, (~ulDataPattern[i]), ulReadBack);
	            uiFailCount++;
			}

			uipAddressPointer++;
		}
    }

	if(oldMAR)
		gpCGEM_regs->MAR[uiMAR_index]= oldMAR; //restore MAR

    return uiFailCount;
}
/*****************************************************************************
 Prototype    : Memory_Data_DDR3_Test
 Description  : this function test the data bus to detect bit stuck, and
                interference between bits.
                ÓÃÓÚ²âÊÔ8Bytes³¤¶È
 Input        : unsigned int uiBaseAddress 
                unsigned int uiBusWidth, number of bits of the data bus.
 Output       : None
 Return Value : fail count
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/
unsigned int Memory_Data_DDR3_Test(unsigned int uiBaseAddress,
	unsigned int uiNumBytes)
{
    int i, j;
	unsigned int uiBusWidth,uiNum8Bytes;
    unsigned int uiNumBitShift, uiBusWidth_64bit;
    unsigned int uiFailCount, oldMAR, uiMAR_index;
    volatile unsigned long long *ulpAddressPointer;
    volatile unsigned long long ulReadBack;
    unsigned long long ulBitMask;
    unsigned long long ulDataPattern[8]={0xaaaaaaaaaaaaaaaa,0x5a5a5a5a5a5a5a5a, 
    	0xcccccccccccccccc, 0xf0f0f0f0f0f0f0f0, 0xff00ff00ff00ff00, 
    	0xffff0000ffff0000, 0xffffffff00000000, 0};

	uiBusWidth=64;
    uiBusWidth_64bit= (uiBusWidth+63)/64; //bus width in number of 64-bit

    /*cache should be disabled for this test, otherwise, the write and 
    read back data may stay in the cache, and can not test real bus*/
	uiMAR_index= uiBaseAddress/16/1024/1024;
	oldMAR= gpCGEM_regs->MAR[uiMAR_index]; //save MAR
	if(oldMAR)
	{
		gpCGEM_regs->MAR[uiMAR_index]=0; //disable cache
		WritebackInvalidAllCache((void *)uiBaseAddress);
	}
	
	ulpAddressPointer = (unsigned long long *)uiBaseAddress;

	uiFailCount=0;

	uiNum8Bytes = uiNumBytes/8;
	/*----------write/readback pattern to detect stuck bit-----------*/
	//²âÊÔ8×éÊı¾İ,Õû¸öDDR3²âÊÔ8´Î
    for (i = 0; i < 8; i++)
    {
		ulpAddressPointer = (unsigned long long *)uiBaseAddress;
		for(j=0; j<uiNum8Bytes; j++)
        {
	        //write a pattern
	        *ulpAddressPointer = ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //ulpAddressPointer[uiNum8Bytes]= ~ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
	        if (ulReadBack != ulDataPattern[i])
			{
				printf(" Data pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (ulDataPattern[i]), ulReadBack);
	            uiFailCount++;
			}	            

	        //write the inversion of the pattern
	        *ulpAddressPointer = ~ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //ulpAddressPointer[uiNum8Bytes]= ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
	        if (ulReadBack != (~ulDataPattern[i]))
			{
				printf(" Data inversion pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (~ulDataPattern[i]), ulReadBack);
	            uiFailCount++;
			}

			ulpAddressPointer++;
		}
    }

	/*-------bit walking to detect interference and stuck betweeen bits--------*/
	//²âÊÔ64×éÊı¾İ,Õû¸öDDR3²âÊÔ64´Î
    ulBitMask = 1;
	uiNumBitShift= uiBusWidth<64?uiBusWidth:64;
    for (i = 0; i < uiNumBitShift; i++)
    {
        ulpAddressPointer = (unsigned long long *)uiBaseAddress;
		for(j=0; j<uiNum8Bytes; j++)
        {
	        // Test "1" in bit position i
	        *ulpAddressPointer = ulBitMask;
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //ulpAddressPointer[uiNum8Bytes]= ~ulBitMask; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
	        if (ulReadBack != ulBitMask)
			{
				printf(" Data bit 1 walking Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (ulBitMask), ulReadBack);
	            uiFailCount++;
			}	            

	        // Test "0" in bit position i
	        *ulpAddressPointer = ~ulBitMask;
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
	        //ulpAddressPointer[uiNum8Bytes]= ulBitMask; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
	        if (ulReadBack != (~ulBitMask))
			{
				printf(" Data bit 0 walking Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (~ulBitMask), ulReadBack);
	            uiFailCount++;
			}

			ulpAddressPointer++;
		}

		ulBitMask = ulBitMask << 1;
    }

	if(oldMAR)
		gpCGEM_regs->MAR[uiMAR_index]= oldMAR; //restore MAR

    return uiFailCount;
}


volatile Uint8 SRIO_temperatureflag1=0;
volatile Uint8 SRIO_temperatureflag2=0;
volatile Uint8 DDR3_temperatureflag1=0;

volatile Uint8 SRIOflag1=0;
volatile Uint8 SRIOflag2=0;

volatile Uint8 DDR3_temperatureflag=1;
volatile Uint8 GPIO_temperatureflag=0;

//ÓÃÓÚÃ»ÓĞ½ÓÊÕµ½SRIOÊı¾İ
volatile Uint32 SRIO_nodoorbellcount=0;






	



unsigned int uiFailCount0=0;
unsigned int uiFailCount1=0;

	
/*****************************************************************************
 Prototype	  : Memory_Data_DDR3_OneCircle_Test
 Description  : this function test the data bus to detect bit stuck, and
				interference between bits.
 Input		  : unsigned int uiBaseAddress 
				unsigned int uiBusWidth, number of bits of the data bus.
 Output 	  : None
 Return Value : fail count
 
  History		:
   1.Date		  : 2017-02-13
   Author		  : 
   Modification   : Created function 


*****************************************************************************/
unsigned int Memory_Data_DDR3_OneCircle_Temperature(unsigned int uiBaseAddress,
	unsigned int uiNumBytes)
{
	int i, j;
	unsigned int uiBusWidth,uiNum8Bytes;
	unsigned int uiNumBitShift, uiBusWidth_64bit;
	unsigned int uiFailCount, oldMAR, uiMAR_index;
	volatile unsigned long long *ulpAddressPointer;
	volatile unsigned long long ulReadBack;
	unsigned long long ulBitMask;
	unsigned long long ulDataPattern[8]={0x5a5a5a5a5a5a5a5a,0xaaaaaaaaaaaaaaaa, 
		0xcccccccccccccccc, 0xf0f0f0f0f0f0f0f0, 0xff00ff00ff00ff00, 
		0xffff0000ffff0000, 0xffffffff00000000, 0};

	uiBusWidth=64;
	uiBusWidth_64bit= (uiBusWidth+63)/64; //bus width in number of 64-bit

	/*cache should be disabled for this test, otherwise, the write and 
	read back data may stay in the cache, and can not test real bus*/
	uiMAR_index= uiBaseAddress/16/1024/1024;
	oldMAR= gpCGEM_regs->MAR[uiMAR_index]; //save MAR
	if(oldMAR)
	{
		gpCGEM_regs->MAR[uiMAR_index]=0; //disable cache
		WritebackInvalidAllCache((void *)uiBaseAddress);
	}
	
	ulpAddressPointer = (unsigned long long *)uiBaseAddress;

	uiFailCount=0;

	uiNum8Bytes = uiNumBytes/8;
	/*----------write/readback pattern to detect stuck bit-----------*/
	//²âÊÔ1×éÊı¾İ,Õû¸öDDR3²âÊÔ1´Î
	for (i = 0; i < 1; i++)
	{
		ulpAddressPointer = (unsigned long long *)uiBaseAddress;
		for(j=0; j<uiNum8Bytes; j++)
		{
			//write a pattern
			*ulpAddressPointer = ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
			//ulpAddressPointer[uiNum8Bytes]= ~ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
			if (ulReadBack != ulDataPattern[i])
			{
				//printf(" Data pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (ulDataPattern[i]), ulReadBack);
				uiFailCount0++;
			}				

			//write the inversion of the pattern
			*ulpAddressPointer = ~ulDataPattern[i];
			//Õû¸öDDR²âÊÔ£¬²»ÄÜÊ¹ÓÃÏÂÃæµÄ£¬»áÔ½½çÒç³ö
			//ulpAddressPointer[uiNum8Bytes]= ulDataPattern[i]; //avoid floating bus
			ulReadBack= *ulpAddressPointer;
			if (ulReadBack != (~ulDataPattern[i]))
			{
				//printf(" Data inversion pattern Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", (unsigned int)ulpAddressPointer, (~ulDataPattern[i]), ulReadBack);
				uiFailCount0++;
			}

			ulpAddressPointer++;
		}
	}

	if(oldMAR)
		gpCGEM_regs->MAR[uiMAR_index]= oldMAR; //restore MAR

	return uiFailCount;
}



volatile Int16 RxDatafpga;

void FPGA_EMIF16_temperature()
{	
	//Int16 TargetNum;
	//Int16 Inquirycode;
	//Int16 *pTxadd;
	//Int16 *pRxadd;
	//Int32 RXangle;
	//Int32 RYangle;
	//Int32 TXangle;
	//Int32 TYangle;
	//Int16 RxData;
	
	//rtxnumber++;
	//pRxadd=(Int16 *)dsp0_EMIF16_RxAdd;
	//pTxadd=(Int16 *)dsp0_EMIF16_TxAdd;

				
	//¶ÁÈ¡¿ØÖÆÃüÁî
	//CommandMode = *(pRxadd+0);
	//Í¨ĞÅÖÊÑ¯»Ø¸´
	//*(pTxadd+45) = *(pRxadd+3);
	RxDatafpga = ((*(Int16 *)(0x70000006))&0x00ff);
	*(Int16 *)(0x70000182)= RxDatafpga;

	//ÎÕÊÖ±êÊ¶Î»
	*(Int16 *)(0x70000186) = 0;//add 20170830
	*(Int16 *)(0x70000184) = 0x01;//ÎÕÊÖ±êÊ¶Î»		
	
	
}

void GPIO_temperature_test()
{
	// 8--GPIO 7--SRIO 6--ddr3 
	//Ë«¿ÚramµØÖ·0xf5	
	if(DDR3_temperatureflag==1)
	{
		//Temperature_test_send(8,1);
		*(Int16 *)(0x700001EA)=0x5a;
	}
	else
	{
		
		//Temperature_test_send(8,0);
		*(Int16 *)(0x700001EA)=0xb6;
	}	

	//SRIO DOORBELL temperature²âÊÔ	//Ë«¿ÚramµØÖ·0xf6	
	SRIO_nodoorbellcount++;
	//Á¬Ğø2´Î£¬ ²»ÔÙÊÕµ½doorbell
	if(SRIO_nodoorbellcount>2)
	{
		SRIOflag1=1;
		SRIOflag2=1;
	}
	//½á¹û´íÎó
	if((SRIOflag1==1)||(SRIOflag2==1))
	{
		//Temperature_test_send(7,0);
		*(Int16 *)(0x700001EC)=0xb6;
	}
	else
	{
		//Temperature_test_send(7,1);	
		*(Int16 *)(0x700001EC)=0x5a;
	}
	SRIOflag1=0;
	SRIOflag2=0;

	//GPIO ²âÊÔ:Ë«¿ÚramµØÖ·0xf7
	GPIO_temperatureflag=1;
	 
	if(GPIO_temperatureflag==1)
	{
		*(Int16 *)(0x700001EE)=0x5a;//½á¹ûÕıÈ·
	}
	else
	{
		//Temperature_test_send(6,0);
		*(Int16 *)(0x700001EE)=0xb6;
	}
	
	

	//20170830 delete :FPGAÖ»ĞèÒªÒ»´Î¸üĞÂÍ¨Öª
	*(Int16 *)0x700001FA=0x1;//Í¨ÖªFPGA¸üĞÂÏÔÊ¾ĞÅÏ¢

	FPGA_EMIF16_temperature();
		
}


//Ê¹ÓÃGPIO10ÖĞ¶Ï,30s FPGA·¢ËÍÒ»´Î
void DDR3_temperature_test()
{
	Uint32 uiRetryCount=0, uiFailCount=0;	
	
	uiFailCount += Memory_Data_DDR3_OneCircle_Temperature(0x90000000, 64);
	if(0==uiFailCount)
	{
		DDR3_temperatureflag=1;
		
	}
	else
	{
		DDR3_temperatureflag=0;
		
	}
		
}


void SRIO_temperature_test()
{
	Uint32 i,j;	
	Int16* pucSrioData1;
	Int16* pucSrioData2;
	Int16  ucSrioData1;
	Int16  ucSrioData2;
	Uint32 flag;
	//ÓÉÓÚFPGA²úÉúµÄÊı¾İĞÅºÅµÚÒ»ĞĞ¿ÉÄÜ³ö´í£¬Ìø¹ıµÚÒ»ĞĞ
	pucSrioData1 = (Int16 *)(0x80000000+0x500);
	pucSrioData2 = (Int16 *)(0x80600000+0x500);

	ucSrioData1 = *pucSrioData1;
	ucSrioData2 = *pucSrioData2;

	
	
	if(SRIO_temperatureflag1==1)
	{
		//½øĞĞÊı¾İ¼ì²â
		SRIO_temperatureflag1=0;
		flag=0;
		for(j=1;j<512;j++)
		{
			
			for(i=0;i<640;i++)
			{
				if(*pucSrioData1!=((i+ucSrioData1)%640))
				{
					//´íÎóĞÅºÅ
					SRIOflag1=1;
				}
				pucSrioData1++;
			}
		}
		
		
	}


	if(SRIO_temperatureflag2==1)
	{
		flag=0;
		//½øĞĞÊı¾İ¼ì²â
		SRIO_temperatureflag2=0;
		for(j=1;j<512;j++)
		{			
			for(i=0;i<640;i++)
			{
				if(*pucSrioData2!=((i+ucSrioData2)%640))
				{
					//´íÎóĞÅºÅ
					SRIOflag2=1;
				}
				pucSrioData2++;
			}
		}
	
	}

	

	if(DDR3_temperatureflag1==1)
	{
		DDR3_temperatureflag1=0;
		DDR3_temperature_test();	
	}
}


Int16 em16=0x5a;
void FPGA_EMIF16_RTXtest()
{	
	Int16 TargetNum;
	Int16 Inquirycode;
	Int16 *pTxadd;
	Int16 *pRxadd;
	Int16 rxdata[8];
	Uint32 i;

	pRxadd=(Int16 *)dsp0_EMIF16_RxAdd;
	pTxadd=(Int16 *)dsp0_EMIF16_TxAdd;

	TargetNum=1;
	
#if 0
	
	//¶ÁÈ¡Í¨ĞÅÖÊÑ¯
	*(pTxadd+3) = Inquirycode;
	Inquirycode +=1;
	
	//Ä¿±ê¸öÊı£¨8bit£©
	*(pTxadd+2) = TargetNum;
	*(Int16 *)dsp0_EMIF16_Inquiry_Check=Inquirycode;

	//image_2fpga_t
	//Í¨ĞÅÏÔÊ¾ĞÅÏ¢
	uiYadd++;
	uiXadd++;
	uiYadd=uiYadd%512;
	uiXadd=uiXadd%640;
	dsp0_updateflag++;
	
	*(Int16 *)(dsp0_EMIF16_TVAdd +0)= uiYadd;// 512 £¬ÒÔÍ¼Ïñ×óÉÏ½ÇÎª£¨ 0,0£©
	*(Int16 *)(dsp0_EMIF16_TVAdd +1)= uiXadd;//640 £¬ÒÔÍ¼Ïñ×óÉÏ½ÇÎª£¨ 0,0£©
	*(Int16 *)(dsp0_EMIF16_upflagAdd) = dsp0_updateflag;
#endif
	//µ÷ÊÔ¶ÁÈ¡FPGA 8×éÊı¾İ

	for(i=0;i<8;i++)
	{
		rxdata[i]=*pRxadd;
		pRxadd++;
		rxdata[i]++;
	}

	for(i=0;i<8;i++)
	{
		*pTxadd=rxdata[i];
		pTxadd++;
	}
	em16++;
	*(Int16 *)0x700001FC=em16;//GPIOºÍFPGAÊ¹ÓÃÎÕÊÖ
	

}


void GPIO_test()
{
	int i;
	volatile Uint32 PID;

	/*toggle all GPIOs*/
	for(i=8; i<16; i++)
	{
		gpGPIO_regs->BANK_REGISTERS[0].SET_DATA= (1<<i);
		PID= gpGPIO_regs->PID; //read to force write out
		TSC_delay_ms(1);
		//check_GPIO();

		gpGPIO_regs->BANK_REGISTERS[0].CLR_DATA= (1<<i);
		PID= gpGPIO_regs->PID; //read to force write out
		TSC_delay_ms(1);
		//check_GPIO();
	}
}

//²âÊÔ1GB¿Õ¼ä,²âÊÔdsp¹Ò½ÓµÄËùÓĞDDR3¿Õ¼ä
void DDR3_test()
{
	Uint32 uiRetryCount=0, uiFailCount=0;
	
	while(DDR_INIT_MAX_RETRY_NUM>uiRetryCount)
	{
		//²âÊÔ1GB¿Õ¼ä
		//uiFailCount += Memory_Data_DDR3_Test(0x80000000, 0x40000000);//²âÊÔDDR3·Ç³£³ä·Ö		

		uiFailCount += Memory_Data_DDR3_OneCircle_Test(0x80000000, 0x40000000);

		//uiFailCount += Memory_Data_DDR3_OneCircle_WTest(0x80000000, 0x40000000);
		
		printf("DDR DDR3_test OK %d\n", uiRetryCount);
		
		if(0==uiFailCount)
			return;
			
		uiRetryCount++;
		printf("DDR DDR3_test %d\n", uiFailCount);
	}
}

