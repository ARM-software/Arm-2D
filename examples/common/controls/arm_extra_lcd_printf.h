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
#include "arm_2d_helper.h"
#include "__common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wpadded"
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

#if 0 // TODO in the future version when string alignment feature is available
#define arm_lcd_banner_printf(__REGION, __FONT_PTR, __FORMAT_STR, ...)          \
        do {                                                                    \
            if (NULL != (__FONT_PTR)) {                                         \
                arm_lcd_text_set_font((const arm_2d_font_t *)(__FONT_PTR));     \
            }                                                                   \
            arm_2d_region_t ARM_2D_SAFE_NAME(tTargetRegion) = (__REGION);       \
            arm_2d_align_centre(                                                \
                            ARM_2D_SAFE_NAME(tTargetRegion),                    \
                            arm_lcd_get_string_line_box((__STR),(__FONT_PTR))) {\
                arm_lcd_text_set_draw_region(&__centre_region);                 \
                arm_lcd_printf(__FORMAT_STR, ##__VA_ARGS__);                    \
            }                                                                   \
        } while(0)
#endif

#define __arm_print_banner3(__STR, __REGION, __FONT_PTR)                        \
        do {                                                                    \
            if (NULL != (__FONT_PTR)) {                                         \
                arm_lcd_text_set_font((const arm_2d_font_t *)(__FONT_PTR));     \
            }                                                                   \
            arm_2d_region_t ARM_2D_SAFE_NAME(tTargetRegion) = (__REGION);       \
            arm_2d_align_centre(                                                \
                            ARM_2D_SAFE_NAME(tTargetRegion),                    \
                            arm_lcd_get_string_line_box((__STR),(__FONT_PTR))) {\
                arm_lcd_text_set_draw_region(&__centre_region);                 \
                arm_lcd_puts(__STR);                                            \
            }                                                                   \
        } while(0)

#define __arm_print_banner2(__STR, __REGION)                                    \
        __arm_print_banner3(__STR, __REGION, NULL)

#define __arm_print_banner1(__STR)                                              \
        do {                                                                    \
            arm_2d_tile_t *ARM_2D_SAFE_NAME(ptTile)                             \
                            = arm_2d_get_default_frame_buffer();                \
            arm_2d_canvas(ARM_2D_SAFE_NAME(ptTile), __banner_canvas) {          \
                __arm_print_banner3(__STR, __banner_canvas, NULL);              \
            }                                                                   \
        } while(0)

#define arm_print_banner(...)                                                   \
            ARM_CONNECT2(   __arm_print_banner,                                 \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define arm_lcd_print_banner(...)               arm_print_banner(__VA_ARGS__)                                

#define arm_lcd_get_string_line_box(__STR, ...)                                 \
            __arm_lcd_get_string_line_box(                                      \
                                (__STR),                                        \
                                (const arm_2d_font_t *)(NULL, ##__VA_ARGS__))

/*============================ TYPES =========================================*/

typedef struct arm_2d_a1_font_t {
    implement(arm_2d_font_t);
    uint32_t nOffset;                                                           //!< Character offset
} arm_2d_a1_font_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_a1_font_t    ARM_2D_FONT_16x24;
extern const arm_2d_a1_font_t    ARM_2D_FONT_6x8;

extern
arm_2d_font_get_char_descriptor_handler_t
    ARM_2D_A1_FONT_GET_CHAR_DESCRIPTOR_HANDLER;

/*============================ PROTOTYPES ====================================*/

/*!
 * \brief initialize lcd text display service
 * \param[in] ptScreen the default display area
 */
extern
void arm_lcd_text_init(arm_2d_region_t *ptScreen);

extern
int arm_lcd_printf(const char *format, ...);

extern
int arm_lcd_printf_label(arm_2d_align_t tAlignment, const char *format, ...);

extern
void arm_lcd_puts(const char *str);

extern 
void arm_lcd_text_location(uint8_t chY, uint8_t chX);

/*!
 * \brief draw a char to a given location in the draw region
 * \param[in] iX the x coordinate
 * \param[in] iY the y coordinate
 * \param[in] ppchCharCode a pointer of pointer that points to the string
 *
 * \note this function will advance the pointer automatically
 * 
 * \param[in] chOpacity the opacity of the char
 *
 * \note this chOpacity has NO effect on the default monochrome(A1) font
 * 
 * \return int16_t char advance in pixels
 */
extern
int16_t lcd_draw_char(int16_t iX, 
                     int16_t iY, 
                     uint8_t **ppchCharCode, 
                     uint_fast8_t chOpacity);

extern
void arm_lcd_putchar(const char *str);

extern 
void arm_lcd_text_set_colour(   COLOUR_INT_TYPE wForeground, 
                                COLOUR_INT_TYPE wBackground);

extern
void arm_lcd_text_set_opacity(uint8_t chOpacity);

extern
void arm_lcd_text_set_scale(float fScale);

/*!
 * \brief Force all char use the same width
 * 
 * \param[in] bForced force or not
 * \return boolean the original configuration
 */
extern
bool arm_lcd_text_force_char_use_same_width(bool bForced);

extern 
void arm_lcd_text_set_target_framebuffer(const arm_2d_tile_t *ptFrameBuffer);

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

extern
arm_2d_err_t arm_lcd_text_set_font(const arm_2d_font_t *ptFont);

extern
arm_2d_size_t __arm_lcd_get_string_line_box(const char *str, const arm_2d_font_t *ptFont);

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#ifdef __cplusplus
}
#endif

#endif
