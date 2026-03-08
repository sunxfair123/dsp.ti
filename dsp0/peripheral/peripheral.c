/****************************************************************************\
 *           Copyright (C) 2012 Texas Instruments Incorporated.             *
 *                           All Rights Reserved                            *
 *                                                                          *
 * GENERAL DISCLAIMER                                                       *
 * ------------------                                                       *
 * All software and related documentation is provided "AS IS" and without   *
 * warranty or support of any kind and Texas Instruments expressly disclaims*
 * all other warranties, express or implied, including, but not limited to, *
 * the implied warranties of merchantability and fitness for a particular   *
 * purpose.  Under no circumstances shall Texas Instruments be liable for   *
 * any incidental, special or consequential damages that result from the    *
 * use or inability to use the software or related documentation, even if   *
 * Texas Instruments has been advised of the liability.     
 * Peripheral_interface used for core 0
 ****************************************************************************
This example shows the features of GPIO on KeyStone DSP
 ****************************************************************************
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
***************************************************************************/
#include <c6x.h>
#include <stdio.h>
#include "dsp0_common.h"
#include "EMIF16_Init.h"


#include <cslr_gpio.h>
#include <csl_bootcfgAux.h>

#include "peripheral.h"





Uint32 num_GPIO=16;
//TDSP_Board_Type DSP_Board_Type;

/*EMIF*/
EMIF16_CE_Config gNorCeCfg;	/*the configuration for the CE of NOR FLASH*/
/* for 319 project debug :remove Nand mode*/
//EMIF16_CE_Config gNandCeCfg;/*the configuration for the CE of NAND FLASH*/
EMIF16_Config gEmif16Cfg;

NOR_InfoObj gNorInfo =
{
	EMIF16_CE1_BaseAddr,/*flashBase;                  */  
	FLASH_BUS_WIDTH_2_BYTES,/*busWidth;                   */  
	FLASH_BUS_WIDTH_2_BYTES,/*chipOperatingWidth;         */  
	FLASH_BUS_WIDTH_2_BYTES/*maxTotalWidth;              */  
};

//for 319 project debug:用于EMIF16调试，调试完后去掉
/*add by sxf 20150119 EMIF16 debug */
#define FLASH_TEST_BUFFER_SIZE 	(64)
#pragma DATA_SECTION(testBuf,".far:testBuf")
Uint16	testBuf[FLASH_TEST_BUFFER_SIZE];
#pragma DATA_SECTION(testReadBuf,".far:testBuf")
Uint16	testReadBuf[FLASH_TEST_BUFFER_SIZE];

unsigned short uiDataPatternTable[] = { 0x7777, 0x1fff, 0xaaaa, 0x5555 };





/*Please note, dual Nyquist EVM only connects port 2 and 3 between 2 DSPs.*/
SRIO_Multiple_Test_Config test_2DSP_cfg=
{
    SRIO_PATH_CTL_4xLaneABCD, /*multiple_port_path*/
    /*packet_type               source                                                   dest                                                size*/
    {{SRIO_PKT_TYPE_SWRITE,     0x73000000,     0x74000000,             256},   /*port0 is not availible for this case*/
    {0,                         0,                                                      0,                                                   0},   /*port1 is not availible for this case*/
    {0,                         0,                                                      0,                                                   0},   /*port2*/
    {0,                         0,                                                      0,                                                   0}}    /*port3*/
};


/*****************************************************************************
 Prototype    : DSP0_GPIO_init
 Description  : GPIO_init
 Input        :                 
 Output       : 
 Return Value : 
 
 History        :
  1.Date         : 2017-02-13
    Author       : 
    Modification : Created function   

*****************************************************************************/
void DSP0_GPIO_init()
{
	/*set GPIO direction*/
	//1:GPn pin is configured as input pin
	//all gpio pin设置为输入
	gpGPIO_regs->BANK_REGISTERS[0].DIR= 0xFFFFFFFF;
	
	/*enable interrupt*/
	/*Enables all GPIO pins as interrupt sources to the DSP CPU*/
	gpGPIO_regs->BINTEN= 3;

	/*trigger interrupt on both rising and falling edge*/	
	gpGPIO_regs->BANK_REGISTERS[0].SET_RIS_TRIG= 0xFFFFFFFF;
	//gpGPIO_regs->BANK_REGISTERS[0].SET_FAL_TRIG= 0xFFFFFFFF;

	/*clear output data*/
	gpGPIO_regs->BANK_REGISTERS[0].CLR_DATA= 0xFFFFFFFF;
}


/*****************************************************************************
 Prototype    : EMIF_init
 Description  : EMIF_init
 Input        : 
 Output       : None
 Return Value : 
 
History        :
  1.Date         : 2017-02-13
    Author       : 
    Modification : Created function    

*****************************************************************************/
void EMIF_init()
{
	memset(&gNorCeCfg, 0, sizeof(gNorCeCfg));
	/* for 319 project debug :remove Nand mode*/
	//memset(&gNandCeCfg, 0, sizeof(gNandCeCfg));	
	memset(&gEmif16Cfg, 0, sizeof(gEmif16Cfg));
	
	/*configuraiton for the CE of NOR FLASH*/
	gNorCeCfg.busWidth= EMIF_BUS_16BIT;
	gNorCeCfg.opMode  = NOR_ASRAM_MODE;
	gNorCeCfg.strobeMode = SS_STROBE;  
	gNorCeCfg.waitMode = EMIF_WAIT_NONE;
	gNorCeCfg.wrSetup = CSL_EMIF16_A0CR_WSETUP_RESETVAL;          
	gNorCeCfg.wrStrobe = CSL_EMIF16_A0CR_WSTROBE_RESETVAL;         
	gNorCeCfg.wrHold= CSL_EMIF16_A0CR_WHOLD_RESETVAL;            
	gNorCeCfg.rdSetup= CSL_EMIF16_A0CR_RSETUP_RESETVAL;           
	gNorCeCfg.rdStrobe= CSL_EMIF16_A0CR_RSTROBE_RESETVAL;          
	gNorCeCfg.rdHold= CSL_EMIF16_A0CR_RHOLD_RESETVAL;            
	gNorCeCfg.turnAroundCycles= CSL_EMIF16_A0CR_TA_RESETVAL;  
	gNorCeCfg.nor_pg_Cfg= NULL;        


	/*使用CE0*/
	gEmif16Cfg.ceCfg[0] = &gNorCeCfg;

	gEmif16Cfg.wait0Polarity= EMIF_LOW_WAIT;
	gEmif16Cfg.wait1Polarity= EMIF_LOW_WAIT;
	gEmif16Cfg.maxWait = CSL_EMIF16_AWCCR_MAXEXTWAIT_RESETVAL;
	
	EMIF16_init(&gEmif16Cfg);

}

/*****************************************************************************
 Prototype    : NOR_init
 Description  : 用于接口验证
 Input        : Uint32 pwrDmnNum  
                Uint32 moduleNum  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/2/29
    Author       : sxf
    Modification : Created function

*****************************************************************************/
Uint32 NOR_init(NOR_InfoHandle hNorInfo)
{
    
  // Specify number of devices
  //hNorInfo->numberDevices = hNorInfo->busWidth/hNorInfo->chipOperatingWidth;

  // Perform CFI Query
    
  // Setup function pointers
  puts("NOR Initialization:");
  
  //use EMIF16 1K storage space
  hNorInfo->flashSize = 0x100;
  hNorInfo->bufferSize = 0x200;

  Flash_Write = &EMIF16_Write;
    
  return E_PASS;
}


// NOR_WriteBytes
Uint32 NOR_writeBytes( NOR_InfoHandle hNorInfo, Uint32 writeAddress, Uint32 numBytes, Uint32 srcDatAddress)
{
	Int32   i;
	Uint32  retval = E_PASS;

	//
	Uint32 ReadAddress= writeAddress;
	Uint32 Readnum2Bytes=numBytes/2;
	Uint32 WriteData =0x1234;

	if(FLASH_BUS_WIDTH_2_BYTES == hNorInfo->busWidth)
	{
		if (writeAddress & 0x00000001)
		{
			puts("address is not aligned to 2-byte boundary");
			return E_FAIL;
		}

		if (numBytes & 0x00000001)
		{
			printf("number of bytes is not even");
			return E_FAIL;
		}
	}

	while (numBytes > 0)
	{
		if( (numBytes < hNorInfo->bufferSize) || (writeAddress & (hNorInfo->bufferSize-1) ))
		{
			//if ((*Flash_Write)(hNorInfo, writeAddress, NOR_flashReadData(hNorInfo,srcDatAddress,0) ) != E_PASS)
			//Flash_Write = &EMIF16_Write;
			if ((*Flash_Write)(hNorInfo, writeAddress, WriteData ) != E_PASS)
			{
				printf("Normal write failed.\r\n");


				//retval = E_FAIL;
			}
			else
			{
				numBytes     -= hNorInfo->busWidth;
				writeAddress += 2;
				srcDatAddress  += hNorInfo->busWidth;
				WriteData++;
			}
		}
		else
		{
			printf("Buffered write failed. Trying normal write");
		}
		// Output status info on the write operation
		if (retval == E_PASS)
		{    
			/*print progress*/
			if  ( (0==(writeAddress & 0xFFFFF)) || (numBytes == 0) )
			{
				printf("NOR Write OK through 0x%x\n", writeAddress);
			}
		}
		else
		{
			printf( "NOR Write Failed...Aborting!");
			return E_FAIL;
		}
	}

	//读出刚才写的数据
	for(i=0;i<Readnum2Bytes;i++)
	{
		testReadBuf[i]=(Uint16)(NOR_flashReadData(hNorInfo, ReadAddress, 0));
		ReadAddress += 2;
	}
	
	return retval;
}

int NOR_FLASH_write_block(unsigned int srcDatAddress, 
	unsigned int dstFlashAddress, unsigned int byteNumber)
{
	// Write the application data to the flash
	if (E_FAIL==NOR_writeBytes( &gNorInfo, dstFlashAddress, byteNumber, srcDatAddress))
	{
		puts("\tERROR: Writing NOR failed.");
		return E_FAIL;
	}
	return E_PASS;
}


/*
This function fills the patterns into memory and reads back and verification
*/
unsigned int FLASH_MEM_FillTest(unsigned int uiStartAddress,
	unsigned int uiByteCount,
	unsigned int * temp_buf,
	unsigned int uiBufByteSize,
	unsigned int uiBitPattern)
{
    unsigned int i, uiFailCount=0;
    unsigned int uiAddress, uiEndAddress, uiCount;
    //volatile unsigned int *uipAddressPointer;
    volatile unsigned int uiReadBack;

	uiEndAddress= uiStartAddress + uiByteCount;
    /* Write the pattern to DMA buffer*/
	for(i= 0; i< uiBufByteSize/4; i++)
		temp_buf[i]= uiBitPattern;

	/*copy the pattern to memory under test*/
	uiAddress = uiStartAddress;
	while(uiAddress < uiEndAddress)
	{
		uiCount = uiEndAddress - uiAddress;
		if(uiCount > uiBufByteSize)
			uiCount = uiBufByteSize;
		NOR_FLASH_write_block((unsigned int)temp_buf, uiAddress, uiCount);
        uiAddress += uiBufByteSize;
    }

	
	return uiFailCount;
}

int FLASH_MEM_Test(unsigned int uiStartAddress, 
	unsigned int uiStopAddress,
	unsigned int uiBufAddress,
	unsigned int uiBufByteSize)
{
    int j;
    unsigned int uiByteCount, uiFailCount=0, uiTotalFailCount=0;

    uiByteCount = (uiStopAddress - uiStartAddress);


    // Fill with values from pattern table
    for (j = 0; j < sizeof(uiDataPatternTable)/4; j++)
    //for (j = 0; j < 1; j++)
    {
		
        uiFailCount = FLASH_MEM_FillTest(uiStartAddress, uiByteCount, 
        	(unsigned int *)uiBufAddress, uiBufByteSize, uiDataPatternTable[j]);
	    if (uiFailCount)
	    {
	    	printf("!!!Failed Memory Fill Test at %d Units with pattern 0x%8x\n",uiFailCount, uiDataPatternTable[j]);
	        uiTotalFailCount+=uiFailCount;
	    }
		else
		   	printf("Passed Memory Fill Test from 0x%8x to 0x%8x with pattern 0x%8x\n",uiStartAddress,uiStopAddress, uiDataPatternTable[j]);
    }
    



  	return uiTotalFailCount;

}

/*****************************************************************************
 Prototype    : NOR_FLASH_test
 Description  : 用于接口验证
 Input        : Uint32 pwrDmnNum  
                Uint32 moduleNum  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/2/29
    Author       : sxf
    Modification : Created function
用于验证flash接口数据TX/RX
*****************************************************************************/
void NOR_FLASH_test(unsigned int uiBufAddress,
	unsigned int uiBufByteSize)
{
	
	if (NOR_init(&gNorInfo))
	{
		printf("\tERROR: NOR Initialization failed." );
		return;
	}

	//call back function for FLASH erase
	//FLASH_erase_blocks= &NOR_FLASH_erase_blocks;

	//call back function for FLASH write
	//FLASH_write_block= &NOR_FLASH_write_block;

	//call back function for FLASH read
	//FLASH_read_block= NULL; /*NOR FLASH can be read directly*/

	FLASH_MEM_Test(gNorInfo.flashBase, 
		gNorInfo.flashBase+ gNorInfo.flashSize,
		uiBufAddress, uiBufByteSize);

	
	
}

/*****************************************************************************
 Prototype    : GPIO_FPGA_Handshake
 Description  : 用于通知FPGA进行rapid IO进行初始化
 Input        : 
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/4/19
    Author       : sxf
    Modification : Created function

*****************************************************************************/
void GPIO_FPGA_Handshake()
{	
	Int16 *p;
	p=(Int16 *)0x700001FC;
	*p=(*p+1);//GPIO和FPGA使用握手
}


/*****************************************************************************
 Prototype    : SRIO_FPGA_Handshake
 Description  : 用于通知FPGA进行rapid IO进行初始化
 Input        : 
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/4/19
    Author       : sxf
    Modification : Created function

*****************************************************************************/
void SRIO_FPGA_Handshake()
{
	Int16 *pRxadd;
	pRxadd=(Int16 *)(0x70000014);
	
	*(Int16 *)0x70000188=0x5a;//SRIO和FPGA进行握手
	TSC_delay_us(20);
	do
	{		
		if(*pRxadd==0x5a)//等待握手成功
		break;
		TSC_delay_us(20);
	}while(1);
	//TSC_delay_ms(20);//等待FPGA srio初始化完成
}

/*****************************************************************************
 Prototype    : Interface_test
 Description  : 用于接口验证
 Input        : Uint32 pwrDmnNum  
                Uint32 moduleNum  
 Output       : None
 Return Value : 
 
  History        :
  1.Date         : 2016/2/29
    Author       : sxf
    Modification : Created function

*****************************************************************************/
void Interface_test()
{
	

	//for 319 project debug:EMIF16接口验证
	NOR_FLASH_test((unsigned int)testBuf, FLASH_TEST_BUFFER_SIZE*2);
}

