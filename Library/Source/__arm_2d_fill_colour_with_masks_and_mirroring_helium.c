/*
 * Copyright (C) 2010-2024 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_fill_colour_with_masks_and_mirroring_helium.c
 * Description:  APIs for colour-filling-with-masks-and-mirroring
 *
 * $Date:        10. Dec 2025
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores with Helium
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif

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
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

/*
 * Target Colour: GRAY8
 */
__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_gray8_colour_filling_with_masks_only)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;
            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pchTarget += (iTargetStride);
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_gray8_colour_filling_with_masks_and_opacity_only)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pchTarget += (iTargetStride);
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_and_x_mirror)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pchTarget += (iTargetStride);
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_x_mirror_and_opacity)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pchTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_and_y_mirror)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pchTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_y_mirror_and_opacity)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            pchMask += 8;
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pchTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_and_xy_mirror)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(   pchTargetLine,
                            __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                            tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pchTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_gray8_colour_filling_with_masks_xy_mirror_and_opacity)(
                            uint8_t *__RESTRICT pchTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint8_t chColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint8_t        *pchTargetLine = pchTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vldrbq_z_u16(pchTargetLine, tailPred);

            vstrbq_p_u16(pchTargetLine,
                        __arm_2d_blend_gray8(vtrgt, chColour, vHwAlpha),
                        tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pchTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

/*
 * Target Colour: RGB565
 */

__OVERRIDE_WEAK 
void __MVE_WRAPPER(__arm_2d_impl_rgb565_colour_filling_with_masks_only)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_rgb565_colour_filling_with_masks_and_opacity_only)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour,
                            uint16_t hwOpacity)
{

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }

}


__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_and_x_mirror)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_x_mirror_and_opacity)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_and_y_mirror)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_y_mirror_and_opacity)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_z_u16(pchMask, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                tailPred);

            pchMask += 8;
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}


__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_and_xy_mirror)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_rgb565_colour_filling_with_masks_xy_mirror_and_opacity)(
                            uint16_t *__RESTRICT phwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint16_t hwColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);


    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint16_t *phwTargetLine = phwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint32_t        incr = iWidth-1;
        uint16x8_t      mskIdxWrp = vddupq_wb_u16(&incr, 1);

        do {
            mve_pred16_t    tailPred = vctp16q(blkCnt);

            uint16x8_t      vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, mskIdxWrp, tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_z_u16(pchTargetMaskLine, tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vtrgt = vld1q_z_u16(phwTargetLine, tailPred);

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(vtrgt, &ColorRGB, vHwAlpha),
                    tailPred);

            mskIdxWrp = vddupq_wb_u16(&incr, 1);
            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        phwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

/*
 * Target Colour: CCCN888
 */
__OVERRIDE_WEAK 
void __MVE_WRAPPER(__arm_2d_impl_cccn888_colour_filling_with_masks_only)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
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

        uint8_t *pchMask = pchSourceMask;
        uint32_t *pwTargetLine = pwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset, 0x3f3f & tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }

}

__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_cccn888_colour_filling_with_masks_and_opacity_only)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t *pchMask = pchSourceMask;
        uint32_t *pwTargetLine = pwTarget;
        uint8_t *pchTargetMaskLine = pchTargetMask;

        int32_t blkCnt = iWidth;

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset, 0x3f3f & tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}


__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_and_x_mirror)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 1, 1, 1, 1, 0, 0, 0, 0 };
    uint16x8_t      offset_normal = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint16x8_t      offsetCur = vaddq_n_u16(offset, (iWidth-2));

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offsetCur, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset_normal, 0x3f3f & tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            offsetCur = offsetCur - 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}


__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_x_mirror_and_opacity)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);

    uint16x8_t      offset = { 1, 1, 1, 1, 0, 0, 0, 0 };
    uint16x8_t      offset_normal = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint16x8_t      offsetCur = vaddq_n_u16(offset, (iWidth-2));

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offsetCur, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset_normal, 0x3f3f & tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            offsetCur = offsetCur - 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;

            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask += iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_and_y_mirror)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;


        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset, 0x3f3f & tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_y_mirror_and_opacity)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    uint16x8_t      offset = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;


        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offset, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset, 0x3f3f & tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            pchMask += 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_and_xy_mirror)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    uint16x8_t      offset = { 1, 1, 1, 1, 0, 0, 0, 0 };
    uint16x8_t      offset_normal = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint16x8_t      offsetCur = vaddq_n_u16(offset, (iWidth-2));

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offsetCur, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset_normal, 0x3f3f & tailPred), 
                            vHwAlpha);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            offsetCur = offsetCur - 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        }  while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

__OVERRIDE_WEAK 
void __MVE_WRAPPER( __arm_2d_impl_cccn888_colour_filling_with_masks_xy_mirror_and_opacity)(
                            uint32_t *__RESTRICT pwTarget,
                            int16_t iTargetStride,
                            uint8_t *__RESTRICT pchSourceMask,
                            int16_t iSourceMaskStride,
                            uint8_t *__RESTRICT pchTargetMask,
                            int16_t iTargetMaskStride,
                            arm_2d_size_t *__RESTRICT ptCopySize,
                            uint32_t wColour,
                            uint16_t hwOpacity)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    uint16x8_t vwColour;
    uint32_t wColourArr[2] = {wColour, wColour};
    vwColour = vldrbq_u16((uint8_t*)wColourArr);
    pchSourceMask += iSourceMaskStride * (iHeight - 1);

    uint16x8_t      offset = { 1, 1, 1, 1, 0, 0, 0, 0 };
    uint16x8_t      offset_normal = { 0, 0, 0, 0, 1, 1, 1, 1 };

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint8_t        *pchMask = pchSourceMask;
        uint32_t       *pwTargetLine = pwTarget;
        uint8_t        *pchTargetMaskLine = pchTargetMask;

        int32_t         blkCnt = iWidth;
        uint16x8_t      offsetCur = vaddq_n_u16(offset, (iWidth-2));

        do {
            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
             */
            uint16x8_t vHwAlpha = vldrbq_gather_offset_z_u16(pchMask, offsetCur, 0x3f3f & tailPred);

            vHwAlpha = __arm_2d_scale_alpha_mask_n_opa(
                            vldrbq_gather_offset_z_u16(pchTargetMaskLine, offset_normal, 0x3f3f & tailPred), 
                            vHwAlpha,
                            hwOpacity);

            vHwAlpha=  256 - vHwAlpha;

            uint16x8_t      vTrg = vldrbq_z_u16((uint8_t*)pwTargetLine, tailPred);

            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888(vTrg, vwColour, vHwAlpha),
                            tailPred);

            offsetCur = offsetCur - 2;
            pwTargetLine += 2;
            pchTargetMaskLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pchSourceMask -= iSourceMaskStride;
        pwTarget += iTargetStride;
        pchTargetMask += iTargetMaskStride;
    }
}

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */
