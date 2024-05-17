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
 * Title:        __arm_2d_fill_colour_with_vertical_line_mask_helium.c
 * Description:  APIs for colour-filling-with-vertical-line-mask
 *
 * $Date:        16. May 2024
 * $Revision:    V.1.0.0
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




/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask)(
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
        uint8_t        *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask)(
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
        int32_t         blkCnt = iWidth;
        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask)(
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
        int32_t         blkCnt = iWidth;
        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {
        uint32_t *pwMask = pwLineMask;
        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

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


/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);


        pchLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    uint8_t *pchMask = pchLineMask;
    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

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




/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif
        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);

        pchLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);


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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)));
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);


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




/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_vertical_line_mask_and_opacity)(
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

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);


        pchLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        pchTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_mask_and_opacity)(
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

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);


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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_gray8_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint8_t *pchTargetLine = pchTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_n_gray8(vtrgt, chColour, hwAlpha),
                tailPred);

            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);


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




/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_vertical_line_mask_and_opacity)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        phwTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_mask_and_opacity)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_rgb565_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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
    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    int_fast16_t iMaskY = iMaskHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint16_t *phwTargetLine = phwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_n_rgb565(vtrgt, &ColorRGB, hwAlpha),
                tailPred);

            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
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




/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_vertical_line_mask_and_opacity)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchLineMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);

        pchLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwLineMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);

        pwLineMask += iMaskStride;
        pwTarget += iTargetStride;
    }
}

/* default low level implementation */
__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_mask_and_opacity)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*pchMask) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
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

__OVERRIDE_WEAK void __MVE_WRAPPER(  __arm_2d_impl_cccn888_repeat_fill_colour_with_vertical_line_chn_mask_and_opacity)(
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
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    /* clear alpha */
    vwColour[3] = vwColour[7] = 0;


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint32_t *pwTargetLine = pwTarget;

        uint16_t hwAlpha = 256 - ((*(uint8_t *)(pwMask)) * (uint16_t)chOpacity >> 8);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwAlpha -= (hwAlpha == 1);
#endif

        int32_t         blkCnt = iWidth;
        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_n_cccn888(vTrg, vwColour, hwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);

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

#ifdef   __cplusplus
}
#endif

#endif