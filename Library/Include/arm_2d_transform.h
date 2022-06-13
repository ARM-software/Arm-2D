/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        #include "arm_2d_transform.h"
 * Description:  Public header file to contain the APIs for transform
 *
 * $Date:        14 April 2022
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_ROTATION_H__
#define __ARM_2D_ROTATION_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/*============================ MACROS ========================================*/
/*! \brief deprecated type alias, should be removed in the future
 */
#define arm_2d_op_rotate_t          arm_2d_op_trans_t
#define arm_2d_op_rotate_opacity_t  arm_2d_op_trans_opa_t 


/*! \brief deprecated API alias, should be removed in the future
 */
#define arm_2dp_tile_rotate                                                     \
                            arm_2dp_tile_transform
#define arm_2dp_gray8_tile_rotate_prepare                                       \
                            arm_2dp_gray8_tile_transform_prepare
#define arm_2dp_rgb565_tile_rotate_prepare                                      \
                            arm_2dp_rgb565_tile_transform_prepare
#define arm_2dp_cccn888_tile_rotate_prepare                                     \
                            arm_2dp_cccn888_tile_transform_prepare
#define arm_2dp_gray8_tile_rotate_with_opacity_prepare                          \
                            arm_2dp_gray8_tile_transform_with_opacity_prepare
#define arm_2dp_rgb565_tile_rotate_with_opacity_prepare                         \
                            arm_2dp_rgb565_tile_transform_with_opacity_prepare
#define arm_2dp_cccn888_tile_rotate_with_opacity_prepare                        \
                            arm_2dp_cccn888_tile_transform_with_opacity_prepare

/*============================ MACROFIED FUNCTIONS ===========================*/

/*! \brief API wrappers: Rotation
 */
#define arm_2d_gray8_tile_rotation_prepare(__SRC_TILE_ADDR,                     \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_gray8_tile_transform_prepare(  NULL,                        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb565_tile_rotation_prepare(__SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_rgb565_tile_transform_prepare(  NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR))
                                     
#define arm_2d_rgb888_tile_rotation_prepare(__SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_cccn888_tile_transform_prepare(  NULL,                      \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR))

#define arm_2d_gray8_tile_rotation_with_alpha_prepare(                          \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_rotation_with_opacity_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_rotation_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_rotation_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb888_tile_rotation_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))
                                     
#define arm_2d_rgb888_tile_rotation_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))


#define arm_2d_cccn888_tile_rotation_with_alpha_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_cccn888_tile_rotation_with_opacity_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_rotation_with_src_mask_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE)                            \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f)

#define arm_2d_rgb565_tile_rotation_with_src_mask_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE)                            \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f)

#define arm_2d_cccn888_tile_rotation_with_src_mask_prepare(                     \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE)                            \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(  NULL,        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f)

#define arm_2d_gray8_tile_rotation_with_src_mask_and_opacity_prepare(           \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __OPACITY)                          \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__OPACITY))

#define arm_2d_rgb565_tile_rotation_with_src_mask_and_opacity_prepare(          \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __OPACITY)                          \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__OPACITY))

#define arm_2d_cccn888_tile_rotation_with_src_mask_and_opacity_prepare(         \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __OPACITY)                          \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                                   1.0f,                        \
                                                   (__OPACITY))

#define arm_2d_tile_rotate( __DES_TILE_ADDR,                                    \
                            __DES_REGION_ADDR,                                  \
                            __DES_CENTRE_ADDR)                                  \
            arm_2dp_tile_transform(NULL,                                        \
                                (__DES_TILE_ADDR),                              \
                                (__DES_REGION_ADDR),                            \
                                (__DES_CENTRE_ADDR))


/*! \note following macro APIs rely on the boolean variable bIsNewFrame. 
 *!       Please make sure you have define it with the correct name and the 
 *!       corresponding value. If you don't use the PFB interfaces for neight 
 *!       the low level rendering nor the high level GUI drawing, please find 
 *!       such variable with the value "true".
 */
 
#define arm_2dp_gray8_tile_rotation(    __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_rotation(   __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_rotation(   __CB_ADDR,                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,...)                       \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_gray8_tile_rotation_with_alpha(                                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_gray8_tile_rotation_with_opacity(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_rotation_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_rotation_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_rotation_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_rotation_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_rotation_with_alpha(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_rotation_with_opacity(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_gray8_tile_rotation_with_src_mask(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_rgb565_tile_rotation_with_src_mask(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_rotation_with_src_mask(                            \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_gray8_tile_rotation_with_src_mask_and_opacity(                  \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_rgb565_tile_rotation_with_src_mask_and_opacity(                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_rotation_with_src_mask_and_opacity(                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_rotation(                                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb565_tile_rotation(                                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_rotation(                                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_rotation_with_alpha(                                  \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_gray8_tile_rotation_with_opacity(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb565_tile_rotation_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_rgb565_tile_rotation_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_rotation_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb888_tile_rotation_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_alpha(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_opacity(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_rotation_with_src_mask(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_rotation_with_src_mask(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_src_mask(                             \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f);                                  \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_rotation_with_src_mask_and_opacity(                   \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_rotation_with_src_mask_and_opacity(                  \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_rotation_with_src_mask_and_opacity(                 \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (__ANGLE),                              \
                                        1.0f,                                   \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

/*! \brief API wrappers: Transform
 */

#define arm_2d_gray8_tile_transform_prepare(__SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_gray8_tile_transform_prepare(  NULL,                        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb565_tile_transform_prepare(__SRC_TILE_ADDR,                   \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_rgb565_tile_transform_prepare(  NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

                                     
#define arm_2d_rgb888_tile_transform_prepare(__SRC_TILE_ADDR,                   \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_cccn888_tile_transform_prepare(  NULL,                      \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_gray8_tile_transform_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_transform_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_transform_with_alpha_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_transform_with_opacity_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb888_tile_transform_with_alpha_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))
                                     
#define arm_2d_rgb888_tile_transform_with_opacity_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))


#define arm_2d_cccn888_tile_transform_with_alpha_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_cccn888_tile_transform_with_opacity_prepare(                     \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_transform_with_src_mask_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE)                            \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE))

#define arm_2d_rgb565_tile_transform_with_src_mask_prepare(                     \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE)                            \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE))

#define arm_2d_cccn888_tile_transform_with_src_mask_prepare(                    \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE)                            \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(  NULL,        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE))

#define arm_2d_gray8_tile_transform_with_src_mask_and_opacity_prepare(          \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_rgb565_tile_transform_with_src_mask_and_opacity_prepare(         \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_cccn888_tile_transform_with_src_mask_and_opacity_prepare(        \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __ANGLE,                            \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                            (float)(__ANGLE),                   \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_tile_transform( __DES_TILE_ADDR,                                 \
                            __DES_REGION_ADDR,                                  \
                            __DES_CENTRE_ADDR)                                  \
            arm_2dp_tile_transform(NULL,                                        \
                                (__DES_TILE_ADDR),                              \
                                (__DES_REGION_ADDR),                            \
                                (__DES_CENTRE_ADDR))


/*! \note following macro APIs rely on the boolean variable bIsNewFrame. 
 *!       Please make sure you have define it with the correct name and the 
 *!       corresponding value. If you don't use the PFB interfaces for neight 
 *!       the low level rendering nor the high level GUI drawing, please find 
 *!       such variable with the value "true".
 */
 
#define arm_2dp_gray8_tile_transform(   __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_transform(   __CB_ADDR,                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_transform(   __CB_ADDR,                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,...)                       \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_gray8_tile_transform_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_gray8_tile_transform_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_transform_with_alpha(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_transform_with_opacity(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_transform_with_alpha(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_transform_with_opacity(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_transform_with_alpha(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_transform_with_opacity(                            \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_gray8_tile_transform_with_src_mask(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_transform_with_src_mask(                            \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_transform_with_src_mask(                           \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })



#define arm_2dp_gray8_tile_transform_with_src_mask_and_opacity(                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity(                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity(               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_gray8_tile_transform(                                            \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_transform(                                           \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_transform(                                           \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_transform_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_gray8_tile_transform_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb565_tile_transform_with_alpha(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_rgb565_tile_transform_with_opacity(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_transform_with_alpha(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb888_tile_transform_with_opacity(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_transform_with_alpha(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_transform_with_opacity(                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_gray8_tile_transform_with_src_mask(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_transform_with_src_mask(                             \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_transform_with_src_mask(                            \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE));                      \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_transform_with_src_mask_and_opacity(                  \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_transform_with_src_mask_and_opacity(                 \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_transform_with_src_mask_and_opacity(                \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE),                       \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

/*! \brief API wrappers: scaling
 */
#define arm_2d_gray8_tile_scaling_prepare(__SRC_TILE_ADDR,                      \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_gray8_tile_transform_prepare(  NULL,                        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb565_tile_scaling_prepare(__SRC_TILE_ADDR,                     \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_rgb565_tile_transform_prepare(  NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_rgb888_tile_scaling_prepare(__SRC_TILE_ADDR,                     \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR)                       \
            arm_2dp_cccn888_tile_transform_prepare(  NULL,                      \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR))

#define arm_2d_gray8_tile_scaling_with_alpha_prepare(                           \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_scaling_with_opacity_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(  NULL,           \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_scaling_with_alpha_prepare(                          \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb565_tile_scaling_with_opacity_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_rgb888_tile_scaling_with_alpha_prepare(                          \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))
                                     
#define arm_2d_rgb888_tile_scaling_with_opacity_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_cccn888_tile_scaling_with_alpha_prepare(                         \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_cccn888_tile_scaling_with_opacity_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __MSK_COLOUR,                       \
                                            __RATIO)                            \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__MSK_COLOUR),              \
                                                   (__RATIO))

#define arm_2d_gray8_tile_scaling_with_src_mask_prepare(                        \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE)                            \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(  NULL,          \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE))

#define arm_2d_rgb565_tile_scaling_with_src_mask_prepare(                       \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE)                            \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(  NULL,         \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE))

#define arm_2d_cccn888_tile_scaling_with_src_mask_prepare(                      \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE)                            \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(  NULL,        \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE))

#define arm_2d_gray8_tile_scaling_with_src_mask_and_opacity_prepare(            \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_rgb565_tile_scaling_with_src_mask_and_opacity_prepare(           \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_cccn888_tile_scaling_with_src_mask_and_opacity_prepare(          \
                                            __SRC_TILE_ADDR,                    \
                                            __SRC_MASK_ADDR,                    \
                                            __CENTRE,                           \
                                            __SCALE,                            \
                                            __OPACITY)                          \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                                    NULL,                       \
                                                   (__SRC_TILE_ADDR),           \
                                                   (__SRC_MASK_ADDR),           \
                                                   (__CENTRE),                  \
                                                   0.0f,                        \
                                            (float)(__SCALE),                   \
                                                   (__OPACITY))

#define arm_2d_tile_scale( __DES_TILE_ADDR,                                     \
                            __DES_REGION_ADDR,                                  \
                            __DES_CENTRE_ADDR)                                  \
            arm_2dp_tile_transform(NULL,                                        \
                                (__DES_TILE_ADDR),                              \
                                (__DES_REGION_ADDR),                            \
                                (__DES_CENTRE_ADDR))


/*! \note following macro APIs rely on the boolean variable bIsNewFrame. 
 *!       Please make sure you have define it with the correct name and the 
 *!       corresponding value. If you don't use the PFB interfaces for neight 
 *!       the low level rendering nor the high level GUI drawing, please find 
 *!       such variable with the value "true".
 */
 
#define arm_2dp_gray8_tile_scaling(    __CB_ADDR,                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
 
#define arm_2dp_rgb565_tile_scaling(   __CB_ADDR,                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_scaling(   __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,...)                       \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_gray8_tile_scaling_with_alpha(                                  \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_gray8_tile_scaling_with_opacity(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_scaling_with_alpha(                                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb565_tile_scaling_with_opacity(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO,...)                            \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_scaling_with_alpha(                                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_rgb888_tile_scaling_with_opacity(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2dp_cccn888_tile_scaling_with_alpha(                                \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_scaling_with_opacity(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
        
#define arm_2dp_gray8_tile_scaling_with_src_mask(                               \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_rgb565_tile_scaling_with_src_mask(                              \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_scaling_with_src_mask(                             \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_gray8_tile_scaling_with_src_mask_and_opacity(                   \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2dp_rgb565_tile_scaling_with_src_mask_and_opacity(                  \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2dp_cccn888_tile_scaling_with_src_mask_and_opacity(                 \
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                        ...)                                    \
        ({assert(NULL != (__CB_ADDR)); if (bIsNewFrame) {                       \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform((arm_2d_op_trans_t *)(__CB_ADDR),                \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_scaling(                                              \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_prepare(                               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_scaling(                                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_prepare(                              \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_scaling(                                             \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR, ...)                      \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_prepare(                             \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR));                        \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_scaling_with_alpha(                                   \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_gray8_tile_scaling_with_opacity(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_opacity_prepare(                  \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb565_tile_scaling_with_alpha(                                  \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_rgb565_tile_scaling_with_opacity(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_opacity_prepare(                 \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })


#define arm_2d_rgb888_tile_scaling_with_alpha(                                  \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb888_tile_scaling_with_opacity(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_scaling_with_alpha(                                 \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_scaling_with_opacity(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __MSK_COLOUR,                           \
                                        __RATIO, ...)                           \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_opacity_prepare(                \
                                        NULL,                                   \
                                        (__SRC_TILE_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__MSK_COLOUR),                         \
                                        (__RATIO));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_scaling_with_src_mask(                                \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_prepare(                 \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_scaling_with_src_mask(                               \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_prepare(                \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_cccn888_tile_scaling_with_src_mask(                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_prepare(               \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE));                             \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_scaling_with_src_mask_and_opacity(                    \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(     \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_scaling_with_src_mask_and_opacity(                   \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(    \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
#define arm_2d_cccn888_tile_scaling_with_src_mask_and_opacity(                  \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MASK_ADDR,                        \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __SCALE,                                \
                                        __OPACITY,                              \
                                         ...)                                   \
        ({{                                                                     \
            arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(   \
                                        (NULL),                                 \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        0.0f,                                   \
                                        (__SCALE),                              \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform(NULL,                                            \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })
        
/*============================ TYPES =========================================*/

typedef struct __arm_2d_transform_info_t {
    float                   fAngle;         //!< target angle
    float                   fScale;          //!< for zooming
    arm_2d_location_t       tCenter;
    union {
        uint8_t             chColour;
        uint32_t            wColour;
        uint16_t            hwColour;
    } Mask;                                 //!< the colour to fill when out of range
    
ARM_PRIVATE(
    arm_2d_location_t       tDummySourceOffset;
    struct {
        arm_2d_region_t     tRegion;
        arm_2d_tile_t       tTile;
    } Target;
)
} __arm_2d_transform_info_t;

/*! \brief arm_2d_op_trans_t is inherit from arm_2d_op_src_orig_t
 */
typedef struct arm_2d_op_trans_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    
    __arm_2d_transform_info_t   tTransform;
    
} arm_2d_op_trans_t;


/*! \brief arm_2d_op_trans_opa_t is inherit from arm_2d_op_trans_t
 */
typedef struct arm_2d_op_trans_opa_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    
    __arm_2d_transform_info_t   tTransform;
    uint8_t                     chOpacity;
    
} arm_2d_op_trans_opa_t;


/*! \brief arm_2d_op_trans_msk_t is inherit from arm_2d_op_src_orig_msk_t
 */
typedef struct arm_2d_op_trans_msk_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    struct {
        const arm_2d_tile_t     *ptOriginSide;  //!< origin side mask
        const arm_2d_tile_t     *ptTargetSide;  //!< target side mask
    } Mask;
    
    
    __arm_2d_transform_info_t   tTransform;
    
} arm_2d_op_trans_msk_t;


/*! \brief arm_2d_op_trans_msk_t is inherit from arm_2d_op_trans_msk_t
 */
typedef struct arm_2d_op_trans_msk_opa_t {
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;        //!< target tile
        const arm_2d_region_t   *ptRegion;      //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< source tile
    }Source;
    uint32_t wMode;
    struct {
        const arm_2d_tile_t     *ptTile;        //!< the origin tile
        arm_2d_tile_t           tDummySource;   //!< the buffer for the source
    }Origin;
    struct {
        const arm_2d_tile_t     *ptOriginSide;  //!< origin side mask
        const arm_2d_tile_t     *ptTargetSide;  //!< target side mask
    } Mask;
    
    
    __arm_2d_transform_info_t   tTransform;
    uint8_t                     chOpacity;
    
} arm_2d_op_trans_msk_opa_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_prepare(
                                        arm_2d_op_trans_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chFillColour);

extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_prepare(
                                        arm_2d_op_trans_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast16_t hwFillColour);
                                     
extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_prepare(
                                        arm_2d_op_trans_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint32_t wFillColour);
extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chRatio);

extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chRatio);
                                     
extern
ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint32_t wFillColour,
                                        uint_fast8_t chRatio);

extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_prepare(
                                        arm_2d_op_trans_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale);

extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_prepare(
                                        arm_2d_op_trans_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale);

extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_prepare(
                                        arm_2d_op_trans_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale);
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(
                                        arm_2d_op_trans_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity);

extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(
                                        arm_2d_op_trans_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity);

extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(
                                        arm_2d_op_trans_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity);
                                        
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_tile_transform( arm_2d_op_trans_t *ptOP,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     const arm_2d_location_t *ptTargetCentre);

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
