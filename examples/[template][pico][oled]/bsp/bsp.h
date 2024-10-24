/****************************************************************************
*  Copyright 2023 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef __RP2040_BSP_H__
#define __RP2040_BSP_H__


/*
 * How to use:
 * --------------------------------
 * Please define the following macros globally to select the corresponding bsp:
 *
 *     - __PICO_USE_STANDARD__      for the Raspberry Pi Pico only (default)
 *     - __PICO_USE_LCD_1IN3__      for Pico LCD 1.3inch
 *     - __RP2040_USE_LCD_1IN28__   for the RP2040 development kit: LCD 1.28inch
 *
 */



#if defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__
#   include "./pico_lcd_1in3/Config/Dev_Config.h"
#   include "./pico_lcd_1in3/LCD/LCD_1In3.h"
#   include "./pico_lcd_1in3/GLCD_Config.h"



#   if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__
#       pragma import(__pico_bsp_use_lcd_1in3__)
#   elif    (defined(__GNUC__) || defined(__clang__))                           \
        &&  (!defined(__IS_COMPILER_IAR__) || !__IS_COMPILER_IAR__)
__asm(".global __pico_bsp_use_lcd_1in3__\n\t");
#   endif

#elif defined(__RP2040_USE_LCD_1IN28__) && __RP2040_USE_LCD_1IN28__
#   include "./rp2040_lcd_1in28/Config/Dev_Config.h"
#   include "./rp2040_lcd_1in28/LCD/LCD_1In28.h"
#   include "./rp2040_lcd_1in28/GLCD_Config.h"


#   if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__
#       pragma import(__rp2040_bsp_use_lcd_1in28__)
#   elif    (defined(__GNUC__) || defined(__clang__))                           \
        &&  (!defined(__IS_COMPILER_IAR__) || !__IS_COMPILER_IAR__)
__asm(".global __rp2040_bsp_use_lcd_touch_1in28__\n\t");
#   endif

#elif defined(__RP2040_USE_LCD_TOUCH_1IN28__) && __RP2040_USE_LCD_TOUCH_1IN28__
#   include "./rp2040_lcd_touch_1in28/Config/Dev_Config.h"
#   include "./rp2040_lcd_touch_1in28/LCD/LCD_1In28.h"
#   include "./rp2040_lcd_touch_1in28/GLCD_Config.h"


#   if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__
#       pragma import(__rp2040_bsp_use_lcd_touch_1in28__)
#   elif    (defined(__GNUC__) || defined(__clang__))                           \
        &&  (!defined(__IS_COMPILER_IAR__) || !__IS_COMPILER_IAR__)
__asm(".global __rp2040_bsp_use_lcd_touch_1in28__\n\t");
#   endif

#elif defined(__RP2040_USE_OLED_1IN3__) && __RP2040_USE_OLED_1IN3__
#   include "./rp2040_oled_1in3/Config/Dev_Config.h"
#   include "./rp2040_oled_1in3/OLED/OLED_1in3_c.h"
#   include "./rp2040_oled_1in3/GLCD_Config.h"


#   if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__
#       pragma import(__rp2040_bsp_use_oled_1in3__)
#   elif    (defined(__GNUC__) || defined(__clang__))                           \
        &&  (!defined(__IS_COMPILER_IAR__) || !__IS_COMPILER_IAR__)
__asm(".global __rp2040_bsp_use_oled_1in3__\n\t");
#   endif

#else

#   if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__
#       pragma import(__pico_bsp_use_standard__)
#   elif    (defined(__GNUC__) || defined(__clang__))                           \
        &&  (!defined(__IS_COMPILER_IAR__) || !__IS_COMPILER_IAR__)
__asm(".global __pico_bsp_use_standard__\n\t");
#   endif

#endif

/*============================ MACROS ========================================*/


#if (defined(__PICO_USE_LCD_1IN3__) && __PICO_USE_LCD_1IN3__)                   \
 || (defined(__RP2040_USE_LCD_1IN28__) && __RP2040_USE_LCD_1IN28__)             \
 || (defined(__RP2040_USE_LCD_TOUCH_1IN28__) && __RP2040_USE_LCD_TOUCH_1IN28__) \
 || (defined(__RP2040_USE_OLED_1IN3__) && __RP2040_USE_OLED_1IN3__)

#else


#   define __PICO_USE_STANDARD__        1
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void bsp_init(void);

extern void breath_led(void);




#endif
