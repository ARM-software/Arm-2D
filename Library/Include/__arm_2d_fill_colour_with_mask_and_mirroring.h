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
 * Title:        __arm_2d_fill_colour_with_mask_and_mirroring.h
 * Description:  A private header file for 
 *               colour-filling-with-mask-and-mirroring
 *
 * $Date:        20. Feb 2024
 * $Revision:    V.0.6.1
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */
 
#ifndef __ARM_2D_FILL_COLOUR_WITH_MIRRORING_H__
#define __ARM_2D_FILL_COLOUR_WITH_MIRRORING_H__

/*============================ INCLUDES ======================================*/

#include "arm_2d_types.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wc11-extensions"
#endif

/*============================ MACROS ========================================*/
/*!
 * \addtogroup gAlpha 4 Alpha Blending Operations
 * @{
 */

/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_rgb565_fill_colour_with_mask_and_x_mirror(                       \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_rgb565_fill_colour_with_mask_and_x_mirror(                  \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR))

#define arm_2d_rgb565_fill_colour_with_mask_x_mirror_and_opacity(               \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __ALPHA_ADDR,   /*   alpha tile address */  \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)      /*   opacity */             \
            arm_2dp_rgb565_fill_colour_with_mask_x_mirror_and_opacity(          \
                                      NULL,                                     \
                                     (__TARGET_ADDR),                           \
                                     (__REGION_ADDR),                           \
                                     (__ALPHA_ADDR),                            \
                                     (__COLOUR),                                \
                                     (__OPACITY))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_and_x_mirror(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_mask_x_mirror_and_opacity(
                                        arm_2d_op_alpha_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptAlpha,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity);

/*! @} */

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
