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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper_shape.h"
 * Description:  the helper service header file for drawing simple shapes
 *
 * $Date:        04. Dec 2024
 * $Revision:    V.1.13.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_HELPER_SHAPE_H__
#define __ARM_2D_HELPER_SHAPE_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#   define COLOUR_INT               uint8_t
#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#   define COLOUR_INT               uint16_t
#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#   define COLOUR_INT               uint32_t
#else
#   error Unsupported colour depth!
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/



#define __draw_round_corner_box2(   __target_tile_ptr,                          \
                                    __region_ptr,                               \
                                    __colour,                                   \
                                    __opacity,                                  \
                                    __is_new_frame,                             \
                                    __circle_mask_ptr)                          \
    __draw_round_corner_box(                                                    \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__colour),                                                     \
                (__opacity),                                                    \
                ((const arm_2d_tile_t *)__circle_mask_ptr))

#define __draw_round_corner_box1(  __target_tile_ptr,                           \
                                    __region_ptr,                               \
                                    __colour,                                   \
                                    __opacity,                                  \
                                    __is_new_frame)                             \
    __draw_round_corner_box2(                                                   \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__colour),                                                     \
                (__opacity),                                                    \
                (__is_new_frame),                                               \
                ((const arm_2d_tile_t *)&c_tileWhiteDotMask))

#define __draw_round_corner_box0(   __target_tile_ptr,                          \
                                    __region_ptr,                               \
                                    __colour,                                   \
                                    __opacity)                                  \
    __draw_round_corner_box2(                                                   \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__colour),                                                     \
                (__opacity),                                                    \
                (true),                                                         \
                ((const arm_2d_tile_t *)&c_tileWhiteDotMask))

#define draw_round_corner_box(  __target_tile_ptr,                              \
                                __region_ptr,                                   \
                                __colour,                                       \
                                __opacity,                                      \
                                ...)                                            \
            ARM_CONNECT2(   __draw_round_corner_box,                            \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(                    \
                                                (__target_tile_ptr),            \
                                                (__region_ptr),                 \
                                                (__colour),                     \
                                                (__opacity), ##__VA_ARGS__)

#define __draw_round_corner_image2( __source_tile_ptr,                          \
                                    __target_tile_ptr,                          \
                                    __region_ptr,                               \
                                    __is_new_frame,                             \
                                    __opacity,                                  \
                                    __circle_mask_addr)                         \
    __draw_round_corner_image(                                                  \
                (__source_tile_ptr),                                            \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__is_new_frame),                                               \
                (__opacity),                                                    \
                ((const arm_2d_tile_t *)(__circle_mask_addr)))

#define __draw_round_corner_image1( __source_tile_ptr,                          \
                                    __target_tile_ptr,                          \
                                    __region_ptr,                               \
                                    __is_new_frame,                             \
                                    __opacity)                                  \
    __draw_round_corner_image(                                                  \
                (__source_tile_ptr),                                            \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__is_new_frame),                                               \
                (__opacity),                                                    \
                ((const arm_2d_tile_t *)&c_tileWhiteDotMask))

#define __draw_round_corner_image0( __source_tile_ptr,                          \
                                    __target_tile_ptr,                          \
                                    __region_ptr,                               \
                                    __is_new_frame)                             \
    __draw_round_corner_image(                                                  \
                (__source_tile_ptr),                                            \
                (__target_tile_ptr),                                            \
                (__region_ptr),                                                 \
                (__is_new_frame),                                               \
                0xFF,                                                           \
                ((const arm_2d_tile_t *)&c_tileWhiteDotMask))

#define draw_round_corner_image(__source_tile_ptr,                              \
                                __target_tile_ptr,                              \
                                __region_ptr,                                   \
                                __is_new_frame,                                 \
                                ...)                                            \
            ARM_CONNECT2(   __draw_round_corner_image,                          \
                            __ARM_VA_NUM_ARGS(__VA_ARGS__))(                    \
                                                (__source_tile_ptr),            \
                                                (__target_tile_ptr),            \
                                                (__region_ptr),                 \
                                                (__is_new_frame),##__VA_ARGS__)

#define draw_round_corner_border(__target_tile_ptr,                             \
                                 __target_region_ptr,                           \
                                 __colour,                                      \
                                 ...)                                           \
            __draw_round_corner_border( (__target_tile_ptr),                    \
                                        (__target_region_ptr),                  \
                                        (__colour),                             \
                                        __VA_ARGS__,                            \
                                        &c_tileCircleMask)

#define draw_round_corner_border_with_circle_mask(__target_tile_ptr,            \
                                 __target_region_ptr,                           \
                                 __colour,                                      \
                                 __circle_mask_ptr,                             \
                                 ...)                                           \
            __draw_round_corner_border(                                         \
                                    (__target_tile_ptr),                        \
                                    (__target_region_ptr),                      \
                                    (__colour),                                 \
                                    __VA_ARGS__,                                \
                                    (const arm_2d_tile_t *)(__circle_mask_ptr))

/*============================ TYPES =========================================*/

typedef struct {
    uint8_t chLeft;
    uint8_t chTop;
    uint8_t chRight;
    uint8_t chBottom;
} arm_2d_border_opacity_t;

typedef struct {
    uint8_t chTopLeft;
    uint8_t chTopRight;
    uint8_t chBottomLeft;
    uint8_t chBottomRight;
} arm_2d_corner_opacity_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern
const arm_2d_tile_t c_tileWhiteDotMask;

extern
const arm_2d_tile_t c_tileWhiteDotMask2;

extern 
const arm_2d_tile_t c_tileCircleMask;

extern 
const arm_2d_tile_t c_tileCircleMask2;
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void __draw_round_corner_box(   const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tColour,
                                uint8_t chOpacity,
                                const arm_2d_tile_t *ptCircleMask);

extern
ARM_NONNULL(1)
void __draw_round_corner_image( const arm_2d_tile_t *ptSource,
                                const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                bool bIsNewFrame,
                                uint8_t chOpacity,
                                const arm_2d_tile_t *ptCircleMask);

extern
ARM_NONNULL(1)
void __draw_round_corner_border(const arm_2d_tile_t *ptTarget,
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tColour,
                                arm_2d_border_opacity_t Opacity,
                                arm_2d_corner_opacity_t CornerOpacity,
                                const arm_2d_tile_t *ptCircleMask);

extern
ARM_NONNULL(1)
void draw_glass_bar(const arm_2d_tile_t *ptTarget,
                    const arm_2d_region_t *ptRegion,
                    uint8_t chOpacity,
                    bool bIsReflectionOnTop);

#ifdef   __cplusplus
}
#endif

#endif
