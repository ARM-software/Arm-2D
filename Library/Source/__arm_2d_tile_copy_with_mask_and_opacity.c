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
 * Title:        __arm_2d_tile_copy_with_mask_and_opacity.c
 * Description:  APIs tile copy with mask and opacity
 *
 * $Date:        30. April 2024
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores
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

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint8_t *__RESTRICT pchTargetLine = pchTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_GRAY8(  pwSourceLine++, 
                                                        pchTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint16_t *__RESTRICT phwTargetLine = phwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_RGB565(  pwSourceLine++, 
                                                        phwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    hwRatio += (hwRatio == 255);

    uint16_t hwRatioCompl = 256 - hwRatio;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwRatioCompl);

        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }

}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*pchSourceMaskLine++) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask_and_opacity(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    /* preprocess the opacity */
    hwOpacity += (hwOpacity == 255);
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (hwOpacity * (*(uint8_t *)(pwSourceMaskLine++)) >> 8);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}

__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint8_t * __RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint8_t *pchSourceMask = pchSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint8_t *__RESTRICT pchSourceMaskLine = pchSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*pchSourceMaskLine++);

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pchSourceMask = pchSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pchSourceMask += iSourceMaskStride;
        }
    }
}


__WEAK
void __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,

                                    uint32_t * __RESTRICT pwSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t * __RESTRICT ptSourceMaskSize,

                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,

                                    arm_2d_size_t *__RESTRICT ptCopySize)
{
    /* handle source mask */
    assert(ptSourceMaskSize->iWidth >= ptCopySize->iWidth);
    int_fast16_t iSourceMaskY = 0;
    int_fast16_t iSourceMaskHeight = ptSourceMaskSize->iHeight;
    uint32_t *pwSourceMask = pwSourceMaskBase;

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;
    
    for (int_fast16_t y = 0; y < iHeight; y++) {

        uint32_t *__RESTRICT pwSourceLine = pwSourceBase;
        uint32_t *__RESTRICT pwTargetLine = pwTargetBase;

        uint32_t *__RESTRICT pwSourceMaskLine = pwSourceMask;

        for (int_fast16_t x = 0; x < iWidth; x++) {

            uint16_t hwTransparency = 256 - (*(uint8_t *)(pwSourceMaskLine++));

            __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_CCCN888(  pwSourceLine++, 
                                                        pwTargetLine++,
                                                        hwTransparency);
        }

        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;

        /* source mask rolling */
        if (++iSourceMaskY >= iSourceMaskHeight) {
            pwSourceMask = pwSourceMaskBase;
            iSourceMaskY = 0;
        } else {
            pwSourceMask += iSourceMaskStride;
        }
    }
}



#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */