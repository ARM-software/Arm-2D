/*
 * Copyright (C) 2024 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_tile_copy_with_transformed_mask_source_mask_and_target_mask.h
 * Description:  The header file of APIs for tile-copy-with-source-mask-transformed-
 *               -mask-and-target-mask
 * 
 * $Date:        16 Dec 2025
 * $Revision:    v0.9.3
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_TILE_COPY_WITH_SOURCE_MASK_TRANSFORMED_MASK_AND_TARGET_MASK_H__
#define __ARM_2D_TILE_COPY_WITH_SOURCE_MASK_TRANSFORMED_MASK_AND_TARGET_MASK_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,64
#endif

/*!
 * \addtogroup gTransform 5 Transform Operations
 * @{
 */
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#define arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
    ({                                                                          \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
    ({                                                                          \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        ...)                                    \
    ({                                                                          \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y));                    \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })


#define arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __TRANS_MASK_ADDR,                      \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
        ({ if (bIsNewFrame) {                                                   \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        };                                                                      \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
        })

#define arm_2d_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity(\
                                        __CB_ADDR,                              \
                                        __SRC_TILE_ADDR,                        \
                                        __SRC_MSK_ADDR,                         \
                                        __MASK_ADDR,                            \
                                        __DES_TILE_ADDR,                        \
                                        __DES_MASK_ADDR,                        \
                                        __DES_REGION_ADDR,                      \
                                        __CENTRE,                               \
                                        __ANGLE,                                \
                                        __SCALE_X,                              \
                                        __SCALE_Y,                              \
                                        __OPACITY,...)                          \
    ({                                                                          \
        arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_prepare(\
                                        (__CB_ADDR),                            \
                                        (__SRC_TILE_ADDR),                      \
                                        (__SRC_MSK_ADDR),                       \
                                        (__MASK_ADDR),                          \
                                        (__DES_MASK_ADDR),                      \
                                        (__CENTRE),                             \
                                        (float)(__ANGLE),                       \
                                        (float)(__SCALE_X),                     \
                                        (float)(__SCALE_Y),                     \
                                        (__OPACITY));                           \
        arm_2dp_tile_transform_xy((arm_2d_op_trans_t *)(__CB_ADDR),             \
                            (__DES_TILE_ADDR),                                  \
                            (__DES_REGION_ADDR),                                \
                            (NULL,##__VA_ARGS__));                              \
    })

/*============================ TYPES =========================================*/

/*! \brief arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t is derived from arm_2d_op_src_orig_msk_extra_t
 */
typedef struct arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t {

    /* base arm_2d_op_src_orig_msk_extra_t begin */
    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< the dummy source tile
    }Source;

    union {
        struct {
            uint32_t                            : 31;
            uint32_t  bInvalid                  : 1;
        };
        uint32_t  wMode;
    };

    struct {
        const arm_2d_tile_t     *ptTile;                //!< the origin tile
        arm_2d_tile_t           tDummySource;           //!< the buffer for the source
    }Origin;

    struct {
        const arm_2d_tile_t     *ptOriginSide;          //!< origin side mask
        const arm_2d_tile_t     *ptTargetSide;          //!< target side mask
    } Mask;

    struct {
        const arm_2d_tile_t     *ptTile;                //!< the extra source tile
        const arm_2d_tile_t     *ptMask;                //!< the mask for the extra source tile
    } ExtraSource;
    /* base arm_2d_op_src_orig_msk_extra_t end */

    /* derived part */
    __arm_2d_transform_info_t   tTransform;     //!< transform context
    uint8_t chOpacity;

}arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief Prepare for a gray8 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask__prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a gray8 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask__and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*!
 * \brief Prepare for a gray8 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask__and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a gray8 tile copy with a source mask, a mask
 *        (to be transformed), a target mask and a given opacity.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_copy_with_transformed_mask_source_mask__target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*!
 * \brief Prepare for a rgb565 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask__prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a rgb565 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask__and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*!
 * \brief Prepare for a rgb565 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask__and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a rgb565 tile copy with a source mask, a mask
 *        (to be transformed), a target mask and a given opacity.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_copy_with_transformed_mask_source_mask__target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*!
 * \brief Prepare for a cccn888 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask__prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a cccn888 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask__and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*!
 * \brief Prepare for a cccn888 tile copy with a source mask, a mask
 *        (to be transformed) and a target mask.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask__and_target_mask_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY);

/*!
 * \brief Prepare for a cccn888 tile copy with a source mask, a mask
 *        (to be transformed), a target mask and a given opacity.
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptTransMask the mask to be transformed
 * \param[in] ptTargetMask the target mask
 * \param[in] tCentre the pivot in the source tile
 * \param[in] fAngle the rotation angle
 * \param[in] fScaleX the scaling factor for X axis
 * \param[in] fScaleY the scaling factor for X axis
 * \param[in] chOpacity the opacity
 * \return arm_2d_err_t the result of the preparing process
 */
extern
ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_copy_with_transformed_mask_source_mask__target_mask_and_opacity_prepare(
                                        arm_2d_op_tile_cp_src_msk_trans_msk_des_msk_opa_t *ptOP,
                                        const arm_2d_tile_t *ptTransMask,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSourceMask,
                                        const arm_2d_tile_t *ptTargetMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_TILE_COPY_WITH_SOURCE_MASK_TRANSFORMED_MASK_AND_TARGET_MASK_H__ */