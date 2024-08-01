
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
 * Title:        __arm_2d_fill_colour.inc
 * Description:  c code template for drawing pattern
 *
 * $Date:        02. May 2023
 * $Revision:    V.1.2.0
 *
 * -------------------------------------------------------------------- */


#ifndef __API_COLOUR
#   error You have to define __API_COLOUR before using this c template
#endif
#ifndef __API_INT_TYPE
#   error You have to define the __API_INT_TYPE before using this c template
#endif
#ifndef __API_INT_TYPE_BIT_NUM
#   error You have to define the __API_INT_TYPE_BIT_NUM before using this c template
#endif
#ifndef __API_PIXEL_BLENDING
#   error You have to define __API_PIXEL_BLENDING before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE
#   error You have to define __API_PIXEL_AVERAGE before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE_RESULT
#   error You have to define __API_PIXEL_AVERAGE_RESULT before using this c template
#endif
#ifndef __API_PIXEL_AVERAGE_INIT
#   define __API_PIXEL_AVERAGE_INIT()   __arm_2d_color_fast_rgb_t tPixel = {0};
#endif

#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#define ____ARM_2D_FUNC(__NAME, __COLOUR)  __arm_2d_impl_##__COLOUR##_##__NAME
#define ___ARM_2D_FUNC(__NAME, __COLOUR)   ____ARM_2D_FUNC(__NAME, __COLOUR)
#define __ARM_2D_FUNC(__NAME)   ___ARM_2D_FUNC(__NAME, __API_COLOUR)

#define MASK_COLOR(sz)  (sz == 8) ? ptInfo->Mask.chColour : ((sz == 16) ?  ptInfo->Mask.hwColour :  ptInfo->Mask.wColour)

static
arm_2d_point_float_t *__arm_2d_transform_point(
                                            const arm_2d_location_t *ptLocation,
                                            const arm_2d_location_t *ptCenter,
                                            float fAngle,
                                            float fScale,
                                            arm_2d_point_float_t *ptOutBuffer);

static
void __arm_2d_transform_regression( arm_2d_size_t * __RESTRICT ptCopySize,
                                    arm_2d_location_t * pSrcPoint,
                                    float fAngle,
                                    float fScale,
                                    arm_2d_location_t * tOffset,
                                    arm_2d_location_t * ptCenter,
                                    arm_2d_rot_linear_regr_t regrCoefs[]);


/*----------------------------------------------------------------------------*
 * Transform with colour keying                                               *
 *----------------------------------------------------------------------------*/

/*! \note Please only set this to zero for function correctness verification of
 *!       __arm_2d_meta_trans_with_masks.inc
 */
#if 1

extern
void __ARM_2D_FUNC(transform)(  __arm_2d_param_copy_orig_t *ptParam,
                                __arm_2d_transform_info_t *ptInfo);

extern
void __ARM_2D_FUNC(transform_with_opacity)(__arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio);



static
void __ARM_2D_FUNC(get_pixel_colour)(   arm_2d_point_fx_t  *ptFxPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        __API_INT_TYPE *pOrigin,
                                        int16_t iOrigStride,
                                        __API_INT_TYPE *pTarget,
                                        //int16_t iTargetSride,
                                        __API_INT_TYPE MaskColour)
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__


    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __API_INT_TYPE *pSample = &pOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    __API_INT_TYPE uPoint0 = *pSample++;
    __API_INT_TYPE uPoint1 = *pSample;
    pSample += iOrigStride;
    __API_INT_TYPE uPoint3 = *pSample--;
    __API_INT_TYPE uPoint2 = *pSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __API_PIXEL_AVERAGE_INIT();

    __API_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    __API_INT_TYPE uPixelResult = __API_PIXEL_AVERAGE_RESULT();
    if (uPixelResult != MaskColour) {
        *pTarget = uPixelResult;
    }
#else
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        if (Temp != MaskColour) {
            *pTarget = Temp;
        }
    //}
#endif
}

__WEAK
void __ARM_2D_FUNC(transform)(   __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
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

            __ARM_2D_FUNC(get_pixel_colour)(
                                        &tPointFast,
                                        &ptParam->tOrigin.tValidRegion,
                                        pOrigin,
                                        iOrigStride,
                                        pTargetBase,
                                        //iTargetStride,
                                        MaskColour
                                    );
            pTargetBase++;
        }
        //phwSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}




__WEAK
void __ARM_2D_FUNC(get_pixel_colour_with_alpha)(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour,
                                            uint_fast16_t hwOpacity)
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __API_INT_TYPE *pSample = &pOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    __API_INT_TYPE uPoint0 = *pSample++;
    __API_INT_TYPE uPoint1 = *pSample;
    pSample += iOrigStride;
    __API_INT_TYPE uPoint3 = *pSample--;
    __API_INT_TYPE uPoint2 = *pSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __API_PIXEL_AVERAGE_INIT();

    __API_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    __API_INT_TYPE uPixelResult = __API_PIXEL_AVERAGE_RESULT();
    if (uPixelResult != MaskColour) {
        __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwOpacity);
    }

#else

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
    __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                    +   tPoint.iX];
    if (Temp != MaskColour) {
        __API_PIXEL_BLENDING( &Temp, pTarget, hwOpacity);
    }
    //}

#endif
}



__WEAK
void __ARM_2D_FUNC(transform_with_opacity)(__arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);

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
                pTargetBase++;
                continue;
            }

            __ARM_2D_FUNC(get_pixel_colour_with_alpha) (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            pTargetBase,
                            MaskColour,
                            hwRatio);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}


#else


#   define __API_MTWM_COLOUR                       __API_COLOUR
#   define __API_MTWM_INT_TYPE                     __API_INT_TYPE
#   define __API_MTWM_INT_TYPE_BIT_NUM             __API_INT_TYPE_BIT_NUM
#   define __API_MTWM_PIXEL_BLENDING               __API_PIXEL_BLENDING
#   define __API_MTWM_PIXEL_AVERAGE                __API_PIXEL_AVERAGE
#   define __API_MTWM_PIXEL_AVERAGE_RESULT         __API_PIXEL_AVERAGE_RESULT
#   define __API_MTWM_PIXEL_AVERAGE_INIT           __API_PIXEL_AVERAGE_INIT

#   define get_pixel_colour_mask                   get_pixel_colour
#   define transform_with_mask                     transform

#   include "__arm_2d_ll_meta_trans_with_masks.inc"

#   define __API_MTWM_COLOUR                       __API_COLOUR
#   define __API_MTWM_INT_TYPE                     __API_INT_TYPE
#   define __API_MTWM_INT_TYPE_BIT_NUM             __API_INT_TYPE_BIT_NUM
#   define __API_MTWM_PIXEL_BLENDING               __API_PIXEL_BLENDING
#   define __API_MTWM_PIXEL_AVERAGE                __API_PIXEL_AVERAGE
#   define __API_MTWM_PIXEL_AVERAGE_RESULT         __API_PIXEL_AVERAGE_RESULT
#   define __API_MTWM_PIXEL_AVERAGE_INIT           __API_PIXEL_AVERAGE_INIT
#   define __API_MTWM_CFG_SUPPORT_OPACITY          1

#   define get_pixel_colour_mask                   get_pixel_colour_with_opacity
#   define transform_with_mask                     transform_with_opacity

#   include "__arm_2d_ll_meta_trans_with_masks.inc"
#endif


/*----------------------------------------------------------------------------*
 * Transform only                                                             *
 *----------------------------------------------------------------------------*/


extern
void __ARM_2D_FUNC(transform_only)( __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo);

extern
void __ARM_2D_FUNC(transform_only_opacity)(
                                    __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio);



__STATIC_INLINE
void __ARM_2D_FUNC(transform_only_get_pixel_colour)(   arm_2d_point_fx_t  *ptFxPoint,
                                        arm_2d_region_t *ptOrigValidRegion,
                                        __API_INT_TYPE *pOrigin,
                                        int16_t iOrigStride,
                                        __API_INT_TYPE *pTarget
                                        //,int16_t iTargetSride
                                        )
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __API_INT_TYPE *pSample = &pOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    __API_INT_TYPE uPoint0 = *pSample++;
    __API_INT_TYPE uPoint1 = *pSample;
    pSample += iOrigStride;
    __API_INT_TYPE uPoint3 = *pSample--;
    __API_INT_TYPE uPoint2 = *pSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __API_PIXEL_AVERAGE_INIT();

    __API_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    *pTarget = __API_PIXEL_AVERAGE_RESULT();

#else
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        //if (Temp != MaskColour) {
            *pTarget = Temp;
        //}
    //}
#endif
}

__WEAK
void __ARM_2D_FUNC(transform_only)( __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo)
{
    int_fast16_t iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t iTargetStride = ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t iOrigStride = ptParam->tOrigin.iStride;
    //__API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
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

            __ARM_2D_FUNC(transform_only_get_pixel_colour)(
                                        &tPointFast,
                                        &ptParam->tOrigin.tValidRegion,
                                        pOrigin,
                                        iOrigStride,
                                        pTargetBase
                                        //iTargetStride,
                                        //MaskColour
                                    );
            pTargetBase++;
        }
        //phwSourceBase += (iSourceStride - iWidth);
        pTargetBase += (iTargetStride - iWidth);
    }
}



__STATIC_INLINE
void __ARM_2D_FUNC(transform_only_get_pixel_colour_with_alpha)(
                                            arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            __API_INT_TYPE *pOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            //__API_INT_TYPE MaskColour,
                                            uint_fast16_t hwOpacity)
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    __API_INT_TYPE *pSample = &pOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    __API_INT_TYPE uPoint0 = *pSample++;
    __API_INT_TYPE uPoint1 = *pSample;
    pSample += iOrigStride;
    __API_INT_TYPE uPoint3 = *pSample--;
    __API_INT_TYPE uPoint2 = *pSample;

    
    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    __API_PIXEL_AVERAGE_INIT();

    __API_PIXEL_AVERAGE(uPoint0,  hwAlpha0);
    __API_PIXEL_AVERAGE(uPoint1,  hwAlpha1);
    __API_PIXEL_AVERAGE(uPoint2,  hwAlpha2);
    __API_PIXEL_AVERAGE(uPoint3,  hwAlpha3);

    __API_INT_TYPE uPixelResult = __API_PIXEL_AVERAGE_RESULT();

    __API_PIXEL_BLENDING( &uPixelResult, pTarget, hwOpacity);


#else

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        __API_INT_TYPE Temp = pOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];
        //if (Temp != MaskColour) {
            __API_PIXEL_BLENDING( &Temp, pTarget, hwOpacity);
        //}
    //}

#endif
}


__WEAK
void __ARM_2D_FUNC(transform_only_opacity)(
                                            __arm_2d_param_copy_orig_t *ptParam,
                                            __arm_2d_transform_info_t *ptInfo,
                                            uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    __API_INT_TYPE     *pOrigin = ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    //__API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);

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
                pTargetBase++;
                continue;
            }

            __ARM_2D_FUNC(transform_only_get_pixel_colour_with_alpha) (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pOrigin,
                            iOrigStride,
                            pTargetBase,
                            //MaskColour,
                            hwRatio);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}


/*----------------------------------------------------------------------------*
 * Transform with source mask                                                 *
 *----------------------------------------------------------------------------*/


#define __API_MTWM_COLOUR                       __API_COLOUR
#define __API_MTWM_INT_TYPE                     __API_INT_TYPE
#define __API_MTWM_INT_TYPE_BIT_NUM             __API_INT_TYPE_BIT_NUM
#define __API_MTWM_PIXEL_BLENDING               __API_PIXEL_BLENDING
#define __API_MTWM_PIXEL_AVERAGE                __API_PIXEL_AVERAGE
#define __API_MTWM_PIXEL_AVERAGE_RESULT         __API_PIXEL_AVERAGE_RESULT
#define __API_MTWM_PIXEL_AVERAGE_INIT           __API_PIXEL_AVERAGE_INIT
#define __API_MTWM_CFG_SUPPORT_SOURCE_MASK      1

#define get_pixel_colour_mask                   get_pixel_colour_src_mask
#define transform_with_mask                     transform_with_src_mask

#include "__arm_2d_ll_meta_trans_with_masks.inc"

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __API_MTWM_COLOUR                        __API_COLOUR
#   define __API_MTWM_INT_TYPE                      __API_INT_TYPE
#   define __API_MTWM_INT_TYPE_BIT_NUM              __API_INT_TYPE_BIT_NUM
#   define __API_MTWM_PIXEL_BLENDING                __API_PIXEL_BLENDING
#   define __API_MTWM_PIXEL_AVERAGE                 __API_PIXEL_AVERAGE
#   define __API_MTWM_PIXEL_AVERAGE_RESULT          __API_PIXEL_AVERAGE_RESULT
#   define __API_MTWM_PIXEL_AVERAGE_INIT            __API_PIXEL_AVERAGE_INIT
#   define __API_MTWM_CFG_SUPPORT_SOURCE_MASK       1
#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE  1
#   define get_pixel_colour_mask                    get_pixel_colour_src_chn_mask
#   define transform_with_mask                      transform_with_src_chn_mask

#   include "__arm_2d_ll_meta_trans_with_masks.inc"
#endif

#define __API_MTWM_COLOUR                       __API_COLOUR
#define __API_MTWM_INT_TYPE                     __API_INT_TYPE
#define __API_MTWM_INT_TYPE_BIT_NUM             __API_INT_TYPE_BIT_NUM
#define __API_MTWM_PIXEL_BLENDING               __API_PIXEL_BLENDING
#define __API_MTWM_PIXEL_AVERAGE                __API_PIXEL_AVERAGE
#define __API_MTWM_PIXEL_AVERAGE_RESULT         __API_PIXEL_AVERAGE_RESULT
#define __API_MTWM_PIXEL_AVERAGE_INIT           __API_PIXEL_AVERAGE_INIT
#define __API_MTWM_CFG_SUPPORT_SOURCE_MASK      1
#define __API_MTWM_CFG_SUPPORT_OPACITY          1

#define get_pixel_colour_mask                   get_pixel_colour_src_mask_opa
#define transform_with_mask                     transform_with_src_mask_and_opacity

#include "__arm_2d_ll_meta_trans_with_masks.inc"

#if __ARM_2D_CFG_SUPPORT_COLOUR_CHANNEL_ACCESS__
#   define __API_MTWM_COLOUR                        __API_COLOUR
#   define __API_MTWM_INT_TYPE                      __API_INT_TYPE
#   define __API_MTWM_INT_TYPE_BIT_NUM              __API_INT_TYPE_BIT_NUM
#   define __API_MTWM_PIXEL_BLENDING                __API_PIXEL_BLENDING
#   define __API_MTWM_PIXEL_AVERAGE                 __API_PIXEL_AVERAGE
#   define __API_MTWM_PIXEL_AVERAGE_RESULT          __API_PIXEL_AVERAGE_RESULT
#   define __API_MTWM_PIXEL_AVERAGE_INIT            __API_PIXEL_AVERAGE_INIT
#   define __API_MTWM_CFG_SUPPORT_SOURCE_MASK       1
#   define __API_MTWM_CFG_SUPPORT_OPACITY           1
#   define __API_MTWM_CFG_CHANNEL_8in32_SUPPORT_ON_SOURCE_SIDE  1
#   define get_pixel_colour_mask                    get_pixel_colour_src_chn_mask_opa
#   define transform_with_mask                      transform_with_src_chn_mask_and_opacity

#   include "__arm_2d_ll_meta_trans_with_masks.inc"
#endif


/*----------------------------------------------------------------------------*
 * Fill Colour with Mask Opacity and Transform                                *
 *----------------------------------------------------------------------------*/

__STATIC_INLINE
void __ARM_2D_FUNC(get_alpha_with_opacity)( arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint8_t *pchOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour,
                                            uint_fast16_t hwOpacity)
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

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

    uint32_t wTotalAlpha = 0;
    uint16_t hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint0,  hwAlpha0);
    hwPixelAlpha = (chPoint0 == 255) * (hwAlpha0 << 8) 
                    + (!(chPoint0 == 255) * chPoint0 * hwAlpha0);
    wTotalAlpha += hwPixelAlpha;
    
    //__API_PIXEL_AVERAGE_GRAY8(hwPoint1,  hwAlpha1);
    hwPixelAlpha = (chPoint1 == 255) * (hwAlpha1 << 8) 
                    + (!(chPoint1 == 255) * chPoint1 * hwAlpha1);
    wTotalAlpha += hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint2,  hwAlpha2);
    hwPixelAlpha = (chPoint2 == 255) * (hwAlpha2 << 8) 
                    + (!(chPoint2 == 255) * chPoint2 * hwAlpha2);
    wTotalAlpha += hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint3,  hwAlpha3);
    hwPixelAlpha = (chPoint3 == 255) * (hwAlpha3 << 8) 
                    + (!(chPoint3 == 255) * chPoint3 * hwAlpha3);
    wTotalAlpha += hwPixelAlpha;

    if (wTotalAlpha) {
        //uint8_t hwAlpha = __API_PIXEL_AVERAGE_RESULT_GRAY8();
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                            + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));
            

        __API_PIXEL_BLENDING( &MaskColour, pTarget, hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    uint16_t hwPixelAlpha = 0;
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        hwPixelAlpha = pchOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];

        hwPixelAlpha =  (hwPixelAlpha == 255) * hwOpacity
                     + !(hwPixelAlpha == 255) * (hwPixelAlpha * hwOpacity >> 8);

        __API_PIXEL_BLENDING( &MaskColour, pTarget, hwPixelAlpha);
    //}

#endif
}


__WEAK
void __ARM_2D_FUNC(colour_filling_mask_opacity_transform)(
                                    __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint8_t             *pchOrigin = (uint8_t *)ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);

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
                pTargetBase++;
                continue;
            }

            __ARM_2D_FUNC(get_alpha_with_opacity) (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pchOrigin,
                            iOrigStride,
                            pTargetBase,
                            MaskColour,
                            hwRatio);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}



__STATIC_INLINE
void __ARM_2D_FUNC(get_chn_alpha_with_opacity)( arm_2d_point_fx_t  *ptFxPoint,
                                            arm_2d_region_t *ptOrigValidRegion,
                                            uint32_t *pwOrigin,
                                            int16_t iOrigStride,
                                            __API_INT_TYPE *pTarget,
                                            //int16_t iTargetSride,
                                            __API_INT_TYPE MaskColour,
                                            uint_fast16_t hwOpacity)
{
#if     defined(__ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__)                          \
    &&  __ARM_2D_HAS_ANTI_ALIAS_TRANSFORM__ == 1

    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    uint32_t *pwSample = &pwOrigin[tPoint.iY * iOrigStride + tPoint.iX];

    uint16_t hwAlphaX = (ptFxPoint->X >> 8) & 0xFF;
    uint16_t hwAlphaY = (ptFxPoint->Y >> 8) & 0xFF;

    uint8_t chPoint0 = *(uint8_t *)(pwSample++);
    uint8_t chPoint1 = *(uint8_t *)pwSample;
    pwSample += iOrigStride;
    uint8_t chPoint3 = *(uint8_t *)(pwSample--);
    uint8_t chPoint2 = *(uint8_t *)pwSample;

    uint16_t hwAlpha1 = (hwAlphaX * (256 - hwAlphaY)) >> 8;
    uint16_t hwAlpha2 = ((256 - hwAlphaX) * hwAlphaY) >> 8;
    uint16_t hwAlpha3 = hwAlphaX * hwAlphaY >> 8;
    uint16_t hwAlpha0 = 256 - hwAlpha1 - hwAlpha2 - hwAlpha3;

    uint32_t wTotalAlpha = 0;
    uint16_t hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint0,  hwAlpha0);
    hwPixelAlpha = (chPoint0 == 255) * (hwAlpha0 << 8) 
                    + (!(chPoint0 == 255) * chPoint0 * hwAlpha0);
    wTotalAlpha += hwPixelAlpha;
    
    //__API_PIXEL_AVERAGE_GRAY8(hwPoint1,  hwAlpha1);
    hwPixelAlpha = (chPoint1 == 255) * (hwAlpha1 << 8) 
                    + (!(chPoint1 == 255) * chPoint1 * hwAlpha1);
    wTotalAlpha += hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint2,  hwAlpha2);
    hwPixelAlpha = (chPoint2 == 255) * (hwAlpha2 << 8) 
                    + (!(chPoint2 == 255) * chPoint2 * hwAlpha2);
    wTotalAlpha += hwPixelAlpha;

    //__API_PIXEL_AVERAGE_GRAY8(hwPoint3,  hwAlpha3);
    hwPixelAlpha = (chPoint3 == 255) * (hwAlpha3 << 8) 
                    + (!(chPoint3 == 255) * chPoint3 * hwAlpha3);
    wTotalAlpha += hwPixelAlpha;

    if (wTotalAlpha) {
        //uint8_t hwAlpha = __API_PIXEL_AVERAGE_RESULT_GRAY8();
        uint16_t hwAlpha = (wTotalAlpha >= 0xFF00) * hwOpacity
                            + (!(wTotalAlpha >= 0xFF00) * (wTotalAlpha * hwOpacity >> 16));
            

        __API_PIXEL_BLENDING( &MaskColour, pTarget, hwAlpha);
    }

#else
    
    arm_2d_location_t  tPoint = {
        .iX = ptFxPoint->X >> 16,
        .iY = ptFxPoint->Y >> 16,
    };

    uint16_t hwPixelAlpha = 0;
    //if (arm_2d_is_point_inside_region(ptOrigValidRegion, &tPoint)) {
        hwPixelAlpha = pchOrigin[   tPoint.iY * iOrigStride
                                     +   tPoint.iX];

        hwPixelAlpha =  (hwPixelAlpha == 255) * hwOpacity
                     + !(hwPixelAlpha == 255) * (hwPixelAlpha * hwOpacity >> 8);

        __API_PIXEL_BLENDING( &MaskColour, pTarget, hwPixelAlpha);
    //}

#endif
}


__WEAK
void __ARM_2D_FUNC(colour_filling_chn_mask_opacity_transform)(
                                    __arm_2d_param_copy_orig_t *ptParam,
                                    __arm_2d_transform_info_t *ptInfo,
                                    uint_fast16_t hwRatio)
{
    int_fast16_t        iHeight = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iHeight;
    int_fast16_t        iWidth = ptParam->use_as____arm_2d_param_copy_t.tCopySize.iWidth;

    int_fast16_t        iTargetStride =
        ptParam->use_as____arm_2d_param_copy_t.tTarget.iStride;
    __API_INT_TYPE     *pTargetBase = ptParam->use_as____arm_2d_param_copy_t.tTarget.pBuffer;
    uint32_t           *pwOrigin = (uint32_t *)ptParam->tOrigin.pBuffer;
    int_fast16_t        iOrigStride = ptParam->tOrigin.iStride;
    __API_INT_TYPE      MaskColour = MASK_COLOR(__API_INT_TYPE_BIT_NUM);
    float               fAngle = -ptInfo->fAngle;
    arm_2d_location_t   tOffset =
        ptParam->use_as____arm_2d_param_copy_t.tSource.tValidRegion.tLocation;

    hwRatio += (hwRatio == 255);

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
                pTargetBase++;
                continue;
            }

            __ARM_2D_FUNC(get_chn_alpha_with_opacity) (
                            &tPointFast,
                            &ptParam->tOrigin.
                            tValidRegion,
                            pwOrigin,
                            iOrigStride,
                            pTargetBase,
                            MaskColour,
                            hwRatio);
            pTargetBase++;
        }
        pTargetBase += (iTargetStride - iWidth);
    }
}


#undef ____ARM_2D_FUNC
#undef ___ARM_2D_FUNC
#undef __ARM_2D_FUNC
#undef __API_COLOUR
#undef __API_INT_TYPE
#undef __API_INT_TYPE_BIT_NUM
#undef __API_PIXEL_BLENDING
#undef __API_PIXEL_AVERAGE
#undef __API_PIXEL_AVERAGE_RESULT
#undef __API_PIXEL_AVERAGE_INIT
