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
 * Description:  The source file of Triangle-Anti-Alias Tile Transform
 *               
 * $Date:        31. July 2024
 * $Revision:    V.0.1.0
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



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/




__WEAK
void __arm_2d_impl_gray8_taa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            uint8_t *pchTarget,
                                            //int16_t iTargetSride,
                                            uint8_t chMaskColour,
                                            uint_fast16_t hwOpacity)
{
#if 0
    arm_2d_location_t tOriginLocation = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_q16(ptFxPoint);

    __API_PIXEL_AVERAGE_INIT();

    uint8_t chTempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint16_t hwAlpha = tAdjacentArray.tMatrix[n].chAlpha;

        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        chTempPixel = (*pchTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            uint8_t chTemp = pchOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (chTemp != chMaskColour) {
                bIsInside = true;
                chTempPixel = chTemp;
            }
        }

        __API_PIXEL_AVERAGE(chTempPixel, hwAlpha);
    }

    if (bIsInside) {
        uint8_t chSourcPixel = __API_PIXEL_AVERAGE_RESULT();

        __API_PIXEL_BLENDING( &chSourcPixel, pchTarget, hwOpacity);
    }

#endif

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        uint8_t chTemp = pchOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (chTemp != chMaskColour) {
            __ARM_2D_PIXEL_BLENDING_GRAY8( &chTemp, pchTarget, hwOpacity);
        }
    }

}

__WEAK
void __arm_2d_impl_gray8_taa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
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

            if (    (tOriginLocation.iX < -1)
                ||  (tOriginLocation.iY < -1)
                ||  (tOriginLocation.iX > ptParam->tOrigin.tValidRegion.tSize.iWidth)
                ||  (tOriginLocation.iY > ptParam->tOrigin.tValidRegion.tSize.iHeight)) {
                pchTargetBase++;
                continue;
            }

            __arm_2d_impl_gray8_taa_get_pixel_colour_with_alpha (
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




__WEAK
void __arm_2d_impl_rgb565_taa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint16_t *phwOrigin,
                                            int16_t iOrigStride,
                                            uint16_t *phwTarget,
                                            //int16_t iTargetSride,
                                            uint16_t hwMaskColour,
                                            uint_fast16_t hwOpacity)
{
#if 0
    arm_2d_location_t tOriginLocation = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_q16(ptFxPoint);

    __API_PIXEL_AVERAGE_INIT();

    uint16_t hwTempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint16_t hwAlpha = tAdjacentArray.tMatrix[n].chAlpha;

        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        hwTempPixel = (*phwTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            uint16_t hwTemp = phwOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (hwTemp != hwMaskColour) {
                bIsInside = true;
                hwTempPixel = hwTemp;
            }
        }

        __API_PIXEL_AVERAGE(hwTempPixel, hwAlpha);
    }

    if (bIsInside) {
        uint16_t hwSourcPixel = __API_PIXEL_AVERAGE_RESULT();

        __API_PIXEL_BLENDING( &hwSourcPixel, phwTarget, hwOpacity);
    }

#endif

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        uint16_t hwTemp = phwOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (hwTemp != hwMaskColour) {
            __ARM_2D_PIXEL_BLENDING_RGB565( &hwTemp, phwTarget, hwOpacity);
        }
    }

}

__WEAK
void __arm_2d_impl_rgb565_taa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
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

            if (    (tOriginLocation.iX < -1)
                ||  (tOriginLocation.iY < -1)
                ||  (tOriginLocation.iX > ptParam->tOrigin.tValidRegion.tSize.iWidth)
                ||  (tOriginLocation.iY > ptParam->tOrigin.tValidRegion.tSize.iHeight)) {
                phwTargetBase++;
                continue;
            }

            __arm_2d_impl_rgb565_taa_get_pixel_colour_with_alpha (
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




__WEAK
void __arm_2d_impl_cccn888_taa_get_pixel_colour_with_alpha(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pwOrigin,
                                            int16_t iOrigStride,
                                            uint32_t *pwTarget,
                                            //int16_t iTargetSride,
                                            uint32_t wMaskColour,
                                            uint_fast16_t hwOpacity)
{
#if 0
    arm_2d_location_t tOriginLocation = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __arm_2d_point_adj_alpha_t tAdjacentArray
        = __arm_2d_point_get_adjacent_alpha_q16(ptFxPoint);

    __API_PIXEL_AVERAGE_INIT();

    uint32_t wTempPixel;
    bool bIsInside = false;

    for (int_fast8_t n = 0; n < 4; n++) {
        uint16_t hwAlpha = tAdjacentArray.tMatrix[n].chAlpha;

        arm_2d_location_t tTemp = {
            .iX = tOriginLocation.iX + tAdjacentArray.tMatrix[n].tOffset.iX,
            .iY = tOriginLocation.iY + tAdjacentArray.tMatrix[n].tOffset.iY,
        };
        wTempPixel = (*pwTarget);

        if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {

            uint32_t wTemp = pwOrigin[   tTemp.iY * iOrigStride
                                            +   tTemp.iX];

            if (wTemp != wMaskColour) {
                bIsInside = true;
                wTempPixel = wTemp;
            }
        }

        __API_PIXEL_AVERAGE(wTempPixel, hwAlpha);
    }

    if (bIsInside) {
        uint32_t wSourcPixel = __API_PIXEL_AVERAGE_RESULT();

        __API_PIXEL_BLENDING( &wSourcPixel, pwTarget, hwOpacity);
    }

#endif

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        uint32_t wTemp = pwOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (wTemp != wMaskColour) {
            __ARM_2D_PIXEL_BLENDING_CCCN888( &wTemp, pwTarget, hwOpacity);
        }
    }

}

__WEAK
void __arm_2d_impl_cccn888_taa_transform_with_opacity(__arm_2d_param_copy_orig_t *ptParam,
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

            if (    (tOriginLocation.iX < -1)
                ||  (tOriginLocation.iY < -1)
                ||  (tOriginLocation.iX > ptParam->tOrigin.tValidRegion.tSize.iWidth)
                ||  (tOriginLocation.iY > ptParam->tOrigin.tValidRegion.tSize.iHeight)) {
                pwTargetBase++;
                continue;
            }

            __arm_2d_impl_cccn888_taa_get_pixel_colour_with_alpha (
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
