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
 * Title:        arm_2d_alpha_blending.c
 * Description:  APIs for various alpha related operations
 *
 * $Date:        04. April 2023
 * $Revision:    V.1.6.2
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__


#include "arm_2d.h"
#include "__arm_2d_impl.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
//static
//const uint8_t c_chA2MaskTable[4]  = {0, 85, 170, 255};

//static
//const uint8_t c_chA4MaskTable[16] = {0, 17, 34,  51, 68, 85, 102, 119,
//                                     136, 153, 170, 187, 204, 221, 238, 255};


/*============================ INCLUDES ======================================*/

#define __ARM_2D_COMPILATION_UNIT
#include "__arm_2d_colour_filling_with_alpha.c"


/*============================ IMPLEMENTATION ================================*/




/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/
 
 
/*! adding support with c code template */

#define __API_COLOUR        gray8
#define __API_INT_TYPE      uint8_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_GRAY8
            
#include "__arm_2d_ll_alpha_blending.inc"


#define __API_COLOUR        rgb565
#define __API_INT_TYPE      uint16_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_RGB565
#define __PATCH_ALPHA_BLENDING

#include "__arm_2d_ll_alpha_blending.inc"


/*! adding support with c code template */
#define __API_COLOUR        cccn888
#define __API_INT_TYPE      uint32_t
#define __API_PIXEL_BLENDING            __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_ll_alpha_blending.inc"


#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_ll_alpha_mask.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_ll_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      gray8
#define __API_CAFWM_INT_TYPE                    uint8_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            8
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_GRAY8

#include "__arm_2d_ll_alpha_mask.inc"



#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_ll_alpha_mask.inc"

#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_ll_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      rgb565
#define __API_CAFWM_INT_TYPE                    uint16_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            16
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_RGB565

#include "__arm_2d_ll_alpha_mask.inc"


#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888
    
#include "__arm_2d_ll_alpha_mask.inc"  
    
#define __API_CAFWM_CFG_1_HORIZONTAL_LINE       1
#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_ll_alpha_mask.inc"

#define __API_CAFWM_CFG_CHANNEL_8in32_SUPPORT   1
#define __API_CAFWM_COLOUR                      cccn888
#define __API_CAFWM_INT_TYPE                    uint32_t
#define __API_CAFWM_INT_TYPE_BIT_NUM            32
#define __API_CAFWM_PIXEL_BLENDING              __ARM_2D_PIXEL_BLENDING_CCCN888

#include "__arm_2d_ll_alpha_mask.inc"


#define __API_ABW_COLOUR                        gray8
#define __API_ABW_INT_TYPE                      uint8_t
#define __API_ABW_INT_TYPE_BIT_NUM              8
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_8BIT

#include "__arm_2d_alpha_blending_wrapper.inc"


#define __API_ABW_COLOUR                        rgb565
#define __API_ABW_INT_TYPE                      uint16_t
#define __API_ABW_INT_TYPE_BIT_NUM              16
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_16BIT

#include "__arm_2d_alpha_blending_wrapper.inc"


#define __API_ABW_COLOUR                        cccn888
#define __API_ABW_INT_TYPE                      uint32_t
#define __API_ABW_INT_TYPE_BIT_NUM              32
#define __API_ABW_COLOUR_SZ                     ARM_2D_COLOUR_SZ_32BIT

#include "__arm_2d_alpha_blending_wrapper.inc"

extern
void __arm_2d_impl_rgb565_tile_copy_with_opacity_direct(
                                        const uint16_t *phwSource,
                                        const uint16_t *phwBackground,
                                        uint16_t *phwDestination,
                                        uint32_t wPixelCount,
                                        uint_fast16_t hwRatio);

extern
void __arm_2d_impl_cccn888_tile_copy_with_opacity_direct(
                                        const uint32_t *__RESTRICT pwSource,
                                        const uint32_t *__RESTRICT pwBackground,
                                        uint32_t *pwDestination,
                                        uint32_t wPixelCount,
                                        uint_fast16_t hwRatio);


/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified masks                              *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4,5)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_masks(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified source mask                      *
 *----------------------------------------------------------------------------*/


ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_src_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptSrcMask,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptSrcMask);
    assert(NULL != ptTarget);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, ptSrcMask,
                                            ptTarget, NULL, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = ptSrcMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

#define ARM_2D_CP_MODE_ONLY                 ARM_2D_CP_MODE_COPY
#define ARM_2D_COLOUR_C8BIT                 ARM_2D_COLOUR_8BIT

#define __arm_2d_impl_gray8_src_chn_msk_copy_only               __arm_2d_impl_gray8_src_chn_msk_copy
#define __arm_2d_impl_rgb565_src_chn_msk_copy_only              __arm_2d_impl_rgb565_src_chn_msk_copy
#define __arm_2d_impl_cccn888_src_chn_msk_copy_only             __arm_2d_impl_cccn888_src_chn_msk_copy

#define __arm_2d_impl_gray8_src_msk_copy_only                   __arm_2d_impl_gray8_src_msk_copy
#define __arm_2d_impl_rgb565_src_msk_copy_only                  __arm_2d_impl_rgb565_src_msk_copy
#define __arm_2d_impl_cccn888_src_msk_copy_only                 __arm_2d_impl_cccn888_src_msk_copy


#define __arm_2d_impl_gray8_des_chn_msk_copy_only               __arm_2d_impl_gray8_des_chn_msk_copy
#define __arm_2d_impl_rgb565_des_chn_msk_copy_only              __arm_2d_impl_rgb565_des_chn_msk_copy
#define __arm_2d_impl_cccn888_des_chn_msk_copy_only             __arm_2d_impl_cccn888_des_chn_msk_copy

#define __arm_2d_impl_gray8_des_msk_copy_only                   __arm_2d_impl_gray8_des_msk_copy
#define __arm_2d_impl_rgb565_des_msk_copy_only                  __arm_2d_impl_rgb565_des_msk_copy
#define __arm_2d_impl_cccn888_des_msk_copy_only                 __arm_2d_impl_cccn888_des_msk_copy


#define __arm_2d_impl_gray8_1h_des_msk_copy_only                __arm_2d_impl_gray8_1h_des_msk_copy
#define __arm_2d_impl_rgb565_1h_des_msk_copy_only               __arm_2d_impl_rgb565_1h_des_msk_copy
#define __arm_2d_impl_cccn888_1h_des_msk_copy_only               __arm_2d_impl_cccn888_1h_des_msk_copy


#define __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy_only   __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_copy
#define __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy_only  __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_copy
#define __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy_only __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_copy

#define __arm_2d_impl_gray8_src_msk_des_chn_msk_copy_only       __arm_2d_impl_gray8_src_msk_des_chn_msk_copy
#define __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy_only      __arm_2d_impl_rgb565_src_msk_des_chn_msk_copy
#define __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy_only     __arm_2d_impl_cccn888_src_msk_des_chn_msk_copy

#define __arm_2d_impl_gray8_src_chn_msk_des_msk_copy_only       __arm_2d_impl_gray8_src_chn_msk_des_msk_copy
#define __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy_only      __arm_2d_impl_rgb565_src_chn_msk_des_msk_copy
#define __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy_only     __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy

#define __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy_only    __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_copy
#define __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy_only   __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_copy
#define __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy_only  __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_copy

#define __arm_2d_impl_gray8_src_msk_1h_des_msk_copy_only        __arm_2d_impl_gray8_src_msk_1h_des_msk_copy
#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy_only       __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy
#define __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy_only      __arm_2d_impl_cccn888_src_msk_1h_des_msk_copy

#define __arm_2d_impl_gray8_masks_copy_only                     __arm_2d_impl_gray8_masks_copy
#define __arm_2d_impl_rgb565_masks_copy_only                    __arm_2d_impl_rgb565_masks_copy
#define __arm_2d_impl_cccn888_masks_copy_only                   __arm_2d_impl_cccn888_masks_copy

#define __arm_2d_gray8_sw_tile_copy_with_src_mask_only          __arm_2d_gray8_sw_tile_copy_with_src_mask_and_only
#define __arm_2d_rgb565_sw_tile_copy_with_src_mask_only         __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_only
#define __arm_2d_cccn888_sw_tile_copy_with_src_mask_only        __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_only

#define arm_2dp_gray8_tile_copy_with_src_mask_and_only          arm_2dp_gray8_tile_copy_with_src_mask_only
#define arm_2dp_rgb565_tile_copy_with_src_mask_and_only         arm_2dp_rgb565_tile_copy_with_src_mask_only
#define arm_2dp_cccn888_tile_copy_with_src_mask_and_only        arm_2dp_cccn888_tile_copy_with_src_mask_only

#define __arm_2d_gray8_sw_tile_copy_with_des_mask_only          __arm_2d_gray8_sw_tile_copy_with_des_mask_and_only
#define __arm_2d_rgb565_sw_tile_copy_with_des_mask_only         __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_only
#define __arm_2d_cccn888_sw_tile_copy_with_des_mask_only        __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_only

#define arm_2dp_gray8_tile_copy_with_des_mask_and_only          arm_2dp_gray8_tile_copy_with_des_mask_only
#define arm_2dp_rgb565_tile_copy_with_des_mask_and_only         arm_2dp_rgb565_tile_copy_with_des_mask_only
#define arm_2dp_cccn888_tile_copy_with_des_mask_and_only        arm_2dp_cccn888_tile_copy_with_des_mask_only

#define __arm_2d_gray8_sw_tile_copy_with_masks_only          __arm_2d_gray8_sw_tile_copy_with_masks_and_only
#define __arm_2d_rgb565_sw_tile_copy_with_masks_only         __arm_2d_rgb565_sw_tile_copy_with_masks_and_only
#define __arm_2d_cccn888_sw_tile_copy_with_masks_only        __arm_2d_cccn888_sw_tile_copy_with_masks_and_only

#define arm_2dp_gray8_tile_copy_with_masks_and_only          arm_2dp_gray8_tile_copy_with_masks_only
#define arm_2dp_rgb565_tile_copy_with_masks_and_only         arm_2dp_rgb565_tile_copy_with_masks_only
#define arm_2dp_cccn888_tile_copy_with_masks_and_only        arm_2dp_cccn888_tile_copy_with_masks_only

#define __arm_2d_impl_gray8_src_chn_msk_fill_only               __arm_2d_impl_gray8_src_chn_msk_fill
#define __arm_2d_impl_rgb565_src_chn_msk_fill_only              __arm_2d_impl_rgb565_src_chn_msk_fill
#define __arm_2d_impl_cccn888_src_chn_msk_fill_only             __arm_2d_impl_cccn888_src_chn_msk_fill

#define __arm_2d_impl_gray8_src_msk_fill_only                   __arm_2d_impl_gray8_src_msk_fill
#define __arm_2d_impl_rgb565_src_msk_fill_only                  __arm_2d_impl_rgb565_src_msk_fill
#define __arm_2d_impl_cccn888_src_msk_fill_only                 __arm_2d_impl_cccn888_src_msk_fill


#define __arm_2d_impl_gray8_des_chn_msk_fill_only               __arm_2d_impl_gray8_des_chn_msk_fill
#define __arm_2d_impl_rgb565_des_chn_msk_fill_only              __arm_2d_impl_rgb565_des_chn_msk_fill
#define __arm_2d_impl_cccn888_des_chn_msk_fill_only             __arm_2d_impl_cccn888_des_chn_msk_fill

#define __arm_2d_impl_gray8_des_msk_fill_only                   __arm_2d_impl_gray8_des_msk_fill
#define __arm_2d_impl_rgb565_des_msk_fill_only                  __arm_2d_impl_rgb565_des_msk_fill
#define __arm_2d_impl_cccn888_des_msk_fill_only                 __arm_2d_impl_cccn888_des_msk_fill

#define __arm_2d_impl_gray8_1h_des_msk_fill_only                __arm_2d_impl_gray8_1h_des_msk_fill
#define __arm_2d_impl_rgb565_1h_des_msk_fill_only               __arm_2d_impl_rgb565_1h_des_msk_fill
#define __arm_2d_impl_cccn888_1h_des_msk_fill_only              __arm_2d_impl_cccn888_1h_des_msk_fill

#define __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill_only   __arm_2d_impl_gray8_src_chn_msk_des_chn_msk_fill
#define __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill_only  __arm_2d_impl_rgb565_src_chn_msk_des_chn_msk_fill
#define __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill_only __arm_2d_impl_cccn888_src_chn_msk_des_chn_msk_fill

#define __arm_2d_impl_gray8_src_msk_des_chn_msk_fill_only       __arm_2d_impl_gray8_src_msk_des_chn_msk_fill
#define __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill_only      __arm_2d_impl_rgb565_src_msk_des_chn_msk_fill
#define __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill_only     __arm_2d_impl_cccn888_src_msk_des_chn_msk_fill

#define __arm_2d_impl_gray8_src_chn_msk_des_msk_fill_only       __arm_2d_impl_gray8_src_chn_msk_des_msk_fill
#define __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill_only      __arm_2d_impl_rgb565_src_chn_msk_des_msk_fill
#define __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill_only     __arm_2d_impl_cccn888_src_chn_msk_des_msk_fill

#define __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill_only    __arm_2d_impl_gray8_src_chn_msk_1h_des_msk_fill
#define __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill_only   __arm_2d_impl_rgb565_src_chn_msk_1h_des_msk_fill
#define __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill_only  __arm_2d_impl_cccn888_src_chn_msk_1h_des_msk_fill

#define __arm_2d_impl_gray8_src_msk_1h_des_msk_fill_only        __arm_2d_impl_gray8_src_msk_1h_des_msk_fill
#define __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill_only       __arm_2d_impl_rgb565_src_msk_1h_des_msk_fill
#define __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill_only      __arm_2d_impl_cccn888_src_msk_1h_des_msk_fill

#define __arm_2d_impl_gray8_masks_fill_only                     __arm_2d_impl_gray8_masks_fill
#define __arm_2d_impl_rgb565_masks_fill_only                    __arm_2d_impl_rgb565_masks_fill
#define __arm_2d_impl_cccn888_masks_fill_only                   __arm_2d_impl_cccn888_masks_fill



#define __arm_2d_gray8_sw_tile_fill_with_src_mask_only          __arm_2d_gray8_sw_tile_fill_with_src_mask_and_only
#define __arm_2d_rgb565_sw_tile_fill_with_src_mask_only         __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_only
#define __arm_2d_cccn888_sw_tile_fill_with_src_mask_only        __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_only

#define __arm_2d_gray8_sw_tile_fill_with_des_mask_only          __arm_2d_gray8_sw_tile_fill_with_des_mask_and_only
#define __arm_2d_rgb565_sw_tile_fill_with_des_mask_only         __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_only
#define __arm_2d_cccn888_sw_tile_fill_with_des_mask_only        __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_only

#define __arm_2d_gray8_sw_tile_fill_with_masks_only          __arm_2d_gray8_sw_tile_fill_with_masks_and_only
#define __arm_2d_rgb565_sw_tile_fill_with_masks_only         __arm_2d_rgb565_sw_tile_fill_with_masks_and_only
#define __arm_2d_cccn888_sw_tile_fill_with_masks_only        __arm_2d_cccn888_sw_tile_fill_with_masks_and_only

#define arm_2dp_gray8_tile_fill_with_src_mask_and_only          arm_2dp_gray8_tile_fill_with_src_mask_only
#define arm_2dp_rgb565_tile_fill_with_src_mask_and_only         arm_2dp_rgb565_tile_fill_with_src_mask_only
#define arm_2dp_cccn888_tile_fill_with_src_mask_and_only        arm_2dp_cccn888_tile_fill_with_src_mask_only

#define arm_2dp_gray8_tile_fill_with_masks_and_only          arm_2dp_gray8_tile_fill_with_masks_only
#define arm_2dp_rgb565_tile_fill_with_masks_and_only         arm_2dp_rgb565_tile_fill_with_masks_only
#define arm_2dp_cccn888_tile_fill_with_masks_and_only        arm_2dp_cccn888_tile_fill_with_masks_only

#define __API_CMMW_COLOUR                     gray8
#define __API_CMMW_COLOUR_UPPERCASE           GRAY8
#define __API_CMMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_8BIT
#define __API_CMMW_INT_TYPE                   uint8_t

#include "__arm_2d_copy_with_mask_and_mirror_wrapper.inc"


#define __API_CMMW_COLOUR                     rgb565
#define __API_CMMW_COLOUR_UPPERCASE           RGB565
#define __API_CMMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_16BIT
#define __API_CMMW_INT_TYPE                   uint16_t

#include "__arm_2d_copy_with_mask_and_mirror_wrapper.inc"


#define __API_CMMW_COLOUR                     cccn888
#define __API_CMMW_COLOUR_UPPERCASE           CCCN888
#define __API_CMMW_COLOUR_SZ                  ARM_2D_COLOUR_SZ_32BIT
#define __API_CMMW_INT_TYPE                   uint32_t

#include "__arm_2d_copy_with_mask_and_mirror_wrapper.inc"



/*----------------------------------------------------------------------------*
 * Copy tile to destination with a specified target mask                      *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);

    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
    

}

ARM_NONNULL(2,3,4)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_des_mask(
                                        arm_2d_op_cp_msk_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_tile_t *ptDesMask,
                                        const arm_2d_region_t *ptRegion,
                                        uint32_t wMode)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);
    assert(NULL != ptDesMask);
    
    ARM_2D_IMPL(arm_2d_op_cp_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(
                                            ptSource, NULL,
                                            ptTarget, ptDesMask, wMode);
    if (tErr < 0) {
        return (arm_fsm_rt_t)tErr;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_DES_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = wMode;
    this.Mask.ptSourceSide = NULL;
    this.Mask.ptTargetSide = ptDesMask;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

/*----------------------------------------------------------------------------*
 * Copy tile to destination with specified transparency ratio (0~255)         *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_opacity(
                                     arm_2d_op_alpha_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_opacity(
                                     arm_2d_op_alpha_t *ptOP,
                                     const arm_2d_tile_t *ptSource,
                                     const arm_2d_tile_t *ptTarget,
                                     const arm_2d_region_t *ptRegion,
                                     uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_opacity( arm_2d_op_alpha_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_t, ptOP);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_OPACITY_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);
    
    if (255 == this.chRatio) {
        __arm_2d_impl_c8bit_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize);
    } else {
        __arm_2d_impl_gray8_tile_copy_opacity( 
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &(ptTask->Param.tCopy.tCopySize),
                                    this.chRatio);
    }
    
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb16_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize);
    } else {
        __arm_2d_impl_rgb565_tile_copy_opacity(
                                ptTask->Param.tCopy.tSource.pBuffer,
                                ptTask->Param.tCopy.tSource.iStride,
                                ptTask->Param.tCopy.tTarget.pBuffer,
                                ptTask->Param.tCopy.tTarget.iStride,
                                &(ptTask->Param.tCopy.tCopySize),
                                this.chRatio);
    }
    
    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);
    
    if (255 == this.chRatio) {
        __arm_2d_impl_rgb32_copy(   ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize);
    } else {
        __arm_2d_impl_cccn888_tile_copy_opacity(   
                                            ptTask->Param.tCopy.tSource.pBuffer,
                                            ptTask->Param.tCopy.tSource.iStride,
                                            ptTask->Param.tCopy.tTarget.pBuffer,
                                            ptTask->Param.tCopy.tTarget.iStride,
                                            &(ptTask->Param.tCopy.tCopySize),
                                            this.chRatio);
    }

    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Blend tile and background with a specified transparency ratio(0~255) and a *
 * specified transparency color mask                                          *
 *----------------------------------------------------------------------------*/

ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_gray8_tile_copy_with_colour_keying_and_opacity(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_gray8_t tColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.chColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_rgb565_tile_copy_with_colour_keying_and_opacity(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_rgb565_t tColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = &ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.hwColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}


ARM_NONNULL(2,3)
arm_fsm_rt_t arm_2dp_cccn888_tile_copy_with_colour_keying_and_opacity(
                                            arm_2d_op_alpha_cl_key_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptTarget,
                                            const arm_2d_region_t *ptRegion,
                                            uint_fast8_t chRatio,
                                            arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptSource);
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t);
    
    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }
    
    //memset(ptThis, 0, sizeof(*ptThis));

    OP_CORE.ptOp = & ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;
    this.Source.ptTile = ptSource;
    this.wMode = 0;
    this.chRatio = chRatio;
    this.wColour = tColour.tValue;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

arm_fsm_rt_t __arm_2d_gray8_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chRatio) {
        __arm_2d_impl_c8bit_cl_key_copy(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chColour);
    } else {
        __arm_2d_impl_gray8_tile_copy_colour_keying_opacity(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.chColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb16_cl_key_copy(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.hwColour);
    } else {
        __arm_2d_impl_rgb565_tile_copy_colour_keying_opacity(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.hwColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_tile_copy_with_colour_keying_and_opacity(
                                        __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_alpha_cl_key_t, ptTask->ptOP)
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chRatio) {
        __arm_2d_impl_rgb32_cl_key_copy(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.wColour);
    } else {
    __arm_2d_impl_cccn888_tile_copy_colour_keying_opacity(
                                    ptTask->Param.tCopy.tSource.pBuffer,
                                    ptTask->Param.tCopy.tSource.iStride,
                                    ptTask->Param.tCopy.tTarget.pBuffer,
                                    ptTask->Param.tCopy.tTarget.iStride,
                                    &ptTask->Param.tCopy.tCopySize,
                                    this.chRatio,
                                    this.wColour);
    }
    
    return arm_fsm_rt_cpl;
}

/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/


__WEAK
void __arm_2d_impl_rgb565_tile_copy_opacity(   uint16_t *__RESTRICT phwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    uint32_t iHeight = ptCopySize->iHeight;
    uint32_t iWidth  = ptCopySize->iWidth;
    
    uint16_t        ratioCompl = 256 - hwRatio;

    ARM_2D_UNUSED(ratioCompl);
    
    for (uint32_t y = 0; y < iHeight; y++) {

#if (defined (__ARM_ARCH_8_1M_MAIN__) && (__ARM_ARCH_8_1M_MAIN__ == 1))
        /* M55 NOMVE optimization */
        register unsigned loopCnt  __asm("lr");
        loopCnt = iWidth;

        __asm volatile(
            "   dls             lr, lr                                  \n"
            ".p2align 2                                                 \n"
            "1:                                                         \n"
            /* read source / target pixel  */
            "   ldrh            r4, [%[src]], #2                        \n"
            "   ldrh            r5, [%[pTarget]]                        \n"

            /* unpack R */
            "   and             r0, r4, #31                             \n"
            "   and             r1, r5, #31                             \n"

            /* unpack B */
            "   lsr             r2, r4, #11                             \n"
            "   lsr             r3, r5, #11                             \n"

            /* unpack G */
            "   ubfx            r4, r4, #5, #6                          \n"
            "   ubfx            r5, r5, #5, #6                          \n"

            /* mix */
            "   mul             r0, r0, %[ratio0]                       \n"
            "   mul             r4, r4, %[ratio0]                       \n"
            "   mul             r2, r2, %[ratio0]                       \n"

            "   mla             r0, %[ratio1], r1, r0                   \n"
            "   mla             r1, %[ratio1], r5, r4                   \n"

            /* pack R */
            "   ubfx            r0, r0, #8, #5                          \n"
            /* shift & mask G */
            "   and             r1, %[cst], r1, lsr #3                  \n"
            /* MLA moved here to fill stall */
            "   mla             r5, %[ratio1], r3, r2                   \n"

            /* pack G */
            "   add             r0, r0, r1                              \n"
            /* pack R */
            "   bic             r1, r5, #255                            \n"
            "   orr             r0, r0, r1, lsl #3                      \n"

            "   strh            r0, [%[pTarget]], #2                    \n"
            "   le              lr, 1b                                  \n"

            : [src] "+r" (phwSourceBase), [pTarget] "+r" (phwTargetBase),
              [cnt] "+r" (loopCnt)
            : [ratio1] "r" ((256 - (uint_fast16_t)hwRatio) ),
              [ratio0] "r" (hwRatio), [cst] "r" (0x7e0 /* mask G */)
            : "r0", "r1", "r2", "r3",
              "r4", "r5", "memory"
        );
#else
        
        for (uint32_t x = 0; x < iWidth; x++) {
            __arm_2d_color_fast_rgb_t srcPix, targetPix;

            __arm_2d_rgb565_unpack(*phwSourceBase++, &srcPix);
            __arm_2d_rgb565_unpack(*phwTargetBase, &targetPix);

            for (int i = 0; i < 3; i++) {
                uint16_t        tmp =
                    (uint16_t) (srcPix.BGRA[i] * hwRatio) +
                    (targetPix.BGRA[i] * ratioCompl);
                targetPix.BGRA[i] = (uint16_t) (tmp >> 8);
            }
            /* pack merged stream */
            *phwTargetBase++ = __arm_2d_rgb565_pack(&targetPix);
        }
#endif
        phwSourceBase += (iSourceStride - iWidth);
        phwTargetBase += (iTargetStride - iWidth);
    }
}

/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_GRAY8,
                __arm_2d_gray8_sw_tile_copy_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_RGB888, 
                __arm_2d_cccn888_sw_tile_copy_with_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks_and_x_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks_and_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_masks_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_masks_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_masks_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_masks_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_masks_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_masks_and_xy_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask_only);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_x_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_x_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_y_mirror);



__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_src_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_src_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_src_mask_and_xy_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_src_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_src_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_src_mask_and_xy_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask_only);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask_only);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_x_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_x_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_x_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_y_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_y_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_des_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_des_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_copy_with_des_mask_and_xy_mirror);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_GRAY8, 
                __arm_2d_gray8_sw_tile_fill_with_des_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_RGB565, 
                __arm_2d_rgb565_sw_tile_fill_with_des_mask_and_xy_mirror);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_CCCN888, 
                __arm_2d_cccn888_sw_tile_fill_with_des_mask_and_xy_mirror);


__WEAK
def_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_tile_copy_with_colour_keying_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_tile_copy_with_colour_keying_and_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB888, 
                __arm_2d_cccn888_sw_tile_copy_with_colour_keying_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_GRAY8, 
                __arm_2d_gray8_sw_colour_filling_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_RGB565, 
                __arm_2d_rgb565_sw_colour_filling_with_opacity);
__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_RGB888, 
                __arm_2d_cccn888_sw_colour_filling_with_opacity);


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_OPACITY_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_OPACITY_RGB888),
            .ptFillLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB565),
            .ptFillLike = NULL, 
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_COLOUR_KEYING_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_TILE_COPY_WITH_COLOUR_KEYING_AND_OPACITY_RGB888),
            .ptFillLike = NULL,
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_GRAY8),
        },
    },
};    

const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_RGB565),
        },
    },
};
    
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_OPACITY_RGB888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = false,
            .bHasTarget     = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_OPACITY,
        
        .LowLevelIO = {
            .ptTileProcessLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_OPACITY_RGB888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_ONLY_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_ONLY_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_X_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_X_MIRROR_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_Y_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_Y_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_MASKS_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_MASKS_AND_XY_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_MASKS_AND_XY_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_ONLY_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_ONLY_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_X_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_X_MIRROR_CCCN888),
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_Y_MIRROR_CCCN888),
        },
    },
};




const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = true,
            .bHasDesMask    = false,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_SOURCE_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_SRC_MASK_AND_XY_MIRROR_CCCN888),
        },
    },
};





const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_ONLY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_ONLY_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_ONLY_CCCN888),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_X_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_X_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_X_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_X_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_X_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_X_MIRROR_CCCN888),
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_Y_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_Y_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_Y_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_Y_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_Y_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_Y_MIRROR_CCCN888),
        },
    },
};




const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_COPY_WITH_DES_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_COPY_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_CCCN888),
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_XY_MIRROR_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_GRAY8),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_XY_MIRROR_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_RGB565),
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TILE_FILL_WITH_DES_MASK_AND_XY_MIRROR_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource     = true,
            .bHasTarget     = true,
            .bHasSrcMask    = false,
            .bHasDesMask    = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_WITH_TARGET_MASK_AND_XY_MIRROR,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_COPY_WITH_DES_MASK_AND_XY_MIRROR_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_FILL_WITH_DES_MASK_AND_XY_MIRROR_CCCN888),
        },
    },
};




#ifdef   __cplusplus
}
#endif
