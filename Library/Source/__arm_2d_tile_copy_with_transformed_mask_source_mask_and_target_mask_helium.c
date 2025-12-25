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
 * $Revision:    V.1.0.0
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
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSource)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint32_t *pwExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint16_t *phwExtraSource,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint16_t *phwExtraSource)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint32_t *pwExtraSource, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint32_t *pwExtraSource)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint32_t *pwExtraSource, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint32_t *pwExtraSource,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint16_t *phwExtraSource,
                                                    uint_fast16_t hwOpacity,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_n_opa( vHwPixelAlpha,
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSource)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint32_t *pwExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint16_t *phwExtraSource,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint16_t *phwExtraSource)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint32_t *pwExtraSource)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint16_t *phwExtraSource,
                                                    uint_fast16_t hwOpacity,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint8_t *pchExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint8_t *pchExtraSource)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint16_t *phwExtraSource,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask(  vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint8_t *pchExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint8_t *pchExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint16_t *phwExtraSource,
                                                    uint_fast16_t hwOpacity,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_mask_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint8_t *pchExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint8_t *pchExtraSource)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint16_t *phwExtraSource,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks( vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask));


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * GRAY8                                                                      *
 *----------------------------------------------------------------------------*/
__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint8_t *pchExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vldrbq_z_u16(pchTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;

    uint16x8_t  vSource = vldrbq_z_u16(pchExtraSource, predTail);
    uint16x8_t  vBlended =
        vrshrq_n_u16(vqaddq(vHwPixelAlpha * vSource, vTarget * vhwTransparency),8);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint8_t *pchExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vldrbq_u16(pchTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t  vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrbq_u16(pchExtraSource);
    uint16x8_t  vBlended =
        vrshrq_n_u16(   vqaddq(vHwPixelAlpha * vSource, 
                        vTarget * vhwTransparency),
                        8);

    vstrbq_u16(pchTarget, vBlended);
}




__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point)(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint8_t *pchTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity,
                                        uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask_p(   pwExtraSource,
                                                                pchTarget,
                                                                vHwPixelAlpha,
                                                                predTail,
                                                                predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint8_t * pchTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);

    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_gray8_with_mask( pwExtraSource,
                                                            pchTarget,
                                                            vHwPixelAlpha);
}


/*----------------------------------------------------------------------------*
 * RGB565                                                                     *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint16_t *phwExtraSource,
                                                    uint_fast16_t hwOpacity,
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_z_u16(phwTarget, predTail);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_z_u16(phwExtraSource, predTail);

    vst1q_p(phwTarget, 
            vpselq_u16( __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha), 
                        vTarget, 
                        predGlb), 
            predTail);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint16_t *phwExtraSource,
                                        uint_fast16_t hwOpacity)
{
    uint16x8_t      vTarget = vld1q_u16(phwTarget);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    vHwPixelAlpha = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t  vSource = vldrhq_u16(phwExtraSource);

    vst1q(phwTarget, 
          __arm_2d_vblend_rgb565(vTarget, vSource, vHwPixelAlpha));
}


__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point )
                                                    (ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t * pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity, 
                                                    uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(vXi, vYi, pchOrigin, ptOrigValidRegion,
                                             iOrigStride, predTail,
                                             vHwPixelAlpha, predGlb);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask_p(
                                                            pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha,
                                                            predTail,
                                                            predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                        ARM_2D_POINT_VEC * ptPoint,
                                        arm_2d_region_t * ptOrigValidRegion,
                                        uint8_t *pchOrigin,
                                        int16_t iOrigStride,
                                        uint16_t *phwTarget,
                                        uint8_t *pchTargetMask,
                                        uint8_t *pchExtraSourceMask,
                                        uint32_t *pwExtraSource,
                                        uint_fast16_t hwOpacity)
{

    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);
    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride,
                                        vHwPixelAlpha);
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    __arm_2d_helium_ccca8888_blend_8pix_to_rgb565_with_mask(pwExtraSource,
                                                            phwTarget, 
                                                            vHwPixelAlpha);

}

/*----------------------------------------------------------------------------*
 * CCCN888                                                                    *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity, 
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask_p( pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_cccn888_blend_8pix_with_mask(   pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha);

}



__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point )(
                                            ARM_2D_POINT_VEC * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pchOrigin,
                                            int16_t iOrigStride,
                                            uint32_t * pwTarget,
                                            uint8_t *pchTargetMask,
                                            uint8_t *pchExtraSourceMask,
                                            uint32_t *pwExtraSource,
                                            uint_fast16_t hwOpacity,
                                            uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */
    mve_pred16_t    predGlb = 0;

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA(ptPoint, vXi, vYi, pchOrigin,
                                               ptOrigValidRegion, iOrigStride,
                                               predTail, predGlb, vAvgPixel);

        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT(
                                vXi, vYi, pchOrigin, ptOrigValidRegion,
                                iOrigStride, predTail, vHwPixelAlpha, predGlb);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_z_u16(pchExtraSourceMask, predTail),
                                    vldrbq_z_u16(pchTargetMask, predTail),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_m_n_u16(vHwPixelAlpha, 0, predTail)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask_p( 
                                                    pwExtraSource,
                                                    pwTarget,
                                                    vHwPixelAlpha,
                                                    predTail,
                                                    predGlb);

}

__STATIC_INLINE
void __MVE_WRAPPER( 
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src )(
                                                    ARM_2D_POINT_VEC * ptPoint,
                                                    arm_2d_region_t * ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint32_t *pwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint8_t *pchExtraSourceMask,
                                                    uint32_t *pwExtraSource,
                                                    uint_fast16_t hwOpacity)
{
    int16x8_t       vXi = __ARM_2D_GET_POINT_COORD(ptPoint->X);
    int16x8_t       vYi = __ARM_2D_GET_POINT_COORD(ptPoint->Y);
    uint16x8_t      vHwPixelAlpha;
    /* predicate accumulator */
    /* tracks all predications conditions for selecting final */
    /* averaged pixed / target pixel */

#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1                                \
    &&  !__ARM_2D_CFG_DISABLE_ANTI_ALIAS_IN_FILL_COLOUR_WITH_TRANSFORMED_MASK_AND_TARGET_MASK__
    {
        /* accumulated pixel vectors */
        PIX_VEC_TYP     vAvgPixel;

        __ARM2D_AVG_NEIGHBR_GRAY8_PIX_WITH_OPA_INSIDE_SRC(  
                                                ptPoint, vXi, vYi, pchOrigin,
                                                ptOrigValidRegion, iOrigStride,
                                                vAvgPixel);


        vHwPixelAlpha = __ARM_2D_CONVERT_TO_PIX_TYP(vAvgPixel);

    }
#else
    {
        __ARM_2D_GRAY8_GET_PIXVEC_FROM_POINT_INSIDE_SRC(
                                        vXi, vYi, pchOrigin, ptOrigValidRegion,
                                        iOrigStride, vHwPixelAlpha);

    }
#endif
#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif
    vHwPixelAlpha = 
        __arm_2d_scale_alpha_masks_n_opa(   
                                    vHwPixelAlpha, 
                                    vldrbq_u16(pchExtraSourceMask),
                                    vldrbq_u16(pchTargetMask),
                                    hwOpacity);


#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    __arm_2d_helium_ccca8888_blend_8pix_to_cccn888_with_mask(   pwExtraSource,
                                                                pwTarget,
                                                                vHwPixelAlpha);

}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pchExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pchExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pchExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : GRAY8
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint8_t        *pchExtraSourceBase = (uint8_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint8_t *pchExtraSourceLine = pchExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_gray8_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pchExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pchExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pchExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint8_t        *pchTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetLine = pchTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_gray8_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pchTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pchTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pchTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    phwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    phwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            phwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    phwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    phwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            phwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    phwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    phwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            phwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    phwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    phwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            phwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            phwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            phwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            phwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : RGB565
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint16_t        *phwExtraSourceBase = (uint16_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint16_t *phwExtraSourceLine = phwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            phwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        phwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint16_t        *phwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        phwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_and_source_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_and_target_mask_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


/*
 * backend API : CCCN888
 */

__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);

    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_cccn888_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase      += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}


__OVERRIDE_WEAK
void __MVE_WRAPPER(
    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity)(
                                            __arm_2d_param_copy_orig_msk_extra_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwOpacity)

{
    __arm_2d_param_copy_t *ptParamCopy = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .use_as____arm_2d_param_copy_t;
    
    int_fast16_t    iHeight = ptParamCopy->tCopySize.iHeight;
    int_fast16_t    iWidth = ptParamCopy->tCopySize.iWidth;
    int_fast16_t    iTargetStride = ptParamCopy->tTarget.iStride;
    uint32_t        *pwTargetBase = ptParamCopy->tTarget.pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)
                                    ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                        .use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .iStride;
    uint8_t         *pchTargetMaskBase = (uint8_t *)
                                            ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                                .tDesMask
                                                    .iStride;

    uint32_t        *pwExtraSourceBase = (uint32_t *)ptParam->tExtraSource.pBuffer;
    int_fast16_t    iExtraSourceStride = ptParam->tExtraSource.iStride;
    uint8_t         *pchExtraSourceMaskBase = ptParam->tExtraSourceMask.pBuffer;
    int_fast16_t    iExtraSourceMaskStride = ptParam->tExtraSourceMask.iStride;

    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset = ptParamCopy->tSource.tValidRegion.tLocation;
    
    arm_2d_region_t *ptOriginValidRegion = &ptParam->use_as____arm_2d_param_copy_orig_msk_t
                                            .use_as____arm_2d_param_copy_orig_t
                                                .tOrigin
                                                    .tValidRegion;
    hwOpacity += (hwOpacity == 255);
    q31_t  invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */

    __arm_2d_transform_regression(
        &ptParamCopy->tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    /* slopes between 1st and last columns */
    int32_t         slopeY, slopeX;

    slopeY = MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX = MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    int32_t         nrmSlopeX = 17 - __CLZ(ABS(slopeX));
    int32_t         nrmSlopeY = 17 - __CLZ(ABS(slopeY));

    slopeX = ARSHIFT(slopeX, nrmSlopeX);
    slopeY = ARSHIFT(slopeY, nrmSlopeY);

    for (int32_t y = 0; y < iHeight; y++) {

        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetLine = pwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;
        uint32_t *pwExtraSourceLine = pwExtraSourceBase;
        uint8_t *pchExtraSourceMaskLine = pchExtraSourceMaskBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                                                            ptOriginValidRegion,
                                                            &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point_inside_src(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, 
                    iOrigStride,
                    pwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    pwExtraSourceLine,
                    hwOpacity,
                    nbVecElts);
            }

            pwTargetLine += 8;
            pchTargetMaskLine += 8;
            pwExtraSourceLine += 8;
            pchExtraSourceMaskLine += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }

        pwTargetBase           += iTargetStride;
        pchTargetMaskBase       += iTargetMaskStride;
        pwExtraSourceBase       += iExtraSourceStride;
        pchExtraSourceMaskBase  += iExtraSourceMaskStride;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */