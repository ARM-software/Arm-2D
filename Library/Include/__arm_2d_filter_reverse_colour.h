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
 * Title:        __arm_2d_filter_reverse_colour.h
 * Description:  APIs for colour-reversing
 *
 * $Date:        29. Nov 2024
 * $Revision:    V.0.3.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_REVERSE_COLOUR_H__
#define __ARM_2D_REVERSE_COLOUR_H__

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
 * \addtogroup gFilter 7 Filter Operations
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

extern
ARM_NONNULL(2)
/*!
 *  \brief reverse the colour of the target region
 *  \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] ptLineMsk the target line mask
 *  \param[in] tColour the target colour
 *  \return arm_fsm_rt_t the operations result
 */
arm_fsm_rt_t arm_2dp_rgb565_reverse_colour( arm_2d_op_fill_cl_msk_t *ptOP,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_REVERSE_COLOUR_H__ */
