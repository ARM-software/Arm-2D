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
 * Title:        __arm_2d_tile_copy_with_transformed_mask_source_mask_and_-
 *               -target_mask_helium.c
 * Description:  The helium implementation for tile-copy-with-transformed-
 *               mask-source-mask-and-target-mask
 * 
 * $Date:        25. Dec 2025
 * $Revision:    V.0.5.0
 *
 * Target Processor:  Cortex-M cores with helium
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#define __ARM_2D_IMPL__

#include "__arm_2d_impl.h"
#include "arm_2d.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#pragma clang diagnostic ignored "-Wunused-variable"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174, 177, 188, 68, 513, 144
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
        __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * phwOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity, 
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    NEAR_OFFSET, ptPoint, vXi, vYi, phwOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT(
                            vXi, vYi, phwOrigin, ptOrigValidRegion, iOrigStride,
                            predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC((uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB,
                                        vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * phwOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    NEAR_OFFSET, ptPoint, vXi, vYi, phwOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR, vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {

        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_INSIDE_SRC(
                            vXi, vYi, phwOrigin, ptOrigValidRegion, iOrigStride,
                            vAvgR, vAvgG, vAvgB );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(phwTarget, vBlended);
}

__STATIC_INLINE
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_offs_compensated)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * phwOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity, 
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX(
                    FAR_OFFSET, ptPoint, vXi, vYi, phwOrigin, ptOrigValidRegion,
                    iOrigStride, predTail, predGlb, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR(
                        vXi, vYi, phwOrigin, ptOrigValidRegion, iOrigStride,
                        predTail, vAvgR, vAvgG, vAvgB, predGlb );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_offs_compensated_inside_src)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint16_t * phwOrigin,
                                        int16_t iOrigStride,
                                        uint16_t * phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vAvgR, vAvgG, vAvgB;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    //mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixelR, vAvgPixelG, vAvgPixelB;

        __ARM2D_AVG_NEIGHBR_RGB565_PIX_INSIDE_SRC(
                    FAR_OFFSET, ptPoint, vXi, vYi, phwOrigin, ptOrigValidRegion,
                    iOrigStride, vAvgPixelR,
                    vAvgPixelG, vAvgPixelB);

        vAvgR = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelR);
        vAvgG = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelG);
        vAvgB = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixelB);
    }
#else
    {
        __ARM_2D_RGB565_GET_RGBVEC_FROM_POINT_FAR_INSIDE_SRC(
                        vXi, vYi, phwOrigin, ptOrigValidRegion, iOrigStride,
                        vAvgR, vAvgG, vAvgB );
    }
#endif
    /* blending */

    uint16x8_t      vBlended;

    __ARM_2D_BLEND_RGB565_TARGET_RGBVEC(
                (uint16_t) hwOpacity, vTarget, vAvgR, vAvgG, vAvgB, vBlended);

    vst1q(phwTarget, vBlended);
}


#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */