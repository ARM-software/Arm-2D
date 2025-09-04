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
 * Title:        __arm_2d_tile_fill_with_source_mask_and_opacity_helium.c
 * Description:  Helium implementation for tile fill with source mask and opacity only
 *
 * $Date:        04. September 2025
 * $Revision:    V.1.1.0
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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,1,64
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void __arm_2d_impl_gray8_src_msk_fill_opa(
    uint8_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint8_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint8_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

extern
void __arm_2d_impl_gray8_src_chn_msk_fill_opa(
    uint8_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint32_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint8_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

extern
void __arm_2d_impl_rgb565_src_msk_fill_opa(
    uint16_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint8_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint16_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

extern
void __arm_2d_impl_rgb565_src_chn_msk_fill_opa(
    uint16_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint32_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint16_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

extern
void __arm_2d_impl_cccn888_src_msk_fill_opa(
    uint32_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint8_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint32_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

extern
void __arm_2d_impl_cccn888_src_chn_msk_fill_opa(
    uint32_t *__restrict ptSourceBase,
    int16_t iSourceStride,
    arm_2d_size_t *__restrict ptSourceSize,

    uint32_t *__restrict ptSourceMaskBase,

    int16_t iSourceMaskStride,
    arm_2d_size_t *__restrict ptSourceMaskSize,

    uint32_t *__restrict ptTargetBase,
    int16_t iTargetStride,
    arm_2d_size_t *__restrict ptTargetSize,
    uint16_t hwOpacity);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

__OVERRIDE_WEAK
void __arm_2d_impl_gray8_tile_fill_with_src_mask_and_opacity(
                        uint8_t * __RESTRICT pchSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t * __RESTRICT pchSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_gray8_src_msk_fill_opa(  pchSourceBase, 
                                            iSourceStride,
                                            ptSourceSize,

                                            pchSourceMaskBase, 
                                            iSourceMaskStride,
                                            ptSourceMaskSize,

                                            pchTargetBase,
                                            iTargetStride,
                                            ptTargetSize,

                                            hwOpacity);
}

__OVERRIDE_WEAK
void __arm_2d_impl_gray8_tile_fill_with_src_chn_mask_and_opacity(
                        uint8_t * __RESTRICT pchSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_gray8_src_chn_msk_fill_opa(  pchSourceBase, 
                                                iSourceStride,
                                                ptSourceSize,

                                                pwSourceMaskBase, 
                                                iSourceMaskStride,
                                                ptSourceMaskSize,

                                                pchTargetBase,
                                                iTargetStride,
                                                ptTargetSize,

                                                hwOpacity);
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_src_mask_and_opacity(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint8_t *__restrict pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize,
                                    uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint8_t *__RESTRICT pchTarget = pchTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_gray8_with_src_mask_and_opacity(
                    pwSource, 
                    pchSourceMask, 
                    pchTarget, 
                    wLength, 
                    hwOpacity);

                pchTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_src_chn_mask_and_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint8_t *__RESTRICT pchTarget = pchTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_gray8_with_src_chn_mask_and_opacity(
                    pwSource, 
                    pwSourceMask, 
                    pchTarget, 
                    wLength, 
                    hwOpacity);

                pchTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_src_mask(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint8_t *__restrict pchTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint8_t *__RESTRICT pchTarget = pchTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_gray8_with_src_mask(
                    pwSource, 
                    pchSourceMask, 
                    pchTarget, 
                    wLength);

                pchTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_gray8_with_src_chn_mask(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint8_t *__RESTRICT pchTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint8_t *__RESTRICT pchTarget = pchTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_gray8_with_src_chn_mask(
                    pwSource, 
                    pwSourceMask, 
                    pchTarget, 
                    wLength);

                pchTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pchTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_tile_fill_with_src_mask_and_opacity(
                        uint16_t * __RESTRICT phwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t * __RESTRICT pchSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_rgb565_src_msk_fill_opa(  phwSourceBase, 
                                            iSourceStride,
                                            ptSourceSize,

                                            pchSourceMaskBase, 
                                            iSourceMaskStride,
                                            ptSourceMaskSize,

                                            phwTargetBase,
                                            iTargetStride,
                                            ptTargetSize,

                                            hwOpacity);
}

__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_tile_fill_with_src_chn_mask_and_opacity(
                        uint16_t * __RESTRICT phwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_rgb565_src_chn_msk_fill_opa(  phwSourceBase, 
                                                iSourceStride,
                                                ptSourceSize,

                                                pwSourceMaskBase, 
                                                iSourceMaskStride,
                                                ptSourceMaskSize,

                                                phwTargetBase,
                                                iTargetStride,
                                                ptTargetSize,

                                                hwOpacity);
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_src_mask_and_opacity(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint16_t *__restrict phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize,
                                    uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint16_t *__RESTRICT phwTarget = phwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_mask_and_opacity(
                    pwSource, 
                    pchSourceMask, 
                    phwTarget, 
                    wLength, 
                    hwOpacity);

                phwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_src_chn_mask_and_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint16_t *__RESTRICT phwTarget = phwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_chn_mask_and_opacity(
                    pwSource, 
                    pwSourceMask, 
                    phwTarget, 
                    wLength, 
                    hwOpacity);

                phwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_src_mask(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint16_t *__restrict phwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint16_t *__RESTRICT phwTarget = phwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_mask(
                    pwSource, 
                    pchSourceMask, 
                    phwTarget, 
                    wLength);

                phwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_rgb565_with_src_chn_mask(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint16_t *__RESTRICT phwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint16_t *__RESTRICT phwTarget = phwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_rgb565_with_src_chn_mask(
                    pwSource, 
                    pwSourceMask, 
                    phwTarget, 
                    wLength);

                phwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            phwTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_cccn888_tile_fill_with_src_mask_and_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint8_t * __RESTRICT pchSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_cccn888_src_msk_fill_opa(  pwSourceBase, 
                                            iSourceStride,
                                            ptSourceSize,

                                            pchSourceMaskBase, 
                                            iSourceMaskStride,
                                            ptSourceMaskSize,

                                            pwTargetBase,
                                            iTargetStride,
                                            ptTargetSize,

                                            hwOpacity);
}

__OVERRIDE_WEAK
void __arm_2d_impl_cccn888_tile_fill_with_src_chn_mask_and_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    __arm_2d_impl_cccn888_src_chn_msk_fill_opa(  pwSourceBase, 
                                                iSourceStride,
                                                ptSourceSize,

                                                pwSourceMaskBase, 
                                                iSourceMaskStride,
                                                ptSourceMaskSize,

                                                pwTargetBase,
                                                iTargetStride,
                                                ptTargetSize,

                                                hwOpacity);
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_src_mask_and_opacity(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint32_t *__restrict pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize,
                                    uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint32_t *__RESTRICT pwTarget = pwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_mask_and_opacity(
                    pwSource, 
                    pchSourceMask, 
                    pwTarget, 
                    wLength, 
                    hwOpacity);

                pwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_src_chn_mask_and_opacity(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize,
                        uint_fast16_t hwOpacity)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint32_t *__RESTRICT pwTarget = pwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_chn_mask_and_opacity(
                    pwSource, 
                    pwSourceMask, 
                    pwTarget, 
                    wLength, 
                    hwOpacity);

                pwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}
                        
__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_src_mask(
                                    uint32_t *__restrict pwSourceBase,
                                    int16_t iSourceStride,
                                    arm_2d_size_t *__RESTRICT ptSourceSize,

                                    uint8_t *__RESTRICT pchSourceMaskBase,
                                    int16_t iSourceMaskStride,
                                    arm_2d_size_t *__RESTRICT ptSourceMaskSize,

                                    uint32_t *__restrict pwTargetBase,
                                    int16_t iTargetStride,
                                    arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint8_t *pchSourceMask = pchSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint32_t *__RESTRICT pwTarget = pwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_mask(
                    pwSource, 
                    pchSourceMask, 
                    pwTarget, 
                    wLength);

                pwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;

            pchSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}

__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_tile_fill_to_cccn888_with_src_chn_mask(
                        uint32_t * __RESTRICT pwSourceBase,
                        int16_t iSourceStride,
                        arm_2d_size_t *__RESTRICT ptSourceSize,
                        
                        uint32_t * __RESTRICT pwSourceMaskBase,
                        int16_t iSourceMaskStride,
                        arm_2d_size_t *__RESTRICT ptSourceMaskSize,
                        
                        uint32_t *__RESTRICT pwTargetBase,
                        int16_t iTargetStride,
                        arm_2d_size_t *__RESTRICT ptTargetSize)
{
    for (int_fast16_t iTargetY = 0; iTargetY < ptTargetSize->iHeight;) {

        uint32_t *__RESTRICT pwSource = pwSourceBase;

        uint32_t *pwSourceMask = pwSourceMaskBase;

        for (int_fast16_t iSourceY = 0; iSourceY < ptSourceSize->iHeight; iSourceY++) {

            uint32_t *__RESTRICT pwTarget = pwTargetBase;

            uint_fast32_t wLengthLeft = ptTargetSize->iWidth;

            do {
                uint_fast32_t wLength = wLengthLeft < ptSourceSize->iWidth
                                      ? wLengthLeft
                                      : ptSourceSize->iWidth;
                __arm_2d_helium_ccca8888_blend_to_cccn888_with_src_chn_mask(
                    pwSource, 
                    pwSourceMask, 
                    pwTarget, 
                    wLength);

                pwTarget += wLength;

                wLengthLeft -= wLength;
            } while (wLengthLeft);

            pwSource += iSourceStride;
            pwTargetBase += iTargetStride;

            pwSourceMask += iSourceMaskStride;

            iTargetY++;
            if (iTargetY >= ptTargetSize->iHeight) {
                break;
            }
        }
    }
}


#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */