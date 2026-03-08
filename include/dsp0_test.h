/****************************************************************************\
 *       Copyright (C) 2011 Texas Instruments Incorporated.      *
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
 * Texas Instruments has been advised of the liability.                     *
 ****************************************************************************
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
***************************************************************************/
#ifndef DSP0_TEST_H
#define DSP0_TEST_H

#define DDR3_testflag 0
#define temperature_testflag 0


extern unsigned int Memory_Data_DDR3_OneCircle_Test(unsigned int uiBaseAddress,unsigned int uiNumBytes);
extern unsigned int Memory_Data_DDR3_OneCircle_WTest(unsigned int uiBaseAddress,unsigned int uiNumBytes);
extern unsigned int Memory_Data_DDR3_Test(unsigned int uiBaseAddress,unsigned int uiNumBytes);
/*=============================basic memory test=============================*/




extern void FPGA_EMIF16_RTXtest();
extern void GPIO_test();
extern void DDR3_test();

extern void GPIO_temperature_test();






#endif
