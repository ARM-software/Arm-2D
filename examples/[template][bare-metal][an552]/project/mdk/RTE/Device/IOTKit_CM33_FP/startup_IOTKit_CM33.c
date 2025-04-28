/******************************************************************************
 * @file     startup_IOTKit_CM33.c
 * @brief    CMSIS Startup File for IOTKit_CM33 Device
 ******************************************************************************/
/* Copyright (c) 2015 - 2022 ARM LIMITED

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

#if   defined (IOTKit_CM33)    || defined (IOTKit_CM33_VHT)
  #include "IOTKit_CM33.h"
#elif defined (IOTKit_CM33_FP) || defined (IOTKit_CM33_FP_VHT)
  #include "IOTKit_CM33_FP.h"
#else
  #error device not specified!
#endif

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler               (void) __attribute__ ((weak));
void MemManage_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));

/* Core IoT Interrupts */
void NONSEC_WATCHDOG_RESET_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void NONSEC_WATCHDOG_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void S32K_TIMER_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void DUALTIMER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void MPC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void PPC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void MSC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void BRIDGE_ERROR_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));

/* External Interrupts */
void UART0RX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0TX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1RX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1TX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2RX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2TX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3RX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3TX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4RX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4TX_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UARTOVF_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void ETHERNET_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void TSC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI4_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_ERROR_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_TC_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_ERROR_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_TC_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_ERROR_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_TC_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA3_ERROR_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA3_TC_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA3_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_6_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_8_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_9_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_10_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_11_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_12_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_13_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_14_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_15_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_6_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_8_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_9_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_10_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_11_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_12_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_13_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_14_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_15_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_6_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_8_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_9_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_10_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_11_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_12_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_13_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_14_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_15_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));

/* VSI Interrupts */
#if defined (IOTKit_CM33_VHT) || defined (IOTKit_CM33_FP_VHT)
void ARM_VSI0_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI3_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI4_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI5_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI6_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ARM_VSI7_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
#endif


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[256];
       const VECTOR_TABLE_Type __VECTOR_TABLE[256] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
  Reset_Handler,                            /*     Reset Handler */
  NMI_Handler,                              /* -14 NMI Handler */
  HardFault_Handler,                        /* -13 Hard Fault Handler */
  MemManage_Handler,                        /* -12 MPU Fault Handler */
  BusFault_Handler,                         /* -11 Bus Fault Handler */
  UsageFault_Handler,                       /* -10 Usage Fault Handler */
  SecureFault_Handler,                      /*  -9 Secure Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  SVC_Handler,                              /*  -5 SVC Handler */
  DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
  0,                                        /*     Reserved */
  PendSV_Handler,                           /*  -2 PendSV Handler */
  SysTick_Handler,                          /*  -1 SysTick Handler */

  /* Core IoT Interrupts */
  NONSEC_WATCHDOG_RESET_Handler,            /*   0 Non-Secure Watchdog Reset Handler */
  NONSEC_WATCHDOG_Handler,                  /*   1 Non-Secure Watchdog Handler */
  S32K_TIMER_Handler,                       /*   2 S32K Timer Handler */
  TIMER0_Handler,                           /*   3 TIMER 0 Handler */
  TIMER1_Handler,                           /*   4 TIMER 1 Handler */
  DUALTIMER_Handler,                        /*   5 Dual Timer Handler */
  0,                                        /*   6 Reserved */
  0,                                        /*   7 Reserved */
  0,                                        /*   8 Reserved */
  MPC_Handler,                              /*   9 MPC Combined (Secure) Handler */
  PPC_Handler,                              /*  10 PPC Combined (Secure) Handler */
  MSC_Handler,                              /*  11 MSC Combined (Secure) Handler */
  BRIDGE_ERROR_Handler,                     /*  12 Bridge Error Combined (Secure) Handler */
  0,                                        /*  13 Reserved */
  0,                                        /*  14 Reserved */
  0,                                        /*  15 Reserved */
  0,                                        /*  16 Reserved */
  0,                                        /*  17 Reserved */
  0,                                        /*  18 Reserved */
  0,                                        /*  19 Reserved */
  0,                                        /*  20 Reserved */
  0,                                        /*  21 Reserved */
  0,                                        /*  22 Reserved */
  0,                                        /*  23 Reserved */
  0,                                        /*  24 Reserved */
  0,                                        /*  25 Reserved */
  0,                                        /*  26 Reserved */
  0,                                        /*  27 Reserved */
  0,                                        /*  28 Reserved */
  0,                                        /*  29 Reserved */
  0,                                        /*  30 Reserved */
  0,                                        /*  31 Reserved */

  /* External Interrupts */
  UART0RX_Handler,                          /*  32 UART 0 RX Handler */
  UART0TX_Handler,                          /*  33 UART 0 TX Handler */
  UART1RX_Handler,                          /*  34 UART 1 RX Handler */
  UART1TX_Handler,                          /*  35 UART 1 TX Handler */
  UART2RX_Handler,                          /*  36 UART 2 RX Handler */
  UART2TX_Handler,                          /*  37 UART 2 TX Handler */
  UART3RX_Handler,                          /*  38 UART 2 RX Handler */
  UART3TX_Handler,                          /*  39 UART 2 TX Handler */
  UART4RX_Handler,                          /*  40 UART 2 RX Handler */
  UART4TX_Handler,                          /*  41 UART 2 TX Handler */
  UART0_Handler,                            /*  42 UART 0 combined Handler */
  UART1_Handler,                            /*  43 UART 1 combined Handler */
  UART2_Handler,                            /*  44 UART 2 combined Handler */
  UART3_Handler,                            /*  45 UART 3 combined Handler */
  UART4_Handler,                            /*  46 UART 4 combined Handler */
  UARTOVF_Handler,                          /*  47 UART 0,1,2,3,4 Overflow Handler */
  ETHERNET_Handler ,                        /*  48 Ethernet Handler */
  I2S_Handler,                              /*  49 I2S Handler */
  TSC_Handler,                              /*  50 Touch Screen Handler */
  SPI0_Handler,                             /*  51 SPI 0 Handler */
  SPI1_Handler,                             /*  52 SPI 1 Handler */
  SPI2_Handler,                             /*  53 SPI 2 Handler */
  SPI3_Handler,                             /*  54 SPI 3 Handler */
  SPI4_Handler,                             /*  55 SPI 4 Handler */
  DMA0_ERROR_Handler,                       /*  56 DMA 0 Error Handler */
  DMA0_TC_Handler,                          /*  57 DMA 0 Terminal Count Handler */
  DMA0_Handler,                             /*  58 DMA 0 Combined Handler */
  DMA1_ERROR_Handler,                       /*  59 DMA 1 Error Handler */
  DMA1_TC_Handler,                          /*  60 DMA 1 Terminal Count Handler */
  DMA1_Handler,                             /*  61 DMA 1 Combined Handler */
  DMA2_ERROR_Handler,                       /*  62 DMA 2 Error Handler */
  DMA2_TC_Handler,                          /*  63 DMA 2 Terminal Count Handler */
  DMA2_Handler,                             /*  64 DMA 2 Combined Handler */
  DMA3_ERROR_Handler,                       /*  65 DMA 3 Error Handler */
  DMA3_TC_Handler,                          /*  66 DMA 3 Terminal Count Handler */
  DMA3_Handler,                             /*  67 DMA 3 Combined Handler */
  GPIO0_Handler,                            /*  68 GPIO 0 Combined Handler */
  GPIO1_Handler,                            /*  69 GPIO 1 Combined Handler */
  GPIO2_Handler,                            /*  70 GPIO 2 Combined Handler */
  GPIO3_Handler,                            /*  71 GPIO 3 Combined Handler */
  GPIO0_0_Handler,                          /*  72 */      /* All P0 I/O pins used as irq source */
  GPIO0_1_Handler,                          /*  73 */      /* There are 16 pins in total         */
  GPIO0_2_Handler,                          /*  74 */
  GPIO0_3_Handler,                          /*  75 */
  GPIO0_4_Handler,                          /*  76 */
  GPIO0_5_Handler,                          /*  77 */
  GPIO0_6_Handler,                          /*  78 */
  GPIO0_7_Handler,                          /*  79 */
  GPIO0_8_Handler,                          /*  80 */
  GPIO0_9_Handler,                          /*  81 */
  GPIO0_10_Handler,                         /*  82 */
  GPIO0_11_Handler,                         /*  83 */
  GPIO0_12_Handler,                         /*  84 */
  GPIO0_13_Handler,                         /*  85 */
  GPIO0_14_Handler,                         /*  86 */
  GPIO0_15_Handler,                         /*  87 */
  GPIO1_0_Handler,                          /*  88 */      /* All P1 I/O pins used as irq source */
  GPIO1_1_Handler,                          /*  89 */      /* There are 16 pins in total         */
  GPIO1_2_Handler,                          /*  90 */
  GPIO1_3_Handler,                          /*  91 */
  GPIO1_4_Handler,                          /*  92 */
  GPIO1_5_Handler,                          /*  93 */
  GPIO1_6_Handler,                          /*  94 */
  GPIO1_7_Handler,                          /*  95 */
  GPIO1_8_Handler,                          /*  96 */
  GPIO1_9_Handler,                          /*  97 */
  GPIO1_10_Handler,                         /*  98 */
  GPIO1_11_Handler,                         /*  99 */
  GPIO1_12_Handler,                         /* 100 */
  GPIO1_13_Handler,                         /* 101 */
  GPIO1_14_Handler,                         /* 102 */
  GPIO1_15_Handler,                         /* 103 */
  GPIO2_0_Handler,                          /* 104 */      /* All P2 I/O pins used as irq source */
  GPIO2_1_Handler,                          /* 105 */      /* There are 16 pins in total         */
  GPIO2_2_Handler,                          /* 106 */
  GPIO2_3_Handler,                          /* 107 */
  GPIO2_4_Handler,                          /* 108 */
  GPIO2_5_Handler,                          /* 109 */
  GPIO2_6_Handler,                          /* 110 */
  GPIO2_7_Handler,                          /* 111 */
  GPIO2_8_Handler,                          /* 112 */
  GPIO2_9_Handler,                          /* 113 */
  GPIO2_10_Handler,                         /* 114 */
  GPIO2_11_Handler,                         /* 115 */
  GPIO2_12_Handler,                         /* 116 */
  GPIO2_13_Handler,                         /* 117 */
  GPIO2_14_Handler,                         /* 118 */
  GPIO2_15_Handler,                         /* 119 */
  GPIO3_0_Handler,                          /* 120 */      /* All P3 I/O pins used as irq source */
  GPIO3_1_Handler,                          /* 121 */      /* There are 4 pins in total          */
  GPIO3_2_Handler,                          /* 122 */
  GPIO3_3_Handler,                          /* 123 */
  0,                                        /* 124 Reserved */
  0,                                        /* 125 Reserved */
  0,                                        /* 126 Reserved */
  0,                                        /* 127 Reserved */
  0,                                        /* 128 Reserved */
  0,                                        /* 129 Reserved */
  0,                                        /* 130 Reserved */
  0,                                        /* 131 Reserved */
  0,                                        /* 132 Reserved */
  0,                                        /* 133 Reserved */
  0,                                        /* 134 Reserved */
  0,                                        /* 135 Reserved */
  0,                                        /* 136 Reserved */
  0,                                        /* 137 Reserved */
  0,                                        /* 138 Reserved */
  0,                                        /* 139 Reserved */
  0,                                        /* 140 Reserved */
  0,                                        /* 141 Reserved */
  0,                                        /* 142 Reserved */
  0,                                        /* 143 Reserved */
  0,                                        /* 144 Reserved */
  0,                                        /* 145 Reserved */
  0,                                        /* 146 Reserved */
  0,                                        /* 147 Reserved */
  0,                                        /* 148 Reserved */
  0,                                        /* 149 Reserved */
  0,                                        /* 150 Reserved */
  0,                                        /* 151 Reserved */
  0,                                        /* 152 Reserved */
  0,                                        /* 153 Reserved */
  0,                                        /* 154 Reserved */
  0,                                        /* 155 Reserved */
  0,                                        /* 156 Reserved */
  0,                                        /* 157 Reserved */
  0,                                        /* 158 Reserved */
  0,                                        /* 159 Reserved */
  0,                                        /* 160 Reserved */
  0,                                        /* 161 Reserved */
  0,                                        /* 162 Reserved */
  0,                                        /* 163 Reserved */
  0,                                        /* 164 Reserved */
  0,                                        /* 165 Reserved */
  0,                                        /* 166 Reserved */
  0,                                        /* 167 Reserved */
  0,                                        /* 168 Reserved */
  0,                                        /* 169 Reserved */
  0,                                        /* 170 Reserved */
  0,                                        /* 171 Reserved */
  0,                                        /* 172 Reserved */
  0,                                        /* 173 Reserved */
  0,                                        /* 174 Reserved */
  0,                                        /* 175 Reserved */
  0,                                        /* 176 Reserved */
  0,                                        /* 177 Reserved */
  0,                                        /* 178 Reserved */
  0,                                        /* 179 Reserved */
  0,                                        /* 180 Reserved */
  0,                                        /* 181 Reserved */
  0,                                        /* 182 Reserved */
  0,                                        /* 183 Reserved */
  0,                                        /* 184 Reserved */
  0,                                        /* 185 Reserved */
  0,                                        /* 186 Reserved */
  0,                                        /* 187 Reserved */
  0,                                        /* 188 Reserved */
  0,                                        /* 189 Reserved */
  0,                                        /* 190 Reserved */
  0,                                        /* 191 Reserved */
  0,                                        /* 192 Reserved */
  0,                                        /* 193 Reserved */
  0,                                        /* 194 Reserved */
  0,                                        /* 195 Reserved */
  0,                                        /* 196 Reserved */
  0,                                        /* 197 Reserved */
  0,                                        /* 198 Reserved */
  0,                                        /* 199 Reserved */
  0,                                        /* 200 Reserved */
  0,                                        /* 201 Reserved */
  0,                                        /* 202 Reserved */
  0,                                        /* 203 Reserved */
  0,                                        /* 204 Reserved */
  0,                                        /* 205 Reserved */
  0,                                        /* 206 Reserved */
  0,                                        /* 207 Reserved */
  0,                                        /* 208 Reserved */
  0,                                        /* 209 Reserved */
  0,                                        /* 210 Reserved */
  0,                                        /* 211 Reserved */
  0,                                        /* 212 Reserved */
  0,                                        /* 213 Reserved */
  0,                                        /* 214 Reserved */
  0,                                        /* 215 Reserved */
  0,                                        /* 216 Reserved */
  0,                                        /* 217 Reserved */
  0,                                        /* 218 Reserved */
  0,                                        /* 219 Reserved */
  0,                                        /* 220 Reserved */
  0,                                        /* 221 Reserved */
  0,                                        /* 222 Reserved */
  0,                                        /* 223 Reserved */
#if defined (IOTKit_CM33_VHT) || defined (IOTKit_CM33_FP_VHT)
  ARM_VSI0_Handler,                         /* 224 VSI 0 interrupt */
  ARM_VSI1_Handler,                         /* 225 VSI 1 interrupt */
  ARM_VSI2_Handler,                         /* 226 VSI 2 interrupt */
  ARM_VSI3_Handler,                         /* 227 VSI 3 interrupt */
  ARM_VSI4_Handler,                         /* 228 VSI 4 interrupt */
  ARM_VSI5_Handler,                         /* 229 VSI 5 interrupt */
  ARM_VSI6_Handler,                         /* 230 VSI 6 interrupt */
  ARM_VSI7_Handler                          /* 231 VSI 7 interrupt */
#else
  0,                                        /* 224 Reserved */
  0,                                        /* 225 Reserved */
  0,                                        /* 226 Reserved */
  0,                                        /* 227 Reserved */
  0,                                        /* 228 Reserved */
  0,                                        /* 229 Reserved */
  0,                                        /* 230 Reserved */
  0                                         /* 231 Reserved */
#endif
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));

  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
  __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

  SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif
