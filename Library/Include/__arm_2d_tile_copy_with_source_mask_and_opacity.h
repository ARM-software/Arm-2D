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
 * Title:        __arm_2d_tile_copy_with_mask_and_opacity.h
 * Description:  APIs for Tile-Copy-with-Source-Mask-and-Opacity operations
 *
 * $Date:        18. May 2024
 * $Revision:    V.1.0.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_TILE_COPY_WITH_MASK_AND_OPACITY_H__
#define __ARM_2D_TILE_COPY_WITH_MASK_AND_OPACITY_H__

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
#endif

/*!
 * \addtogroup gAlpha 4 Alpha Blending Operations
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_tile_copy_with_src_mask_and_opacity_only(                 \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __OPACITY)      /*   opacity */             \
            arm_2dp_gray8_tile_copy_with_src_mask_and_opacity_only(            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__OPACITY))

#define arm_2d_rgb565_tile_copy_with_src_mask_and_opacity_only(                 \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __OPACITY)      /*   opacity */             \
            arm_2dp_rgb565_tile_copy_with_src_mask_and_opacity_only(            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__OPACITY))

#define arm_2d_cccn888_tile_copy_with_src_mask_and_opacity_only(                 \
                                    __SRC_ADDR,     /*   source tile address */ \
                                    __SRC_MSK_ADDR, /*   source mask address */ \
                                    __DES_ADDR,     /*   target tile address */ \
                                    __REGION,       /*   region address */      \
                                    __OPACITY)      /*   opacity */             \
            arm_2dp_cccn888_tile_copy_with_src_mask_and_opacity_only(            \
                                        NULL,                                   \
                                        (__SRC_ADDR),                           \
                                        (__SRC_MSK_ADDR),                       \
                                        (__DES_ADDR),                           \
                                        (__REGION),                             \
                                        (__OPACITY))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


extern
ARM_NONNULL(2,3,4)
/*!
 * \brief copy a source tile to a target tile with a source mask
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_src_mask_and_opacity_only(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity);


extern
ARM_NONNULL(2,3,4)
/*!
 * \brief copy a source tile to a target tile with a source mask
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_src_mask_and_opacity_only(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity);


extern
ARM_NONNULL(2,3,4)
/*!
 * \brief copy a source tile to a target tile with a source mask
 * \param[in] ptOP the control block, NULL means using the default control block
 * \param[in] ptSource the source tile
 * \param[in] ptSrcMask the mask on the source side
 * \param[in] ptTarget the target tile
 * \param[in] ptRegion the target region
 * \param[in] chOpacity the opacity
 * \return arm_fsm_rt_t the operation result
 */
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_src_mask_and_opacity_only(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint8_t chOpacity);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_TILE_COPY_WITH_MASK_AND_OPACITY_H__ */
