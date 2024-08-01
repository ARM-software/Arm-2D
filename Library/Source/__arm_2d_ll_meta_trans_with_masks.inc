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
 * Title:        __arm_2d_meta_trans_with_masks.inc
 * Description:  c code template for copy and fill like operations
 *
 * $Date:        24. May 2022
 * $Revision:    V.1.1.1
 *
 * -------------------------------------------------------------------- */

#ifndef __API_MTWM_COLOUR
#   error You have to define __API_MTWM_COLOUR before using this c template
#endif
#ifndef __API_MTWM_INT_TYPE
#   error You have to define the __API_MTWM_INT_TYPE before using this c template
#endif
#ifndef __API_MTWM_INT_TYPE_BIT_NUM
#   error You have to define the __API_MTWM_INT_TYPE_BIT_NUM before using this c template
#endif
#ifndef __API_MTWM_PIXEL_BLENDING
#   error You have to define __API_MTWM_PIXEL_BLENDING before using this c template
#endif
#ifndef __API_MTWM_PIXEL_AVERAGE
#   error You have to define __API_MTWM_PIXEL_AVERAGE before using this c template
#endif
#ifndef __API_MTWM_PIXEL_AVERAGE_RESULT
#   error You have to define __API_MTWM_PIXEL_AVERAGE_RESULT before using this c template
#endif
#ifndef __API_MTWM_PIXEL_AVERAGE_INIT
#   define __API_MTWM_PIXEL_AVERAGE_INIT()   __arm_2d_color_fast_rgb_t tPixel = {0};
#endif


/*! disable this feature by default */
#ifndef __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING
#   define __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING               0
#endif

//#ifndef __API_MTWM_CFG_1_HORIZONTAL_LINE
//#   define __API_MTWM_CFG_1_HORIZONTAL_LINE                     0
//#endif

//#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT
//#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT                 0
//#endif

#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE  0
#endif

//#ifndef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
//#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE  0
//#endif

#ifndef __API_MTWM_CFG_SUPPORT_SOURCE_MASK
#   define __API_MTWM_CFG_SUPPORT_SOURCE_MASK                   0
#endif

//#ifndef __API_MTWM_CFG_SUPPORT_TARGET_MASK
//#   define __API_MTWM_CFG_SUPPORT_TARGET_MASK                   0
//#endif

#ifndef __API_MTWM_CFG_SUPPORT_OPACITY
#   define __API_MTWM_CFG_SUPPORT_OPACITY                       0
#endif

#undef ____MTWM_FUNC
#undef ___MTWM_FUNC
#undef __MTWM_FUNC



#ifndef __API_MTWM_OP_NAME
#   define ____MTWM_FUNC(__NAME, __COLOUR)                                      \
        __arm_2d_impl_##__COLOUR##_##__NAME
#   define ___MTWM_FUNC(__NAME, __COLOUR)   ____MTWM_FUNC(__NAME, __COLOUR)
#else
#   define _____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)                          \
        __arm_2d_impl_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)                           \
        _____MTWM_FUNC(__OP_NAME, __NAME, __COLOUR)
#   define ___MTWM_FUNC(__NAME, __COLOUR)                                       \
        ____MTWM_FUNC(__API_MTWM_OP_NAME, __NAME, __COLOUR)
#endif

#define __MTWM_FUNC(__NAME)   ___MTWM_FUNC(__NAME, __API_MTWM_COLOUR)


#undef ____MTWM_TYPE
#undef ___MTWM_TYPE
#undef __MTWM_TYPE

#ifndef __API_MTWM_OP_NAME
#   define ____MTWM_TYPE(__NAME, __COLOUR)  arm_2d_##__COLOUR##_##__NAME
#   define ___MTWM_TYPE(__NAME, __COLOUR)   ____MTWM_TYPE(__NAME, __COLOUR)
#else
#   define _____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)                          \
        arm_2d_##__COLOUR##_##__OP_NAME##_##__NAME
#   define ____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)                           \
        _____MTWM_TYPE(__OP_NAME, __NAME, __COLOUR)
#   define ___MTWM_TYPE(__NAME, __COLOUR)                                       \
        ____MTWM_TYPE(__API_MTWM_OP_NAME, __NAME, __COLOUR)
#endif


#define __MTWM_TYPE(__NAME)   ___MTWM_TYPE(__NAME, __API_MTWM_COLOUR)

#define MASK_COLOR(sz)  (sz == 8) ? ptInfo->Mask.chColour : ((sz == 16) ?  ptInfo->Mask.hwColour :  ptInfo->Mask.wColour)

/*============================ PROTOTYPES ====================================*/
extern
void __MTWM_FUNC(transform_with_mask)(
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK      \
                                     || __API_MTWM_CFG_SUPPORT_TARGET_MASK
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                    #else
                                        __arm_2d_param_copy_orig_t *ptParam,
                                    #endif
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        );

/*============================ IMPLEMENTATION ================================*/



__STATIC_INLINE
void __MTWM_FUNC(get_pixel_colour_mask)(arm_2d_point_fx_t  *ptFxPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        __API_INT_TYPE *pOrigin,
                                        int16_t iOrigStride,
                            #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                            #   if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                                        uint32_t *pwOrigMask,
                            #   else
                                        uint8_t *pchOrigMask,
                            #   endif
                                        int16_t iOrigMaskStride,
                            #else
                                        __API_INT_TYPE MaskColour,
                            #endif
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                        uint16_t hwOpacity,
                                    #endif
                                        __API_INT_TYPE *pTarget
                                    )
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                            \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    /*
     * [0][1]
     * [2][3]
     */

    /* anti-alias alpha table */
    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    uint16_t hwAlpha0 = ((256 - hwAlphaX) * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = 256 - hwAlpha0 - hwAlpha1 - hwAlpha2;

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
    /* mask */
#   if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *pwAlphaSample = &pwOrigMask[tPoint.iY * iOrigMaskStride + tPoint.iX];

    uint8_t chPixelAlpha0 = *(uint8_t *)(pwAlphaSample++);
    uint8_t chPixelAlpha1 = *(uint8_t *)pwAlphaSample;
    pwAlphaSample += iOrigMaskStride;
    uint8_t chPixelAlpha3 = *(uint8_t *)(pwAlphaSample--);
    uint8_t chPixelAlpha2 = *(uint8_t *)pwAlphaSample;
#else
    uint8_t *pchAlphaSample = &pchOrigMask[tPoint.iY * iOrigMaskStride + tPoint.iX];

    uint8_t chPixelAlpha0 = *pchAlphaSample++;
    uint8_t chPixelAlpha1 = *pchAlphaSample;
    pchAlphaSample += iOrigMaskStride;
    uint8_t chPixelAlpha3 = *pchAlphaSample--;
    uint8_t chPixelAlpha2 = *pchAlphaSample;
#endif

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
#endif

    /* RGB channel */
    __API_INT_TYPE *pSample = &pOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    __API_INT_TYPE uPoint0 = *pSample++;
    __API_INT_TYPE uPoint1 = *pSample;
    pSample += iOrigStride;
    __API_INT_TYPE uPoint3 = *pSample--;
    __API_INT_TYPE uPoint2 = *pSample;

    __API_PIXEL_AVERAGE_INIT();

    __API_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    __API_INT_TYPE uPixelResult = __API_PIXEL_AVERAGE_RESULT();

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
    #if __API_MTWM_CFG_SUPPORT_OPACITY
        uint16_t hwPixelAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                              + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));

        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwPixelAlpha);
    #else
        uint16_t hwPixelAlpha = wTotalAlpha >> 8;

        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwPixelAlpha);
    #endif
#else
    if (uPixelResult != MaskColour) {

    #if __API_MTWM_CFG_SUPPORT_OPACITY
        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwOpacity);
    #else
        *pTarget = uPixelResult;
    #endif

    }
#endif



#else

    arm_2d_location_t tTemp;

    tTemp.iX = ptFxPoint->X >> 16;
    tTemp.iY = ptFxPoint->Y >> 16;

    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tTemp)) {
        __API_INT_TYPE Temp = pOrigin[   tTemp.iY * iOrigStride
                                     +   tTemp.iX];

    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
        #if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
            uint16_t hwPixelAlpha = pchOrigMask[tTemp.iY * iOrigMaskStride
                                            +   tTemp.iX * 4];
        #else
            uint16_t hwPixelAlpha = pchOrigMask[tTemp.iY * iOrigMaskStride
                                            +   tTemp.iX];
        #endif

        #if __API_MTWM_CFG_SUPPORT_OPACITY
        hwPixelAlpha = hwPixelAlpha * hwOpacity >> 8;
        assert(hwOpacity != 255);
        #endif

        #if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
        hwPixelAlpha += (hwPixelAlpha == 255);
        #endif

        __API_MTWM_PIXEL_BLENDING( &Temp, pTarget, hwPixelAlpha);
    #else

        if (Temp != MaskColour) {
        #if __API_MTWM_CFG_SUPPORT_OPACITY
            __API_MTWM_PIXEL_BLENDING( &Temp, pTarget, hwOpacity);
        #else
            *pTarget = Temp;
        #endif
        }
    #endif
    //}
#endif
}

__WEAK
void __MTWM_FUNC(transform_with_mask)(
                                    #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK      \
                                     || __API_MTWM_CFG_SUPPORT_TARGET_MASK
                                        __arm_2d_param_copy_orig_msk_t *ptThis,
                                    #else
                                        __arm_2d_param_copy_orig_t *ptParam,
                                    #endif
                                        __arm_2d_transform_info_t *ptInfo
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
                                       ,uint_fast16_t hwOpacity
                                    #endif
                                        )
{
#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK                                          \
 || __API_MTWM_CFG_SUPPORT_TARGET_MASK
    __arm_2d_param_copy_orig_t *ptParam =
        &(ptThis->use_as____arm_2d_param_copy_orig_t);
#endif

    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;

#if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
#   if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
    uint32_t *pwOriginMask = (uint32_t *)this.tOrigMask.pBuffer;
#else
    uint8_t *pchOriginMask = (uint8_t *)this.tOrigMask.pBuffer;
#endif
    int_fast16_t iOrigMaskStride = this.tOrigMask.iStride;
#else
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_MTWM_INT_TYPE_BIT_NUM);
#endif

#if __API_MTWM_CFG_SUPPORT_OPACITY
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
                pTargetBase++;
                continue;
            }

            __ARM_2D_FUNC(get_pixel_colour_mask)(
                                        &tPointFast,
                                        &ptParam->tOrigin.tValidRegion,
                                        pOrigin,
                                        iOrigStride,
                                #if __API_MTWM_CFG_SUPPORT_SOURCE_MASK
                                #   if __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
                                        pwOriginMask,
                                #   else 
                                        pchOriginMask,
                                #   endif
                                        iOrigMaskStride,
                                #else
                                        MaskColour,
                                #endif
                                    #if __API_MTWM_CFG_SUPPORT_OPACITY
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


#undef get_pixel_colour_mask
#undef transform_with_mask


#undef __API_MTWM_COPY_LIKE_OP_NAME
#undef __API_MTWM_OP_NAME
#undef __API_MTWM_PIXEL_BLENDING
#undef ____MTWM_FUNC
#undef ___MTWM_FUNC
#undef __MTWM_FUNC
#undef __API_MTWM_COLOUR
#undef __API_MTWM_INT_TYPE
#undef __API_MTWM_INT_TYPE_BIT_NUM
#undef ____MTWM_TYPE
#undef ___MTWM_TYPE
#undef __MTWM_TYPE
#undef __API_MTWM_CFG_SUPPORT_SRC_MSK_WRAPING
#undef __API_MTWM_CFG_1_HORIZONTAL_LINE
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE
#undef __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_TARGET_SIDE
#undef __API_MTWM_PIXEL_AVERAGE
#undef __API_MTWM_PIXEL_AVERAGE_RESULT
#undef __API_MTWM_PIXEL_AVERAGE_INIT
#undef __API_MTWM_CFG_SUPPORT_SOURCE_MASK
#undef __API_MTWM_CFG_SUPPORT_TARGET_MASK
#undef __API_MTWM_CFG_SUPPORT_OPACITY
