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
 * Title:        __arm_2d_tile_taa_transform.c
 * Description:  The source file of 2x SuperSampling Anti Alias Tile 
 *               Transform
 * 
 * $Date:        01. Aug 2024
 * $Revision:    V.0.3.0
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
#undef OP_CORE
#define OP_CORE this.use_as__arm_2d_op_t.use_as__arm_2d_op_core_t

#define OPCODE this.use_as__arm_2d_op_t

#define __ARM_2D_PIXEL_AVERAGE_INIT_GRAY8       uint16_t tPixel = 0;
#define __ARM_2D_PIXEL_AVERAGE_INIT_RGB565      __arm_2d_color_fast_rgb_t tPixel = {0};
#define __ARM_2D_PIXEL_AVERAGE_INIT_CCCN888     __arm_2d_color_fast_rgb_t tPixel = {0}; uint32_t *pTarget = pwTarget;

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/




__STATIC_INLINE
void __arm_2d_impl_gray8_2xssaa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            //int16_t iTargetSride,
                                            uint8_t chMaskColour,
                                            uint_fast16_t hwOpacity)
{
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    /*
     * [P0][P1]
     * [P2][P3]
     */

    uint8_t *pchSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;

    uint8_t chPoint0 = *pchSample++;
    uint8_t chPoint1 = *pchSample;

    __ARM_2D_PIXEL_BLENDING_GRAY8( &chPoint0, &chPoint1, hwAlphaX);

    if (chPoint1 != chMaskColour) {
        __ARM_2D_PIXEL_BLENDING_GRAY8( &chPoint1, pchTarget, hwOpacity);
    }


#if 0 /* reference code */
    uint8_t *pchSample = &pchOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    uint8_t chPoint0 = *pchSample++;
    uint8_t chPoint1 = *pchSample;
    pchSample += iOrigStride;
    uint8_t chPoint3 = *pchSample--;
    uint8_t chPoint2 = *pchSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __ARM_2D_PIXEL_AVERAGE_INIT_GRAY8

    __ARM_2D_PIXEL_AVERAGE_GRAY8(chPoint0,  hwAlpha0);
    __ARM_2D_PIXEL_AVERAGE_GRAY8(chPoint1,  hwAlpha1);
    __ARM_2D_PIXEL_AVERAGE_GRAY8(chPoint2,  hwAlpha2);
    __ARM_2D_PIXEL_AVERAGE_GRAY8(chPoint3,  hwAlpha3);

    uint8_t chPixelResult = __API_PIXEL_AVERAGE_RESULT_GRAY8();
    if (chPixelResult != chMaskColour) {
        __ARM_2D_PIXEL_BLENDING_GRAY8( &chPixelResult, pchTarget, hwOpacity);
    }
#endif

}

__WEAK
void __arm_2d_impl_gray8_2xssaa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint8_t     *pchTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint8_t     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    uint8_t      chMaskColour = ptInfo->Mask.chColour;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);
    hwRatio = 256 - hwRatio;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_fx_t tPointFast;

            tPointFast.X = __QDADD(colFirstX, slopeX * x);
            tPointFast.Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.X > 0) {
                tPointFast.X += __CALIBFX;
            } else {
                tPointFast.X -= __CALIBFX;
            }
            if (tPointFast.Y > 0) {
                tPointFast.Y += __CALIBFX;
            } else {
                tPointFast.Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = (tPointFast.X >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iX,
                .iY = (tPointFast.Y >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptParam->tOrigin.tValidRegion.tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptParam->tOrigin.tValidRegion.tSize.iHeight - 1))) {
                pchTargetBase++;
                continue;
            }

            __arm_2d_impl_gray8_2xssaa_get_pixel_colour_with_alpha (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            pchTargetBase,
                            chMaskColour,
                            hwRatio);
            pchTargetBase++;
        }
        pchTargetBase += (iTargetStride - iWidth);
    }
}




__STATIC_INLINE
void __arm_2d_impl_rgb565_2xssaa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint16_t *phwOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            //int16_t iTargetSride,
                                            uint16_t hwMaskColour,
                                            uint_fast16_t hwOpacity)
{
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    /*
     * [P0][P1]
     * [P2][P3]
     */

    uint16_t *phwSample = &phwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;

    uint16_t hwPoint0 = *phwSample++;
    uint16_t hwPoint1 = *phwSample;

    __ARM_2D_PIXEL_BLENDING_RGB565( &hwPoint0, &hwPoint1, hwAlphaX);

    if (hwPoint1 != hwMaskColour) {
        __ARM_2D_PIXEL_BLENDING_RGB565( &hwPoint1, phwTarget, hwOpacity);
    }


#if 0 /* reference code */
    uint16_t *phwSample = &phwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    uint16_t hwPoint0 = *phwSample++;
    uint16_t hwPoint1 = *phwSample;
    phwSample += iOrigStride;
    uint16_t hwPoint3 = *phwSample--;
    uint16_t hwPoint2 = *phwSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __ARM_2D_PIXEL_AVERAGE_INIT_RGB565

    __ARM_2D_PIXEL_AVERAGE_RGB565(hwPoint0,  hwAlpha0);
    __ARM_2D_PIXEL_AVERAGE_RGB565(hwPoint1,  hwAlpha1);
    __ARM_2D_PIXEL_AVERAGE_RGB565(hwPoint2,  hwAlpha2);
    __ARM_2D_PIXEL_AVERAGE_RGB565(hwPoint3,  hwAlpha3);

    uint16_t hwPixelResult = __API_PIXEL_AVERAGE_RESULT_RGB565();
    if (hwPixelResult != hwMaskColour) {
        __ARM_2D_PIXEL_BLENDING_RGB565( &hwPixelResult, phwTarget, hwOpacity);
    }
#endif

}

__WEAK
void __arm_2d_impl_rgb565_2xssaa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint16_t     *phwTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint16_t     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    uint16_t      hwMaskColour = ptInfo->Mask.hwColour;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);
    hwRatio = 256 - hwRatio;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_fx_t tPointFast;

            tPointFast.X = __QDADD(colFirstX, slopeX * x);
            tPointFast.Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.X > 0) {
                tPointFast.X += __CALIBFX;
            } else {
                tPointFast.X -= __CALIBFX;
            }
            if (tPointFast.Y > 0) {
                tPointFast.Y += __CALIBFX;
            } else {
                tPointFast.Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = (tPointFast.X >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iX,
                .iY = (tPointFast.Y >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptParam->tOrigin.tValidRegion.tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptParam->tOrigin.tValidRegion.tSize.iHeight - 1))) {
                phwTargetBase++;
                continue;
            }

            __arm_2d_impl_rgb565_2xssaa_get_pixel_colour_with_alpha (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            phwTargetBase,
                            hwMaskColour,
                            hwRatio);
            phwTargetBase++;
        }
        phwTargetBase += (iTargetStride - iWidth);
    }
}




__STATIC_INLINE
void __arm_2d_impl_cccn888_2xssaa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pwOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            //int16_t iTargetSride,
                                            uint32_t wMaskColour,
                                            uint_fast16_t hwOpacity)
{
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    /*
     * [P0][P1]
     * [P2][P3]
     */

    uint32_t *pwSample = &pwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;

    uint32_t wPoint0 = *pwSample++;
    uint32_t wPoint1 = *pwSample;

    __ARM_2D_PIXEL_BLENDING_CCCN888( &wPoint0, &wPoint1, hwAlphaX);

    if (wPoint1 != wMaskColour) {
        __ARM_2D_PIXEL_BLENDING_CCCN888( &wPoint1, pwTarget, hwOpacity);
    }


#if 0 /* reference code */
    uint32_t *pwSample = &pwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    uint32_t wPoint0 = *pwSample++;
    uint32_t wPoint1 = *pwSample;
    pwSample += iOrigStride;
    uint32_t wPoint3 = *pwSample--;
    uint32_t wPoint2 = *pwSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __ARM_2D_PIXEL_AVERAGE_INIT_CCCN888

    __ARM_2D_PIXEL_AVERAGE_CCCN888(wPoint0,  hwAlpha0);
    __ARM_2D_PIXEL_AVERAGE_CCCN888(wPoint1,  hwAlpha1);
    __ARM_2D_PIXEL_AVERAGE_CCCN888(wPoint2,  hwAlpha2);
    __ARM_2D_PIXEL_AVERAGE_CCCN888(wPoint3,  hwAlpha3);

    uint32_t wPixelResult = __API_PIXEL_AVERAGE_RESULT_CCCN888();
    if (wPixelResult != wMaskColour) {
        __ARM_2D_PIXEL_BLENDING_CCCN888( &wPixelResult, pwTarget, hwOpacity);
    }
#endif

}

__WEAK
void __arm_2d_impl_cccn888_2xssaa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    uint32_t     *pwTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint32_t     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    uint32_t      wMaskColour = ptInfo->Mask.wColour;
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);
    hwRatio = 256 - hwRatio;

    q31_t               invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScale,
        &tOffset,
        &(ptInfo->tCenter),
        regrCoefs);

    /* slopes between 1st and last cols */
    int32_t         slopeY, slopeX;

    slopeY =
        MULTFX((regrCoefs[1].interceptY - regrCoefs[0].interceptY), invIWidth);
    slopeX =
        MULTFX((regrCoefs[1].interceptX - regrCoefs[0].interceptX), invIWidth);

    for (int_fast16_t y = 0; y < iHeight; y++) {
        /* 1st column estimates */
        int32_t         colFirstY =
            __QADD((regrCoefs[0].slopeY * y), regrCoefs[0].interceptY);
        int32_t         colFirstX =
            __QADD((regrCoefs[0].slopeX * y), regrCoefs[0].interceptX);


        for (int_fast16_t x = 0; x < iWidth; x++) {
            arm_2d_point_fx_t tPointFast;

            tPointFast.X = __QDADD(colFirstX, slopeX * x);
            tPointFast.Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.X > 0) {
                tPointFast.X += __CALIBFX;
            } else {
                tPointFast.X -= __CALIBFX;
            }
            if (tPointFast.Y > 0) {
                tPointFast.Y += __CALIBFX;
            } else {
                tPointFast.Y -= __CALIBFX;
            }
        #endif

            arm_2d_location_t tOriginLocation = {
                .iX = (tPointFast.X >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iX,
                .iY = (tPointFast.Y >> 16) - ptParam->tOrigin.tValidRegion.tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptParam->tOrigin.tValidRegion.tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptParam->tOrigin.tValidRegion.tSize.iHeight - 1))) {
                pwTargetBase++;
                continue;
            }

            __arm_2d_impl_cccn888_2xssaa_get_pixel_colour_with_alpha (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            pwTargetBase,
                            wMaskColour,
                            hwRatio);
            pwTargetBase++;
        }
        pwTargetBase += (iTargetStride - iWidth);
    }
}


#ifdef   __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */
