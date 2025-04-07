/*
 * Copyright (C) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_meta_ccca8888_trans.inc
 * Description:  c code template for ccca8888 tile transform operations
 *
 * $Date:        7. April 2025
 * $Revision:    V.1.0.0
 *
 * -------------------------------------------------------------------- */

#ifndef __API_CCCA8888_TRANS_COLOUR
#   error You have to define __API_CCCA8888_TRANS_COLOUR before using this c template
#endif
#ifndef __API_CCCA8888_TRANS_COLOUR_UPPER_CASE
#   error You have to define __API_CCCA8888_TRANS_COLOUR_UPPER_CASE before using this c template
#endif
#ifndef __API_CCCA8888_TRANS_INT_TYPE
#   error You have to define the __API_CCCA8888_TRANS_INT_TYPE before using this c template
#endif
#ifndef __API_CCCA8888_TRANS_INT_TYPE_BIT_NUM
#   error You have to define the __API_CCCA8888_TRANS_INT_TYPE_BIT_NUM before using this c template
#endif

#ifndef __API_CCCA8888_TRANS_PIXEL_AVERAGE
#   error You have to define __API_CCCA8888_TRANS_PIXEL_AVERAGE before using this c template
#endif
#ifndef __API_CCCA8888_TRANS_PIXEL_AVERAGE_RESULT
#   error You have to define __API_CCCA8888_TRANS_PIXEL_AVERAGE_RESULT before using this c template
#endif
#ifndef __API_CCCA8888_TRANS_PIXEL_AVERAGE_INIT
#   define __API_CCCA8888_TRANS_PIXEL_AVERAGE_INIT()   __arm_2d_color_fast_rgb_t tPixel = {0};
#endif

#ifndef __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
#   define __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY                       0
#endif

#define ______API_CCCA8888_TRANS_PIXEL_BLENDING(__COLOUR)   __ARM_2D_PIXEL_BLENDING_CCCA8888_TO_##__COLOUR
#define ____API_CCCA8888_TRANS_PIXEL_BLENDING(__COLOUR)     ______API_CCCA8888_TRANS_PIXEL_BLENDING(__COLOUR)
#define __API_CCCA8888_TRANS_PIXEL_BLENDING                 ____API_CCCA8888_TRANS_PIXEL_BLENDING(__API_CCCA8888_TRANS_COLOUR_UPPER_CASE)

#undef ____CCCA8888_TRANS_FUNC
#undef ___CCCA8888_TRANS_FUNC
#undef __CCCA8888_TRANS_FUNC

#   define ____CCCA8888_TRANS_FUNC(__NAME, __COLOUR)                                      \
        __arm_2d_impl_ccca8888_##__NAME##_to_##__COLOUR
#   define ___CCCA8888_TRANS_FUNC(__NAME, __COLOUR)   ____CCCA8888_TRANS_FUNC(__NAME, __COLOUR)

#define __CCCA8888_TRANS_FUNC(__NAME)   ___CCCA8888_TRANS_FUNC(__NAME, __API_CCCA8888_TRANS_COLOUR)


#define __CCCA8888_TRANS_TYPE(__NAME)   ___CCCA8888_TRANS_TYPE(__NAME, __API_CCCA8888_TRANS_COLOUR)

#define MASK_COLOR(sz)  (sz == 8) ? ptInfo->Mask.chColour : ((sz == 16) ?  ptInfo->Mask.hwColour :  ptInfo->Mask.wColour)

/*============================ PROTOTYPES ====================================*/
extern
void __CCCA8888_TRANS_FUNC(transform)(
                                        __arm_2d_param_copy_orig_t *ptParam,
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        );

/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE
void __CCCA8888_TRANS_FUNC(get_pixel_colour)(arm_2d_point_fx_t  *ptFxPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        uint32_t *pwOrigin,
                                        int16_t iOrigStride,
                                    #if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
                                        uint16_t hwOpacity,
                                    #endif
                                        __API_CCCA8888_TRANS_INT_TYPE *pTarget
                                    )
{
    uint32_t *pwOrigMask = (uint32_t *)(((uintptr_t)pwOrigin) + 3);
    int_fast16_t iOrigMaskStride = iOrigStride;
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    /*
     * [0][1]
     * [2][3]
     */

    /* anti-alias alpha table */
    uint16_t hwAlphaX = (ptFxPoint->q16X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->q16Y >> 8) & 0xFF;

    uint16_t hwAlpha0 = ((256 - hwAlphaX) * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = 256 - hwAlpha0 - hwAlpha1 - hwAlpha2;

    /* mask */
    uint32_t *pwAlphaSample = &pwOrigMask[tPoint.iY * iOrigMaskStride + tPoint.iX];

    uint8_t chPixelAlpha0 = *(uint8_t *)(pwAlphaSample++);
    uint8_t chPixelAlpha1 = *(uint8_t *)pwAlphaSample;
    pwAlphaSample += iOrigMaskStride;
    uint8_t chPixelAlpha3 = *(uint8_t *)(pwAlphaSample--);
    uint8_t chPixelAlpha2 = *(uint8_t *)pwAlphaSample;

    uint32_t wTotalAlpha = 0;
    uint16_t hwTempAlpha;

    hwTempAlpha = (chPixelAlpha0 == 255) * (hwAlpha0 << 8) 
                    + (!(chPixelAlpha0 == 255) * chPixelAlpha0 * hwAlpha0);
    wTotalAlpha += hwTempAlpha;
    
    hwTempAlpha = (chPixelAlpha1 == 255) * (hwAlpha1 << 8) 
                    + (!(chPixelAlpha1 == 255) * chPixelAlpha1 * hwAlpha1);
    wTotalAlpha += hwTempAlpha;

    hwTempAlpha = (chPixelAlpha2 == 255) * (hwAlpha2 << 8) 
                    + (!(chPixelAlpha2 == 255) * chPixelAlpha2 * hwAlpha2);
    wTotalAlpha += hwTempAlpha;

    hwTempAlpha = (chPixelAlpha3 == 255) * (hwAlpha3 << 8) 
                    + (!(chPixelAlpha3 == 255) * chPixelAlpha3 * hwAlpha3);
    wTotalAlpha += hwTempAlpha;

    if (0 == wTotalAlpha) {
        return ;
    }


    /* RGB channel */
    uint32_t *pwSample = &pwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint32_t uPoint0 = *pwSample++;
    uint32_t uPoint1 = *pwSample;
    pwSample += iOrigStride;
    uint32_t uPoint3 = *pwSample--;
    uint32_t uPoint2 = *pwSample;

    __API_CCCA8888_TRANS_PIXEL_AVERAGE_INIT();

    __API_CCCA8888_TRANS_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_CCCA8888_TRANS_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_CCCA8888_TRANS_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_CCCA8888_TRANS_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    __API_CCCA8888_TRANS_INT_TYPE uPixelResult = __API_CCCA8888_TRANS_PIXEL_AVERAGE_RESULT();

    #if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
        uint16_t hwPixelAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                              + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwPixelAlpha);
    #else
        uint16_t hwPixelAlpha = wTotalAlpha >> 8;

        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwPixelAlpha);
    #endif

#else

    arm_2d_location_t  tPoint = {
        .iX = reinterpret_s16_q16(ptFxPoint->q16X),
        .iY = reinterpret_s16_q16(ptFxPoint->q16Y),
    };

    uint32_t uPixelResult = pwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwPixelAlpha = ((uint8_t *)&uPixelResult)[3];

#if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
    hwPixelAlpha = hwPixelAlpha * hwOpacity >> 8;
    assert(hwOpacity != 255);
#endif

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
    hwPixelAlpha += (hwPixelAlpha == 255);
#endif

    __API_CCCA8888_TRANS_PIXEL_BLENDING( &uPixelResult, pTarget, 256 - hwPixelAlpha);

#endif
}

__WEAK
void __CCCA8888_TRANS_FUNC(transform)(
                                        __arm_2d_param_copy_orig_t *ptParam,
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        )
{
    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_CCCA8888_TRANS_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint32_t *pwOrigin = (uint32_t *)ptParam->tOrigin.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

#if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
    hwOpacity += (hwOpacity == 255);
#endif

    float fAngle = -ptInfo->fAngle;
    arm_2d_location_t tOffset = ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;
    q31_t             invIWidth = iWidth > 1 ? 0x7fffffff / (iWidth - 1) : 0x7fffffff;
    arm_2d_rot_linear_regr_t regrCoefs[2];
    arm_2d_location_t   SrcPt = ptInfo->tDummySourceOffset;

    /* get regression parameters over 1st and last column */
    __arm_2d_transform_regression(
        &ptParam->use_as____arm_2d_param_copy_t.tCopySize,
        &SrcPt,
        fAngle,
        ptInfo->fScaleX,
        ptInfo->fScaleY,
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

            tPointFast.q16X = __QDADD(colFirstX, slopeX * x);
            tPointFast.q16Y = __QDADD(colFirstY, slopeY * x);

#define __CALIBFX 590
        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_CALIB_IN_TRANSFORM__)
            if (tPointFast.q16X > 0) {
                tPointFast.q16X += __CALIBFX;
            } else {
                tPointFast.q16X -= __CALIBFX;
            }
            if (tPointFast.q16Y > 0) {
                tPointFast.q16Y += __CALIBFX;
            } else {
                tPointFast.q16Y -= __CALIBFX;
            }
        #endif


            arm_2d_location_t tOriginLocation = {
                .iX = reinterpret_s16_q16(tPointFast.q16X) - ptParam->tOrigin.tValidRegion.tLocation.iX,
                .iY = reinterpret_s16_q16(tPointFast.q16Y) - ptParam->tOrigin.tValidRegion.tLocation.iY,
            };

            if (    (tOriginLocation.iX < 0)
                ||  (tOriginLocation.iY < 0)
                ||  (tOriginLocation.iX >= (ptParam->tOrigin.tValidRegion.tSize.iWidth - 1))
                ||  (tOriginLocation.iY >= (ptParam->tOrigin.tValidRegion.tSize.iHeight - 1))) {
                pTargetBase++;
                continue;
            }

            __CCCA8888_TRANS_FUNC(get_pixel_colour)(
                                        &tPointFast,
                                        &ptParam->tOrigin.tValidRegion,
                                        pwOrigin,
                                        iOrigStride,
                                    #if __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
                                        hwOpacity,
                                    #endif
                                        pTargetBase
                                    );
            pTargetBase++;
        }
        //phwSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}

#undef get_pixel_colour
#undef transform


#undef __API_CCCA8888_TRANS_COPY_LIKE_OP_NAME
#undef __API_CCCA8888_TRANS_OP_NAME
#undef __API_CCCA8888_TRANS_PIXEL_BLENDING
#undef ____CCCA8888_TRANS_FUNC
#undef ___CCCA8888_TRANS_FUNC
#undef __CCCA8888_TRANS_FUNC
#undef __API_CCCA8888_TRANS_COLOUR
#undef __API_CCCA8888_TRANS_COLOUR_UPPER_CASE
#undef __API_CCCA8888_TRANS_INT_TYPE
#undef __API_CCCA8888_TRANS_INT_TYPE_BIT_NUM
#undef __API_CCCA8888_TRANS_PIXEL_AVERAGE
#undef __API_CCCA8888_TRANS_PIXEL_AVERAGE_RESULT
#undef __API_CCCA8888_TRANS_PIXEL_AVERAGE_INIT
#undef __API_CCCA8888_TRANS_CFG_SUPPORT_OPACITY
