/****************************************************************************/
/*  319.cmd                                                               */
/*  Copyright (c) 2011 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an C6678              */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/*    Usage:       The map below divides the external memory in segments    */
/*                 Use the linker option --define=COREn=1                   */
/*                 Where n is the core number.                              */
/*                                                                          */
/****************************************************************************/
-x
-stack 0x10000
-heap 0x8000
MEMORY
{
    /* 4MB Multicore shared Memmory */
    //SL2_CODE:             o = 0x0C000000  l = 0x00100000  /* 1MB Multicore shared Memmory */
    /* SL2_DATA:total 512k,其中32k*8用于core的.cinit .const .switch,其它用于共享数据*/    
    CORE0_SL2_DATA:             o = 0x0C000000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE1_SL2_DATA:             o = 0x0C008000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE2_SL2_DATA:             o = 0x0C010000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE3_SL2_DATA:             o = 0x0C018000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE4_SL2_DATA:             o = 0x0C020000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE5_SL2_DATA:             o = 0x0C028000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE6_SL2_DATA:             o = 0x0C030000  l = 0x00008000  /* 32k Multicore shared Memmory */
    CORE7_SL2_DATA:             o = 0x0C038000  l = 0x00008000  /* 32k Multicore shared Memmory */
    SL2_IMAGE:                  o = 0x0C040000  l = 0x00280000  /* 2M+512k Multicore shared Memmory  调试算法使用 20171020 modified*/
    
    /* 剩余空间:根据需要分配给上面的SL2空间*/
    SL2_RES:              o = 0x0C2C0000  l = 0x00010000 /* 64k Multicore shared Memmory */ 
    SL2_RES0:             o = 0x0C2D0000  l = 0x00010000 /* 64k Multicore shared Memmory */
    SL2_RES1:             o = 0x0C2E0000  l = 0x00010000 /* 64KB Multicore shared Memmory */ 
    SL2_CODE:             o = 0x0C2f0000  l = 0x00010000 /* 64KB Multicore shared Memmory */
    SL2_RES2:             o = 0x0C300000  l = 0x000FF000 /* 1023KB Multicore shared Memmory */ 
     
    /* remap 1kB SL2 from 0x0C300000 to 0x18000000,对应修改代码中的XMC_MPAX_cfg_table_C6678*/
    SL2_REMAP:            o = 0x18000000  l = 0x00001000
    
    
    
  
      
    /* CORE0 Local L2, 0.5MB*/
    CORE0_VECTORS:              o = 0x10800000  l = 0x00000200  
    BOOT_CORE0 									o = 0x10800200  l = 0x000000C0 
    CORE0_LL2_CODE:             o = 0x108002C0  l = 0x0003FD40
    CORE0_LL2_DATA:             o = 0x10840000  l = 0x00040000    
    CORE0_L1P_SRAM:  o = 0x10E00000 l = 0x00008000   /* 32kB CORE0 L1P/SRAM */
    CORE0_L1D_SRAM:  o = 0x10F00000 l = 0x00008000   /* 32kB CORE0 L1D/SRAM */
    
    
    /* CORE1 Local L2, 0.5MB*/
    CORE1_VECTORS:              o = 0x11800000  l = 0x00000200
    BOOT_CORE1 									o = 0x11800200  l = 0x000000C0    
    CORE1_LL2_CODE:             o = 0x118002C0  l = 0x0001FD40
    CORE1_LL2_DATA:             o = 0x11820000  l = 0x00060000    
    CORE1_L1P_SRAM:  o = 0x11E00000 l = 0x00008000   /* 32kB CORE0 L1P/SRAM */
    CORE1_L1D_SRAM:  o = 0x11F00000 l = 0x00008000   /* 32kB CORE0 L1D/SRAM */
    
    
    /* CORE2 Local L2, 0.5MB*/
    CORE2_VECTORS:              o = 0x12800000  l = 0x00000200   
    BOOT_CORE2 									o = 0x12800200  l = 0x000000C0    
    CORE2_LL2_CODE:             o = 0x128002C0  l = 0x0001FD40
    CORE2_LL2_DATA:             o = 0x12820000  l = 0x00060000    
    CORE2_L1P_SRAM:  o = 0x12E00000 l = 0x00008000   /* 32kB CORE0 L1P/SRAM */
    CORE2_L1D_SRAM:  o = 0x12F00000 l = 0x00008000   /* 32kB CORE0 L1D/SRAM */
    
    
    /* CORE3 Local L2, 0.5MB*/
    CORE3_VECTORS:              o = 0x13800000  l = 0x00000200  
    BOOT_CORE3 									o = 0x13800200  l = 0x000000C0    
    CORE3_LL2_CODE:             o = 0x138002C0  l = 0x0001FD40 
    CORE3_LL2_DATA:             o = 0x13820000  l = 0x00060000    
    CORE3_L1P_SRAM:  o = 0x13E00000 l = 0x00008000   /* 32kB CORE3 L1P/SRAM */
    CORE3_L1D_SRAM:  o = 0x13F00000 l = 0x00008000   /* 32kB CORE3 L1D/SRAM */
    
    
    /* CORE4 Local L2, 0.5MB*/
    CORE4_VECTORS:              o = 0x14800000  l = 0x00000200
    BOOT_CORE4 									o = 0x14800200  l = 0x000000C0    
    CORE4_LL2_CODE:             o = 0x148002C0  l = 0x0001FD40   
    CORE4_LL2_DATA:             o = 0x14820000  l = 0x00060000    
    CORE4_L1P_SRAM:  o = 0x14E00000 l = 0x00008000   /* 32kB CORE4 L1P/SRAM */
    CORE4_L1D_SRAM:  o = 0x14F00000 l = 0x00008000   /* 32kB CORE4 L1D/SRAM */
    
    /* CORE5 Local L2, 0.5MB*/
    CORE5_VECTORS:              o = 0x15800000  l = 0x00000200
    BOOT_CORE5 									o = 0x15800200  l = 0x000000C0    
    CORE5_LL2_CODE:             o = 0x158002C0  l = 0x0001FD40   
    CORE5_LL2_DATA:             o = 0x15820000  l = 0x00060000    
    CORE5_L1P_SRAM:  o = 0x15E00000 l = 0x00008000   /* 32kB CORE6 L1P/SRAM */
    CORE5_L1D_SRAM:  o = 0x15F00000 l = 0x00008000   /* 32kB CORE6 L1D/SRAM */
    
    /* CORE6 Local L2, 0.5MB*/
    CORE6_VECTORS:              o = 0x16800000  l = 0x00000200  
    BOOT_CORE6 									o = 0x16800200  l = 0x000000C0    
    CORE6_LL2_CODE:             o = 0x168002C0  l = 0x0001FD40 
    CORE6_LL2_DATA:             o = 0x16820000  l = 0x00060000    
    CORE6_L1P_SRAM:  o = 0x16E00000 l = 0x00008000   /* 32kB CORE6 L1P/SRAM */
    CORE6_L1D_SRAM:  o = 0x16F00000 l = 0x00008000   /* 32kB CORE6 L1D/SRAM */
    
    /* CORE7 Local L2, 0.5MB*/
    CORE7_VECTORS:              o = 0x17800000  l = 0x00000200  
    BOOT_CORE7 									o = 0x17800200  l = 0x000000C0    
    CORE7_LL2_CODE:             o = 0x178002C0  l = 0x0001FD40 
    CORE7_LL2_DATA:             o = 0x17820000  l = 0x00060000    
    CORE7_L1P_SRAM:  o = 0x17E00000 l = 0x00008000   /* 32kB CORE7 L1P/SRAM */
    CORE7_L1D_SRAM:  o = 0x17F00000 l = 0x00008000   /* 32kB CORE7 L1D/SRAM */
    
    
    
    
    
  
    EMIF16_CS2:      o = 0x70000000 l = 0x04000000   /* 64MB EMIF16 CS2 Data Memory */
    EMIF16_CS3:      o = 0x74000000 l = 0x04000000   /* 64MB EMIF16 CS3 Data Memory */
    EMIF16_CS4:      o = 0x78000000 l = 0x04000000   /* 64MB EMIF16 CS4 Data Memory */
    EMIF16_CS5:      o = 0x7C000000 l = 0x04000000   /* 64MB EMIF16 CS5 Data Memory */
    
    
    
    
    /* External DDR3, upto 1GB  */
    /* 256MB  */
    DDR3_CODE:            o = 0x81000000  l = 0x01000000
    DDR3_DATA:            o = 0x82000000  l = 0x04000000
    //DDR3_Profile_DATA:    o = 0x86000000  l = 0x02000000
    //DDR3_ECC_DATA:        o = 0x88000000  l = 0x01000000
    DDR3_RES:             o = 0x89000000  l = 0x07000000
    
    
    /*add by sxf 20151208 CORE0 DDR for transfer data */
    //CORE0_DDR3:      o = 0x80000000 l = 0x10000000   /* 256MB DDR3 SDRAM for CORE0 */
    CORE0_DDR3_Descriptor:     o = 0x90000000 l = 0x00040000   /* 256KB DDR3 SDRAM for CORE0 */
    CORE0_DDR3_PBUF:           o = 0x90040000 l = 0x0FFC0000   /* 256MB-256KB DDR3 SDRAM for CORE0 */
  
  
    CORE1_DDR3_Descriptor:     o = 0xA0000000 l = 0x00008000   /* 256KB DDR3 SDRAM for CORE0 */
    CORE1_DDR3_PBUF:           o = 0xA0008000 l = 0x0FFF8000   /* 256MB-32KB DDR3 SDRAM for CORE0 */
}
 
SECTIONS
{
#ifdef CORE0
     vecs                           > CORE0_VECTORS 
    .text                           > CORE0_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE0_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE0_LL2_DATA
    .far                            > CORE0_LL2_DATA
    .fardata                        > CORE0_LL2_DATA
    .cio                            > CORE0_LL2_DATA
    .sysmem                         > CORE0_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE0_LL2_CODE
    
    .cinit                          > CORE0_SL2_DATA
    .const                          > CORE0_SL2_DATA
    .switch                         > CORE0_SL2_DATA      
    core0_Data.SL2                  > CORE1_SL2_DATA  /* add 20170614 */
     
   	/* add by sxf 20160108 目前需要队列不超过16k,编译时候不会出现下面的段*/
   	QMSS_Data.linkingRAM1 		      >SL2_CODE	
   	ImageData.SL2										>SL2_IMAGE
   	
   	
   	/* add by sxf 20171122临时调试使用*/
   	ImageData3.SL2                    >SL2_REMAP
   	
   	
  	
    
    QMSS_Data.Core0_Descriptor_DDR  > CORE0_DDR3_Descriptor
    PacketData.Core0_buffer_DDR 		> CORE1_DDR3_PBUF
    /* add by sxf 20160607 ,用于imagetarget工程中的数据存储*/ 
    /*ArrayData.Core0_buffer_DDR 		> CORE0_DDR3_PBUF*/
    /*ArrayData.Core0_buffer_DDR 		> SL2_RES0*/
    
    
    QMSS_Data.Core1_Descriptor_DDR  > CORE1_DDR3_Descriptor
    PacketData.Core1_buffer_DDR 		> CORE1_DDR3_PBUF
    
    ImageData.DDR3> CORE0_DDR3_PBUF
    
    //for 319 project debug:PKTDMA add SL2 test
    QMSS_Data.LL2										> CORE0_LL2_DATA
    QMSS_Data.SL2										> SL2_RES
    
    //for 319 project debug:semaphore test
    .sync														> SL2_RES1
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE0_LL2_CODE
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE0
     
     //for 319 project debug:用于EMIF16调试，调试完后去掉
     far:testBuf    >    SL2_RES
#endif

#ifdef CORE1
     vecs                           > CORE1_VECTORS 
    .text                           > CORE1_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE1_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE1_LL2_DATA
    .far                            > CORE1_LL2_DATA
    .fardata                        > CORE1_LL2_DATA
    .cio                            > CORE1_LL2_DATA
    .sysmem                         > CORE1_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE1_LL2_CODE
    
    .cinit                          > CORE1_SL2_DATA
    .const                          > CORE1_SL2_DATA
    .switch                         > CORE1_SL2_DATA  
    
    /* add by sxf 20160607 ,用于imagetarget工程中的数据存储*/ 
    ArrayData.Core0_buffer_DDR 		> CORE0_DDR3_PBUF
    /*ArrayData.Core0_buffer_DDR 		> SL2_RES0*/
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE1_LL2_CODE  
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE1
#endif

#ifdef CORE2
     vecs                           > CORE2_VECTORS 
    .text                           > CORE2_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE2_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE2_LL2_DATA
    .far                            > CORE2_LL2_DATA
    .fardata                        > CORE2_LL2_DATA
    .cio                            > CORE2_LL2_DATA
    .sysmem                         > CORE2_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE2_LL2_CODE
    
    .cinit                          > CORE2_SL2_DATA
    .const                          > CORE2_SL2_DATA
    .switch                         > CORE2_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE2_LL2_CODE  
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE2
#endif

#ifdef CORE3
     vecs                           > CORE3_VECTORS 
    .text                           > CORE3_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE3_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE3_LL2_DATA
    .far                            > CORE3_LL2_DATA
    .fardata                        > CORE3_LL2_DATA
    .cio                            > CORE3_LL2_DATA
    .sysmem                         > CORE3_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE3_LL2_CODE
    
    .cinit                          > CORE3_SL2_DATA
    .const                          > CORE3_SL2_DATA
    .switch                         > CORE3_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE3_LL2_CODE 
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE3
#endif

#ifdef CORE4
     vecs                           > CORE4_VECTORS 
    .text                           > CORE4_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE4_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE4_LL2_DATA
    .far                            > CORE4_LL2_DATA
    .fardata                        > CORE4_LL2_DATA
    .cio                            > CORE4_LL2_DATA
    .sysmem                         > CORE4_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE4_LL2_CODE
    
    .cinit                          > CORE4_SL2_DATA
    .const                          > CORE4_SL2_DATA
    .switch                         > CORE4_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE4_LL2_CODE
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE4 
#endif

#ifdef CORE5
     vecs                           > CORE5_VECTORS 
    .text                           > CORE5_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE5_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE5_LL2_DATA
    .far                            > CORE5_LL2_DATA
    .fardata                        > CORE5_LL2_DATA
    .cio                            > CORE5_LL2_DATA
    .sysmem                         > CORE5_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE5_LL2_CODE
    
    .cinit                          > CORE5_SL2_DATA
    .const                          > CORE5_SL2_DATA
    .switch                         > CORE5_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE5_LL2_CODE 
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE5
#endif

#ifdef CORE6
     vecs                           > CORE6_VECTORS 
    .text                           > CORE6_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE6_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE6_LL2_DATA
    .far                            > CORE6_LL2_DATA
    .fardata                        > CORE6_LL2_DATA
    .cio                            > CORE6_LL2_DATA
    .sysmem                         > CORE6_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE6_LL2_CODE
    
    .cinit                          > CORE6_SL2_DATA
    .const                          > CORE6_SL2_DATA
    .switch                         > CORE6_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE6_LL2_CODE 
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE6
#endif

#ifdef CORE7
     vecs                           > CORE7_VECTORS 
    .text                           > CORE7_LL2_CODE      
    .stack: fill= 0xBFBFBFBF        > CORE7_LL2_DATA
    GROUP
    {
        .neardata
        .rodata
        .bss
    }                               > CORE7_LL2_DATA
    .far                            > CORE7_LL2_DATA
    .fardata                        > CORE7_LL2_DATA
    .cio                            > CORE7_LL2_DATA
    .sysmem                         > CORE7_LL2_DATA
                                    
    .text:Core_MPAX                 > CORE7_LL2_CODE
    
    .cinit                          > CORE7_SL2_DATA
    .const                          > CORE7_SL2_DATA
    .switch                         > CORE7_SL2_DATA  
    
    /*for 319 project debug :add by sxf 20160108用于led flash*/
     platform_lib > CORE7_LL2_CODE 
     
     .myboot
     {
        *.*<boot.obj>(.text)

     } > BOOT_CORE7
#endif
}
