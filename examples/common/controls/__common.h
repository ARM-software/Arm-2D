/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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



#ifndef ____COMMON_H__
#define ____COMMON_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_features.h"

/*============================ MACROS ========================================*/
#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define __arm_2d_color_t         arm_2d_color_gray8_t
#   define COLOUR_INT               uint8_t

#   define arm_2d_draw_pattern      arm_2d_c8bit_draw_pattern
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_gray8_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_gray8_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_gray8_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_gray8_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_gray8_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_c8bit_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_c8bit_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_c8bit_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_c8bit_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_c8bit_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_c8bit_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_c8bit_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_c8bit_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_c8bit_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_c8bit_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_c8bit_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_gray8_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_gray8_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_gray8_tile_copy_with_des_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_gray8_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_gray8_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_gray8_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_gray8_tile_rotation_with_alpha
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define __arm_2d_color_t         arm_2d_color_rgb565_t
#   define COLOUR_INT               uint16_t

#   define arm_2d_draw_pattern      arm_2d_rgb16_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_rgb565_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_rgb565_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_rgb565_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_rgb565_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_rgb565_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_rgb16_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_rgb16_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_rgb16_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_rgb16_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_rgb16_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_rgb16_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_rgb16_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_rgb16_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_rgb16_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_rgb16_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_rgb16_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_rgb565_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_rgb565_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_rgb565_tile_copy_with_des_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_rgb565_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_rgb565_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_rgb565_tile_rotation_with_alpha
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define __arm_2d_color_t         arm_2d_color_cccn888_t
#   define COLOUR_INT               uint32_t

#   define arm_2d_draw_pattern      arm_2d_rgb32_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_cccn888_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_cccn888_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_cccn888_fill_colour_with_mask_and_opacity
#   define arm_2d_alpha_blending_with_colour_keying                             \
                arm_2d_cccn888_alpha_blending_with_colour_keying
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_cccn888_tile_copy_with_masks
#   define arm_2d_tile_fill_with_x_mirror                                       \
                arm_2d_rgb32_tile_fill_with_x_mirror
#   define arm_2d_tile_fill_with_y_mirror                                       \
                arm_2d_rgb32_tile_fill_with_y_mirror
#   define arm_2d_tile_fill_with_xy_mirror                                      \
                arm_2d_rgb32_tile_fill_with_xy_mirror
#   define arm_2d_tile_fill_only                                                \
                arm_2d_rgb32_tile_fill_only
#   define arm_2d_tile_copy_with_x_mirror                                       \
                arm_2d_rgb32_tile_copy_with_x_mirror
#   define arm_2d_tile_copy_with_y_mirror                                       \
                arm_2d_rgb32_tile_copy_with_y_mirror
#   define arm_2d_tile_copy_with_xy_mirror                                      \
                arm_2d_rgb32_tile_copy_with_xy_mirror
#   define arm_2d_tile_copy_only                                                \
                arm_2d_rgb32_tile_copy_only
#   define arm_2d_tile_copy                                                     \
                arm_2d_rgb32_tile_copy
#   define arm_2d_fill_colour                                                   \
                arm_2d_rgb32_fill_colour
#   define arm_2d_tile_copy_with_colour_keying                                  \
                arm_2d_rgb32_tile_copy_with_colour_keying
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_cccn888_fill_colour_with_opacity
#   define arm_2d_alpha_blending                                                \
                arm_2d_cccn888_alpha_blending
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_cccn888_tile_copy_with_des_mask
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_cccn888_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation                arm_2dp_cccn888_tile_rotation
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_cccn888_tile_rotation_with_alpha

#else
#   error Unsupported colour depth!
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/

/*---------------------- Graphic LCD color definitions -----------------------*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define __RGB(__R, __G, __B)    ((((__R) + (__G) + (__B)) / 3) & 0xFF)
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define __RGB(__R, __G, __B)    ((((uint16_t)(__R) & 0xFF) >> 3 << 11)   |   \
                                    (((uint16_t)(__G) & 0xFF) >> 2 << 5)    |   \
                                    (((uint16_t)(__B) & 0xFF) >> 3 << 0)    )
#else /* __GLCD_CFG_COLOUR_DEPTH__ == 32 */
#   define __RGB(__R, __G, __B)    ((((uint32_t)(__R) & 0xFF) << 16)        |   \
                                    (((uint32_t)(__G) & 0xFF) << 8)         |   \
                                    (((uint32_t)(__B) & 0xFF) << 0)         |   \
                                    (uint32_t)0xFF << 24)
#endif

/* GLCD RGB color definitions                            */
#define GLCD_COLOR_BLACK        __RGB(   0,   0,   0  )
#define GLCD_COLOR_NAVY         __RGB(   0,   0, 128  )
#define GLCD_COLOR_DARK_GREEN   __RGB(   0, 128,   0  )
#define GLCD_COLOR_DARK_CYAN    __RGB(   0, 128, 128  )
#define GLCD_COLOR_MAROON       __RGB( 128,   0,   0  )
#define GLCD_COLOR_PURPLE       __RGB( 128,   0, 128  )
#define GLCD_COLOR_OLIVE        __RGB( 128, 128,   0  )
#define GLCD_COLOR_LIGHT_GREY   __RGB( 192, 192, 192  )
#define GLCD_COLOR_DARK_GREY    __RGB( 128, 128, 128  )
#define GLCD_COLOR_BLUE         __RGB(   0,   0, 255  )
#define GLCD_COLOR_GREEN        __RGB(   0, 255,   0  )
#define GLCD_COLOR_CYAN         __RGB(   0, 255, 255  )
#define GLCD_COLOR_RED          __RGB( 255,   0,   0  )
#define GLCD_COLOR_MAGENTA      __RGB( 255,   0, 255  )
#define GLCD_COLOR_YELLOW       __RGB( 255, 255, 0    )
#define GLCD_COLOR_WHITE        __RGB( 255, 255, 255  )

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void controls_init(void);

#endif
