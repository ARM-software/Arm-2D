/*
 * Copyright (C) 2022 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "__arm_2d_helper_common.h"
 * Description:  Public header file for the all common definitions used in 
 *               arm-2d helper services
 *
 * $Date:        28. Nov 2023
 * $Revision:    V.1.1.1
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_COMMON_H__
#define __ARM_2D_HELPER_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#include <stdlib.h>
#include <assert.h>

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wgcc-compat"
#endif


/*!
 * \addtogroup gHelper 7 Helper Services
 * @{
 */
/*============================ MACROS ========================================*/

/*!
 * \addtogroup Deprecated
 * @{
 */

#define arm_2d_alpha_blending   arm_2d_tile_copy_with_opacity

#define arm_2d_alpha_blending_with_colour_keying                                \
            arm_2d_tile_copy_with_colour_keying_and_opacity

/*! @} */


/*
 * IMPORTANT: The following colour-type-free macros are only used in examples. 
 *            They only cover a small range of APIs. They are not guaranteed and
 *            subject to change. Please do NOT use them in your applications.
 */

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define __arm_2d_color_t         arm_2d_color_gray8_t
#   define COLOUR_INT               uint8_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_GRAY8

#   define arm_2d_draw_pattern      arm_2d_c8bit_draw_pattern
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_gray8_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_gray8_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_gray8_fill_colour_with_mask_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_gray8_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_gray8_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_gray8_tile_copy_with_masks
#   define arm_2d_tile_copy_with_masks_only                                     \
                arm_2d_gray8_tile_copy_with_masks_only
#   define arm_2d_tile_copy_with_masks_and_x_mirror                             \
                arm_2d_gray8_tile_copy_with_masks_and_x_mirror
#   define arm_2d_tile_copy_with_masks_and_y_mirror                             \
                arm_2d_gray8_tile_copy_with_masks_and_y_mirror
#   define arm_2d_tile_copy_with_masks_and_xy_mirror                            \
                arm_2d_gray8_tile_copy_with_masks_and_xy_mirror
#   define arm_2d_tile_fill_with_masks_only                                     \
                arm_2d_gray8_tile_fill_with_masks_only
#   define arm_2d_tile_fill_with_masks_and_x_mirror                             \
                arm_2d_gray8_tile_fill_with_masks_and_x_mirror
#   define arm_2d_tile_fill_with_masks_and_y_mirror                             \
                arm_2d_gray8_tile_fill_with_masks_and_y_mirror
#   define arm_2d_tile_fill_with_masks_and_xy_mirror                            \
                arm_2d_gray8_tile_fill_with_masks_and_xy_mirror
#   define arm_2d_tile_copy_with_src_mask_only                                  \
                arm_2d_gray8_tile_copy_with_src_mask_only
#   define arm_2d_tile_copy_with_src_mask_and_x_mirror                          \
                arm_2d_gray8_tile_copy_with_src_mask_and_x_mirror
#   define arm_2d_tile_copy_with_src_mask_and_y_mirror                          \
                arm_2d_gray8_tile_copy_with_src_mask_and_y_mirror
#   define arm_2d_tile_copy_with_src_mask_and_xy_mirror                         \
                arm_2d_gray8_tile_copy_with_src_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_src_mask_only                                  \
                arm_2d_gray8_tile_fill_with_src_mask_only
#   define arm_2d_tile_fill_with_src_mask_and_x_mirror                          \
                arm_2d_gray8_tile_fill_with_src_mask_and_x_mirror
#   define arm_2d_tile_fill_with_src_mask_and_y_mirror                          \
                arm_2d_gray8_tile_fill_with_src_mask_and_y_mirror
#   define arm_2d_tile_fill_with_src_mask_and_xy_mirror                         \
                arm_2d_gray8_tile_fill_with_src_mask_and_xy_mirror
#   define arm_2d_tile_copy_with_des_mask_only                                  \
                arm_2d_gray8_tile_copy_with_des_mask_only
#   define arm_2d_tile_copy_with_des_mask_and_x_mirror                          \
                arm_2d_gray8_tile_copy_with_des_mask_and_x_mirror
#   define arm_2d_tile_copy_with_des_mask_and_y_mirror                          \
                arm_2d_gray8_tile_copy_with_des_mask_and_y_mirror
#   define arm_2d_tile_copy_with_des_mask_and_xy_mirror                         \
                arm_2d_gray8_tile_copy_with_des_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_des_mask_only                                  \
                arm_2d_gray8_tile_fill_with_des_mask_only
#   define arm_2d_tile_fill_with_des_mask_and_x_mirror                          \
                arm_2d_gray8_tile_fill_with_des_mask_and_x_mirror
#   define arm_2d_tile_fill_with_des_mask_and_y_mirror                          \
                arm_2d_gray8_tile_fill_with_des_mask_and_y_mirror
#   define arm_2d_tile_fill_with_des_mask_and_xy_mirror                         \
                arm_2d_gray8_tile_fill_with_des_mask_and_xy_mirror
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
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_gray8_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_only                             \
                arm_2d_c8bit_tile_copy_with_colour_keying_only
#   define arm_2d_tile_copy_with_colour_keying_and_x_mirror                     \
                arm_2d_c8bit_tile_copy_with_colour_keying_and_x_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_y_mirror                     \
                arm_2d_c8bit_tile_copy_with_colour_keying_and_y_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_xy_mirror                    \
                arm_2d_c8bit_tile_copy_with_colour_keying_and_xy_mirror
#   define arm_2d_tile_fill_with_colour_keying_only                             \
                arm_2d_c8bit_tile_fill_with_colour_keying_only
#   define arm_2d_tile_fill_with_colour_keying_and_x_mirror                     \
                arm_2d_c8bit_tile_fill_with_colour_keying_and_x_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_y_mirror                     \
                arm_2d_c8bit_tile_fill_with_colour_keying_and_y_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_xy_mirror                    \
                arm_2d_c8bit_tile_fill_with_colour_keying_and_xy_mirror
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_gray8_fill_colour_with_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_gray8_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_gray8_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_gray8_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_only                                          \
                arm_2dp_gray8_tile_transform_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_gray8_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_gray8_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_gray8_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation            arm_2dp_gray8_tile_rotation
#   define arm_2dp_tile_rotation_with_colour_keying                             \
                arm_2dp_gray8_tile_rotation_with_colour_keying
#   define arm_2dp_tile_rotation_only       arm_2dp_gray8_tile_rotation_only
#   define arm_2dp_tile_rotation_only_with_opacity                              \
                arm_2dp_gray8_tile_rotation_only_with_opacity
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_gray8_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_opacity                                   \
                arm_2dp_gray8_tile_rotation_with_opacity
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_gray8_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_gray8_tile_rotation_with_src_mask
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_gray8_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_gray8_fill_colour_with_a2_mask
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_gray8_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_gray8_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_draw_point        arm_2d_gray8_draw_point
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define __arm_2d_color_t         arm_2d_color_rgb565_t
#   define COLOUR_INT               uint16_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_RGB565

#   define arm_2d_draw_pattern      arm_2d_rgb16_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_rgb565_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_rgb565_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_rgb565_fill_colour_with_mask_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_rgb565_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_rgb565_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_rgb565_tile_copy_with_masks
#   define arm_2d_tile_copy_with_masks_only                                     \
                arm_2d_rgb565_tile_copy_with_masks_only
#   define arm_2d_tile_copy_with_masks_and_x_mirror                             \
                arm_2d_rgb565_tile_copy_with_masks_and_x_mirror
#   define arm_2d_tile_copy_with_masks_and_y_mirror                             \
                arm_2d_rgb565_tile_copy_with_masks_and_y_mirror
#   define arm_2d_tile_copy_with_masks_and_xy_mirror                            \
                arm_2d_rgb565_tile_copy_with_masks_and_xy_mirror
#   define arm_2d_tile_fill_with_masks_only                                     \
                arm_2d_rgb565_tile_fill_with_masks_only
#   define arm_2d_tile_fill_with_masks_and_x_mirror                             \
                arm_2d_rgb565_tile_fill_with_masks_and_x_mirror
#   define arm_2d_tile_fill_with_masks_and_y_mirror                             \
                arm_2d_rgb565_tile_fill_with_masks_and_y_mirror
#   define arm_2d_tile_fill_with_masks_and_xy_mirror                            \
                arm_2d_rgb565_tile_fill_with_masks_and_xy_mirror
#   define arm_2d_tile_copy_with_src_mask_only                                  \
                arm_2d_rgb565_tile_copy_with_src_mask_only
#   define arm_2d_tile_copy_with_src_mask_and_x_mirror                          \
                arm_2d_rgb565_tile_copy_with_src_mask_and_x_mirror
#   define arm_2d_tile_copy_with_src_mask_and_y_mirror                          \
                arm_2d_rgb565_tile_copy_with_src_mask_and_y_mirror
#   define arm_2d_tile_copy_with_src_mask_and_xy_mirror                         \
                arm_2d_rgb565_tile_copy_with_src_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_src_mask_only                                  \
                arm_2d_rgb565_tile_fill_with_src_mask_only
#   define arm_2d_tile_fill_with_src_mask_and_x_mirror                          \
                arm_2d_rgb565_tile_fill_with_src_mask_and_x_mirror
#   define arm_2d_tile_fill_with_src_mask_and_y_mirror                          \
                arm_2d_rgb565_tile_fill_with_src_mask_and_y_mirror
#   define arm_2d_tile_fill_with_src_mask_and_xy_mirror                         \
                arm_2d_rgb565_tile_fill_with_src_mask_and_xy_mirror
#   define arm_2d_tile_copy_with_des_mask_only                                  \
                arm_2d_rgb565_tile_copy_with_des_mask_only
#   define arm_2d_tile_copy_with_des_mask_and_x_mirror                          \
                arm_2d_rgb565_tile_copy_with_des_mask_and_x_mirror
#   define arm_2d_tile_copy_with_des_mask_and_y_mirror                          \
                arm_2d_rgb565_tile_copy_with_des_mask_and_y_mirror
#   define arm_2d_tile_copy_with_des_mask_and_xy_mirror                         \
                arm_2d_rgb565_tile_copy_with_des_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_des_mask_only                                  \
                arm_2d_rgb565_tile_fill_with_des_mask_only
#   define arm_2d_tile_fill_with_des_mask_and_x_mirror                          \
                arm_2d_rgb565_tile_fill_with_des_mask_and_x_mirror
#   define arm_2d_tile_fill_with_des_mask_and_y_mirror                          \
                arm_2d_rgb565_tile_fill_with_des_mask_and_y_mirror
#   define arm_2d_tile_fill_with_des_mask_and_xy_mirror                         \
                arm_2d_rgb565_tile_fill_with_des_mask_and_xy_mirror
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
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_rgb565_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_only                             \
                arm_2d_rgb16_tile_copy_with_colour_keying_only
#   define arm_2d_tile_copy_with_colour_keying_and_x_mirror                     \
                arm_2d_rgb16_tile_copy_with_colour_keying_and_x_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_y_mirror                     \
                arm_2d_rgb16_tile_copy_with_colour_keying_and_y_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_xy_mirror                    \
                arm_2d_rgb16_tile_copy_with_colour_keying_and_xy_mirror
#   define arm_2d_tile_fill_with_colour_keying_only                             \
                arm_2d_rgb16_tile_fill_with_colour_keying_only
#   define arm_2d_tile_fill_with_colour_keying_and_x_mirror                     \
                arm_2d_rgb16_tile_fill_with_colour_keying_and_x_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_y_mirror                     \
                arm_2d_rgb16_tile_fill_with_colour_keying_and_y_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_xy_mirror                    \
                arm_2d_rgb16_tile_fill_with_colour_keying_and_xy_mirror
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_rgb565_fill_colour_with_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_rgb565_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_rgb565_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_rgb565_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_only                                          \
                arm_2dp_rgb565_tile_transform_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_rgb565_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_rgb565_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation            arm_2dp_rgb565_tile_rotation
#   define arm_2dp_tile_rotation_with_colour_keying                             \
                arm_2dp_rgb565_tile_rotation_with_colour_keying
#   define arm_2dp_tile_rotation_only       arm_2dp_rgb565_tile_rotation_only
#   define arm_2dp_tile_rotation_only_with_opacity                              \
                arm_2dp_rgb565_tile_rotation_only_with_opacity
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_rgb565_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_opacity                                   \
                arm_2dp_rgb565_tile_rotation_with_opacity
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_rgb565_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_rgb565_tile_rotation_with_src_mask
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_rgb565_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_rgb565_fill_colour_with_a2_mask
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_rgb565_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_rgb565_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_draw_point        arm_2d_rgb565_draw_point
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define __arm_2d_color_t         arm_2d_color_cccn888_t
#   define COLOUR_INT               uint32_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_CCCA8888

#   define arm_2d_draw_pattern      arm_2d_rgb32_draw_pattern 
#   define arm_2dp_fill_colour_with_opacity                                     \
                arm_2dp_cccn888_fill_colour_with_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_cccn888_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_cccn888_fill_colour_with_mask_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_cccn888_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_cccn888_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_masks                                          \
                arm_2d_cccn888_tile_copy_with_masks
#   define arm_2d_tile_copy_with_masks_only                                     \
                arm_2d_cccn888_tile_copy_with_masks_only
#   define arm_2d_tile_copy_with_masks_and_x_mirror                             \
                arm_2d_cccn888_tile_copy_with_masks_and_x_mirror
#   define arm_2d_tile_copy_with_masks_and_y_mirror                             \
                arm_2d_cccn888_tile_copy_with_masks_and_y_mirror
#   define arm_2d_tile_copy_with_masks_and_xy_mirror                            \
                arm_2d_cccn888_tile_copy_with_masks_and_xy_mirror
#   define arm_2d_tile_fill_with_masks_only                                     \
                arm_2d_cccn888_tile_fill_with_masks_only
#   define arm_2d_tile_fill_with_masks_and_x_mirror                             \
                arm_2d_cccn888_tile_fill_with_masks_and_x_mirror
#   define arm_2d_tile_fill_with_masks_and_y_mirror                             \
                arm_2d_cccn888_tile_fill_with_masks_and_y_mirror
#   define arm_2d_tile_fill_with_masks_and_xy_mirror                            \
                arm_2d_cccn888_tile_fill_with_masks_and_xy_mirror
#   define arm_2d_tile_copy_with_src_mask_only                                  \
                arm_2d_cccn888_tile_copy_with_src_mask_only
#   define arm_2d_tile_copy_with_src_mask_and_x_mirror                          \
                arm_2d_cccn888_tile_copy_with_src_mask_and_x_mirror
#   define arm_2d_tile_copy_with_src_mask_and_y_mirror                          \
                arm_2d_cccn888_tile_copy_with_src_mask_and_y_mirror
#   define arm_2d_tile_copy_with_src_mask_and_xy_mirror                         \
                arm_2d_cccn888_tile_copy_with_src_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_src_mask_only                                  \
                arm_2d_cccn888_tile_fill_with_src_mask_only
#   define arm_2d_tile_fill_with_src_mask_and_x_mirror                          \
                arm_2d_cccn888_tile_fill_with_src_mask_and_x_mirror
#   define arm_2d_tile_fill_with_src_mask_and_y_mirror                          \
                arm_2d_cccn888_tile_fill_with_src_mask_and_y_mirror
#   define arm_2d_tile_fill_with_src_mask_and_xy_mirror                         \
                arm_2d_cccn888_tile_fill_with_src_mask_and_xy_mirror
#   define arm_2d_tile_copy_with_des_mask_only                                  \
                arm_2d_cccn888_tile_copy_with_des_mask_only
#   define arm_2d_tile_copy_with_des_mask_and_x_mirror                          \
                arm_2d_cccn888_tile_copy_with_des_mask_and_x_mirror
#   define arm_2d_tile_copy_with_des_mask_and_y_mirror                          \
                arm_2d_cccn888_tile_copy_with_des_mask_and_y_mirror
#   define arm_2d_tile_copy_with_des_mask_and_xy_mirror                         \
                arm_2d_cccn888_tile_copy_with_des_mask_and_xy_mirror
#   define arm_2d_tile_fill_with_des_mask_only                                  \
                arm_2d_cccn888_tile_fill_with_des_mask_only
#   define arm_2d_tile_fill_with_des_mask_and_x_mirror                          \
                arm_2d_cccn888_tile_fill_with_des_mask_and_x_mirror
#   define arm_2d_tile_fill_with_des_mask_and_y_mirror                          \
                arm_2d_cccn888_tile_fill_with_des_mask_and_y_mirror
#   define arm_2d_tile_fill_with_des_mask_and_xy_mirror                         \
                arm_2d_cccn888_tile_fill_with_des_mask_and_xy_mirror
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
#   define arm_2d_tile_copy_with_colour_keying_and_opacity                      \
                arm_2d_cccn888_tile_copy_with_colour_keying_and_opacity
#   define arm_2d_tile_copy_with_colour_keying_only                             \
                arm_2d_rgb32_tile_copy_with_colour_keying_only
#   define arm_2d_tile_copy_with_colour_keying_and_x_mirror                     \
                arm_2d_rgb32_tile_copy_with_colour_keying_and_x_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_y_mirror                     \
                arm_2d_rgb32_tile_copy_with_colour_keying_and_y_mirror
#   define arm_2d_tile_copy_with_colour_keying_and_xy_mirror                    \
                arm_2d_rgb32_tile_copy_with_colour_keying_and_xy_mirror
#   define arm_2d_tile_fill_with_colour_keying_only                             \
                arm_2d_rgb32_tile_fill_with_colour_keying_only
#   define arm_2d_tile_fill_with_colour_keying_and_x_mirror                     \
                arm_2d_rgb32_tile_fill_with_colour_keying_and_x_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_y_mirror                     \
                arm_2d_rgb32_tile_fill_with_colour_keying_and_y_mirror
#   define arm_2d_tile_fill_with_colour_keying_and_xy_mirror                    \
                arm_2d_rgb32_tile_fill_with_colour_keying_and_xy_mirror
#   define arm_2d_fill_colour_with_opacity                                      \
                arm_2d_cccn888_fill_colour_with_opacity
#   define arm_2d_tile_copy_with_opacity                                        \
                arm_2d_cccn888_tile_copy_with_opacity
#   define arm_2d_tile_copy_with_des_mask                                       \
                arm_2d_cccn888_tile_copy_with_des_mask
#   define arm_2d_tile_copy_with_src_mask                                       \
                arm_2d_cccn888_tile_copy_with_src_mask
#   define arm_2dp_tile_transform_only                                          \
                arm_2dp_cccn888_tile_transform_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_cccn888_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_cccn888_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation            arm_2dp_cccn888_tile_rotation
#   define arm_2dp_tile_rotation_with_colour_keying                             \
                arm_2dp_cccn888_tile_rotation_with_colour_keying
#   define arm_2dp_tile_rotation_only       arm_2dp_cccn888_tile_rotation_only
#   define arm_2dp_tile_rotation_only_with_opacity                              \
                arm_2dp_cccn888_tile_rotation_only_with_opacity
#   define arm_2dp_tile_rotation_with_alpha                                     \
                arm_2dp_cccn888_tile_rotation_with_alpha
#   define arm_2dp_tile_rotation_with_opacity                                   \
                arm_2dp_cccn888_tile_rotation_with_opacity
#   define arm_2dp_tile_rotation_with_src_mask_and_opacity                      \
                arm_2dp_cccn888_tile_rotation_with_src_mask_and_opacity
#   define arm_2dp_tile_rotation_with_src_mask                                  \
                arm_2dp_cccn888_tile_rotation_with_src_mask
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_cccn888_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_cccn888_fill_colour_with_a2_mask
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_cccn888_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_draw_point        arm_2d_cccn888_draw_point
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


/*---------------------- Graphic LCD color definitions -----------------------*/

#define __RGB32(__R, __G, __B)     ((((uint32_t)(__R) & 0xFF) << 16)        |   \
                                    (((uint32_t)(__G) & 0xFF) << 8)         |   \
                                    (((uint32_t)(__B) & 0xFF) << 0)         |   \
                                    (uint32_t)0xFF << 24)

#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define __RGB(__R, __G, __B)    ((((__R) + (__G) + (__B)) / 3) & 0xFF)
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define __RGB(__R, __G, __B)    ((((uint16_t)(__R) & 0xFF) >> 3 << 11)   |   \
                                    (((uint16_t)(__G) & 0xFF) >> 2 << 5)    |   \
                                    (((uint16_t)(__B) & 0xFF) >> 3 << 0)    )
#else /* __GLCD_CFG_COLOUR_DEPTH__ == 32 */
#   define __RGB(__R, __G, __B)    __RGB32(__R, __G, __B)
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

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __declare_tile(__name)                                                  \
            extern const arm_2d_tile_t __name;

/*!
 * \brief declare a tile
 * \param __name the name of the tile
 */
#define declare_tile(__name)            __declare_tile(__name)

/*!
 * \brief declare a tile
 * \param __name the name of the tile
 */
#define dcl_tile(__name)                declare_tile(__name)

/*!
 * \brief declare a framebuffer
 * \param __name the name of the framebuffer
 */
#define dcl_fb(__name)                  declare_tile(__name)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __impl_fb(__name, __width, __height, __type, ...)                       \
            ARM_NOINIT static __type                                            \
                __name##Buffer[(__width) * (__height)];                         \
            const arm_2d_tile_t __name = {                                      \
                .tRegion = {                                                    \
                    .tSize = {(__width), (__height)},                           \
                },                                                              \
                .tInfo.bIsRoot = true,                                          \
                .pchBuffer = (uint8_t *)__name##Buffer,                         \
                __VA_ARGS__                                                     \
            }

/*!
 * \brief implement a framebuffer 
 * \param __name the name of the framebuffer
 * \param __width the width
 * \param __height the height
 * \param __type the type of the pixel
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_fb(__name, __width, __height, __type, ...)                         \
            __impl_fb(__name, __width, __height, __type, ##__VA_ARGS__)

/*!
 * \brief implement a framebuffer and allocate it from the heap
 * \note  the framebuffer will be freed automatically when run out of the code
 *        body. 
 * \param __tile_name the name of the framebuffer
 * \param __width the width
 * \param __height the height
 * \param __colour_type the type of the pixel
 */
#define impl_heap_fb(__tile_name, __width, __height, __colour_type)             \
    arm_using(                                                                  \
        arm_2d_tile_t __tile_name = {                                           \
            .pchBuffer = malloc((__width) * (__height) * sizeof(__colour_type)),\
        },                                                                      \
        ({__tile_name.tRegion.tSize.iWidth = (__width);                         \
         __tile_name.tRegion.tSize.iHeight = (__height);                        \
         __tile_name.tInfo.bIsRoot = true;                                      \
         assert(NULL != __tile_name.pchBuffer);                                 \
        }),                                                                     \
        ({ arm_2d_op_wait_async(NULL); free(__tile_name.phwBuffer); }) )

/*!
 * \brief calculate the number of pixels in a given tile
 * \param __name the target tile
 * \return uint32_t the number of pixels
 */
#define get_tile_buffer_pixel_count(__name)                                     \
            (uint32_t)(     (__name.tRegion.tSize.iWidth)                       \
                        *   (__name.tRegion.tSize.iHeight))

/*!
 * \brief calculate the size of a given framebuffer
 * \param __name the target tile
 * \param __type the pixel type
 * \return uint32_t the buffer size
 */
#define get_tile_buffer_size(__name, __type)                                    \
            (get_tile_buffer_pixel_count(__name) * sizeof(__type))


/*!
 * \brief implement a child tile for a given parent
 * \param __parent the parent tile
 * \param __x the x offset in the parent region
 * \param __y the y offset in the parent region
 * \param __width the width of the new tile
 * \param __height the height of the new tile
 * \param ... an optional initialisation for other members of arm_2d_tile_t
 */
#define impl_child_tile(__parent, __x, __y, __width, __height, ...) {           \
        .tRegion = {                                                            \
            .tLocation = {                                                      \
                .iX = (__x),                                                    \
                .iY = (__y),                                                    \
            },                                                                  \
            .tSize = {                                                          \
                .iWidth = (__width),                                            \
                .iHeight = (__height),                                          \
            },                                                                  \
        },                                                                      \
        .tInfo.bIsRoot = false,                                                 \
        .tInfo.bDerivedResource = true,                                         \
        .ptParent = (arm_2d_tile_t *)&(__parent),                               \
        __VA_ARGS__                                                             \
    }

/*----------------------------------------------------------------------------*
 * Helper Macros for Alignment                                                *
 *----------------------------------------------------------------------------*/

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_container( __tile_ptr,                                         \
                            __container_name,                                   \
                            __region_ptr,                                       \
                            ...)                                                \
            for (arm_2d_margin_t ARM_2D_SAFE_NAME(tMargin),                     \
                *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;              \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    (({                                                         \
                    if (!__ARM_VA_NUM_ARGS(__VA_ARGS__)) {                      \
                        ARM_2D_SAFE_NAME(tMargin) = (arm_2d_margin_t){0};       \
                    } else {                                                    \
                        ARM_2D_SAFE_NAME(tMargin)                               \
                            = (arm_2d_margin_t){__VA_ARGS__};                   \
                    };                                                          \
                    }),1) : 0;)                                                 \
            arm_using(                                                          \
                arm_2d_tile_t __container_name = {0},                           \
                {                                                               \
                    arm_2d_region_t                                             \
                        ARM_2D_SAFE_NAME(ContainerRegion) = {0};                \
                    arm_2d_tile_t *ARM_2D_SAFE_NAME(ptTile)                     \
                        = (arm_2d_tile_t *)(__tile_ptr);                        \
                                                                                \
                    if (NULL == ARM_2D_SAFE_NAME(ptTile)) {                     \
                        ARM_2D_SAFE_NAME(ptTile)                                \
                            = arm_2d_get_default_frame_buffer();                \
                        if (NULL == ARM_2D_SAFE_NAME(ptTile)) {                 \
                            break;                                              \
                        }                                                       \
                    }                                                           \
                    if (NULL == (__region_ptr)) {                               \
                        ARM_2D_SAFE_NAME(ContainerRegion).tSize                 \
                            = ARM_2D_SAFE_NAME(ptTile)->tRegion.tSize;          \
                    } else {                                                    \
                        ARM_2D_SAFE_NAME(ContainerRegion) = *(__region_ptr);    \
                    }                                                           \
                    ARM_2D_SAFE_NAME(ContainerRegion).tLocation.iX              \
                        += ARM_2D_SAFE_NAME(tMargin).chLeft;                    \
                    ARM_2D_SAFE_NAME(ContainerRegion).tLocation.iY              \
                        += ARM_2D_SAFE_NAME(tMargin).chTop;                     \
                    ARM_2D_SAFE_NAME(ContainerRegion).tSize.iWidth              \
                        -= ARM_2D_SAFE_NAME(tMargin).chLeft                     \
                         + ARM_2D_SAFE_NAME(tMargin).chRight;                   \
                    ARM_2D_SAFE_NAME(ContainerRegion).tSize.iHeight             \
                        -= ARM_2D_SAFE_NAME(tMargin).chTop                      \
                        + ARM_2D_SAFE_NAME(tMargin).chBottom;                   \
                    arm_2d_tile_generate_child(                                 \
                                            ARM_2D_SAFE_NAME(ptTile),           \
                                            &ARM_2D_SAFE_NAME(ContainerRegion), \
                                            &(__container_name),                \
                                            false);                             \
                },                                                              \
                {                                                               \
                    (void)0;                                                    \
                })  arm_2d_canvas(  &(__container_name),                        \
                                    __container_name##_canvas)

/*!
 * \brief generate a child tile with a given name, a reference region and an 
 *        optional inner margin. A canvas will also be generated with a name:
 *        <container name>_canvas.
 * \note you should append a {} after the arm_2d_container.
 *
 * \param[in] __tile_ptr the address of the parent tile
 * \param[in] __container_name the name of the child tile, we call this child 
 *            tile "Container".
 * \note      a canvas will be generated with a postfix "_canvas" following the
 *            __container_name. 
 *            For example, if the container name is called __inner_container, 
 *            then a canvas called __inner_container_canvas will be generated
 * \param[in] __region_ptr the reference region in the parent tile
 * \param[in] ... an optional inner margin. You can specify the left, right, top
 *                and the bottom margin in order. You can also use .chLeft, 
 *                .chRight, .chTop and .chBottom to set the specified margin. 
 */
#define arm_2d_container(   __tile_ptr,                                         \
                            __container_name,                                   \
                            __region_ptr,                                       \
                            ...)                                                \
            __arm_2d_container( (__tile_ptr),                                   \
                                __container_name,                               \
                                (__region_ptr),##__VA_ARGS__)

#define arm_2d_canvas(__tile_ptr, __region_name)                                \
            arm_using(arm_2d_region_t __region_name = {0},                      \
                        {__region_name.tSize = (__tile_ptr)->tRegion.tSize;},   \
                        {arm_2d_op_wait_async(NULL);})

#define arm_2d_layout(__region)                                                 \
        arm_using(  arm_2d_region_t __arm_2d_layout = {                         \
                        .tLocation = (__region).tLocation                       \
                    },                                                          \
                    __arm_2d_layout_area = (__region),                          \
                    {                                                           \
                          ARM_2D_UNUSED(__arm_2d_layout_area);                  \
                    },                                                          \
                    {                                                           \
                        arm_2d_op_wait_async(NULL);                             \
                    }                                                           \
                )

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_horizontal1(__size)                                       \
            ____item_line_horizontal5(__size,0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_horizontal2(__width, __height)                            \
            ____item_line_horizontal6(__width, __height,0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_horizontal5(__size, __left, __right, __top, __bottom)     \
            ____item_line_horizontal6(  (__size).iWidth, (__size).iHeight,      \
                                        (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_horizontal6(  __width, __height,                          \
                                    __left, __right, __top, __bottom)           \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation = __arm_2d_layout.tLocation;        \
                    __item_region.tLocation.iX += (__left);                     \
                    __item_region.tLocation.iY += (__top);                      \
                },                                                              \
                {                                                               \
                    __arm_2d_layout.tLocation.iX +=                             \
                        (__width) + (__left) + (__right);                       \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __arm_2d_layout.tSize.iHeight = MAX(                        \
                                                __arm_2d_layout.tSize.iHeight,  \
                                                ARM_2D_SAFE_NAME(iHeight));     \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __arm_2d_layout.tSize.iWidth = MAX(                         \
                                                __arm_2d_layout.tSize.iWidth,   \
                                                ARM_2D_SAFE_NAME(iWidth));      \
                    arm_2d_op_wait_async(NULL);                                 \
                })


/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to place a specific 
 *        rectangular area horizontally.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_line_horizontal(__size) {
 *              code body that can use __item_region
 *          }
 * 
 * \note prototype 2:
 *          __item_line_horizontal(__width, __height) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 3:
 *          __item_line_horizontal(__size, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 4:
 *          __item_line_horizontal(__width, __height, 
                                 __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_line_horizontal(...)                                             \
            ARM_CONNECT2(   ____item_line_horizontal,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_vertical1(__size)                                         \
            ____item_line_vertical5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_vertical2(__width, __height)                              \
            ____item_line_vertical6(__width, __height, 0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_vertical5(__size, __left, __right, __top, __bottom)       \
            ____item_line_vertical6((__size).iWidth, (__size).iHeight,          \
                                    (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_line_vertical6(__width, __height,                              \
                                __left, __right, __top, __bottom)               \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation = __arm_2d_layout.tLocation;        \
                    __item_region.tLocation.iX += (__left);                     \
                    __item_region.tLocation.iY += (__top);                      \
                },                                                              \
                {                                                               \
                    __arm_2d_layout.tLocation.iY +=                             \
                        (__height) + (__top) + (__bottom);                      \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __arm_2d_layout.tSize.iHeight = MAX(                        \
                                                __arm_2d_layout.tSize.iHeight,  \
                                                ARM_2D_SAFE_NAME(iHeight));     \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __arm_2d_layout.tSize.iWidth = MAX(                         \
                                                __arm_2d_layout.tSize.iWidth,   \
                                                ARM_2D_SAFE_NAME(iWidth));      \
                    arm_2d_op_wait_async(NULL);                                 \
                })

/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to place a specific 
 *        rectangular area vertically.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_line_vertical(__size) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 2:
 *          __item_line_vertical(__width, __height) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 3:
 *          __item_line_vertical(__size, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 4:
 *          __item_line_vertical(__width, __height, 
                                 __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_line_vertical(...)                                               \
            ARM_CONNECT2(   ____item_line_vertical,                             \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_horizontal1(__size, __height)                                  \
            ____item_horizontal5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_horizontal2(__width, __height)                                 \
            ____item_horizontal6(  __width, __height, 0, 0, 0, 0)



/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_horizontal5( __size, __left, __right, __top, __bottom)         \
            ____item_horizontal6(   (__size).iWidth, (__size).iHeight,          \
                                    (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_horizontal6(  __width, __height,                               \
                                __left, __right, __top, __bottom)               \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    int16_t iTempX = __arm_2d_layout.tLocation.iX               \
                                   + (__left) + (__width);                      \
                    /* is end of the line */                                    \
                    if (    iTempX                                              \
                       >=   (   __arm_2d_layout_area.tLocation.iX               \
                            +   __arm_2d_layout_area.tSize.iWidth)) {           \
                        /* move to the next line */                             \
                        __arm_2d_layout.tLocation.iY +=                         \
                            __arm_2d_layout.tSize.iHeight;                      \
                        /* reset the max line height */                         \
                        __arm_2d_layout.tSize.iHeight = 0;                      \
                        /* start from the left */                               \
                        __arm_2d_layout.tLocation.iX                            \
                            = __arm_2d_layout_area.tLocation.iX;                \
                    }                                                           \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation = __arm_2d_layout.tLocation;        \
                    __item_region.tLocation.iX += (__left);                     \
                    __item_region.tLocation.iY += (__top);                      \
                },                                                              \
                {                                                               \
                    __arm_2d_layout.tLocation.iX +=                             \
                        (__width) + (__left) + (__right);                       \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __arm_2d_layout.tSize.iHeight = MAX(                        \
                                                __arm_2d_layout.tSize.iHeight,  \
                                                ARM_2D_SAFE_NAME(iHeight));     \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __arm_2d_layout.tSize.iWidth = MAX(                         \
                                                __arm_2d_layout.tSize.iWidth,   \
                                                ARM_2D_SAFE_NAME(iWidth));      \
                    /* is end of the line */                                    \
                    if (    __arm_2d_layout.tLocation.iX                        \
                       >=   (   __arm_2d_layout_area.tLocation.iX               \
                            +   __arm_2d_layout_area.tSize.iWidth)) {           \
                        /* move to the next line */                             \
                        __arm_2d_layout.tLocation.iY +=                         \
                            __arm_2d_layout.tSize.iHeight;                      \
                        /* reset the max line height */                         \
                        __arm_2d_layout.tSize.iHeight = 0;                      \
                        /* start from the left */                               \
                        __arm_2d_layout.tLocation.iX                            \
                            = __arm_2d_layout_area.tLocation.iX;                \
                    }                                                           \
                    arm_2d_op_wait_async(NULL);                                 \
                })


/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to place a specific 
 *        rectangular area horizontally and wrap around when hit the boarder.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_horizontal(__size) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 2:
 *          __item_horizontal(__width, __height) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 3:
 *          __item_horizontal(__size, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 4:
 *          __item_horizontal(__width, __height, 
                                 __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_horizontal(...)                                                  \
            ARM_CONNECT2(   ____item_horizontal,                                \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)



/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_vertical1(__size, __height)                                    \
            ____item_vertical5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_vertical2(__width, __height)                                   \
            ____item_vertical6(  __width, __height, 0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_vertical5( __size, __left, __right, __top, __bottom)           \
            ____item_vertical6((__size).iWidth, (__size).iHeight,               \
                               (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define ____item_vertical6( __width, __height,                                  \
                            __left, __right, __top, __bottom)                   \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    int16_t iTempY = __arm_2d_layout.tLocation.iY               \
                                   + (__top) + (__bottom);                      \
                    /* is end of the column */                                  \
                    if (    iTempY                                              \
                       >=   (   __arm_2d_layout_area.tLocation.iY               \
                            +   __arm_2d_layout_area.tSize.iHeight)) {          \
                        /* move to the next column */                           \
                        __arm_2d_layout.tLocation.iX +=                         \
                            __arm_2d_layout.tSize.iWidth;                       \
                        /* reset the max line width */                          \
                        __arm_2d_layout.tSize.iWidth = 0;                       \
                        /* start from the top */                                \
                        __arm_2d_layout.tLocation.iY                            \
                            = __arm_2d_layout_area.tLocation.iY;                \
                    }                                                           \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation = __arm_2d_layout.tLocation;        \
                    __item_region.tLocation.iX += (__left);                     \
                    __item_region.tLocation.iY += (__top);                      \
                },                                                              \
                {                                                               \
                    __arm_2d_layout.tLocation.iY +=                             \
                        (__height) + (__top) + (__bottom);                      \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __arm_2d_layout.tSize.iHeight = MAX(                        \
                                                __arm_2d_layout.tSize.iHeight,  \
                                                ARM_2D_SAFE_NAME(iHeight));     \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __arm_2d_layout.tSize.iWidth = MAX(                         \
                                                __arm_2d_layout.tSize.iWidth,   \
                                                ARM_2D_SAFE_NAME(iWidth));      \
                    /* is end of the column */                                  \
                    if (    __arm_2d_layout.tLocation.iY                        \
                       >=   (   __arm_2d_layout_area.tLocation.iY               \
                            +   __arm_2d_layout_area.tSize.iHeight)) {          \
                        /* move to the next column */                           \
                        __arm_2d_layout.tLocation.iX +=                         \
                            __arm_2d_layout.tSize.iWidth;                       \
                        /* reset the max line width */                          \
                        __arm_2d_layout.tSize.iWidth = 0;                       \
                        /* start from the top */                                \
                        __arm_2d_layout.tLocation.iY                            \
                            = __arm_2d_layout_area.tLocation.iY;                \
                    }                                                           \
                    arm_2d_op_wait_async(NULL);                                 \
                })


/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to place a specific 
 *        rectangular area vertically and wrap around when hit the boarder.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_vertical(__size) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 2:
 *          __item_vertical(__width, __height) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 3:
 *          __item_vertical(__size, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 4:
 *          __item_vertical(__width, __height, 
                                 __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_vertical(...)                                                    \
            ARM_CONNECT2(   ____item_vertical,                                  \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_left2(__region, __size)                              \
            arm_using(                                                          \
                arm_2d_region_t __top_left_region,                              \
                {                                                               \
                    __top_left_region.tSize.iWidth = (__size).iWidth;           \
                    __top_left_region.tSize.iHeight = (__size).iHeight;         \
                    __top_left_region.tLocation = (__region).tLocation;         \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                    arm_2d_region_t __top_left_region,                          \
                    {                                                           \
                        __top_left_region.tSize.iWidth = (__width);             \
                        __top_left_region.tSize.iHeight = (__height);           \
                        __top_left_region.tLocation = (__region).tLocation;     \
                    },                                                          \
                    {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_top_left(__region, __size) {
 *              code body that can use __top_left_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_top_left(__region, __width, __height) {
 *              code body that can use __top_left_region
 *          }
 *          
 */
#define arm_2d_align_top_left(...)                                              \
            ARM_CONNECT2(   __arm_2d_align_top_left,                            \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_centre2(__region, __size)                            \
            arm_using(                                                          \
                arm_2d_region_t __top_centre_region,                            \
                {                                                               \
                    __top_centre_region.tSize.iWidth = (__size).iWidth;         \
                    __top_centre_region.tSize.iHeight = (__size).iHeight;       \
                    __top_centre_region.tLocation = (__region).tLocation;       \
                    __top_centre_region.tLocation.iX                            \
                        += ((__region).tSize.iWidth - (__size).iWidth) >> 1;    \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_centre3(__region, __width, __height)                 \
            arm_using(                                                          \
                    arm_2d_region_t __top_centre_region,                        \
                    {                                                           \
                        __top_centre_region.tSize.iWidth = (__width);           \
                        __top_centre_region.tSize.iHeight = (__height);         \
                        __top_centre_region.tLocation = (__region).tLocation;   \
                        __top_centre_region.tLocation.iX                        \
                            += ((__region).tSize.iWidth - (__width)) >> 1;      \
                    },                                                          \
                    {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_top_centre(__region, __size) {
 *              code body that can use __top_centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_top_centre(__region, __width, __height) {
 *              code body that can use __top_centre_region
 *          }
 *          
 */
#define arm_2d_align_top_centre(...)                                            \
            ARM_CONNECT2(   __arm_2d_align_top_centre,                          \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_right2(__region, __size)                             \
            arm_using(                                                          \
                arm_2d_region_t __top_right_region,                             \
                {                                                               \
                    __top_right_region.tSize.iWidth = (__size).iWidth;          \
                    __top_right_region.tSize.iHeight = (__size).iHeight;        \
                    __top_right_region.tLocation = (__region).tLocation;        \
                    __top_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_top_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                    arm_2d_region_t __top_right_region,                         \
                    {                                                           \
                        __top_right_region.tSize.iWidth = (__width);            \
                        __top_right_region.tSize.iHeight = (__height);          \
                        __top_right_region.tLocation = (__region).tLocation;    \
                        __top_right_region.tLocation.iX                         \
                            += ((__region).tSize.iWidth - (__width));           \
                    },                                                          \
                    {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         top-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_top_right(__region, __size) {
 *              code body that can use __top_right_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_top_right(__region, __width, __height) {
 *              code body that can use __top_right_region
 *          }
 *          
 */
#define arm_2d_align_top_right(...)                                             \
            ARM_CONNECT2(   __arm_2d_align_top_right,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)




/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_left2(__region, __size)                              \
            arm_using(                                                          \
                arm_2d_region_t __mid_left_region,                              \
                {                                                               \
                    __mid_left_region.tSize.iWidth = (__size).iWidth;           \
                    __mid_left_region.tSize.iHeight = (__size).iHeight;         \
                    __mid_left_region.tLocation = (__region).tLocation;         \
                    __mid_left_region.tLocation.iY                              \
                        += ((__region).tSize.iHeight - (__size).iHeight) >> 1;  \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                    arm_2d_region_t __mid_left_region,                          \
                    {                                                           \
                        __mid_left_region.tSize.iWidth = (__width);             \
                        __mid_left_region.tSize.iHeight = (__height);           \
                        __mid_left_region.tLocation = (__region).tLocation;     \
                        __mid_left_region.tLocation.iY                          \
                            += ((__region).tSize.iHeight - (__height)) >> 1;    \
                    },                                                          \
                    {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         middle-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_mid_left(__region, __size) {
 *              code body that can use __mid_left_region
 *          }
 *
 * \note prototype 2:
 *          arm_2d_align_mid_left(__region, __width, __height) {
 *              code body that can use __mid_left_region
 *          }
 *
 */
#define arm_2d_align_mid_left(...)                                              \
            ARM_CONNECT2(   __arm_2d_align_mid_left,                            \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_left(__region, __size) {
 *              code body that can use __left_region
 *          }
 *
 * \note prototype 2:
 *          arm_2d_align_left(__region, __width, __height) {
 *              code body that can use __left_region
 *          }
 *
 */
#define arm_2d_align_left(...)      arm_2d_align_mid_left(__VA_ARGS__)

/*!
 * \brief the alias of __mid_left_region
 */
#define __left_region               __mid_left_region

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre2(__region, __size)                                \
            arm_using(                                                          \
                arm_2d_region_t __centre_region,                                \
                {                                                               \
                    __centre_region.tSize.iWidth = (__size).iWidth;             \
                    __centre_region.tSize.iHeight = (__size).iHeight;           \
                    __centre_region.tLocation = (__region).tLocation;           \
                    __centre_region.tLocation.iX                                \
                        += ((__region).tSize.iWidth - (__size).iWidth)  >> 1;   \
                    __centre_region.tLocation.iY                                \
                        += ((__region).tSize.iHeight - (__size).iHeight)>> 1;   \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_centre3(__region, __width, __height)                     \
            arm_using(  arm_2d_region_t __centre_region,                        \
                        {                                                       \
                            __centre_region.tSize.iWidth = (__width);           \
                            __centre_region.tSize.iHeight = (__height);         \
                            __centre_region.tLocation = (__region).tLocation;   \
                            __centre_region.tLocation.iX                        \
                                += ((__region).tSize.iWidth - (__width))  >> 1; \
                            __centre_region.tLocation.iY                        \
                                += ((__region).tSize.iHeight - (__height))>> 1; \
                        },                                                      \
                        {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_centre(__region, __size) {
 *              code body that can use __centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_centre(__region, __width, __height) {
 *              code body that can use __centre_region
 *          }
 *          
 */
#define arm_2d_align_centre(...)                                                \
            ARM_CONNECT2(   __arm_2d_align_centre,                              \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_right2(__region, __size)                             \
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__size).iWidth;          \
                    __mid_right_region.tSize.iHeight = (__size).iHeight;        \
                    __mid_right_region.tLocation = (__region).tLocation;        \
                    __mid_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                    __mid_right_region.tLocation.iY                             \
                        += ((__region).tSize.iHeight - (__size).iHeight) >> 1;  \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_mid_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__width);                \
                    __mid_right_region.tSize.iHeight = (__height);              \
                    __mid_right_region.tLocation = (__region).tLocation;        \
                    __mid_right_region.tLocation.iX                             \
                        += ((__region).tSize.iWidth - (__width));               \
                    __mid_right_region.tLocation.iY                             \
                        += ((__region).tSize.iHeight - (__height)) >> 1;        \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         middle-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_mid_right(__region, __size) {
 *              code body that can use __mid_right_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_mid_right(__region, __width, __height) {
 *              code body that can use __mid_right_region
 *          }
 *          
 */
#define arm_2d_align_mid_right(...)                                             \
            ARM_CONNECT2(   __arm_2d_align_mid_right,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_right(__region, __size) {
 *              code body that can use __right_region
 *          }
 *
 * \note prototype 2:
 *          arm_2d_align_right(__region, __width, __height) {
 *              code body that can use __right_region
 *          }
 *
 */
#define arm_2d_align_right(...)      arm_2d_align_mid_right(__VA_ARGS__)

/*!
 * \brief the alias of __mid_right_region
 */
#define __right_region               __mid_right_region

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_left2(__region, __size)                           \
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__size).iWidth;        \
                    __bottom_left_region.tSize.iHeight = (__size).iHeight;      \
                    __bottom_left_region.tLocation = (__region).tLocation;      \
                    __bottom_left_region.tLocation.iY                           \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_left3(__region, __width, __height)                \
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__width);              \
                    __bottom_left_region.tSize.iHeight = (__height);            \
                    __bottom_left_region.tLocation = (__region).tLocation;      \
                    __bottom_left_region.tLocation.iY                           \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-left alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_bottom_left(__region, __size) {
 *              code body that can use __bottom_left_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_bottom_left(__region, __width, __height) {
 *              code body that can use __bottom_left_region
 *          }
 *          
 */
#define arm_2d_align_bottom_left(...)                                           \
            ARM_CONNECT2(   __arm_2d_align_bottom_left,                         \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_centre2(__region, __size)                         \
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__size).iWidth;      \
                    __bottom_centre_region.tSize.iHeight = (__size).iHeight;    \
                    __bottom_centre_region.tLocation = (__region).tLocation;    \
                    __bottom_centre_region.tLocation.iX                         \
                        += ((__region).tSize.iWidth - (__size).iWidth)  >> 1;   \
                    __bottom_centre_region.tLocation.iY                         \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_centre3(__region, __width, __height)              \
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__width);            \
                    __bottom_centre_region.tSize.iHeight = (__height);          \
                    __bottom_centre_region.tLocation = (__region).tLocation;    \
                    __bottom_centre_region.tLocation.iX                         \
                        += ((__region).tSize.iWidth - (__width))  >> 1;         \
                    __bottom_centre_region.tLocation.iY                         \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-central alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_align_bottom_centre(__region, __size) {
 *              code body that can use __bottom_centre_region
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_align_bottom_centre(__region, __width, __height) {
 *              code body that can use __bottom_centre_region
 *          }
 *          
 */
#define arm_2d_align_bottom_centre(...)                                         \
            ARM_CONNECT2(   __arm_2d_align_bottom_centre,                       \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_right2(__region, __size)                          \
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__size).iWidth;       \
                    __bottom_right_region.tSize.iHeight = (__size).iHeight;     \
                    __bottom_right_region.tLocation = (__region).tLocation;     \
                    __bottom_right_region.tLocation.iX                          \
                        += ((__region).tSize.iWidth - (__size).iWidth);         \
                    __bottom_right_region.tLocation.iY                          \
                        += ((__region).tSize.iHeight - (__size).iHeight);       \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief Please do NOT use this macro
 * 
 */
#define __arm_2d_align_bottom_right3(__region, __width, __height)               \
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__width);             \
                    __bottom_right_region.tSize.iHeight = (__height);           \
                    __bottom_right_region.tLocation = (__region).tLocation;     \
                    __bottom_right_region.tLocation.iX                          \
                        += ((__region).tSize.iWidth - (__width));               \
                    __bottom_right_region.tLocation.iY                          \
                        += ((__region).tSize.iHeight - (__height));             \
                },                                                              \
                {arm_2d_op_wait_async(NULL);})

/*!
 * \brief generate a temporary arm_2d_region_t object with use specified info for
*         bottom-right alignment. 
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __arm_2d_align_bottom_right(__region, __size) {
 *              code body that can use __bottom_right_region
 *          }
 * 
 * \note prototype 2:
 *          __arm_2d_align_bottom_right(__region, __width, __height) {
 *              code body that can use __bottom_right_region
 *          }
 *          
 */
#define arm_2d_align_bottom_right(...)                                          \
            ARM_CONNECT2(   __arm_2d_align_bottom_right,                        \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief a template for implement a on draw event handler
 */
#define IMPL_ON_DRAW_EVT(__NAME)                                                \
            arm_fsm_rt_t __NAME(void *pTarget,                                  \
                                const arm_2d_tile_t *ptTile,                    \
                                bool bIsNewFrame)

/*============================ TYPES =========================================*/

/*! 
 * \brief alignment 
 */
enum {
    ARM_2D_ALIGN_LEFT               = _BV(0),                                   /*!< align to left */
    ARM_2D_ALIGN_RIGHT              = _BV(1),                                   /*!< align to right */
    ARM_2D_ALIGN_TOP                = _BV(2),                                   /*!< align to top */
    ARM_2D_ALIGN_BOTTOM             = _BV(3),                                   /*!< align to bottom */
    
    ARM_2D_ALIGN_CENTRE             = 0,                                        /*!< align to centre */
    ARM_2D_ALIGN_CENTRE_ALIAS       = ARM_2D_ALIGN_LEFT                         /*!< align to centre */
                                    | ARM_2D_ALIGN_RIGHT
                                    | ARM_2D_ALIGN_TOP
                                    | ARM_2D_ALIGN_BOTTOM,

    ARM_2D_ALIGN_TOP_LEFT           = ARM_2D_ALIGN_TOP                          /*!< align to top left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_TOP_RIGHT          = ARM_2D_ALIGN_TOP                          /*!< align to top right corner */
                                    | ARM_2D_ALIGN_RIGHT,
    ARM_2D_ALIGN_BOTTOM_LEFT        = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_BOTTOM_RIGHT       = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom right corner */
                                    | ARM_2D_ALIGN_RIGHT,
};

/*!
 * \brief the margin inside a region / container
 */
typedef struct arm_2d_margin_t {
    uint8_t chLeft;                                                             /*!< left margin */
    uint8_t chRight;                                                            /*!< right margin */
    uint8_t chTop;                                                              /*!< top margin */
    uint8_t chBottom;                                                           /*!< bottom margin */
} arm_2d_margin_t;

/*!
 * \brief the padding between rectanglar areas
 */
typedef struct arm_2d_padding_t {
    int8_t chLeft;                                                              /*!< left padding */
    int8_t chRight;                                                             /*!< right padding */
    int8_t chTop;                                                               /*!< top padding */
    int8_t chBottom;                                                            /*!< bottom padding */
} arm_2d_padding_t;

/*!
 * \brief the On-Drawing event handler for application layer
 * 
 * \param[in] pTarget a user attached target address 
 * \param[in] ptTile a tile for the virtual screen
 * \param[in] bIsNewFrame a flag indicate the starting of a new frame
 * \return arm_fsm_rt_t the status of the FSM.  
 */
typedef arm_fsm_rt_t arm_2d_helper_draw_handler_t( 
                                          void *pTarget,
                                          const arm_2d_tile_t *ptTile,
                                          bool bIsNewFrame);

/*!
 * \brief on drawing event 
 */
typedef struct arm_2d_helper_draw_evt_t {
    arm_2d_helper_draw_handler_t *fnHandler;                //!< event handler function
    void *pTarget;                                          //!< user attached target
} arm_2d_helper_draw_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

__STATIC_INLINE
uint8_t arm_2d_helper_alpha_mix(uint_fast8_t chAlpha1, 
                                                uint_fast8_t chAlpha2)
{
    chAlpha1 = MIN(255, chAlpha1);
    chAlpha2 = MIN(255, chAlpha2);
    return (uint8_t)((chAlpha1 == 255) ? chAlpha2 
                             : ((chAlpha2 == 255) ? chAlpha1 
                                                  : (chAlpha1 * chAlpha2 >> 8)));
}

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
