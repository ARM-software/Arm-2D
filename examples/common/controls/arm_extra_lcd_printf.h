/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LCD_PRINTF_H__
#define __LCD_PRINTF_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include "arm_2d.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#ifndef __GLCD_CFG_COLOUR_DEPTH__
#   warning Please specify the colour depth by defining the macro __GLCD_CFG_COLOUR_DEPTH__, default value 16 is used for now
#   define __GLCD_CFG_COLOUR_DEPTH__      16
#endif

#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define COLOUR_INT_TYPE         uint8_t 

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define COLOUR_INT_TYPE         uint16_t 

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define COLOUR_INT_TYPE         uint32_t 

#else
#   error Unsupported colour depth!
#endif

#ifndef __GLCD_CFG_SCEEN_WIDTH__
#warning Please specify the screen width by defining the macro __GLCD_CFG_SCEEN_WIDTH__, default value 320 is used for now
#define __GLCD_CFG_SCEEN_WIDTH__                            320
#endif

#ifndef __GLCD_CFG_SCEEN_HEIGHT__
#   warning Please specify the screen height by defining the macro __GLCD_CFG_SCEEN_HEIGHT__, default value 240 is used for now
#   define __GLCD_CFG_SCEEN_HEIGHT__                        320
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define arm_print_banner(__STR)                                                 \
        do {                                                                    \
            arm_lcd_text_location(                                              \
                (__GLCD_CFG_SCEEN_HEIGHT__ / 8) / 2 - 1,                        \
                ((__GLCD_CFG_SCEEN_WIDTH__ / 6) - sizeof(__STR)) / 2);          \
            arm_lcd_puts(__STR);                                                \
        } while(0)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


extern
int arm_lcd_printf(const char *format, ...);

extern
void arm_lcd_puts(const char *str);

extern 
void arm_lcd_text_location(uint8_t chY, uint8_t chX);

extern 
void arm_lcd_text_set_colour(   COLOUR_INT_TYPE wForeground, 
                                COLOUR_INT_TYPE wBackground);

extern 
void arm_lcd_text_set_target_framebuffer(arm_2d_tile_t *ptFrameBuffer);

/*!
 * \brief set the display mode for characters
 * \param[in] wMode the display mode which could be the combination of following 
              enum values:
              - ARM_2D_DRW_PATN_MODE_COPY (default) - Copy without background colour
              - ARM_2D_DRW_PATN_MODE_WITH_BG_COLOR - with background colour
              - ARM_2D_DRW_PATN_MODE_NO_FG_COLOR - without foreground colour
              - ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR - use complement value as the foreground colour
 */

extern
void arm_lcd_text_set_display_mode(uint32_t wMode);

extern 
void arm_lcd_text_set_draw_region(arm_2d_region_t *ptRegion);

#ifdef __cplusplus
}
#endif

#endif
