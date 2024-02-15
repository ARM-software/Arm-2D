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
 * Project:      Arm-2D RGB565 ACI Library
 * Title:        arm_2d_rgb565_aci_lib.c
 * Description:  Provides RGB565 ACI acceleration
 *
 * $Date:        17. Jan 2024
 * $Revision:    V 1.0.0
 *
 * Target Processor:  Cortex-M cores with Helium & RGB565 Vector CDE
 *
 * -------------------------------------------------------------------- */

#ifdef __ARM_FEATURE_CDE

#define __ARM_2D_IMPL__

#include "arm_2d_rgb565_aci_lib.h"
#include "__arm_2d_math_helium.h"
#include "__arm_2d_utils_helium.h"


bool            __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                              arm_2d_location_t * pSrcPoint,
                                              float fAngle,
                                              float fScale,
                                              arm_2d_location_t * tOffset,
                                              arm_2d_location_t * center,
                                              int32_t iOrigStride,
                                              arm_2d_rot_linear_regr_t regrCoefs[]
    );

/* duplication to be fixed */

#define ARSHIFT(x, shift)   (shift > 0 ? x >> shift : x << (-shift))
#define TO_Q16(x)           ((x) << 16)
#define GET_Q6INT(x)        ((x) >> 6)
#define SET_Q6INT(x)        ((x) << 6)


/* 2 and 4-bit alpha expansions helper tables */
static uint8_t  A2_expand_offs[4 * 3] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2
};

static uint8_t  A2_masks[4 * 3] = {
    0x03, 0x0c, 0x30, 0xc0,
    0x03, 0x0c, 0x30, 0xc0,
    0x03, 0x0c, 0x30, 0xc0
};

static int8_t   A2_masks_shifts[4 * 3] = {
    0x00, 0x02, 0x04, 0x06,
    0x00, 0x02, 0x04, 0x06,
    0x00, 0x02, 0x04, 0x06,
};


static uint8_t  A4_expand_offs[4 * 3] = {
    0, 0, 1, 1,
    2, 2, 3, 3,
    4, 4, 5, 5
};

static uint8_t  A4_masks[4 * 3] = {
    0x0f, 0xf0, 0x0f, 0xf0,
    0x0f, 0xf0, 0x0f, 0xf0,
    0x0f, 0xf0, 0x0f, 0xf0
};

static int8_t   A4_masks_shifts[4 * 3] = {
    0x00, 0x04, 0x00, 0x04,
    0x00, 0x04, 0x00, 0x04,
    0x00, 0x04, 0x00, 0x04,
};


//#define COMPARE_CDE_TEST 1

#if COMPARE_CDE_TEST
#include <stdio.h>
#include "arm_2d_conversion.h"



static uint32_t errorCnt = 0;
static uint32_t okCnt = 0;

#define DUMP_Q15_VEC(vec)   printf("%s { %d %d %d %d %d %d %d %d }\n",#vec, vec[0], vec[1], vec[2], vec[3], vec[4], vec[5], vec[6], vec[7])

int COMPARE_Q15_VEC(uint16x8_t vec1, uint16x8_t vec2)
{
    for (int i = 0; i < 8; i++) {
        if (vec1[i] != vec2[i]) {
            return 0;
        }
    }
    return 1;
}
#endif

static void __arm_2d_cde_rgb565_init(void)
{
#define COPRO_FULL_ACCESS 0x3U

    SCB->CPACR |= COPRO_FULL_ACCESS << (ARM_2D_RGB565_ACI_LIB_COPRO_ID * 2);
    __ISB();
}


__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_colour_filling_with_opacity(uint16_t *
                                                          __RESTRICT pTargetBase,
                                                          int16_t iTargetStride,
                                                          arm_2d_size_t *
                                                          __RESTRICT ptCopySize,
                                                          uint16_t Colour, uint_fast16_t hwRatio)
{

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif
    uint16x8_t      vecColour = vdupq_n_u16(Colour);
    uint16x8_t      vecRatio = vdupq_n_u16((uint16_t) (hwRatio >> 0));
    int32_t         blkCnt;

#if COMPARE_CDE_TEST
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;

    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
    uint16x8_t      vecIn;
    uint16x8_t      vecColorR, vecColorB, vecColorG;

    /* unpack color & scale */
    vecIn = vdupq_n_u16(Colour);
    vecColorR = (vecIn & vecMaskR) * ratio1x8;

    vecColorB = (vecIn >> 11) * ratio1x8;

    vecColorG = vecIn >> 5;
    vecColorG = (vecColorG & vecMaskG) * ratio1x4;
#endif

    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        uint16_t       *phwTarget = pTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

#if COMPARE_CDE_TEST
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack stream */
            vecIn = vld1q(phwTarget);
            vecR1 = vecIn & vecMaskR;

            vecB1 = vecIn >> 11;

            vecG1 = vecIn >> 5;
            vecG1 = vecG1 & vecMaskG;


            /* merge */
            vecR0 = vecColorR + vecR1 * ratio2x8;
            vecR0 = vecR0 >> 8;

            vecG0 = vecColorG + vecG1 * ratio2x4;
            vecG0 = vecG0 >> 8;

            vecB0 = vecColorB + vecB1 * ratio2x8;
            vecB0 = vecB0 >> 8;

            /* pack */
            uint16x8_t      vOut = vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
                | vmulq((vecB0 & vecMaskBpck), 256);


            uint16x8_t      cde = __arm_2d_cde_vrgb565_blend(vecColour,
                                                             vecIn,
                                                             vecRatio);

            if (COMPARE_Q15_VEC(cde, vOut) == 0) {
                printf("\nhwRatio %d\n", hwRatio);
                DUMP_Q15_VEC(vecIn);
                DUMP_Q15_VEC(vecColour);
                DUMP_Q15_VEC(cde);
                DUMP_Q15_VEC(vOut);
            }
#endif
            vst1q_p_u16(phwTarget,
                        __arm_2d_cde_vrgb565_blend_m(vecColour, vld1q_z(phwTarget, p), vecRatio, p),
                        p);

            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pTargetBase += iTargetStride;
    }
}



__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_colour_filling_mask_opacity(uint16_t * __RESTRICT pTarget,
                                                          int16_t iTargetStride,
                                                          uint8_t * __RESTRICT pchAlpha,
                                                          int16_t iAlphaStride,
                                                          arm_2d_size_t * __RESTRICT ptCopySize,
                                                          uint16_t Colour, uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    uint16x8_t      v256 = vdupq_n_u16(256);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *) pchAlpha;
        uint16_t       *pCurTarget = pTarget;
        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);
            uint16x8_t      vecTarget = vld1q_z(pCurTarget, p);
            uint16x8_t      vecTransp = vldrbq_z_u16(pAlpha, p);
            vecTransp = (uint16x8_t) vmulhq((uint8x16_t) vecTransp, vOpacity);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecTransp = vpselq(v256, vecTransp, vcmpeqq_n_u16(vecTransp, 254));
#endif
            uint16x8_t      vecAlpha = vsubq_u16(v256, vecTransp);

            vecTarget = __arm_2d_cde_vrgb565_blend_m(vecTarget, vdupq_n_u16(Colour), vecAlpha, p);


            vst1q_p_u16(pCurTarget, vecTarget, p);
            pAlpha += (8 * 1);
            pCurTarget += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    };

}


__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_tile_copy_opacity(uint16_t * __RESTRICT phwSourceBase,
                                                int16_t iSourceStride,
                                                uint16_t * __RESTRICT phwTargetBase,
                                                int16_t iTargetStride,
                                                arm_2d_size_t * __RESTRICT ptCopySize,
                                                uint_fast16_t hwRatio)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif


    int32_t         blkCnt;
#if COMPARE_CDE_TEST
    uint16_t        ratio1x8 = hwRatio * 8;
    uint16_t        ratio1x4 = hwRatio * 4;
    uint16_t        ratio2x8 = (256 - hwRatio) * 8;
    uint16_t        ratio2x4 = (256 - hwRatio) * 4;
    uint16x8_t      vecMaskR = vdupq_n_u16(0x001f);
    uint16x8_t      vecMaskG = vdupq_n_u16(0x003f);
    uint16x8_t      vecMaskBpck = vdupq_n_u16(0x00f8);
    uint16x8_t      vecMaskGpck = vdupq_n_u16(0x00fc);
#endif

    for (int32_t y = 0; y < ptCopySize->iHeight; y++) {
        const uint16_t *phwSource = phwSourceBase;
        uint16_t       *phwTarget = phwTargetBase;

        blkCnt = ptCopySize->iWidth;
        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

#if COMPARE_CDE_TEST
            uint16x8_t      vecIn;
            uint16x8_t      vecR0, vecB0, vecG0;
            uint16x8_t      vecR1, vecB1, vecG1;

            /* unpack 1st stream */
            vecIn = vld1q(phwSource);
            vecR0 = vecIn & vecMaskR;

            vecB0 = vecIn >> 11;

            vecG0 = vecIn >> 5;
            vecG0 = vecG0 & vecMaskG;


            /* unpack 2nd stream */
            vecIn = vld1q(phwTarget);
            vecR1 = vecIn & vecMaskR;

            vecB1 = vecIn >> 11;

            vecG1 = vecIn >> 5;
            vecG1 = vecG1 & vecMaskG;


            /* merge */
            vecR0 = vecR0 * ratio1x8 + vecR1 * ratio2x8;
            vecR0 = vecR0 >> 8;

            vecG0 = vecG0 * ratio1x4 + vecG1 * ratio2x4;
            vecG0 = vecG0 >> 8;

            vecB0 = vecB0 * ratio1x8 + vecB1 * ratio2x8;
            vecB0 = vecB0 >> 8;


            /* pack */
            uint16x8_t      vOut = vecR0 >> 3 | vmulq((vecG0 & vecMaskGpck), 8)
                | vmulq((vecB0 & vecMaskBpck), 256);

            uint16x8_t      cde = __arm_2d_cde_vrgb565_blend(vld1q(phwSource),
                                                             vld1q(phwTarget),
                                                             vdupq_n_u16(hwRatio));


            if (COMPARE_Q15_VEC(cde, vOut) == 0) {
                errorCnt++;
                printf("error0 %d %d\n", errorCnt, okCnt);
                DUMP_Q15_VEC(cde);
                DUMP_Q15_VEC(vOut);

            } else {
                okCnt++;
                //printf("ok %d %d\n", errorCnt, okCnt);
            }
#else
            uint16x8_t      vOut = __arm_2d_cde_vrgb565_blend_m(vld1q(phwSource),
                                                              vld1q(phwTarget),
                                                              vdupq_n_u16(hwRatio), p);
#endif

            vst1q_p(phwTarget, vOut, p);

            phwSource += 8;
            phwTarget += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        phwSourceBase += iSourceStride;
        phwTargetBase += iTargetStride;
    }
}


__OVERRIDE_WEAK
void __arm_2d_impl_rgb565_tile_copy_colour_keying_opacity(uint16_t * __RESTRICT pSourceBase,
                                                              int16_t iSourceStride,
                                                              uint16_t * __RESTRICT pTargetBase,
                                                              int16_t iTargetStride,
                                                              arm_2d_size_t * __RESTRICT ptCopySize,
                                                              uint_fast16_t hwRatio,
                                                              uint16_t Colour)
{
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

    uint32_t        iHeight = ptCopySize->iHeight;
    uint32_t        iWidth = ptCopySize->iWidth;
    int32_t         blkCnt;


    for (uint32_t y = 0; y < iHeight; y++) {
        const uint16_t *pSource = pSourceBase;
        uint16_t       *pTarget = pTargetBase;
        blkCnt = iWidth;

        while (blkCnt > 0) {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

            uint16x8_t      vecIn = vld1q(pSource);
            uint16x8_t      vecTarget = vld1q(pTargetBase);

            uint16x8_t      vOut = __arm_2d_cde_vrgb565_blend_m(vecIn,
                                                              vecTarget,
                                                              vdupq_n_u16(hwRatio), p);

            vst1q_p(pTarget, vOut, vcmpneq_m_n_s16(vecIn, Colour, p));

            pSource += 8;
            pTarget += 8;
            blkCnt -= 8;
        }

        pSourceBase += iSourceStride;
        pTargetBase += iTargetStride;
    }
}

void __arm_2d_impl_rgb565_src_msk_1h_des_msk_copy(uint16_t * __RESTRICT pSourceBase,
                                                  int16_t iSourceStride,
                                                  uint8_t * __RESTRICT ptSourceMaskBase,
                                                  int16_t iSourceMaskStride,
                                                  arm_2d_size_t * __RESTRICT ptSourceMaskSize,
                                                  uint16_t * __RESTRICT pTargetBase,
                                                  int16_t iTargetStride,
                                                  uint8_t * __RESTRICT ptTargetMaskBase,
                                                  int16_t iTargetMaskStride,
                                                  arm_2d_size_t * __RESTRICT ptTargetMaskSize,
                                                  arm_2d_size_t * __RESTRICT ptCopySize)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint16x8_t      v256 = vdupq_n_u16(256);
    uint8_t        *ptSourceMask = ptSourceMaskBase;
    uint8_t        *ptTargetMask = ptTargetMaskBase;

    for (int_fast16_t y = 0; y < iHeight; y++) {
        uint16_t       *__RESTRICT ptSrc = pSourceBase;
        uint16_t       *__RESTRICT ptTargetCur = pTargetBase;
        uint8_t        *__RESTRICT ptSrcMsk = ptSourceMask;
        uint8_t        *__RESTRICT ptTargetMaskCur = ptTargetMask;

        int32_t         blkCnt = iWidth;

        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

            uint16x8_t      vecTarget = vld1q_z(ptTargetCur, p);
            uint16x8_t      vecSource = vld1q_z(ptSrc, p);
            uint16x8_t      vecSrcMsk = vldrbq_z_u16(ptSrcMsk, p);
            uint16x8_t      vecTargetMask = vldrbq_z_u16(ptTargetMaskCur, p);
            uint16x8_t      vecHwOpacity = vsubq_x(v256, (vecSrcMsk * vecTargetMask) >> 8, p);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecHwOpacity =
                vpselq(vdupq_n_u16(0), vecHwOpacity, vcmpeqq_m_n_u16(vecHwOpacity, 2, p));
#endif


#if COMPARE_CDE_TEST
            uint16x8_t      vecTarget2 =
                __arm_2d_rgb565_blending_opacity_single_vec(vecTarget, vecSource, vecHwOpacity);

            vecTarget = __arm_2d_cde_vrgb565_blend_m(vecTarget, vecSource, vecHwOpacity, p);

            if (COMPARE_Q15_VEC(vecTarget2, vecTarget) == 0) {
                printf("error\n");
                errorCnt++;
            } else
                okCnt++;
#else
            vecTarget = __arm_2d_cde_vrgb565_blend_m(vecTarget, vecSource, vecHwOpacity, p);
#endif

            vst1q_p(ptTargetCur, vecTarget, p);

            ptSrcMsk += 8;
            ptTargetMaskCur += 8;

            ptTargetCur += 8;
            ptSrc += 8;
            blkCnt -= 8;
        }
        while (blkCnt > 0);

        pSourceBase += (iSourceStride);
        pTargetBase += (iTargetStride);
        ptSourceMask += (iSourceMaskStride);
        ptTargetMask = ptTargetMaskBase;
    }
}


static
mve_pred16_t __arm_2d_is_point_vec_inside_region_s16(const arm_2d_region_t * ptRegion,
                                                   const arm_2d_point_s16x8_t * ptPoint)
{
    mve_pred16_t    p0 = vcmpgeq(ptPoint->X, ptRegion->tLocation.iX);
    p0 = vcmpgeq_m(ptPoint->Y, ptRegion->tLocation.iY, p0);
    p0 = vcmpltq_m(ptPoint->X, ptRegion->tLocation.iX + ptRegion->tSize.iWidth, p0);
    p0 = vcmpltq_m(ptPoint->Y, ptRegion->tLocation.iY + ptRegion->tSize.iHeight, p0);

    return p0;
}



static void __arm_2d_impl_rgb565_get_alpha_with_opacity(arm_2d_point_s16x8_t * ptPoint,
                                                        arm_2d_region_t * ptOrigValidRegion,
                                                        uint8_t * pOrigin,
                                                        int16_t iOrigStride,
                                                        uint16_t * pTarget,
                                                        uint16_t MaskColour,
                                                        uint_fast16_t hwOpacity, uint32_t elts)
{
    mve_pred16_t    predTail = vctp16q(elts);
    uint16x8_t      vTarget = vld1q_u16(pTarget);
    int16x8_t       vXi = ((ptPoint->X) >> 6);
    int16x8_t       vYi = ((ptPoint->Y) >> 6);
    uint16x8_t      vHwPixelAlpha;
    mve_pred16_t    predGlb = 0;

#if defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__) &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__
    {
        uint16x8_t      vAvgPixel;
        uint16x8_t      ptVal8;
        uint16x8_t      vAreaTR, vAreaTL, vAreaBR, vAreaBL;
        int16x8_t       vOne = vdupq_n_s16(((1) << 6));
        int16x8_t       vWX = ptPoint->X - ((vXi) << 6);
        int16x8_t       vWY = ptPoint->Y - ((vYi) << 6);
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
            mve_pred16_t    p = __arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t         correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;

            uint8_t        *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);
            vAvgPixel = vrmulhq_u16(vAreaBL, ptVal8);

        }
        {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vYi };
            mve_pred16_t    p = __arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t         correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t        *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaBR, ptVal8));
        }
        {
            arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vaddq_n_s16(vYi, 1) };
            mve_pred16_t    p = __arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t         correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t        *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTL, ptVal8));
        }
        {
            arm_2d_point_s16x8_t vPoint = {.X = vaddq_n_s16(vXi, 1),.Y = vaddq_n_s16(vYi, 1) };
            mve_pred16_t    p = __arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
            predGlb |= p;
            int16_t         correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
            uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
            uint8_t        *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);
            ptVal8 = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);
            vAvgPixel = vqaddq(vAvgPixel, vrmulhq_u16(vAreaTR, ptVal8));
        };


        vHwPixelAlpha = (vAvgPixel);
        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                               vcmphiq_n_u16(vHwPixelAlpha, 255));
    }
#else
    {
        arm_2d_point_s16x8_t vPoint = {.X = vXi,.Y = vYi };
        mve_pred16_t    p = __arm_2d_is_point_vec_inside_region_s16(ptOrigValidRegion, &vPoint);
        predGlb |= p;
        int16_t         correctionOffset = vminvq_s16(32767, vPoint.Y) - 1;
        uint16x8_t      ptOffs = vPoint.X + (vPoint.Y - correctionOffset) * iOrigStride;
        uint8_t        *pOriginCorrected = pOrigin + (correctionOffset * iOrigStride);
        vHwPixelAlpha = vldrbq_gather_offset_z_u16(pOriginCorrected, ptOffs, predTail & p);;

        vHwPixelAlpha = vpselq(vdupq_n_u16(hwOpacity),
                               vrshrq_n_u16(vmulq(vHwPixelAlpha, (uint16_t) hwOpacity), 8),
                               vcmpeqq_n_u16(vHwPixelAlpha, 255));
    }

#endif
    uint16x8_t      vhwTransparency = vdupq_n_u16(256) - vHwPixelAlpha;
    uint16x8_t      vBlended;

#if COMPARE_CDE_TEST
    __arm_2d_color_fast_rgb_t tSrcPix;

    __arm_2d_rgb565_unpack(*(&MaskColour), &tSrcPix);

    vBlended = __arm_2d_rgb565_blending_single_vec_with_scal(vTarget, &tSrcPix, vhwTransparency);

    uint16x8_t      cde =
        __arm_2d_cde_vrgb565_blend(vTarget, vdupq_n_u16(MaskColour), vhwTransparency);


    if (COMPARE_Q15_VEC(cde, vBlended) == 0) {
        printf("error %d %d\n", errorCnt, okCnt);
        DUMP_Q15_VEC(vBlended);
        DUMP_Q15_VEC(cde);
        errorCnt++;
    } else
        //printf("ok %d %d\n", errorCnt, okCnt);
        okCnt++;
#else
    vBlended = __arm_2d_cde_vrgb565_blend(vTarget, vdupq_n_u16(MaskColour), vhwTransparency);
#endif

    vTarget = vpselq_u16(vBlended, vTarget, predGlb);

    vst1q_p(pTarget, vTarget, predTail);
}




__OVERRIDE_WEAK
    void __arm_2d_impl_rgb565_colour_filling_mask_opacity_transform(__arm_2d_param_copy_orig_t *
                                                                    ptParam,
                                                                    __arm_2d_transform_info_t *
                                                                    ptInfo, uint_fast16_t hwRatio)
{
    int32_t         iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int32_t         iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int32_t         iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t       *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint8_t        *pchOrigin = (uint8_t *) ptParam->tOrigin.pBuffer;
    int32_t         iOrigStride = ptParam->tOrigin.iStride;
    uint16_t        MaskColour = ptInfo->Mask.hwColour;
    float           fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    arm_2d_location_t *pCenter = &(ptInfo->tCenter);

    q31_t           invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t SrcPt = ptInfo->tDummySourceOffset;
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwRatio += (hwRatio == 255);
#endif

    __arm_2d_transform_regression(&ptParam->use_as____arm_2d_param_copy_t.tCopySize,
                                  &SrcPt,
                                  fAngle,
                                  ptInfo->fScale, &tOffset, pCenter, iOrigStride, regrCoefs);


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
        int32_t         colFirstY = __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX = __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);

        /* Q6 conversion */
        colFirstX = colFirstX >> 10;
        colFirstY = colFirstY >> 10;

        int32_t         nbVecElts = iWidth;
        int16x8_t       vX = (int16x8_t) vidupq_n_u16(0, 1);
        uint16_t       *pTargetBaseCur = pTargetBase;

        /* Q9.6 coversion */
        vX = SET_Q6INT(vX);

        while (nbVecElts > 0) {
            /* interpolation */
            arm_2d_point_s16x8_t tPointV;

            tPointV.X = vqdmulhq_n_s16(vX, slopeX);
            tPointV.X = vaddq_n_s16(vqrshlq_n_s16(tPointV.X, nrmSlopeX), colFirstX);

            tPointV.Y = vqdmulhq_n_s16(vX, slopeY);
            tPointV.Y = vaddq_n_s16(vqrshlq_n_s16(tPointV.Y, nrmSlopeY), colFirstY);

            __arm_2d_impl_rgb565_get_alpha_with_opacity(&tPointV,
                                                        &ptParam->tOrigin.tValidRegion,
                                                        pchOrigin, iOrigStride,
                                                        pTargetBaseCur,
                                                        MaskColour, hwRatio, nbVecElts);
            pTargetBaseCur += 8;
            vX += SET_Q6INT(8);
            nbVecElts -= 8;
        }
        pTargetBase += iTargetStride;
    }
}




void __arm_2d_impl_rgb565_colour_filling_a2_mask_opacity(uint16_t * __restrict pTarget,
                                                         int16_t iTargetStride,
                                                         uint8_t * __restrict pchAlpha,
                                                         int16_t iAlphaStride,
                                                         int32_t nAlphaOffset,
                                                         arm_2d_size_t * __restrict ptCopySize,
                                                         uint16_t Colour, uint_fast16_t hwOpacity)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    uint8x16_t      vOpacity = vdupq_n_u8(hwOpacity);
    nAlphaOffset &= 0x03;
    iAlphaStride = (iAlphaStride + 3) & ~0x03;
    iAlphaStride >>= 2;

    uint16x8_t      curA2Offs = vldrbq_u16(A2_expand_offs + nAlphaOffset);
    uint16x8_t      curA2Masks = vldrbq_u16(A2_masks + nAlphaOffset);
    int16x8_t       curA2MasksShift = vnegq_s16(vldrbq_s16(A2_masks_shifts + nAlphaOffset));

#if COMPARE_CDE_TEST
    __arm_2d_color_fast_rgb_t tSrcPix;
    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);
#endif

    uint16x8_t      v256 = vdupq_n_u16(256);
    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *) pchAlpha;
        uint16_t       *pCurTarget = pTarget;
        int32_t         blkCnt = iWidth;
        volatile int    cst254 = 254;
        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

            uint16x8_t      vecTarget = vld1q_z(pCurTarget, p);
            uint16x8_t      vecTransp =
                vmulq_x_n_u16(vshlq_x_u16
                              (vandq_x
                               (vldrbq_gather_offset_z_u16(pAlpha, curA2Offs, p), curA2Masks, p),
                               curA2MasksShift, p), 85, p);
            vecTransp = (uint16x8_t) vmulhq_x((uint8x16_t) vecTransp, vOpacity, p);
#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vecTransp = vpselq(vdupq_n_u16(256), vecTransp, vcmpeqq_m_n_u16(vecTransp, cst254, p));
#endif
            uint16x8_t      vecAlpha = vsubq_x_u16(v256, vecTransp, p);

#if COMPARE_CDE_TEST


            uint16x8_t      tmp = vdupq_n_u16(0);
            tmp = vaddq_m_n_u16(tmp, tmp, 1, p);


            uint16x8_t      cde =
                __arm_2d_cde_vrgb565_blend(vecTarget, vdupq_n_u16(Colour), vecAlpha);


            uint16x8_t      vecR, vecG, vecB;
            __arm_2d_rgb565_unpack_single_vec(vecTarget, &vecR, &vecG, &vecB);
            vecR = vmulq(vecR, vecAlpha);
            vecR = vmlaq(vecR, vecTransp, (uint16_t) tSrcPix.R);
            vecR = vecR >> 8;
            vecG = vmulq(vecG, vecAlpha);
            vecG = vmlaq(vecG, vecTransp, (uint16_t) tSrcPix.G);
            vecG = vecG >> 8;
            vecB = vmulq(vecB, vecAlpha);
            vecB = vmlaq(vecB, vecTransp, (uint16_t) tSrcPix.B);
            vecB = vecB >> 8;
            vecTarget = __arm_2d_rgb565_pack_single_vec(vecR, vecG, vecB);


            vecTarget = vecTarget * tmp;
            cde = cde * tmp;

            if (COMPARE_Q15_VEC(cde, vecTarget) == 0) {
                printf("errorz %d %d %x\n", errorCnt++, okCnt, p);
                DUMP_Q15_VEC(vecTarget);
                DUMP_Q15_VEC(cde);
            } else
                okCnt++;
            //   printf("ok2 %d %d\n", errorCnt, okCnt++);
#else
            vecTarget = __arm_2d_cde_vrgb565_blend_m(vecTarget, vdupq_n_u16(Colour), vecAlpha, p);;
#endif


            vst1q_p_u16(pCurTarget, vecTarget, p);
            pAlpha += (8 * 1 / 4);
            pCurTarget += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);
        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    };

}


void __arm_2d_impl_rgb565_colour_filling_a4_mask(uint16_t * __restrict pTarget,
                                                 int16_t iTargetStride,
                                                 uint8_t * __restrict pchAlpha,
                                                 int16_t iAlphaStride,
                                                 int32_t nAlphaOffset,
                                                 arm_2d_size_t *
                                                 __restrict ptCopySize, uint16_t Colour)
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    nAlphaOffset &= 0x01;
    iAlphaStride = (iAlphaStride + 1) & ~0x01;
    iAlphaStride >>= 1;


    uint16x8_t      curA4Offs = vldrbq_u16(A4_expand_offs + nAlphaOffset);
    uint16x8_t      curA4Masks = vldrbq_u16(A4_masks + nAlphaOffset);
    int16x8_t       curA4MasksShift = vnegq_s16(vldrbq_s16(A4_masks_shifts + nAlphaOffset));

#if COMPARE_CDE_TEST
    __arm_2d_color_fast_rgb_t tSrcPix;
    __arm_2d_rgb565_unpack(*(&Colour), &tSrcPix);
#endif

    uint16x8_t      v256 = vdupq_n_u16(256);
    for (int_fast16_t y = 0; y < iHeight; y++) {
        const uint8_t  *pAlpha = (const uint8_t *) pchAlpha;
        uint16_t       *pCurTarget = pTarget;
        int32_t         blkCnt = iWidth;
        volatile int    cst255 = 255;
        do {
            mve_pred16_t    p = vctp16q((uint32_t) blkCnt);

            uint16x8_t      vecTarget = vld1q_z(pCurTarget, p);
            uint16x8_t      vecTransp =
                vmulq_x_n_u16(vshlq_x_u16
                              (vandq_x
                               (vldrbq_gather_offset_z_u16(pAlpha, curA4Offs, p), curA4Masks, p),
                               curA4MasksShift, p), 17, p);
            vecTransp = vecTransp;
            vecTransp = vpselq(vdupq_n_u16(256), vecTransp, vcmpeqq_m_n_u16(vecTransp, cst255, p));
            uint16x8_t      vecAlpha = vsubq_x_u16(v256, vecTransp, p);

#if COMPARE_CDE_TEST

            uint16x8_t      tmp = vdupq_n_u16(0);
            tmp = vaddq_m_n_u16(tmp, tmp, 1, p);


            uint16x8_t      cde =
                __arm_2d_cde_vrgb565_blend(vecTarget, vdupq_n_u16(Colour), vecAlpha);

            uint16x8_t      vecR, vecG, vecB;
            __arm_2d_rgb565_unpack_single_vec(vecTarget, &vecR, &vecG, &vecB);

            vecR = vmulq(vecR, vecAlpha);
            vecR = vmlaq(vecR, vecTransp, (uint16_t) tSrcPix.R);
            vecR = vecR >> 8;
            vecG = vmulq(vecG, vecAlpha);
            vecG = vmlaq(vecG, vecTransp, (uint16_t) tSrcPix.G);
            vecG = vecG >> 8;
            vecB = vmulq(vecB, vecAlpha);
            vecB = vmlaq(vecB, vecTransp, (uint16_t) tSrcPix.B);
            vecB = vecB >> 8;



            vecTarget = __arm_2d_rgb565_pack_single_vec(vecR, vecG, vecB);

            vecTarget = vecTarget * tmp;
            cde = cde * tmp;
            if (COMPARE_Q15_VEC(cde, vecTarget) == 0) {
                printf("error3  %d %d %x\n", errorCnt++, okCnt, p);
                DUMP_Q15_VEC(vecR);
                DUMP_Q15_VEC(vecG);
                DUMP_Q15_VEC(vecB);
                DUMP_Q15_VEC(vecTarget);
                DUMP_Q15_VEC(cde);

            } else
                okCnt++;
            //   printf("ok3 %d %d\n", errorCnt, okCnt++);
#else
            vecTarget = __arm_2d_cde_vrgb565_blend_m(vecTarget, vdupq_n_u16(Colour), vecAlpha, p);;
#endif
            vst1q_p_u16(pCurTarget, vecTarget, p);
            pAlpha += (8 * 1 / 2);
            pCurTarget += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchAlpha += (iAlphaStride);
        pTarget += (iTargetStride);
    };

}


#endif

void __arm_2d_aci_init(void)
{
    if (__ARM_FEATURE_CDE_COPROC & (1 << ARM_2D_RGB565_ACI_LIB_COPRO_ID))
        __arm_2d_cde_rgb565_init();
}
