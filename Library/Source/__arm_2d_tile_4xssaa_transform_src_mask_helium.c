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
 * Title:        __arm_2d_tile_4xssaa_transform_src_mask_helium.c
 * Description:  The helium source file of 4x SuperSampling Anti Alias  
 *               Tile Transform with source mask
 * 
 * $Date:        06. Jan 2025
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
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,64
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


static
void __arm_2d_impl_gray8_4xssaa_get_pixel_colour_src_mask (
                                            arm_2d_point_s16x8_t * ptPoint,
                                            arm_2d_region_t * ptOrigValidRegion,
                                            uint8_t * pOrigin, 
                                            int16_t iOrigStride,
                                            uint8_t * pTarget,
                                            uint8_t * pchOrigMask, 
                                            int16_t iOrigmaskStride,
                                            uint32_t elts)
{
    iOrigmaskStride *= 1;

    mve_pred16_t predTail = vctp16q(elts);
    uint16x8_t vTarget = vldrbq_u16(pTarget);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixel;

    mve_pred16_t predGlb = 0;
    uint16x8_t vAvgTransparency;

    {
        uint16x8_t ptVal8, vPixelAlpha; 
        __typeof__ (vAvgPixel) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
        
        {
            arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
            vAvgTransparency = vAreaBL - vAlpha;
            vAvgPixel = vrmulhq(vAlpha, ptVal8);
        } 
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi }; 
            
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8));
        } 
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p);
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8)); 
        } 
        
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p);
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            
            uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8));
        };
    }

    uint16x8_t vBlended = vminq(vAvgPixel + vrmulhq(vTarget, vAvgTransparency), vdupq_n_u16(255));

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

void __arm_2d_impl_gray8_4xssaa_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo)
{

    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint8_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);


    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint8_t *pTargetBaseCur = pTargetBase;

        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_gray8_4xssaa_get_pixel_colour_src_mask(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,
                                pOriginMask,
                                iOrigMaskStride,
                                nbVecElts
                            );

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}

static
void __arm_2d_impl_gray8_4xssaa_get_pixel_colour_src_mask_opa (
                                                 arm_2d_point_s16x8_t * ptPoint,
                                                 arm_2d_region_t * ptOrigValidRegion,
                                                 uint8_t * pOrigin, int16_t iOrigStride,
                                                 uint8_t * pTarget,
                                                 uint8_t * pchOrigMask, int16_t iOrigmaskStride,
                                                 uint16_t hwOpacity,
                                                 uint32_t elts)
{
    iOrigmaskStride *= 1;

    mve_pred16_t predTail = vctp16q(elts);
    uint16x8_t vTarget = vldrbq_u16(pTarget);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixel;

    mve_pred16_t predGlb = 0;
    uint16x8_t vAvgTransparency;

    {
        uint16x8_t ptVal8, vPixelAlpha; 
        __typeof__ (vAvgPixel) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8; 
            
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
            vAvgTransparency = vAreaBL - vAlpha; vAvgPixel = vrmulhq(vAlpha, ptVal8);
        } 
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi };
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
            
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8));
        } 
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8)); 
        } 
        { 
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
            mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
            predGlb |= p; 
            int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
            uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
            uint8_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
            uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
            uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
            vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
            vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
            vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
            uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
            vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq(vAlpha, ptVal8));
        };

    }

    uint16x8_t vBlended = vminq(vAvgPixel + vrmulhq(vTarget, vAvgTransparency), vdupq_n_u16(255));;

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vstrbq_p_u16(pTarget, vTarget, predTail);
}

void __arm_2d_impl_gray8_4xssaa_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo,
                                        uint_fast16_t hwOpacity)
{

    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint8_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    hwOpacity += (hwOpacity == 255);

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint8_t *pTargetBaseCur = pTargetBase;


        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_gray8_4xssaa_get_pixel_colour_src_mask_opa(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,

                                pOriginMask,
                                iOrigMaskStride,

                                hwOpacity,

                                nbVecElts
                            );

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}


static
void __arm_2d_impl_rgb565_4xssaa_get_pixel_colour_src_mask(arm_2d_point_s16x8_t * ptPoint,
                                                            arm_2d_region_t * ptOrigValidRegion,
                                                            uint16_t * pOrigin,
                                                            int16_t iOrigStride,
                                                            uint16_t * pTarget,
                                                            uint8_t * pchOrigMask,
                                                            int16_t iOrigmaskStride,
                                                            uint32_t elts)
{
    iOrigmaskStride *= 1;

    mve_pred16_t predTail = vctp16q(elts);
    uint16x8_t vTarget = vld1q(pTarget);
    uint16x8_t vAvgTransparency;

    mve_pred16_t predGlb = 0;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);


    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha; 
    __typeof__ (vAvgPixelR) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
    
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
        vAvgTransparency = vAreaBL - vAlpha; vAvgPixelR = vrmulhq_u16(vAlpha, R ); 
        vAvgPixelG = vrmulhq_u16(vAlpha, G ); 
        vAvgPixelB = vrmulhq_u16(vAlpha, B );
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p);
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    };

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vBlended;
    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans)); 
    vAvgR = vminq(vAvgR, vdupq_n_u16(255)); 
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans)); 
    vAvgG = vminq(vAvgG, vdupq_n_u16(255)); 
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans)); 
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

void __arm_2d_impl_rgb565_4xssaa_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo)
{


    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);


    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;


    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;


    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint16_t *pTargetBaseCur = pTargetBase;


        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_rgb565_4xssaa_get_pixel_colour_src_mask(
                                                &tPointV,
                                                &ptParam->tOrigin.tValidRegion,
                                                ptParam->tOrigin.pBuffer,
                                                iOrigStride,
                                                pTargetBaseCur,
                                                pOriginMask,
                                                iOrigMaskStride,
                                                nbVecElts);

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}


static
void __arm_2d_impl_rgb565_4xssaa_get_pixel_colour_src_mask_opa(arm_2d_point_s16x8_t * ptPoint,
                                                            arm_2d_region_t * ptOrigValidRegion,
                                                            uint16_t * pOrigin,
                                                            int16_t iOrigStride,
                                                            uint16_t * pTarget,
                                                            uint8_t * pchOrigMask,
                                                            int16_t iOrigmaskStride,
                                                            uint16_t hwOpacity,
                                                            uint32_t elts)
{
    iOrigmaskStride *= 1;

    mve_pred16_t predTail = vctp16q(elts);
    uint16x8_t vTarget = vld1q(pTarget);
    uint16x8_t vAvgTransparency;

    mve_pred16_t predGlb = 0;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha; 
    __typeof__ (vAvgPixelR) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
        vAvgTransparency = vAreaBL - vAlpha; 
        vAvgPixelR = vrmulhq_u16(vAlpha, R ); 
        vAvgPixelG = vrmulhq_u16(vAlpha, G ); 
        vAvgPixelB = vrmulhq_u16(vAlpha, B );
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8; 
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255)); 
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint); 
        predGlb |= p; 
        int16_t correctionOffset = vminvq_s16(32767, vPoint.Y) - 1; 
        uint16x8_t ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride; 
        uint16_t *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride); 
        uint16x8_t ptVal = vldrhq_gather_shifted_offset_z_u16(pOriginCorrected, ptOffs, predTail & p); 
        __arm_2d_rgb565_unpack_single_vec(ptVal, &R, &G, &B); 
        uint16x8_t maskOffs = 1 * vPoint.X + (vPoint.Y - correctionOffset) * iOrigmaskStride; 
        uint8_t *pMaskCorrected = pchOrigMask + (correctionOffset * iOrigmaskStride); 
        vPixelAlpha = vldrbq_gather_offset_z_u16(pMaskCorrected, maskOffs, predTail & p); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    };

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);


    uint16x8_t vBlended;
    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans)); 
    vAvgR = vminq(vAvgR, vdupq_n_u16(255)); 
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans)); 
    vAvgG = vminq(vAvgG, vdupq_n_u16(255)); 
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans)); 
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));;

    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}

void __arm_2d_impl_rgb565_4xssaa_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo,
                                        uint_fast16_t hwOpacity)
{

    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    hwOpacity += (hwOpacity == 255);

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint16_t *pTargetBaseCur = pTargetBase;

        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_rgb565_4xssaa_get_pixel_colour_src_mask_opa(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,

                                pOriginMask,
                                iOrigMaskStride,
                                hwOpacity,

                                nbVecElts
                            );

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}


static
void __arm_2d_impl_cccn888_4xssaa_get_pixel_colour_src_mask(arm_2d_point_s16x8_t *ptPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        uint32_t *pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t *pTarget,

                                        uint8_t * pchOrigMask,
                                        int16_t iOrigmaskStride,

                                        uint32_t elts)
{
    iOrigmaskStride *= 1;

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *) scratch32;
    uint32x4_t vTargetLo = vld1q(pTarget);
    uint32x4_t vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    mve_pred16_t predGlbLo = 0, predGlbHi = 0;
    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha; 
    __typeof__ (vAvgPixelR) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        
        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
        vAvgTransparency = vAreaBL - vAlpha; 
        vAvgPixelR = vrmulhq_u16(vAlpha, R ); 
        vAvgPixelG = vrmulhq_u16(vAlpha, G ); 
        vAvgPixelB = vrmulhq_u16(vAlpha, B );
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) };
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    };

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans)); 
    vAvgR = vminq(vAvgR, vdupq_n_u16(255)); 
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans)); 
    vAvgG = vminq(vAvgG, vdupq_n_u16(255)); 
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans)); 
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t TempPixel = vld1q(scratch32);

    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

    vst1q_p(pTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);

    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pTarget + 4, TempPixel, predTailHigh);
}

void __arm_2d_impl_cccn888_4xssaa_transform_with_src_mask(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo)
{


    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);


    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint32_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;


    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;


    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint32_t *pTargetBaseCur = pTargetBase;

        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_cccn888_4xssaa_get_pixel_colour_src_mask(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,

                                pOriginMask,
                                iOrigMaskStride,

                                nbVecElts
                            );

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}


static
void __arm_2d_impl_cccn888_4xssaa_get_pixel_colour_src_mask_opa(arm_2d_point_s16x8_t *ptPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        uint32_t *pOrigin,
                                        int16_t iOrigStride,
                                        uint32_t *pTarget,
                                        uint8_t * pchOrigMask,
                                        int16_t iOrigmaskStride,
                                        uint16_t hwOpacity,
                                        uint32_t elts)
{
    iOrigmaskStride *= 1;

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *) scratch32;
    uint32x4_t vTargetLo = vld1q(pTarget);
    uint32x4_t vTargetHi = vld1q(pTarget + 4);
    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    mve_pred16_t predGlbLo = 0, predGlbHi = 0;
    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha; 
    __typeof__ (vAvgPixelR) vAreaTR, vAreaTL, vAreaBR, vAreaBL; 
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
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha); 
        vAvgTransparency = vAreaBL - vAlpha; 
        vAvgPixelR = vrmulhq_u16(vAlpha, R ); 
        vAvgPixelG = vrmulhq_u16(vAlpha, G ); 
        vAvgPixelB = vrmulhq_u16(vAlpha, B );
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha); 
        
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; 
        vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    } 
    { 
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) }; 
        arm_2d_point_s32x4_t tPointLo, tPointHi; vst1q(pscratch16, vPoint.X); 
        tPointLo.X = vldrhq_s32(pscratch16); 
        tPointHi.X = vldrhq_s32(pscratch16 + 4); 
        vst1q(pscratch16, vPoint.Y); 
        tPointLo.Y = vldrhq_s32(pscratch16); 
        tPointHi.Y = vldrhq_s32(pscratch16 + 4); 
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo); 
        predGlbLo |= p; 
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride; 
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p); 
        vst1q(scratch32, ptVal); 
        uint32x4_t maskOffs = 1 * tPointLo.X + tPointLo.Y * iOrigmaskStride; 
        uint32x4_t maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailLow & p); 
        vst1q(scratch32+8, maskVal); 
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi); 
        predGlbHi |= p; 
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride; 
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p); 
        vst1q(scratch32 + 4, ptVal); 
        maskOffs = 1 * tPointHi.X + tPointHi.Y * iOrigmaskStride; 
        maskVal = vldrbq_gather_offset_z_u32(pchOrigMask, maskOffs, predTailHigh & p); 
        vst1q(scratch32+12, maskVal); 
        __arm_2d_unpack_rgb888_from_mem((uint8_t *) scratch32, &R, &G, &B); 
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *) &scratch32[8], vidupq_n_u16(0, 4)); 
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha); 
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha); 
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R)); 
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G)); 
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B)); 
    };

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);


    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_unpack_rgb888_from_mem((const uint8_t *) pTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans)); 
    vAvgR = vminq(vAvgR, vdupq_n_u16(255)); 
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans)); 
    vAvgG = vminq(vAvgG, vdupq_n_u16(255)); 
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans)); 
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));;

    __arm_2d_pack_rgb888_to_mem((uint8_t *) scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t TempPixel = vld1q(scratch32);

    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);

    vst1q_p(pTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);

    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pTarget + 4, TempPixel, predTailHigh);
}

void __arm_2d_impl_cccn888_4xssaa_transform_with_src_mask_and_opacity(
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                        __arm_2d_transform_info_t *ptInfo,
                                        uint_fast16_t hwOpacity)
{

    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);


    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint32_t *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    uint8_t *pOriginMask = (*ptThis).tOrigMask.pBuffer;
    int_fast16_t iOrigMaskStride = (*ptThis).tOrigMask.iStride;

    hwOpacity += (hwOpacity == 255);


    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t) ((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

    int32_t nrmSlopeX = 17 - __clz(((slopeX) > 0 ? (slopeX) : -(slopeX)));
    int32_t nrmSlopeY = 17 - __clz(((slopeY) > 0 ? (slopeY) : -(slopeY)));

    slopeX = (nrmSlopeX > 0 ? slopeX >> nrmSlopeX : slopeX << (-nrmSlopeX));
    slopeY = (nrmSlopeY > 0 ? slopeY >> nrmSlopeY : slopeY << (-nrmSlopeY));

    for (int_fast16_t y = 0; y < iHeight; y++) {

        int32_t colFirstY =
            __qadd((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t colFirstX =
            __qadd((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t nbVecElts = iWidth;
        int16x8_t vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint32_t *pTargetBaseCur = pTargetBase;

        vX = ((vX) << 6);

        while (nbVecElts > 0) {
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_cccn888_4xssaa_get_pixel_colour_src_mask_opa(
                                &tPointV,
                                &ptParam->tOrigin.tValidRegion,
                                ptParam->tOrigin.pBuffer,
                                iOrigStride,
                                pTargetBaseCur,
                                pOriginMask,
                                iOrigMaskStride,
                                hwOpacity,
                                nbVecElts);

            pTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}


#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */
