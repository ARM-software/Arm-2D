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
 * Title:        arm-2d_transform.c
 * Description:  APIs for tile transform
 *
 * $Date:        17 April 2025
 * $Revision:    V.2.3.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/
#define __ARM_2D_IMPL__

#include "arm_2d.h"
#include "__arm_2d_impl.h"
#include "math.h"


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
#   pragma clang diagnostic ignored "-Wfloat-conversion"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wundef"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#   pragma diag_suppress 174,177,188,68,513,144,1296
#elif defined(__IS_COMPILER_IAR__)
#   pragma diag_suppress=Pa093,Pa021
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

#include <arm_math.h>

/*============================ MACROS ========================================*/
#undef __PI
#define __PI                3.1415926f

#define __CALIB             0.009f
/* faster ATAN */
#define FAST_ATAN_F32_1(x, xabs)    \
                            (x * (PI / 4.0f) + 0.273f * x * (1.0f - xabs))
#define EPS_ATAN2           1e-5f

/*----------------------------------------------------------------------------*
 * Code Template                                                              *
 *----------------------------------------------------------------------------*/

#define __API_PIXEL_AVERAGE_RESULT_GRAY8()                      \
    (   tPixel >> 8)

#define __API_PIXEL_AVERAGE_RESULT_RGB565()                     \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        __arm_2d_rgb565_pack(&tPixel))

#define __API_PIXEL_AVERAGE_RESULT_CCCN888()                    \
    (   tPixel.R >>= 8,                                         \
        tPixel.G >>= 8,                                         \
        tPixel.B >>= 8,                                         \
        tPixel.A = *((uint8_t *)pTarget + 3),                   \
        __arm_2d_ccca888_pack(&tPixel))

#define __API_CCCA8888_PIXEL_AVERAGE_RESULT_GRAY8()             \
        ((((int32_t)tPixel.R + (int32_t)tPixel.G + (int32_t)tPixel.B) / 3) >> 8)

#define __API_COLOUR                gray8
#define __API_COLOUR_UPPER_CASE     GRAY8
#define __API_INT_TYPE              uint8_t
#define __API_INT_TYPE_BIT_NUM      8
#define __API_PIXEL_AVERAGE_INIT()  uint16_t tPixel = 0;
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_GRAY8
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_GRAY8
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_GRAY8
#include "__arm_2d_ll_transform.inc"

#define __API_COLOUR                rgb565
#define __API_COLOUR_UPPER_CASE     RGB565
#define __API_INT_TYPE              uint16_t
#define __API_INT_TYPE_BIT_NUM      16
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_RGB565
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_RGB565
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_RGB565
#include "__arm_2d_ll_transform.inc"

#define __API_COLOUR                cccn888
#define __API_COLOUR_UPPER_CASE     CCCN888
#define __API_INT_TYPE              uint32_t
#define __API_INT_TYPE_BIT_NUM      32
#define __API_PIXEL_BLENDING        __ARM_2D_PIXEL_BLENDING_OPA_CCCN888
#define __API_PIXEL_AVERAGE         __ARM_2D_PIXEL_AVERAGE_CCCN888
#define __API_PIXEL_AVERAGE_RESULT  __API_PIXEL_AVERAGE_RESULT_CCCN888

#include "__arm_2d_ll_transform.inc"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Utilities                                                                  *
 *----------------------------------------------------------------------------*/

static
void __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                arm_2d_location_t * pSrcPoint,
                                float fAngle,
                                float fScaleX,
                                float fScaleY,
                                arm_2d_location_t * tOffset,
                                arm_2d_point_float_t * ptCenter,
                                arm_2d_rot_linear_regr_t regrCoefs[])
{
#define ONE_BY_2PI_Q31      341782637.0f

    int_fast16_t        iHeight = ptCopySize->iHeight;
    int_fast16_t        iWidth = ptCopySize->iWidth;
    q31_t               invHeightFx;

    if (1 == iHeight) {
        invHeightFx = 0x7fffffff;
    } else {
        invHeightFx = 0x7fffffff / (iHeight - 1);
    }

    int32_t             AngleFx = ARM_2D_LROUNDF(fAngle * ONE_BY_2PI_Q31);
    int32_t             ScaleXFx = reinterpret_q16_f32(fScaleX);
    int32_t             ScaleYFx = reinterpret_q16_f32(fScaleY);

    q16_t               cosAngleFx = reinterpret_q16_q31(arm_cos_q31(AngleFx));
    q16_t               sinAngleFx = reinterpret_q16_q31(arm_sin_q31(AngleFx));

    arm_2d_point_fx_t   tPointCornerFx[2][2];
    arm_2d_point_fx_t   centerQ16;
    arm_2d_point_fx_t   srcPointQ16;
    arm_2d_point_fx_t   tOffsetQ16;
    arm_2d_point_fx_t   tmp;
    arm_2d_point_q16_t  tPoint;  


    /* Q16 conversion */
    centerQ16.q16X = reinterpret_q16_f32(ptCenter->fX);
    centerQ16.q16Y = reinterpret_q16_f32(ptCenter->fY);

    srcPointQ16.q16X = reinterpret_q16_s16(pSrcPoint->iX);
    srcPointQ16.q16Y = reinterpret_q16_s16(pSrcPoint->iY);

    tOffsetQ16.q16X = reinterpret_q16_s16(tOffset->iX);
    tOffsetQ16.q16Y = reinterpret_q16_s16(tOffset->iY);


    /* (0,0) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X;
    tmp.q16Y = srcPointQ16.q16Y + 0 + tOffsetQ16.q16Y;

    tPoint.q16X = tmp.q16X - centerQ16.q16X;
    tPoint.q16Y = tmp.q16Y - centerQ16.q16Y;

#define __PT_TRANSFORM(__PT) \
    do {                                                                            \
        /* rotation first, then scaling */                                          \
        __PT.q16Y =qadd_q16(   centerQ16.q16Y,                                      \
                            mul_q16(qadd_q16(   mul_q16(tPoint.q16Y, cosAngleFx),   \
                                                mul_q16(tPoint.q16X, sinAngleFx)),  \
                                    ScaleYFx));                                     \
        __PT.q16X =qadd_q16(   centerQ16.q16X,                                      \
                            mul_q16(qsub_q16(   mul_q16(tPoint.q16X, cosAngleFx),   \
                                                mul_q16(tPoint.q16Y, sinAngleFx)),  \
                                    ScaleXFx));                                     \
    } while(0)

    __PT_TRANSFORM(tPointCornerFx[0][0]);

    /* ((iWidth - 1),0) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X + reinterpret_q16_s16(iWidth - 1);
    tPoint.q16X = tmp.q16X - centerQ16.q16X;


    __PT_TRANSFORM(tPointCornerFx[1][0]);

    /* ((iWidth - 1),(iHeight - 1)) corner */
    tmp.q16Y = srcPointQ16.q16Y + tOffsetQ16.q16Y + reinterpret_q16_s16(iHeight - 1);
    tPoint.q16Y = tmp.q16Y - centerQ16.q16Y;

    __PT_TRANSFORM(tPointCornerFx[1][1]);

    /* (0,(iHeight - 1)) corner */
    tmp.q16X = srcPointQ16.q16X + 0 + tOffsetQ16.q16X;
    tPoint.q16X = tmp.q16X - centerQ16.q16X;

    __PT_TRANSFORM(tPointCornerFx[0][1]);

    /* regression */
    int32_t           slopeXFx, slopeYFx;

    /* interpolation in Y direction for 1st elements column */
    slopeXFx = MULTFX(  (tPointCornerFx[0][1].q16X - tPointCornerFx[0][0].q16X),
                        invHeightFx);
    slopeYFx = MULTFX((tPointCornerFx[0][1].q16Y - tPointCornerFx[0][0].q16Y),
                        invHeightFx);

    regrCoefs[0].slopeY = slopeYFx * 2;
    regrCoefs[0].slopeX = slopeXFx * 2;
    regrCoefs[0].interceptY = tPointCornerFx[0][0].q16Y;
    regrCoefs[0].interceptX = tPointCornerFx[0][0].q16X;


    /* interpolation in Y direction for the last elements column */
    slopeXFx = MULTFX(  (tPointCornerFx[1][1].q16X - tPointCornerFx[1][0].q16X),
                        invHeightFx);
    slopeYFx = MULTFX(  (tPointCornerFx[1][1].q16Y - tPointCornerFx[1][0].q16Y),
                        invHeightFx);

    regrCoefs[1].slopeY = slopeYFx* 2;
    regrCoefs[1].slopeX = slopeXFx* 2;
    regrCoefs[1].interceptY = tPointCornerFx[1][0].q16Y;
    regrCoefs[1].interceptX = tPointCornerFx[1][0].q16X;
}


#if 0 /* we keep this original algorithm for reference, please do NOT remove it */

static
void __arm_2d_transform_regression(arm_2d_size_t * __RESTRICT ptCopySize,
                                arm_2d_location_t * pSrcPoint,
                                float fAngle,
                                float fScaleX,
                                float fScaleY,
                                arm_2d_location_t * tOffset,
                                arm_2d_location_t * ptCenter,
                                arm_2d_rot_linear_regr_t regrCoefs[])
{
    int_fast16_t    iHeight = ptCopySize->iHeight;
    int_fast16_t    iWidth = ptCopySize->iWidth;
    float           invHeight;

    if (1 == iHeight ) {
        invHeight = __LARGEINVF32;
    } else {
        invHeight = 1.0f / (float) (iHeight - 1);
    }

    float           cosAngle = arm_cos_f32(fAngle);
    float           sinAngle = arm_sin_f32(fAngle);
    arm_2d_location_t tSrcPoint;
    arm_2d_point_float_t tPointCorner[2][2];
    int16_t         iX, iY;

    tSrcPoint.iX = pSrcPoint->iX + 0 + tOffset->iX;
    tSrcPoint.iY = pSrcPoint->iY + 0 + tOffset->iY;

    iX = tSrcPoint.iX - ptCenter->iX;
    iY = tSrcPoint.iY - ptCenter->iY;

    tPointCorner[0][0].fY = (iY * cosAngle + iX * sinAngle) * fScaleY + ptCenter->iY;
    tPointCorner[0][0].fX = (iX * cosAngle - iY * sinAngle) * fScaleX + ptCenter->iX;

    tSrcPoint.iX = pSrcPoint->iX + (iWidth - 1) + tOffset->iX;
    iX = tSrcPoint.iX - ptCenter->iX;

    tPointCorner[1][0].fY = (iY * cosAngle + iX * sinAngle) * fScaleY + ptCenter->iY;
    tPointCorner[1][0].fX = (iX * cosAngle - iY * sinAngle) * fScaleX + ptCenter->iX;

    tSrcPoint.iY = pSrcPoint->iY + (iHeight - 1) + tOffset->iY;
    iY = tSrcPoint.iY - ptCenter->iY;

    tPointCorner[1][1].fY = (iY * cosAngle + iX * sinAngle) * fScaleY + ptCenter->iY;
    tPointCorner[1][1].fX = (iX * cosAngle - iY * sinAngle) * fScaleX + ptCenter->iX;

    tSrcPoint.iX = pSrcPoint->iX + 0 + tOffset->iX;
    iX = tSrcPoint.iX - ptCenter->iX;

    tPointCorner[0][1].fY = (iY * cosAngle + iX * sinAngle) * fScaleY + ptCenter->iY;
    tPointCorner[0][1].fX = (iX * cosAngle - iY * sinAngle) * fScaleX + ptCenter->iX;

    float           slopeX, slopeY;

    /* interpolation in Y direction for 1st elements column */
    slopeX = (tPointCorner[0][1].fX - tPointCorner[0][0].fX) * invHeight;
    slopeY = (tPointCorner[0][1].fY - tPointCorner[0][0].fY) * invHeight;

    regrCoefs[0].slopeY = slopeY;
    regrCoefs[0].slopeX = slopeX;
    regrCoefs[0].interceptY = tPointCorner[0][0].fY;
    regrCoefs[0].interceptX = tPointCorner[0][0].fX;


    /* interpolation in Y direction for the last elements column */
    slopeX = (tPointCorner[1][1].fX - tPointCorner[1][0].fX) * invHeight;
    slopeY = (tPointCorner[1][1].fY - tPointCorner[1][0].fY) * invHeight;

    regrCoefs[1].slopeY = slopeY;
    regrCoefs[1].slopeX = slopeX;
    regrCoefs[1].interceptY = tPointCorner[1][0].fY;
    regrCoefs[1].interceptX = tPointCorner[1][0].fX;
}

#endif


ARM_NONNULL(1,2,6)
static
arm_2d_point_float_t *__arm_2d_transform_point(
                                            const arm_2d_location_t *ptLocation,
                                            const arm_2d_point_float_t *ptCenter,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            arm_2d_point_float_t *ptOutBuffer)
{
    /* scaling first, then rotate */ 
    float fX0 = ((float)ptLocation->iX - ptCenter->fX) * fScaleX;
    float fY0 = ((float)ptLocation->iY - ptCenter->fY) * fScaleY;

    float           cosAngle = arm_cos_f32(fAngle);
    float           sinAngle = arm_sin_f32(fAngle);

    float fY = (fY0 * cosAngle + fX0 * sinAngle);
    float fX = (fX0 * cosAngle - fY0 * sinAngle);

    fX += ptCenter->fX;
    fY += ptCenter->fY;

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
    if (fX > 0) {
        ptOutBuffer->fX = fX + __CALIB;
    } else {
        ptOutBuffer->fX = fX - __CALIB;
    }
    if (fY > 0) {
        ptOutBuffer->fY = fY + __CALIB;
    } else {
        ptOutBuffer->fY = fY - __CALIB;
    }
#else
    ptOutBuffer->fX = fX;
    ptOutBuffer->fY = fY;
#endif

    return ptOutBuffer;
}


static arm_2d_err_t __arm_2d_transform_preprocess_source(
                                        arm_2d_op_trans_t *ptThis,
                                        __arm_2d_transform_info_t *ptTransform)
{
    arm_2d_tile_t *ptSource = (arm_2d_tile_t *)this.Source.ptTile;

    memset(ptSource, 0, sizeof(*ptSource));

    ptSource->tInfo = this.Origin.ptTile->tInfo;
    ptSource->bIsRoot = true;
    ptSource->pchBuffer = NULL;                 //!< special case

    arm_2d_region_t tOrigValidRegion;
    if (NULL == arm_2d_tile_get_root(this.Origin.ptTile, &tOrigValidRegion, NULL)) {
        return ARM_2D_ERR_OUT_OF_REGION;
    }

    //! angle validation
    ptTransform->fAngle = ARM_2D_FMODF(ptTransform->fAngle, ARM_2D_ANGLE(360));
    if (ptTransform->fScaleX <= 0.001f) {
        ptTransform->fScaleX = 1.0f;
    }
    if (ptTransform->fScaleY <= 0.001f) {
        ptTransform->fScaleY = ptTransform->fScaleX;
    }

    /* update source center (using root tile's coordinates) */
    do {
        arm_2d_location_t tResourceAsoluteLocation;
        arm_2d_tile_get_absolute_location(this.Origin.ptTile, &tResourceAsoluteLocation);
        ptTransform->tCenter.fX += tResourceAsoluteLocation.iX;
        ptTransform->tCenter.fY += tResourceAsoluteLocation.iY;
    } while(0);

    //! calculate the source region
    do {
        arm_2d_point_float_t tPoint;

        arm_2d_point_float_t tTopLeft = {.fX = (float)INT16_MAX, .fY = (float)INT16_MAX};
        arm_2d_point_float_t tBottomRight = {.fX = (float)INT16_MIN, .fY = (float)INT16_MIN};

        //! Top Left
        arm_2d_location_t tCornerPoint = tOrigValidRegion.tLocation;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScaleX,
                                ptTransform->fScaleY,
                                &tPoint);

        do {
            tTopLeft.fX = MIN(tTopLeft.fX, tPoint.fX);
            tTopLeft.fY = MIN(tTopLeft.fY, tPoint.fY);

            tBottomRight.fX = MAX(tBottomRight.fX, tPoint.fX);
            tBottomRight.fY = MAX(tBottomRight.fY, tPoint.fY);
        } while(0);

        //! Bottom Left
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScaleX,
                                ptTransform->fScaleY,
                                &tPoint);

        do {
            tTopLeft.fX = MIN(tTopLeft.fX, tPoint.fX);
            tTopLeft.fY = MIN(tTopLeft.fY, tPoint.fY);

            tBottomRight.fX = MAX(tBottomRight.fX, tPoint.fX);
            tBottomRight.fY = MAX(tBottomRight.fY, tPoint.fY);
        } while(0);

        //! Top Right
        tCornerPoint = tOrigValidRegion.tLocation;
        tCornerPoint.iX += tOrigValidRegion.tSize.iWidth - 1;

        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScaleX,
                                ptTransform->fScaleY,
                                &tPoint);

        do {
            tTopLeft.fX = MIN(tTopLeft.fX, tPoint.fX);
            tTopLeft.fY = MIN(tTopLeft.fY, tPoint.fY);

            tBottomRight.fX = MAX(tBottomRight.fX, tPoint.fX);
            tBottomRight.fY = MAX(tBottomRight.fY, tPoint.fY);
        } while(0);

        //! Bottom Right
        tCornerPoint.iY += tOrigValidRegion.tSize.iHeight - 1;
        __arm_2d_transform_point(  &tCornerPoint,
                                &ptTransform->tCenter,
                                ptTransform->fAngle,
                                ptTransform->fScaleX,
                                ptTransform->fScaleY,
                                &tPoint);

        do {
            tTopLeft.fX = MIN(tTopLeft.fX, tPoint.fX);
            tTopLeft.fY = MIN(tTopLeft.fY, tPoint.fY);

            tBottomRight.fX = MAX(tBottomRight.fX, tPoint.fX);
            tBottomRight.fY = MAX(tBottomRight.fY, tPoint.fY);
        } while(0);

        /* expand */
        tTopLeft.fX -= 1.0f;
        tTopLeft.fY -= 1.0f;

        tBottomRight.fX += 1.0f;
        tBottomRight.fY += 1.0f;

        //! calculate the region
        ptTransform->tDummySourceOffset = (arm_2d_location_t){
                                            (int16_t)(tTopLeft.fX + (tTopLeft.fX < 0 ? -0.5f : 0.5f)), 
                                            (int16_t)(tTopLeft.fY + (tTopLeft.fY < 0 ? -0.5f : 0.5f)),
                                        };

        ptSource->tRegion.tSize.iHeight = (int16_t)(tBottomRight.fY - tTopLeft.fY + 1.9f);
        ptSource->tRegion.tSize.iWidth = (int16_t)(tBottomRight.fX - tTopLeft.fX + 1.9f);

        ptTransform->fScaleX = 1.0f / ptTransform->fScaleX;
        ptTransform->fScaleY = 1.0f / ptTransform->fScaleY;

    } while(0);

    return ARM_2D_ERR_NONE;
}


static void __arm_2d_transform_preprocess_target(
                                        arm_2d_op_trans_t *ptThis,
                                        const arm_2d_point_float_t *ptTargetCentre)
{
    __arm_2d_transform_info_t *ptTransform
        = (__arm_2d_transform_info_t *)
            (   (uintptr_t)ptThis
            +   this.use_as__arm_2d_op_core_t.ptOp->Info.chInClassOffset);

    //! the following code is correct. DO NOT modify it unless you 100% sure.
    ptTransform->Target.tRegion.tSize = this.Source.ptTile->tRegion.tSize;

#if 0  //!< please keep this code for understanding the original meaning
    arm_2d_region_t tTargetRegion = {0};
    if (NULL != this.Target.ptRegion) {
        tTargetRegion = *this.Target.ptRegion;
    } else {
        tTargetRegion.tSize = this.Target.ptTile->tRegion.tSize;
    }
#else
    //! equivalent code
    assert(NULL == this.Target.ptRegion);

    arm_2d_region_t tTargetRegion = {
        .tSize = this.Target.ptTile->tRegion.tSize,
    };
#endif

    this.Target.ptRegion = &ptTransform->Target.tRegion;

    ptTransform->Target.tRegion.tLocation = tTargetRegion.tLocation;

    //! align with the specified center point
    do {
        arm_2d_point_float_t tOffset = {
            .fX = ptTransform->tCenter.fX - (float)ptTransform->tDummySourceOffset.iX,
            .fY = ptTransform->tCenter.fY - (float)ptTransform->tDummySourceOffset.iY,
        };

        if (NULL == ptTargetCentre) {
            arm_2d_point_float_t tTargetCenter = {
                .fX = (float)tTargetRegion.tSize.iWidth / 2.0f,
                .fY = (float)tTargetRegion.tSize.iHeight / 2.0f,
            };

            tOffset.fX = tTargetCenter.fX - tOffset.fX;
            tOffset.fY = tTargetCenter.fY - tOffset.fY;
        } else {
            tOffset.fX = ptTargetCentre->fX - tOffset.fX;
            tOffset.fY = ptTargetCentre->fY - tOffset.fY;
        }
        ptTransform->Target.tRegion.tLocation.iX += (tOffset.fX + (tOffset.fX < 0 ? -0.5f : 0.5f));
        ptTransform->Target.tRegion.tLocation.iY += (tOffset.fY + (tOffset.fY < 0 ? -0.5f : 0.5f));

    } while(0);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint_fast8_t chFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = chFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chFillColour)
{
    return arm_2dp_gray8_tile_transform_xy_with_colour_keying_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chFillColour
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint_fast16_t hwFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = hwFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast16_t hwFillColour)
{
    return arm_2dp_rgb565_tile_transform_xy_with_colour_keying_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        hwFillColour
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint32_t wFillColour)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.wColour = wFillColour;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_colour_keying_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint32_t wFillColour)
{
    return arm_2dp_cccn888_tile_transform_xy_with_colour_keying_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        wFillColour
    );
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t)
        -= ptTask->Param.tCopyOrig.tOrigin.nOffset;

    __arm_2d_impl_gray8_transform(  &(ptTask->Param.tCopyOrig),
                                    &this.tTransform);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 2;

    __arm_2d_impl_rgb565_transform( &(ptTask->Param.tCopyOrig),
                                    &this.tTransform);

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform_with_colour_keying(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 4;

    __arm_2d_impl_cccn888_transform(&(ptTask->Param.tCopyOrig),
                                    &this.tTransform);

    return arm_fsm_rt_cpl;
}



ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_gray8_tile_transform_xy_only_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_rgb565_tile_transform_xy_only_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;

    return __arm_2d_transform_preprocess_source(ptThis, &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_only_prepare(
                                            arm_2d_op_trans_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_cccn888_tile_transform_xy_only_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform_only(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        __arm_2d_impl_ccca8888_transform_to_gray8(  &(ptTask->Param.tCopyOrig),
                                                    &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_gray8_transform_only( &(ptTask->Param.tCopyOrig),
                                            &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform_only(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 2;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        __arm_2d_impl_ccca8888_transform_to_rgb565( &(ptTask->Param.tCopyOrig),
                                                    &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_rgb565_transform_only( &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform_only(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 4;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        __arm_2d_impl_ccca8888_transform_to_rgb565( &(ptTask->Param.tCopyOrig),
                                                    &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_cccn888_transform_only(&(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.chColour = chFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_gray8_tile_transform_xy_with_colour_keying_and_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chFillColour,
        chOpacity
    );
}


ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = hwFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_rgb565_tile_transform_xy_with_colour_keying_and_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        hwFillColour,
        chOpacity
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint32_t wFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.wColour = wFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_colour_keying_and_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint32_t wFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_cccn888_tile_transform_xy_with_colour_keying_and_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        wFillColour,
        chOpacity
    );
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

    if (255 == this.chOpacity) {
        __arm_2d_impl_gray8_transform(  &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
    #if __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 2
        __arm_2d_impl_gray8_2xssaa_transform_with_opacity(  &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    #else
        __arm_2d_impl_gray8_transform_with_opacity(  &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    #endif
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 2;

    if (255 == this.chOpacity) {
        __arm_2d_impl_rgb565_transform( &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
    #if __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 2
        __arm_2d_impl_rgb565_2xssaa_transform_with_opacity(   &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    #else
        __arm_2d_impl_rgb565_transform_with_opacity(   &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    #endif
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform_with_colour_keying_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 4;

    if (255 == this.chOpacity) {
        __arm_2d_impl_cccn888_transform(&(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
    #if __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 2
        __arm_2d_impl_cccn888_2xssaa_transform_with_opacity(   &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    #else
        __arm_2d_impl_cccn888_transform_with_opacity(  &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    #endif
    }
    return arm_fsm_rt_cpl;
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_tile_transform_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_gray8_tile_transform_xy_only_with_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_tile_transform_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_rgb565_tile_transform_xy_only_with_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptOP);

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_tile_transform_only_with_opacity_prepare(
                                        arm_2d_op_trans_opa_t *ptOP,
                                        const arm_2d_tile_t *ptSource,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_cccn888_tile_transform_xy_only_with_opacity_prepare(
        ptOP,
        ptSource,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

arm_fsm_rt_t __arm_2d_gray8_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        if (255 == this.chOpacity) {
            __arm_2d_impl_ccca8888_transform_to_gray8(  &(ptTask->Param.tCopyOrig),
                                                        &this.tTransform);

        } else {
            __arm_2d_impl_ccca8888_transform_with_opacity_to_gray8(  &(ptTask->Param.tCopyOrig),
                                                        &this.tTransform,
                                                        this.chOpacity);

        }
    } else
#endif
    if (255 == this.chOpacity) {
        __arm_2d_impl_gray8_transform_only(  &(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
        __arm_2d_impl_gray8_transform_only_opacity(  &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 2;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        if (255 == this.chOpacity) {
            __arm_2d_impl_ccca8888_transform_to_rgb565( &(ptTask->Param.tCopyOrig),
                                                        &this.tTransform);

        } else {
            __arm_2d_impl_ccca8888_transform_with_opacity_to_rgb565(  
                                                        &(ptTask->Param.tCopyOrig),
                                                        &this.tTransform,
                                                        this.chOpacity);

        }
    } else
#endif

    if (255 == this.chOpacity) {
        __arm_2d_impl_rgb565_transform_only(    &(ptTask->Param.tCopyOrig),
                                                &this.tTransform);
    } else {
        __arm_2d_impl_rgb565_transform_only_opacity(   
                                                &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_transform_only_with_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset * 4;

#if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
    if (ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme == ARM_2D_COLOUR_CCCA8888) {
        if (255 == this.chOpacity) {
            __arm_2d_impl_ccca8888_transform_to_cccn888(&(ptTask->Param.tCopyOrig),
                                                        &this.tTransform);

        } else {
            __arm_2d_impl_ccca8888_transform_with_opacity_to_cccn888(  
                                                        &(ptTask->Param.tCopyOrig),
                                                        &this.tTransform,
                                                        this.chOpacity);

        }
    } else
#endif

    if (255 == this.chOpacity) {
        __arm_2d_impl_cccn888_transform_only(&(ptTask->Param.tCopyOrig),
                                        &this.tTransform);
    } else {
        __arm_2d_impl_cccn888_transform_only_opacity(  &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}


ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_tile_transform_xy( arm_2d_op_trans_t *ptOP,
                                        const arm_2d_tile_t *ptTarget,
                                        const arm_2d_region_t *ptRegion,
                                        const arm_2d_point_float_t *ptTargetCentre)
{
    assert(NULL != ptTarget);

    ARM_2D_IMPL(arm_2d_op_trans_t, ptOP);
    arm_2d_point_float_t tTargetCentre;

    if (!__arm_2d_op_acquire((arm_2d_op_core_t *)ptThis)) {
        return arm_fsm_rt_on_going;
    }

    if (this.bInvalid) {
        __arm_2d_op_depose( (arm_2d_op_core_t *)ptThis, 
                            (arm_fsm_rt_t)ARM_2D_ERR_INVALID_STATUS);
        return (arm_fsm_rt_t)ARM_2D_ERR_INVALID_STATUS;
    }

    arm_2d_region_t tTargetRegion = {
        .tSize = ptTarget->tRegion.tSize
    };
    if (NULL == ptRegion) {
        ptRegion = &tTargetRegion;
    }

    __arm_2d_transform_info_t *ptTransform
        = (__arm_2d_transform_info_t *)
            (   (uintptr_t)ptThis
            +   this.use_as__arm_2d_op_core_t.ptOp->Info.chInClassOffset);


    this.Target.ptTile = arm_2d_tile_generate_child(
                                                ptTarget,
                                                ptRegion,
                                                &ptTransform->Target.tTile,
                                                false);
    if (NULL == this.Target.ptTile) {
        arm_fsm_rt_t tResult = (arm_fsm_rt_t)ARM_2D_ERR_OUT_OF_REGION;
        if (ARM_2D_RUNTIME_FEATURE.TREAT_OUT_OF_RANGE_AS_COMPLETE) {
            tResult = arm_fsm_rt_cpl;
        }

        return __arm_2d_op_depose((arm_2d_op_core_t *)ptThis, tResult);
    }

    if (NULL != ptTargetCentre) {
        tTargetCentre.fX = ptTargetCentre->fX - ptRegion->tLocation.iX;
        tTargetCentre.fY = ptTargetCentre->fY - ptRegion->tLocation.iY;

        ptTargetCentre = &tTargetCentre;
    }

    this.Target.ptRegion = NULL;

    __arm_2d_transform_preprocess_target(   ptThis, ptTargetCentre);
    return __arm_2d_op_invoke((arm_2d_op_core_t *)ptThis);
}

ARM_NONNULL(2)
arm_fsm_rt_t arm_2dp_tile_transform(arm_2d_op_trans_t *ptOP,
                                 const arm_2d_tile_t *ptTarget,
                                 const arm_2d_region_t *ptRegion,
                                 const arm_2d_location_t *ptTargetCentre)
{
    if (NULL != ptTargetCentre) {
        arm_2d_point_float_t tTargetCentre = {
            .fX = ptTargetCentre->iX,
            .fY = ptTargetCentre->iY,
        };

        return arm_2dp_tile_transform_xy(ptOP, ptTarget, ptRegion, &tTargetCentre);

    }

    return arm_2dp_tile_transform_xy(ptOP, ptTarget, ptRegion, NULL);
}



ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = chFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_gray8_tile_transform_xy_with_src_mask_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = hwFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_rgb565_tile_transform_xy_with_src_mask_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}


ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = wFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_prepare(
                                            arm_2d_op_trans_msk_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale)
{
    return arm_2dp_cccn888_tile_transform_xy_with_src_mask_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale
    );
}

arm_fsm_rt_t
__arm_2d_gray8_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_gray8_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_gray8_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_rgb565_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset * 2;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_rgb565_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_rgb565_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_cccn888_sw_transform_with_src_mask(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset * 4;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_cccn888_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    } else
#endif
    {
        __arm_2d_impl_cccn888_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
    }

    return arm_fsm_rt_cpl;
}



ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_xy_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = chFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_gray8_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    return arm_2dp_gray8_tile_transform_xy_with_src_mask_and_opacity_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_xy_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = hwFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_rgb565_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    return arm_2dp_rgb565_tile_transform_xy_with_src_mask_and_opacity_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_xy_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_point_float_t tCentre,
                                            float fAngle,
                                            float fScaleX,
                                            float fScaleY,
                                            uint_fast8_t chOpacity)
{
    assert(NULL != ptSource);

    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptOP);

    arm_2d_err_t tErr = __arm_mask_validate(ptSource,
                                            ptSourceMask,
                                            NULL,
                                            NULL,
                                            0);
    if (tErr < 0) {
        this.bInvalid = true;
        return tErr;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptSource;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    //this.tTransform.Mask.hwColour = wFillColour;
    this.Mask.ptOriginSide = ptSourceMask;
    this.Mask.ptTargetSide = NULL;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2,3)
arm_2d_err_t arm_2dp_cccn888_tile_transform_with_src_mask_and_opacity_prepare(
                                            arm_2d_op_trans_msk_opa_t *ptOP,
                                            const arm_2d_tile_t *ptSource,
                                            const arm_2d_tile_t *ptSourceMask,
                                            const arm_2d_location_t tCentre,
                                            float fAngle,
                                            float fScale,
                                            uint_fast8_t chOpacity)
{
    return arm_2dp_cccn888_tile_transform_xy_with_src_mask_and_opacity_prepare(
        ptOP,
        ptSource,
        ptSourceMask,
        (arm_2d_point_float_t){.fX = tCentre.iX, .fY = tCentre.iY,},
        fAngle,
        fScale,
        fScale,
        chOpacity
    );
}

arm_fsm_rt_t
__arm_2d_gray8_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_8BIT == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;
        
#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_gray8_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_gray8_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_gray8_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_gray8_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }


    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_rgb565_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset * 2;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_rgb565_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_rgb565_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_rgb565_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_rgb565_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t
__arm_2d_cccn888_sw_transform_with_src_mask_and_opacity(__arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_trans_msk_opa_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(
        ptTask->Param.tCopyOrigMask
            .use_as____arm_2d_param_copy_orig_t.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrigMask.use_as____arm_2d_param_copy_orig_t.tOrigin.nOffset * 4;

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrigMask.tOrigMask.pBuffer, intptr_t) -=
        ptTask->Param.tCopyOrigMask.tOrigMask.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrigMask.tOrigMask.tColour.chScheme);
#endif

    if (255 == this.chOpacity) {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_cccn888_transform_with_src_chn_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        } else
    #endif
        {
            __arm_2d_impl_cccn888_transform_with_src_mask(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform);
        }
    } else {
    #if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
        if (bIsMaskChannel8In32) {
            __arm_2d_impl_cccn888_transform_with_src_chn_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        } else
    #endif
        {
            __arm_2d_impl_cccn888_transform_with_src_mask_and_opacity(
                                                &(ptTask->Param.tCopyOrigMask),
                                                &this.tTransform,
                                                this.chOpacity);
        }
    }
    return arm_fsm_rt_cpl;
}


ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_fill_colour_with_mask_opacity_and_transform_xy_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptOP);
    
    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask,    __ARM_2D_MASK_ALLOW_A8 
#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                                    |   __ARM_2D_MASK_ALLOW_8in32
#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_GRAY8;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptMask;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.chColour = chFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_gray8_fill_colour_with_mask_opacity_and_transform_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast8_t chFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_gray8_fill_colour_with_mask_opacity_and_transform_xy_prepare(
        ptOP, 
        ptMask, 
        (arm_2d_point_float_t){ .fX = tCentre.iX, .fY = tCentre.iY },
        fAngle,
        fScale,
        fScale,
        chFillColour,
        chOpacity
    );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_fill_colour_with_mask_opacity_and_transform_xy_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask,    __ARM_2D_MASK_ALLOW_A8 
#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                                    |   __ARM_2D_MASK_ALLOW_8in32
#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_RGB565;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptMask;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.hwColour = hwFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_rgb565_fill_colour_with_mask_opacity_and_transform_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint_fast16_t hwFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_rgb565_fill_colour_with_mask_opacity_and_transform_xy_prepare(
                    ptOP, 
                    ptMask, 
                    (arm_2d_point_float_t){ .fX = tCentre.iX, .fY = tCentre.iY },
                    fAngle,
                    fScale,
                    fScale,
                    hwFillColour,
                    chOpacity
                );
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform_xy_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_point_float_t tCentre,
                                        float fAngle,
                                        float fScaleX,
                                        float fScaleY,
                                        uint32_t wFillColour,
                                        uint_fast8_t chOpacity)
{
    assert(NULL != ptMask);

    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptOP);

    //! valid alpha mask tile
    if (!__arm_2d_valid_mask(ptMask,    __ARM_2D_MASK_ALLOW_A8 
#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
                                    |   __ARM_2D_MASK_ALLOW_8in32
#endif
        )) {
        this.bInvalid = true;
        return ARM_2D_ERR_INVALID_PARAM;
    }

    if (!arm_2d_op_wait_async((arm_2d_op_core_t *)ptThis)) {
        return ARM_2D_ERR_BUSY;
    }

    OP_CORE.ptOp = &ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_CCCN888;

    this.Source.ptTile = &this.Origin.tDummySource;
    this.Origin.ptTile = ptMask;
    this.wMode = 0;
    this.tTransform.fAngle = fAngle;
    this.tTransform.fScaleX = fScaleX;
    this.tTransform.fScaleY = fScaleY;
    this.tTransform.tCenter = tCentre;
    this.tTransform.Mask.wColour = wFillColour;
    this.chOpacity = chOpacity;

    return __arm_2d_transform_preprocess_source((arm_2d_op_trans_t *)ptThis,
                                                &this.tTransform);
}

ARM_NONNULL(2)
arm_2d_err_t arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform_prepare(
                                        arm_2d_op_fill_cl_msk_opa_trans_t *ptOP,
                                        const arm_2d_tile_t *ptMask,
                                        const arm_2d_location_t tCentre,
                                        float fAngle,
                                        float fScale,
                                        uint32_t wFillColour,
                                        uint_fast8_t chOpacity)
{
    return arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform_xy_prepare(
                    ptOP, 
                    ptMask, 
                    (arm_2d_point_float_t){ .fX = tCentre.iX, .fY = tCentre.iY },
                    fAngle,
                    fScale,
                    fScale,
                    wFillColour,
                    chOpacity
                );
}

arm_fsm_rt_t __arm_2d_gray8_sw_colour_filling_with_mask_opacity_and_transform(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_GRAY8 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_gray8_colour_filling_chn_mask_opacity_transform(  
                                            &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    } else 
#endif
    {
        __arm_2d_impl_gray8_colour_filling_mask_opacity_transform(  
                                            &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    }

    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_rgb565_sw_colour_filling_with_mask_opacity_and_transform(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_RGB565 == OP_CORE.ptOp->Info.Colour.chScheme);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_rgb565_colour_filling_chn_mask_opacity_transform(  
                                            &(ptTask->Param.tCopyOrig),
                                            &this.tTransform,
                                            this.chOpacity);
    } else 
#endif
    {
        __arm_2d_impl_rgb565_colour_filling_mask_opacity_transform(   
                                                &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}

arm_fsm_rt_t __arm_2d_cccn888_sw_colour_filling_with_mask_opacity_and_transform(
                                                    __arm_2d_sub_task_t *ptTask)
{
    ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, ptTask->ptOP);
    assert(ARM_2D_COLOUR_SZ_32BIT == OP_CORE.ptOp->Info.Colour.u3ColourSZ);

    ARM_TYPE_CONVERT(ptTask->Param.tCopyOrig.tOrigin.pBuffer, intptr_t) -= 
          ptTask->Param.tCopyOrig.tOrigin.nOffset;

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
    bool bIsMaskChannel8In32 = (ARM_2D_CHANNEL_8in32
            ==  ptTask->Param.tCopyOrig.tOrigin.tColour.chScheme);

    if (bIsMaskChannel8In32) {
        __arm_2d_impl_cccn888_colour_filling_chn_mask_opacity_transform(  
                                                &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    } else 
#endif
    {
        __arm_2d_impl_cccn888_colour_filling_mask_opacity_transform(  
                                                &(ptTask->Param.tCopyOrig),
                                                &this.tTransform,
                                                this.chOpacity);
    }
    return arm_fsm_rt_cpl;
}


/*----------------------------------------------------------------------------*
 * Accelerable Low Level APIs                                                 *
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * Low Level IO Interfaces                                                    *
 *----------------------------------------------------------------------------*/

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_GRAY8,
                __arm_2d_gray8_sw_transform_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_RGB565,
                __arm_2d_rgb565_sw_transform_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_CCCN888,
                __arm_2d_cccn888_sw_transform_with_colour_keying);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_GRAY8,
                __arm_2d_gray8_sw_transform_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_RGB565,
                __arm_2d_rgb565_sw_transform_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_CCCN888,
                __arm_2d_cccn888_sw_transform_only);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8,
                __arm_2d_gray8_sw_transform_with_colour_keying_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_RGB565,
                __arm_2d_rgb565_sw_transform_with_colour_keying_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_transform_with_colour_keying_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_GRAY8,
                __arm_2d_gray8_sw_transform_only_with_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_RGB565,
                __arm_2d_rgb565_sw_transform_only_with_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_transform_only_with_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_GRAY8,
                __arm_2d_gray8_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_RGB565,
                __arm_2d_rgb565_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_CCCN888,
                __arm_2d_cccn888_sw_transform_with_src_mask);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8,
                __arm_2d_gray8_sw_transform_with_src_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565,
                __arm_2d_rgb565_sw_transform_with_src_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888,
                __arm_2d_cccn888_sw_transform_with_src_mask_and_opacity);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_GRAY8,
                __arm_2d_gray8_sw_colour_filling_with_mask_opacity_and_transform);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_RGB565,
                __arm_2d_rgb565_sw_colour_filling_with_mask_opacity_and_transform);

__WEAK
def_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_CCCN888,
                __arm_2d_cccn888_sw_colour_filling_with_mask_opacity_and_transform);


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_COLOUR_KEYING_AND_OPACITY_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_ONLY_WITH_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
        #if __ARM_2D_CFG_SUPPORT_CCCA8888_IMPLICIT_CONVERSION__
            .bAllowEnforcedColour   = true,
        #endif
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_ONLY_WITH_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_ONLY_WITH_OPACITY_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_8BIT,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};

const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bHasSrcMask            = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_TRANSFORM_WITH_SOURCE_MASK_AND_OPACITY,
        .chInClassOffset    = offsetof(arm_2d_op_trans_msk_opa_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_TRANSFORM_WITH_SRC_MSK_AND_OPACITY_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};



const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_GRAY8 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_GRAY8,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_OPACITY_AND_TRANFORM,
        .chInClassOffset    = offsetof(arm_2d_op_fill_cl_msk_opa_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_GRAY8),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_RGB565 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_RGB565,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_OPACITY_AND_TRANFORM,
        .chInClassOffset    = offsetof(arm_2d_op_fill_cl_msk_opa_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_RGB565),
            .ptFillOrigLike = NULL,
        },
    },
};


const __arm_2d_op_info_t ARM_2D_OP_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_CCCN888 = {
    .Info = {
        .Colour = {
            .chScheme   = ARM_2D_COLOUR_CCCN888,
        },
        .Param = {
            .bHasSource             = true,
            .bHasOrigin             = true,
            .bHasTarget             = true,
            .bAllowEnforcedColour   = true,
        },
        .chOpIndex          = __ARM_2D_OP_IDX_FILL_COLOUR_WITH_MASK_OPACITY_AND_TRANFORM,
        .chInClassOffset    = offsetof(arm_2d_op_fill_cl_msk_opa_trans_t, tTransform),

        .LowLevelIO = {
            .ptCopyOrigLike = ref_low_lv_io(__ARM_2D_IO_FILL_COLOUR_WITH_MSK_OPACITY_AND_TRANSFORM_CCCN888),
            .ptFillOrigLike = NULL,
        },
    },
};

/*============================ INCLUDES ======================================*/
#define __ARM_2D_COMPILATION_UNIT
#include "__arm_2d_tile_2xssaa_transform.c"


#ifdef   __cplusplus
}
#endif
