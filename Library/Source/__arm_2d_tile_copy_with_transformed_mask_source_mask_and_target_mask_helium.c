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

    __arm_2d_scale_alpha_masks_n_opa(   vHwPixelAlpha, 
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
    uint16x8_t      vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*phwExtraSource, &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(   vTarget, 
                                                                &tSrcPix, 
                                                                vhwTransparency);

    /* select between target pixel, averaged pixed */
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);
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

    __arm_2d_scale_alpha_masks_n_opa(   vHwPixelAlpha, 
                                        vldrbq_u16(pchExtraSourceMask),
                                        vldrbq_u16(pchTargetMask),
                                        hwOpacity);

#if __ARM_2D_CFG_OPTIMIZE_FOR_HOLLOW_OUT_MASK_IN_TRANSFORM__
    if (0 == vcmpneq_n_u16(vHwPixelAlpha, 0)) {
        return ;
    }
#endif

    /* blending */
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*phwExtraSource, &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget, 
                                                             &tSrcPix, 
                                                             vhwTransparency);

    vst1q(phwTarget, vBlended);
}


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
                    pchOrigin, iOrigStride,
                    phwTargetLine,
                    pchTargetMaskLine,
                    pchExtraSourceMaskLine,
                    phwExtraSourceLine,
                    hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_tile_copy_with_transformed_mask_source_mask_target_mask_and_opacity_process_point(
                    &tPointV,
                    ptOriginValidRegion,
                    pchOrigin, iOrigStride,
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


#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */