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
 * Title:        __arm_2d_fill_colour_with_transformed_mask_and_target_mask_helium.c
 * Description:  APIs for 
 *               colour-filling-with-transformed-mask-and-target-mask
 *
 * $Date:        18 Nov 2025
 * $Revision:    V.0.5.0
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

__STATIC_INLINE
void __rgb565_fill_colour_with_transformed_mask_target_mask_and_opacity_process_point(
                                                    arm_2d_point_s16x8_t *ptPoint,
                                                    arm_2d_region_t *ptOrigValidRegion,
                                                    uint8_t *pchOrigin,
                                                    int16_t iOrigStride,
                                                    uint16_t *phwTarget,
                                                    uint8_t *pchTargetMask,
                                                    uint16_t hwMaskColour,
                                                    uint_fast16_t hwOpacity, 
                                                    uint32_t elts)
{
    mve_pred16_t predTail = vctp16q(elts);
    uint16x8_t vTarget = vld1q_u16(phwTarget);
    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);
    uint16x8_t vHwPixelAlpha;

    mve_pred16_t predGlb = 0;

    do {
        uint16x8_t vAvgPixel;

        uint16x8_t ptVal8;
        __typeof__(vAvgPixel) vAreaTR, vAreaTL, vAreaBR, vAreaBL;
        int16x8_t vOne = vdupq_n_s16(((1) << 6));
        int16x8_t vWX = ptPoint->X - ((vXi) << 6);
        int16x8_t vWY = ptPoint->Y - ((vYi) << 6);
        vAreaTR = vmulq_u16(vWX, vWY);
        vAreaTL = vmulq_u16((vOne - vWX), vWY);
        vAreaBR = vmulq_u16(vWX, (vOne - vWY));
        vAreaBL = vmulq_u16((vOne - vWX), (vOne - vWY));
        vAreaTR = vqshlq_n_u16(vAreaTR, 4);
        vAreaTL = vqshlq_n_u16(vAreaTL, 4);
        vAreaBR = vqshlq_n_u16(vAreaBR, 4);
        vAreaBL = vqshlq_n_u16(vAreaBL, 4);
    
        do {
            arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vYi};
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);

            vAvgPixel = vrmulhq_u16(vAreaBL, ptVal8);
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);

            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaBR, ptVal8));
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);

            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTL, ptVal8));
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTR, ptVal8));
        } while(0);

        vHwPixelAlpha = (vAvgPixel);
    } while(0);

    vHwPixelAlpha = __arm_2d_scale_alpha_mask_opa(  vHwPixelAlpha, 
                                                    vldrbq_z_u16(pchTargetMask, predTail), 
                                                    hwOpacity);

    uint16x8_t vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&hwMaskColour), &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget, &tSrcPix, vhwTransparency);

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);
}

__STATIC_INLINE
void __rgb565_fill_colour_with_transformed_mask_target_mask_and_opacity_process_point_inside_src(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint8_t *pchTargetMask,
                                            uint16_t hwMaskColour,
                                            uint_fast16_t hwOpacity)
{
    uint16x8_t vTarget = vld1q_u16(phwTarget);
    

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);
    uint16x8_t vHwPixelAlpha;

    do {
        uint16x8_t vAvgPixel;

        uint16x8_t ptVal8;
        __typeof__(vAvgPixel) vAreaTR, vAreaTL, vAreaBR, vAreaBL;
        int16x8_t vOne = vdupq_n_s16(((1) << 6));
        int16x8_t vWX = ptPoint->X - ((vXi) << 6);
        int16x8_t vWY = ptPoint->Y - ((vYi) << 6);
        vAreaTR = vmulq_u16(vWX, vWY);
        vAreaTL = vmulq_u16((vOne - vWX), vWY);
        vAreaBR = vmulq_u16(vWX, (vOne - vWY));
        vAreaBL = vmulq_u16((vOne - vWX), (vOne - vWY));
        vAreaTR = vqshlq_n_u16(vAreaTR, 4);
        vAreaTL = vqshlq_n_u16(vAreaTL, 4);
        vAreaBR = vqshlq_n_u16(vAreaBR, 4);
        vAreaBL = vqshlq_n_u16(vAreaBL, 4);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vYi};
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_u16(pOriginCorrected, ptOffs);

            vAvgPixel = vrmulhq_u16(vAreaBL, ptVal8);
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_u16(pOriginCorrected, ptOffs);
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaBR, ptVal8));
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_u16(pOriginCorrected, ptOffs);

            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTL, ptVal8));
        } while(0);

        do {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t *pOriginCorrected = pchOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_u16(pOriginCorrected, ptOffs);

            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTR, ptVal8));
        } while(0);

        vHwPixelAlpha = (vAvgPixel);
    } while(0);

    vHwPixelAlpha = __arm_2d_scale_alpha_mask_opa(  vHwPixelAlpha, 
                                                    vldrbq_u16(pchTargetMask), 
                                                    hwOpacity);

    uint16x8_t vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t vBlended;
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&hwMaskColour), &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget,
                                                            &tSrcPix,
                                                            vhwTransparency);

    vst1q(phwTarget, vBlended);
}

__OVERRIDE_WEAK 
void 
__MVE_WRAPPER(
    __arm_2d_impl_rgb565_colour_filling_with_transformed_mask_target_mask_and_opacity)(
                                    __arm_2d_param_copy_orig_msk_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptParam->use_as____arm_2d_param_copy_orig_t
                                .use_as____arm_2d_param_copy_t
                                    .tCopySize
                                        .iHeight;

    int_fast16_t    iWidth = ptParam->use_as____arm_2d_param_copy_orig_t
                                .use_as____arm_2d_param_copy_t
                                    .tCopySize
                                        .iWidth;

    int_fast16_t    iTargetStride = ptParam->use_as____arm_2d_param_copy_orig_t
                                .use_as____arm_2d_param_copy_t
                                    .tTarget
                                        .iStride;

    uint16_t        *phwTargetBase = ptParam->use_as____arm_2d_param_copy_orig_t
                                        .use_as____arm_2d_param_copy_t
                                            .tTarget
                                                .pBuffer;

    uint8_t         *pchOrigin = (uint8_t *)ptParam
                                        ->use_as____arm_2d_param_copy_orig_t
                                            .tOrigin
                                                .pBuffer;

    int_fast16_t    iOrigStride = ptParam
                                    ->use_as____arm_2d_param_copy_orig_t
                                        .tOrigin
                                            .iStride;

    uint8_t         *pchTargetMaskBase = (uint8_t *)ptParam->tDesMask.pBuffer;
    int_fast16_t    iTargetMaskStride = ptParam->tDesMask.iStride;
                                        
    uint16_t        hwMaskColour = ptInfo->Mask.hwColour;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_point_float_t *pCenter = &(ptInfo->tCenter);

    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    hwOpacity += (hwOpacity == 255);

    __arm_2d_transform_regression(  &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                    &SrcPt,
                                    fAngle,
                                    ptInfo->fScaleX,
                                    ptInfo->fScaleY,
                                    &tOffset,
                                    pCenter,
                                    iOrigStride,
                                    regrCoefs);

    int32_t slopeY, slopeX;

    slopeY = (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX = (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int32_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetLine = phwTargetBase;
        uint8_t *pchTargetMaskLine = pchTargetMaskBase;

        vX = ((vX) << 6);

        while (nbVecElts > 0) {

            arm_2d_point_s16x8_t tPointV, tPointTemp;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            tPointTemp.X = tPointV.X >> 6;
            tPointTemp.Y = tPointV.Y >> 6;
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16_safe(
                &ptParam->tOrigin.tValidRegion,
                &tPointTemp);
            p &= vctp16q(nbVecElts);

            if (0xFFFF == p) {
                __arm_2d_impl_rgb565_get_alpha_with_opacity_inside_src(
                                                &tPointV,
                                                &ptParam->tOrigin.tValidRegion,
                                                pchOrigin, iOrigStride,
                                                phwTargetLine,
                                                pchTargetMaskLine,
                                                hwMaskColour,
                                                hwOpacity);
            } else if (0 != p) {
                __arm_2d_impl_rgb565_get_alpha_with_opacity(
                                                &tPointV,
                                                &ptParam->tOrigin.tValidRegion,
                                                pchOrigin, iOrigStride,
                                                phwTargetLine,
                                                pchTargetMaskLine,
                                                hwMaskColour,
                                                hwOpacity,
                                                nbVecElts);
            }

            phwTargetLine += 8;
            pchTargetMaskLine += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }

        phwTargetBase += iTargetStride;
        pchTargetMaskBase += iTargetMaskStride;
    }
}

#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */
