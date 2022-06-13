/**************************************************************************//**
 * @file     partition_IOTKit_CM33.h
 * @brief    CMSIS-CORE Initial Setup for Secure / Non-Secure Zones for IOTKit_CM33
 * @version  V1.0.0
 * @date     13. December 2016
 ******************************************************************************/
/* Copyright (c) 2015 - 2016 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef PARTITION_IOTKit_CM33_H
#define PARTITION_IOTKit_CM33_H

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/

/*
// <e>Initialize Security Attribution Unit (SAU) CTRL register
*/
#define SAU_INIT_CTRL          1

/*
//   <q> Enable SAU
//   <i> Value for SAU->CTRL register bit ENABLE
*/
#define SAU_INIT_CTRL_ENABLE   1

/*
//   <o> When SAU is disabled
//     <0=> All Memory is Secure
//     <1=> All Memory is Non-Secure
//   <i> Value for SAU->CTRL register bit ALLNS
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS  0

/*
// </e>
*/

/*
// <h>Initialize Security Attribution Unit (SAU) Address Regions
// <i>SAU configuration specifies regions to be one of:
// <i> - Secure and Non-Secure Callable
// <i> - Non-Secure
// <i>Note: All memory regions not configured by SAU are Secure
*/
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
//   <e>Initialize SAU Region 0
//   <i> Setup SAU Region 0 memory attributes
*/
#define SAU_INIT_REGION0    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0x10000000      /* start address of SAU region 0 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       0x101FFFFF      /* end address of SAU region 0 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC0       1
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 1
//   <i> Setup SAU Region 1 memory attributes
*/
#define SAU_INIT_REGION1    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     0x00200000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       0x003FFFFF

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC1       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 2
//   <i> Setup SAU Region 2 memory attributes
*/
#define SAU_INIT_REGION2    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START2     0x28200000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       0x283FFFFF

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC2       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 3
//   <i> Setup SAU Region 3 memory attributes
*/
#define SAU_INIT_REGION3    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START3     0x40000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END3       0x403FFFFF

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC3       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 4
//   <i> Setup SAU Region 4 memory attributes
*/
#define SAU_INIT_REGION4    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START4     0x00000000      /* start address of SAU region 4 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END4       0x00000000      /* end address of SAU region 4 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC4       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 5
//   <i> Setup SAU Region 5 memory attributes
*/
#define SAU_INIT_REGION5    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START5     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END5       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC5       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 6
//   <i> Setup SAU Region 6 memory attributes
*/
#define SAU_INIT_REGION6    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START6     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END6       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC6       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 7
//   <i> Setup SAU Region 7 memory attributes
*/
#define SAU_INIT_REGION7    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START7     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END7       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC7       0
/*
//   </e>
*/

/*
// </h>
*/

/*
// <e>Setup behaviour of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  1

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  1

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x80-0xFF
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      1

/*
//   <o>BusFault, HardFault, and NMI target
//     <0=> Secure state
//     <1=> Non-Secure state
//   <i> Value for SCB->AIRCR register bit BFHFNMINS
*/
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
// </e>
*/

/*
// <e>Setup behaviour of Floating Point Unit
*/
#define TZ_FPU_NS_USAGE 1

/*
// <o>Floating Point Unit usage
//     <0=> Secure state only
//     <3=> Secure and Non-Secure state
//   <i> Value for SCB->NSACR register bits CP10, CP11
*/
#define SCB_NSACR_CP10_11_VAL       3

/*
// <o>Treat floating-point registers as Secure
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit TS
*/
#define FPU_FPCCR_TS_VAL            0

/*
// <o>Clear on return (CLRONRET) accessibility
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for FPU->FPCCR register bit CLRONRETS
*/
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
// <o>Clear floating-point caller saved registers on exception return
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit CLRONRET
*/
#define FPU_FPCCR_CLRONRET_VAL      1

/*
// </e>
*/

/*
// <h>Setup Interrupt Target
*/

/*
//   <e>Initialize ITNS 0 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS0    0

/*
// Interrupts 0..31
//   <o.0>  Non-Secure Watchdog Reset Request      <0=> Secure state <1=> Non-Secure state
//   <o.1>  Non-Secure Watchdog interrupt          <0=> Secure state <1=> Non-Secure state
//   <o.2>  S32K Timer interrupt                   <0=> Secure state <1=> Non-Secure state
//   <o.3>  Timer 0 interrupt                      <0=> Secure state <1=> Non-Secure state
//   <o.4>  Timer 1 interrupt                      <0=> Secure state <1=> Non-Secure state
//   <o.5>  Dual Timer interrupt                   <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS0_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 1 (Interrupts 32..63)
*/
#define NVIC_INIT_ITNS1    0

/*
// Interrupts 32..63
//   <o.0>  UART 0 receive interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.1>  UART 0 transmit interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.2>  UART 1 receive interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.3>  UART 1 transmit interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.4>  UART 2 teceive interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.5>  UART 2 transmit interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.6>  UART 3 receive interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.7>  UART 3 transmit interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.8>  UART 4 receive interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.9>  UART 4 transmit interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.10> UART 0 combined interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.11> UART 1 combined interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.12> UART 2 combined interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.13> UART 3 combined interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.14> UART 4 combined interrupt              <0=> Secure state <1=> Non-Secure state
//   <o.15> UART 0, 1, 2, 3, 4 overflow interrupt  <0=> Secure state <1=> Non-Secure state
//   <o.16> Ethernet interrupt                     <0=> Secure state <1=> Non-Secure state
//   <o.17> Audio I2S interrupt                    <0=> Secure state <1=> Non-Secure state
//   <o.18> Touch Screen interrupt                 <0=> Secure state <1=> Non-Secure state
//   <o.19> SPI 0 (SPI Header) interrupt           <0=> Secure state <1=> Non-Secure state
//   <o.20> SPI 1 (CLCD) interrupt                 <0=> Secure state <1=> Non-Secure state
//   <o.21> SPI 2 (Shield ADC) interrupt           <0=> Secure state <1=> Non-Secure state
//   <o.22> SPI 3 (Shield 0 SPI) interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.23> SPI 4 (Shield 1 SPI) interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.24> DMA 0 error interrupt                  <0=> Secure state <1=> Non-Secure state
//   <o.25> DMA 0 terminal count interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.26> DMA 0 combined interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.27> DMA 1 error interrupt                  <0=> Secure state <1=> Non-Secure state
//   <o.28> DMA 1 terminal count interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.29> DMA 1 combined interrupt               <0=> Secure state <1=> Non-Secure state
//   <o.30> DMA 2 error interrupt                  <0=> Secure state <1=> Non-Secure state
//   <o.31> DMA 2 terminal count interrupt         <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS1_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 2 (Interrupts 64..95)
*/
#define NVIC_INIT_ITNS2    0

/*
// Interrupts 64..95
//   <o.0>  DMA 2 combined interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.1>  DMA 3 error interrupt            <0=> Secure state <1=> Non-Secure state
//   <o.2>  DMA 3 terminal count interrupt   <0=> Secure state <1=> Non-Secure state
//   <o.3>  DMA 3 combined interrupt         <0=> Secure state <1=> Non-Secure state
//   <o.4>  GPIO 0 combined interrupt        <0=> Secure state <1=> Non-Secure state
//   <o.5>  GPIO 1 combined interrupt        <0=> Secure state <1=> Non-Secure state
//   <o.6>  GPIO 2 combined interrupt        <0=> Secure state <1=> Non-Secure state
//   <o.7>  GPIO 3 combined interrupt        <0=> Secure state <1=> Non-Secure state
//   <o.8>  GPIO 0 individual interrupt ( 0) <0=> Secure state <1=> Non-Secure state
//   <o.9>  GPIO 0 individual interrupt ( 1) <0=> Secure state <1=> Non-Secure state
//   <o.10> GPIO 0 individual interrupt ( 2) <0=> Secure state <1=> Non-Secure state
//   <o.11> GPIO 0 individual interrupt ( 3) <0=> Secure state <1=> Non-Secure state
//   <o.12> GPIO 0 individual interrupt ( 4) <0=> Secure state <1=> Non-Secure state
//   <o.13> GPIO 0 individual interrupt ( 5) <0=> Secure state <1=> Non-Secure state
//   <o.14> GPIO 0 individual interrupt ( 6) <0=> Secure state <1=> Non-Secure state
//   <o.15> GPIO 0 individual interrupt ( 7) <0=> Secure state <1=> Non-Secure state
//   <o.16> GPIO 0 individual interrupt ( 8) <0=> Secure state <1=> Non-Secure state
//   <o.17> GPIO 0 individual interrupt ( 9) <0=> Secure state <1=> Non-Secure state
//   <o.18> GPIO 0 individual interrupt (10) <0=> Secure state <1=> Non-Secure state
//   <o.19> GPIO 0 individual interrupt (11) <0=> Secure state <1=> Non-Secure state
//   <o.20> GPIO 0 individual interrupt (12) <0=> Secure state <1=> Non-Secure state
//   <o.21> GPIO 0 individual interrupt (13) <0=> Secure state <1=> Non-Secure state
//   <o.22> GPIO 0 individual interrupt (14) <0=> Secure state <1=> Non-Secure state
//   <o.23> GPIO 0 individual interrupt (15) <0=> Secure state <1=> Non-Secure state
//   <o.24> GPIO 1 individual interrupt ( 0) <0=> Secure state <1=> Non-Secure state
//   <o.25> GPIO 1 individual interrupt ( 1) <0=> Secure state <1=> Non-Secure state
//   <o.26> GPIO 1 individual interrupt ( 2) <0=> Secure state <1=> Non-Secure state
//   <o.27> GPIO 1 individual interrupt ( 3) <0=> Secure state <1=> Non-Secure state
//   <o.28> GPIO 1 individual interrupt ( 4) <0=> Secure state <1=> Non-Secure state
//   <o.29> GPIO 1 individual interrupt ( 5) <0=> Secure state <1=> Non-Secure state
//   <o.30> GPIO 1 individual interrupt ( 6) <0=> Secure state <1=> Non-Secure state
//   <o.31> GPIO 1 individual interrupt ( 7) <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS2_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 3 (Interrupts 96..127)
*/
#define NVIC_INIT_ITNS3    0

/*
// Interrupts 96..127
//   <o.0>  GPIO 1 individual interrupt ( 8) <0=> Secure state <1=> Non-Secure state
//   <o.1>  GPIO 1 individual interrupt ( 9) <0=> Secure state <1=> Non-Secure state
//   <o.2>  GPIO 1 individual interrupt (10) <0=> Secure state <1=> Non-Secure state
//   <o.3>  GPIO 1 individual interrupt (11) <0=> Secure state <1=> Non-Secure state
//   <o.4>  GPIO 1 individual interrupt (12) <0=> Secure state <1=> Non-Secure state
//   <o.5>  GPIO 1 individual interrupt (13) <0=> Secure state <1=> Non-Secure state
//   <o.6>  GPIO 1 individual interrupt (14) <0=> Secure state <1=> Non-Secure state
//   <o.7>  GPIO 1 individual interrupt (15) <0=> Secure state <1=> Non-Secure state
//   <o.8>  GPIO 2 individual interrupt ( 0) <0=> Secure state <1=> Non-Secure state
//   <o.9>  GPIO 2 individual interrupt ( 1) <0=> Secure state <1=> Non-Secure state
//   <o.10> GPIO 2 individual interrupt ( 2) <0=> Secure state <1=> Non-Secure state
//   <o.11> GPIO 2 individual interrupt ( 3) <0=> Secure state <1=> Non-Secure state
//   <o.12> GPIO 2 individual interrupt ( 4) <0=> Secure state <1=> Non-Secure state
//   <o.13> GPIO 2 individual interrupt ( 5) <0=> Secure state <1=> Non-Secure state
//   <o.14> GPIO 2 individual interrupt ( 6) <0=> Secure state <1=> Non-Secure state
//   <o.15> GPIO 2 individual interrupt ( 7) <0=> Secure state <1=> Non-Secure state
//   <o.16> GPIO 2 individual interrupt ( 8) <0=> Secure state <1=> Non-Secure state
//   <o.17> GPIO 2 individual interrupt ( 9) <0=> Secure state <1=> Non-Secure state
//   <o.18> GPIO 2 individual interrupt (10) <0=> Secure state <1=> Non-Secure state
//   <o.19> GPIO 2 individual interrupt (11) <0=> Secure state <1=> Non-Secure state
//   <o.20> GPIO 2 individual interrupt (12) <0=> Secure state <1=> Non-Secure state
//   <o.21> GPIO 2 individual interrupt (13) <0=> Secure state <1=> Non-Secure state
//   <o.22> GPIO 2 individual interrupt (14) <0=> Secure state <1=> Non-Secure state
//   <o.23> GPIO 2 individual interrupt (15) <0=> Secure state <1=> Non-Secure state
//   <o.24> GPIO 3 individual interrupt ( 0) <0=> Secure state <1=> Non-Secure state
//   <o.25> GPIO 3 individual interrupt ( 1) <0=> Secure state <1=> Non-Secure state
//   <o.26> GPIO 3 individual interrupt ( 2) <0=> Secure state <1=> Non-Secure state
//   <o.27> GPIO 3 individual interrupt ( 3) <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS3_VAL      0x00000000

/*
//   </e>
*/

/*
// </h>
*/



/*
    max 128 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

/**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
           registers SAU_RNR, SAU_RBAR, and SAU_RLAR
 */
__STATIC_INLINE void TZ_SAU_Setup (void)
{

#if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

  #if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
  #endif

  #if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
  #endif

  #if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
  #endif

  #if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
  #endif

  #if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
    SAU_INIT_REGION(4);
  #endif

  #if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
    SAU_INIT_REGION(5);
  #endif

  #if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
    SAU_INIT_REGION(6);
  #endif

  #if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
    SAU_INIT_REGION(7);
  #endif

  /* repeat this for all possible SAU regions */

#endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */


  #if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
  #endif

  #if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk    )) |
                   ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

    SCB->AIRCR = (SCB->AIRCR & ~(SCB_AIRCR_VECTKEY_Msk   | SCB_AIRCR_SYSRESETREQS_Msk |
                                 SCB_AIRCR_BFHFNMINS_Msk | SCB_AIRCR_PRIS_Msk          ))                    |
                   ((0x05FAU                    << SCB_AIRCR_VECTKEY_Pos)      & SCB_AIRCR_VECTKEY_Msk)      |
                   ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                   ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk)         |
                   ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk);
  #endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

  #if defined (__FPU_USED) && (__FPU_USED == 1U) && \
      defined (TZ_FPU_NS_USAGE) && (TZ_FPU_NS_USAGE == 1U)

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP10_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                   ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                   ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                   ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
  #endif

  #if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
    NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
    NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
    NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
    NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
  #endif

  /* repeat this for all possible ITNS elements */

}

#endif  /* PARTITION_IOTKit_CM33_H */
