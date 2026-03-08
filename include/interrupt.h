/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : interrupt.h
  Version       : 
  Author        : 
  Created       : 
  Last Modified :
  Description   : Robust_INTC.c header file
                  This file setup interrupt controller, and includes:
                1, ISR for memory protection and EDC/ECC.
                2, function to manually tigger two interrupts to show the interrup drop monitor
  Function List :
  History	   :
  1.Date		 : 2017-02-13
  Author		 : 
  Modification	 : Created function 
******************************************************************************/

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

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define CORENUM                         8

/*add by sxf 20160108 for IPC interrupt*/
#define IPCGR0  						(0x02620240u)
#define IPCGR1							(0x02620244u)
#define IPCGR2  						(0x02620248u)
#define IPCGR3  						(0x0262024Cu)
#define IPCGR4  						(0x02620250u)
#define IPCGR5							(0x02620254u)
#define IPCGR6  						(0x02620258u)
#define IPCGR7  						(0x0262025Cu)
#define IPCAR0  						(0x02620280u)
#define IPCAR1  						(0x02620284u)
#define IPCAR2  						(0x02620288u)
#define IPCAR3  						(0x0262028Cu)
#define IPCAR4  						(0x02620290u)
#define IPCAR5  						(0x02620294u)
#define IPCAR6  						(0x02620298u)
#define IPCAR7  						(0x0262029Cu)


#define GPIO8_eventFlag_ISR  	0x40000
#define GPIO9_eventFlag_ISR  	0x80000
#define GPIO10_eventFlag_ISR  	0x100000
#define GPIO89_eventFlag_ISR  	0xc0000


// BOOT and CONFIG dsp system modules Definitions
#define CHIP_LEVEL_REG  0x02620000
// Boot cfg registers
#define KICK0			*(unsigned int*)(CHIP_LEVEL_REG + 0x0038)
#define KICK1			*(unsigned int*)(CHIP_LEVEL_REG + 0x003C)
#define KICK0_UNLOCK (0x83E70B13)
#define KICK1_UNLOCK (0x95A4F1E0)
#define KICK_LOCK    0

/*semaphore:Ò»¹²32¸ö*/
#define SEMAPHORE_NUM0  8
#define SEMAPHORE_NUM1  9



extern Uint32 iIPCGRInfo[CORENUM];
extern Uint32 iIPCARInfo[CORENUM];

extern Uint32  SemaphoreSyncMark[32];


extern void core0_Interrupts_Init(void);
extern void othercores_Interrupts_Init(void);






//extern void INT_drop_test();
//extern interrupt void LL2_EDC_ISR();
//extern interrupt void SL2_EDC_ISR();

extern interrupt void IPC_ACT_ISR(void);
extern interrupt void SRIO_Doorbell_ISR();
extern interrupt void GPIO_ISR();




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ROBUST_INTC_H__ */
