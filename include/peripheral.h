/******************************************************************************
	
 Copyright (C), 2001-2012, Texas Instrument.

******************************************************************************
File Name 	: Robust_peripheral_interface.h
Version		: Initial Draft
Author		: sxf
Created		: 2016/01/08
Last Modified :
Description	: Robust_peripheral_interface.c header file
Function List :
History		:

  1.Date		: 2016/01/08
    Author	: sxf
    Modification: Created file
	
******************************************************************************/


#ifndef _PERIPHERAL_H
#define _PERIPHERAL_H

#include "SRIO_Init.h"


#define FLASH_BUS_WIDTH_1_BYTE 		1
#define FLASH_BUS_WIDTH_2_BYTES 	2

#define EMIF16_CE1_BaseAddr	 		0x74000000
#define uiEMIF16Mark                0





#ifndef E_PASS
  #define E_PASS    (0x00000000u)
#endif
#ifndef E_FAIL
  #define E_FAIL    (0x00000001u)
#endif

// Struct to hold discovered flash parameters
typedef struct _NOR_INFO_
{
  Uint32      flashBase;                    // 32-bit address of flash start
  Uint8       busWidth;                     // 8-bit or 16-bit bus width
  Uint8       chipOperatingWidth;           // The operating width of each chip
  Uint8       maxTotalWidth;                // Maximum extent of width of all chips combined - determines offset shifts
  Uint8		  res1;
  Uint32      flashSize;                    // Size of NOR flash regions in bytes (numberDevices * size of one device)
  Uint32      bufferSize;                   // Size of write buffer
  //NOR_CmdSet  commandSet;                   // command set id (see CFI documentation)
  Uint8       numberDevices;                // Number of deives used in parallel
  Uint8       numberRegions;                // Number of regions of contiguous regions of same block size
  //Uint32      numberBlocks[CFI_MAXREGIONS]; // Number of blocks in a region
  //Uint32      blockSize[CFI_MAXREGIONS];    // Size of the blocks in a region
  //NOR_ManfID  manfID;                       // Manufacturer's ID
  Uint16      devID1;                       // Device ID
  Uint16      devID2;                       // Used for AMD 3-byte ID devices
}
NOR_InfoObj,*NOR_InfoHandle;


/*define the message convey by doorbell for this test*/
typedef enum
{
	SRIO_DOORBELL_MSG_LATENCY_TEST= 0,
	SRIO_DOORBELL_MSG_NREAD_DONE    ,
	SRIO_DOORBELL_MSG_NWRITE_DONE   ,
	SRIO_DOORBELL_MSG_NWRITE_R_DONE ,
	SRIO_DOORBELL_MSG_SWRITE_DONE   
}SRIO_Doorbell_Message;


 
extern void DSP0_GPIO_init();
extern void EMIF_init();

// Generic commands that will point to either AMD or Intel command set
static Uint32   (* Flash_Write)(NOR_InfoHandle, Uint32, volatile Uint32);

int FLASH_MEM_Test(unsigned int uiStartAddress, 
	unsigned int uiStopAddress,
	unsigned int uiBufAddress,
	unsigned int uiBufByteSize);


/*EMIF 写数据*/
static void NOR_flashWriteData(NOR_InfoHandle hNorInfo, Uint32 address, Uint32 data)
{
	if (FLASH_BUS_WIDTH_1_BYTE==hNorInfo->busWidth)
		*(Uint8 *)address = (Uint8)data;
	else /*FLASH_BUS_WIDTH_2_BYTES*/
		*(Uint16 *)address = (Uint16)data;
}

/*EMIF 读取数据*/
static Uint32 NOR_flashReadData(NOR_InfoHandle hNorInfo, Uint32 address, Uint32 offset)
{
	Uint32 data;
	Uint32 addr= address + offset*hNorInfo->busWidth;

	if (FLASH_BUS_WIDTH_1_BYTE==hNorInfo->busWidth)
		data = *(Uint8 *)addr;
	else /*FLASH_BUS_WIDTH_2_BYTES*/
		data = *(Uint16 *)addr;

	return data;
}

// AMD Flash Write
static Uint32 EMIF16_Write(NOR_InfoHandle hNorInfo, Uint32 address, volatile Uint32 data )
{
  Uint32 retval = E_PASS;
	
  // Send Commands
  //LOCAL_AMDPrefixCommands(hNorInfo);
  //NOR_flashWriteCmd(hNorInfo,hNorInfo->flashBase, AMD_CMD2_ADDR, AMD_PROG_CMD);
  NOR_flashWriteData(hNorInfo,address, data);

  // Wait for ready.
  //while(TRUE)
  while(uiEMIF16Mark)
  {
    if ( (NOR_flashReadData(hNorInfo, address, 0 ) & ((1<<7) | (1<<15) )) == (data & ((1<<7) | (1<<15)) ) )
    {
      break;
    }
	#if 0
    else
    {
      if(NOR_flashIsSetAll(hNorInfo, address, 0, (1<<5)))
      {
        if ( (NOR_flashReadData(hNorInfo, address, 0 ) & ((1<<7) | (1<<15)) ) != (data & ((1<<7) | (1<<15)) ) )
        {
          PRINT("Timeout ocurred.");
          retval = E_FAIL;
        }
        break;				
      }
    }
	#endif
  }
	
  // Return Read Mode
	//LOCAL_AMDSoftReset(hNorInfo);
	
	// Verify the data.
	#if 0
  if ( (retval == E_PASS) && ( NOR_flashReadData(hNorInfo, address, 0) != data) )
    retval = E_FAIL;
  #endif
	
	return retval;
}

extern SRIO_Multiple_Test_Config test_2DSP_cfg;

extern void NOR_FLASH_test(unsigned int uiBufAddress,
	unsigned int uiBufByteSize);

extern void GPIO_FPGA_Handshake();
extern void SRIO_FPGA_Handshake();
extern void Interface_test();

#endif

