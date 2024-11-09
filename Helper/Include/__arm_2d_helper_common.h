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
 * $Date:        09. Nov 2024
 * $Revision:    V.1.6.11
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
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress=1296,64,1,177
#endif


/*!
 * \addtogroup gHelper 8 Helper Services
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

#   define arm_2d_pixel_from_brga8888  arm_2d_pixel_brga8888_to_gray8

#   define __arm_2d_iir_blur_acc_t  __arm_2d_iir_blur_acc_gray8_t

#   define arm_2dp_filter_iir_blur  arm_2dp_gray8_filter_iir_blur

#   define arm_2dp_filter_iir_blur_depose                                       \
                arm_2dp_gray8_filter_iir_blur_depose

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
#   define arm_2d_tile_copy_with_src_mask_and_opacity_only                      \
                arm_2d_gray8_tile_copy_with_src_mask_and_opacity_only
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
#   define arm_2d_tile_scaling_only                                             \
                arm_2d_gray8_tile_scaling_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_gray8_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_gray8_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_gray8_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_transform_only_with_opacity                             \
                arm_2dp_gray8_tile_transform_only_with_opacity
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
#   define arm_2d_fill_colour_with_a1_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a1_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a1_mask                                      \
                arm_2d_gray8_fill_colour_with_a1_mask
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_gray8_fill_colour_with_a2_mask
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_gray8_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a8_mask                                      \
                arm_2d_gray8_fill_colour_with_a8_mask
#   define arm_2d_fill_colour_with_a8_mask_and_opacity                          \
                arm_2d_gray8_fill_colour_with_a8_mask_and_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_gray8_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_gray8_fill_colour_with_mask_and_opacity
#   define arm_2d_fill_colour_with_mask_and_x_mirror                            \
                arm_2d_gray8_fill_colour_with_mask_and_x_mirror
#   define arm_2d_fill_colour_with_mask_x_mirror_and_opacity                    \
                arm_2d_gray8_fill_colour_with_mask_x_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_y_mirror                            \
                arm_2d_gray8_fill_colour_with_mask_and_y_mirror
#   define arm_2d_fill_colour_with_mask_y_mirror_and_opacity                    \
                arm_2d_gray8_fill_colour_with_mask_y_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_xy_mirror                           \
                arm_2d_gray8_fill_colour_with_mask_and_xy_mirror
#   define arm_2d_fill_colour_with_mask_xy_mirror_and_opacity                   \
                arm_2d_gray8_fill_colour_with_mask_xy_mirror_and_opacity
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_gray8_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_gray8_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_horizontal_line_mask                         \
                arm_2d_gray8_fill_colour_with_horizontal_line_mask
#   define arm_2d_fill_colour_with_horizontal_line_mask_and_opacity             \
                arm_2d_gray8_fill_colour_with_horizontal_line_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_line_mask                           \
                arm_2d_gray8_fill_colour_with_vertical_line_mask
#   define arm_2d_fill_colour_with_vertical_line_mask_and_opacity               \
                arm_2d_gray8_fill_colour_with_vertical_line_mask_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient                          \
                arm_2d_gray8_fill_colour_with_4pts_alpha_gradient
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_opacity              \
                arm_2d_gray8_fill_colour_with_4pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient                          \
                arm_2d_gray8_fill_colour_with_3pts_alpha_gradient
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_opacity              \
                arm_2d_gray8_fill_colour_with_3pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient                      \
                arm_2d_gray8_fill_colour_with_vertical_alpha_gradient
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_opacity          \
            arm_2d_gray8_fill_colour_with_vertical_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient                    \
                arm_2d_gray8_fill_colour_with_horizontal_alpha_gradient
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_opacity        \
            arm_2d_gray8_fill_colour_with_horizontal_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_mask                 \
                arm_2d_gray8_fill_colour_with_4pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_mask_and_opacity         \
            arm_2d_gray8_fill_colour_with_4pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_mask                 \
                arm_2d_gray8_fill_colour_with_3pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_mask_and_opacity         \
            arm_2d_gray8_fill_colour_with_3pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_mask             \
                arm_2d_gray8_fill_colour_with_vertical_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_mask_and_opacity     \
        arm_2d_gray8_fill_colour_with_vertical_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_mask           \
                arm_2d_gray8_fill_colour_with_horizontal_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity   \
        arm_2d_gray8_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity

#   define arm_2d_draw_point        arm_2d_gray8_draw_point
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define __arm_2d_color_t         arm_2d_color_rgb565_t
#   define COLOUR_INT               uint16_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_RGB565

#   define arm_2d_pixel_from_brga8888  arm_2d_pixel_brga8888_to_rgb565

#   define __arm_2d_iir_blur_acc_t  __arm_2d_iir_blur_acc_rgb565_t

#   define arm_2dp_filter_iir_blur  arm_2dp_rgb565_filter_iir_blur

#   define arm_2dp_filter_iir_blur_depose                                       \
                arm_2dp_rgb565_filter_iir_blur_depose

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
#   define arm_2d_tile_copy_with_src_mask_and_opacity_only                      \
                arm_2d_rgb565_tile_copy_with_src_mask_and_opacity_only
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
#   define arm_2d_tile_scaling_only                                             \
                arm_2d_rgb565_tile_scaling_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_rgb565_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_rgb565_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_transform_only_with_opacity                             \
                arm_2dp_rgb565_tile_transform_only_with_opacity
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
#   define arm_2d_fill_colour_with_a1_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a1_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a1_mask                                      \
                arm_2d_rgb565_fill_colour_with_a1_mask
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_rgb565_fill_colour_with_a2_mask
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_rgb565_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a8_mask                                      \
                arm_2d_rgb565_fill_colour_with_a8_mask
#   define arm_2d_fill_colour_with_a8_mask_and_opacity                          \
                arm_2d_rgb565_fill_colour_with_a8_mask_and_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_rgb565_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_rgb565_fill_colour_with_mask_and_opacity
#   define arm_2d_fill_colour_with_mask_and_x_mirror                            \
                arm_2d_rgb565_fill_colour_with_mask_and_x_mirror
#   define arm_2d_fill_colour_with_mask_x_mirror_and_opacity                    \
                arm_2d_rgb565_fill_colour_with_mask_x_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_y_mirror                            \
                arm_2d_rgb565_fill_colour_with_mask_and_y_mirror
#   define arm_2d_fill_colour_with_mask_y_mirror_and_opacity                    \
                arm_2d_rgb565_fill_colour_with_mask_y_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_xy_mirror                           \
                arm_2d_rgb565_fill_colour_with_mask_and_xy_mirror
#   define arm_2d_fill_colour_with_mask_xy_mirror_and_opacity                   \
                arm_2d_rgb565_fill_colour_with_mask_xy_mirror_and_opacity
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_rgb565_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_rgb565_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_horizontal_line_mask                         \
                arm_2d_rgb565_fill_colour_with_horizontal_line_mask
#   define arm_2d_fill_colour_with_horizontal_line_mask_and_opacity             \
                arm_2d_rgb565_fill_colour_with_horizontal_line_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_line_mask                           \
                arm_2d_rgb565_fill_colour_with_vertical_line_mask
#   define arm_2d_fill_colour_with_vertical_line_mask_and_opacity               \
                arm_2d_rgb565_fill_colour_with_vertical_line_mask_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient                          \
                arm_2d_rgb565_fill_colour_with_4pts_alpha_gradient
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_opacity              \
                arm_2d_rgb565_fill_colour_with_4pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient                          \
                arm_2d_rgb565_fill_colour_with_3pts_alpha_gradient
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_opacity              \
                arm_2d_rgb565_fill_colour_with_3pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient                      \
                arm_2d_rgb565_fill_colour_with_vertical_alpha_gradient
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_opacity          \
            arm_2d_rgb565_fill_colour_with_vertical_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient                    \
                arm_2d_rgb565_fill_colour_with_horizontal_alpha_gradient
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_opacity        \
            arm_2d_rgb565_fill_colour_with_horizontal_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_mask                 \
                arm_2d_rgb565_fill_colour_with_4pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_mask_and_opacity         \
            arm_2d_rgb565_fill_colour_with_4pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_mask                 \
                arm_2d_rgb565_fill_colour_with_3pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_mask_and_opacity         \
            arm_2d_rgb565_fill_colour_with_3pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_mask             \
                arm_2d_rgb565_fill_colour_with_vertical_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_mask_and_opacity     \
        arm_2d_rgb565_fill_colour_with_vertical_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_mask           \
                arm_2d_rgb565_fill_colour_with_horizontal_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity   \
        arm_2d_rgb565_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity

#   define arm_2d_draw_point        arm_2d_rgb565_draw_point
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define __arm_2d_color_t         arm_2d_color_cccn888_t
#   define COLOUR_INT               uint32_t
#   define ARM_2D_COLOUR            ARM_2D_COLOUR_CCCA8888

#   define arm_2d_pixel_from_brga8888  

#   define __arm_2d_iir_blur_acc_t  __arm_2d_iir_blur_acc_cccn888_t

#   define arm_2dp_filter_iir_blur  arm_2dp_cccn888_filter_iir_blur
#   define arm_2dp_filter_iir_blur_depose                                       \
                arm_2dp_cccn888_filter_iir_blur_depose

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
#   define arm_2d_tile_copy_with_src_mask_and_opacity_only                      \
                arm_2d_cccn888_tile_copy_with_src_mask_and_opacity_only
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
#   define arm_2d_tile_scaling_only                                             \
                arm_2d_cccn888_tile_scaling_only
#   define arm_2dp_tile_transform_with_colour_keying                            \
                arm_2dp_cccn888_tile_transform_with_colour_keying
#   define arm_2dp_tile_transform_with_opacity                                  \
                arm_2dp_cccn888_tile_transform_with_opacity
#   define arm_2dp_tile_transform_with_src_mask_and_opacity                     \
                arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity
#   define arm_2dp_tile_transform_only_with_opacity                             \
                arm_2dp_cccn888_tile_transform_only_with_opacity
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
#   define arm_2d_fill_colour_with_a1_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a1_mask_and_opacity
#   define arm_2d_fill_colour_with_a2_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a2_mask_and_opacity
#   define arm_2d_fill_colour_with_a1_mask                                      \
                arm_2d_cccn888_fill_colour_with_a1_mask
#   define arm_2d_fill_colour_with_a2_mask                                      \
                arm_2d_cccn888_fill_colour_with_a2_mask
#   define arm_2d_fill_colour_with_a4_mask                                      \
                arm_2d_cccn888_fill_colour_with_a4_mask
#   define arm_2d_fill_colour_with_a4_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a4_mask_and_opacity
#   define arm_2d_fill_colour_with_a8_mask                                      \
                arm_2d_cccn888_fill_colour_with_a8_mask
#   define arm_2d_fill_colour_with_a8_mask_and_opacity                          \
                arm_2d_cccn888_fill_colour_with_a8_mask_and_opacity
#   define arm_2d_fill_colour_with_mask                                         \
                arm_2d_cccn888_fill_colour_with_mask
#   define arm_2d_fill_colour_with_mask_and_opacity                             \
                arm_2d_cccn888_fill_colour_with_mask_and_opacity
#   define arm_2d_fill_colour_with_mask_and_x_mirror                            \
                arm_2d_cccn888_fill_colour_with_mask_and_x_mirror
#   define arm_2d_fill_colour_with_mask_x_mirror_and_opacity                    \
                arm_2d_cccn888_fill_colour_with_mask_x_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_y_mirror                            \
                arm_2d_cccn888_fill_colour_with_mask_and_y_mirror
#   define arm_2d_fill_colour_with_mask_y_mirror_and_opacity                    \
                arm_2d_cccn888_fill_colour_with_mask_y_mirror_and_opacity
#   define arm_2d_fill_colour_with_mask_and_xy_mirror                           \
                arm_2d_cccn888_fill_colour_with_mask_and_xy_mirror
#   define arm_2d_fill_colour_with_mask_xy_mirror_and_opacity                   \
                arm_2d_cccn888_fill_colour_with_mask_xy_mirror_and_opacity
#   define arm_2dp_fill_colour_with_mask_opacity_and_transform                  \
                arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_mask_opacity_and_transform                   \
                arm_2d_cccn888_fill_colour_with_mask_opacity_and_transform
#   define arm_2d_fill_colour_with_horizontal_line_mask                         \
                arm_2d_cccn888_fill_colour_with_horizontal_line_mask
#   define arm_2d_fill_colour_with_horizontal_line_mask_and_opacity             \
                arm_2d_cccn888_fill_colour_with_horizontal_line_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_line_mask                           \
                arm_2d_cccn888_fill_colour_with_vertical_line_mask
#   define arm_2d_fill_colour_with_vertical_line_mask_and_opacity               \
                arm_2d_cccn888_fill_colour_with_vertical_line_mask_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient                          \
                arm_2d_cccn888_fill_colour_with_4pts_alpha_gradient
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_opacity              \
                arm_2d_cccn888_fill_colour_with_4pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient                          \
                arm_2d_cccn888_fill_colour_with_3pts_alpha_gradient
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_opacity              \
                arm_2d_cccn888_fill_colour_with_3pts_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient                      \
                arm_2d_cccn888_fill_colour_with_vertical_alpha_gradient
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_opacity          \
            arm_2d_cccn888_fill_colour_with_vertical_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient                    \
                arm_2d_cccn888_fill_colour_with_horizontal_alpha_gradient
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_opacity        \
        arm_2d_cccn888_fill_colour_with_horizontal_alpha_gradient_and_opacity
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_and_mask                 \
                arm_2d_cccn888_fill_colour_with_4pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_4pts_alpha_gradient_mask_and_opacity         \
            arm_2d_cccn888_fill_colour_with_4pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_and_mask                 \
                arm_2d_cccn888_fill_colour_with_3pts_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_3pts_alpha_gradient_mask_and_opacity         \
            arm_2d_cccn888_fill_colour_with_3pts_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_and_mask             \
                arm_2d_cccn888_fill_colour_with_vertical_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_vertical_alpha_gradient_mask_and_opacity     \
        arm_2d_cccn888_fill_colour_with_vertical_alpha_gradient_mask_and_opacity
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_and_mask           \
            arm_2d_cccn888_fill_colour_with_horizontal_alpha_gradient_and_mask
#   define arm_2d_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity   \
    arm_2d_cccn888_fill_colour_with_horizontal_alpha_gradient_mask_and_opacity

#   define arm_2d_draw_point        arm_2d_cccn888_draw_point
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


/*---------------------- Graphic LCD color definitions -----------------------*/

#undef __RGB
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
#define GLCD_COLOR_ORANGE       __RGB( 255, 128, 0    )

/*!
 * \brief Please do NOT use this macro directly
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __impl_fb(__name, __width, __height, __type, ...)                       \
            ARM_SECTION(".bss.noinit." #__name)                                 \
            __type __name##Buffer[(__width) * (__height)];                      \
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
            .pchBuffer = __arm_2d_allocate_scratch_memory(                      \
                (__width) * (__height) * sizeof(__colour_type),                 \
                __alignof__(__colour_type),                                     \
                ARM_2D_MEM_TYPE_FAST),                                          \
        },                                                                      \
        ({__tile_name.tRegion.tSize.iWidth = (__width);                         \
         __tile_name.tRegion.tSize.iHeight = (__height);                        \
         __tile_name.tInfo.bIsRoot = true;                                      \
         assert(NULL != __tile_name.pchBuffer);                                 \
        }),                                                                     \
        ({  ARM_2D_OP_WAIT_ASYNC();                                             \
            __arm_2d_free_scratch_memory(                                       \
                ARM_2D_MEM_TYPE_FAST,                                           \
                __tile_name.phwBuffer); }) )

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

#if !defined(__ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__)
#   define __ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__      0
#endif

//#if !__ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
#   define __ARM_2D_CONTAINER_DEBUG__(__tile_ptr, __name, __region_ptr)
//#else
//#   define __ARM_2D_CONTAINER_DEBUG__(__tile_ptr, __name, __region_ptr)
//#endif

/*!
 * \brief Please do NOT use this macro directly directly
 * 
 */
#define __arm_2d_container( __tile_ptr,                                         \
                            __container_name,                                   \
                            __region_ptr,                                       \
                            ...)                                                \
        __ARM_2D_CONTAINER_DEBUG__(__tile_ptr, __container_name, __region_ptr)  \
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
                }) arm_2d_canvas( &__container_name, __container_name##_canvas)

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

#if !__ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
#   define __ARM_2D_CANVAS_DEBUG__(__tile_ptr, __region_name)
#else
#   define __ARM_2D_CANVAS_DEBUG__(__tile_ptr, __region_name)                   \
        __ARM_USING2(__arm_2d_layout_debug_t __arm_2d_reserve_canvas__ = {      \
                        .ptTile = (arm_2d_tile_t *)(__tile_ptr)                 \
                    },                                                          \
                    { /* on leave */                                            \
                        COLOUR_INT tColor = arm_2d_pixel_from_brga8888(         \
                                        __arm_2d_helper_colour_slider(          \
                                            __RGB32(0, 0xFF, 0),                \
                                            __RGB32(0, 0, 0xFF),                \
                                            8,                                  \
                                            __arm_2d_reserve_canvas__.wLevel)); \
                        arm_2d_helper_draw_box(                                 \
                            (__arm_2d_reserve_canvas__.ptTile),                 \
                            NULL,                                               \
                            1,                                                  \
                            tColor,                                             \
                            128);                                               \
                })
#endif

#define __arm_2d_canvas(__tile_ptr, __region_name, ...)                         \
            arm_using(arm_2d_region_t __region_name = {0},                      \
                    {                                                           \
                        ARM_2D_UNUSED(__region_name);                           \
                        __region_name.tSize = (__tile_ptr)->tRegion.tSize;      \
                    },                                                          \
                    {                                                           \
                        ARM_2D_OP_WAIT_ASYNC();                                 \
                    })

#if 1
#define arm_2d_canvas(__tile_ptr, __region_name, ...)                           \
            __ARM_2D_CANVAS_DEBUG__(__tile_ptr, __region_name)                  \
                for (arm_2d_region_t __region_name = {0},                       \
                    *ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr) = NULL;          \
                 ARM_CONNECT3(__ARM_USING_, __LINE__,_ptr)++ == NULL ?          \
                    ({ /* on enter operations */                                \
                        ARM_2D_UNUSED(__region_name);                           \
                        __region_name.tSize                                     \
                            = (__tile_ptr)->tRegion.tSize;                      \
                        const arm_2d_tile_t *ARM_2D_SAFE_NAME(ptScreen) = NULL; \
                        const arm_2d_tile_t *ARM_2D_SAFE_NAME(ptRootTile)       \
                            = __arm_2d_tile_get_virtual_screen_or_root(         \
                                        (const arm_2d_tile_t *)(__tile_ptr),    \
                                        NULL,                                   \
                                        NULL,                                   \
                                        &ARM_2D_SAFE_NAME(ptScreen),            \
                                        false);                                 \
                        bool bDrawCanvas =                                      \
                        (   NULL != ARM_2D_SAFE_NAME(ptRootTile)                \
                        ||  (   (NULL != ARM_2D_SAFE_NAME(ptScreen))            \
                            &&  (ARM_2D_SAFE_NAME(ptScreen)->tInfo.u3ExtensionID\
                                    == ARM_2D_TILE_EXTENSION_PFB)               \
                            &&  (   ARM_2D_SAFE_NAME(ptScreen)                  \
                                        ->tInfo.Extension.PFB.bIsDryRun         \
                                ||  ARM_2D_SAFE_NAME(ptScreen)                  \
                                        ->tInfo.Extension.PFB.bIsNewFrame))     \
                        );                                                      \
                        if (bDrawCanvas) {                                      \
                            ARM_2D_LOG_INFO(                                    \
                                APP,                                            \
                                0,                                              \
                                "Canvas " #__region_name,                       \
                                "Draw Canvas for the target tile [%p] ",        \
                                (__tile_ptr)                                    \
                            );                                                  \
                        } else {                                                \
                            ARM_2D_LOG_INFO(                                    \
                                APP,                                            \
                                0,                                              \
                                "Canvas " #__region_name,                       \
                                "Skip Canvas for the target tile [%p]"          \
                                "as the PFB is out of range.",                  \
                                (__tile_ptr)                                    \
                            );                                                  \
                        }                                                       \
                        (bDrawCanvas, ##__VA_ARGS__);                           \
                    }) : 0;                                                     \
                 ({ARM_2D_OP_WAIT_ASYNC();})                                    \
                )
#else
#   define arm_2d_canvas(__tile_ptr, __region_name, ...)                        \
            __arm_2d_canvas((__tile_ptr, __region_name, ##__VA_ARGS__))
#endif

#if !__ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
#   define __ARM_2D_LAYOUT_DEBUG_BEGIN__(__bool_debug)
#   define __ARM_2D_LAYOUT_DEBUG_END__()
#else
#   define __ARM_2D_LAYOUT_DEBUG_BEGIN__(__bool_debug)                          \
            do {                                                                \
                if (__bool_debug) {                                             \
                    __layout_assistant__.ptDebug = &__arm_2d_reserve_canvas__;  \
                    __layout_assistant__.chLevel =                              \
                        __arm_2d_reserve_canvas__.wLevel++;                     \
                    __layout_assistant__.chInternalLevel = 0;                   \
                }                                                               \
            } while(0)

#   define __ARM_2D_LAYOUT_DEBUG_END__()                                        \
            do {                                                                \
                if (NULL != __layout_assistant__.ptDebug) {                     \
                    COLOUR_INT tColor = arm_2d_pixel_from_brga8888(             \
                                    __arm_2d_helper_colour_slider(              \
                                        __RGB32(0x40, 0x40, 0x00),              \
                                        __RGB32(0xFF, 0xFF, 0x00),              \
                                        4,                                      \
                                        __layout_assistant__.chLevel));         \
                    arm_2d_helper_draw_box(                                     \
                        (__layout_assistant__.ptDebug->ptTile),                 \
                        &__layout_assistant__.tArea,                            \
                        2,                                                      \
                        tColor,                                                 \
                        128);                                                   \
                }                                                               \
            } while(0)

#endif

/* this is an ugly fix for backward compatibility, this should be removed in 
 * the future.
 */
#define ARM_2D_LAYOUT_ALIGN_1                                                   \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN

/* this is an ugly fix for backward compatibility, this should be removed in 
 * the future.
 */
#define ARM_2D_LAYOUT_ALIGN_0                                                   \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN

#define ARM_2D_LAYOUT_ALIGN_DEFAULT                                             \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN

#define ARM_2D_LAYOUT_ALIGN_FORWARD                                             \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN
#define ARM_2D_LAYOUT_ALIGN_REVERSE                                             \
            ARM_2D_LAYOUT_ALIGN_RIGHT_TO_LEFT_BOTTOM_UP

#define ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT                                       \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN
#define ARM_2D_LAYOUT_ALIGN_TOP_TO_DOWN                                         \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN

#define ARM_2D_LAYOUT_ALIGN_RIGHT_TO_LEFT                                       \
            ARM_2D_LAYOUT_ALIGN_RIGHT_TO_LEFT_TOP_DOWN

#define ARM_2D_LAYOUT_ALIGN_BOTTOM_UP                                           \
            ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_BOTTOM_UP

#define arm_2d_layout1(__region)                                                \
        arm_using(  __arm_2d_layout_t __layout_assistant__ = {                  \
                            .tAlignTable                                        \
                                = ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN    \
                        },                                                      \
                    {                                                           \
                        __layout_assistant__.tLayout.tLocation                  \
                            = (__region).tLocation;                             \
                        __layout_assistant__.tArea = (__region);                \
                    },                                                          \
                    {                                                           \
                        ARM_2D_OP_WAIT_ASYNC();                                 \
                    }                                                           \
                )

#define arm_2d_layout2(__region, __bool_debug)                                  \
        arm_using(  __arm_2d_layout_t __layout_assistant__ = {                  \
                            .tAlignTable                                        \
                                = ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN    \
                        },                                                      \
                    {                                                           \
                        __layout_assistant__.tLayout.tLocation                  \
                            = (__region).tLocation;                             \
                        __layout_assistant__.tArea = (__region);                \
                        __ARM_2D_LAYOUT_DEBUG_BEGIN__(__bool_debug);            \
                    },                                                          \
                    {                                                           \
                        ARM_2D_OP_WAIT_ASYNC();                                 \
                        __ARM_2D_LAYOUT_DEBUG_END__();                          \
                    }                                                           \
                )

#define arm_2d_layout2(__region, __align)                                       \
            arm_using(  __arm_2d_layout_t __layout_assistant__ = {              \
                            .tAlignTable                                        \
                                = ARM_2D_LAYOUT_ALIGN_##__align                 \
                        },                                                      \
                        {                                                       \
                            __layout_assistant__.tLayout.tLocation              \
                                = (__region).tLocation;                         \
                            __layout_assistant__.tArea = (__region);            \
                                                                                \
                            __layout_assistant__.tLayout.tLocation.iX -=        \
                                __layout_assistant__.tArea.tSize.iWidth *       \
                                __layout_assistant__.tAlignTable                \
                                    .Horizontal.sWidth;                         \
                            __layout_assistant__.tLayout.tLocation.iY -=        \
                                __layout_assistant__.tArea.tSize.iHeight *      \
                                __layout_assistant__.tAlignTable                \
                                    .Vertical.sHeight;                          \
                        },                                                      \
                        {                                                       \
                            ARM_2D_OP_WAIT_ASYNC();                             \
                        }                                                       \
                    )

#define arm_2d_layout3(__region, __align, __bool_debug)                         \
            arm_using(  __arm_2d_layout_t __layout_assistant__ = {              \
                            .tAlignTable                                        \
                                = ARM_2D_LAYOUT_ALIGN_##__align                 \
                        },                                                      \
                        {                                                       \
                            __layout_assistant__.tLayout.tLocation              \
                                = (__region).tLocation;                         \
                            __layout_assistant__.tArea = (__region);            \
                                                                                \
                            __layout_assistant__.tLayout.tLocation.iX -=        \
                                __layout_assistant__.tArea.tSize.iWidth *       \
                                __layout_assistant__.tAlignTable                \
                                    .Horizontal.sWidth;                         \
                            __layout_assistant__.tLayout.tLocation.iY -=        \
                                __layout_assistant__.tArea.tSize.iHeight *      \
                                __layout_assistant__.tAlignTable                \
                                    .Vertical.sHeight;                          \
                            __ARM_2D_LAYOUT_DEBUG_BEGIN__(__bool_debug);        \
                        },                                                      \
                        {                                                       \
                            ARM_2D_OP_WAIT_ASYNC();                             \
                            __ARM_2D_LAYOUT_DEBUG_END__();                      \
                        }                                                       \
                    )


/*!
 * \brief prepare for layout
 * \param[in] __region the target region for layout
 * \param[in] __alignment the alignment requirement for layout, the valid mode is
 *            DEFAULT, FORWARD, REVERSE, LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_DOWN 
 *            and BOTTOM_UP
 * \param[in] __is_debug whether enable the layout debug mode
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          arm_2d_layout(__region) {
 *              ...
 *          }
 * 
 * \note prototype 2:
 *          arm_2d_layout(__region, __alignment) {
 *              ...
 *          }
 *
 * \note prototype 3:
 *          arm_2d_layout(__region, __alignment, __is_debug) {
 *              ...
 *          }
 */
#define arm_2d_layout(...)                                                      \
        ARM_CONNECT2(arm_2d_layout, __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


#if !__ARM_2D_HELPER_CFG_LAYOUT_DEBUG_MODE__
#   define __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__()
#   define __ARM_2D_LAYOUT_ITEM_DEBUG_END__()
#else
#   define __ARM_2D_LAYOUT_ITEM_DEBUG_END__()
#   define __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__()                                 \
                if (NULL != __layout_assistant__.ptDebug) {                     \
                    uint32_t chLevel = __layout_assistant__.chInternalLevel++ ; \
                    chLevel &= 0x7;                                             \
                    COLOUR_INT tColor = arm_2d_pixel_from_brga8888(             \
                                    __arm_2d_helper_colour_slider(              \
                                        __RGB32(0x00, 0x40, 0x00),              \
                                        __RGB32(0xFF, 0x80, 0x00),              \
                                        8,                                      \
                                        chLevel));                              \
                    arm_2d_fill_colour_with_opacity(                            \
                        (__layout_assistant__.ptDebug->ptTile),                 \
                        &__item_region,                                         \
                        (__arm_2d_color_t){tColor},                             \
                        200);                                                   \
                } while(0)
#endif
/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_horizontal1(__size)                                       \
            ____item_line_horizontal5(__size,0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_horizontal2(__width, __height)                            \
            ____item_line_horizontal6(__width, __height,0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_horizontal5(__size, __left, __right, __top, __bottom)     \
            ____item_line_horizontal6(  (__size).iWidth, (__size).iHeight,      \
                                        (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_horizontal6(  __width, __height,                          \
                                    __left, __right, __top, __bottom)           \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                    __layout_assistant__.tLayout.tLocation.iX                   \
                        += ((__width) + (__left) + (__right))                   \
                         * __layout_assistant__.tAlignTable.Horizontal.sAdvance;\
                                                                                \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __layout_assistant__.tLayout.tSize.iHeight = MAX(           \
                            __layout_assistant__.tLayout.tSize.iHeight,         \
                            ARM_2D_SAFE_NAME(iHeight));                         \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __layout_assistant__.tLayout.tSize.iWidth = MAX(            \
                                    __layout_assistant__.tLayout.tSize.iWidth,  \
                                    ARM_2D_SAFE_NAME(iWidth));                  \
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
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
 *                               __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_line_horizontal(...)                                             \
            ARM_CONNECT2(   ____item_line_horizontal,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_horizontal5( __width,                                \
                                        __left, __right, __top, __bottom)       \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth = MAX((__width), 0);             \
                    __item_region.tSize.iHeight                                 \
                        = (__layout_assistant__.tArea.tSize.iHeight)            \
                        - (__top) - (__bottom);                                 \
                    __item_region.tSize.iHeight                                 \
                        = MAX(0, __item_region.tSize.iHeight);                  \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                    __layout_assistant__.tLayout.tSize.iHeight =                \
                        MAX(__layout_assistant__.tArea.tSize.iHeight,           \
                            __layout_assistant__.tLayout.tSize.iHeight);        \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                                = MAX((__width), 0) + (__left) + (__right);     \
                    __layout_assistant__.tLayout.tSize.iWidth = MAX(            \
                                    __layout_assistant__.tLayout.tSize.iWidth,  \
                                    ARM_2D_SAFE_NAME(iWidth));                  \
                    __layout_assistant__.tLayout.tLocation.iX                   \
                        += ARM_2D_SAFE_NAME(iWidth)                             \
                         * __layout_assistant__.tAlignTable.Horizontal.sAdvance;\
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
                })

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_horizontal4(__left, __right, __top, __bottom)        \
            ____item_line_dock_horizontal5(                                     \
                    ( __layout_assistant__.tArea.tLocation.iX                   \
                    + __layout_assistant__.tArea.tSize.iWidth                   \
                    * (__layout_assistant__.tAlignTable.Horizontal.sWidth + 1)  \
                    - __layout_assistant__.tLayout.tLocation.iX                 \
                    - (__left) - (__right)),                                    \
                    (__left), (__right), (__top), (__bottom))


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_horizontal1(__width)                                 \
            ____item_line_dock_horizontal5(__width, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_horizontal0()                                        \
            ____item_line_dock_horizontal5(                                     \
                    ( __layout_assistant__.tArea.tLocation.iX                   \
                    + __layout_assistant__.tArea.tSize.iWidth                   \
                    * (__layout_assistant__.tAlignTable.Horizontal.sWidth + 1)  \
                    - __layout_assistant__.tLayout.tLocation.iX),               \
                    0, 0, 0, 0)

/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to dock a specific 
 *        rectangular area horizontally.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_line_dock_horizontal() {
 *              code body that can use __item_region
 *          }
 * 
 * \note prototype 2:
 *          __item_line_dock_horizontal(__width) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 3:
 *          __item_line_dock_horizontal(__width, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 */
#define __item_line_dock_horizontal(...)                                        \
            ARM_CONNECT2(   ____item_line_dock_horizontal,                      \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)



/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_vertical1(__size)                                         \
            ____item_line_vertical5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_vertical2(__width, __height)                              \
            ____item_line_vertical6(__width, __height, 0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_vertical5(__size, __left, __right, __top, __bottom)       \
            ____item_line_vertical6((__size).iWidth, (__size).iHeight,          \
                                    (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_vertical6(__width, __height,                              \
                                __left, __right, __top, __bottom)               \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                                                                                \
                    __layout_assistant__.tLayout.tLocation.iY                   \
                        += ARM_2D_SAFE_NAME(iHeight)                            \
                         * __layout_assistant__.tAlignTable.Vertical.sAdvance;  \
                                                                                \
                    __layout_assistant__.tLayout.tSize.iHeight = MAX(           \
                                __layout_assistant__.tLayout.tSize.iHeight,     \
                                ARM_2D_SAFE_NAME(iHeight));                     \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __layout_assistant__.tLayout.tSize.iWidth = MAX(            \
                                    __layout_assistant__.tLayout.tSize.iWidth,  \
                                    ARM_2D_SAFE_NAME(iWidth));                  \
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_vertical5(   __height,                               \
                                        __left, __right, __top, __bottom)       \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    __item_region.tSize.iWidth                                  \
                        = (__layout_assistant__.tArea.tSize.iWidth)             \
                        - (__left) - (__right);                                 \
                    __item_region.tSize.iWidth                                  \
                        = MAX(0, __item_region.tSize.iWidth);                   \
                    __item_region.tSize.iHeight = MAX((__height), 0);           \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                                =  MAX((__height), 0) + (__top) + (__bottom);   \
                                                                                \
                    __layout_assistant__.tLayout.tLocation.iY                   \
                        += ARM_2D_SAFE_NAME(iHeight)                            \
                         * __layout_assistant__.tAlignTable.Vertical.sAdvance;  \
                                                                                \
                    __layout_assistant__.tLayout.tSize.iHeight = MAX(           \
                                    __layout_assistant__.tLayout.tSize.iHeight, \
                                    ARM_2D_SAFE_NAME(iHeight));                 \
                                                                                \
                    __layout_assistant__.tLayout.tSize.iWidth =                 \
                        MAX(__layout_assistant__.tArea.tSize.iWidth,            \
                            __layout_assistant__.tLayout.tSize.iWidth);         \
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
                })

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_vertical4(__left, __right, __top, __bottom)          \
            ____item_line_dock_vertical5(                                       \
                    ( __layout_assistant__.tArea.tLocation.iY                   \
                    + __layout_assistant__.tArea.tSize.iHeight                  \
                    * (__layout_assistant__.tAlignTable.Vertical.sHeight + 1)   \
                    - __layout_assistant__.tLayout.tLocation.iY                 \
                    - (__top) - (__bottom)),                                    \
                    (__left), (__right), (__top), (__bottom))


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_vertical1(__height)                                  \
            ____item_line_dock_vertical5(__height, 0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_line_dock_vertical0()                                          \
            ____item_line_dock_vertical5(                                       \
                    ( __layout_assistant__.tArea.tLocation.iY                   \
                    + __layout_assistant__.tArea.tSize.iHeight                  \
                    * (__layout_assistant__.tAlignTable.Vertical.sHeight + 1)   \
                    - __layout_assistant__.tLayout.tLocation.iY),               \
                    0, 0, 0, 0)

/*!
 * \brief generate a arm_2d_region (i.e. __item_region) to dock a specific 
 *        rectangular area vertically.
 * \param ... parameter list 
 * 
 * \note prototype 1:
 *          __item_line_dock_vertical(__size) {
 *              code body that can use __item_region
 *          }
 *
 * \note prototype 2:
 *          __item_line_dock_vertical(__size, __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 *
 */
#define __item_line_dock_vertical(...)                                          \
            ARM_CONNECT2(   ____item_line_dock_vertical,                        \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_horizontal1(__size, __height)                                  \
            ____item_horizontal5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_horizontal2(__width, __height)                                 \
            ____item_horizontal6(  __width, __height, 0, 0, 0, 0)



/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_horizontal5( __size, __left, __right, __top, __bottom)         \
            ____item_horizontal6(   (__size).iWidth, (__size).iHeight,          \
                                    (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_horizontal6(  __width, __height,                               \
                                __left, __right, __top, __bottom)               \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    int16_t iTempX = __layout_assistant__.tLayout.tLocation.iX  \
                                   + (__left) + (__width);                      \
                    /* is end of the line */                                    \
                    __arm_2d_layout_wrap_horizontal(iTempX,                     \
                                                    &__layout_assistant__);     \
                                                                                \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __layout_assistant__.tLayout.tSize.iHeight = MAX(           \
                                    __layout_assistant__.tLayout.tSize.iHeight, \
                                    ARM_2D_SAFE_NAME(iHeight));                 \
                                                                                \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __layout_assistant__.tLayout.tLocation.iX                   \
                        += ARM_2D_SAFE_NAME(iWidth)                             \
                         * __layout_assistant__.tAlignTable.Horizontal.sAdvance;\
                                                                                \
                    __layout_assistant__.tLayout.tSize.iWidth = MAX(            \
                                    __layout_assistant__.tLayout.tSize.iWidth,  \
                                    ARM_2D_SAFE_NAME(iWidth));                  \
                                                                                \
                    /* is end of the line */                                    \
                    __arm_2d_layout_wrap_horizontal(                            \
                                    __layout_assistant__.tLayout.tLocation.iX,  \
                                    &__layout_assistant__);                     \
                                                                                \
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_vertical1(__size, __height)                                    \
            ____item_vertical5(__size, 0, 0, 0, 0)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_vertical2(__width, __height)                                   \
            ____item_vertical6(  __width, __height, 0, 0, 0, 0)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_vertical5( __size, __left, __right, __top, __bottom)           \
            ____item_vertical6((__size).iWidth, (__size).iHeight,               \
                               (__left), (__right), (__top), (__bottom))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define ____item_vertical6( __width, __height,                                  \
                            __left, __right, __top, __bottom)                   \
    arm_using(  arm_2d_region_t __item_region,                                  \
                {                                                               \
                    int16_t iTempY = __layout_assistant__.tLayout.tLocation.iY  \
                                   + (__top) + (__bottom);                      \
                    /* is end of the line */                                    \
                    __arm_2d_layout_wrap_vertical(iTempY,                       \
                                                    &__layout_assistant__);     \
                                                                                \
                    __item_region.tSize.iWidth = (__width);                     \
                    __item_region.tSize.iHeight = (__height);                   \
                    __item_region.tLocation =                                   \
                        __layout_assistant__.tLayout.tLocation;                 \
                                                                                \
                    __item_region.tLocation.iX                                  \
                        +=  (__left)                                            \
                        *   __layout_assistant__.tAlignTable.Horizontal.sLeft;  \
                    __item_region.tLocation.iX                                  \
                        +=  (__right)                                           \
                        *   __layout_assistant__.tAlignTable.Horizontal.sRight; \
                    __item_region.tLocation.iX                                  \
                        +=  (__item_region.tSize.iWidth)                        \
                        *   __layout_assistant__.tAlignTable.Horizontal.sWidth; \
                    __item_region.tLocation.iY                                  \
                        +=  (__top)                                             \
                        *   __layout_assistant__.tAlignTable.Vertical.sTop;     \
                    __item_region.tLocation.iY                                  \
                        +=  (__bottom)                                          \
                        *   __layout_assistant__.tAlignTable.Vertical.sBottom;  \
                    __item_region.tLocation.iY                                  \
                        +=  (__item_region.tSize.iHeight)                       \
                        *   __layout_assistant__.tAlignTable.Vertical.sHeight;  \
                                                                                \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_BEGIN__();                       \
                },                                                              \
                {                                                               \
                                                                                \
                    int16_t ARM_2D_SAFE_NAME(iHeight)                           \
                        = (__height) + (__top) + (__bottom);                    \
                    __layout_assistant__.tLayout.tLocation.iY                   \
                        += ARM_2D_SAFE_NAME(iHeight)                            \
                         * __layout_assistant__.tAlignTable.Vertical.sAdvance;  \
                                                                                \
                    __layout_assistant__.tLayout.tSize.iHeight = MAX(           \
                                    __layout_assistant__.tLayout.tSize.iHeight, \
                                    ARM_2D_SAFE_NAME(iHeight));                 \
                    int16_t ARM_2D_SAFE_NAME(iWidth)                            \
                        = (__width) + (__left) + (__right);                     \
                    __layout_assistant__.tLayout.tSize.iWidth = MAX(            \
                                    __layout_assistant__.tLayout.tSize.iWidth,  \
                                    ARM_2D_SAFE_NAME(iWidth));                  \
                                                                                \
                    /* is end of the line */                                    \
                    __arm_2d_layout_wrap_vertical(                              \
                                    __layout_assistant__.tLayout.tLocation.iY,  \
                                    &__layout_assistant__);                     \
                                                                                \
                    ARM_2D_OP_WAIT_ASYNC();                                     \
                    __ARM_2D_LAYOUT_ITEM_DEBUG_END__();                         \
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
 *                          __left, __right, __top, __bottom) {
 *              code body that can use __item_region
 *          }
 */
#define __item_vertical(...)                                                    \
            ARM_CONNECT2(   ____item_vertical,                                  \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_left2(__region, __size)                              \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_left_region,                              \
                {                                                               \
                    __top_left_region.tSize.iWidth = (__size).iWidth;           \
                    __top_left_region.tSize.iHeight = (__size).iHeight;         \
                    __top_left_region.tLocation                                 \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_left_region,                              \
                {                                                               \
                    __top_left_region.tSize.iWidth = (__width);                 \
                    __top_left_region.tSize.iHeight = (__height);               \
                    __top_left_region.tLocation                                 \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        top-left alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_centre2(__region, __size)                            \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_centre_region,                            \
                {                                                               \
                    __top_centre_region.tSize.iWidth = (__size).iWidth;         \
                    __top_centre_region.tSize.iHeight = (__size).iHeight;       \
                    __top_centre_region.tLocation                               \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __top_centre_region.tLocation.iX                            \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__size).iWidth) >> 1;                          \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_centre3(__region, __width, __height)                 \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_centre_region,                            \
                {                                                               \
                    __top_centre_region.tSize.iWidth = (__width);               \
                    __top_centre_region.tSize.iHeight = (__height);             \
                    __top_centre_region.tLocation                               \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __top_centre_region.tLocation.iX                            \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__width)) >> 1;                                \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        top-central alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_right2(__region, __size)                             \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_right_region,                             \
                {                                                               \
                    __top_right_region.tSize.iWidth = (__size).iWidth;          \
                    __top_right_region.tSize.iHeight = (__size).iHeight;        \
                    __top_right_region.tLocation                                \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __top_right_region.tLocation.iX                             \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__size).iWidth);                               \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_top_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                    arm_2d_region_t __top_right_region,                         \
                    {                                                           \
                        __top_right_region.tSize.iWidth = (__width);            \
                        __top_right_region.tSize.iHeight = (__height);          \
                        __top_right_region.tLocation                            \
                            = ARM_2D_SAFE_NAME(tTempRegion).tLocation;          \
                        __top_right_region.tLocation.iX                         \
                            += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth  \
                               -    (__width));                                 \
                    },                                                          \
                    {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        top-right alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_mid_left2(__region, __size)                              \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __mid_left_region,                              \
                {                                                               \
                    __mid_left_region.tSize.iWidth = (__size).iWidth;           \
                    __mid_left_region.tSize.iHeight = (__size).iHeight;         \
                    __mid_left_region.tLocation                                 \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __mid_left_region.tLocation.iY                              \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__size).iHeight) >> 1;                         \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_mid_left3(__region, __width, __height)                   \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                    arm_2d_region_t __mid_left_region,                          \
                    {                                                           \
                        __mid_left_region.tSize.iWidth = (__width);             \
                        __mid_left_region.tSize.iHeight = (__height);           \
                        __mid_left_region.tLocation                             \
                            = ARM_2D_SAFE_NAME(tTempRegion).tLocation;          \
                        __mid_left_region.tLocation.iY                          \
                            += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight \
                               -    (__height)) >> 1;                           \
                    },                                                          \
                    {ARM_2D_OP_WAIT_ASYNC();})

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
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        left alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_centre2(__region, __size)                                \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __centre_region,                                \
                {                                                               \
                    arm_2d_size_t ARM_2D_SAFE_NAME(tSize) = (__size);           \
                    __centre_region.tSize.iWidth                                \
                        = ARM_2D_SAFE_NAME(tSize).iWidth;                       \
                    __centre_region.tSize.iHeight                               \
                        = ARM_2D_SAFE_NAME(tSize).iHeight;                      \
                    __centre_region.tLocation                                   \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __centre_region.tLocation.iX                                \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    ARM_2D_SAFE_NAME(tSize).iWidth)  >> 1;          \
                    __centre_region.tLocation.iY                                \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    ARM_2D_SAFE_NAME(tSize).iHeight)>> 1;           \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_centre3(__region, __width, __height)                     \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __centre_region,                                \
                    {                                                           \
                        __centre_region.tSize.iWidth = (__width);               \
                        __centre_region.tSize.iHeight = (__height);             \
                        __centre_region.tLocation                               \
                            = ARM_2D_SAFE_NAME(tTempRegion).tLocation;          \
                        __centre_region.tLocation.iX                            \
                            += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth  \
                               -    (__width))  >> 1;                           \
                        __centre_region.tLocation.iY                            \
                            += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight \
                               -    (__height))>> 1;                            \
                    },                                                          \
                    {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        central alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_mid_right2(__region, __size)                             \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__size).iWidth;          \
                    __mid_right_region.tSize.iHeight = (__size).iHeight;        \
                    __mid_right_region.tLocation                                \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __mid_right_region.tLocation.iX                             \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__size).iWidth);                               \
                    __mid_right_region.tLocation.iY                             \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__size).iHeight) >> 1;                         \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_mid_right3(__region, __width, __height)                  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __mid_right_region,                             \
                {                                                               \
                    __mid_right_region.tSize.iWidth = (__width);                \
                    __mid_right_region.tSize.iHeight = (__height);              \
                    __mid_right_region.tLocation                                \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __mid_right_region.tLocation.iX                             \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__width));                                     \
                    __mid_right_region.tLocation.iY                             \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__height)) >> 1;                               \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

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
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        right alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_left2(__region, __size)                           \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__size).iWidth;        \
                    __bottom_left_region.tSize.iHeight = (__size).iHeight;      \
                    __bottom_left_region.tLocation                              \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_left_region.tLocation.iY                           \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__size).iHeight);                              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_left3(__region, __width, __height)                \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_left_region,                           \
                {                                                               \
                    __bottom_left_region.tSize.iWidth = (__width);              \
                    __bottom_left_region.tSize.iHeight = (__height);            \
                    __bottom_left_region.tLocation                              \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_left_region.tLocation.iY                           \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__height));                                    \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        bottom-left alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_centre2(__region, __size)                         \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__size).iWidth;      \
                    __bottom_centre_region.tSize.iHeight = (__size).iHeight;    \
                    __bottom_centre_region.tLocation                            \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_centre_region.tLocation.iX                         \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__size).iWidth)  >> 1;                         \
                    __bottom_centre_region.tLocation.iY                         \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__size).iHeight);                              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_centre3(__region, __width, __height)              \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_centre_region,                         \
                {                                                               \
                    __bottom_centre_region.tSize.iWidth = (__width);            \
                    __bottom_centre_region.tSize.iHeight = (__height);          \
                    __bottom_centre_region.tLocation                            \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_centre_region.tLocation.iX                         \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__width))  >> 1;                               \
                    __bottom_centre_region.tLocation.iY                         \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__height));                                    \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        bottom-central alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_right2(__region, __size)                          \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__size).iWidth;       \
                    __bottom_right_region.tSize.iHeight = (__size).iHeight;     \
                    __bottom_right_region.tLocation                             \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_right_region.tLocation.iX                          \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__size).iWidth);                               \
                    __bottom_right_region.tLocation.iY                          \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__size).iHeight);                              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_align_bottom_right3(__region, __width, __height)               \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_right_region,                          \
                {                                                               \
                    __bottom_right_region.tSize.iWidth = (__width);             \
                    __bottom_right_region.tSize.iHeight = (__height);           \
                    __bottom_right_region.tLocation                             \
                        = ARM_2D_SAFE_NAME(tTempRegion).tLocation;              \
                    __bottom_right_region.tLocation.iX                          \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth      \
                           -    (__width));                                     \
                    __bottom_right_region.tLocation.iY                          \
                        += (    ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight     \
                           -    (__height));                                    \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object with user specified info for
 *        bottom-right alignment. 
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
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_top0(__region, __height)                                  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_region = ARM_2D_SAFE_NAME(tTempRegion),   \
                {                                                               \
                    __top_region.tSize.iHeight = (__height);                    \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_top2(__region, __height, __margin_left, __margin_right)   \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __top_region = ARM_2D_SAFE_NAME(tTempRegion),   \
                {                                                               \
                    __top_region.tSize.iHeight = (__height);                    \
                    __top_region.tSize.iWidth -= (__margin_left)                \
                                               + (__margin_right);              \
                    __top_region.tLocation.iX += (__margin_left);               \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_top1(__region, __height, __margin)                        \
            __arm_2d_dock_top2((__region), (__height), (__margin), (__margin))

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the top with 
 *        an user specified height. 
 * 
 * \note prototype1:
 *          arm_2d_dock_top(__region, __height) {
 *              code body that can use __top_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_top(__region, __height, __margin) {
 *              code body that can use __top_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_top(__region, __height, __margin_left, __margin_right) {
 *              code body that can use __top_region
 *          }
 */
#define arm_2d_dock_top(__region, __height, ...)                                \
            ARM_CONNECT2(   __arm_2d_dock_top,                                  \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__height), ##__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_bottom2(__region, __height, __margin_left, __margin_right)\
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_region = ARM_2D_SAFE_NAME(tTempRegion),\
                {                                                               \
                    __bottom_region.tSize.iHeight = (__height);                 \
                    __bottom_region.tLocation.iY                                \
                        += ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight          \
                         - (__height);                                          \
                    __bottom_region.tSize.iWidth -= (__margin_left)             \
                                                  + (__margin_right);           \
                    __bottom_region.tLocation.iX += (__margin_left);            \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_bottom1(__region, __height, __margin)                     \
            __arm_2d_dock_bottom2((__region), (__height), (__margin), (__margin))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_bottom0(__region, __height)                               \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __bottom_region = ARM_2D_SAFE_NAME(tTempRegion),\
                {                                                               \
                    __bottom_region.tSize.iHeight = (__height);                 \
                    __bottom_region.tLocation.iY                                \
                        += ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight          \
                         - (__height);                                          \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the bottom
 *        with an user specified height. 
 * 
 * \note prototype1:
 *          arm_2d_dock_bottom(__region, __height) {
 *              code body that can use __bottom_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_bottom(__region, __height, __margin) {
 *              code body that can use __bottom_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_bottom(__region, __height, __margin_left, __margin_right) {
 *              code body that can use __bottom_region
 *          }          
 */
#define arm_2d_dock_bottom(__region, __height, ...)                             \
            ARM_CONNECT2(   __arm_2d_dock_bottom,                               \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__height), ##__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_left2(__region, __width, __margin_top, __margin_bottom)   \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __left_region = ARM_2D_SAFE_NAME(tTempRegion),  \
                {                                                               \
                    __left_region.tSize.iWidth = (__width);                     \
                    __left_region.tSize.iHeight -= (__margin_top)               \
                                                 + (__margin_bottom);           \
                    __left_region.tLocation.iY += (__margin_top);               \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_left1(__region, __width, __margin)                        \
            __arm_2d_dock_left2((__region), (__width), (__margin), (__margin))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_left0(__region, __width)                                  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __left_region = ARM_2D_SAFE_NAME(tTempRegion),  \
                {                                                               \
                    __left_region.tSize.iWidth = (__width);                     \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the left with 
 *        an user specified height. 
 * 
 * \note prototype1:
 *          arm_2d_dock_left(__region, __width) {
 *              code body that can use __left_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_left(__region, __width, __margin) {
 *              code body that can use __left_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_left(__region, __width, __margin_top, __margin_bottom) {
 *              code body that can use __left_region
 *          }            
 */
#define arm_2d_dock_left(__region, __width, ...)                                \
            ARM_CONNECT2(   __arm_2d_dock_left,                                 \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__width), ##__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_right2(__region, __width, __margin_top, __margin_bottom)  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __right_region = ARM_2D_SAFE_NAME(tTempRegion), \
                {                                                               \
                    __right_region.tSize.iWidth = (__width);                    \
                    __right_region.tLocation.iX                                 \
                        += ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth           \
                         - (__width);                                           \
                    __right_region.tSize.iHeight -= (__margin_top)              \
                                                 + (__margin_bottom);           \
                    __right_region.tLocation.iY += (__margin_top);              \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_right1(__region, __width, __margin)                       \
            __arm_2d_dock_right2((__region), (__width), (__margin), (__margin))
/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_right0(__region, __width)                                 \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __right_region = ARM_2D_SAFE_NAME(tTempRegion), \
                {                                                               \
                    __right_region.tSize.iWidth = (__width);                    \
                    __right_region.tLocation.iX                                 \
                        += ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth           \
                         - (__width);                                           \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})


/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the right
 *        with an user specified height. 
 * 
 * \note prototype1:
 *          arm_2d_dock_right(__region, __width) {
 *              code body that can use __right_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_right(__region, __width, __margin) {
 *              code body that can use __right_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_right(__region, __width, __margin_top, __margin_bottom) {
 *              code body that can use __right_region
 *          }           
 */
#define arm_2d_dock_right(__region, __width, ...)                               \
            ARM_CONNECT2(   __arm_2d_dock_right,                                \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__width), ##__VA_ARGS__)

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_vertical2(__region,                                       \
                                __height,                                       \
                                __margin_left,                                  \
                                __margin_right)                                 \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __vertical_region                               \
                    = ARM_2D_SAFE_NAME(tTempRegion),                            \
                {                                                               \
                    __vertical_region.tSize.iHeight = (__height);               \
                    __vertical_region.tLocation.iY                              \
                        += (ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight         \
                         - (__height))>> 1;                                     \
                    __vertical_region.tSize.iWidth -= (__margin_left)           \
                                                    + (__margin_right);         \
                    __vertical_region.tLocation.iX += (__margin_left);          \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_vertical1(__region, __height, __margin)                   \
            __arm_2d_dock_vertical2((__region),                                 \
                                    (__height),                                 \
                                    (__margin),                                 \
                                    (__margin))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_vertical0(__region, __height)                             \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __vertical_region                               \
                    = ARM_2D_SAFE_NAME(tTempRegion),                            \
                {                                                               \
                    __vertical_region.tSize.iHeight = (__height);               \
                    __vertical_region.tLocation.iY                              \
                        += (ARM_2D_SAFE_NAME(tTempRegion).tSize.iHeight         \
                         - (__height))>> 1;                                     \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the region
 *        centre vertically with an user specified __height. 
 * 
 * \note prototype1:
 *          arm_2d_dock_vertical(__region, __height) {
 *              code body that can use __vertical_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_vertical(__region, __width, __margin) {
 *              code body that can use __vertical_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_vertical(__region, __width, __margin_left, __margin_right) {
 *              code body that can use __vertical_region
 *          }          
 */
#define arm_2d_dock_vertical(__region, __height, ...)                           \
            ARM_CONNECT2(   __arm_2d_dock_vertical,                             \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__height), ##__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_horizontal2(  __region,                                   \
                                    __width,                                    \
                                    __margin_top,                               \
                                    __margin_bottom)                            \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __horizontal_region                             \
                                    = ARM_2D_SAFE_NAME(tTempRegion),            \
                {                                                               \
                    __horizontal_region.tSize.iWidth = (__width);               \
                    __horizontal_region.tLocation.iX                            \
                        += (ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth          \
                         - (__width))  >> 1;                                    \
                    __horizontal_region.tSize.iHeight -= (__margin_top)         \
                                                       + (__margin_bottom);     \
                    __horizontal_region.tLocation.iY += (__margin_top);         \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_horizontal1(__region, __width, __margin)                  \
            __arm_2d_dock_horizontal2(  (__region),                             \
                                        (__width),                              \
                                        (__margin_top),                         \
                                        (__margin_bottom))                      \
/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_horizontal0(__region, __width)                            \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __horizontal_region                             \
                                    = ARM_2D_SAFE_NAME(tTempRegion),            \
                {                                                               \
                    __horizontal_region.tSize.iWidth = (__width);               \
                    __horizontal_region.tLocation.iX                            \
                        += (ARM_2D_SAFE_NAME(tTempRegion).tSize.iWidth          \
                         - (__width))  >> 1;                                    \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})


/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the region
 *        centre horizontally with an user specified __width. 
 * 
 * \note prototype1:
 *          arm_2d_dock_horizontal(__region, __width) {
 *              code body that can use __horizontal_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_horizontal(__region, __width, __margin) {
 *              code body that can use __horizontal_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_horizontal(__region, __width, __margin_top, __margin_bottom) {
 *              code body that can use __horizontal_region
 *          }   
 */
#define arm_2d_dock_horizontal(__region, __width, ...)                          \
            ARM_CONNECT2(   __arm_2d_dock_horizontal,                           \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), (__width), ##__VA_ARGS__)


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_with_margin4(__region, __left, __right, __top, __bottom)  \
            arm_using(                                                          \
                arm_2d_region_t ARM_2D_SAFE_NAME(tTempRegion) = (__region))     \
            arm_using(                                                          \
                arm_2d_region_t __dock_region = ARM_2D_SAFE_NAME(tTempRegion),  \
                {                                                               \
                    __dock_region.tSize.iWidth -= (__left) + (__right);         \
                    __dock_region.tLocation.iX += (__left);                     \
                    __dock_region.tSize.iHeight -= (__top) + (__bottom);        \
                    __dock_region.tLocation.iY += (__top);                      \
                },                                                              \
                {ARM_2D_OP_WAIT_ASYNC();})


/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_with_margin1(__region, __margin)                          \
            __arm_2d_dock_with_margin4( (__region),                             \
                                        (__margin),                             \
                                        (__margin),                             \
                                        (__margin),                             \
                                        (__margin))

/*!
 * \brief Please do NOT use this macro directly
 * 
 */
#define __arm_2d_dock_with_margin0(__region)                                    \
            __arm_2d_dock_with_margin4( (__region), 0 ,0, 0, 0)

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the target
 *        region with given margins
 * 
 * \note prototype1:
 *          arm_2d_dock_with_margin(__region) {
 *              code body that can use __dock_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock_with_margin(__region, __margin) {
 *              code body that can use __dock_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock_with_margin(__region, __left, __right, __top, __bottom) {
 *              code body that can use __dock_region
 *          }   
 */
#define arm_2d_dock_with_margin(__region, ...)                                  \
            ARM_CONNECT2(   __arm_2d_dock_with_margin,                          \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))                     \
                                ((__region), ##__VA_ARGS__)

/*!
 * \brief generate a temporary arm_2d_region_t object that docks to the target
 *        region with given margins
 * 
 * \note prototype1:
 *          arm_2d_dock(__region) {
 *              code body that can use __dock_region
 *          }
 * 
 * \note prototype2:
 *          arm_2d_dock(__region, __margin) {
 *              code body that can use __dock_region
 *          }
 *
 * \note prototype3:
 *          arm_2d_dock(__region, __left, __right, __top, __bottom) {
 *              code body that can use __dock_region
 *          }   
 */
#define arm_2d_dock(__region, ...)                                              \
            arm_2d_dock_with_margin(__region, ##__VA_ARGS__)

/*!
 * \brief a template for implement a on draw event handler
 */
#define IMPL_ON_DRAW_EVT(__NAME)                                                \
            arm_fsm_rt_t __NAME(void *pTarget,                                  \
                                const arm_2d_tile_t *ptTile,                    \
                                bool bIsNewFrame)

/*============================ TYPES =========================================*/

typedef struct __arm_2d_layout_debug_t {
    const arm_2d_tile_t *ptTile;
    uint32_t wLevel;
} __arm_2d_layout_debug_t;

typedef struct __arm_2d_layout_align_tab_t {
    union {
        struct {
            int8_t sWidth   : 2;
            int8_t sLeft    : 2;
            int8_t sRight   : 2;
            int8_t sAdvance : 2;
        };
        uint8_t chScheme;
    } Horizontal;

    union {
        struct {
            int8_t sHeight  : 2;
            int8_t sTop     : 2;
            int8_t sBottom  : 2;
            int8_t sAdvance : 2;
        };
        uint8_t chScheme;
    } Vertical;
} __arm_2d_layout_align_tab_t;

typedef struct __arm_2d_layout_t {
    __arm_2d_layout_debug_t *ptDebug;
    arm_2d_region_t tLayout;
    arm_2d_region_t tArea;
    uint8_t chLevel;
    uint8_t chInternalLevel;
    const __arm_2d_layout_align_tab_t tAlignTable;
} __arm_2d_layout_t;

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

static
const __arm_2d_layout_align_tab_t ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_TOP_DOWN = {
    .Horizontal = {
        .sWidth = 0,
        .sLeft = 1,
        .sRight = 0,
        .sAdvance = 1,
    },
    .Vertical = {
        .sHeight = 0,
        .sTop = 1,
        .sBottom = 0,
        .sAdvance = 1,
    },
};

static
const __arm_2d_layout_align_tab_t ARM_2D_LAYOUT_ALIGN_RIGHT_TO_LEFT_TOP_DOWN = {
    .Horizontal = {
        .sWidth = -1,
        .sLeft = 0,
        .sRight = -1,
        .sAdvance = -1,
    },
    .Vertical = {
        .sHeight = 0,
        .sTop = 1,
        .sBottom = 0,
        .sAdvance = 1,
    },
};

static
const __arm_2d_layout_align_tab_t ARM_2D_LAYOUT_ALIGN_LEFT_TO_RIGHT_BOTTOM_UP = {
    .Horizontal = {
        .sWidth = 0,
        .sLeft = 1,
        .sRight = 0,
        .sAdvance = 1,
    },
    .Vertical = {
        .sHeight = -1,
        .sTop = 0,
        .sBottom = -1,
        .sAdvance = -1,
    },
};

static
const __arm_2d_layout_align_tab_t ARM_2D_LAYOUT_ALIGN_RIGHT_TO_LEFT_BOTTOM_UP = {
    .Horizontal = {
        .sWidth = -1,
        .sLeft = 0,
        .sRight = -1,
        .sAdvance = -1,
    },
    .Vertical = {
        .sHeight = -1,
        .sTop = 0,
        .sBottom = -1,
        .sAdvance = -1,
    },
};


/*============================ PROTOTYPES ====================================*/

__STATIC_INLINE
uint8_t arm_2d_helper_alpha_mix(uint_fast8_t chAlpha1, 
                                uint_fast8_t chAlpha2)
{
    chAlpha1 = MIN(255, chAlpha1);
    chAlpha2 = MIN(255, chAlpha2);
    return (uint8_t)(   (chAlpha1 == 255) 
                    ?   chAlpha2 
                    :   (   (chAlpha2 == 255) 
                        ?   chAlpha1 
                        :   ((uint16_t)chAlpha1 * (uint16_t)chAlpha2 >> 8)));
}

extern
ARM_NONNULL(2)
void __arm_2d_layout_wrap_horizontal(   int16_t iTempX, 
                                        __arm_2d_layout_t *ptLayout);

extern
ARM_NONNULL(2)
void __arm_2d_layout_wrap_vertical( int16_t iTempX, 
                                    __arm_2d_layout_t *ptLayout);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
