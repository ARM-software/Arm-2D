/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
 * Title:        __arm_2d_fill_colour_with_vertical_line_mask.c
 * Description:  APIs for colour-filling-with-vertical-line-mask
 *
 * $Date:        21. May 2024
 * $Revision:    V.1.0.3
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

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
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wswitch-enum"
#   pragma clang diagnostic ignored "-Wswitch"
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
/*============================ IMPLEMENTATION ================================*/

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_vertical_line_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_gray8_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.chColour = tColour.tValue;
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        pchTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        pchTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8,
                __arm_2d_gray8_sw_repeat_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_GRAY8),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_vertical_line_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_rgb565_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.hwColour = tColour.tValue;
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        phwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        phwTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565,
                __arm_2d_rgb565_sw_repeat_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_RGB565),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_vertical_line_mask(
                                        arm_2d_op_fill_cl_msk_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_cccn888_t tColour)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.wColour = tColour.tValue;
    

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        pwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        pwTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888,
                __arm_2d_cccn888_sw_repeat_colour_filling_with_vertical_line_mask);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_CCCN888),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_gray8_fill_colour_with_vertical_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_gray8_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.chColour = tColour.tValue;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*pchMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        pchTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_GRAY8(&chColour, pchTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        pchTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8,
                __arm_2d_gray8_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_GRAY8),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_rgb565_fill_colour_with_vertical_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_rgb565_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.hwColour = tColour.tValue;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        phwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_RGB565(&hwColour, phwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        phwTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565,
                __arm_2d_rgb565_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_RGB565),
        },
    },
};

/*
 * the Frontend API
 */

ARM_NONNULL(2,4)
arm_fsm_rt_t arm_2dp_cccn888_fill_colour_with_vertical_line_mask_and_opacity(
                                        arm_2d_op_fill_cl_msk_opc_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_tile_t *ptLineMask,
                                        arm_2d_color_cccn888_t tColour,
                                        uint8_t chOpacity)
{
    assert(NULL != ptTarget);
    assert(NULL != ptLineMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptLineMask, 
                                __ARM_2D_MASK_ALLOW_A8
                        #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                            |   __ARM_2D_MASK_ALLOW_8in32
                        #endif
                            )) {
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_PARAM;
    }

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888;

    this.Target.ptTile = ptTarget;
    this.Target.ptRegion = ptRegion;

    this.Mask.ptTile = ptLineMask;

    this.wMode = ARM_2D_CP_MODE_FILL;
    this.wColour = tColour.tValue;
    this.chOpacity = chOpacity;

    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);

}


/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pchLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        pwLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

/* default low level implementation */
__WEAK
void __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pchMask = pchLineMask;
        } else {
            pchMask += iMaskStride;
        }

        pwTarget += iTargetStride;
    }
}

__WEAK
void __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    int_fast16_t iMaskHeight = ptMaskSize->iHeight;

    int_fast16_t iMaskY = iMaskHeight;
    uint32_t *pwMask = pwLineMask;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        for (int_fast16_t x = 0; x < iWidth; x++) {
            __ARM_2D_PIXEL_BLENDING_CCCN888(&wColour, pwTargetLine++, hwAlpha);
        }

        /* maintain mask line */
        if (0 == --iMaskY) {
            iMaskY = iMaskHeight;
            pwMask = pwLineMask;
        } else {
            pwMask += iMaskStride;
        }

        pwTarget += iTargetStride;
    }
}

/*
 * OPCODE Low Level Implementation Entries
 */
__WEAK
def_low_lv_io(  __ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

__WEAK
def_low_lv_io(  __ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity);      /* Default SW Implementation */

/*
 * OPCODE
 */
const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasTarget     = true,
            .bHasSource     = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex      = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY,
        
        .LowLevelIO = {
            .ptCopyLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888),
            .ptFillLike = ref_low_lv_io(__ARM_2D_IO_REPEAT_FILL_COLOUR_WITH_VERTICAL_LINE_MASK_AND_OPACITY_CCCN888),
        },
    },
};

arm_fsm_rt_t __arm_2d_gray8_sw_repeat_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.chColour);
    #endif
    } else {
        __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.chColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.chColour);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.chColour);
    }
    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_gray8_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_repeat_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.chColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.chColour,
                                                this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_8BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_gray8_sw_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.chColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.chColour,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_repeat_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.hwColour);
    #endif
    } else {
        __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.hwColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.hwColour);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.hwColour);
    }
    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_rgb565_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_repeat_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.hwColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.hwColour,
                                                this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_16BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_rgb565_sw_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.hwColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.hwColour,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_repeat_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.wColour);
    #endif
    } else {
        __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.wColour);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_vertical_line_mask(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.wColour);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.wColour);
    }
    return arm_fsm_rt_cpl;
}


arm_fsm_rt_t __arm_2d_cccn888_sw_repeat_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_repeat_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.wColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tFill.tTarget.pBuffer,
                                                ptTask->Param.tFill.tTarget.iStride,
                                            &(ptTask->Param.tFill.tTarget.tValidRegion.tSize),
                                                ptTask->Param.tFill.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tFill.tSource.iStride,  /* mask stride */
                                            &(ptTask->Param.tFill.tSource.tValidRegion.tSize),
                                                this.wColour,
                                                this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_vertical_line_mask_and_opacity(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opc_t, ptTask->ptOP);

    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    if (255 == this.chOpacity) {
        return __arm_2d_cccn888_sw_colour_filling_with_vertical_line_mask(ptTask);
    }

    if (ARM_2D_CHANNEL_8in32 == ptTask->Param.tCopy.tSource.tColour.chScheme) {
    
    #if !__ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        return (arm_fsm_rt_t)ARM_2D_ERR_UNSUPPORTED_COLOUR;
    #else
        __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.wColour,
                                                this.chOpacity);
    #endif
    } else {
        __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask_and_opacity(
                                                ptTask->Param.tCopy.tTarget.pBuffer,
                                                ptTask->Param.tCopy.tTarget.iStride,
                                                ptTask->Param.tCopy.tSource.pBuffer,    /* mask */
                                                ptTask->Param.tCopy.tSource.iStride,  /* mask stride */
                                                &(ptTask->Param.tCopy.tCopySize),
                                                this.wColour,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

#ifdef   __cplusplus
}
#endif

#endif
