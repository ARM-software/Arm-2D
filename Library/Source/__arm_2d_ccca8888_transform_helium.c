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
 * Title:        __arm_2d_ccca8888_transform_helium.c
 * Description:  The source code of APIs for ccca8888 transform to other 
 *               colour formats: gray8, rgb565 and cccn888
 * 
 * $Date:        25. April 2025
 * $Revision:    V.1.1.0
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

/*
 * gray8
 */

static 
void __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8_get_pixel_colour(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint8_t *pchTarget,
    uint16_t hwOpacity,
    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    mve_pred16_t predTail = vctp16q(elts);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlb = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vldrbq_u16(pchTarget);
    uint16x8_t vAvg = vAvgG + vAvgR + vAvgB;
    vAvg = vAvg / 3;
    vAvg = vqaddq(vAvg, vrmulhq(vTarget, vAvgTrans));
    vAvg = vminq(vAvg, vdupq_n_u16(255));

    vTarget = vpselq_u16(vAvg, vTarget, predGlb);
    vstrbq_p_u16(pchTarget, vTarget, predTail);

}

static void __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8_get_pixel_colour_inside_src(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint8_t *pchTarget,

    uint16_t hwOpacity,

    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;

        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        ;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        ;
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vldrbq_u16(pchTarget);
    uint16x8_t vAvg = vAvgG + vAvgR + vAvgB;
    vAvg = vAvg / 3;
    vAvg = vqaddq(vAvg, vrmulhq(vTarget, vAvgTrans));
    vAvg = vminq(vAvg, vdupq_n_u16(255));

    vstrbq_u16(pchTarget, vAvg);

}


__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo,
    uint_fast16_t hwOpacity)
{

    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint8_t *pchTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    hwOpacity += (hwOpacity == 255);

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptY 
                                 -  regrCoefs[0].interceptY))
                       * (q63_t)(invIWidth)) 
                >> 32);
    slopeX =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptX 
                                 -  regrCoefs[0].interceptX))
                       * (q63_t)(invIWidth)) 
                >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetBaseCur = pchTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pchTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pchTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            }

            pchTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pchTargetBase += iTargetStride;
    }
}



static 
void __arm_2d_impl_ccca8888_transform_to_gray8_get_pixel_colour(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    mve_pred16_t predTail = vctp16q(elts);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlb = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vldrbq_u16(pchTarget);
    uint16x8_t vAvg = vAvgG + vAvgR + vAvgB;
    vAvg = vAvg / 3;
    vAvg = vqaddq(vAvg, vrmulhq(vTarget, vAvgTrans));
    vAvg = vminq(vAvg, vdupq_n_u16(255));

    vTarget = vpselq_u16(vAvg, vTarget, predGlb);
    vstrbq_p_u16(pchTarget, vTarget, predTail);
}

static 
void __arm_2d_impl_ccca8888_transform_to_gray8_get_pixel_colour_inside_src(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vldrbq_u16(pchTarget);
    uint16x8_t vAvg = vAvgG + vAvgR + vAvgB;
    vAvg = vAvg / 3;
    vAvg = vqaddq(vAvg, vrmulhq(vTarget, vAvgTrans));
    vAvg = vminq(vAvg, vdupq_n_u16(255));
    vstrbq_u16(pchTarget, vAvg);

}



__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_to_gray8(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo)
{
    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint8_t *pchTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint8_t *pchTargetBaseCur = pchTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_to_gray8_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pchTargetBaseCur,
                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_to_gray8_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pchTargetBaseCur,
                    nbVecElts);
            }

            pchTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pchTargetBase += iTargetStride;
    }
}

/*
 * rgb565
 */

static 
void __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565_get_pixel_colour(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint16_t *phwTarget,
    uint16_t hwOpacity,
    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    mve_pred16_t predTail = vctp16q(elts);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlb = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vBlended;
    uint16x8_t vTarget = vld1q(phwTarget);

    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);

}

static void __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565_get_pixel_colour_inside_src(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint16_t *phwTarget,

    uint16_t hwOpacity,

    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;

        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        ;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        ;
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vld1q(phwTarget);
    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    vst1q(phwTarget, __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB));

}


__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo,
    uint_fast16_t hwOpacity)
{

    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t *phwTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    hwOpacity += (hwOpacity == 255);

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptY 
                                 -  regrCoefs[0].interceptY))
                       * (q63_t)(invIWidth)) 
                >> 32);
    slopeX =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptX 
                                 -  regrCoefs[0].interceptX))
                       * (q63_t)(invIWidth)) 
                >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetBaseCur = phwTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    phwTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    phwTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            }

            phwTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        phwTargetBase += iTargetStride;
    }
}



static 
void __arm_2d_impl_ccca8888_transform_to_rgb565_get_pixel_colour(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;
    mve_pred16_t predTail = vctp16q(elts);

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlb = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p = arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vBlended;
    uint16x8_t vTarget = vld1q(phwTarget);

    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    vBlended = __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB);
    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(phwTarget, vTarget, predTail);
}

static 
void __arm_2d_impl_ccca8888_transform_to_rgb565_get_pixel_colour_inside_src(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    uint16x8_t vTarget = vld1q(phwTarget);
    __arm_2d_rgb565_unpack_single_vec(vTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    vst1q(phwTarget, __arm_2d_rgb565_pack_single_vec(vAvgR, vAvgG, vAvgB));

}



__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_to_rgb565(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo)
{
    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t *phwTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint16_t *phwTargetBaseCur = phwTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_to_rgb565_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    phwTargetBaseCur,
                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_to_rgb565_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    phwTargetBaseCur,
                    nbVecElts);
            }

            phwTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        phwTargetBase += iTargetStride;
    }
}

/*
 * cccn888
 */

static 
void __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888_get_pixel_colour(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint32_t *pwTarget,
    uint16_t hwOpacity,
    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlbLo = 0, predGlbHi = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    __arm_2d_unpack_cccn888_from_mem((const uint8_t *)pwTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    uint32x4_t vTargetLo = vld1q(pwTarget);
    uint32x4_t vTargetHi = vld1q(pwTarget + 4);
    __arm_2d_pack_rgb888_to_mem((uint8_t *)scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t TempPixel = vld1q(scratch32);
    TempPixel = vpselq_u32(TempPixel, vTargetLo, predGlbLo);
    vst1q_p(pwTarget, TempPixel, predTailLow);

    TempPixel = vld1q(scratch32 + 4);
    TempPixel = vpselq_u32(TempPixel, vTargetHi, predGlbHi);

    vst1q_p(pwTarget + 4, TempPixel, predTailHigh);

}

static void __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888_get_pixel_colour_inside_src(
    arm_2d_point_s16x8_t *ptPoint,
    arm_2d_region_t *ptOrigValidRegion,
    uint32_t *pOrigin,
    int16_t iOrigStride,
    uint32_t *pwTarget,

    uint16_t hwOpacity,

    uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;

        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        ;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));
        ;
        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_unpack_cccn888_from_mem((const uint8_t *)pwTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    __arm_2d_pack_rgb888_to_mem((uint8_t *)scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t TempPixel = vld1q(scratch32);
    vst1q(pwTarget, TempPixel);

    TempPixel = vld1q(scratch32 + 4);
    vst1q(pwTarget + 4, TempPixel);

}


__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo,
    uint_fast16_t hwOpacity)
{

    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint32_t *pwTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    hwOpacity += (hwOpacity == 255);

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptY 
                                 -  regrCoefs[0].interceptY))
                       * (q63_t)(invIWidth)) 
                >> 32);
    slopeX =
        (q31_t)((q63_t)( (q63_t)((  regrCoefs[1].interceptX 
                                 -  regrCoefs[0].interceptX))
                       * (q63_t)(invIWidth)) 
                >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetBaseCur = pwTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pwTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pwTargetBaseCur,

                    hwOpacity,

                    nbVecElts);
            }

            pwTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pwTargetBase += iTargetStride;
    }
}



static 
void __arm_2d_impl_ccca8888_transform_to_cccn888_get_pixel_colour(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    mve_pred16_t predTailLow = vctp32q(elts);
    mve_pred16_t predTailHigh = elts - 4 > 0 ? vctp32q(elts - 4) : 0;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;
    mve_pred16_t predGlbLo = 0, predGlbHi = 0;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        mve_pred16_t p;
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);
    
        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        mve_pred16_t p;

        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointLo);
        predGlbLo |= p;
        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailLow & p);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        p = arm_2d_is_point_vec_inside_region_s32(ptOrigValidRegion, &tPointHi);
        predGlbHi |= p;
        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_z_u32(pOrigin, ptOffs, predTailHigh & p);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;
    __arm_2d_unpack_cccn888_from_mem((const uint8_t *)pwTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    uint32x4_t vTargetLo = vld1q(pwTarget);
    uint32x4_t vTargetHi = vld1q(pwTarget + 4);

    __arm_2d_pack_rgb888_to_mem((uint8_t *)scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t vTempPixel = vld1q(scratch32);
    vTempPixel = vpselq_u32(vTempPixel, vTargetLo, predGlbLo);
    vst1q_p(pwTarget, vTempPixel, predTailLow);

    vTempPixel = vld1q(scratch32 + 4);
    vTempPixel = vpselq_u32(vTempPixel, vTargetHi, predGlbHi);
    vst1q_p(pwTarget + 4, vTempPixel, predTailHigh);
}

static 
void __arm_2d_impl_ccca8888_transform_to_cccn888_get_pixel_colour_inside_src(
                                            arm_2d_point_s16x8_t *ptPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            uint32_t elts)
{
    int16_t iOrigmaskStride = iOrigStride * 4;
    uint8_t *pchOrigMask = (uint8_t *)((uintptr_t)pOrigin + 3);

    __attribute__((aligned(8))) uint32_t scratch32[32];
    int16_t *pscratch16 = (int16_t *)scratch32;

    int16x8_t vXi = ((ptPoint->X) >> 6);
    int16x8_t vYi = ((ptPoint->Y) >> 6);

    uint16x8_t vAvgPixelR, vAvgPixelG, vAvgPixelB;

    uint16x8_t vAvgTransparency;
    uint16x8_t vAvgR, vAvgG, vAvgB, vAvgTrans;

    uint16x8_t R, G, B, vPixelAlpha;
    uint16x8_t vAreaTR, vAreaTL, vAreaBR, vAreaBL;
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
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBL >> 8), vPixelAlpha);
        vAvgTransparency = vAreaBL - vAlpha;
        vAvgPixelR = vrmulhq_u16(vAlpha, R);
        vAvgPixelG = vrmulhq_u16(vAlpha, G);
        vAvgPixelB = vrmulhq_u16(vAlpha, B);
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vYi};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaBR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaBR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vXi, .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTL >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTL - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    do {
        arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1), .Y = vaddq_n_s16(vYi, 1)};
        arm_2d_point_s32x4_t tPointLo, tPointHi;
        vst1q(pscratch16, vPoint.X);
        tPointLo.X = vldrhq_s32(pscratch16);
        tPointHi.X = vldrhq_s32(pscratch16 + 4);
        vst1q(pscratch16, vPoint.Y);
        tPointLo.Y = vldrhq_s32(pscratch16);
        tPointHi.Y = vldrhq_s32(pscratch16 + 4);

        uint32x4_t ptOffs = tPointLo.X + tPointLo.Y * iOrigStride;
        uint32x4_t ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32, ptVal);
        uint32x4_t maskVal = ptVal >> 24;
        vst1q(scratch32 + 8, maskVal);

        ptOffs = tPointHi.X + tPointHi.Y * iOrigStride;
        ptVal = vldrwq_gather_shifted_offset_u32(pOrigin, ptOffs);
        vst1q(scratch32 + 4, ptVal);
        maskVal = ptVal >> 24;
        vst1q(scratch32 + 12, maskVal);

        __arm_2d_unpack_cccn888_from_mem((uint8_t *)scratch32, &R, &G, &B);
        vPixelAlpha = vldrbq_gather_offset_u16((uint8_t *)&scratch32[8], vidupq_n_u16(0, 4));
        //vPixelAlpha = (vPixelAlpha * hwOpacity) >> 8;
        vPixelAlpha = vpselq(vdupq_n_u16(256), vPixelAlpha, vcmpeqq_n_u16(vPixelAlpha, 255));

        uint16x8_t vAlpha = vmulq_u16((vAreaTR >> 8), vPixelAlpha);
        vAvgTransparency = vqaddq(vAvgTransparency, vAreaTR - vAlpha);
        vAvgPixelR = vqaddq(vAvgPixelR, vrmulhq_u16(vAlpha, R));
        vAvgPixelG = vqaddq(vAvgPixelG, vrmulhq_u16(vAlpha, G));
        vAvgPixelB = vqaddq(vAvgPixelB, vrmulhq_u16(vAlpha, B));
    } while(0);

    vAvgR = (vAvgPixelR);
    vAvgG = (vAvgPixelG);
    vAvgB = (vAvgPixelB);
    vAvgTrans = (vAvgTransparency);

    uint16x8_t vTargetR, vTargetG, vTargetB;

    __arm_2d_unpack_cccn888_from_mem((const uint8_t *)pwTarget, &vTargetR, &vTargetG, &vTargetB);

    vAvgR = vqaddq(vAvgR, vrmulhq(vTargetR, vAvgTrans));
    vAvgR = vminq(vAvgR, vdupq_n_u16(255));
    vAvgG = vqaddq(vAvgG, vrmulhq(vTargetG, vAvgTrans));
    vAvgG = vminq(vAvgG, vdupq_n_u16(255));
    vAvgB = vqaddq(vAvgB, vrmulhq(vTargetB, vAvgTrans));
    vAvgB = vminq(vAvgB, vdupq_n_u16(255));

    __arm_2d_pack_rgb888_to_mem((uint8_t *)scratch32, vAvgR, vAvgG, vAvgB);

    uint32x4_t TempPixel = vld1q(scratch32);
    vst1q(pwTarget, TempPixel);

    TempPixel = vld1q(scratch32 + 4);
    vst1q(pwTarget + 4, TempPixel);

}



__OVERRIDE_WEAK
void __arm_2d_impl_ccca8888_transform_to_cccn888(
    __arm_2d_param_copy_orig_t *ptParam,
    __arm_2d_transform_info_t *ptInfo)
{
    int_fast16_t iHeight 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth 
        = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint32_t *pwTargetBase 
        = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset 
        = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;

    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
        &tOffset,
        &(ptInfo->tCenter),
        iOrigStride,
        regrCoefs);

    int32_t slopeY, slopeX;

    slopeY =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptY - regrCoefs[0].interceptY)) * (q63_t)(invIWidth)) >> 32);
    slopeX =
        (q31_t)((q63_t)((q63_t)((regrCoefs[1].interceptX - regrCoefs[0].interceptX)) * (q63_t)(invIWidth)) >> 32);

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
        int16x8_t vX = (int16x8_t)vidupq_n_u16(0, 1);
        uint32_t *pwTargetBaseCur = pwTargetBase;

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

            if (0xFFFF == p) {
                __arm_2d_impl_ccca8888_transform_to_cccn888_get_pixel_colour_inside_src(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pwTargetBaseCur,
                    nbVecElts);
            } else if (0 != p) {
                __arm_2d_impl_ccca8888_transform_to_cccn888_get_pixel_colour(
                    &tPointV,
                    &ptParam->tOrigin.tValidRegion,
                    ptParam->tOrigin.pBuffer,
                    iOrigStride,
                    pwTargetBaseCur,
                    nbVecElts);
            }

            pwTargetBaseCur += 8;
            vX += ((8) << 6);
            nbVecElts -= 8;
        }
        pwTargetBase += iTargetStride;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */