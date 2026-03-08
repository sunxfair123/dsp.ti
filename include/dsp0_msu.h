/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : dsp0_msu.h
  Version       : Initial Draft
  Author        : sxf
  Created       : 2016/01/08
  Last Modified :
  Description   : Robust_INTC.c header file
                  This file setup interrupt controller, and includes:
                1, ISR for memory protection and EDC/ECC.
                2, function to manually tigger two interrupts to show the interrup drop monitor
  Function List :
  History       :
  1.Date        : 2016/01/08
    Author      : sxf
    Modification: Created file

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

#ifndef __DSP0_MSU_H__
#define __DSP0_MSU_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <csl_qm_queue.h>






extern Uint32 packetBuffer_DDR_Check[64];
extern void MSU_Image();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DSP0_H__ */
