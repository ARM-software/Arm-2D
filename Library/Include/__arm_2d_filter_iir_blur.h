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
 * Title:        __arm_2d_filter_iir_blur.h
 * Description:  APIs for IIR Blur
 *
 * $Date:        25. April 2025
 * $Revision:    V.2.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FILTER_IIR_BLUR_H__
#define __ARM_2D_FILTER_IIR_BLUR_H__

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
 * \addtogroup gFilter 7 Filter Operations
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum {
    ARM_IIR_BLUR_MODE_HORIZONTAL            = (1 << 0),
    ARM_IIR_BLUR_MODE_VERTICAL              = (1 << 1),

    ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL    = (1 << 2),
    ARM_IIR_BLUR_MODE_REVERSE_VERTICAL      = (1 << 3),

    ARM_IIR_BLUR_MODE_FORWARD               = ARM_IIR_BLUR_MODE_HORIZONTAL
                                            | ARM_IIR_BLUR_MODE_VERTICAL,

    ARM_IIR_BLUR_MODE_REVERSE               = ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL
                                            | ARM_IIR_BLUR_MODE_REVERSE_VERTICAL,
    
    ARM_IIR_BLUR_MODE_DEFAULT               = 0,
    ARM_IIR_BLUR_MODE_BEST                  = ARM_IIR_BLUR_MODE_FORWARD
                                            | ARM_IIR_BLUR_MODE_REVERSE,

    ARM_IIR_BLUR_MODE_BEST_HORIZONTAL       = ARM_IIR_BLUR_MODE_HORIZONTAL
                                            | ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL,

    ARM_IIR_BLUR_MODE_BEST_VERTICAL         = ARM_IIR_BLUR_MODE_VERTICAL
                                            | ARM_IIR_BLUR_MODE_REVERSE_VERTICAL,
} arm_2d_iir_blur_mode_t;

typedef struct __arm_2d_iir_blur_acc_cccn888_t {
    uint16_t hwB;
    uint16_t hwG;
    uint16_t hwR;
} __arm_2d_iir_blur_acc_cccn888_t;

typedef __arm_2d_iir_blur_acc_cccn888_t __arm_2d_iir_blur_acc_rgb565_t;

typedef struct __arm_2d_iir_blur_acc_gray8_t {
    uint16_t hwC;
} __arm_2d_iir_blur_acc_gray8_t;

typedef struct arm_2d_filter_iir_blur_descriptor_t {
ARM_PRIVATE(
    implement(arm_2d_op_t);                         /* inherit from base class arm_2d_op_cp_t*/

    union {
        uint8_t chBlurMode;
        struct {
            uint8_t bForwardHorizontal  : 1;
            uint8_t bForwardVertical    : 1;
            uint8_t bReverseHorizontal  : 1;
            uint8_t bReverseVertical    : 1;
        };
    };

    uint8_t chBlurDegree;
)

    arm_2d_scratch_mem_t tScratchMemory;

}arm_2d_filter_iir_blur_descriptor_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 *  \brief Apply IIR blur to a specified region on the target tile
 *  \param[in] ptOP the control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] chBlurDegree the blur degree, 
 *  \note - 0~127 works as generic anti-alias, and
 *        - 128~239 works as blur
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1,2)
arm_fsm_rt_t arm_2dp_gray8_filter_iir_blur(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP,
                    const arm_2d_tile_t *ptTarget,
                    const arm_2d_region_t *ptRegion,
                    uint8_t chBlurDegree);



/*!
 *  \brief Release the resource used by the given IIR blur
 *  \param[in] ptOP the control block
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2dp_gray8_filter_iir_blur_depose(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP);

/*!
 *  \brief Apply IIR blur to a specified region on the target tile
 *  \param[in] ptOP the control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] chBlurDegree the blur degree, 
 *  \note - 0~127 works as generic anti-alias, and
 *        - 128~239 works as blur
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1,2)
arm_fsm_rt_t arm_2dp_rgb565_filter_iir_blur(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP,
                    const arm_2d_tile_t *ptTarget,
                    const arm_2d_region_t *ptRegion,
                    uint8_t chBlurDegree);



/*!
 *  \brief Release the resource used by the given IIR blur
 *  \param[in] ptOP the control block
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2dp_rgb565_filter_iir_blur_depose(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP);

/*!
 *  \brief Apply IIR blur to a specified region on the target tile
 *  \param[in] ptOP the control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] chBlurDegree the blur degree, 
 *  \note - 0~127 works as generic anti-alias, and
 *        - 128~239 works as blur
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1,2)
arm_fsm_rt_t arm_2dp_cccn888_filter_iir_blur(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP,
                    const arm_2d_tile_t *ptTarget,
                    const arm_2d_region_t *ptRegion,
                    uint8_t chBlurDegree);



/*!
 *  \brief Release the resource used by the given IIR blur
 *  \param[in] ptOP the control block
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(1)
arm_fsm_rt_t arm_2dp_cccn888_filter_iir_blur_depose(  
                    arm_2d_filter_iir_blur_descriptor_t *ptOP);



/*!
 * \brief set IIR blur working mode
 * \param[in] ptOP the control block
 * \param[in] chModeMask working mode ARM_IIR_BLUR_MODE_xxxx
 * \return none
 */
extern
ARM_NONNULL(1)
void arm_2dp_filter_iir_blur_mode_set(arm_2d_filter_iir_blur_descriptor_t *ptOP,
                                      uint_fast8_t chModeMask);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_FILTER_IIR_BLUR_H__ */