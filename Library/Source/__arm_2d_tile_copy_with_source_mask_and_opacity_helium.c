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
 * Title:        __arm_2d_tile_copy_with_mask_and_opacity_helium.c
 * Description:  APIs tile copy with mask and opacity
 *
 * $Date:        25. August 2025
 * $Revision:    V.1.0.0
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


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_ccca8888_tile_copy_to_gray8_with_opacity)(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint8_t *__RESTRICT pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    int16_t iWidth = ptCopySize->iWidth;
    int16_t iHeight = ptCopySize->iHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_gray8_with_opacity(   pwSourceBase, 
                                                                pchTargetBase, 
                                                                iWidth, 
                                                                hwOpacity);

        pwSourceBase += iSourceStride;
        pchTargetBase += iTargetStride;
    }
}



__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_gray8_with_src_mask_and_opacity(
                                                                pwSourceBase,
                                                                pchSourceMask,
                                                                pchTargetBase,
                                                                iWidth,
                                                                hwOpacity);

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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_gray8_with_src_chn_mask_and_opacity(
                                                                pwSourceBase,
                                                                pwSourceMask,
                                                                pchTargetBase,
                                                                iWidth,
                                                                hwOpacity);

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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_mask)(
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

        __arm_2d_helium_ccca8888_blend_to_gray8_with_src_mask(  pwSourceBase,
                                                                pchSourceMask,
                                                                pchTargetBase,
                                                                iWidth);

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


__OVERRIDE_WEAK
void __MVE_WRAPPER(__arm_2d_impl_ccca8888_tile_copy_to_gray8_with_src_chn_mask)(
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

        __arm_2d_helium_ccca8888_blend_to_gray8_with_src_chn_mask(
                                                                pwSourceBase,
                                                                pwSourceMask,
                                                                pchTargetBase,
                                                                iWidth);

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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_opacity)(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint16_t *__RESTRICT phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{

    hwOpacity += (hwOpacity == 255);
    int16_t iWidth = ptCopySize->iWidth;
    int16_t iHeight = ptCopySize->iHeight;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_rgb565_with_opacity(  pwSourceBase,
                                                                phwTargetBase,
                                                                iWidth,
                                                                hwOpacity);


        pwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }

}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_mask_and_opacity(  
                                                                pwSourceBase,
                                                                pchSourceMask,
                                                                phwTargetBase,
                                                                iWidth,
                                                                hwOpacity);
                                                                
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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask_and_opacity)(
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
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_chn_mask_and_opacity(  
                                                                pwSourceBase,
                                                                pwSourceMask,
                                                                phwTargetBase,
                                                                iWidth,
                                                                hwOpacity);

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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask)(
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

        __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_mask( pwSourceBase,
                                                                pchSourceMask,
                                                                phwTargetBase,
                                                                iWidth);
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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_chn_mask)(
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
    uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_chn_mask( pwSourceBase,
                                                                pwSourceMask,
                                                                phwTargetBase,
                                                                iWidth);
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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_opacity)(
                                    uint32_t *__RESTRICT pwSourceBase,
                                    int16_t iSourceStride,
                                    uint32_t *__RESTRICT pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptCopySize,
                                    uint_fast16_t hwOpacity)
{
    hwOpacity += (hwOpacity == 255);
    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_cccn888_with_opacity( pwSourceBase,
                                                                pwTargetBase,
                                                                iWidth,
                                                                hwOpacity);
                                                                
        pwSourceBase += iSourceStride;
        pwTargetBase += iTargetStride;
    }

}



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask_and_opacity)(
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
    uint8_t *pchSourceMask = pchSourceMaskBase;  /// useless

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_mask_and_opacity( 
                                                                pwSourceBase,
                                                                pchSourceMask,
                                                                pwTargetBase,
                                                                iWidth,
                                                                hwOpacity);

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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_chn_mask_and_opacity( 
                                                                pwSourceBase,
                                                                pwSourceMask,
                                                                pwTargetBase,
                                                                iWidth,
                                                                hwOpacity);
                                                                
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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask)(
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
    uint8_t *pchSourceMask = pchSourceMaskBase;  /// useless

    int_fast16_t iHeight = ptCopySize->iHeight;
    int_fast16_t iWidth  = ptCopySize->iWidth;

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_mask(pwSourceBase,
                                                                pchSourceMask,
                                                                pwTargetBase,
                                                                iWidth);

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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_chn_mask)(
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

        __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_chn_mask(
                                                                pwSourceBase,
                                                                pwSourceMask,
                                                                pwTargetBase,
                                                                iWidth);

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



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_tile_copy_with_src_mask_and_opacity)(
                                    uint8_t *__RESTRICT pchSourceBase,
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_gray8_blend_with_src_mask_and_opacity(
                pchSourceBase,
                pchSourceMask,
                pchTargetBase,
                iWidth,
                hwOpacity);

        pchSourceBase += iSourceStride;
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


__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_gray8_tile_copy_with_src_chn_mask_and_opacity)(
                                    uint8_t *__RESTRICT pchSourceBase,
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_gray8_blend_with_src_chn_mask_and_opacity(
                pchSourceBase,
                pwSourceMask,
                pchTargetBase,
                iWidth,
                hwOpacity);

        pchSourceBase += iSourceStride;
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



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_tile_copy_with_src_mask_and_opacity)(
                                    uint16_t *__RESTRICT phwSourceBase,
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_rgb565_blend_with_src_mask_and_opacity(
                phwSourceBase,
                pchSourceMask,
                phwTargetBase,
                iWidth,
                hwOpacity);

        phwSourceBase += iSourceStride;
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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_rgb565_tile_copy_with_src_chn_mask_and_opacity)(
                                    uint16_t *__RESTRICT phwSourceBase,
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_rgb565_blend_with_src_chn_mask_and_opacity(
                phwSourceBase,
                pwSourceMask,
                phwTargetBase,
                iWidth,
                hwOpacity);

        phwSourceBase += iSourceStride;
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



__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_tile_copy_with_src_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_cccn888_blend_with_src_mask_and_opacity(
                pwSourceBase,
                pchSourceMask,
                pwTargetBase,
                iWidth,
                hwOpacity);

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

__OVERRIDE_WEAK
void __MVE_WRAPPER( __arm_2d_impl_cccn888_tile_copy_with_src_chn_mask_and_opacity)(
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

    for (int_fast16_t y = 0; y < iHeight; y++) {

        __arm_2d_helium_cccn888_blend_with_src_chn_mask_and_opacity(
                pwSourceBase,
                pwSourceMask,
                pwTargetBase,
                iWidth,
                hwOpacity);

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
