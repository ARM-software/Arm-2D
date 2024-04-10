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
 * Title:        __arm_2d_fill_colour_with_horizontal_line_mask.h
 * Description:  APIs for colour-filling-with-horizontal-line-mask
 *
 * $Date:        10. April 2024
 * $Revision:    V.0.9.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FILL_COLOUR_WITH_HORIZONTAL_LINE_MASK_H__
#define __ARM_2D_FILL_COLOUR_WITH_HORIZONTAL_LINE_MASK_H__

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


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define arm_2d_gray8_fill_colour_with_horizontal_line_mask(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_gray8_fill_colour_with_horizontal_line_mask(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_gray8_t){(__COLOUR).tValue})

#define arm_2d_rgb565_fill_colour_with_horizontal_line_mask(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_rgb565_fill_colour_with_horizontal_line_mask(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_rgb565_t){(__COLOUR).tValue})

#define arm_2d_cccn888_fill_colour_with_horizontal_line_mask(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR)       /*   colour */              \
            arm_2dp_cccn888_fill_colour_with_horizontal_line_mask(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_cccn888_t){(__COLOUR).tValue})

#define arm_2d_gray8_fill_colour_with_horizontal_line_mask_and_opacity(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)      /*    Opacity */            \
            arm_2dp_gray8_fill_colour_with_horizontal_line_mask_and_opacity(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_gray8_t){(__COLOUR).tValue},    \
                                 (__OPACITY)}

#define arm_2d_rgb565_fill_colour_with_horizontal_line_mask_and_opacity(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)      /*    Opacity */            \
            arm_2dp_rgb565_fill_colour_with_horizontal_line_mask_and_opacity(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_rgb565_t){(__COLOUR).tValue},    \
                                 (__OPACITY)}

#define arm_2d_cccn888_fill_colour_with_horizontal_line_mask_and_opacity(                   \
                                    __TARGET_ADDR,  /*   target tile address*/  \
                                    __REGION_ADDR,  /*   target region address*/\
                                    __MASK_ADDR,    /*   mask tile address */   \
                                    __COLOUR,       /*   colour */              \
                                    __OPACITY)      /*    Opacity */            \
            arm_2dp_cccn888_fill_colour_with_horizontal_line_mask_and_opacity(              \
                                  NULL,                                         \
                                 (__TARGET_ADDR),                               \
                                 (__REGION_ADDR),                               \
                                 (__MASK_ADDR),                                 \
                                 (arm_2d_color_cccn888_t){(__COLOUR).tValue},    \
                                 (__OPACITY)}

/*============================ TYPES =========================================*/

/*!
 * \brief control block for colour-filling-with-mask operations
 * \note arm_2d_op_fill_cl_l_msk_t inherits from arm_2d_op_msk_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_l_msk_t {

    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< source tile
    }Source;
    uint32_t wMode;

    /* derived part */
    struct {
        const arm_2d_tile_t     *ptSourceSide;          //!< source side mask
        const arm_2d_tile_t     *ptTargetSide;          //!< target side mask
    } Mask;

    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };

    arm_2d_tile_t tDummySource;

} arm_2d_op_fill_cl_l_msk_t;

/*!
 * \brief control block for colour-filling-with-mask operations
 * \note arm_2d_op_fill_cl_l_msk_t inherits from arm_2d_op_msk_t explicitly 
 */
typedef struct arm_2d_op_fill_cl_l_msk_opc_t {

    inherit(arm_2d_op_core_t);
    struct {
        const arm_2d_tile_t     *ptTile;                //!< target tile
        const arm_2d_region_t   *ptRegion;              //!< target region
    } Target;
    struct {
        const arm_2d_tile_t     *ptTile;                //!< source tile
    }Source;
    uint32_t wMode;

    /* derived part */
    struct {
        const arm_2d_tile_t     *ptSourceSide;          //!< source side mask
        const arm_2d_tile_t     *ptTargetSide;          //!< target side mask
    } Mask;

    union {
        uint8_t  chColour;                      //!< 8bit key colour
        uint16_t hwColour;                      //!< 16bit key colour
        uint32_t wColour;                       //!< 32bit key colour
    };

    arm_2d_tile_t tDummySource;

    uint8_t chOpacity;

} arm_2d_op_fill_cl_l_msk_opc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_horizontal_line_mask(
                                        arm_2d_op_fill_cl_l_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_gray8_t tColour);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_horizontal_line_mask(
                                        arm_2d_op_fill_cl_l_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_rgb565_t tColour);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_horizontal_line_mask(
                                        arm_2d_op_fill_cl_l_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_cccn888_t tColour);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_horizontal_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_l_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_horizontal_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_l_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity);

extern
ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_horizontal_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_l_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_FILL_COLOUR_WITH_HORIZONTAL_LINE_MASK_H__ */