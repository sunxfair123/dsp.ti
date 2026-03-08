/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : KeyStone_Navigator_init_drv.c
  Version       : Initial Draft
  Author        : sxf
  Created       : 2016/01/08
  Last Modified :
  Description   : Multicore Navigator initialization and driver

  Function List :
              KeyStone_Copy_FDQ
              KeyStone_Host_Descriptor_Queues_init
              KeyStone_Mono_Descriptor_Queues_init
              KeyStone_pktDma_configureRxFlow
              KeyStone_pktDma_Global_Control
              KeyStone_pktDma_TxCh_config
              KeyStone_Qmss_Config_Acc_Channel
              KeyStone_Qmss_config_Acc_Timer
              KeyStone_Qmss_Config_Reclaim_Queue
              KeyStone_QMSS_Descriptor_Regions_init
              KeyStone_Qmss_disable_Acc_Channel
              KeyStone_Qmss_Download_Firmware
              KeyStone_QMSS_Linking_RAM_init
  History       :
  1.Date        : 2016/01/08
    Author      : sxf
    Modification: Created file
******************************************************************************/
#include <c6x.h>
#include <stdio.h>
#include <string.h>
#include <cslr_device.h>
#include <csl_cacheAux.h>
#include "dsp0_common.h"
#include "Navigator_init_drv.h"
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
/*Queue Manager definition*/


CSL_Cppidma_tx_channel_configRegs * gpSRIO_DMA_TxChCfgRegs= 
	(CSL_Cppidma_tx_channel_configRegs * )CSL_SRIO_CONFIG_CPPI_DMA_TX_CFG_REGS;
CSL_Cppidma_rx_channel_configRegs * gpSRIO_DMA_RxChCfgRegs= 
	(CSL_Cppidma_rx_channel_configRegs * )CSL_SRIO_CONFIG_CPPI_DMA_RX_CFG_REGS;


