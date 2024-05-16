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
 * Title:        __arm_2d_fill_colour_with_horizontal_line_mask.c
 * Description:  APIs for colour-filling-with-horizontal-line-mask
 *
 * $Date:        17. April 2024
 * $Revision:    V.1.0.1
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


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_blend_gray8(uint16x8_t vtrgt, uint16_t vChColour, uint16x8_t opa)

{
    uint16x8_t      vTrans = 256 - opa;

    return (vmulq(vTrans, vChColour) + vmulq(vtrgt, opa)) >> 8;
}




__STATIC_FORCEINLINE
uint16x8_t __arm_2d_blend_rgb565(uint16x8_t vtrgt, __arm_2d_color_fast_rgb_t * RGB, uint16x8_t opa)
{
    uint16x8_t      vTrans = 256 - opa;
    uint16x8_t      vRtgt, vGtgt, vBtgt;

    __arm_2d_rgb565_unpack_single_vec(vtrgt, &vRtgt, &vGtgt, &vBtgt);

    vRtgt = (vmulq(vTrans, RGB->R) + vmulq(vRtgt, opa)) >> 8;
    vGtgt = (vmulq(vTrans, RGB->G) + vmulq(vGtgt, opa)) >> 8;
    vBtgt = (vmulq(vTrans, RGB->B) + vmulq(vBtgt, opa)) >> 8;

    return __arm_2d_rgb565_pack_single_vec(vRtgt, vGtgt, vBtgt);
}


__STATIC_FORCEINLINE
uint16x8_t __arm_2d_blend_cccn888(uint16x8_t       vTrg, uint16x8_t vChColour, uint16x8_t opa)
{
    uint16x8_t      vTrans = 256 - opa;

    return (vTrg * opa + vChColour * vTrans) >> 8;
}

__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_horizontal_line_mask)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint8_t *pchTargetLine = pchTarget;
        int32_t  blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_horizontal_line_chn_mask)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint8_t *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      vStride4Offs = vidupq_wb_u16(&incr, 4);


        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t*)pwMask, vStride4Offs, tailPred);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);
            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            vStride4Offs = vidupq_wb_u16(&incr, 4);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pchTarget += iTargetStride;
    }
}



__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_repeat_fill_colour_with_horizontal_line_mask)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchLineMask;
        uint8_t        *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}

__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_repeat_fill_colour_with_horizontal_line_chn_mask)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = (uint8_t *)pwLineMask;
        uint8_t        *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}






__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_horizontal_line_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_u16(pchMask);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_horizontal_line_chn_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint16_t *phwTargetLine = phwTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      vStride4Offs = vidupq_wb_u16(&incr, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t*)pwMask, vStride4Offs, tailPred);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            vStride4Offs = vidupq_wb_u16(&incr, 4);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        phwTarget += iTargetStride;
    }
}



__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_repeat_fill_colour_with_horizontal_line_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_u16(pchMask, mskIdxWrp);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}




__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_repeat_fill_colour_with_horizontal_line_chn_mask)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = (uint8_t *)pwLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_u16(pchMask, mskIdxWrp);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}





__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_horizontal_line_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_horizontal_line_chn_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 4, 4, 4, 4 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint32_t *pwTargetLine = pwTarget;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t *)pwMask, offset, 0x3f3f);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwMask += 2;
            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}



__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_repeat_fill_colour_with_horizontal_line_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;
        uint16x8_t      mskIdxWrp = offset;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, 0x3f3f);
            mskIdxWrp = mskIdxWrp + 2;
            mskIdxWrp = vsubq_m_n_u16(mskIdxWrp, mskIdxWrp, ptMaskSize->iWidth, vcmpcsq(mskIdxWrp, ptMaskSize->iWidth));


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}





__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_repeat_fill_colour_with_horizontal_line_chn_mask)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    uint16x8_t      offset = { 0, 0, 0, 0, 4, 4, 4, 4 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = (uint8_t *)pwLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;
        uint16x8_t      mskIdxWrp = offset;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, 0x3f3f);
            mskIdxWrp = mskIdxWrp + 8;
            mskIdxWrp = vsubq_m_n_u16(mskIdxWrp, mskIdxWrp, maskWidthInByte, vcmpcsq(mskIdxWrp, maskWidthInByte));


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}







__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint8_t *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint8_t *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      vStride4Offs = vidupq_wb_u16(&incr, 4);


        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t*)pwMask, vStride4Offs, tailPred);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);
            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            vStride4Offs = vidupq_wb_u16(&incr, 4);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pchTarget += iTargetStride;
    }
}



__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_repeat_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchLineMask;
        uint8_t        *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_gray8_repeat_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint8_t *__RESTRICT pchTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint8_t chColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = (uint8_t *)pwLineMask;
        uint8_t        *pchTargetLine = pchTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_m_n_u16(vHwAlpha, 255, tailPred));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);
            pchTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        pchTarget += iTargetStride;
    }
}





__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_u16(pchMask);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint16_t *phwTargetLine = phwTarget;
        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      vStride4Offs = vidupq_wb_u16(&incr, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t*)pwMask, vStride4Offs, tailPred);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            vStride4Offs = vidupq_wb_u16(&incr, 4);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        phwTarget += iTargetStride;
    }
}



__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_repeat_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_u16(pchMask, mskIdxWrp);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  ptMaskSize->iWidth, 1);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}




__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_rgb565_repeat_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint16_t *__RESTRICT phwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint16_t hwColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = (uint8_t *)pwLineMask;
        uint16_t *phwTargetLine = phwTarget;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = 0;
        uint16x8_t      mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_u16(pchMask, mskIdxWrp);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q(phwTargetLine);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            mskIdxWrp = viwdupq_wb_u16(&incr,  maskWidthInByte, 4);
            phwTargetLine += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);
        phwTarget += iTargetStride;
    }
}






__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptCopySize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 4, 4, 4, 4 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *pwMask = pwLineMask;
        uint32_t *pwTargetLine = pwTarget;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16((uint8_t *)pwMask, offset, 0x3f3f);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwMask += 2;
            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_repeat_fill_colour_with_horizontal_line_mask_and_opacity)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint8_t *__RESTRICT pchLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;
        uint16x8_t      mskIdxWrp = offset;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, 0x3f3f);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;
            mskIdxWrp = mskIdxWrp + 2;
            mskIdxWrp = vsubq_m_n_u16(mskIdxWrp, mskIdxWrp, ptMaskSize->iWidth, vcmpcsq(mskIdxWrp, ptMaskSize->iWidth));


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}


__OVERRIDE_WEAK void __MVE_WRAPPER( __arm_2d_impl_cccn888_repeat_fill_colour_with_horizontal_line_chn_mask_and_opacity)(
                                        uint32_t *__RESTRICT pwTarget,
                                        int16_t iTargetStride,
                                        arm_2d_size_t *__RESTRICT ptTargetSize,
                                        uint32_t *__RESTRICT pwLineMask,
                                        //int16_t iMaskStride,
                                        arm_2d_size_t *__RESTRICT ptMaskSize,
                                        uint32_t wColour,
                                        uint8_t chOpacity)
{
    int_fast16_t iHeight = ptTargetSize->iHeight;
    int_fast16_t iWidth  = ptTargetSize->iWidth;
    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    uint32_t     maskWidthInByte = ptMaskSize->iWidth * 4;

    uint16x8_t      offset = { 0, 0, 0, 0, 4, 4, 4, 4 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = (uint8_t *)pwLineMask;
        uint32_t *pwTargetLine = pwTarget;
        int32_t         blkCnt = iWidth;
        uint16x8_t      mskIdxWrp = offset;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, 0x3f3f);
            vHwAlpha = vmulq_n_u16(vHwAlpha, chOpacity) >> 8;
            mskIdxWrp = mskIdxWrp + 8;
            mskIdxWrp = vsubq_m_n_u16(mskIdxWrp, mskIdxWrp, maskWidthInByte, vcmpcsq(mskIdxWrp, maskWidthInByte));


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vHwAlpha = vpselq(vdupq_n_u16(256),vHwAlpha, vcmpeqq_n_u16(vHwAlpha, 255));
#endif
            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_u16((uint8_t*)pwTargetLine);

            vstrbq_p_u16((uint8_t*)pwTargetLine,
                __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                tailPred);

            pwTargetLine += 2;
            blkCnt -= 2;
        }
        while (blkCnt > 0);
        pwTarget += iTargetStride;
    }
}



#ifdef   __cplusplus
}
#endif

#endif