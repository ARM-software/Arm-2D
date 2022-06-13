/*----------------------------------------------------------------------------
 * Name:    SMM_MPS2.h
 * Purpose: SMM MPS2 definitions
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2011 - 2017 ARM LIMITED

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

#ifndef SMM_MPS2_H_
#define SMM_MPS2_H_

#include "Device.h"                         /* device specific header file */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*----------------------------------------------------------------------------
  FPGA System Register declaration
 *----------------------------------------------------------------------------*/
typedef struct
{
  __IO uint32_t LED;             /* Offset: 0x000 (R/W)  LED connections */
                                 /*                         [31:2] : Reserved */
                                 /*                          [1:0] : LEDs */
       uint32_t RESERVED0[1];
  __IO uint32_t BUTTON;          /* Offset: 0x008 (R/W)  Buttons */
                                 /*                         [31:2] : Reserved */
                                 /*                          [1:0] : Buttons */
       uint32_t RESERVED1[1];
  __IO uint32_t CLK1HZ;          /* Offset: 0x010 (R/W)  1Hz up counter */
  __IO uint32_t CLK100HZ;        /* Offset: 0x014 (R/W)  100Hz up counter */
  __IO uint32_t COUNTER;         /* Offset: 0x018 (R/W)  Cycle Up Counter */
                                 /*                         Increments when 32-bit prescale counter reach zero */
       uint32_t RESERVED2[1];
  __IO uint32_t PRESCALE;        /* Offset: 0x020 (R/W)  Prescaler */
                                 /*                         Bit[31:0] : reload value for prescale counter */
  __IO uint32_t PSCNTR;          /* Offset: 0x024 (R/W)  32-bit Prescale counter */
                                 /*                         current value of the pre-scaler counter */
								 /*                         The Cycle Up Counter increment when the prescale down counter reach 0 */
								 /*                         The pre-scaler counter is reloaded with PRESCALE after reaching 0 */
       uint32_t RESERVED3[9];
  __IO uint32_t MISC;            /* Offset: 0x04C (R/W)  Misc control */
                                 /*                         [31:10] : Reserved */
                                 /*                            [9] : SHIELD_1_SPI_nCS */
                                 /*                            [8] : SHIELD_0_SPI_nCS */
                                 /*                            [7] : ADC_SPI_nCS */
                                 /*                            [6] : CLCD_BL_CTRL */
                                 /*                            [5] : CLCD_RD */
                                 /*                            [4] : CLCD_RS */
                                 /*                            [3] : CLCD_RESET */
                                 /*                            [2] : RESERVED */
                                 /*                            [1] : SPI_nSS */
                                 /*                            [0] : CLCD_CS */
} MPS2_FPGAIO_TypeDef;


/*----------------------------------------------------------------------------
  SCC Register declaration
 *----------------------------------------------------------------------------*/
typedef struct
{
  __IO uint32_t CFG_REG0;        /* Offset: 0x000 (R/W) Reserved */
                                 /*                         [31:0] : Reserved */
  __IO uint32_t CFG_REG1;        /* Offset: 0x004 (R/W)  Controls the MCC user LEDs */
                                 /*                         [31:8] : Reserved */
                                 /*                          [7:0] : MCC LEDs: 0 = OFF 1 = ON */
  __IO uint32_t CFG_REG2;        /* Offset: 0x008 (R/W) Reserved */
                                 /*                         [31:0] : Reserved */
  __I  uint32_t CFG_REG3;        /* Offset: 0x00C (R/ )  Denotes the state of the MCC user switches */
                                 /*                         [31:8] : Reserved */
                                 /*                          [7:0] : MCC switches: 0 = OFF 1 = ON */
  __I  uint32_t CFG_REG4;        /* Offset: 0x010 (R/ )  Denotes the board revision */
                                 /*                         [31:4] : Reserved */
                                 /*                          [3:0] : Board Revision */
       uint32_t RESERVED1[35];
  __IO uint32_t SYS_CFGDATA_RTN; /* Offset: 0x0A0 (R/W)  User data register */
                                 /*                         [31:0] : Data */
  __IO uint32_t SYS_CFGDATA_OUT; /* Offset: 0x0A4 (R/W)  User data register */
                                 /*                         [31:0] : Data */
  __IO uint32_t SYS_CFGCTRL;     /* Offset: 0x0A8 (R/W)  Control register */
                                 /*                           [31] : Start (generates interrupt on write to this bit) */
                                 /*                           [30] : R/W access */
                                 /*                        [29:26] : Reserved */
                                 /*                        [25:20] : Function value */
                                 /*                        [19:12] : Reserved */
                                 /*                         [11:0] : Device (value of 0/1/2 for supported clocks) */
  __IO uint32_t SYS_CFGSTAT;     /* Offset: 0x0AC (R/W)  Contains status information */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Error */
                                 /*                            [0] : Complete */
  __IO uint32_t RESERVED2[20];
  __IO uint32_t SCC_DLL;         /* Offset: 0x100 (R/W)  DLL Lock Register */
                                 /*                        [31:24] : DLL LOCK MASK[7:0] - Indicate if the DLL locked is masked */
                                 /*                        [23:16] : DLL LOCK MASK[7:0] - Indicate if the DLLs are locked or unlocked */
                                 /*                         [15:1] : Reserved */
                                 /*                            [0] : This bit indicates if all enabled DLLs are locked */
       uint32_t RESERVED3[957];
  __I  uint32_t SCC_AID;         /* Offset: 0xFF8 (R/ )  SCC AID Register */
                                 /*                        [31:24] : FPGA build number */
                                 /*                        [23:20] : V2M-MPS2 target board revision (A = 0, B = 1) */
                                 /*                        [19:11] : Reserved */
                                 /*                           [10] : if “1” SCC_SW register has been implemented */
                                 /*                            [9] : if “1” SCC_LED register has been implemented */
                                 /*                            [8] : if “1” DLL lock register has been implemented */
                                 /*                          [7:0] : number of SCC configuration register */
  __I  uint32_t SCC_ID;          /* Offset: 0xFFC (R/ )  Contains information about the FPGA image */
                                 /*                        [31:24] : Implementer ID: 0x41 = ARM */
                                 /*                        [23:20] : Application note IP variant number */
                                 /*                        [19:16] : IP Architecture: 0x4 =AHB */
                                 /*                         [15:4] : Primary part number: 386 = AN386 */
                                 /*                          [3:0] : Application note IP revision number */
} MPS2_SCC_TypeDef;


/*----------------------------------------------------------------------------
  SSP Peripheral declaration  (Document DDI0194G_ssp_pl022_r1p3_trm.pdf)
 *----------------------------------------------------------------------------*/
typedef struct
{
  __IO uint32_t CR0;             /* Offset: 0x000 (R/W)  Control register 0 */
                                 /*                        [31:16] : Reserved */
                                 /*                         [15:8] : Serial clock rate */
                                 /*                            [7] : SSPCLKOUT phase,    applicable to Motorola SPI frame format only */
                                 /*                            [6] : SSPCLKOUT polarity, applicable to Motorola SPI frame format only */
                                 /*                          [5:4] : Frame format */
                                 /*                          [3:0] : Data Size Select */
  __IO uint32_t CR1;             /* Offset: 0x004 (R/W)  Control register 1 */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : Slave-mode output disable */
                                 /*                            [2] : Master or slave mode select */
                                 /*                            [1] : Synchronous serial port enable */
                                 /*                            [0] : Loop back mode */
  __IO uint32_t DR;              /* Offset: 0x008 (R/W)  Data register */
                                 /*                        [31:16] : Reserved */
                                 /*                         [15:0] : Transmit/Receive FIFO */
  __I  uint32_t SR;              /* Offset: 0x00C (R/ )  Status register */
                                 /*                         [31:5] : Reserved */
                                 /*                            [4] : PrimeCell SSP busy flag */
                                 /*                            [3] : Receive FIFO full */
                                 /*                            [2] : Receive FIFO not empty */
                                 /*                            [1] : Transmit FIFO not full */
                                 /*                            [0] : Transmit FIFO empty */
  __IO uint32_t CPSR;            /* Offset: 0x010 (R/W)  Clock prescale register */
                                 /*                         [31:8] : Reserved */
                                 /*                          [8:0] : Clock prescale divisor */
  __IO uint32_t IMSC;            /* Offset: 0x014 (R/W)  Interrupt mask set or clear register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : Transmit FIFO interrupt mask */
                                 /*                            [2] : Receive FIFO interrupt mask */
                                 /*                            [1] : Receive timeout interrupt mask */
                                 /*                            [0] : Receive overrun interrupt mask */
  __I  uint32_t RIS;             /* Offset: 0x018 (R/ )  Raw interrupt status register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : raw interrupt state, prior to masking, of the SSPTXINTR interrupt */
                                 /*                            [2] : raw interrupt state, prior to masking, of the SSPRXINTR interrupt */
                                 /*                            [1] : raw interrupt state, prior to masking, of the SSPRTINTR interrupt */
                                 /*                            [0] : raw interrupt state, prior to masking, of the SSPRORINTR interrupt */
  __I  uint32_t MIS;             /* Offset: 0x01C (R/ )  Masked interrupt status register */
                                 /*                         [31:4] : Reserved */
                                 /*                            [3] : transmit FIFO masked interrupt state, after masking, of the SSPTXINTR interrupt */
                                 /*                            [2] : receive FIFO masked interrupt state, after masking, of the SSPRXINTR interrupt */
                                 /*                            [1] : receive timeout masked interrupt state, after masking, of the SSPRTINTR interrupt */
                                 /*                            [0] : receive over run masked interrupt status, after masking, of the SSPRORINTR interrupt */
  __O  uint32_t ICR;             /* Offset: 0x020 ( /W)  Interrupt clear register */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Clears the SSPRTINTR interrupt */
                                 /*                            [0] : Clears the SSPRORINTR interrupt */
  __IO uint32_t DMACR;           /* Offset: 0x024 (R/W)  DMA control register */
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : Transmit DMA Enable */
                                 /*                            [0] : Receive DMA Enable */
} MPS2_SSP_TypeDef;


/*----------------------------------------------------------------------------
  I2C Peripheral declaration
 *----------------------------------------------------------------------------*/
typedef struct
{
  union {
  __O  uint32_t CONTROLS;        /* Offset: 0x000 ( /W)  CONTROL Set Register */
  __I  uint32_t CONTROL;         /* Offset: 0x000 (R/ )  CONTROL Status Register */
  };
                                 /*                         [31:2] : Reserved */
                                 /*                            [1] : SDA */
                                 /*                            [0] : SCL */
  __O    uint32_t  CONTROLC;     /* Offset: 0x004 ( /W)  CONTROL Clear Register */
} MPS2_I2C_TypeDef;


/*----------------------------------------------------------------------------
  I2S Peripheral declaration
 *----------------------------------------------------------------------------*/
typedef struct
{
  __IO uint32_t CONTROL;         /* Offset: 0x000 (R/W)  CONTROL Register */
                                 /*                           [17] : Audio Codec reset */
                                 /*                           [16] : FIFO reset */
                                 /*                       [14..12] : RX Buffer Water Level */
                                 /*                        [10..8] : TX Buffer Water Level */
                                 /*                            [3] : RX IRQ Enable */
                                 /*                            [2] : RX Enable */
                                 /*                            [1] : TX IRQ Enable */
                                 /*                            [0] : TX Enable */
  __I  uint32_t STATUS;          /* Offset: 0x004 (R/ )  STATUS Register */
                                 /*                            [5] : RX Buffer Full */
                                 /*                            [4] : RX Buffer Empty */
                                 /*                            [3] : TX Buffer Full */
                                 /*                            [2] : TX Buffer Empty */
                                 /*                            [1] : RX Buffer alert */
                                 /*                            [0] : TX Buffer alert */
  union {
  __I  uint32_t ERROR;           /* Offset: 0x008 (R/ )  Error Status Register */
  __O  uint32_t ERRORCLR;        /* Offset: 0x008 ( /W)  Error Clear Register */
  };
                                 /*                            [1] : RX error */
                                 /*                            [0] : TX error */
  __IO uint32_t DIVIDE;          /* Offset: 0x00C (R/W)  Divide ratio Register */
                                 /*                         [9..0] : TX error (default 0x80) */
  __O  uint32_t TXBUF;           /* Offset: 0x010 ( /W)  Transmit Buffer */
                                 /*                        [31..16] : Left channel */
                                 /*                         [15..0] : Right channel */
  __I  uint32_t RXBUF;           /* Offset: 0x014 (R/ )  Receive Buffer */
                                 /*                        [31..16] : Left channel */
                                 /*                         [15..0] : Right channel */
       uint32_t RESERVED0[186];
  __IO uint32_t ITCR;            /* Offset: 0x300 (R/W)  Integration Test Control Register */
                                 /*                             [0] : ITEN */
  __O  uint32_t ITIP1;           /* Offset: 0x304 (R/W)  Integration Test Input Register 1 */
                                 /*                             [0] : SDIN */
  __O  uint32_t ITOP1;           /* Offset: 0x308 (R/W)  Integration Test Output Register 1 */
                                 /*                             [3] : IRQOUT */
                                 /*                             [2] : LRCK */
                                 /*                             [1] : SCLK */
                                 /*                             [0] : SDOUT */
} MPS2_I2S_TypeDef;


/*----------------------------------------------------------------------------
  SMSC9220 Register Definitions
 *----------------------------------------------------------------------------*/
typedef struct
{
  __I   uint32_t  RX_DATA_PORT;  /* Offset: 0x000 (R/ )  Receive FIFO Ports */
        uint32_t  RESERVED0[7];
  __O   uint32_t  TX_DATA_PORT;  /* Offset: 0x020 ( /W)  Transmit FIFO Ports */
        uint32_t  RESERVED1[7];
  __I   uint32_t  RX_STAT_PORT;  /* Offset: 0x040 (R/ )  Receive FIFO status port */
  __I   uint32_t  RX_STAT_PEEK;  /* Offset: 0x044 (R/ )  Receive FIFO status peek */
  __I   uint32_t  TX_STAT_PORT;  /* Offset: 0x048 (R/ )  Transmit FIFO status port */
  __I   uint32_t  TX_STAT_PEEK;  /* Offset: 0x04C (R/ )  Transmit FIFO status peek */
  __I   uint32_t  ID_REV;        /* Offset: 0x050 (R/ )  Chip ID and Revision */
  __IO  uint32_t  IRQ_CFG;       /* Offset: 0x054 (R/W)  Main Interrupt Configuration */
  __IO  uint32_t  INT_STS;       /* Offset: 0x058 (R/W)  Interrupt Status */
  __IO  uint32_t  INT_EN;        /* Offset: 0x05C (R/W)  Interrupt Enable Register */
        uint32_t  RESERVED3;     /* Offset: 0x060 (R/W)  Reserved for future use */
  __I   uint32_t  BYTE_TEST;     /* Offset: 0x064 (R/ )  Read-only byte order testing register 87654321h */
  __IO  uint32_t  FIFO_INT;      /* Offset: 0x068 (R/W)  FIFO Level Interrupts */
  __IO  uint32_t  RX_CFG;        /* Offset: 0x06C (R/W)  Receive Configuration */
  __IO  uint32_t  TX_CFG;        /* Offset: 0x070 (R/W)  Transmit Configuration */
  __IO  uint32_t  HW_CFG;        /* Offset: 0x074 (R/W)  Hardware Configuration */
  __IO  uint32_t  RX_DP_CTL;     /* Offset: 0x078 (R/W)  RX Datapath Control */
  __I   uint32_t  RX_FIFO_INF;   /* Offset: 0x07C (R/ )  Receive FIFO Information */
  __I   uint32_t  TX_FIFO_INF;   /* Offset: 0x080 (R/W)  Transmit FIFO Information */
  __IO  uint32_t  PMT_CTRL;      /* Offset: 0x084 (R/W)  Power Management Control */
  __IO  uint32_t  GPIO_CFG;      /* Offset: 0x088 (R/W)  General Purpose IO Configuration */
  __IO  uint32_t  GPT_CFG;       /* Offset: 0x08C (R/W)  General Purpose Timer Configuration */
  __I   uint32_t  GPT_CNT;       /* Offset: 0x090 (R/W)  General Purpose Timer Count */
        uint32_t  RESERVED4;     /* Offset: 0x094 (R/W)  Reserved for future use */
  __IO  uint32_t  ENDIAN;        /* Offset: 0x098 (R/W)  WORD SWAP Register */
  __I   uint32_t  FREE_RUN;      /* Offset: 0x09C (R/W)  Free Run Counter */
  __I   uint32_t  RX_DROP;       /* Offset: 0x0A0 (R/W)  RX Dropped Frames Counter */
  __IO  uint32_t  MAC_CSR_CMD;   /* Offset: 0x0A4 (R/W)  MAC CSR Synchronizer Command */
  __IO  uint32_t  MAC_CSR_DATA;  /* Offset: 0x0A8 (R/W)  MAC CSR Synchronizer Data */
  __IO  uint32_t  AFC_CFG;       /* Offset: 0x0AC (R/W)  Automatic Flow Control Configuration */
  __IO  uint32_t  E2P_CMD;       /* Offset: 0x0B0 (R/W)  EEPROM Command */
  __IO  uint32_t  E2P_DATA;      /* Offset: 0x0B4 (R/W)  EEPROM Data */
} SMSC9220_TypeDef;


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
#define MPS2_SSP0_BASE                 (0x40205000UL)       /* User SSP PL022 Base Address */
#define MPS2_SSP1_BASE                 (0x40206000UL)       /* CLCD SSP PL022 Base Address */
#define MPS2_I2C0_BASE                 (0x40207000UL)       /* Touch Screen I2C Base Address */
#define MPS2_I2C1_BASE                 (0x40208000UL)       /* Audio Interface I2C Base Address */
#define MPS2_SSP2_BASE                 (0x40209000UL)       /* ADC SPI PL022 Base Address */
#define MPS2_SSP3_BASE                 (0x4020A000UL)       /* Shield 0 SPI PL022 Base Address */
#define MPS2_SSP4_BASE                 (0x4020B000UL)       /* Shield 1 SPI PL022 Base Address */
#define MPS2_I2C2_BASE                 (0x4020C000UL)       /* Shield 0 SBCon Base Address */
#define MPS2_I2C3_BASE                 (0x4020D000UL)       /* Shield 1 SBCon Base Address */
#define MPS2_SCC_BASE                  (0x40300000UL)       /* SCC Base Address */
#define MPS2_I2S_BASE                  (0x40301000UL)       /* Audio Interface I2S Base Address */
#define MPS2_FPGAIO_BASE               (0x40302000UL)       /* FPGAIO Base Address */

#define SMSC9220_BASE                  (0x42000000UL)       /* Ethernet SMSC9220 Base Address */

#define MPS2_VGA_BUFFER                (0x41100000UL)       /* VGA Buffer Base Address */
#define MPS2_VGA_TEXT_BUFFER           (0x41000000UL)       /* VGA Text Buffer Address */


/******************************************************************************/
/*                      Secure Peripheral memory map                          */
/******************************************************************************/
#define MPS2_SECURE_SSP0_BASE          (0x50205000UL)       /* User SSP PL022 Base Address */
#define MPS2_SECURE_SSP1_BASE          (0x50206000UL)       /* CLCD SSP PL022 Base Address */
#define MPS2_SECURE_I2C0_BASE          (0x50207000UL)       /* Touch Screen I2C Base Address */
#define MPS2_SECURE_I2C1_BASE          (0x50208000UL)       /* Audio Interface I2C Base Address */
#define MPS2_SECURE_SSP2_BASE          (0x50209000UL)       /* ADC SPI PL022 Base Address */
#define MPS2_SECURE_SSP3_BASE          (0x5020A000UL)       /* Shield 0 SPI PL022 Base Address */
#define MPS2_SECURE_SSP4_BASE          (0x5020B000UL)       /* Shield 1 SPI PL022 Base Address */
#define MPS2_SECURE_I2C2_BASE          (0x5020C000UL)       /* Shield 0 SBCon Base Address */
#define MPS2_SECURE_I2C3_BASE          (0x5020D000UL)       /* Shield 1 SBCon Base Address */
#define MPS2_SECURE_FPGAIO_BASE        (0x50302000UL)       /* FPGAIO Base Address */
#define MPS2_SECURE_I2S_BASE           (0x50301000UL)       /* Audio Interface I2S Base Address */
#define MPS2_SECURE_SCC_BASE           (0x50300000UL)       /* SCC Base Address */

#define MPS2_SECURE_SMSC9220_BASE      (0x52000000UL)       /* Ethernet SMSC9220 Base Address */

#define MPS2_SECURE_VGA_BUFFER         (0x51100000UL)       /* VGA Buffer Base Address */
#define MPS2_SECURE_VGA_TEXT_BUFFER    (0x51000000UL)       /* VGA Text Buffer Address */


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define MPS2_I2C0                      ((MPS2_I2C_TypeDef      *) MPS2_I2C0_BASE )
#define MPS2_I2C1                      ((MPS2_I2C_TypeDef      *) MPS2_I2C1_BASE )
#define MPS2_I2C2                      ((MPS2_I2C_TypeDef      *) MPS2_I2C2_BASE )
#define MPS2_I2C3                      ((MPS2_I2C_TypeDef      *) MPS2_I2C3_BASE )
#define MPS2_I2S                       ((MPS2_I2S_TypeDef      *) MPS2_I2S_BASE )
#define MPS2_FPGAIO                    ((MPS2_FPGAIO_TypeDef   *) MPS2_FPGAIO_BASE )
#define MPS2_SCC                       ((MPS2_SCC_TypeDef      *) MPS2_SCC_BASE )
#define MPS2_SSP0                      ((MPS2_SSP_TypeDef      *) MPS2_SSP0_BASE )
#define MPS2_SSP1                      ((MPS2_SSP_TypeDef      *) MPS2_SSP1_BASE )
#define SMSC9220                       ((SMSC9220_TypeDef      *) SMSC9220_BASE)

/******************************************************************************/
/*                      Secure Peripheral declaration                         */
/******************************************************************************/
#define MPS2_SECURE_I2C0               ((MPS2_I2C_TypeDef      *) MPS2_SECURE_I2C0_BASE )
#define MPS2_SECURE_I2C1               ((MPS2_I2C_TypeDef      *) MPS2_SECURE_I2C1_BASE )
#define MPS2_SECURE_I2C2               ((MPS2_I2C_TypeDef      *) MPS2_SECURE_I2C2_BASE )
#define MPS2_SECURE_I2C3               ((MPS2_I2C_TypeDef      *) MPS2_SECURE_I2C3_BASE )
#define MPS2_SECURE_I2S                ((MPS2_I2S_TypeDef      *) MPS2_SECURE_I2S_BASE )
#define MPS2_SECURE_FPGAIO             ((MPS2_FPGAIO_TypeDef   *) MPS2_SECURE_FPGAIO_BASE )
#define MPS2_SECURE_SCC                ((MPS2_SCC_TypeDef      *) MPS2_SECURE_SCC_BASE )
#define MPS2_SECURE_SSP0               ((MPS2_SSP_TypeDef      *) MPS2_SECURE_SSP0_BASE )
#define MPS2_SECURE_SSP1               ((MPS2_SSP_TypeDef      *) MPS2_SECURE_SSP1_BASE )
#define MPS2_SECURE_SMSC9220           ((SMSC9220_TypeDef      *) MPS2_SECURE_SMSC9220_BASE)

#endif /* SMM_MPS2_H_ */
