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
 * Title:        __arm_2d_fill_colour_with_alpha_gradient.c
 * Description:  The header file of APIs for colour-filling-with-alpha-gradient
 *
 * $Date:        16. July 2024
 * $Revision:    V.0.8.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FILL_COLOUR_WITH_ALPHA_GRADIENT_H__
#define __ARM_2D_FILL_COLOUR_WITH_ALPHA_GRADIENT_H__

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
 * \addtogroup gAlpha 4 Alpha Blending Operations
 * @{
 */

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_fill_colour_with_4pts_alpha_gradient(                      \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __COLOUR,       /*   target colour     */   \
                                    ...)            /*   sample points     */   \
            arm_2dp_gray8_fill_colour_with_4pts_alpha_gradient(                 \
                                                        NULL,                   \
                                                        (__TARGET_ADDR),        \
                                                        (__REGION_ADDR),        \
                                                        (__COLOUR),             \
                                                        __VA_ARGS__)

#define arm_2d_rgb565_fill_colour_with_4pts_alpha_gradient(                     \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __COLOUR,       /*   target colour     */   \
                                    ...)            /*   sample points     */   \
            arm_2dp_rgb565_fill_colour_with_4pts_alpha_gradient(                \
                                                        NULL,                   \
                                                        (__TARGET_ADDR),        \
                                                        (__REGION_ADDR),        \
                                                        (__COLOUR),             \
                                                        __VA_ARGS__)

#define arm_2d_cccn888_fill_colour_with_4pts_alpha_gradient(                    \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __COLOUR,       /*   target colour     */   \
                                    ...)            /*   sample points     */   \
            arm_2dp_cccn888_fill_colour_with_4pts_alpha_gradient(               \
                                                        NULL,                   \
                                                        (__TARGET_ADDR),        \
                                                        (__REGION_ADDR),        \
                                                        (__COLOUR),             \
                                                        __VA_ARGS__)

/*============================ TYPES =========================================*/

typedef union arm_2d_alpha_samples_4pts_t {
    struct {
        uint8_t chTopLeft;
        uint8_t chTopRight;
        uint8_t chBottomLeft;
        uint8_t chBottomRight;
    };
    uint8_t chAlpha[4];
} arm_2d_alpha_samples_4pts_t;

typedef union arm_2d_alpha_samples_3pts_t {
    struct {
        uint8_t chTopLeft;
        uint8_t chTopRight;
        uint8_t chBottomLeft;
    };
    uint8_t chAlpha[3];
} arm_2d_alpha_samples_3pts_t;

typedef union arm_2d_alpha_samples_2pts_t {
    struct {
        uint8_t chLeft;
        uint8_t chRight;
    };
    struct {
        uint8_t chTop;
        uint8_t chBottom;
    };
    uint8_t chAlpha[2];
} arm_2d_alpha_samples_2pts_t;

typedef struct arm_2d_fill_cl_4p_al_grd_t arm_2d_fill_cl_4p_al_grd_t;

struct arm_2d_fill_cl_4p_al_grd_t {
ARM_PRIVATE(
    implement(arm_2d_op_t);                     /* inherit from base class arm_2d_op_cp_t*/

    union {
        uint8_t  chColour;                      //!< 8bit colour
        uint16_t hwColour;                      //!< 16bit colour
        uint32_t wColour;                       //!< 32bit colour
    };

    arm_2d_alpha_samples_4pts_t tSamplePoints;
)
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*!
 * \brief fill the target region with a given colour and an alpha gradient that
 *        is defined by 4 corner sample points. 
 * \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] tColour the target colour
 *  \param[in] tSamplePoints the 4 corner sample points.
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_4pts_alpha_gradient(  
                            arm_2d_fill_cl_4p_al_grd_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            arm_2d_color_gray8_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints);

/*!
 * \brief fill the target region with a given colour and an alpha gradient that
 *        is defined by 4 corner sample points. 
 * \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] tColour the target colour
 *  \param[in] tSamplePoints the 4 corner sample points.
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_4pts_alpha_gradient(  
                            arm_2d_fill_cl_4p_al_grd_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            arm_2d_color_rgb565_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints);

/*!
 * \brief fill the target region with a given colour and an alpha gradient that
 *        is defined by 4 corner sample points. 
 * \param[in] ptOP the control block, NULL means using the default control block
 *  \param[in] ptTarget the target tile
 *  \param[in] ptRegion the target region
 *  \param[in] tColour the target colour
 *  \param[in] tSamplePoints the 4 corner sample points.
 *  \return arm_fsm_rt_t the operations result
 */
extern
ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_4pts_alpha_gradient(  
                            arm_2d_fill_cl_4p_al_grd_t *ptOP,
                            const arm_2d_tile_t *ptTarget,
                            const arm_2d_region_t *ptRegion,
                            arm_2d_color_cccn888_t tColour,
                            arm_2d_alpha_samples_4pts_t tSamplePoints);

/*! @} */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_FILTER_IIR_BLUR_H__ */