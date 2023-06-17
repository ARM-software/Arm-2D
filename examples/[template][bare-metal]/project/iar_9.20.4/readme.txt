/**
  @page DMA2D_MemToMemWithBlending DMA2D Memory to Memory with blending example
  
  @verbatim
  ******************************************************************************
  * @file    DMA2D/DMA2D_MemToMemWithBlending/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the STM32F7xx DMA2D Memory to Memory with blending example.
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to configure the DMA2D peripheral in Memory-to-memory with blending
transfer mode.

  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  Then the SystemClock_Config() function is used to configure the system
  clock (SYSCLK) to run at 216 MHz.
 
  In this transfer mode two input sources are fetched : foreground and background.
  In this example, the foreground and background are configured as following :
   - Foreground object 
		- input memory address at FLASH memory (static image arrays coded in FLASH).
		- Format : RGB565
		- Size   : 240x130
   - Background object 
		- input memory address at FLASH memory (static image arrays coded in FLASH).
		- Format : RGB565
		- Size   : 240x130
   - The constant alpha for foreground is decreased to see the background.
   - The alpha mode for foreground and background is configured to see two 
     superposed images in the resulting image out of the ChromArt DMA2D operation.
        
  The DMA2D blends the two sources pixels from FLASH to internal FIFOs to compute 
  the resulting pixel in dedicated output FIFO. 
  The transferred data to the output memory address is the result of the blending 
  operation between background and foreground.
 
  Next step in the pixel pipeline is the use of the LTDC with only 1 layer which 
  is the output of the DMA2D operation.
  This in order to show on LCD panel the result of the blending of the 
  foreground and background.      
  
  In this example, the background object represents the ST logo and the foreground
  object represents the "life.augmented" slogan.

STM32746G-DISCOVERY board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the DMA2D transfer is complete.
 - LED1 is Toggled with a period of 200 ms when there is a DMA2D transfer error. 
 - LED1 is Toggled with a period of 1000 ms when there is an error in Init process.
         
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Display, Graphic, DMA2D, LCD, SRAM, ARGB4444, Blending, Memory to memory, Foreground,
Background, RGB565, LTDC, Pixel

@Note If the user code size exceeds the DTCM-RAM size or starts from internal cacheable memories (SRAM1 and SRAM2),that is shared between several processors,
      then it is highly recommended to enable the CPU cache and maintain its coherence at application level.
      The address and the size of cacheable buffers (shared between CPU and other masters)  must be properly updated to be aligned to cache line size (32 bytes).

@Note It is recommended to enable the cache and maintain its coherence, but depending on the use case
      It is also possible to configure the MPU as "Write through", to guarantee the write access coherence.
      In that case, the MPU must be configured as Cacheable/Bufferable/Not Shareable.
      Even though the user must manage the cache coherence for read accesses.
      Please refer to the AN4838 “Managing memory protection unit (MPU) in STM32 MCUs”
      Please refer to the AN4839 “Level 1 cache on STM32F7 Series”

@par Directory contents

    - DMA2D/DMA2D_MemToMemWithBlending/Inc/main.h                Main configuration file
    - DMA2D/DMA2D_MemToMemWithBlending/Inc/stm32f7xx_it.h        Interrupt handlers header file
    - DMA2D/DMA2D_MemToMemWithBlending/Inc/stm32f7xx_hal_conf.h  HAL configuration file 
    - DMA2D/DMA2D_MemToMemWithBlending/Inc/RGB565_230x170_1.h    Image used for DMAD2D validation
    - DMA2D/DMA2D_MemToMemWithBlending/Inc/RGB565_230x170_2.h    Image used for DMAD2D validation
    - DMA2D/DMA2D_MemToMemWithBlending/Src/main.c                      Main program  
    - DMA2D/DMA2D_MemToMemWithBlending/Src/stm32f7xx_it.c        Interrupt handlers
    - DMA2D/DMA2D_MemToMemWithBlending/Src/stm32f7xx_hal_msp.c   HAL MSP module
    - DMA2D/DMA2D_MemToMemWithBlending/Src/system_stm32f7xx.c    STM32F7xx system clock configuration file

@par Hardware and Software environment  

  - This example runs on STM32F756xx/STM32F746xx devices.
  
  - This example has been tested with STM32746G-DISCOVERY boards and can be easily 
    tailored to any other supported device and development board. 

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 
                          
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
                                   